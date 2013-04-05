<?php

/**
 * @file
 *   Views integration for the Field Extractor module.
 */

/**
 * Implements hook_views_data_alter().
 */
function field_extractor_views_data_alter(&$data) {
  // Expose generally usable entity-related fields.
  foreach (entity_get_info() as $entity_type => $info) {
    // Prepare a list of supported reference fields.
    $found_fields = array();
    foreach (field_info_instances($entity_type) as $bundle => $instances) {
      foreach ($instances as $field_name => $instance) {
        $field = field_info_field($field_name);
        if (in_array($field['type'], array('commerce_product_reference', 'entityreference', 'taxonomy_term_reference'))) {
          $found_fields[$field_name] = field_extractor_settings($field, $instance);
        }
      }
    }

    // Now add an extractor field for each reference field.
    foreach ($found_fields as $field_name => $settings) {
      $field_label = field_extractor_field_label($field_name);
      $data['views_entity_' . $entity_type]['field_extractor_' . $field_name] = array(
        'title' => t('Field extracted from entity referenced through field @field_label', array('@field_label' => $field_label)),
        'help' => t('Allows you to display a field with values from the referenced entities.'),
        'field' => array(
          'handler' => 'field_extractor_handler_field',
          'field_name' => $field_name,
        ),
      );
    }
  }
}

/**
 * Returns the label of a certain field.
 *
 * Therefore it looks up in all bundles to find the most used instance.
 *
 * Stolen from Views (field_views_field_label()).
 */
function field_extractor_field_label($field_name) {
  $label_counter = array();
  $all_labels = array();
  // Count the amount of instances per label per field.
  $instances = field_info_instances();
  foreach ($instances as $entity_name => $entity_type) {
    foreach ($entity_type as $bundle) {
      if (isset($bundle[$field_name])) {
        $label_counter[$bundle[$field_name]['label']] = isset($label_counter[$bundle[$field_name]['label']]) ? ++$label_counter[$bundle[$field_name]['label']] : 1;
        $all_labels[$entity_name][$bundle[$field_name]['label']] = TRUE;
      }
    }
  }
  if (empty($label_counter)) {
    return array($field_name, $all_labels);
  }
  // Sort the field lables by it most used label and return the most used one.
  arsort($label_counter);
  $label_counter = array_keys($label_counter);
  return $label_counter[0];
}

