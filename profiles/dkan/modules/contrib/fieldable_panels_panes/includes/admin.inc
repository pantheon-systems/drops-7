<?php

/**
 * @file
 * Contains administrative pages and functions for fieldable entity panes.
 */

/**
 * List all entities for the given type.
 */
function fieldable_panels_panes_entities_list_page($type) {
  return views_embed_view('fieldable_pane_entities', 'default', $type);
}

/**
 * Page callback to add a new pane entity.
 */
function fieldable_panels_panes_entities_add_page($type) {
  $form_state = array(
    'entity' => fieldable_panels_panes_create(array('bundle' => $type)),
    'add submit' => TRUE,
  );

  // Default these to reusable.
  $form_state['entity']->reusable = TRUE;
  return drupal_build_form('fieldable_panels_panes_entity_edit_form', $form_state);
}

/**
 * Page callback to view a entity.
 *
 * This represents an administrative view only. It is not available to
 * the general public. These entities are meant to be viewed as panel
 * panes (or blocks).
 */
function fieldable_panels_panes_entity_view_page($entity) {
  return fieldable_panels_panes_view($entity, 'preview');
}

/**
 * Page callback to view a entity.
 */
function fieldable_panels_panes_entity_edit_page($entity) {
  $form_state = array(
    'entity' => $entity,
    'add submit' => TRUE,
  );

  return drupal_build_form('fieldable_panels_panes_entity_edit_form', $form_state);
}

/**
 * Menu callback -- ask for confirmation of node deletion
 */
function fieldable_panels_panes_entity_delete_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $entity->title)),
    'admin/structure/fieldable-panels-panes/view/' . $entity->fpid,
    t('This action cannot be undone. Note that deleting this entity will not delete panes using it, they will exist but display nothing.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function fieldable_panels_panes_entity_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $entity = $form_state['entity'];
    fieldable_panels_panes_delete($entity->fpid);
    watchdog('fieldable_panels_pane', '@type: deleted %title.', array('@type' => $entity->bundle, '%title' => $entity->title));
    drupal_set_message(t('@type %title has been deleted.', array('@type' => fieldable_panels_panes_get_bundle_label($entity->bundle), '%title' => $entity->title)));
  }

  $form_state['redirect'] = '<front>';
}

/**
 * Menu callback -- ask for confirmation of revision deletion
 */
function fieldable_panels_panes_entity_delete_revision_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;
  return confirm_form($form,
    t('Are you sure you want to delete the revision from %revision-date?', array('%revision-date' => format_date($entity->timestamp))),
    'admin/structure/fieldable-panels-panes/view/' . $entity->fpid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute revision deletion
 */
function fieldable_panels_panes_entity_delete_revision_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $entity = $form_state['entity'];
    fieldable_panels_panes_delete_revision($entity->fpid, $entity->vid);
    watchdog('fieldable_panels_pane', '@type: deleted revision of %title from %date.', array(
      '@type' => $entity->bundle,
      '%title' => $entity->title,
      '%date' => format_date($entity->timestamp),
    ));
    drupal_set_message(t('The revision from %date has been deleted.', array('%date' => format_date($entity->timestamp))));
  }

  $form_state['redirect'] = 'admin/structure/fieldable-panels-panes/view/' . $entity->fpid . '/revision';
}

/**
 * Page callback to edit access control of an entity pane.
 *
 * @param $op
 *   Either 'edit' or 'view' to determine which type of access.
 * @param $entity
 *   The entity to control access to.
 */
function fieldable_panels_panes_entity_edit_access_page($op, $entity) {
  ctools_include('context-access-admin');
  // @todo -- add view_access field.

  $argument = $op . ':' . $entity->fpid;

  ctools_include('object-cache');

  // Ensure that if they visit this page fresh, any cached data is removed:
  if (empty($_POST)) {
    ctools_object_cache_clear('fieldable_panels_panes', $argument);
  }
  else {
    $access = ctools_object_cache_get('fieldable_panels_panes', $argument);
  }

  if (empty($access)) {
    $access = $entity->{$op . '_access'};
  }

  $form_state = array(
    'access' => $access,
    'module' => 'fieldable_panels_panes',
    'callback argument' => $argument,
    // A bug in context-access-admin requires this until it's fixed.
    'argument' => $argument,
    'contexts' => fieldable_panels_panes_get_base_context($entity),
    'no_redirect' => TRUE,
  );

  $output = drupal_build_form('ctools_access_admin_form', $form_state);

  if (!empty($form_state['executed'])) {
    ctools_object_cache_clear('fieldable_panels_panes', $argument);
    $entity->{$op . '_access'} = $form_state['access'];

    fieldable_panels_panes_save($entity);
    drupal_set_message(t('The access control has been updated.'));
    drupal_goto($_GET['q']);
  }

  return $output;
}

/**
 * List all entities for the given type.
 */
function fieldable_panels_panes_entity_list_revisions_page($entity) {
  return views_embed_view('fieldable_pane_entity_revisions', 'default', $entity->fpid);
}

/**
 * Callback to make a revision current.
 *
 * This is an operation only and has no output; it is assumed a destination
 * will be provided, though it will go ahead and redirect you to the view of
 * the entity.
 *
 * It is token protected.
 */
function fieldable_panels_panes_entity_make_current_page($entity) {
  if (!isset($_GET['token']) || !drupal_valid_token($_GET['token'], $entity->fpid . ':' . $entity->vid)) {
    return MENU_ACCESS_DENIED;
  }

  // Since this was loaded with a different revision, a simple save will make
  // that current without creating a new revision or disturbing the 'current'
  // revision.
  fieldable_panels_panes_save($entity);

  drupal_set_message(t('The revision has been made current.'));
  drupal_goto("admin/structure/fieldable-panels-panes/view/$entity->fpid");
}
