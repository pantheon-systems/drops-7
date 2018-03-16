<?php
/**
 * @file
 * Hooks provided by the Fieldable Panels Panes module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Respond to fieldable panels pane deletion.
 *
 * @param $panels_pane
 *   The fieldable panels pane that is being deleted.
 *
 * @ingroup fieldable_panels_pane_api_hooks
 */
function hook_fieldable_panels_pane_delete($panels_pane) {
  db_delete('mytable')
    ->condition('fpid', $panels_pane->fpid)
    ->execute();
}

/**
 * Respond to creation of a new fieldable panels pane.
 *
 * @param $panels_pane
 *   The fieldable that is being created.
 *
 * @ingroup fieldable_panels_pane_api_hooks
 */
function hook_fieldable_panels_pane_insert($panels_pane) {
  db_insert('mytable')
    ->fields(array(
      'fpid' => $panels_pane->fpid,
      'vid' => $panels_pane->vid,
    ))
    ->execute();
}

/**
 * Act on a fieldable panels pane being inserted or updated.
 *
 * @param $panels_pane
 *   The fieldable panels pane that is being inserted or updated.
 *
 * @ingroup fieldable_panels_pane_api_hooks
 */
function hook_fieldable_panels_pane_presave($panels_pane) {
  // @todo: Needs example.
}

/**
 * Respond to updates to a fieldable panels pane.
 *
 * @param $panels_pane
 *   The fieldable panels pane that is being updated.
 *
 * @ingroup fieldable_panels_pane_api_hooks
 */
function hook_fieldable_panels_pane_update($panels_pane) {
  db_update('mytable')
    ->fields(array('fpid' => $panels_pane->fpid))
    ->condition('vid', $panels_pane->vid)
    ->execute();
}

/**
 * Act on a fieldable panels pane that is being assembled before rendering.
 *
 * @param $panels_pane
 *   The fieldable panels pane that is being assembled for rendering.
 * @param $view_mode
 *   The $view_mode parameter.
 * @param $langcode
 *   The language code used for rendering.
 *
 * @see hook_entity_view()
 *
 * @ingroup fieldable_panels_pane_api_hooks
 */
function hook_fieldable_panels_pane_view($panels_pane, $view_mode, $langcode) {
  $panels_pane->content['my_additional_field'] = array(
    '#markup' => $additional_field,
    '#weight' => 10,
    '#theme' => 'mymodule_my_additional_field',
  );
}

/**
 * All the list of CTools plugin specs for FPP objects to be modified.
 *
 * @param array $types
 *   All of the CTools plugin specifications for these FPP objects.
 * @param string $bundle
 *   The FPP bundle.
 * @param array $entities
 *   All of the FPP entities for this bundle indexed by their CTools subtype,
 *   e.g. fpid:123, vid:123, uuid:123.
 */
function hook_fieldable_panels_panes_content_types_alter(&$types, $bundle, $entities) {
  foreach ($types as $name => &$type) {
    $type['icon'] = 'icon_funnyface.png';
  }
}

/**
 * Allow other modules to control access to Fieldable Panels Pane objects.
 *
 * @param string $op
 *   The operation to be performed.
 * @param $entity
 *   The fieldable panels pane that is being accessed.
 * @param $account
 *   The user account whose access should be checked.
 *
 * @return TRUE|FALSE|NULL
 *   Returns TRUE to allow access, FALSE to deny, or NULL to pass the access
 *   decision off to the next hook or the module itself.
 *
 * @ingroup fieldable_panels_pane_api_hooks
 */
function hook_fieldable_panels_panes_access($op, $entity = NULL, $account = NULL) {
  // Example implementation which restricts access to edit reusable panes.
  if ($op == 'update' && !empty($entity) && $entity->reusable && !user_access('administer fieldable panels panes')) {
    return FALSE;
  }
  return NULL;
}

/**
 * @} End of "addtogroup hooks".
 */
