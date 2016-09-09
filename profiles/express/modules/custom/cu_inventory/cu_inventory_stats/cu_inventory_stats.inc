<?php

/**
 * @file
 * Functions that run on cron to create an array of statistics.
 */

/**
 * Empty the 'statistics' list in the inventory.
 */
function cu_inventory_stats_empty_site_stats() {
  $site_empty_data['statistics'] = new stdClass();
  $site_empty_data['statistics'] = array($site_empty_data['statistics']);
  cu_inventory_update_site($site_empty_data, FALSE);
}

/**
 * Create an array of statistics for the Inventory as part of each site record.
 *
 * @return array
 *   $site_data: A partial PATCH to the site document in the Inventory.
 */
function cu_inventory_stats_process_site_stats($process = TRUE) {
  $site_data = array();

  $site_data['sid'] = variable_get('cu_sid');

  // Deal with variables first.
  $variables_to_check = array(
    'site_403',
    'site_404',
    'theme_default',
    'ga_account',
    'livechat_license_number',
    'cron_last',
  );
  foreach ($variables_to_check as $variable) {
    $variable_name = 'var_' . $variable;
    $site_data['statistics'][$variable_name] = variable_get($variable, '');
  }

  // Eve cannot accept null strings for things that are booleans.
  $bool_variables_to_check = array(
    'inactive_30_email',
    'inactive_55_email',
    'inactive_60_email',
  );
  foreach ($bool_variables_to_check as $variable) {
    $variable_name = 'var_' . $variable;
    $site_data['statistics'][$variable_name] = variable_get($variable, FALSE);
  }

  $node_query = db_select('node', 'n');
  $node_query->fields('n', array('type'));
  $node_query->addExpression('COUNT(type)', 'count_type');
  $node_query->addExpression('MAX(changed)', 'max_changed');
  $node_query->groupBy('n.type');
  $node_query->orderBy('max_changed', 'DESC');

  $node_result = $node_query->execute();

  // Counts for each node type.
  $all_types = $node_result->fetchAll();
  $type_counts = array();
  foreach ($all_types as $object) {
    $type_count = 'node_type_' . $object->type;
    $type_counts[$type_count] = $object->count_type;
  }
  $site_data['statistics']['node_types'] = $type_counts;

  // Total up the number of nodes.
  $number_nodes = array_sum($site_data['statistics']['node_types']);
  if (empty($number_nodes)) {
    $number_nodes = 0;
  }
  $site_data['statistics']['num_nodes'] = $number_nodes;

  // Days since last edit.
  $most_recent_edit = $all_types[0]->max_changed;
  $days_since_last_edit = ((time() - $most_recent_edit) / 86400);
  $days_since_last_edit = floor($days_since_last_edit);
  $site_data['statistics']['num_days_since_last_edit'] = $days_since_last_edit;

  // Bean counting time.
  $bean_query = db_select('bean', 'b');
  $bean_query->fields('b', array('type'));
  $bean_query->addExpression('COUNT(type)', 'count_bean');
  $bean_query->groupBy('b.type');

  $bean_result = $bean_query->execute();

  // Counts for each bean type.
  $all_beans = $bean_result->fetchAll();
  $bean_counts = array();
  foreach ($all_beans as $object) {
    $bean_count = 'bean_type_' . $object->type;
    $bean_counts[$bean_count] = $object->count_bean;
  }
  $site_data['statistics']['bean_types'] = $bean_counts;

  // Total the bean counts.
  $number_beans = array_sum($site_data['statistics']['bean_types']);
  if (empty($number_beans)) {
    $number_beans = 0;
  }
  $site_data['statistics']['num_beans'] = $number_beans;

  // Profile + Core code version.
  $site_data['statistics']['express_code_version'] = express_version_get_version();

  // Core schema version.
  $version = _get_installed_schema_version(variable_get('express_core_version', 'cu_core'));
  $site_data['statistics']['express_core_schema_version'] = (int) $version;

  // Module Manager.
  if ($diff = profile_module_manager_is_config_ideal()) {
    $checks = implode(", ", $diff);
    $site_data['statistics']['module_manager'] = $checks;
  }
  else {
    $site_data['statistics']['module_manager'] = NULL;
  }

  // Responsive Theme Check.
  $is_responsive = theme_get_setting('alpha_responsive');
  if ($is_responsive == NULL || $is_responsive == 0) {
    $is_responsive = FALSE;
  }
  elseif ($is_responsive == 1) {
    $is_responsive = TRUE;
  }
  $site_data['statistics']['is_theme_responsive'] = $is_responsive;

  // TODO: Add Permissions report.

  // Removing test user accounts from list of users.
  $test_user_emails = array();
  $test_roles = array('site_owner', 'content_editor', 'edit_my_content');
  foreach ($test_roles as $role) {
    $users = cu_users_get_users($role);
    foreach ($users as $email) {
      $test_user_emails[] = $email;
    }
  }

  // We want a list of user email addresses and a list of Identikeys to use in
  // communications. We are breaking them up by role to allow segmentation by
  // role.
  $roles_to_check = array(
    'site_owner',
    'content_editor',
    'edit_my_content'
  );

  foreach ($roles_to_check as $role_name) {
    // Return the role object.
    $role = user_role_load_by_name($role_name);

    $role_query = db_select('users_roles', 'ur');
    $role_query->join('users', 'u', 'ur.uid = u.uid');
    $role_query
      ->condition('ur.rid', $role->rid)
      ->condition('u.mail', $test_user_emails, 'NOT IN')
      ->fields('u', array('mail', 'name'));

    $role_result = $role_query->execute();
    $all_role_results = $role_result->fetchAll();

    // Need to empty the strings during each loop.
    $email_list = NULL;
    $identikey_list = NULL;
    foreach ($all_role_results as $object) {
      if ($email_list && $identikey_list) {
        $email_list = $email_list . '; ' . $object->mail;
        $identikey_list = $identikey_list . '; ' . $object->name;
      }
      else {
        $email_list = $object->mail;
        $identikey_list = $object->name;
      }
    }
    $site_data['statistics']['users']['email'][$role_name] = $email_list;
    $site_data['statistics']['users']['identikey'][$role_name] = $identikey_list;
  }

  // Add report of overridden features.
  $site_data['statistics']['overriden_features'] = _get_features_statuses();

  // Add report of stats related to bundles.
  $site_data['statistics']['bundles'] = _get_bundle_stats();

  // Last, add the wrapper array. This adds a wrapper Python list which allows
  // us to null Statistics. We keep a version without a wrapper for POSTing to
  // logstash.
  $site_data_nowrapper = $site_data;
  $site_data['statistics'] = array($site_data['statistics']);

  // Function defaults to true.
  if ($process && module_exists('cu_inventory')) {
    // Ready to patch. We do not want to use the cached site object.
    cu_inventory_update_site($site_data, FALSE);
    cu_inventory_send_to_logstash($site_data_nowrapper);
  }
  else {
    return $site_data['statistics'];
  }
}

/**
 * Send 'statistics' to logstash.
 */
function cu_inventory_send_to_logstash($site_data) {
  if (cu_inventory_connected_env() == "local") {
    $request_path = 'http://logs.local:8080';
  }
  else {
    $request_path = 'http://wlogstash.colorado.edu:8080';
  }

  $curl = curl_init();
  curl_setopt($curl, CURLOPT_URL, $request_path);
  // Use a POST rather than a GET.
  curl_setopt($curl, CURLOPT_POST, TRUE);
  // JSON encode the payload.
  $curl_payload = drupal_json_encode($site_data);
  curl_setopt($curl, CURLOPT_POSTFIELDS, $curl_payload);
  if (!$curl_response = curl_exec($curl)) {
    watchdog('express_inventory', 'Statistics POST to the inventory failed.', WATCHDOG_WARNING);
  }
  else {
    watchdog('express_inventory', 'Statistics POST to the inventory succeded.', WATCHDOG_INFO);
  }
  curl_close($curl);
}

/**
 * Helper to return the schema_version of any module, theme, or profile.
 *
 * drupal_get_installed_schema_version doesn't return profiles again
 * at this bootstrap level as of 7.41.  SEE FIT-1165.
 *
 * @param string $project_name
 *   String of the modules, theme or profile.
 *
 * Returns an int
 */
function _get_installed_schema_version($project_name = NULL) {
  return db_query("SELECT schema_version FROM {system} WHERE name = :name", array(':name' => $project_name))->fetchField();
}

/**
 * Function that checks each feature and returns a list of overridden ones.
 *
 * @return array
 *   List of overridden features and their statuses.
 */
function _get_features_statuses() {
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
 * Callback function to return statistics relating to bundles.
 *
 * @return array|string
 *   List of enabled bundles as a string or array.
 */
function _get_bundle_stats() {
  $bundle_stats = array();
  // Get enabled bundles.
  $enabled_bundles = db_query("SELECT name, schema_version FROM {system} WHERE type = 'module' AND status = 1 AND name LIKE '%_bundle' ORDER BY weight ASC, name ASC")->fetchAllAssoc('name');
  // Turn returned object into an array.
  foreach ($enabled_bundles as $key => $bundle) {
    $bundle_stats[$bundle->name] = array(
      'schema_version' => $bundle->schema_version,
    );
  }
  return $bundle_stats;
}
