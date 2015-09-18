<?php
/**
 * @file
 * Author Behavior.
 * 
 * Save the user id of the user when the entity is created. 
 */

$plugin = array(
  'label' => "Author",
  'entity_save' => 'eck_author_property_entity_save',
  'property_info' => 'eck_author_property_property_info',
);

/**
 * When the entity is first saved, store the current user id as the author.
 */
function eck_author_property_entity_save($property, $vars) {
  $entity = $vars['entity'];
  if (isset($entity->is_new) && $entity->is_new && empty($entity->{$property})) {
    global $user;
    $entity->{$property} = $user->uid;
  }
}

/**
 * Let the system know that this property holds a user id.
 */
function eck_author_property_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'user';
  return $vars;
}
