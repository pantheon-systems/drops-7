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
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['hide_empty'] = 0;
  $handler->display->display_options['row_options']['default_field_elements'] = 0;
  /* Relationship: Content: Author(s) (field_op_author) */
  $handler->display->display_options['relationships']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['relationships']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['relationships']['field_op_author_nid']['field'] = 'field_op_author_nid';
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
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['fields'] = FALSE;
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
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_related_nref']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_related_nref']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['group_rows'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_related_nref']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['delta_first_last'] = 0;
  $handler->display->display_options['fields']['field_op_related_nref']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Related Content (field_op_related_nref) */
  $handler->display->display_options['filters']['field_op_related_nref_nid']['id'] = 'field_op_related_nref_nid';
  $handler->display->display_options['filters']['field_op_related_nref_nid']['table'] = 'field_data_field_op_related_nref';
  $handler->display->display_options['filters']['field_op_related_nref_nid']['field'] = 'field_op_related_nref_nid';
  $handler->display->display_options['filters']['field_op_related_nref_nid']['operator'] = 'not empty';

  /* Display: Node Author */
  $handler = $view->new_display('block', 'Node Author', 'block_1');
  $handler->display->display_options['display_description'] = 'Displays an image and name of the node author, as well as the node date.';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['hide_empty'] = 0;
  $handler->display->display_options['row_options']['default_field_elements'] = 1;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['relationships'] = FALSE;
  /* Relationship: Content: Author(s) (field_op_author) - reverse */
  $handler->display->display_options['relationships']['reverse_field_op_author_node']['id'] = 'reverse_field_op_author_node';
  $handler->display->display_options['relationships']['reverse_field_op_author_node']['table'] = 'node';
  $handler->display->display_options['relationships']['reverse_field_op_author_node']['field'] = 'reverse_field_op_author_node';
  $handler->display->display_options['relationships']['reverse_field_op_author_node']['label'] = 'field_op_author_reverse';
  $handler->display->display_options['relationships']['reverse_field_op_author_node']['required'] = 1;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Photo */
  $handler->display->display_options['fields']['field_profile_photo']['id'] = 'field_profile_photo';
  $handler->display->display_options['fields']['field_profile_photo']['table'] = 'field_data_field_profile_photo';
  $handler->display->display_options['fields']['field_profile_photo']['field'] = 'field_profile_photo';
  $handler->display->display_options['fields']['field_profile_photo']['label'] = '';
  $handler->display->display_options['fields']['field_profile_photo']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_profile_photo']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_profile_photo']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_profile_photo']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_profile_photo']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_profile_photo']['settings'] = array(
    'image_style' => 'author_thumbnail',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_profile_photo']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'by';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['external'] = 0;
  $handler->display->display_options['fields']['title']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['title']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['element_label_colon'] = 0;
  $handler->display->display_options['fields']['title']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['relationship'] = 'reverse_field_op_author_node';
  $handler->display->display_options['fields']['created']['label'] = '';
  $handler->display->display_options['fields']['created']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['created']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['created']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['created']['alter']['external'] = 0;
  $handler->display->display_options['fields']['created']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['created']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['created']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['created']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['created']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['created']['alter']['html'] = 0;
  $handler->display->display_options['fields']['created']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['created']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['created']['hide_empty'] = 0;
  $handler->display->display_options['fields']['created']['empty_zero'] = 0;
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['created']['date_format'] = 'custom';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'F j, Y';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'field';
  $handler->display->display_options['arguments']['nid']['default_argument_options']['field'] = 'nodereference::field_op_author';
  $handler->display->display_options['arguments']['nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['nid']['not'] = 0;
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid_1']['id'] = 'nid_1';
  $handler->display->display_options['arguments']['nid_1']['table'] = 'node';
  $handler->display->display_options['arguments']['nid_1']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid_1']['relationship'] = 'reverse_field_op_author_node';
  $handler->display->display_options['arguments']['nid_1']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid_1']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['nid_1']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['nid_1']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid_1']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid_1']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['nid_1']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['nid_1']['not'] = 0;
  $export['openpublish_related_content'] = $view;

  return $export;
}
