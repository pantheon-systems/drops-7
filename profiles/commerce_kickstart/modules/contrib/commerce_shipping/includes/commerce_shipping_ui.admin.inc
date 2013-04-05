<?php

/**
 * @file
 * Administrative page callbacks for the Shipping UI module.
 */

/**
 * Displays all shipping methods or services defined for a method in a table.
 */
function commerce_shipping_ui_overview($type, $method = NULL) {
  drupal_add_css(drupal_get_path('module', 'commerce_shipping_ui') . '/theme/commerce_shipping.admin.css');

  // Load the items that will be represented in the overview table.
  if ($type == 'methods') {
    $items = commerce_shipping_methods();
  }
  else {
    $items = commerce_shipping_services($method);
    uasort($items, 'drupal_sort_weight');
  }

  $header = array(
    t('Title'),
    t('Operations'),
  );

  $rows = array();

  // Loop through all of the items to include in the overview.
  foreach ($items as $name => $item) {
    if ($item['admin_list']) {
      // Build the operation links for the current item.
      $arg = ($type == 'methods') ? strtr($name, '_', '-') : strtr($method . '-' . $name, '_', '-');

      $links = menu_contextual_links('commerce-shipping-' . $type, 'admin/commerce/config/shipping/' . $type, array($arg));

      // Add the item's row to the table's rows array.
      $rows[] = array(
        ($type == 'methods') ? theme('shipping_method_admin_overview', array('shipping_method' => $item)) : theme('shipping_service_admin_overview', array('shipping_service' => $item)),
        theme('links', array('links' => $links, 'attributes' => array('class' => 'links inline operations'))),
      );
    }
  }

  // If no items are defined...
  if (empty($rows)) {
    // Add a standard empty row with a link to add a new item.
    if ($type == 'methods' || $method == NULL) {
      $empty_text = t('There are no shipping methods enabled.');
    }
    else {
      $empty_text = t('There are no services defined for the %title shipping method.', array('%title' => commerce_shipping_method_get_title($method)));
    }

    $rows[] = array(
      array(
        'data' => $empty_text,
        'colspan' => 2,
      )
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Builds an overview of a shipping method for display to an administrator.
 *
 * @param $variables
 *   An array of variables used to generate the display; by default includes the
 *     shipping_method key with a value of the shipping method info array.
 *
 * @ingroup themeable
 */
function theme_shipping_method_admin_overview($variables) {
  $shipping_method = $variables['shipping_method'];

  // Build the actual output.
  $output = check_plain($shipping_method['title']);
  $output .= ' <small> (Machine name: ' . check_plain($shipping_method['name']) . ')</small>';
  $output .= '<div class="description">' . filter_xss_admin($shipping_method['description']) . '</div>';

  return $output;
}

/**
 * Builds an overview of a shipping service for display to an administrator.
 *
 * @param $variables
 *   An array of variables used to generate the display; by default includes the
 *     shipping_service key with a value of the shipping service info array.
 *
 * @ingroup themeable
 */
function theme_shipping_service_admin_overview($variables) {
  $shipping_service = $variables['shipping_service'];

  $output = check_plain($shipping_service['title']);
  $output .= ' <small> (Machine name: ' . check_plain($shipping_service['name']) . ')</small>';
  $output .= '<div class="description">' . filter_xss_admin($shipping_service['description']) . '</div>';

  return $output;
}

/**
 * Builds the shipping rate calculation Rules Overview page.
 */
function commerce_shipping_ui_rate_calculation_rules() {
  RulesPluginUI::$basePath = 'admin/commerce/config/shipping/calculation-rules';
  $options = array('show plugin' => FALSE);

  $content['enabled']['title']['#markup'] = '<h3>' . t('Enabled shipping rate calculation rules') . '</h3>';

  $conditions = array('event' => 'commerce_shipping_calculate_rate', 'plugin' => 'reaction rule', 'active' => TRUE);
  $content['enabled']['rules'] = RulesPluginUI::overviewTable($conditions, $options);
  $content['enabled']['rules']['#empty'] = t('There are no active shipping rate calculation rules.');

  $content['disabled']['title']['#markup'] = '<h3>' . t('Disabled shipping rate calculation rules') . '</h3>';

  $conditions['active'] = FALSE;
  $content['disabled']['rules'] = RulesPluginUI::overviewTable($conditions, $options);
  $content['disabled']['rules']['#empty'] = t('There are no disabled shipping rate calculation rules.');

  return $content;
}
