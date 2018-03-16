<?php

/**
 * @file
 * Feeds integration for Double Field module.
 */

/**
 * Implements hook_feeds_processor_targets_alter().
 *
 * @see FeedsNodeProcessor::getMappingTargets()
 */
function double_field_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);
    if ($info['type'] == 'double_field') {
      foreach ($info['columns'] as $subfield => $column) {
        $targets["$name:$subfield"] = array(
          'name' => $instance['label'] . ':' . $subfield,
          'callback' => 'double_field_feeds_set_target',
          // @codingStandardsIgnoreStart
          'description' => t(ucfirst($subfield) . ' subfield of the @name field.', array('@name' => $instance['label'])),
          // @codingStandardsIgnoreEnd
          'real_target' => $name,
        );
      }
    }
  }
}

/**
 * Callback for mapping.
 */
function double_field_feeds_set_target(FeedsSource $source, $entity, $target, array $values, array $mapping) {
  $language = $mapping['language'];

  list($field_name, $subfield) = explode(':', $target . ':value');

  // Iterate over all values.
  foreach ($values as $delta => $item) {
    if (!is_array($item) && !is_object($item)) {
      $entity->{$field_name}[$language][$delta][$subfield] = $item;
    }
  }

}
