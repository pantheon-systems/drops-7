<?php

/**
 * @file
 *   Provide default Views.
 */

/**
 * Implementation of hook_views_default_views()
 */
function commerce_node_checkout_expire_views_default_views() {
  $views = array();

  $view = new view();
  $view->name = 'commerce_node_checkout_expire_posts';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Commerce Node Checkout Expire Posts';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */
  
  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'My ordered nodes';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['group_by'] = TRUE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Filter';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'commerce_node_checkout_expires' => 'commerce_node_checkout_expires',
    'node_relist_form' => 'node_relist_form',
  );
  $handler->display->display_options['style_options']['default'] = 'commerce_node_checkout_expires';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'commerce_node_checkout_expires' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'node_relist_form' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Entity Reference: Referencing entity */
  $handler->display->display_options['relationships']['reverse_commerce_node_checkout_node_commerce_line_item']['id'] = 'reverse_commerce_node_checkout_node_commerce_line_item';
  $handler->display->display_options['relationships']['reverse_commerce_node_checkout_node_commerce_line_item']['table'] = 'node';
  $handler->display->display_options['relationships']['reverse_commerce_node_checkout_node_commerce_line_item']['field'] = 'reverse_commerce_node_checkout_node_commerce_line_item';
  $handler->display->display_options['relationships']['reverse_commerce_node_checkout_node_commerce_line_item']['required'] = TRUE;
  /* Relationship: Content: Author */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'node';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  $handler->display->display_options['relationships']['uid']['required'] = TRUE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: MAX(Commerce Line item: Unpublish on) */
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['id'] = 'commerce_node_checkout_expires';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['table'] = 'field_data_commerce_node_checkout_expires';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['field'] = 'commerce_node_checkout_expires';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['relationship'] = 'reverse_commerce_node_checkout_node_commerce_line_item';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['group_type'] = 'max';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['label'] = 'Expires on';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['empty'] = '<em>Not yet purchased</em>';
  $handler->display->display_options['fields']['commerce_node_checkout_expires']['settings'] = array(
    'format_type' => 'long',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  /* Field: Content: Node relist form */
  $handler->display->display_options['fields']['node_relist_form']['id'] = 'node_relist_form';
  $handler->display->display_options['fields']['node_relist_form']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['node_relist_form']['field'] = 'node_relist_form';
  $handler->display->display_options['fields']['node_relist_form']['label'] = 'Relist';
  $handler->display->display_options['fields']['node_relist_form']['element_label_colon'] = FALSE;
  /* Filter criterion: User: Current */
  $handler->display->display_options['filters']['uid_current']['id'] = 'uid_current';
  $handler->display->display_options['filters']['uid_current']['table'] = 'users';
  $handler->display->display_options['filters']['uid_current']['field'] = 'uid_current';
  $handler->display->display_options['filters']['uid_current']['relationship'] = 'uid';
  $handler->display->display_options['filters']['uid_current']['value'] = '1';
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
  );
  
  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'posts';
  
  $views['commerce_node_checkout_expire_posts'] = $view;

  return $views;
}
