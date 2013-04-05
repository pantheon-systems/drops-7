<?php

/**
 * @file
 * Views for the default Commerce Kiala views.
 */

/**
 * Implements hook_views_default_views().
 */
function commerce_kiala_ps_views_default_views() {
  $views = array();

// -----------------------------------------------------------------------
// Web service order logs
  $view = new view();
  $view->name = 'commerce_kiala_ps_ws_order_logs';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'commerce_kiala_ps_ws_log';
  $view->human_name = 'Commerce Kiala Pack & Ship web service order logs ';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Kiala Pack & Ship web service order logs';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view kiala packship ws logs';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['row_class'] = 'kiala-ws-status-[status]';
  $handler->display->display_options['style_options']['columns'] = array(
    'log_id' => 'log_id',
    'timestamp' => 'timestamp',
    'method' => 'method',
    'status' => 'status',
    'tracking_number' => 'tracking_number',
    'operations' => 'operations',
  );
  $handler->display->display_options['style_options']['default'] = 'timestamp';
  $handler->display->display_options['style_options']['info'] = array(
    'log_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'timestamp' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'method' => array(
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
    'tracking_number' => array(
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
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'No service logs found.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Commerce Kiala Pack and Ship Web Service Log: Log ID */
  $handler->display->display_options['fields']['log_id']['id'] = 'log_id';
  $handler->display->display_options['fields']['log_id']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['fields']['log_id']['field'] = 'log_id';
  /* Field: Commerce Kiala Pack and Ship Web Service Log: Timestamp */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'short';
  /* Field: Commerce Kiala Pack and Ship Web Service Log: Service method */
  $handler->display->display_options['fields']['method']['id'] = 'method';
  $handler->display->display_options['fields']['method']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['fields']['method']['field'] = 'method';
  $handler->display->display_options['fields']['method']['label'] = 'Method';
  /* Field: Commerce Kiala Pack and Ship Web Service Log: Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  /* Field: Commerce Kiala Pack and Ship Web Service Log: Kiala Tracking Number */
  $handler->display->display_options['fields']['tracking_number']['id'] = 'tracking_number';
  $handler->display->display_options['fields']['tracking_number']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['fields']['tracking_number']['field'] = 'tracking_number';
  /* Field: Commerce Kiala Pack and Ship Web Service Log: Operations links */
  $handler->display->display_options['fields']['operations']['id'] = 'operations';
  $handler->display->display_options['fields']['operations']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['fields']['operations']['field'] = 'operations';
  $handler->display->display_options['fields']['operations']['label'] = 'Operations';
  /* Contextual filter: Commerce Kiala Pack and Ship Web Service Log: Order ID */
  $handler->display->display_options['arguments']['order_id']['id'] = 'order_id';
  $handler->display->display_options['arguments']['order_id']['table'] = 'commerce_kiala_ps_ws_log';
  $handler->display->display_options['arguments']['order_id']['field'] = 'order_id';
  $handler->display->display_options['arguments']['order_id']['default_action'] = 'empty';
  $handler->display->display_options['arguments']['order_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['order_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['order_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['order_id']['summary_options']['items_per_page'] = '25';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['path'] = 'admin/commerce/orders/%/kiala-packship-ws/logs';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Logs';
  $handler->display->display_options['menu']['weight'] = '10';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;


  $views[$view->name] = $view;


  return $views;
}
