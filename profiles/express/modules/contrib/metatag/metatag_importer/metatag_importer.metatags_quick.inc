<?php

/**
 * @file
 * Convert data from Metatags Quick to Metatag.
 */

/**
 * Import all data from Metatags Quick and delete it.
 */
function metatag_importer_metatags_quick_import() {
  // Get a list of Metatags Quick fields.
  $fields = metatag_importer_get_quick_fields();

  $message = 'Converting records from @count Metatags Quick field(s).';
  $vars = array('@count' => count($data));
  if (drupal_is_cli() && function_exists('drush_print')) {
    drush_print(dt($message, $vars));
  }
  else {
    drupal_set_message(t($message, $vars));
  }

  // Get all of the data for these fields.
  $data = metatag_importer_get_quick_data($fields);

  // Process the data.
  foreach ($data as $quick) {
    $message = 'Converting @count records from Metatags Quick..';
    $vars = array('@count' => count($quick));
    if (drupal_is_cli() && function_exists('drush_print')) {
      drush_print(dt($message, $vars));
    }
    else {
      drupal_set_message(t($message, $vars));
    }

    metatag_importer_metatags_quick_process($quick);
  }

  if (drupal_is_cli() && function_exists('drush_print')) {
    drush_print(dt('All done!'));
  }
  else {
    drupal_set_message(t('All done!'));
  }
}

/**
 * Process a row from Metatags Quick.
 *
 * Imports data into metatag structure, and deletes Metatags Quick data.
 *
 * @param array $quick
 *   Row of data from Metatags Quick.
 */
function metatag_importer_metatags_quick_process(array $quick) {
  // Identify which Metatag meta tags will be filled by Metatags Quick values.
  $tag_map = array(
    'title' => 'title',
    'keywords' => 'keywords',
    'abstract' => 'abstract',
    'description' => 'description',
    'canonical' => 'canonical',
  );
  if (module_exists('metatag_opengraph')) {
    $tag_map['og:title'] = 'title';
    $tag_map['og:description'] = 'description';
  }
  if (module_exists('metatag_twitter_cards')) {
    $tag_map['twitter:title'] = 'title';
    $tag_map['twitter:description'] = 'description';
  }

  $entity_type = $quick['entity_type'];
  $entity_id = $quick['entity_id'];
  $revision_id = $quick['revision_id'];
  $langcode = $quick['language'];

  // Fallback to entity language if no field language is set.
  if (LANGUAGE_NONE == $langcode) {
    $entities = entity_load($entity_type, array($entity_id));
    if (!empty($entities[$entity_id])) {
      $langcode = entity_language($entity_type, $entities[$entity_id]);
    }
  }

  // Check for an existing record.
  $data = metatag_metatags_load($entity_type, $entity_id);

  // Drop back one level because the results will be keyed by revision_id.
  if (!empty($data)) {
    $data = reset($data);
  }

  // Map the Quick meta tags.
  foreach ($tag_map as $dest => $source) {
    if (!empty($quick['fields'][$source]['value'])) {
      $data[$langcode][$dest] = array('value' => $quick['fields'][$source]['value']);
      // Add the default suffix to the page title.
      if ($dest == 'title') {
        $data[$langcode][$dest]['value'] .= ' | [site:name]';
      }
    }
  }

  // Create or update the {metatag} record.
  if (!empty($data)) {
    metatag_metatags_save($entity_type, $entity_id, $revision_id, $data);
  }

  if (!empty($quick['fields'])) {
    metatag_importer_delete_quick_data($quick['fields']);
  }

  // Reset the entity cache. If entitycache module is used, this also resets
  // its permanent cache.
  entity_get_controller($entity_type)->resetCache(array($entity_id));
}

/**
 * Get all fields from Metatags Quick.
 *
 * @return array
 *   Array of field names, keyed by name.
 */
function metatag_importer_get_quick_fields() {
  $fields = array();

  // Get a list of all entities that use a Metatags Quick field.
  foreach (field_info_instances() as $entity_type => $bundles) {
    // Skip the custon entity type provided by Metatags Quick.
    if ($entity_type == 'metatags_path_based') {
      continue;
    }
    foreach ($bundles as $bundle_name => $bundle) {
      foreach ($bundle as $field_name => $field) {
        if ($field['widget']['module'] == 'metatags_quick') {
          $fields[$field_name] = $field_name;
        }
      }
    }
  }

  return $fields;
}

/**
 * Get metatags_quick data from the database.
 *
 * @param array $fields
 *   Array of field names.
 *
 * @return array
 *   Metadata
 */
function metatag_importer_get_quick_data(array $fields) {
  $data = array();
  foreach ($fields as $field_name) {
    $meta_tag = str_replace('meta_', '', str_replace('field_', '', $field_name));
    $results = db_select('field_data_' . $field_name, 'f')
      ->fields('f', array(
        'entity_type',
        'bundle',
        'entity_id',
        'revision_id',
        'language',
        $field_name . '_metatags_quick',
      ))
      ->condition('f.entity_type', array('metatags_path_based'), '<>')
      ->orderBy('f.entity_type', 'ASC')
      ->orderBy('f.entity_id', 'ASC')
      ->orderBy('f.revision_id', 'ASC')
      ->execute();

    foreach ($results as $result) {
      $id = implode(':', array(
        $result->entity_type,
        $result->entity_id,
        $result->revision_id,
        $result->language,
      ));

      if (!isset($data[$id])) {
        $data[$id] = array(
          'entity_type' => $result->entity_type,
          'bundle' => $result->bundle,
          'entity_id' => $result->entity_id,
          'revision_id' => $result->revision_id,
          'language' => $result->language,
          'fields' => array(),
        );
      }
      $data[$id]['fields'][$meta_tag] = array(
        'field_name' => $field_name,
        'value' => $result->{$field_name . '_metatags_quick'},
        'meta_tag' => $meta_tag,
      );
    }
  }

  return $data;
}

/**
 * Delete the old metatags_quick records.
 *
 * @param array $quick
 *   Records to delete.
 */
function metatag_importer_delete_quick_data(array $quick) {
  foreach ($quick as $field) {
    db_delete('field_data_' . $field['field_name'])
      ->condition('entity_type', $quick['entity_type'])
      ->condition('entity_id', $quick['entity_id'])
      ->condition('language', $quick['language'])
      ->execute();

    db_delete('field_revision_' . $field['field_name'])
      ->condition('entity_type', $quick['entity_type'])
      ->condition('entity_id', $quick['entity_id'])
      ->condition('language', $quick['language'])
      ->execute();
  }
}
