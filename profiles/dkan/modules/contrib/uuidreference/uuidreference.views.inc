<?php

/**
 * @file
 * Views integration for UUID Reference.
 */

/**
 * Implements hook_field_views_data().
 */
function uuidreference_field_views_data($field) {
  $data = field_views_field_default_views_data($field);
  $entity_info = entity_get_info($field['settings']['target_type']);

  // Only support this field relationship with entitites that support 'uuid' =>
  // TRUE, and a uuid entity key in their entity info.
  if (!empty($entity_info['uuid'])) {
    foreach ($data as $table_name => $table_data) {
      if (isset($entity_info['base table'])) {
        $entity = $entity_info['label'];
        if ($entity == t('Node')) {
          $entity = t('Content');
        }

        $field_name = $field['field_name'] . '_target_uuid';
        $parameters = array('@entity' => $entity, '!field_name' => $field['field_name']);
        $data[$table_name][$field_name]['relationship'] = array(
          'handler' => 'views_handler_relationship',
          'base' => $entity_info['base table'],
          'base field' => $entity_info['entity keys']['uuid'],
          'label' => t('@entity UUID referenced from !field_name', $parameters),
          'group' => t('UUID Reference'),
          'title' => t('Referenced Entity'),
          'help' => t('A bridge to the @entity entity that is referenced via !field_name', $parameters),
        );
      }
    }
  }

  return $data;
}
