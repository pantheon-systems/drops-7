<?php
/**
 * @file
 * Commerce Search API callbacks.
 */

/**
 * Generate a SearchApiIndex with common defaults configuration.
 *
 * @param SearchApiServer $server
 *   The server the generated index will resides on.
 *
 * @param $index_machine_name
 *   The machine name of the generated index.
 *
 * @return SearchApiIndex
 *   The generated SearchApiIndex.
 */
function commerce_search_api_generate_product_display_index(SearchApiServer $server, $index_machine_name) {
  $product_reference_fields = commerce_info_fields('commerce_product_reference', 'node');
  // We only generate an index if there are product display types.
  if (empty($product_reference_fields) || !isset($server->machine_name)) {
    return entity_create('search_api_index', array());
  }
  $title = 'title';
  // Check if we should index the translated title or not.
  if (module_exists('title') && $title_field = title_field_replacement_info('node', 'title')) {
    $title = $title_field['field']['field_name'];
  }
  // We prepend temp_ to the machine name to prevent Search API from caching
  // the wrong fields when calling the getFields() method.
  $values = array(
    'name' => t('Product display'),
    'machine_name' => 'temp_' . $index_machine_name,
    'server' => $server->machine_name,
    'item_type' => 'node',
    'enabled' => 1,
    'read_only' => 0,
    'options' => array(
      'index_directly' => 1,
      'cron_limit' => 50,
      'fields' => array(
        'nid' => array(
          'type' => 'integer',
        ),
        'search_api_language' => array(
          'type' => 'string',
        ),
        'type' => array(
          'type' => 'string',
        ),
        $title => array(
          'type' => 'string',
        ),
        'created' => array(
          'type' => 'date',
        ),
        'changed' => array(
          'type' => 'date',
        ),
        'status' => array(
          'type' => 'integer',
        ),
        'search_api_aggregation_1' => array(
          'type' => 'text',
        ),
      ),
      'data_alter_callbacks' => array(
        'commerce_search_api_product_display_filter' => array(
          'status' => 1,
          'weight' => -10,
        ),
        'commerce_search_api_alter_product_status' => array(
          'status' => 1,
          'weight' => -9,
        ),
        'search_api_alter_add_aggregation' => array(
          'status' => 1,
          'weight' => 0,
          'settings' => array(
            'fields' => array(
              'search_api_aggregation_1' => array(
                'name' => t('Title (Fulltext)'),
                'type' => 'fulltext',
                'fields' => array(
                  $title,
                ),
                'description' => t('A Fulltext aggregation of the title field.'),
              ),
            ),
          ),
        ),
      ),
    ),
  );
  // If a product reference field is called field_product, take it.
  if (isset($product_reference_fields['field_product'])) {
    $product_reference_field = $product_reference_fields['field_product'];
  }
  // Else, take the first one.
  else {
    $product_reference_field = reset($product_reference_fields);
  }
  $type = $product_reference_field['cardinality'] != 1 ? 'list<integer>' : 'integer';
  $values['options']['fields'] += array(
    $product_reference_field['field_name'] => array(
      'type' => $type,
      'entity_type' => 'commerce_product',
    ),
  );
  // Adding fields in the additional fields array  will alow us to browse them.
  $values['options']['additional fields'][$product_reference_field['field_name']] = $product_reference_field['field_name'];
  $values['options']['additional fields'][$product_reference_field['field_name'] . ':commerce_price'] = $product_reference_field['field_name'] . ':commerce_price';

  // Create a temporary node index that allows us to browse the fields.
  $index = entity_create('search_api_index', $values);
  $aggregation_index = 2;
  foreach ($index->getFields(FALSE) as $field_name => $field) {
    // Index term reference fields.
    if (isset($field['entity_type']) && $field['entity_type'] == 'taxonomy_term') {
      $field_info = field_info_field($field_name);
      // Check if the field belongs to a product display bundle to include it
      // in the index and create the aggregated field.
      if (!empty($field_info['bundles']['node']) && !array_intersect($field_info['bundles']['node'], $product_reference_field['bundles']['node'])) {
        continue;
      }
      $values['options']['fields'] += array(
        $field_name => array(
          'type' => $field['type'],
          'entity_type' => $field['entity_type'],
        ),
        // We're also indexing the name property of the taxonomy term.
        $field_name . ':name' => array(
          'type' => search_api_nest_type('string', $field['type']),
        ),
      );
      // Check if the fulltext search is enabled for this taxonomy field.
      if (strpos($field_name, ':') === FALSE) {
        $values['options']['data_alter_callbacks']['search_api_alter_add_aggregation']['settings']['fields'] += array(
          'search_api_aggregation_' . $aggregation_index => array(
            'name' => t('@field name (Fulltext)', array('@field' => $field['name'])),
            'type' => 'fulltext',
            'fields' => array(
              $field_name . ':name',
            ),
            'description' => t('Name property of @field as fulltext', array('@field' => $field['name'])),
          ),
        );
        $values['options']['fields']['search_api_aggregation_' . $aggregation_index] = array(
          'type' => 'text',
        );
      }
      $aggregation_index++;
    }
    else {
      // Index the price decimal property.
      if ($field_name == $product_reference_field['field_name'] . ':commerce_price:amount_decimal') {
        $values['options']['fields'] += array(
          $field_name => array(
            'type' => $field['type'],
          ),
        );
        // Ranges support, we're using the decimal amount (Useful for facets).
        if (search_api_is_list_type($field['type']) && module_exists('search_api_ranges')) {
          $prefix = str_replace(':', '_', $field_name);
          $values['options']['fields'][$prefix . '_asc'] = array(
            'type' => 'decimal',
          );
          $values['options']['fields'][$prefix . '_desc'] = array(
            'type' => 'decimal',
          );
          $values['options']['data_alter_callbacks'] += array(
            'search_api_ranges_alter' => array(
              'status' => 1,
              'weight' => 1,
              'settings' => array(
                'fields' => drupal_map_assoc(array($field_name)),
              ),
            ),
          );
        }
      }
    }
  }
  $index->options = $values['options'];
  $index->machine_name = $index_machine_name;
  return $index;
}

/**
 * Generate and enable Facetapi facets with some defaults.
 *
 * @param $index_machine_name
 *   The machine name of the index.
 *
 * @return array
 *   An array of facets exports with some default configurations.
 */
function commerce_search_api_generate_facets($index_machine_name) {
  $export = array();
  $searcher = 'search_api@' . $index_machine_name;
  $index = search_api_index_load($index_machine_name);
  $adapter = facetapi_adapter_load($searcher);
  if ((empty($adapter) || empty($index)) || (!empty($index) && !$index->server()->supportsFeature('search_api_facets'))) {
    return $export;
  }
  $product_attributes = commerce_info_fields('taxonomy_term_reference', 'commerce_product');
  $realm = facetapi_realm_load('block');
  $search_api_ranges_exists = module_exists('search_api_ranges');
  foreach (facetapi_get_facet_info($searcher) as $field_name => $facet_info) {
    $export_facet = FALSE;
    if (isset($facet_info['field type']) && $facet_info['field type'] == 'taxonomy_term') {
      $widget = 'facetapi_checkbox_links';
      $export_facet = TRUE;
    }
    if ($search_api_ranges_exists && strpos($field_name, ':amount_decimal') !== FALSE) {
      $widget = 'search_api_ranges_ui_slider';
      $export_facet = TRUE;
    }
    if ($export_facet) {
      $facet = $adapter->getFacetSettings($facet_info, $realm);
      if ($facet->export_type == EXPORT_IN_DATABASE) {
        continue;
      }
      $facet->settings['widget'] = $widget;
      $facet->settings['filters'] += array(
        'active_items' => array(
          'status' => 1,
          'weight' => -1,
        ),
      );
      // Adding extra filters to product attribute fields.
      if (!empty($product_attributes)) {
        if (strpos($field_name, ':') !== FALSE) {
          list($prefix, $suffix) = explode(':', $field_name, 2);
          if (isset($product_attributes[$suffix])) {
            $facet->settings['filters'] += array(
              'useless_searches' => array(
                'status' => 1,
                'weight' => 0,
              ),
              'hide_search_start' => array(
                'status' => 1,
                'weight' => 1,
              ),
            );
          }
        }
      }
      $facet->api_version = 1;
      $facet->disabled = FALSE;
      $facet->enabled = TRUE;
      $export[$facet->name] = $facet;
    }
  }
  return $export;
}
