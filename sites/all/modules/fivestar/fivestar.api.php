<?php

/**
 * @file
 * Provides API documentation for the fivestar module.
 */


/**
 * Implementation of hook_fivestar_widgets().
 *
 * This hook allows other modules to create additional custom widgets for
 * the fivestar module.
 *
 * @return array
 *   An array of key => value pairs suitable for inclusion as the #options in a
 *   select or radios form element. Each key must be the location of a css
 *   file for a fivestar widget. Each value should be the name of the widget.
 *
 * @see fivestar_fivestar_widgets()
 */
function hook_fivestar_widgets() {
  // Letting fivestar know about my Cool and Awesome Stars.
  $widgets = array(
    'path/to/my/awesome/fivestar/css.css' => 'Awesome Stars',
    'path/to/my/cool/fivestar/css.css' => 'Cool Stars',
  );

  return $widgets;
}

/**
 * Implementation of hook_fivestar_access().
 *
 * This hook is called before every vote is cast through Fivestar. It allows
 * modules to allow or deny voting on any type of entity, such as nodes, users, or
 * comments.
 *
 * @param $entity_type
 *   Type entity.
 * @param $id
 *   Identifier within the type.
 * @param $tag
 *   The VotingAPI tag string.
 * @param $uid
 *   The user ID trying to cast the vote.
 *
 * @return boolean or NULL
 *   Returns TRUE if voting is supported on this object.
 *   Returns NULL if voting is not supported on this object by this module.
 *   If needing to absolutely deny all voting on this object, regardless
 *   of permissions defined in other modules, return FALSE. Note if all
 *   modules return NULL, stating no preference, then access will be denied.
 *
 * @see fivestar_validate_target()
 * @see fivestar_fivestar_access()
 */
function hook_fivestar_access($entity_type, $id, $tag, $uid) {
  if ($uid == 1) {
    // We are never going to allow the admin user case a fivestar vote.
    return FALSE;
  }
}

/**
 * Implementation of hook_fivestar_target_info().
 *
 * @param $field
 *   The field structure for the operation.
 * @param $instance
 *   The instance structures for the $field.
 *
 * @return array
 *   An array of key => value pairs. Each key must be unique the identifier for this
 *   target selection. The Value is an array of key => value pairs for a title and a
 *   callback function. The title value is used for displaying in the #options array
 *   of the target selection option. The callback function is used when trying to decided
 *   which target the current vote should be cast against.
 *
 * @see fivestar_get_targets()
 * @see fivestar_fivestar_target_info()
 */
function hook_fivestar_target_info($field, $instance) {
  $entity_type = $instance['entity_type'];
  $bundle = $instance['bundle'];

  $options = array(
    // Declase a new Target Type.
    // This will allow users to vote on a Node and have the vote cast against the
    // node's author instead of the actual node.
    'example_node_author' => array(
      'title' => t('Node Author'),
      'callback' => '_example_target_node_author'
    ),
  );

  return $options;
}

/**
 * Define a custom voting behavior for this target selection type.
 *
 * Invoked from fivestar_get_targets().
 *
 * @param $entity
 *   The entity for the operation.
 * @param $field
 *   The field structure for the operation.
 * @param $instance
 *   The instance structure for $field on $entity's bundle.
 * @param $langcode
 *   The language associated with $items.
 *
 * @return array
 *   An array of key => value pairs. The return array must contain an entity_id key
 *   and a entity_type key. The value os the entity_id and entity_type is what the
 *   fivestar vote is going to be cast against when a user has selected this option
 *   as the target selection.
 *
 * @see _fivestar_target_comment_parent_node()
 * @see _fivestar_target_node_reference()
 * @see fivestar_get_targets()
 * @see hook_fivestar_target_info()
 */
function _example_target_node_author($entity, $field, $instance, $langcode) {
  $target = array(
    'entity_id' => 2,
    'entity_type' => 'user',
  );

  return $target;
}
