<?php

/**
 * @file
 * Features file for the workbench_moderation module.
 */

/**
 * Implements COMPONENT_features_export_options().
 *
 * Inform features about the available states in the database.
 */
function workbench_moderation_states_features_export_options() {
  $states = db_select('workbench_moderation_states', 'states')
    ->fields('states', array('name', 'name'))
    ->execute()
    ->fetchAllKeyed();

  return $states;
}

/**
 * Implements COMPONENT_features_export().
 *
 * Process the features export array for states.
 */
function workbench_moderation_states_features_export($data, &$export, $module_name) {
  $export['dependencies']['workbench_moderation_states'] = 'workbench_moderation';

  foreach ($data as $component) {
    $export['features']['workbench_moderation_states'][$component] = $component;
  }

  return array();
}

/**
 * Implements COMPONENT_features_export_render().
 *
 * Render workbench moderation states as code.
 */
function workbench_moderation_states_features_export_render($module_name, $data) {
  $items = array();

  foreach ($data as $state) {
    $items[$state] = workbench_moderation_state_load($state);
  }

  $code = "  \$items = " . features_var_export($items, '  ') . ";\n";
  $code .= '  return $items;';

  return array('workbench_moderation_export_states' => $code);
}

/**
 * Implements COMPONENT_features_revert().
 */
function workbench_moderation_states_features_revert($module) {
  workbench_moderation_states_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_enable_feature().
 */
function workbench_moderation_states_features_enable_feature($module) {
  workbench_moderation_states_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_rebuild().
 *
 * Store each exported transition in the database.
 */
function workbench_moderation_states_features_rebuild($module) {
  $defaults = features_get_default('workbench_moderation_states', $module);
  foreach ($defaults as $state) {
    workbench_moderation_state_save((object) $state);
  }
  drupal_static_reset('workbench_moderation_states');
}

/**
 * Implements COMPONENT_features_export_options().
 *
 * Inform features about the available transitions in the database.
 */
function workbench_moderation_transitions_features_export_options() {
  $options = array();

  foreach (workbench_moderation_transitions() as $transition) {
    $options[$transition->from_name . ':' . $transition->to_name] = $transition->from_name . ' -> ' . $transition->to_name;
  }

  return $options;
}

/**
 * Implements COMPONENT_features_export().
 *
 * Process the features export array for transitions.
 */
function workbench_moderation_transitions_features_export($data, &$export, $module_name) {
  $export['dependencies']['workbench_moderation_transitions'] = 'workbench_moderation';

  foreach ($data as $component) {
    $export['features']['workbench_moderation_transitions'][$component] = $component;
  }

  return array();
}

/**
 * Implements COMPONENT_features_export_render().
 *
 * Render workbench moderation transitions as code.
 */
function workbench_moderation_transitions_features_export_render($module_name, $data) {
  $items = array();

  foreach ($data as $transition) {
    list($from_name, $to_name) = explode(':', $transition);
    $item = db_select('workbench_moderation_transitions', 't')
      ->fields('t', array('from_name', 'to_name', 'name'))
      ->condition('from_name', $from_name)
      ->condition('to_name', $to_name)
      ->execute()
      ->fetchObject();

    if (!empty($item)) {
      $items[$item->from_name . ':' . $item->to_name] = $item;
    }
  }

  $code = "  \$items = " . features_var_export($items, '  ') . ";\n";
  $code .= '  return $items;';

  return array('workbench_moderation_export_transitions' => $code);
}

/**
 * Implements COMPONENT_features_revert().
 */
function workbench_moderation_transitions_features_revert($module) {
  workbench_moderation_transitions_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_enable_feature().
 */
function workbench_moderation_transitions_features_enable_feature($module) {
  workbench_moderation_transitions_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_rebuild().
 *
 * Store each exported transition in the database.
 */
function workbench_moderation_transitions_features_rebuild($module) {
  $defaults = features_get_default('workbench_moderation_transitions', $module);
  foreach ($defaults as $machine_name => $transition) {
    workbench_moderation_transition_save((object) $transition);
  }
  drupal_static_reset('workbench_moderation_transitions');
}
