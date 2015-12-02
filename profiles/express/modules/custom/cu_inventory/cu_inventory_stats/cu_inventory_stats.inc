<?php
/**
 * Create an array of stats to send to the Inventory API as part of each sites'
 * record.
 *
 * @return array
 *   $site_data: A partial PATCH to the site document in the Inventory.
 */
function cu_inventory_stats_process_site_stats() {
  $site_data = array();

  $site_data['sid'] = variable_get('cu_sid');

  // Deal with variables first.
  $variables_to_check = array(
    'grid_size_blocks',
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
  // Removed until the inventory can accept unknowns.
  $site_data['statistics']['node_types'] = $type_counts;

  // Total up the number of nodes.
  $number_nodes = array_sum($site_data['statistics']['node_types']);
  if (empty($number_nodes)) { $number_nodes = 0; }
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
  // Removed until the inventory can accept unknowns.
  $site_data['statistics']['bean_types'] = $bean_counts;

  // Total the bean counts.
  $number_beans = array_sum($site_data['statistics']['bean_types']);
  if (empty($number_beans)) { $number_beans = 0; }
  $site_data['statistics']['num_beans'] = $number_beans;

  // Profile schema version
  $version = drupal_get_installed_schema_version('cu_fit');
  $site_data['statistics']['cu_fit_schema_version'] = (int)$version;

  // Module Manager
  if ($diff = cu_module_manager_is_config_ideal()) {
    $checks =  implode(", ", $diff);
    $site_data['statistics']['module_manager'] = $checks;
  } else {
	$site_data['statistics']['module_manager'] = NULL;
  }

  // Responsive Theme Check
  $is_responsive = theme_get_setting('alpha_responsive');
  if ($is_responsive == NULL || $is_responsive == 0) {
    $is_responsive = 'FALSE';
  } elseif ($is_responsive == 1) {
    $is_responsive = 'TRUE';
  }
  $site_data['statistics']['is_theme_responsive'] =  $is_responsive;

  // TODO: Add Permissions report

  // We want a list of user email addresses to use in communications. We are
  // breaking them up by role to allow for communication to a specific role only.
  $roles_to_check = array(
    'site_owner',
    'content_editor',
    'edit_my_content'
  );
  foreach ($roles_to_check as $role_name) {
    // Return the role object
    $role = user_role_load_by_name($role_name);

    $role_query = db_select('users_roles', 'ur');
    $role_query->join('users', 'u', 'ur.uid = u.uid');
    $role_query
      ->condition('ur.rid', $role->rid)
      ->fields('u', array('mail'));

    $role_result = $role_query->execute();
    $all_role_results = $role_result->fetchAll();

    // Need to empty the string during each loop.
    $email_list = NULL;
    foreach ($all_role_results as $object) {
      if ($email_list) {
        $email_list = $email_list . '; ' . $object->mail;
      }
      else {
        $email_list = $object->mail;
      }
    }
    $site_data['statistics']['users'][$role_name] = $email_list;
  }


  // Ready to patch.
  cu_inventory_update_site($site_data);
}
