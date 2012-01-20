<?php
/**
 * @file
 * openpublish_related_content.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function openpublish_related_content_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'openpublish_related_content';
  $view->description = 'Base view for displaying related content set for node reference in content.';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'OpenPublish Related Content';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Also See';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['hide_empty'] = 0;
  $handler->display->display_options['row_options']['default_field_elements'] = 0;
  /* Field: Content: Related Content */
  $handler->display->display_options['fields']['field_op_related_nref']['id'] = 'field_op_related_nref';
  $handler->display->display_options['fields']['field_op_related_nref']['table'] = 'field_data_field_op_related_nref';
  $handler->display->display_options['fields']['field_op_related_nref']['field'] = 'field_op_related_nref';
  $handler->display->display_options['fields']['field_op_related_nref']['label'] = '';
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_related_nref']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_related_nref']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_related_nref']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_related_nref']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['field_api_classes'] = 0;
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['nid']['not'] = 0;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $export['openpublish_related_content'] = $view;

  return $export;
}
