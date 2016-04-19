<?php
/**
 * @file
 * Features file for Workbench Email Module
 */

/**
 * Implements COMPONENT_features_export_options().
 *
 * Inform features about the available email transitions in the database.
 */
function workbench_email_features_export_options() {
  $options = array();

  $workbench_emails = workbench_email_get();
  foreach ($workbench_emails as $transition_label => $email_transition_set) {
    foreach ($email_transition_set as $rid => $email_transition) {
      if ($rid == WORKBENCH_EMAIL_AUTHOR) {
        $role = workbench_email_get_author_role();
      }
      else {
        $role = user_role_load($rid);
      }
      $options[$email_transition->from_name . ':' . $email_transition->to_name . '::' . str_replace('_', ' ', strtolower($role->name))] = $role->name . ': ' . $email_transition->from_name . ' to ' . $email_transition->to_name;
    }
  }

  return $options;
}

/**
 * Implements COMPONENT_features_export().
 *
 * Process the features export array for email transitions.
 */
function workbench_email_features_export($data, &$export, $module_name) {
  $export['dependencies']['workbench_email'] = 'workbench_email';
  $export['dependencies']['workbench_moderation'] = 'workbench_moderation';
  $export['dependencies']['token'] = 'token';

  foreach ($data as $component) {
    $export['features']['workbench_email'][$component] = $component;
  }

  return array();
}

/**
 * Implements COMPONENT_features_export_render().
 *
 * Render workbench email transitions as code.
 */
function workbench_email_features_export_render($module_name, $data) {
  $items = array();

  foreach ($data as $transition) {
    list($from_name, $to_name) = explode(':', $transition);
    $transition_role = explode('::', $transition);
    $role = workbench_email_get_role_by_name($transition_role[1]);
    $item = db_select('workbench_emails', 'we')
      ->fields('we', array('from_name', 'to_name', 'subject', 'message', 'author', 'automatic'))
      ->condition('from_name', $from_name)
      ->condition('to_name', $to_name)
      ->condition('rid', $role->rid)
      ->execute()
      ->fetchObject();

    if (!empty($item)) {
      $item->role = $role->name;
      $items[$item->from_name . ':' . $item->to_name . ':' . $role->name] = $item;
    }
  }

  $code = "  \$items = " . features_var_export($items, '  ') . ";\n";
  $code .= '  return $items;';

  return array('workbench_email_export' => $code);
}

/**
 * Implements COMPONENT_features_revert().
 */
function workbench_email_features_revert($module) {
  workbench_email_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_rebuild().
 *
 * Store each exported email transition in the database.
 */
function workbench_email_features_rebuild($module) {
  $defaults = features_get_default('workbench_email', $module);
  foreach ($defaults as $machine_name => $transition) {
    $role = workbench_email_get_role_by_name($transition['role']);
    $subject = array_key_exists('subject', $transition) ? $transition['subject'] : NULL;
    $message = array_key_exists('message', $transition) ? $transition['message'] : NULL;
    workbench_email_save((object) $transition, $role->rid, $subject, $message, $transition['author'], $transition['automatic']);  }
  drupal_static_reset('workbench_email');
}
