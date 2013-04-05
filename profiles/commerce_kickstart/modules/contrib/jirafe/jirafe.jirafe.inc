<?php
require_once 'Jirafe/Autoloader.php';
Jirafe_Autoloader::register();

/**
 * Create and synchronize the site configuration.
 */
function jirafe_synchronize() {
  $config = variable_get('jirafe_config', array(
    'token' => NULL,
    'app_id' => NULL,
  ));

  if (empty($config['token']) || empty($config['app_id'])) {
    throw new Exception(t('Jirafe token or application ID missing.'));
  }

  $jirafe_resources = array(
    'sites' => array(),
    'users' => array(),
  );

  // By default, create a single site and a single user.
  $jirafe_resources_sites = array(
    'url' => $GLOBALS['base_url'],
    'description' => t('Main site'),
    'timezone' => date_default_timezone_get(),
    'currency' => commerce_default_currency(),
  );

  $jirafe_resources_users = array(
    'email' => variable_get('site_mail', ini_get('sendmail_from')),
    'first_name' => 'admin',
  );

  if (!empty($config['sites'][0])) {
    $existing_sites = $config['sites'][0];
    $jirafe_resources_sites = array_merge($jirafe_resources_sites, $existing_sites);
  }

  if (!empty($config['users'][0])) {
    $existing_users = $config['users'][0];
    $jirafe_resources_users = array_merge($jirafe_resources_users, $existing_users);
  }

  $jirafe_resources['sites'][] = $jirafe_resources_sites;
  $jirafe_resources['users'][] = $jirafe_resources_users;

  // Allow other modules to alter the resources.
  drupal_alter('jirafe_resources', $jirafe_resources, $config);

  // Synchronize with Jirafe.
  $client = new Jirafe_Client($config['token']);
  try {
    $additional_parameters =     array(
       'platform_type' => 'drupal',
       'platform_version' => '1.0.0',
       'plugin_version' => '0.1.0',
      );
    $result = $client->applications($config['app_id'])->resources()->sync($jirafe_resources['sites'], $jirafe_resources['users'], $additional_parameters);
  }
  catch(Jirafe_Exception $exception) {
    watchdog('jirafe', $exception->getMessage());
  }

  $config['sites'] = $result['sites'];
  $config['users'] = $result['users'];
  variable_set('jirafe_config', $config);
}

/**
 * Get the site that will receive Jirafe calls for the current page request.
 */
function jirafe_get_current_site() {
  $config = variable_get('jirafe_config', array());
  if (empty($config['token']) || empty($config['sites'])) {
    return NULL;
  }

  // Default logic: pick the site that matches with our current base URL.
  $current_site = NULL;
  foreach ($config['sites'] as $site) {
    if ($site['url'] == $GLOBALS['base_url']) {
      $current_site = $site;
    }
  }

  // Allow modules to override the default logic.
  drupal_alter('jirafe_current_site', $current_site, $config);
  return $current_site;
}

/**
 * Get a Piwik tracker object for the current request.
 */
function jirafe_get_tracker() {
  $config = variable_get('jirafe_config');
  if (empty($config['token'])) {
    return;
  }
  $current_site = jirafe_get_current_site();
  if (!$current_site) {
    return;
  }

  $tracker = new Jirafe_PiwikTracker($current_site['site_id'], 'https://data.jirafe.com/piwik.php');
  $tracker->setTokenAuth($config['token']);
  try {
    $tracker->setVisitorId($tracker->getVisitorId());
  }
  catch(Exception $exception) {
    watchdog('jirafe', $exception->getMessage());
  }
  $tracker->disableCookieSupport();
  return $tracker;
}

/**
 * Build the order structure for Jirafe from a Drupal Commerce order.
 */
function jirafe_commerce_order_data($order) {
  $data = array(
    'order_id' => $order->order_id,
    'products' => array(),
    'total' => array(
      'base' => 0,
      'tax' => 0,
      'shipping' => 0,
      'discount' => 0,
    ),
  );
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Derive the currency of the order from the order total field.
  $currency_code = $order_wrapper->commerce_order_total->currency_code->value();

  $jirafe_component_types = array(
    'base' => array(
      'base_price',
    ),
    'tax' => array(),
    'shipping' => array(),
    'discount' => array(),
  );
  // Automatically detect tax component types.
  foreach (commerce_price_component_types() as $component_name => $component_info) {
    if (!empty($component_info['tax_rate'])) {
      $jirafe_component_types['tax'][] = $component_name;
    }
  }
  // TODO: implement mapping for shipping and discounts.

  // Let the rest of the modules alter the mapping.
  drupal_alter('jirafe_component_types', $jirafe_component_types);

  // Decompose the order total based on the mapping.
  $order_total = $order_wrapper->commerce_order_total->value();
  foreach ($order_total['data']['components'] as $component_data) {
    // By default put everything in the base price.
    $jirafe_type = 'base';

    // Find if we have a more specific class.
    foreach ($jirafe_component_types as $type => $price_component_types) {
      if (in_array($component_data['name'], $price_component_types)) {
        $jirafe_type = $type;
        break;
      }
    }

    $data['total'][$jirafe_type] += commerce_currency_convert(
      $component_data['price']['amount'],
      $component_data['price']['currency_code'],
      $currency_code
    );
  }

  foreach ($order_wrapper->commerce_line_items as $line_item_id => $line_item_wrapper) {
    $line_item = $line_item_wrapper->value();
    if (in_array($line_item->type, commerce_product_line_item_types())) {
      $product = $line_item_wrapper->commerce_product->value();
      $line_item_price = $line_item_wrapper->commerce_total->value();

      // Jirafe doesn't support per line item currency.
      // Convert the price in the currency of the order.
      $amount = commerce_currency_convert(
        $line_item_price['amount'],
        $line_item_price['currency_code'],
        $currency_code
      );

      // Set sane defaults.
      $jirafe_product = array(
        'sku' => $product->sku,
        'name' => $product->title,
        'category' => NULL,
        'price' => $line_item_price['amount'],
        'quantity' => (int) $line_item->quantity,
      );

      // Implementations of hook_jirafe_commerce_order_line_item_alter()
      // can override any of those values.
      drupal_alter('jirafe_commerce_order_line_item', $jirafe_product, $line_item);

      // We even allow not to return the product. We are that nice.
      if ($jirafe_product) {
        $data['products'][] = $jirafe_product;
      }
    }
  }
  drupal_alter('jirafe_commerce_order_data', $data, $order);

  // Now convert all the prices into floats.
  foreach ($data['total'] as $component => &$amount) {
    $amount = commerce_currency_amount_to_decimal($amount, $currency_code);
  }
  foreach ($data['products'] as &$product) {
    $product['price'] = commerce_currency_amount_to_decimal($product['price'], $currency_code);
  }

  return $data;
}
