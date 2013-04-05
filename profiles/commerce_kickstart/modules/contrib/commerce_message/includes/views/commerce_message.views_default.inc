<?php

/**
 * @file
 * Views for order management.
 */

/**
 * Implements hook_views_default_views().
 */
function commerce_message_views_default_views() {

  $view = new view;
  $view->name = 'commerce_message_messages';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'message';
  $view->human_name = 'Commerce Message: Order messages';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Order History';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer commerce_order entities';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all'] = 0;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'type' => 'type',
    'message_order_display_name' => 'message_order_display_name',
    'timestamp' => 'timestamp',
    'name' => 'name',
    'message_render' => 'message_render',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'type' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'message_order_display_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'timestamp' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'message_render' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['empty_table'] = 1;
  /* Footer: Message: Add new message */
  $handler->display->display_options['footer']['add']['id'] = 'add';
  $handler->display->display_options['footer']['add']['table'] = 'message';
  $handler->display->display_options['footer']['add']['field'] = 'add';
  $handler->display->display_options['footer']['add']['empty'] = TRUE;
  $handler->display->display_options['footer']['add']['valid_types'] = array(
    'commerce_order_admin_comment' => 'commerce_order_admin_comment',
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'No history has been recorded for this order.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  $handler->display->display_options['empty']['area']['tokenize'] = 0;
  /* Relationship: Message: User uid */
  $handler->display->display_options['relationships']['user']['id'] = 'user';
  $handler->display->display_options['relationships']['user']['table'] = 'message';
  $handler->display->display_options['relationships']['user']['field'] = 'user';
  $handler->display->display_options['relationships']['user']['required'] = 1;
  /* Relationship: Message: Type */
  $handler->display->display_options['relationships']['type']['id'] = 'type';
  $handler->display->display_options['relationships']['type']['table'] = 'message';
  $handler->display->display_options['relationships']['type']['field'] = 'type';
  $handler->display->display_options['relationships']['type']['required'] = 1;
  /* Field: Message: Type */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'message';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  $handler->display->display_options['fields']['type']['label'] = '';
  $handler->display->display_options['fields']['type']['exclude'] = TRUE;
  $handler->display->display_options['fields']['type']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['type']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['type']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['type']['alter']['external'] = 0;
  $handler->display->display_options['fields']['type']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['type']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['type']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['type']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['type']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['type']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['type']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['type']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['type']['alter']['html'] = 0;
  $handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['type']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['type']['hide_empty'] = 0;
  $handler->display->display_options['fields']['type']['empty_zero'] = 0;
  $handler->display->display_options['fields']['type']['hide_alter_empty'] = 1;
  /* Field: Message type: Display name */
  $handler->display->display_options['fields']['message_order_display_name']['id'] = 'message_order_display_name';
  $handler->display->display_options['fields']['message_order_display_name']['table'] = 'field_data_message_order_display_name';
  $handler->display->display_options['fields']['message_order_display_name']['field'] = 'message_order_display_name';
  $handler->display->display_options['fields']['message_order_display_name']['relationship'] = 'type';
  $handler->display->display_options['fields']['message_order_display_name']['label'] = 'Type';
  $handler->display->display_options['fields']['message_order_display_name']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['text'] = '<span class="[type]">[message_order_display_name]</span>';
  $handler->display->display_options['fields']['message_order_display_name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['message_order_display_name']['element_default_classes'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['empty_zero'] = 0;
  $handler->display->display_options['fields']['message_order_display_name']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['message_order_display_name']['field_api_classes'] = 0;
  /* Field: Message: Timestamp */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'message';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['label'] = 'Date';
  $handler->display->display_options['fields']['timestamp']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['external'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['timestamp']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['timestamp']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['html'] = 0;
  $handler->display->display_options['fields']['timestamp']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['timestamp']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['timestamp']['hide_empty'] = 0;
  $handler->display->display_options['fields']['timestamp']['empty_zero'] = 0;
  $handler->display->display_options['fields']['timestamp']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'short';
  /* Field: User: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'users';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'user';
  $handler->display->display_options['fields']['name']['label'] = 'User';
  $handler->display->display_options['fields']['name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['name']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['name']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['name']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['name']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['name']['empty_zero'] = 0;
  $handler->display->display_options['fields']['name']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['name']['link_to_user'] = 1;
  $handler->display->display_options['fields']['name']['overwrite_anonymous'] = 0;
  $handler->display->display_options['fields']['name']['format_username'] = 1;
  /* Field: Message: Render message (Get text) */
  $handler->display->display_options['fields']['message_render']['id'] = 'message_render';
  $handler->display->display_options['fields']['message_render']['table'] = 'message';
  $handler->display->display_options['fields']['message_render']['field'] = 'message_render';
  $handler->display->display_options['fields']['message_render']['label'] = 'Description';
  $handler->display->display_options['fields']['message_render']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['external'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['message_render']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['message_render']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['message_render']['alter']['html'] = 0;
  $handler->display->display_options['fields']['message_render']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['message_render']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['message_render']['hide_empty'] = 0;
  $handler->display->display_options['fields']['message_render']['empty_zero'] = 0;
  $handler->display->display_options['fields']['message_render']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['message_render']['partials'] = 0;
  $handler->display->display_options['fields']['message_render']['partials_delta'] = '0';
  /* Sort criterion: Message: Timestamp */
  $handler->display->display_options['sorts']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['table'] = 'message';
  $handler->display->display_options['sorts']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['order'] = 'DESC';
  /* Sort criterion: Message: Message ID */
  $handler->display->display_options['sorts']['mid']['id'] = 'mid';
  $handler->display->display_options['sorts']['mid']['table'] = 'message';
  $handler->display->display_options['sorts']['mid']['field'] = 'mid';
  $handler->display->display_options['sorts']['mid']['order'] = 'DESC';
  /* Contextual filter: Message: Order (message_commerce_order) */
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['id'] = 'message_commerce_order_target_id';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['table'] = 'field_data_message_commerce_order';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['field'] = 'message_commerce_order_target_id';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['message_commerce_order_target_id']['not'] = 0;
  /* Filter criterion: Message: Message category */
  $handler->display->display_options['filters']['type_category']['id'] = 'type_category';
  $handler->display->display_options['filters']['type_category']['table'] = 'message';
  $handler->display->display_options['filters']['type_category']['field'] = 'type_category';
  $handler->display->display_options['filters']['type_category']['value'] = array(
    'commerce_order_message' => 'commerce_order_message',
  );

  /* Display: Entity content */
  $handler = $view->new_display('entity_view', 'Entity content', 'order_view');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '4';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['entity_type'] = 'commerce_order';
  $handler->display->display_options['bundles'] = array(
    0 => 'commerce_order',
  );
  $handler->display->display_options['show_title'] = 1;

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all'] = 0;
  $views[$view->name] = $view;

  return $views;
}
