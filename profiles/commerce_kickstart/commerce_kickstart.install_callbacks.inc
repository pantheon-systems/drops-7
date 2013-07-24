<?php

/**
 * @file
 * Contains Batch API callbacks used during installation.
 */

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_install_additional_modules()
 */
function _commerce_kickstart_enable_theme($theme, &$context) {
  theme_enable(array($theme));
  variable_set('theme_default', $theme);

  $context['message'] = st('Installed the default theme.');
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_install_additional_modules()
 */
function _commerce_kickstart_enable_module($module, $module_name, &$context) {
  module_enable(array($module), FALSE);
  $context['message'] = st('Installed %module module.', array('%module' => $module_name));
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_install_additional_modules()
 */
function _commerce_kickstart_setup_localization($operation, &$context) {
  require_once DRUPAL_ROOT . '/includes/language.inc';
  $context['message'] = t('@operation', array('@operation' => $operation));

  // Enable en prefix for english language.
  db_update('languages')
    ->fields(array(
      'prefix' => 'en',
    ))
    ->condition('language',  'en')
    ->execute();

  // Enable language detection via url.
  $negotiation['locale-url'] = array(
    'types' => array(
      'language_content',
      'language',
      'language_url',
    ),
    'callbacks' => array(
      'language' => 'locale_language_from_url',
      'switcher' => 'locale_language_switcher_url',
      'url_rewrite' => 'locale_language_url_rewrite_url',
    ),
    'file' => 'includes/locale.inc',
    'weight' => '-8,',
    'name' => 'URL',
    'description' => t('Determine the language from the URL (Path prefix or domain).'),
    'config' => 'admin/config/regional/language/configure/url',
  );
  language_negotiation_set('language', $negotiation);
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_install_additional_modules()
 */
function _commerce_kickstart_flush_caches($operation, &$context) {
  $context['message'] = t('@operation', array('@operation' => $operation));
  drupal_flush_all_caches();
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_import_content()
 */
function _commerce_kickstart_example_taxes($operation, &$context) {
  $context['message'] = t('@operation', array('@operation' => $operation));

  $commerce_kickstart_choose_tax_country = variable_get('commerce_kickstart_choose_tax_country', NULL);

  // Create the choosen tax.
  if (!empty($commerce_kickstart_choose_tax_country)) {
    if ($commerce_kickstart_choose_tax_country == 'us') {
      $tax = array(
        'name' => 'sample_michigan_sales_tax',
        'title' => 'Sample Michigan Sales Tax 6%',
        'display_title' => 'Sample Michigan Sales Tax 6%',
        'description' => '',
        'rate' => 0.06,
        'type' => 'sales_tax', // vat
        'rules_component' => '',
        'default_rules_component' => FALSE,
        'tax_component' => '',
        'admin_list' => TRUE,
        'calculation_callback' => 'commerce_tax_rate_calculate',
        'module' => 'commerce_tax_ui',
        'is_new' => TRUE,
      );
      commerce_tax_ui_tax_rate_save($tax);

      $rule = '{ "commerce_tax_rate_sample_michigan_sales_tax" : {
        "LABEL" : "Calculate Sample Michigan Sales Tax 6%",
        "PLUGIN" : "rule",
        "REQUIRES" : [ "commerce_order", "commerce_tax" ],
        "USES VARIABLES" : {
          "commerce_line_item" : {
            "label" : "Line item",
            "type" : "commerce_line_item"
          }
        },
        "IF" : [{
          "commerce_order_compare_address" : {
            "commerce_order" : [ "commerce-line-item:order" ],
            "address_field" : "commerce_customer_shipping|commerce_customer_address",
            "address_component" : "administrative_area",
            "value" : "MI"
          }
        }],
        "DO" : [{
          "commerce_tax_rate_apply" : {
            "USING" : {
              "commerce_line_item" : [ "commerce-line-item" ],
              "tax_rate_name" : "sample_michigan_sales_tax"},
              "PROVIDE" : {
                "applied_tax" : {
                  "applied_tax" : "Applied tax"
                }
              }
            }
          }
        ]}
      }';
      $rules_config = rules_import($rule);
      $rules_config->save();

      $tax = array(
        'name' => 'sample_michigan_sales_tax',
        'rules_component' => 'commerce_tax_rate_sample_michigan_sales_tax',
        'is_new' => FALSE,
      );
      commerce_tax_ui_tax_rate_save($tax);
    }

    if ($commerce_kickstart_choose_tax_country == 'europe') {
      variable_set('import_choosen_tax', 'europe');
      $tax = array(
        'name' => 'sample_french_vat_tax',
        'title' => 'Sample French VAT 19,6%',
        'display_title' => 'Sample French VAT 19,6%',
        'description' => '',
        'rate' => 0.196,
        'type' => 'vat', // vat
        'default_rules_component' => TRUE,
        'tax_component' => '',
        'admin_list' => TRUE,
        'calculation_callback' => 'commerce_tax_rate_calculate',
        'module' => 'commerce_tax_ui',
        'is_new' => TRUE,
      );
      commerce_tax_ui_tax_rate_save($tax);
    }
  }
  // delete the variables we used during this task.
  variable_del('commerce_kickstart_choose_tax_country');
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_import_content()
 */
function _commerce_kickstart_taxonomy_menu($operation, &$context) {
  $context['message'] = t('@operation', array('@operation' => $operation));


  // Set up taxonomy main menu.
  $vocabulary_machine_name = variable_get('commerce_kickstart_demo_store', FALSE) ? 'collection' : 'product_category';
  if ($collection = taxonomy_vocabulary_machine_name_load($vocabulary_machine_name)) {
    $variable_name = _taxonomy_menu_build_variable('vocab_menu', $collection->vid);
    variable_set($variable_name, 'main-menu');
    $variable_name = _taxonomy_menu_build_variable('vocab_parent', $collection->vid);
    variable_set($variable_name, '0');
    $variable_name = _taxonomy_menu_build_variable('path', $collection->vid);
    variable_set($variable_name, 'commerce_kickstart_taxonomy_term_path');
    $variable_name = _taxonomy_menu_build_variable('rebuild', $collection->vid);
    variable_set($variable_name, 1);
    $variable_name = _taxonomy_menu_build_variable('sync', $collection->vid);
    variable_set($variable_name, 1);
  }
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_import_content()
 */
function _commerce_kickstart_import($operation, $type, &$context) {
  $context['message'] = t('@operation', array('@operation' => $type));
  $migration =  Migration::getInstance($operation);
  $migration->processImport();
}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_import_content()
 */
function _commerce_kickstart_example_user($operation, &$context) {
  $context['message'] = t('@operation', array('@operation' => $operation));

  // Create a sample user.
  $user = array(
    'name' => 'Sample Customer',
    'mail' => 'customer@example.com',
    'pass' => 'customer',
    'status' => 1,
  );
  $user = user_save(NULL, $user);

  // Create a bIlling customer profile.
  $billing_profile = commerce_customer_profile_new('billing', $user->uid);
  $billing_profile->commerce_customer_address[LANGUAGE_NONE][0]['name_line'] = 'Sample Customer';
  $billing_profile->commerce_customer_address[LANGUAGE_NONE][0]['country'] = 'US';
  $billing_profile->commerce_customer_address[LANGUAGE_NONE][0]['thoroughfare'] = '16 Hampton Ct';
  $billing_profile->commerce_customer_address[LANGUAGE_NONE][0]['locality'] = 'Visalia';
  $billing_profile->commerce_customer_address[LANGUAGE_NONE][0]['administrative_area'] = 'CA';
  $billing_profile->commerce_customer_address[LANGUAGE_NONE][0]['postal_code'] = '93277-8329';
  commerce_customer_profile_save($billing_profile);
  commerce_addressbook_set_default_profile($billing_profile);

  // Create two shipping customer profile samples.
  $shipping_profile = commerce_customer_profile_new('shipping', $user->uid);
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['name_line'] = 'Sample Customer';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['country'] = 'US';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['thoroughfare'] = '2843 Sherman Ave';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['locality'] = 'Camden';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['administrative_area'] = 'NJ';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['postal_code'] = '08105-442';
  commerce_customer_profile_save($shipping_profile);
  commerce_addressbook_set_default_profile($shipping_profile);

  $shipping_profile = commerce_customer_profile_new('shipping', $user->uid);
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['name_line'] = 'Sample Customer';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['country'] = 'FR';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['thoroughfare'] = '27 rue des sapins';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['locality'] = 'Paris';
  $shipping_profile->commerce_customer_address[LANGUAGE_NONE][0]['postal_code'] = '75004';
  commerce_customer_profile_save($shipping_profile);

  // Create 3 order samples.
  _commerce_kickstart_create_order($user->uid, 2 , TRUE);
  _commerce_kickstart_create_order($user->uid, 5);
  _commerce_kickstart_create_order($user->uid, 3);

}

/**
 * BatchAPI callback.
 *
 * @see commerce_kickstart_import_content()
 */
function _commerce_kickstart_post_import($operation, &$context) {
  $context['message'] = t('@operation', array('@operation' => $operation));

  _commerce_kickstart_setup_blocks();
  _commerce_kickstart_setup_crumbs();
}

/**
 * Configures the theme and assign blocks to the appropriate regions.
 */
function _commerce_kickstart_setup_blocks() {
  $admin_theme = variable_get('admin_theme', 'commerce_kickstart_admin');
  $default_theme = variable_get('theme_default', 'omega_kickstart');

  $blocks = array(
    array(
      'module' => 'system',
      'delta' => 'help',
      'theme' => $admin_theme,
      'status' => 1,
      'weight' => 0,
      'region' => 'help',
      'pages' => '',
      'cache' => DRUPAL_NO_CACHE,
    ),
    array(
      'module' => 'system',
      'delta' => 'help',
      'theme' => $default_theme,
      'status' => 1,
      'weight' => 0,
      'region' => 'preface_first',
      'pages' => '',
      'cache' => DRUPAL_NO_CACHE,
    ),
    array(
      'module' => 'system',
      'delta' => 'main',
      'theme' => $default_theme,
      'status' => 1,
      'weight' => 0,
      'region' => 'content',
      'pages' => '<front>', // Do not show the block on front.
      'visibility' => 0,
      'cache' => DRUPAL_NO_CACHE,
    ),
    // Connector.
    array(
      'module' => 'connector',
      'delta' => 'one_click_block',
      'theme' => $default_theme,
      'status' => 1,
      'weight' => 1,
      'region' => 'content',
      'pages' => "user\r\nuser/login",
      'visibility' => BLOCK_VISIBILITY_LISTED,
      'cache' => DRUPAL_NO_CACHE,
    ),
    // Search sorting.
    array(
      'module' => 'search_api_sorts',
      'delta' => 'search-sorts',
      'theme' => $default_theme,
      'status' => 1,
      'weight' => -30,
      'region' => 'content',
      'pages' => 'products',
      'visibility' => BLOCK_VISIBILITY_LISTED,
      'cache' => DRUPAL_NO_CACHE,
    ),
    // Commerce checkout progress
    array(
      'module' => 'commerce_checkout_progress',
      'delta' => 'indication',
      'theme' => $default_theme,
      'status' => 1,
      'weight' => -10,
      'region' => 'breadcrumb',
      'pages' => "checkout/*\ncart",
      'visibility' => BLOCK_VISIBILITY_LISTED,
      'cache' => DRUPAL_NO_CACHE,
      'title' => '<none>',
    ),
  );

  drupal_static_reset();
  _block_rehash($admin_theme);
  _block_rehash($default_theme);
  foreach ($blocks as $record) {
    $module = array_shift($record);
    $delta = array_shift($record);
    $theme = array_shift($record);
    db_update('block')
      ->fields($record)
      ->condition('module', $module)
      ->condition('delta', $delta)
      ->condition('theme', $theme)
      ->execute();
  }
}

/**
 * Configures the Crumbs module and enables the required plugins.
 */
function _commerce_kickstart_setup_crumbs() {
  variable_set('crumbs_show_current_page', TRUE);
  variable_set('crumbs_weights', commerce_kickstart_crumbs_get_info());
}

/*
 * Helper function to create a sample order.
 *
 * @param $uid
 * @param $order_state
 * @param $items_number
 */
function _commerce_kickstart_create_order($uid, $items_number, $is_cart = FALSE) {
  // Create order as a cart.
  $order = commerce_order_new($uid, 'cart');
  commerce_order_save($order);

  $message = message_create('commerce_order_created', array('uid' => $uid));
  $wrapper = entity_metadata_wrapper('message', $message);
  $wrapper->message_commerce_order->set($order);
  $wrapper->save();

  // Add products to the cart.
  $line_items = array();
  $products = entity_load('commerce_product');
  $products_key = array_keys($products);
  $products_number = count($products_key);
  $count = $products_number < $items_number ? $products_number: $items_number;
  for($i = 0; $i < $count; $i++) {
    $product = $products[$products_key[rand(0, $products_number - 1)]];
    $line_item = commerce_product_line_item_new($product, 1, $order->order_id);
    commerce_line_item_save($line_item);
    $line_items[] = array('line_item_id' => $line_item->line_item_id);

    $message = message_create('commerce_order_cart_add', array('uid' => $uid));
    $wrapper = entity_metadata_wrapper('message', $message);
    $wrapper->message_commerce_order->set($order);
    $wrapper->message_commerce_line_item->set($line_item);
    $wrapper->save();
  }
  $order->commerce_line_items[LANGUAGE_NONE] = $line_items;

  // Stop here for cart creation.
  if ($is_cart) {
    commerce_order_save($order);
    return;
  }

  // Attach customer profile.
  _commerce_kickstart_update_order_status($order, 'checkout_checkout');
  $billing_profile_id = commerce_addressbook_get_default_profile_id($uid, 'billing');
  $shippin_profile_id = commerce_addressbook_get_default_profile_id($uid, 'shipping');
  if (!empty($billing_profile_id)) {
    $order->commerce_customer_billing[LANGUAGE_NONE][0]['profile_id'] = $billing_profile_id;
  }
  if (!empty($shippin_profile_id)) {
    $order->commerce_customer_shipping[LANGUAGE_NONE][0]['profile_id'] = $shippin_profile_id;
  }

  //@TODO Add shipping service.
  _commerce_kickstart_update_order_status($order, 'checkout_shipping');

  // Proceed to payment.
  _commerce_kickstart_update_order_status($order, 'checkout_review');
  $order->data['payment_method'] = 'commerce_kickstart_payment|commerce_payment_commerce_kickstart_payment';
  $order->data['commerce_kickstart_payment'] = array(
    'credit_card' => array(
      'type' => 'visa',
      'number' => '4111111111111111',
      'exp_month' => '06',
      'exp_year' => '2012',

    ),
  );
  commerce_order_save($order);
  $rule = rules_config_load('commerce_payment_commerce_kickstart_payment');
  $payment_method = commerce_payment_method_load('commerce_kickstart_payment');
  $method_instance_id = commerce_payment_method_instance_id('commerce_kickstart_payment', $rule);
  $method_instance = commerce_payment_method_instance_load($method_instance_id);
  $transaction = commerce_kickstart_payment_transaction($method_instance, $order, $order->commerce_order_total[LANGUAGE_NONE][0], '');

  $message = message_create('commerce_order_payment_entered', array('uid' => $uid));
  $wrapper = entity_metadata_wrapper('message', $message);
  $wrapper->message_commerce_order->set($order);
  $wrapper->message_commerce_payment->set($transaction);
  $wrapper->save();

  _commerce_kickstart_update_order_status($order, 'checkout_payment');
  _commerce_kickstart_update_order_status($order, 'checkout_complete');
  _commerce_kickstart_update_order_status($order, 'pending');
}

/**
 * Helper function to update the order status and set the message.
 *
 * @param $order
 * @param $status
 */
function _commerce_kickstart_update_order_status($order, $status) {
  // We need to harcode the previous state, as the entity will not
  // have this value on run-time.
  $statuses = commerce_order_statuses();
  $arguments = array(
    '@order_status_previous' => $statuses[$order->status]['title'],
    '@order_status' => $statuses[$status]['title'],
  );
  $order = commerce_order_status_update($order, $status);

  $message = message_create('commerce_order_state', array('uid' => $order->uid, 'arguments' => $arguments));
  $wrapper = entity_metadata_wrapper('message', $message);
  $wrapper->message_commerce_order->set($order);
  $wrapper->save();
}
