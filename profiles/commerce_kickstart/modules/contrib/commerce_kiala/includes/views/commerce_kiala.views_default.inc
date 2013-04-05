<?php

/**
 * @file
 * Views for the default Commerce Kiala views.
 */

/**
 * Implements hook_views_default_views().
 */
function commerce_kiala_views_default_views() {
  $views = array();

// -----------------------------------------------------------------------
// Order admin

$view = new view();
$view->name = 'commerce_kiala_orders';
$view->description = 'Display a list of orders for the store admin.';
$view->tag = 'commerce';
$view->base_table = 'commerce_order';
$view->human_name = 'Kiala Orders';
$view->core = 0;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['title'] = 'Kiala Orders';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'view any commerce_order entity';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['query_comment'] = FALSE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = 50;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'order_number' => 'order_number',
  'created' => 'created',
  'commerce_customer_address' => 'commerce_customer_address',
  'name' => 'name',
  'commerce_order_total' => 'commerce_order_total',
  'status' => 'status',
  'operations' => 'operations',
);
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'order_number' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'commerce_customer_address' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'commerce_order_total' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'status' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'operations' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['style_options']['empty_table'] = TRUE;
/* Header: Global: View order form */
$handler->display->display_options['header']['view_order_form']['id'] = 'view_order_form';
$handler->display->display_options['header']['view_order_form']['table'] = 'views';
$handler->display->display_options['header']['view_order_form']['field'] = 'view_order_form';
/* No results behavior: Commerce Order: Empty text */
$handler->display->display_options['empty']['empty_text']['id'] = 'empty_text';
$handler->display->display_options['empty']['empty_text']['table'] = 'commerce_order';
$handler->display->display_options['empty']['empty_text']['field'] = 'empty_text';
$handler->display->display_options['empty']['empty_text']['add_path'] = 'admin/commerce/orders/add';
/* Relationship: Commerce Order: Owner */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'commerce_order';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Relationship: Commerce Order: Referenced customer profile */
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['id'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['table'] = 'field_data_commerce_customer_billing';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['field'] = 'commerce_customer_billing_profile_id';
/* Relationship: Commerce Order: Referenced line item */
$handler->display->display_options['relationships']['commerce_line_items_line_item_id']['id'] = 'commerce_line_items_line_item_id';
$handler->display->display_options['relationships']['commerce_line_items_line_item_id']['table'] = 'field_data_commerce_line_items';
$handler->display->display_options['relationships']['commerce_line_items_line_item_id']['field'] = 'commerce_line_items_line_item_id';
/* Field: Commerce Order: Order number */
$handler->display->display_options['fields']['order_number']['id'] = 'order_number';
$handler->display->display_options['fields']['order_number']['table'] = 'commerce_order';
$handler->display->display_options['fields']['order_number']['field'] = 'order_number';
$handler->display->display_options['fields']['order_number']['link_to_order'] = 'admin';
/* Field: Commerce Order: Created date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'commerce_order';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Created';
$handler->display->display_options['fields']['created']['date_format'] = 'medium';
/* Field: Commerce Customer profile: Address */
$handler->display->display_options['fields']['commerce_customer_address']['id'] = 'commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address']['table'] = 'field_data_commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address']['field'] = 'commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address']['relationship'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['fields']['commerce_customer_address']['label'] = 'Name';
$handler->display->display_options['fields']['commerce_customer_address']['empty'] = '-';
$handler->display->display_options['fields']['commerce_customer_address']['hide_alter_empty'] = FALSE;
$handler->display->display_options['fields']['commerce_customer_address']['click_sort_column'] = 'country';
$handler->display->display_options['fields']['commerce_customer_address']['settings'] = array(
  'use_widget_handlers' => 0,
  'format_handlers' => array(
    'name-oneline' => 'name-oneline',
  ),
);
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'User';
/* Field: Commerce Order: Order total */
$handler->display->display_options['fields']['commerce_order_total']['id'] = 'commerce_order_total';
$handler->display->display_options['fields']['commerce_order_total']['table'] = 'field_data_commerce_order_total';
$handler->display->display_options['fields']['commerce_order_total']['field'] = 'commerce_order_total';
$handler->display->display_options['fields']['commerce_order_total']['label'] = 'Total';
$handler->display->display_options['fields']['commerce_order_total']['click_sort_column'] = 'amount';
$handler->display->display_options['fields']['commerce_order_total']['type'] = 'commerce_price_formatted_amount';
$handler->display->display_options['fields']['commerce_order_total']['settings'] = array(
  'calculation' => FALSE,
);
/* Field: Commerce Order: Order status */
$handler->display->display_options['fields']['status']['id'] = 'status';
$handler->display->display_options['fields']['status']['table'] = 'commerce_order';
$handler->display->display_options['fields']['status']['field'] = 'status';
/* Field: Commerce Order: Operations links */
$handler->display->display_options['fields']['operations']['id'] = 'operations';
$handler->display->display_options['fields']['operations']['table'] = 'commerce_order';
$handler->display->display_options['fields']['operations']['field'] = 'operations';
$handler->display->display_options['fields']['operations']['label'] = 'Operations';
/* Field: Commerce Kiala: Kiala Point ID */
$handler->display->display_options['fields']['point_id']['id'] = 'point_id';
$handler->display->display_options['fields']['point_id']['table'] = 'commerce_kiala_line_item_point';
$handler->display->display_options['fields']['point_id']['field'] = 'point_id';
$handler->display->display_options['fields']['point_id']['relationship'] = 'commerce_line_items_line_item_id';
$handler->display->display_options['fields']['point_id']['separator'] = '';
/* Filter criterion: Commerce Order: Order type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'commerce_order';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['exposed'] = TRUE;
$handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
$handler->display->display_options['filters']['type']['expose']['label'] = 'Order type';
$handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
$handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
$handler->display->display_options['filters']['type']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);

/* Display: Admin page */
$handler = $view->new_display('page', 'Admin page', 'page_1');
$handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Commerce Order: Order number */
$handler->display->display_options['fields']['order_number']['id'] = 'order_number';
$handler->display->display_options['fields']['order_number']['table'] = 'commerce_order';
$handler->display->display_options['fields']['order_number']['field'] = 'order_number';
$handler->display->display_options['fields']['order_number']['link_to_order'] = 'admin';
/* Field: Commerce Order: Created date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'commerce_order';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Created';
$handler->display->display_options['fields']['created']['date_format'] = 'medium';
/* Field: Commerce Customer profile: Address */
$handler->display->display_options['fields']['commerce_customer_address']['id'] = 'commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address']['table'] = 'field_data_commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address']['field'] = 'commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address']['relationship'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['fields']['commerce_customer_address']['label'] = 'Name';
$handler->display->display_options['fields']['commerce_customer_address']['empty'] = '-';
$handler->display->display_options['fields']['commerce_customer_address']['hide_alter_empty'] = FALSE;
$handler->display->display_options['fields']['commerce_customer_address']['click_sort_column'] = 'country';
$handler->display->display_options['fields']['commerce_customer_address']['settings'] = array(
  'use_widget_handlers' => 0,
  'format_handlers' => array(
    'name-oneline' => 'name-oneline',
  ),
);
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'User';
/* Field: Commerce Order: Order total */
$handler->display->display_options['fields']['commerce_order_total']['id'] = 'commerce_order_total';
$handler->display->display_options['fields']['commerce_order_total']['table'] = 'field_data_commerce_order_total';
$handler->display->display_options['fields']['commerce_order_total']['field'] = 'commerce_order_total';
$handler->display->display_options['fields']['commerce_order_total']['label'] = 'Total';
$handler->display->display_options['fields']['commerce_order_total']['click_sort_column'] = 'amount';
$handler->display->display_options['fields']['commerce_order_total']['type'] = 'commerce_price_formatted_amount';
$handler->display->display_options['fields']['commerce_order_total']['settings'] = array(
  'calculation' => FALSE,
);
/* Field: Commerce Order: Order status */
$handler->display->display_options['fields']['status']['id'] = 'status';
$handler->display->display_options['fields']['status']['table'] = 'commerce_order';
$handler->display->display_options['fields']['status']['field'] = 'status';
/* Field: Commerce Kiala: Kiala Point ID */
$handler->display->display_options['fields']['point_id']['id'] = 'point_id';
$handler->display->display_options['fields']['point_id']['table'] = 'commerce_kiala_line_item_point';
$handler->display->display_options['fields']['point_id']['field'] = 'point_id';
$handler->display->display_options['fields']['point_id']['relationship'] = 'commerce_line_items_line_item_id';
$handler->display->display_options['fields']['point_id']['separator'] = '';
/* Field: Commerce Order: Operations links */
$handler->display->display_options['fields']['operations']['id'] = 'operations';
$handler->display->display_options['fields']['operations']['table'] = 'commerce_order';
$handler->display->display_options['fields']['operations']['field'] = 'operations';
$handler->display->display_options['fields']['operations']['label'] = 'Operations';
/* Field: Bulk operations: Commerce Order */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'commerce_order';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_operations'] = array(
  'action::commerce_kiala_ps_bulk_export' => array(
    'selected' => 1,
    'skip_confirmation' => 1,
    'override_label' => 0,
    'label' => '',
  ),
  'action::views_bulk_operations_delete_item' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'action::views_bulk_operations_script_action' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'action::views_bulk_operations_modify_action' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
    'settings' => array(
      'show_all_tokens' => 1,
      'display_values' => array(
        '_all_' => '_all_',
      ),
    ),
  ),
  'action::views_bulk_operations_argument_selector_action' => array(
    'selected' => 0,
    'skip_confirmation' => 1,
    'override_label' => 0,
    'label' => '',
    'settings' => array(
      'url' => 'admin/commerce/orders/kiala-packship-desktop-export',
    ),
  ),
  'rules_component::commerce_shipping_service_express_shipping' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::commerce_shipping_service_free_shipping' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::commerce_shipping_service_kiala_test_service' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::commerce_shipping_service_standard_shipping' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'action::system_send_email_action' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_commerce_order_status_canceled' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_commerce_order_status_cart' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_order_status_checkout_checkout' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_order_status_checkout_complete' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_order_status_checkout_payment' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_order_status_checkout_review' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_order_status_checkout_shipping' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_commerce_order_status_completed' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_commerce_order_status_pending' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
  'rules_component::rules_commerce_order_status_processing' => array(
    'selected' => 0,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'override_label' => 0,
    'label' => '',
  ),
);
$handler->display->display_options['defaults']['filter_groups'] = FALSE;
$handler->display->display_options['defaults']['filters'] = FALSE;
/* Filter criterion: Date: Date (commerce_order) */
$handler->display->display_options['filters']['date_filter']['id'] = 'date_filter';
$handler->display->display_options['filters']['date_filter']['table'] = 'commerce_order';
$handler->display->display_options['filters']['date_filter']['field'] = 'date_filter';
$handler->display->display_options['filters']['date_filter']['operator'] = 'between';
$handler->display->display_options['filters']['date_filter']['group'] = 1;
$handler->display->display_options['filters']['date_filter']['exposed'] = TRUE;
$handler->display->display_options['filters']['date_filter']['expose']['operator_id'] = 'date_filter_op';
$handler->display->display_options['filters']['date_filter']['expose']['label'] = 'Date';
$handler->display->display_options['filters']['date_filter']['expose']['use_operator'] = TRUE;
$handler->display->display_options['filters']['date_filter']['expose']['operator'] = 'date_filter_op';
$handler->display->display_options['filters']['date_filter']['expose']['identifier'] = 'date_filter';
$handler->display->display_options['filters']['date_filter']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['date_filter']['date_fields'] = array(
  'commerce_order.created' => 'commerce_order.created',
);
/* Filter criterion: Commerce Kiala: Kiala Point ID */
$handler->display->display_options['filters']['point_id']['id'] = 'point_id';
$handler->display->display_options['filters']['point_id']['table'] = 'commerce_kiala_line_item_point';
$handler->display->display_options['filters']['point_id']['field'] = 'point_id';
$handler->display->display_options['filters']['point_id']['relationship'] = 'commerce_line_items_line_item_id';
$handler->display->display_options['filters']['point_id']['operator'] = 'not empty';
$handler->display->display_options['filters']['point_id']['group'] = 1;
/* Filter criterion: Commerce Order: Order status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'commerce_order';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['exposed'] = TRUE;
$handler->display->display_options['filters']['status']['expose']['operator_id'] = 'status_op';
$handler->display->display_options['filters']['status']['expose']['label'] = 'Order status';
$handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
$handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
$handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Commerce Kiala: Kiala Point ID */
$handler->display->display_options['filters']['point_id_1']['id'] = 'point_id_1';
$handler->display->display_options['filters']['point_id_1']['table'] = 'commerce_kiala_line_item_point';
$handler->display->display_options['filters']['point_id_1']['field'] = 'point_id';
$handler->display->display_options['filters']['point_id_1']['relationship'] = 'commerce_line_items_line_item_id';
$handler->display->display_options['filters']['point_id_1']['group'] = 1;
$handler->display->display_options['filters']['point_id_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['point_id_1']['expose']['operator_id'] = 'point_id_1_op';
$handler->display->display_options['filters']['point_id_1']['expose']['label'] = 'Kiala Point ID';
$handler->display->display_options['filters']['point_id_1']['expose']['operator'] = 'point_id_1_op';
$handler->display->display_options['filters']['point_id_1']['expose']['identifier'] = 'point_id_1';
$handler->display->display_options['filters']['point_id_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['path'] = 'admin/commerce/orders/kiala';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Kiala';
$handler->display->display_options['menu']['weight'] = '15';
$handler->display->display_options['menu']['name'] = 'management';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['tab_options']['title'] = 'Orders';
$handler->display->display_options['tab_options']['description'] = 'Manage orders in the store.';
$handler->display->display_options['tab_options']['weight'] = '';
$handler->display->display_options['tab_options']['name'] = 'management';

  $views[$view->name] = $view;


  return $views;
}
