<?php

/**
 * @file
 * API documentation file for Workbench Moderation.
 */

/**
 * Allows modules to alter moderation access.
 *
 * @param &$access
 *   A boolean access declaration. Passed by reference.
 * @param $op
 *   The operation being performed. May be 'view', 'update', 'delete',
 *   'view revisions' or 'moderate'.
 * @param $node
 *   The node being acted upon.
 */
function hook_workbench_moderation_access_alter(&$access, $op, $node) {
  global $user;
  // If the node is marked private, only let its owner moderate it.
  if (empty($node->private) || $op != 'moderate') {
    return;
  }
  if ($user->uid != $node->uid) {
    $access = FALSE;
  }
}

/**
 * Allows modules to alter the list of possible next states for a node.
 *
 * @param &$states
 *   An array of possible state changes, or FALSE if none were found before
 *   invoking this hook. Passed by reference.
 * @param $current_state
 *   The current moderation state.
 * @param $context
 *   An associative array containing:
 *   - 'account': The user object being checked.
 *   - 'node': The node object being acted upon.
 *
 * @see workbench_moderation_states_next()
 */
function hook_workbench_moderation_states_next_alter(&$states, $current_state, $context) {
  // Do not permit users to give final approval to their own nodes, even if
  // they would otherwise have rights to do so.
  $published = workbench_moderation_state_published();
  if (isset($states[$published]) && ($context['account']->uid == $context['node']->uid)) {
    unset($states[$published]);
  }
}

/**
 * Allows modules to respond to state transitions.
 *
 * @param $node
 *  The node that is being transitioned.
 *
 * @param $previous_state
 *  The state of the revision before the transition occurred.
 *
 * @param $new_state
 *  The new state of the revision.
 */
function hook_workbench_moderation_transition($node, $previous_state, $new_state) {
  // Your code here.
}

/**
 * Allows modules to respond when a transition is saved.
 *
 * @param object $state
 *   The state which was just saved.
 * @param int $status
 *   Either MergeQuery::STATUS_INSERT or MergeQuery::STATUS_UPDATE depending
 *   on if this INSERT'ing a new transation or UPDATE'ing an existing one.
 */
function hook_workbench_moderation_state_save($state, $status) {
  if ($status == MergeQuery::STATUS_INSERT) {
    // Add data to a custom table for each new transition.
    db_insert('mytable')
      ->fields(array(
        'state' => $state->name,
      ))
      ->execute();
  }
}

/**
 * Allows modules to respond when a transition is saved.
 *
 * @param object $transition
 *   The transition which was just saved.
 * @param int $status
 *   Either MergeQuery::STATUS_INSERT or MergeQuery::STATUS_UPDATE depending
 *   on if this INSERT'ing a new transation or UPDATE'ing an existing one.
 */
function hook_workbench_moderation_transition_save($transition, $status) {
  if ($status == MergeQuery::STATUS_INSERT) {
    // Add data to a custom table for each new transition.
    db_insert('mytable')
      ->fields(array(
        'from_state' => $transition->from_name,
        'to_state' => $transition->to_name,
      ))
      ->execute();
  }
}

/**
 * Allows modules to respond when a state is deleted.
 *
 * @param object $state
 *  The state which was just deleted.
 */
function hook_workbench_moderation_state_delete($state) {
  // Remove data from a custom table which refers to the old state.
  db_delete('mytable')
    ->condition('state', $state->name)
    ->execute();
}

/**
 * Allows modules to respond when a transition is deleted.
 *
 * @param object $transition
 *  The transition which was just deleted.
 */
function hook_workbench_moderation_transition_delete($transition) {
  // Remove data from a custom table which refers to the old state.
  db_delete('mytable')
    ->condition('from_state', $transition->from_name)
    ->condition('to_state', $transition->to_name)
    ->execute();
}
