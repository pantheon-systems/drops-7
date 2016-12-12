<?php

/**
 * @file
 * Functions that run on cron to create an array of statistics.
 *
 * Also defines an API for adding statistics.
 */

/**
 * Get all statistics defined by the API.
 */
function atlas_statistics_get_statistics() {
  // TODO: Support overriding statisitcs. array_unique won't work here because
  // we need to compare the keys not the values.
  $statistics = array_merge(module_invoke_all('atlas_statistics'));
  return $statistics;
}

/**
 * Implements hook_atlas_statistics().
 */
function atlas_statistics_atlas_statistics() {
  $statistics_data = array();

  // Count Nodes.
  $node_query = db_select('node', 'n');
  $node_query->addExpression('MAX(changed)', 'max_changed');
  $node_query->orderBy('max_changed', 'DESC');
  $node_result = $node_query->execute();
  $all_nodes = $node_result->fetchAll();

  $statistics_data['nodes_total'] = count($all_nodes);

  // Days since last edit.
  $most_recent_edit = $all_nodes[0]->max_changed;
  $days_since_last_edit = ((time() - $most_recent_edit) / 86400);
  $statistics_data['days_since_last_edit'] = floor($days_since_last_edit);

  // List users.
  $users = entity_load('user');
  $usernames = array();
  $useremails = array();
  foreach ($users as $user_id => $user) {
    // Make sure username and email exist before putting them in the array.
    // Removes user 0 (aka Anonymous).
    if ($user->name) {
      $usernames[] = $user->name;
    }
    if ($user->mail) {
      $useremails[] = $user->mail;
    }
  }
  $statistics_data['users']['username'] = $usernames;
  $statistics_data['users']['email_address'] = $useremails;

  // Add report of overridden features.
  if (module_exists('features')) {
    $statistics_data['overridden_features'] = _atlas_statistics_get_features_statuses();
  }

  return $statistics_data;
}

/**
 * Function that checks each feature and returns a list of overridden ones.
 *
 * @return array
 *   List of overridden features and their statuses.
 */
function _atlas_statistics_get_features_statuses() {
  module_load_include('inc', 'features', 'features.export');
  $overridden_features = array();
  $feature_statuses = array(
    1 => 'Overridden',
    2 => 'Needs Review',
    3 => 'Rebuilding',
    4 => 'Conflict',
    5 => 'Disabled',
    6 => 'Checking',
  );
  // Loop through each feature, check the status, and capture overridden ones.
  $features = features_get_features();
  foreach ($features as $feature) {
    $feature_status = features_get_storage($feature->name);
    if ($feature_status != 0) {
      $overridden_features[$feature->name] = $feature_statuses[$feature_status];
    }
  }
  return $overridden_features;
}
