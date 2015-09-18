<?php
/**
 * @file
 * Changed behavior.
 * 
 * Store the current time in the property every time it is updated.
 */

$plugin = array(
  'label' => "Changed",
  'entity_save' => 'eck_changed_property_entity_save',
  'property_info' => 'eck_changed_property_property_info',
);

/**
 * Store the current time in the property when the entity is changing.
 */
function eck_changed_property_entity_save($property, $vars) {
  $entity = $vars['entity'];
  if (empty($entity->is_new) || empty($entity->{$property})) {
    $entity->{$property} = time();
  }
}

/**
 * Let the system know that this property is a date.
 */
function eck_changed_property_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'date';
  $vars['properties'][$property]['description'] = t("The last time this entity was updated.");
  return $vars;
}
