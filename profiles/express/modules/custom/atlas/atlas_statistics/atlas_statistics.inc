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

  $statistics_data['name'] = variable_get('site_name', NULL);
  $statistics_data['status'] = variable_get('atlas_status', NULL);
  $statistics_data['site'] = variable_get('atlas_id', NULL);

  // Count Nodes.
  $node_query = db_select('node', 'n');
  $node_query->addExpression('COUNT(type)', 'count_type');
  $node_query->addExpression('MAX(changed)', 'max_changed');
  $node_query->orderBy('max_changed', 'DESC');
  $node_result = $node_query->execute();
  $all_nodes = $node_result->fetchAll();

  // Count by type.
  $node_type_counts = array();
  foreach ($all_nodes as $object) {
    $node_type_counts[] = $object->count_type;
  }
  // Total up the number of nodes.
  $number_nodes = array_sum($node_type_counts);
  if (empty($number_nodes)) {
    $number_nodes = 0;
  }
  $statistics_data['nodes_total'] = $number_nodes;

  // Days since last edit.
  $most_recent_edit = $all_nodes[0]->max_changed;
  $days_since_last_edit = ((time() - $most_recent_edit) / 86400);
  $statistics_data['days_since_last_edit'] = floor($days_since_last_edit);

  // List users.
  $users = entity_load('user');
  $user_roles = [
    'access_manager',
    'campaign_manager',
    'configuration_manager',
    'content_editor',
    'edit_my_content',
    'form_manager',
    'site_editor',
    'site_owner',
  ];
  foreach ($users as $user_id => $user) {
    // Make sure username and email exist before putting them in the array.
    // Removes user 0 (aka Anonymous).
    if ($user->name && $user->mail) {
      foreach ($user_roles as $role) {
        if (in_array($role, $user->roles, FALSE)) {
          $statistics_data['users']['username'][$role][] = $user->name;
          $statistics_data['users']['email_address'][$role][] = $user->mail;
          $statistics_data['users']['counts'][$role]++;
        }
      }
    }
  }

  // Add report of overridden features if the module is enabled and there are
  // overridden features.
  $overridden = _atlas_statistics_get_features_statuses();
  if (module_exists('features') && !empty($overridden)) {
    $statistics_data['overridden_features'] = $overridden;
  }

  // Returns system status boolean and resolves to TRUE or FALSE.
  module_load_include('inc', 'system', 'system.admin');
  $statistics_data['drupal_system_status'] = (system_status(TRUE)) ? TRUE : FALSE;

  // If webform is enabled then gather its statistics.
  $webform_stats = _atlas_statistics_get_webform_statistics();
  if (module_exists('webform')) {
    $statistics_data['webforms'] = $webform_stats;
  }

  // Get role ids to search for in query.
  $role_ids = [];
  $user_roles = [
    'access_manager',
    'campaign_manager',
    'configuration_manager',
    'content_editor',
    'edit_my_content',
    'form_manager',
    'site_editor',
    'site_owner',
  ];
  foreach ($user_roles as $val) {
    $temp_role = user_role_load_by_name($val);
    if (isset($temp_role->rid)) {
      $role_ids[] = $temp_role->rid;
    }
  }

  // Add last login of any user.
  $last_login = db_query('SELECT MAX(login) FROM users INNER JOIN users_roles ON users.uid=users_roles.uid WHERE rid IN (:roles) AND login > 1', [':roles' => $role_ids])->fetchCol('login');
  $statistics_data['days_since_last_login'] = is_array($last_login) && is_numeric($last_login[0]) ? floor((time() - $last_login[0]) / (60*60*24)) : '-1';

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

/**
 * Get count of total submissions, active forms, and inactive forms.
 *
 * @return array
 *   An array of webform statistics.
 */
function _atlas_statistics_get_webform_statistics() {
  $webform_stats = array(
    'total_submissions' => 0,
    'active_forms' => 0,
    'inactive_forms' => 0,
  );

  // Load all webforms.
  $webforms = node_load_multiple(array(), array(
    'type' => 'webform',
  ));

  // Loop through webforms and tally the statistic totals.
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  foreach ($webforms as $webform) {
    // Tally submission count.
    $webform_stats['total_submissions'] += webform_get_submission_count($webform->nid);

    // Set active/inactive bit and tally.
    $is_active = atlas_statistics_webform_is_active($webform->nid);
    $webform_stats['active_forms'] += $is_active == TRUE ? 1 : 0;
    $webform_stats['inactive_forms'] += $is_active == FALSE ? 1 : 0;
  }

  return $webform_stats;
}

/**
 * Determines if the webform is active based upon the date of last submission.
 *
 * @param int $nid
 *   Webform node id.
 * @param string $date
 *   A human readable date. Defaults to '13 months ago'.
 *
 * @return bool
 *   TRUE if last submission was within the provided date. FALSE otherwise.
 */
function atlas_statistics_webform_is_active($nid, $date = '13 months ago') {
  $submissions = webform_get_submissions(array('nid' => $nid));
  if (empty($submissions)) {
    // If there are no submissions, then consider the webform inactive.
    return FALSE;
  }
  else {
    // Get last submission; check that it was submitted since the provided date.
    $last_submission = end($submissions);
    return $last_submission->submitted > strtotime($date);
  }
}
