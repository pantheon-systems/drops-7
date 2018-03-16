<?php 
/**
 * @file
 * Created behavior.
 * 
 * Save the current time when an entity is created.
 */

$plugin = array(
  'label' => 'Created',
  'entity_save' => 'eck_created_property_entity_insert',
  'property_info' => 'eck_created_property_property_info',
);

/**
 * Save the current time when the entity is saved.
 */
function eck_created_property_entity_insert($property, $vars) {
  $entity = $vars['entity'];
  if (isset($entity->is_new) && $entity->is_new && empty($entity->{$property})) {
    $entity->{$property} = time();
  }
}

/**
 * Let the system know this property is a date.
 */
function eck_created_property_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'date';
  $vars['properties'][$property]['description'] = t("The date the entity was created.");
  return $vars;
}
