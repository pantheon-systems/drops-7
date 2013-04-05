<?php

/**
 * Implements hook_views_data_alter().
 */
function commerce_kickstart_taxonomy_views_data_alter(&$data) {
  // Extend the default entity_views_handler_field_text for taxonomy.
  if (isset($data['entity_taxonomy_term'])) {
    foreach ($data['entity_taxonomy_term'] as $key => $field) {
      if (isset($field['field']) && $field['field']['handler'] == 'entity_views_handler_field_text') {
        $data['entity_taxonomy_term'][$key]['field']['handler'] = 'commerce_kickstart_taxonomy_handler_field_text';
      }
    }
  }
}
