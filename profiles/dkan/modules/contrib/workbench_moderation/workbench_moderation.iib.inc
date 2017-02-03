<?php
/**
 * @file Provides workbench integration for iib.
 */

/**
 * Implements hook_iib_entity_item().
 *
 * Add the workbench moderation status to the iib.
 */
function workbench_moderation_iib_entity_item($entity, $entity_type, $view_mode) {
  if ($entity_type == 'node' && $view_mode == 'full' && !empty($entity->workbench_moderation)) {
    $state = $entity->workbench_moderation;
    $status = check_plain(workbench_moderation_state_label($state['my_revision']->state));

    $items['center'][] = array(
      '#prefix' => '<div class="workbench-status workbench-iib">',
      '#markup' => t('Current State: @state', array('@state' => $status)),
      '#suffix' => '</div>',
    );

    $node_published = FALSE;
    $revision_published = FALSE;
    $revision_current = FALSE;

    if (!empty($state['published'])) {
      $node_published = TRUE;
    }
    if ($state['my_revision']->published) {
      $revision_published = TRUE;
    }
    if ($state['my_revision']->vid == $state['current']->vid) {
      $revision_current = TRUE;
    }

    // Add a moderation form.
    if ($revision_current && !$revision_published && _workbench_moderation_access('update', $entity) &&
      $moderate_form = drupal_get_form('workbench_moderation_moderate_form', $entity, "node/{$entity->nid}/current-revision")) {

      $items['center'][] = array(
        '#prefix' => '<div class="workbench-moderate workbench-iib">',
        'label' => array(
          '#markup' => t('Moderate:'),
        ),
        'form' => $moderate_form,
        '#suffix' => '</div>',
      );

    }
    return $items;
  }
  return array();
}