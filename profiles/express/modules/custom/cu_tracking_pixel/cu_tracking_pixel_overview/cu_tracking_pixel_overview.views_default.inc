<?php
/**
 * @file
 * cu_tracking_pixel_overview.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function cu_tracking_pixel_overview_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'manage_tracking_pixels';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'tracking_pixel';
  $view->human_name = 'Manage Tracking Pixels';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Manage Tracking Pixels';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view tracking pixels overview';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'id' => 'id',
    'field_tracking_name' => 'field_tracking_name',
    'field_tracking_path' => 'field_tracking_path',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_tracking_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_tracking_path' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'You have no tracking pixels.';
  $handler->display->display_options['empty']['area']['format'] = 'wysiwyg';
  /* Field: Tracking Pixel: Tracking pixel ID */
  $handler->display->display_options['fields']['id']['id'] = 'id';
  $handler->display->display_options['fields']['id']['table'] = 'tracking_pixel';
  $handler->display->display_options['fields']['id']['field'] = 'id';
  $handler->display->display_options['fields']['id']['exclude'] = TRUE;
  /* Field: Tracking Pixel: Name */
  $handler->display->display_options['fields']['field_tracking_name']['id'] = 'field_tracking_name';
  $handler->display->display_options['fields']['field_tracking_name']['table'] = 'field_data_field_tracking_name';
  $handler->display->display_options['fields']['field_tracking_name']['field'] = 'field_tracking_name';
  $handler->display->display_options['fields']['field_tracking_name']['element_type'] = '0';
  $handler->display->display_options['fields']['field_tracking_name']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_tracking_name']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_tracking_name']['field_api_classes'] = TRUE;
  /* Field: Tracking Pixel: Path */
  $handler->display->display_options['fields']['field_tracking_path']['id'] = 'field_tracking_path';
  $handler->display->display_options['fields']['field_tracking_path']['table'] = 'field_data_field_tracking_path';
  $handler->display->display_options['fields']['field_tracking_path']['field'] = 'field_tracking_path';
  $handler->display->display_options['fields']['field_tracking_path']['element_type'] = '0';
  $handler->display->display_options['fields']['field_tracking_path']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_tracking_path']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_tracking_path']['click_sort_column'] = 'url';
  $handler->display->display_options['fields']['field_tracking_path']['type'] = 'link_url';
  $handler->display->display_options['fields']['field_tracking_path']['field_api_classes'] = TRUE;
  /* Field: Tracking Pixel: Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'tracking_pixel';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['status']['alter']['text'] = 'Published';
  $handler->display->display_options['fields']['status']['empty'] = 'Unpublished';
  $handler->display->display_options['fields']['status']['empty_zero'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/content/trackingpixels/[id]/edit';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/content/trackingpixels/[id]/delete';
  /* Filter criterion: Tracking Pixel: Name */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'tracking_pixel';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
  );
  /* Filter criterion: Tracking Pixel: Path */
  $handler->display->display_options['filters']['path']['id'] = 'path';
  $handler->display->display_options['filters']['path']['table'] = 'tracking_pixel';
  $handler->display->display_options['filters']['path']['field'] = 'path';
  $handler->display->display_options['filters']['path']['operator'] = 'contains';
  $handler->display->display_options['filters']['path']['exposed'] = TRUE;
  $handler->display->display_options['filters']['path']['expose']['operator_id'] = 'path_op';
  $handler->display->display_options['filters']['path']['expose']['label'] = 'Path';
  $handler->display->display_options['filters']['path']['expose']['operator'] = 'path_op';
  $handler->display->display_options['filters']['path']['expose']['identifier'] = 'path';
  $handler->display->display_options['filters']['path']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/content/trackingpixels';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Tracking Pixels';
  $handler->display->display_options['menu']['description'] = 'Manage Tracking Pixels';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $export['manage_tracking_pixels'] = $view;

  return $export;
}
