<?php
/**
 * @file
 * openpublish_listings.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function openpublish_listings_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'op_recent_content';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'OpenPublish - Recent Content';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Recent Articles';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';
  $handler->display->display_options['row_options']['links'] = 0;
  $handler->display->display_options['row_options']['comments'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  /* Contextual filter: Global: Limit */
  $handler->display->display_options['arguments']['limit']['id'] = 'limit';
  $handler->display->display_options['arguments']['limit']['table'] = 'views';
  $handler->display->display_options['arguments']['limit']['field'] = 'limit';
  $handler->display->display_options['arguments']['limit']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['limit']['summary']['format'] = 'default_summary';
  /* Contextual filter: Content: Section (field_op_section_term) */
  $handler->display->display_options['arguments']['field_op_section_term_tid']['id'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['table'] = 'field_data_field_op_section_term';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['field'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['format'] = 'default_summary';
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
  );
  /* Filter criterion: Content: Nid */
  $handler->display->display_options['filters']['nid']['id'] = 'nid';
  $handler->display->display_options['filters']['nid']['table'] = 'node';
  $handler->display->display_options['filters']['nid']['field'] = 'nid';

  /* Display: Recent Articles */
  $handler = $view->new_display('block', 'Recent Articles', 'block_recent_articles');
  $handler->display->display_options['display_description'] = 'A block display of the most recent articles.';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: By Author(optional)[node]{bundles:profile} */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['ui_name'] = 'By Author(optional)[node]{bundles:profile}';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_author_nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['not'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
  );

  /* Display: Top Stories */
  $handler = $view->new_display('block', 'Top Stories', 'block_top_stories');
  $handler->display->display_options['display_description'] = 'a curated list of Top Stories.';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'medium',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = 'Posted by';
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_type'] = 'span';
  $handler->display->display_options['fields']['field_op_author']['element_label_type'] = 'span';
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Arguments: Multi-item Argument Order */
  $handler->display->display_options['sorts']['weight']['id'] = 'weight';
  $handler->display->display_options['sorts']['weight']['table'] = 'views_sort_by_arg_order';
  $handler->display->display_options['sorts']['weight']['field'] = 'weight';
  $handler->display->display_options['sorts']['weight']['order'] = 'DESC';
  $handler->display->display_options['sorts']['weight']['argument_number'] = '0';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: All Content Nodes */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['ui_name'] = 'All Content Nodes';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['nid']['break_phrase'] = 1;
  $handler->display->display_options['arguments']['nid']['not'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'all' => 'all',
    'openpublish_article' => 'openpublish_article',
    'openpublish_blog_post' => 'openpublish_blog_post',
    'openpublish_photo' => 'openpublish_photo',
    'openpublish_photo_gallery' => 'openpublish_photo_gallery',
    'profile' => 'profile',
    'section_front' => 'section_front',
    'openpublish_site_page' => 'openpublish_site_page',
    'openpublish_video' => 'openpublish_video',
  );

  /* Display: Latest Galleries (by section) */
  $handler = $view->new_display('block', 'Latest Galleries (by section)', 'block_recent_galleries_section');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Latest Galleries';
  $handler->display->display_options['display_description'] = 'A block display of the most recent galleries filtered by section.';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '3';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_type'] = '0';
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'thumbnail',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['external'] = 0;
  $handler->display->display_options['fields']['title']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['title']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['element_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_photo_gallery' => 'openpublish_photo_gallery',
  );
  /* Filter criterion: Taxonomy Filter: Node Taxonomy Filter */
  $handler->display->display_options['filters']['nid']['id'] = 'nid';
  $handler->display->display_options['filters']['nid']['table'] = 'views_node_taxonomy_filter';
  $handler->display->display_options['filters']['nid']['field'] = 'nid';
  $handler->display->display_options['filters']['nid']['value'] = array(
    0 => array(
      'sections' => 'sections',
    ),
    1 => 1,
  );

  /* Display: More Galleries */
  $handler = $view->new_display('block', 'More Galleries', 'block_more_galleries');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'More Photo Galleries';
  $handler->display->display_options['display_description'] = 'A block display of the most recent galleries.';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '3';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_type'] = '0';
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'medium',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['external'] = 0;
  $handler->display->display_options['fields']['title']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['title']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['element_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '250',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_photo_gallery' => 'openpublish_photo_gallery',
  );

  /* Display: Recent Articles (filtered) */
  $handler = $view->new_display('block', 'Recent Articles (filtered)', 'block_recent_articles_taxonomy');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Recent Articles';
  $handler->display->display_options['display_description'] = 'A block display of the most recent articles manually filtered by section.';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
  );
  /* Filter criterion: Content: Has taxonomy term */
  $handler->display->display_options['filters']['tid']['id'] = 'tid';
  $handler->display->display_options['filters']['tid']['table'] = 'taxonomy_index';
  $handler->display->display_options['filters']['tid']['field'] = 'tid';
  $handler->display->display_options['filters']['tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['tid']['expose']['operator_id'] = 'tid_op';
  $handler->display->display_options['filters']['tid']['expose']['label'] = 'Has taxonomy term';
  $handler->display->display_options['filters']['tid']['expose']['operator'] = 'tid_op';
  $handler->display->display_options['filters']['tid']['expose']['identifier'] = 'tid';
  $handler->display->display_options['filters']['tid']['expose']['required'] = 1;
  $handler->display->display_options['filters']['tid']['expose']['reduce'] = 0;
  $handler->display->display_options['filters']['tid']['reduce_duplicates'] = 0;
  $handler->display->display_options['filters']['tid']['type'] = 'select';
  $handler->display->display_options['filters']['tid']['vocabulary'] = 'sections';
  $handler->display->display_options['filters']['tid']['hierarchy'] = 1;
  $handler->display->display_options['filters']['tid']['error_message'] = 1;

  /* Display: Author Articles Block */
  $handler = $view->new_display('block', 'Author Articles Block', 'block_1');
  $handler->display->display_options['display_description'] = 'Block for Authors\' pages to display current content from that author';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'custom';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'F jS, Y';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_author_nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['not'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
    'openpublish_blog_post' => 'openpublish_blog_post',
    'openpublish_photo_gallery' => 'openpublish_photo_gallery',
    'openpublish_video' => 'openpublish_video',
  );

  /* Display: Author Articles Block Lite */
  $handler = $view->new_display('block', 'Author Articles Block Lite', 'block_2');
  $handler->display->display_options['display_description'] = 'Lighter display for Author content mean to show just titles and can receive an arg for number of items to show.';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = '';
  $handler->display->display_options['fields']['created']['exclude'] = TRUE;
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'custom';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'F jS, Y';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['exclude'] = TRUE;
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_author_nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['not'] = 0;
  /* Contextual filter: Global: Limit */
  $handler->display->display_options['arguments']['limit']['id'] = 'limit';
  $handler->display->display_options['arguments']['limit']['table'] = 'views';
  $handler->display->display_options['arguments']['limit']['field'] = 'limit';
  $handler->display->display_options['arguments']['limit']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['limit']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['limit']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['limit']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['limit']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['limit']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
    'openpublish_blog_post' => 'openpublish_blog_post',
    'openpublish_photo_gallery' => 'openpublish_photo_gallery',
    'openpublish_video' => 'openpublish_video',
  );

  /* Display: Recent Videos */
  $handler = $view->new_display('block', 'Recent Videos', 'block_3');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Recent Videos';
  $handler->display->display_options['display_description'] = 'View of the most recent videos';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['exclude'] = TRUE;
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'medium',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_video' => 'openpublish_video',
  );

  /* Display: Recent Videos... (by section) */
  $handler = $view->new_display('block', 'Recent Videos... (by section)', 'block_4');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Recent Videos (by section)';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['exclude'] = TRUE;
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'medium',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_author_nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_author_nid']['not'] = 0;
  /* Contextual filter: Global: Limit */
  $handler->display->display_options['arguments']['limit']['id'] = 'limit';
  $handler->display->display_options['arguments']['limit']['table'] = 'views';
  $handler->display->display_options['arguments']['limit']['field'] = 'limit';
  $handler->display->display_options['arguments']['limit']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['limit']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['limit']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['limit']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['limit']['summary_options']['items_per_page'] = '25';
  /* Contextual filter: Content: Section (field_op_section_term) */
  $handler->display->display_options['arguments']['field_op_section_term_tid']['id'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['table'] = 'field_data_field_op_section_term';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['field'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_type'] = 'field';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_options']['field'] = 'content_taxonomy::field_op_section_term';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_options']['settings'] = array(
    'content_taxonomy_format' => 'name',
  );
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_section_term_tid']['not'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_video' => 'openpublish_video',
  );
  /* Filter criterion: Taxonomy Filter: Node Taxonomy Filter */
  $handler->display->display_options['filters']['nid']['id'] = 'nid';
  $handler->display->display_options['filters']['nid']['table'] = 'views_node_taxonomy_filter';
  $handler->display->display_options['filters']['nid']['field'] = 'nid';
  $handler->display->display_options['filters']['nid']['value'] = array(
    0 => array(
      'sections' => 'sections',
    ),
    1 => 0,
  );

  /* Display: Recent Articles Carousel (by section) */
  $handler = $view->new_display('block', 'Recent Articles Carousel (by section)', 'block_5');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Recent Articles Carousel (by Section)';
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'jcarousel';
  $handler->display->display_options['style_options']['wrap'] = 'both';
  $handler->display->display_options['style_options']['skin'] = 'tango';
  $handler->display->display_options['style_options']['visible'] = '';
  $handler->display->display_options['style_options']['auto'] = '0';
  $handler->display->display_options['style_options']['autoPause'] = 1;
  $handler->display->display_options['style_options']['easing'] = '';
  $handler->display->display_options['style_options']['vertical'] = 0;
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'thumbnail',
    'image_link' => '',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = '';
  $handler->display->display_options['fields']['created']['exclude'] = TRUE;
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['exclude'] = TRUE;
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  /* Contextual filter: Global: Limit */
  $handler->display->display_options['arguments']['limit']['id'] = 'limit';
  $handler->display->display_options['arguments']['limit']['table'] = 'views';
  $handler->display->display_options['arguments']['limit']['field'] = 'limit';
  $handler->display->display_options['arguments']['limit']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['limit']['summary']['format'] = 'default_summary';
  /* Contextual filter: Content: Section (field_op_section_term) */
  $handler->display->display_options['arguments']['field_op_section_term_tid']['id'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['table'] = 'field_data_field_op_section_term';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['field'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_type'] = 'field';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_options']['field'] = 'content_taxonomy::field_op_section_term';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_options']['settings'] = array(
    'content_taxonomy_format' => 'tid',
  );
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_section_term_tid']['not'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
  );
  /* Filter criterion: Taxonomy Filter: Node Taxonomy Filter */
  $handler->display->display_options['filters']['nid']['id'] = 'nid';
  $handler->display->display_options['filters']['nid']['table'] = 'views_node_taxonomy_filter';
  $handler->display->display_options['filters']['nid']['field'] = 'nid';
  $handler->display->display_options['filters']['nid']['value'] = array(
    0 => array(
      'sections' => 'sections',
    ),
    1 => 0,
  );

  /* Display: Recent Blogs Carousel (by section) */
  $handler = $view->new_display('block', 'Recent Blogs Carousel (by section)', 'block_6');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Recent Blogs Carousel (by Section)';
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'jcarousel';
  $handler->display->display_options['style_options']['wrap'] = 'both';
  $handler->display->display_options['style_options']['skin'] = 'tango';
  $handler->display->display_options['style_options']['visible'] = '';
  $handler->display->display_options['style_options']['auto'] = '0';
  $handler->display->display_options['style_options']['autoPause'] = 1;
  $handler->display->display_options['style_options']['easing'] = '';
  $handler->display->display_options['style_options']['vertical'] = 0;
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Author(s) */
  $handler->display->display_options['fields']['field_op_author']['id'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['fields']['field_op_author']['field'] = 'field_op_author';
  $handler->display->display_options['fields']['field_op_author']['label'] = '';
  $handler->display->display_options['fields']['field_op_author']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_op_author']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_author']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_author']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_author']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_author']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_author']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_author']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['field_op_author']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_op_author']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_op_author']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_op_author']['field_api_classes'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = '';
  $handler->display->display_options['fields']['created']['exclude'] = TRUE;
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
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['created']['date_format'] = 'time ago';
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['exclude'] = TRUE;
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '300',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  /* Field: Content: Main Image */
  $handler->display->display_options['fields']['field_op_main_image']['id'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['table'] = 'field_data_field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['field'] = 'field_op_main_image';
  $handler->display->display_options['fields']['field_op_main_image']['label'] = '';
  $handler->display->display_options['fields']['field_op_main_image']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_op_main_image']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_op_main_image']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_op_main_image']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_op_main_image']['settings'] = array(
    'image_style' => 'thumbnail',
    'image_link' => '',
  );
  $handler->display->display_options['fields']['field_op_main_image']['field_api_classes'] = 0;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Author(s) (field_op_author) */
  $handler->display->display_options['arguments']['field_op_author_nid']['id'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['table'] = 'field_data_field_op_author';
  $handler->display->display_options['arguments']['field_op_author_nid']['field'] = 'field_op_author_nid';
  $handler->display->display_options['arguments']['field_op_author_nid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_op_author_nid']['summary']['format'] = 'default_summary';
  /* Contextual filter: Global: Limit */
  $handler->display->display_options['arguments']['limit']['id'] = 'limit';
  $handler->display->display_options['arguments']['limit']['table'] = 'views';
  $handler->display->display_options['arguments']['limit']['field'] = 'limit';
  $handler->display->display_options['arguments']['limit']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['limit']['summary']['format'] = 'default_summary';
  /* Contextual filter: Content: Section (field_op_section_term) */
  $handler->display->display_options['arguments']['field_op_section_term_tid']['id'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['table'] = 'field_data_field_op_section_term';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['field'] = 'field_op_section_term_tid';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_type'] = 'field';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_options']['field'] = 'content_taxonomy::field_op_blogpost_blog';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_options']['settings'] = array(
    'content_taxonomy_format' => 'tid',
  );
  $handler->display->display_options['arguments']['field_op_section_term_tid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_op_section_term_tid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_op_section_term_tid']['not'] = 0;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_blog_post' => 'openpublish_blog_post',
  );
  /* Filter criterion: Taxonomy Filter: Node Taxonomy Filter */
  $handler->display->display_options['filters']['nid']['id'] = 'nid';
  $handler->display->display_options['filters']['nid']['table'] = 'views_node_taxonomy_filter';
  $handler->display->display_options['filters']['nid']['field'] = 'nid';
  $handler->display->display_options['filters']['nid']['value'] = array(
    0 => array(
      'blog' => 'blog',
    ),
    1 => 0,
  );
  $export['op_recent_content'] = $view;

  return $export;
}
