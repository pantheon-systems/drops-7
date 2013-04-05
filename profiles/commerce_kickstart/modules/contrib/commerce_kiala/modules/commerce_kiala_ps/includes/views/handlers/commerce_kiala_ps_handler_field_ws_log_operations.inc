<?php

/**
 * Field handler to present a Kiala P&S web service log operations links.
 */
class commerce_kiala_ps_handler_field_ws_log_operations extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['log_id'] = 'log_id';
    $this->additional_fields['order_id'] = 'order_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $log_id = $this->get_value($values, 'log_id');
    $order_id = $this->get_value($values, 'order_id');
    $base_path = 'admin/commerce/orders/' . $order_id . '/kiala-packship-ws/logs';

    $links = menu_contextual_links('commerce-kiala-ps-ws-log', $base_path, array($log_id));
    if (!empty($links)) {
      drupal_add_css(drupal_get_path('module', 'commerce_kiala_ps') . '/theme/commerce_kiala_ps.admin.css');
      return theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'operations'))));
    }
  }
}
