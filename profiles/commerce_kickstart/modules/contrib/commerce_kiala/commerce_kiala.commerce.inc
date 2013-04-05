<?php

/**
 * @file
 * Implementations of Commerce Hooks
 */


/**
 * Implements hook_commerce_kiala_settings_info().
 */
function commerce_kiala_commerce_kiala_settings_info() {
  $countries = commerce_kiala_country_get_list();

  return array(
    'dspid' => array(
      'default' => '',
    ),
    'shipping_countries_enabled' => array(
      'default' => array_keys($countries),
    ),
    'country' => array(
      'default' => 'FR',
    ),
    'preparationdelay' => array(
      'default' => 2,
    ),

    'ls_ws_details_url' => array(
      'default' => 'http://locateandselect.kiala.com/kplist',
      'group' => 'ls_ws_details',
    ),
    'ls_ws_details_cache_lifetime' => array(
      'default' => 43200,
      'group' => 'ls_ws_details',
    ),
    'ls_ws_details_image_style' => array(
      'default' => 'thumbnail',
      'group' => 'ls_ws_details',
    ),
    'ls_ws_details_params' => array(
      'group' => 'ls_ws_details',
      'default' => array(),
    ),

    'ls_ws_search_url' => array(
      'default' => 'http://locateandselect.kiala.com/kplist',
      'group' => 'ls_ws_search',
    ),
    'ls_ws_search_params' => array(
      'group' => 'ls_ws_search',
      'default' => array(
        'sort-method' => '',
        'max-result' => '',
        'zipfilter' => '',
      ),
    ),

    'ls_map_search_url' => array(
      'default' => 'http://locateandselect.kiala.com/search',
      'group' => 'ls_map_search',
    ),
    'ls_map_search_embed_type' => array(
      'default' => 'inline',
      'group' => 'ls_map_search',
    ),
    'ls_map_search_modal_link_text' => array(
      'default' => 'Select on Map',
      'group' => 'ls_map_search',
    ),
    'ls_map_search_width' => array(
      'default' => 640,
      'group' => 'ls_map_search',
    ),
    'ls_map_search_height' => array(
      'default' => 480,
      'group' => 'ls_map_search',
    ),
    'ls_map_search_params' => array(
      'group' => 'ls_map_search',
      'default' => array(
        'gui' => 'sleek',
        'target' => '_parent', // "_blank", "_self", "_parent", "_top"
        'select-text' => '',
        'css' => '',
        'header' => '',
        'thumbnails' => '',
        'map' => '',
        'map-controls' => '',
        'align' => '',
        'pl' => 'map',
        'sort-method' => '',
        'max-result' => '',
        'zipfilter' => '',
      ),
    ),

    'ls_map_details_url' => array(
      'default' => 'http://locateandselect.kiala.com/locateandselect/details',
      'group' => 'ls_map_details',
    ),
    'ls_map_details_embed_type' => array(
      'default' => 'inline',
      'group' => 'ls_map_details',
    ),
    'ls_map_details_modal_link_text' => array(
      'default' => 'Show Kiala Point',
      'group' => 'ls_map_details',
    ),
    'ls_map_details_width' => array(
      'default' => 640,
      'group' => 'ls_map_details',
    ),
    'ls_map_details_height' => array(
      'default' => 480,
      'group' => 'ls_map_details',
    ),
    'ls_map_details_params' => array(
      'group' => 'ls_map_details',
      'default' => array(
        'gui' => 'sleek',
        'map' => 'on',
        'css' => '',
      ),
    ),
  );
}

/**
 * Implements hook_commerce_shipping_method_info().
 */
function commerce_kiala_commerce_shipping_method_info() {
  return array(
    'kiala' => array(
      'title' => t('Kiala'),
      'description' => t('Lets you create Kiala services, specifying their base rates and use Rules to apply additional rate calculation logic.'),
    ),
  );
}

/**
 * Implements hook_commerce_shipping_service_info().
 */
function commerce_kiala_commerce_shipping_service_info() {
  $services = array();

  // Look for services currently defined in the database.
  $result = db_query('SELECT * FROM {commerce_kiala_service}')->fetchAllAssoc('name', PDO::FETCH_ASSOC);

  if (!empty($result)) {
    foreach ($result as $name => $service) {
      // Create a base rate price array for the service
      $base_rate = array(
        'amount' => $service['amount'],
        'currency_code' => $service['currency_code'],
        'data' => array(),
      );

      // Unserialize the data array for the service.
      $data = !empty($service['data']) ? unserialize($service['data']) : array();

      $price_component_type = 'kiala_' . $name;

      // If the data array specifies an included tax, include it now.
      if (!empty($data['include_tax']) && $tax_rate = commerce_tax_rate_load($data['include_tax'])) {
        // Reverse apply the tax.
        $tax_amount = $base_rate['amount'] - ($base_rate['amount'] / (1 + $tax_rate['rate']));
        $tax_amount = commerce_tax_rate_round_amount($tax_rate, $tax_amount);

        // Add a base price to the data array.
        $component = array(
          'amount' => $base_rate['amount'] - $tax_amount,
          'currency_code' => $base_rate['currency_code'],
          'data' => array(),
        );

        $base_rate['data'] = commerce_price_component_add($base_rate, $price_component_type, $component, TRUE, FALSE);

        // Add the tax to the data array.
        $component['amount'] = $tax_amount;
        $component['data']['tax_rate'] = $tax_rate;

        $base_rate['data'] = commerce_price_component_add($base_rate, $tax_rate['price_component'], $component, TRUE);
      }

      // Add the full service array to our return value.
      $services[$name] = array(
        'title' => $service['title'],
        'display_title' => !empty($service['display_title']) ? $service['display_title'] : $service['title'],
        'description' => $service['description'],
        'shipping_method' => 'kiala',
        'rules_component' => !empty($service['rules_component']),
        'price_component' => $price_component_type,
        'callbacks' => array(
          'rate' => 'commerce_kiala_service_rate_order',
          'details_form' => 'commerce_kiala_service_details_form',
          'details_form_validate' => 'commerce_kiala_service_details_form_validate',
          'details_form_submit' => 'commerce_kiala_service_details_form_submit',
        ),
        'base_rate' => $base_rate,
        'data' => $data,
      );
    }
  }

  return $services;
}

/**
 * Shipping service Rate callback
 *
 * @return
 *  The base rate for a service or FALSE if not available.
 */
function commerce_kiala_service_rate_order($shipping_service, $order) {
  if (commerce_kiala_method_rate_access($shipping_service, $order)) {
    return $shipping_service['base_rate'];
  }

  return FALSE;
}

/**
 * Shipping method rate access check
 *
 * Common rate checks for all Kiala services
 * - Ensure address
 * - Restrict services based on enabled countries
 * - Check if order is shippable
 *
 * @return
 *  The TRUE if service is allowed or FALSE if not available.
 */
function commerce_kiala_method_rate_access($shipping_service, $order) {
  // extract recipient address from order
  $address = commerce_kiala_order_recipient_address($order);

  // DENY if there is not a country set
  if (empty($address) || empty($address['country'])) {
    return TRUE;
  }

  // DENY if address is restricted by the Kiala settings
  if (!commerce_kiala_can_ship_to_country($address['country']) ) {
    return FALSE;
  }

  // ALLOW by default
  return TRUE;
}

/**
 * Shipping service Details Form callback
 */
function commerce_kiala_service_details_form($pane_form, $pane_values, $checkout_pane, $order, $shipping_service) {
  global $user;

  $details_form = array();
  $details_values = !empty($pane_values['service_details']) ? $pane_values['service_details'] : array();
  $method_settings = commerce_kiala_settings(array(
    'include_passwords' => FALSE,
    'decrypt' => TRUE,
  ));

  // wrap the order
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // determine embed type
  $map_embed_type = 'inline';
  if (!empty($method_settings['ls_map_search_embed_type'])) {
    $map_embed_type = $method_settings['ls_map_search_embed_type'];
  }

  // extract recipient address
  $recipient_address = commerce_kiala_order_recipient_address($order);

  // create the map select back url
  $map_back_url = url(commerce_kiala_create_map_select_callback_path($order, $shipping_service['name']), array(
    'absolute' => TRUE,
    'query' => array(
      'destination' => 'checkout/' . $order->order_id . '/shipping',
    ),
  ));
  $map_back_url .= (strpos($map_back_url, '?') !== FALSE ? '&' : '?&');

  // Set the map parameters
  $map_params = array('bckUrl' => $map_back_url);
  $map_params += commerce_kiala_checkout_point_search_api_params($order);

  // build point options
  $kp_options = array();

  // Add the order history option
  if (!empty($order->uid)) {
    $owner_last_point_id = commerce_kiala_user_last_point($order_wrapper->owner->value());
    if (!empty($owner_last_point_id)) {
      $kp_options[$owner_last_point_id] = t('Previously selected Kiala Point');
    }
  }

  // Add the closest point option
  $kp_closest_data = commerce_kiala_get_closest_point_details($recipient_address['zip'], $map_params);
  if (!empty($kp_closest_data)) {
    $kp_closest_point_id = $kp_closest_data['shortId'];
    if (!isset($kp_options[$kp_closest_point_id])) {
      $kp_options[$kp_closest_point_id] = t('Closest Kiala Point');
    }
  }

  // determine default point
  $kp_default = NULL;
  if (isset($details_values['kiala_point'])) {
    $kp_default = $details_values['kiala_point'];
    $kp_options[$kp_default] = t('Currently selected Kiala Point');
  }

  // ensure default
  if (!isset($kp_options[$kp_default])) {
    // if default and is not an option, then add it
    $kp_options[$kp_default] = $kp_default;
  }

  // render previous points
  $checkout_details_api_url_params = commerce_kiala_checkout_point_details_api_params($order);
  foreach ($kp_options as $point_id => $point_label) {
    $kp_data = commerce_kiala_get_point_details($point_id, $checkout_details_api_url_params);

    // only provide available points
    if (empty($kp_data) || empty($kp_data['status']['available'])) {
      unset($kp_options[$point_id]);
      continue;
    }

    // add a title if the label is not just the point id
    $kp_point_title = NULL;
    if (!empty($point_label) && !is_numeric($point_label)) {
      $kp_point_title = $point_label;
    }

    // theme point details
    $kp_options[$point_id] = theme('commerce_kiala_point_details', array(
      'point_details' => $kp_data,
      'point_title' => $kp_point_title,
    ));

    $kp_options[$point_id] .= '<span class="button">' . t('Select') . '</span>';
    $kp_options[$point_id] .= '<span class="button button-selected">' . t('Selected') . '</span>';
  }


  // Build form elements

  // Kiala help
  if (!empty($shipping_service['description'])) {
    $details_form['kiala_point_description'] = array(
      '#type' => 'item',
      'help' => array(
        '#prefix' => '<span class="commerce-kiala-service-info-label">' . t('Collection Point Delivery') . '</span>',
        '#markup' => '<span class="commerce-kiala-service-info" title="' . $shipping_service['description']. '"></span>',
      ),
    );
  }

  // Point options
  if (!empty($kp_options)) {
    $details_form['kiala_point'] = array(
      '#type' => 'radios',
      '#default_value' => $kp_default,
      '#options' => $kp_options,
      '#attributes' => array('class' => array('commerce-kiala-point-option clearfix')),
    );
  }

  // Search Map
  $map_select_element = commerce_kiala_render_embedded_search_map($recipient_address['zip'], $map_params);
  $map_select_token = commerce_kiala_map_select_get_token();

  if (!empty($map_select_element) && commerce_kiala_map_select_access($order, $shipping_service, $map_select_token)) {
    $details_form['kiala_point_map_select'] = $map_select_element + array(
      '#prefix' => '<div class="commerce-kiala-point-map-select">',
      '#suffix' => '</div>',
      '#weight' => 10,
    );

    $details_form['kiala_point_map_select'] += array(
      '#title' => t('Select another Kiala Point on the map'),
    );
  }

  $details_form['#attached']['css'][] = drupal_get_path('module', 'commerce_kiala') . '/theme/commerce_kiala.css';
  $details_form['#attached']['js'][] = drupal_get_path('module', 'commerce_kiala') . '/theme/commerce_kiala.js';
  $details_form['#attributes']['class'][] = 'clearfix';
  $details_form['#attributes']['class'][] = 'commerce-kiala-service-details-form';
  $details_form['#attributes']['class'][] = drupal_html_class('commerce-kiala-service-details-form-'. $map_embed_type);


  return $details_form;
}

/**
 * Shipping service Details Form Validate callback
 */
function commerce_kiala_service_details_form_validate($details_form, $details_values, $shipping_service, $order, $form_parents) {
  // Require a point selection even if no options shown and have access to select on the map
  if (empty($details_values['kiala_point'])) {
    $map_select_token = commerce_kiala_map_select_get_token();
    if (commerce_kiala_map_select_access($order, $shipping_service, $map_select_token)) {
      drupal_set_message(t('Please select a Kiala Point where you will collect your order.'), 'error');
    }
    else {
      drupal_set_message(t('Please select a valid shipping service.'), 'error');
    }

    return FALSE;
  }
}

/**
 * Shipping service Details Form Submit callback
 */
function commerce_kiala_service_details_form_submit($details_form, $details_values, $line_item) {
}


/**
 * Implements hook_commerce_checkout_pane_info()
 */
function commerce_kiala_commerce_checkout_pane_info() {
  $checkout_panes = array();

  $checkout_panes['commerce_kiala_order_point_pane'] = array(
    'title' => t('Kiala Delivery Point Selected'),
    'file' => 'includes/commerce_kiala.checkout_pane.inc',
    'page' => 'review',
    'weight' => 8,
  );

  return $checkout_panes;
}
