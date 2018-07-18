<?php

/**
 * @file
 * Plugin to provide access control based upon node lock status.
 */

$plugin = array(
  'title' => t("Node: locked"),
  'description' => t("Control access by the node's lock status."),
  'callback' => 'content_lock_node_lock_ctools_access_check',
  'summary' => 'content_lock_node_lock_ctools_access_summary',
  'required context' => new ctools_context_required(t('Node'), 'node'),
);

/**
 * Checks if the node is locked.
 */
function content_lock_node_lock_ctools_access_check($conf, $context) {
  global $user;

  if (user_is_logged_in() && !empty($context->data)) {
    $node = $context->data;
    $lock = content_lock_fetch_lock($node->nid);

    return $lock && $lock->uid != $user->uid;
  }

  return FALSE;
}

/**
 * Provides a summary description.
 */
function content_lock_node_lock_ctools_access_summary($conf, $context) {
  return t('Returns true if the node is locked.');
}
