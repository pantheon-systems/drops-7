<?php
/**
 * @file
 * commerce_kickstart_taxonomy.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function commerce_kickstart_taxonomy_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'collection_products';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'search_api_index_product_display';
  $view->human_name = 'Collection products';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = '%1';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'name',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['class'] = 'products-per-collection';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Header: Global: View area */
  $handler->display->display_options['header']['view']['id'] = 'view';
  $handler->display->display_options['header']['view']['table'] = 'views';
  $handler->display->display_options['header']['view']['field'] = 'view';
  $handler->display->display_options['header']['view']['view_to_insert'] = 'collection_products:block_1';
  $handler->display->display_options['header']['view']['inherit_arguments'] = TRUE;
  /* Relationship: Category */
  $handler->display->display_options['relationships']['field_category']['id'] = 'field_category';
  $handler->display->display_options['relationships']['field_category']['table'] = 'search_api_index_product_display';
  $handler->display->display_options['relationships']['field_category']['field'] = 'field_category';
  $handler->display->display_options['relationships']['field_category']['ui_name'] = 'Category';
  /* Field: Taxonomy term: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'entity_taxonomy_term';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'field_category';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['name']['link_to_entity'] = 0;
  /* Field: Content: Rendered Node */
  $handler->display->display_options['fields']['rendered_entity']['id'] = 'rendered_entity';
  $handler->display->display_options['fields']['rendered_entity']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['rendered_entity']['field'] = 'rendered_entity';
  $handler->display->display_options['fields']['rendered_entity']['label'] = '';
  $handler->display->display_options['fields']['rendered_entity']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['rendered_entity']['link_to_entity'] = 1;
  $handler->display->display_options['fields']['rendered_entity']['display'] = 'view';
  $handler->display->display_options['fields']['rendered_entity']['view_mode'] = 'product_list';
  /* Sort criterion: Category: Name (indexed) */
  $handler->display->display_options['sorts']['field_category_name']['id'] = 'field_category_name';
  $handler->display->display_options['sorts']['field_category_name']['table'] = 'search_api_index_product_display';
  $handler->display->display_options['sorts']['field_category_name']['field'] = 'field_category_name';
  /* Contextual filter: Indexed Node: Collection */
  $handler->display->display_options['arguments']['field_collection']['id'] = 'field_collection';
  $handler->display->display_options['arguments']['field_collection']['table'] = 'search_api_index_product_display';
  $handler->display->display_options['arguments']['field_collection']['field'] = 'field_collection';
  $handler->display->display_options['arguments']['field_collection']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['field_collection']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_collection']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_collection']['summary_options']['grouping'] = array(
    0 => array(
      'field' => 'name',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['arguments']['field_collection']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['field_collection']['validate']['type'] = 'taxonomy_term';
  $handler->display->display_options['arguments']['field_collection']['validate_options']['vocabularies'] = array(
    'collection' => 'collection',
  );
  $handler->display->display_options['arguments']['field_collection']['break_phrase'] = 0;
  /* Filter criterion: Indexed Node: Status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'search_api_index_product_display';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = array(
    1 => '1',
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = '%1';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Taxonomy term: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'entity_taxonomy_term';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['relationship'] = 'field_category';
  $handler->display->display_options['fields']['name_1']['label'] = '';
  $handler->display->display_options['fields']['name_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['name_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['name_1']['link_to_entity'] = 0;
  $handler->display->display_options['fields']['name_1']['replace_spaces'] = TRUE;
  /* Field: Taxonomy term: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'entity_taxonomy_term';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'field_category';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['text'] = '<span id="[name_1]"></span><div class="collection"><a>[name]</a></div>';
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['name']['link_to_entity'] = 0;
  /* Field: Content: Rendered Node */
  $handler->display->display_options['fields']['rendered_entity']['id'] = 'rendered_entity';
  $handler->display->display_options['fields']['rendered_entity']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['rendered_entity']['field'] = 'rendered_entity';
  $handler->display->display_options['fields']['rendered_entity']['label'] = '';
  $handler->display->display_options['fields']['rendered_entity']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['rendered_entity']['link_to_entity'] = 0;
  $handler->display->display_options['fields']['rendered_entity']['display'] = 'view';
  $handler->display->display_options['fields']['rendered_entity']['view_mode'] = 'product_list';
  $handler->display->display_options['path'] = 'collection/%';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'collection-terms';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'name_1',
      'rendered' => 0,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Taxonomy term: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'entity_taxonomy_term';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'field_category';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['name']['link_to_entity'] = 0;
  $handler->display->display_options['fields']['name']['replace_spaces'] = TRUE;
  /* Field: Taxonomy term: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'entity_taxonomy_term';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['relationship'] = 'field_category';
  $handler->display->display_options['fields']['name_1']['label'] = '';
  $handler->display->display_options['fields']['name_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['name_1']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['name_1']['alter']['text'] = '<a href="#[name]">[name_1]</a>';
  $handler->display->display_options['fields']['name_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['name_1']['link_to_entity'] = 0;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['nothing']['hide_empty'] = TRUE;
  $export['collection_products'] = $view;

  return $export;
}

/**
 * Implements hook_views_default_views_alter().
 *
 * Add specific overrides for the no demo store.
 */
function commerce_kickstart_taxonomy_views_default_views_alter(&$views) {
  $install_demo_store = variable_get('commerce_kickstart_demo_store', FALSE);
  if (empty($install_demo_store)) {
    $view = &$views['collection_products'];
    $page_handler = &$view->display['page']->handler;
    $default_handler = &$view->display['default']->handler;

    if (!is_array($page_handler->display->display_options['fields'])) {
      return;
    }
    // Unset the title.
    unset($page_handler->display->display_options['title']);
    unset($default_handler->display->display_options['sorts']);
    // Start the overrides.
    $default_handler->display->display_options['relationships']['field_product_category'] = $default_handler->display->display_options['relationships']['field_category'];
    unset($default_handler->display->display_options['relationships']['field_category']);
    $default_handler->display->display_options['relationships']['field_product_category']['id'] = 'field_product_category';
    $default_handler->display->display_options['relationships']['field_product_category']['field'] = 'field_product_category';
    $page_handler->display->display_options['fields']['name']['relationship'] = 'field_product_category';
    $page_handler->display->display_options['fields']['name']['replace_spaces'] = TRUE;
    // Replace the contextual filter.
    unset($default_handler->display->display_options['arguments']['field_collection']);
    $default_handler->display->display_options['arguments']['field_product_category']['id'] = 'field_product_category';
    $default_handler->display->display_options['arguments']['field_product_category']['field'] = 'field_product_category';
    $default_handler->display->display_options['arguments']['field_product_category']['table'] = 'search_api_index_product_display';
    $default_handler->display->display_options['arguments']['field_product_category']['default_action'] = 'not found';
    $default_handler->display->display_options['arguments']['field_product_category']['default_argument_type'] = 'fixed';
    $default_handler->display->display_options['arguments']['field_product_category']['summary']['format'] = 'default_summary';
    $default_handler->display->display_options['arguments']['field_product_category']['summary_options']['grouping'] = array(
      0 => array(
        'field' => 'name',
        'rendered' => 1,
        'rendered_strip' => 0,
      ),
    );
    $default_handler->display->display_options['arguments']['field_product_category']['specify_validation'] = TRUE;
    $default_handler->display->display_options['arguments']['field_product_category']['validate']['type'] = 'taxonomy_term';
    $default_handler->display->display_options['arguments']['field_product_category']['validate_options']['vocabularies'] = array(
      'product_category' => 'product_category',
    );
    $default_handler->display->display_options['arguments']['field_product_category']['break_phrase'] = 0;
    // Update the path.
    $page_handler->display->display_options['path'] = 'product_category/%';
  }
}
