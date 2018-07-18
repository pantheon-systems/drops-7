<?php
/**
 * @file
 * cu_feature_callout.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function cu_feature_callout_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'feature_callouts';
  $view->description = 'Feature callouts view.';
  $view->tag = 'callout';
  $view->base_table = 'bean';
  $view->human_name = 'Feature Callouts';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['css_class'] = 'clearfix';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['style_options']['row_class'] = 'clearfix';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Relationship: Block: Callouts (field_callouts) */
  $handler->display->display_options['relationships']['field_callouts_value']['id'] = 'field_callouts_value';
  $handler->display->display_options['relationships']['field_callouts_value']['table'] = 'field_data_field_callouts';
  $handler->display->display_options['relationships']['field_callouts_value']['field'] = 'field_callouts_value';
  $handler->display->display_options['relationships']['field_callouts_value']['delta'] = '-1';
  /* Field: Block: Internal, numeric block ID */
  $handler->display->display_options['fields']['bid']['id'] = 'bid';
  $handler->display->display_options['fields']['bid']['table'] = 'bean';
  $handler->display->display_options['fields']['bid']['field'] = 'bid';
  $handler->display->display_options['fields']['bid']['exclude'] = TRUE;
  /* Field: Field collection item: Image */
  $handler->display->display_options['fields']['field_callout_image']['id'] = 'field_callout_image';
  $handler->display->display_options['fields']['field_callout_image']['table'] = 'field_data_field_callout_image';
  $handler->display->display_options['fields']['field_callout_image']['field'] = 'field_callout_image';
  $handler->display->display_options['fields']['field_callout_image']['relationship'] = 'field_callouts_value';
  $handler->display->display_options['fields']['field_callout_image']['label'] = '';
  $handler->display->display_options['fields']['field_callout_image']['element_type'] = '0';
  $handler->display->display_options['fields']['field_callout_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_callout_image']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_callout_image']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_callout_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_callout_image']['settings'] = array(
    'image_style' => 'square_thumbnail',
    'image_link' => '',
  );
  $handler->display->display_options['fields']['field_callout_image']['field_api_classes'] = TRUE;
  /* Field: Field collection item: Title */
  $handler->display->display_options['fields']['field_callout_title']['id'] = 'field_callout_title';
  $handler->display->display_options['fields']['field_callout_title']['table'] = 'field_data_field_callout_title';
  $handler->display->display_options['fields']['field_callout_title']['field'] = 'field_callout_title';
  $handler->display->display_options['fields']['field_callout_title']['relationship'] = 'field_callouts_value';
  $handler->display->display_options['fields']['field_callout_title']['label'] = '';
  $handler->display->display_options['fields']['field_callout_title']['element_type'] = 'h3';
  $handler->display->display_options['fields']['field_callout_title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_callout_title']['click_sort_column'] = 'url';
  /* Field: Field collection item: Text */
  $handler->display->display_options['fields']['field_callout_text']['id'] = 'field_callout_text';
  $handler->display->display_options['fields']['field_callout_text']['table'] = 'field_data_field_callout_text';
  $handler->display->display_options['fields']['field_callout_text']['field'] = 'field_callout_text';
  $handler->display->display_options['fields']['field_callout_text']['relationship'] = 'field_callouts_value';
  $handler->display->display_options['fields']['field_callout_text']['label'] = '';
  $handler->display->display_options['fields']['field_callout_text']['element_label_colon'] = FALSE;
  /* Contextual filter: Block: Internal, numeric block ID */
  $handler->display->display_options['arguments']['bid']['id'] = 'bid';
  $handler->display->display_options['arguments']['bid']['table'] = 'bean';
  $handler->display->display_options['arguments']['bid']['field'] = 'bid';
  $handler->display->display_options['arguments']['bid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['bid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['bid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['bid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['bid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Block: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'bean';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'feature_callout' => 'feature_callout',
  );

  /* Display: EVA Field */
  $handler = $view->new_display('entity_view', 'EVA Field', 'entity_view_1');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['entity_type'] = 'bean';
  $handler->display->display_options['bundles'] = array(
    0 => 'feature_callout',
  );
  $export['feature_callouts'] = $view;

  return $export;
}
