<?php

/**
 * @file
 * Views defaults for Commerce kickstart order module.
 */

/**
 * Implements hook_views_default_views_alter().
 */
function commerce_kickstart_order_views_default_views_alter(&$views) {
  if (isset($views['commerce_cart_form'])) {
    $view = &$views['commerce_cart_form'];

    $view->name = 'commerce_cart_form';
    $view->description = 'Display a shopping cart update form.';
    $view->tag = 'commerce';
    $view->base_table = 'commerce_order';
    $view->human_name = 'Shopping cart form';
    $view->core = 0;
    $view->api_version = '3.0';
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

    /* Display: Defaults */
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->display->display_options['title'] = 'Shopping cart';
    $handler->display->display_options['access']['type'] = 'none';
    $handler->display->display_options['cache']['type'] = 'none';
    $handler->display->display_options['query']['type'] = 'views_query';
    $handler->display->display_options['query']['options']['query_comment'] = FALSE;
    $handler->display->display_options['exposed_form']['type'] = 'basic';
    $handler->display->display_options['pager']['type'] = 'none';
    $handler->display->display_options['style_plugin'] = 'table';
    $handler->display->display_options['style_options']['columns'] = array(
      'commerce_display_path' => 'commerce_display_path',
      'line_item_title' => 'line_item_title',
      'commerce_unit_price' => 'commerce_unit_price',
      'edit_quantity' => 'edit_quantity',
      'edit_delete' => 'edit_delete',
      'commerce_total' => 'commerce_total',
    );
    $handler->display->display_options['style_options']['default'] = '-1';
    $handler->display->display_options['style_options']['info'] = array(
      'commerce_display_path' => array(
        'sortable' => 0,
        'default_sort_order' => 'asc',
        'align' => '',
        'separator' => '',
      ),
      'line_item_title' => array(
        'align' => '',
        'separator' => '',
      ),
      'commerce_unit_price' => array(
        'sortable' => 0,
        'default_sort_order' => 'asc',
        'align' => '',
        'separator' => '',
      ),
      'edit_quantity' => array(
        'align' => '',
        'separator' => '',
      ),
      'edit_delete' => array(
        'align' => '',
        'separator' => '',
      ),
      'commerce_total' => array(
        'sortable' => 0,
        'default_sort_order' => 'asc',
        'align' => '',
        'separator' => '',
      ),
    );
    $handler->display->display_options['style_options']['override'] = 1;
    $handler->display->display_options['style_options']['sticky'] = 0;
    /* Footer: Commerce Order: Order total */
    $handler->display->display_options['footer']['order_total']['id'] = 'order_total';
    $handler->display->display_options['footer']['order_total']['table'] = 'commerce_order';
    $handler->display->display_options['footer']['order_total']['field'] = 'order_total';
    $handler->display->display_options['footer']['order_total']['empty'] = FALSE;
    /* Relationship: Commerce Order: Referenced line item */
    $handler->display->display_options['relationships']['commerce_line_items_line_item_id']['id'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['relationships']['commerce_line_items_line_item_id']['table'] = 'field_data_commerce_line_items';
    $handler->display->display_options['relationships']['commerce_line_items_line_item_id']['field'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['relationships']['commerce_line_items_line_item_id']['required'] = 1;
    /* Field: Commerce Line item: Display path */
    $handler->display->display_options['fields']['commerce_display_path']['id'] = 'commerce_display_path';
    $handler->display->display_options['fields']['commerce_display_path']['table'] = 'field_data_commerce_display_path';
    $handler->display->display_options['fields']['commerce_display_path']['field'] = 'commerce_display_path';
    $handler->display->display_options['fields']['commerce_display_path']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['commerce_display_path']['label'] = '';
    $handler->display->display_options['fields']['commerce_display_path']['exclude'] = TRUE;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['alter_text'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['external'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['alter']['html'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['element_label_colon'] = 1;
    $handler->display->display_options['fields']['commerce_display_path']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['commerce_display_path']['hide_empty'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['empty_zero'] = 0;
    $handler->display->display_options['fields']['commerce_display_path']['field_api_classes'] = 0;
    /* Field: Commerce Line Item: Title */
    $handler->display->display_options['fields']['line_item_title']['id'] = 'line_item_title';
    $handler->display->display_options['fields']['line_item_title']['table'] = 'commerce_line_item';
    $handler->display->display_options['fields']['line_item_title']['field'] = 'line_item_title';
    $handler->display->display_options['fields']['line_item_title']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['line_item_title']['label'] = '';
    $handler->display->display_options['fields']['line_item_title']['exclude'] = TRUE;
    $handler->display->display_options['fields']['line_item_title']['alter']['alter_text'] = 1;
    $handler->display->display_options['fields']['line_item_title']['alter']['text'] = 'View product';
    $handler->display->display_options['fields']['line_item_title']['alter']['make_link'] = 1;
    $handler->display->display_options['fields']['line_item_title']['alter']['path'] = '[commerce_display_path]';
    $handler->display->display_options['fields']['line_item_title']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['external'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['replace_spaces'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['trim_whitespace'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['line_item_title']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['line_item_title']['alter']['more_link'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['line_item_title']['alter']['html'] = 0;
    $handler->display->display_options['fields']['line_item_title']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['line_item_title']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['line_item_title']['hide_empty'] = 0;
    $handler->display->display_options['fields']['line_item_title']['empty_zero'] = 0;
    $handler->display->display_options['fields']['line_item_title']['hide_alter_empty'] = 1;
    /* Field: Commerce Line item: Product */
    $handler->display->display_options['fields']['commerce_product']['id'] = 'commerce_product';
    $handler->display->display_options['fields']['commerce_product']['table'] = 'field_data_commerce_product';
    $handler->display->display_options['fields']['commerce_product']['field'] = 'commerce_product';
    $handler->display->display_options['fields']['commerce_product']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['commerce_product']['label'] = '';
    $handler->display->display_options['fields']['commerce_product']['exclude'] = TRUE;
    $handler->display->display_options['fields']['commerce_product']['alter']['alter_text'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['external'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['replace_spaces'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['trim_whitespace'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['commerce_product']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['commerce_product']['alter']['more_link'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['commerce_product']['alter']['html'] = 0;
    $handler->display->display_options['fields']['commerce_product']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['commerce_product']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['commerce_product']['hide_empty'] = 0;
    $handler->display->display_options['fields']['commerce_product']['empty_zero'] = 0;
    $handler->display->display_options['fields']['commerce_product']['hide_alter_empty'] = 1;
    $handler->display->display_options['fields']['commerce_product']['type'] = 'commerce_product_reference_rendered_product';
    $handler->display->display_options['fields']['commerce_product']['settings'] = array(
      'view_mode' => 'product_in_cart',
      'page' => 1,
    );
    $handler->display->display_options['fields']['commerce_product']['field_api_classes'] = 0;
    /* Field: Global: Custom text */
    $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
    $handler->display->display_options['fields']['nothing']['table'] = 'views';
    $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
    $handler->display->display_options['fields']['nothing']['label'] = '';
    $handler->display->display_options['fields']['nothing']['alter']['text'] = '[commerce_product]
    [line_item_title]';
    $handler->display->display_options['fields']['nothing']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['external'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['replace_spaces'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['trim_whitespace'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['nothing']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['nothing']['alter']['more_link'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['nothing']['alter']['html'] = 0;
    $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['nothing']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['nothing']['hide_empty'] = 0;
    $handler->display->display_options['fields']['nothing']['empty_zero'] = 0;
    $handler->display->display_options['fields']['nothing']['hide_alter_empty'] = 0;
    /* Field: Commerce Line item: Unit price */
    $handler->display->display_options['fields']['commerce_unit_price']['id'] = 'commerce_unit_price';
    $handler->display->display_options['fields']['commerce_unit_price']['table'] = 'field_data_commerce_unit_price';
    $handler->display->display_options['fields']['commerce_unit_price']['field'] = 'commerce_unit_price';
    $handler->display->display_options['fields']['commerce_unit_price']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['commerce_unit_price']['label'] = 'Price';
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['alter_text'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['external'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['alter']['html'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['element_class'] = 'price';
    $handler->display->display_options['fields']['commerce_unit_price']['element_label_colon'] = 1;
    $handler->display->display_options['fields']['commerce_unit_price']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['commerce_unit_price']['hide_empty'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['empty_zero'] = 0;
    $handler->display->display_options['fields']['commerce_unit_price']['click_sort_column'] = 'amount';
    $handler->display->display_options['fields']['commerce_unit_price']['type'] = 'commerce_price_formatted_amount';
    $handler->display->display_options['fields']['commerce_unit_price']['field_api_classes'] = 0;
    /* Field: Commerce Line Item: Quantity text field */
    $handler->display->display_options['fields']['edit_quantity']['id'] = 'edit_quantity';
    $handler->display->display_options['fields']['edit_quantity']['table'] = 'commerce_line_item';
    $handler->display->display_options['fields']['edit_quantity']['field'] = 'edit_quantity';
    $handler->display->display_options['fields']['edit_quantity']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['edit_quantity']['label'] = 'Quantity';
    $handler->display->display_options['fields']['edit_quantity']['alter']['alter_text'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['external'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['word_boundary'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['ellipsis'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['alter']['html'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['element_label_colon'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['edit_quantity']['hide_empty'] = 0;
    $handler->display->display_options['fields']['edit_quantity']['empty_zero'] = 0;
    /* Field: Commerce Line Item: Delete button */
    $handler->display->display_options['fields']['edit_delete']['id'] = 'edit_delete';
    $handler->display->display_options['fields']['edit_delete']['table'] = 'commerce_line_item';
    $handler->display->display_options['fields']['edit_delete']['field'] = 'edit_delete';
    $handler->display->display_options['fields']['edit_delete']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['edit_delete']['label'] = 'Remove';
    $handler->display->display_options['fields']['edit_delete']['alter']['alter_text'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['external'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['edit_delete']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['edit_delete']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['edit_delete']['alter']['html'] = 0;
    $handler->display->display_options['fields']['edit_delete']['element_label_colon'] = 1;
    $handler->display->display_options['fields']['edit_delete']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['edit_delete']['hide_empty'] = 0;
    $handler->display->display_options['fields']['edit_delete']['empty_zero'] = 0;
    /* Field: Commerce Line item: Total */
    $handler->display->display_options['fields']['commerce_total']['id'] = 'commerce_total';
    $handler->display->display_options['fields']['commerce_total']['table'] = 'field_data_commerce_total';
    $handler->display->display_options['fields']['commerce_total']['field'] = 'commerce_total';
    $handler->display->display_options['fields']['commerce_total']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['fields']['commerce_total']['alter']['alter_text'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['make_link'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['absolute'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['external'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['nl2br'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['word_boundary'] = 1;
    $handler->display->display_options['fields']['commerce_total']['alter']['ellipsis'] = 1;
    $handler->display->display_options['fields']['commerce_total']['alter']['strip_tags'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['trim'] = 0;
    $handler->display->display_options['fields']['commerce_total']['alter']['html'] = 0;
    $handler->display->display_options['fields']['commerce_total']['element_class'] = 'price';
    $handler->display->display_options['fields']['commerce_total']['element_label_colon'] = 1;
    $handler->display->display_options['fields']['commerce_total']['element_default_classes'] = 1;
    $handler->display->display_options['fields']['commerce_total']['hide_empty'] = 0;
    $handler->display->display_options['fields']['commerce_total']['empty_zero'] = 0;
    $handler->display->display_options['fields']['commerce_total']['click_sort_column'] = 'amount';
    $handler->display->display_options['fields']['commerce_total']['type'] = 'commerce_price_formatted_amount';
    $handler->display->display_options['fields']['commerce_total']['field_api_classes'] = 0;
    /* Sort criterion: Commerce Line Item: Line item ID */
    $handler->display->display_options['sorts']['line_item_id']['id'] = 'line_item_id';
    $handler->display->display_options['sorts']['line_item_id']['table'] = 'commerce_line_item';
    $handler->display->display_options['sorts']['line_item_id']['field'] = 'line_item_id';
    $handler->display->display_options['sorts']['line_item_id']['relationship'] = 'commerce_line_items_line_item_id';
    /* Contextual filter: Commerce Order: Order ID */
    $handler->display->display_options['arguments']['order_id']['id'] = 'order_id';
    $handler->display->display_options['arguments']['order_id']['table'] = 'commerce_order';
    $handler->display->display_options['arguments']['order_id']['field'] = 'order_id';
    $handler->display->display_options['arguments']['order_id']['default_action'] = 'empty';
    $handler->display->display_options['arguments']['order_id']['default_argument_type'] = 'fixed';
    $handler->display->display_options['arguments']['order_id']['summary']['number_of_records'] = '0';
    $handler->display->display_options['arguments']['order_id']['summary']['format'] = 'default_summary';
    $handler->display->display_options['arguments']['order_id']['summary_options']['items_per_page'] = '25';
    $handler->display->display_options['arguments']['order_id']['break_phrase'] = 0;
    $handler->display->display_options['arguments']['order_id']['not'] = 0;
    /* Filter criterion: Commerce Line Item: Line item is of a product line item type */
    $handler->display->display_options['filters']['product_line_item_type']['id'] = 'product_line_item_type';
    $handler->display->display_options['filters']['product_line_item_type']['table'] = 'commerce_line_item';
    $handler->display->display_options['filters']['product_line_item_type']['field'] = 'product_line_item_type';
    $handler->display->display_options['filters']['product_line_item_type']['relationship'] = 'commerce_line_items_line_item_id';
    $handler->display->display_options['filters']['product_line_item_type']['value'] = '1';
    $handler->display->display_options['filters']['product_line_item_type']['group'] = 0;
  }
}
