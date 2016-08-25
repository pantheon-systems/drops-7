<?php

/**
 * Page callback for admin/config/development/module-manager/settings
 */
function profile_module_manager_admin_settings() {
  $form = array();

  $form['profile_module_manager_disable_ui_lock'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('profile_module_manager_disable_ui_lock', 0),
    '#title' => t('Disable UI Lock'),
    '#description' => t('Disables the lock on ability enable or disable modules required by install profile through the user interface.  This also impacts drush.'),
  );
  $form['profile_module_manager_disable_enabling'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('profile_module_manager_disable_enabling', 0),
    '#title' => t('Disable the ability for users to enable bundles'),
    '#description' => t('Turns off the ability for users to enable bundles on the bundles list page.'),
  );
  $form['profile_module_manager_disable_enabling_text'] = array(
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('profile_module_manager_disable_enabling_text'),
    '#title' => t('Bundles Disabled Text'),
    '#description' => t('Message that is displayed for users when they can\'t enable bundles.'),
    '#states' => array(
      'visible' => array(
        ':input[name="profile_module_manager_disable_enabling"]' => array('checked' => TRUE),
      ),
    ),
  );
  // Add explode of ignore variable.
  $ignore_modules = variable_get('profile_module_manager_ignore', array());
  $ignore_modules = implode(',', $ignore_modules);

  $form['profile_module_manager_ignore'] = array(
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => $ignore_modules,
    '#title' => t('Ignore these projects'),
    '#description' => t('Comma separated list of the machine names of the permissions to ignore like \'varnish, memcache\'.'),
  );

  $bundles = array_keys(profile_module_manager_get_bundles('all'));
  $form['profile_module_manager_bundle_ignore'] = array(
    '#title' => t('Hidden Bundles'),
    '#type' => 'checkboxes',
    '#description' => t('You can select which bundles you want to be hidden from the end user on the bundles list page. This can be useful for soft launches or other use cases where you don\'t want certain users to be able to enable the bundle.'),
    '#default_value' => variable_get('profile_module_manager_bundle_ignore'),
    '#options' => drupal_map_assoc($bundles),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save Configuration'),
    ),
  );

  return $form;
}

/**
 * Submit handler for admin settings form.
 *
 * @param $form
 * @param $form_state
 */
function profile_module_manager_admin_settings_submit($form, &$form_state) {
  // Save module ignore variable as array.
  $ignore_modules = explode(',', $form_state['values']['profile_module_manager_ignore']);
  variable_set('profile_module_manager_ignore', $ignore_modules);

  // Unset ignore modules values.
  unset($form_state['values']['profile_module_manager_ignore']);

  // Save rest of form values.
  system_settings_form_submit($form, &$form_state);
}

/**
 * Callback for admin/settings/bundles/list
 */
function profile_module_manager_bundle_list() {
  $output = array();
  $bundle_page = array();
  // Check to see if bundle enabling has been disabled.
  if (variable_get('profile_module_manager_disable_enabling', 0) == 1) {
    $output = '<p>' . variable_get('profile_module_manager_disable_enabling_text', 'The ability to enable bundles has been turned off.') . '</p>';
    return $output;
  }

  // Get all bundles
  $bundles = profile_module_manager_get_bundles('all');
  $admin_bundles = array();
  // Get hidden bundles to compare to $bundles array.
  $hidden_bundles = array();
  $hidden_bundles_list = variable_get('profile_module_manager_bundle_ignore');
  foreach ($hidden_bundles_list as $bundle) {
    if ($bundle !== 0) {
      $hidden_bundles[] = $bundle;
    }
  }
  // check for _bundle in the machine name, but also look for something in the info
  // to avoid false positives from contrib?
  foreach ($bundles as $key => $bundle) {
    if (!strpos($bundle->name, '_bundle')) {
      unset($bundles[$key]);
    }
    // Remove bundles if set to hidden in config form.
    if (in_array($bundle->name, $hidden_bundles)) {
      unset($bundles[$key]);
    }
    // Separate admin bundles from list.
    if (strpos($bundle->name, '_admin_bundle')) {
      unset($bundles[$key]);
      $admin_bundles[$key] = $bundle;
    }
  }

  // Put admin bundles last in bundle array for separating during display.
  foreach ($admin_bundles as $key => $bundle) {
    $bundles[$key] = $bundle;
  }

  // Step through bundles
  $first_admin_bundle = TRUE;
  foreach ($bundles as $key => $bundle) {
    $vars = array();
    $info_file = str_replace(".module", ".info", $bundle->filename);
    if (file_exists($info_file)) {
      $info = drupal_parse_info_file($info_file);
      $enabled = module_exists($bundle->name) ? 1 : 0;
      // Set bundle group
      $group = strpos($bundle->name, '_admin_bundle') ? 'group_admin_bundle' : 'group_bundle';

      // Bundle status
      $vars['enabled'] = $enabled;

      // Add classes for bundle wrapper
      $classes_array = array();
      $classes_array[] = 'module-bundle-wrapper';
      $classes_array[] = $enabled ? 'bundle-enabled' : 'bundle-disabled';

      // Bundle title
      $vars['title'] = $info['name'];

      // Bundle description
      $vars['description'] = $info['description'];

      // Bundle actions
      $actions = array();
      $action_vars = array();
      if ($enabled) {
        $vars['actions'] = theme('profile_module_manager_bundle_actions_enabled', array('actions' => $action_vars));
      }
      else {
        $action_vars['enable_url'] = current_path() . '/confirm/' . $bundle->name;
        if (isset($info['project_demo_url'])) {
          $action_vars['demo_url'] = $info['project_demo_url'];
        }
        $vars['actions'] = theme('profile_module_manager_bundle_actions_disabled', $action_vars);
      }
      $vars['classes'] = join(' ', $classes_array);

      // Build bundle output
      $single_bundle['#markup'] = theme('profile_module_manager_bundle', array('bundle' => $vars));
      $all_bundles[$group][] = $single_bundle;
    }
  }
  // Build bundle groups
  foreach ($all_bundles as $group_key => $group) {
    $bundle_page['bundle_groups'][]['#markup'] = theme('profile_module_manager_bundle_group', array('group' => $group_key, 'bundles' => $group));
  }
  return theme('profile_module_manager_bundle_page', array('content' => $bundle_page));
}

/**
 * Callback function for admin/settings/bundles/list/confirm/%
 */
function profile_module_manager_bundle_confirm($bundle_name) {
  $output = '';

  if (variable_get('profile_module_manager_disable_enabling', 0) == 1) {
    $output = '<p>' . variable_get('profile_module_manager_disable_enabling_text', 'The ability to enable bundles has been turned off.') . '</p>';
    return $output;
  }

  $bundle_path = drupal_get_path('module', $bundle_name);
  $bundle = drupal_parse_info_file($bundle_path . '/' . $bundle_name . '.info');

  // Only tell users about logout if bundle requires logout.
  if (isset($bundle['bundle_logout']) && $bundle['bundle_logout'] == 1) {
    // List all currently logged in users
    $interval = REQUEST_TIME - 900;
    $items = db_query('SELECT u.uid, u.name, MAX(s.timestamp) AS max_timestamp FROM {users} u INNER JOIN {sessions} s ON u.uid = s.uid WHERE s.timestamp >= :interval AND s.uid > 0 GROUP BY u.uid, u.name ORDER BY max_timestamp DESC', array(':interval' => $interval))->fetchAll();
    $users = '';
    $users_count = count($items) -1;
    foreach ($items as $key => $item) {
      if ($key == $users_count && $users_count != 0) {
        $users .= 'and ' . $item->name . ',';
      } else {
        $users .= $item->name . ', ';
      }
    }
    $output = '<p>You are about to enable the ' . $bundle['name'] . ' Bundle. <strong>NOTE: When this process starts it will logout all users, except: ' . $users . ' from editing the site until the process is complete.</strong></p>';
  } else {
    $output = '<p>You are about to enable the ' . $bundle['name'] . ' Bundle.</p>';
  }
  $output .= '<a href="' . $GLOBALS['base_url'] . '/admin/settings/bundles/list/enable/' . $bundle_name . '" title="Confirm"><div class="btn btn-primary">Confirm</div></a>';
  $output .= '<a href="' . $GLOBALS['base_url'] . '/admin/settings/bundles/list/" title="Cancel"><div class="btn btn-default">Cancel</div></a>';
  return $output;
}

/**
 * Callback function for admin/settings/bundles/list/enable/%
 */
function profile_module_manager_bundle_enable($bundle) {

  if (variable_get('profile_module_manager_disable_enabling', 0) == 1) {
    $output = '<p>' . variable_get('profile_module_manager_disable_enabling_text', 'The ability to enable bundles has been turned off.') . '</p>';
    return $output;
  }
  // Start bundle enable timer.
  variable_set('profile_module_manager_enable_timer', microtime(TRUE));

  // Make sure this is a bundle
  if (strpos($bundle, '_bundle')) {
    // look up depenencies & enable those first
    $path = drupal_get_path('module', $bundle) . '/' . $bundle . '.info';
    $info = drupal_parse_info_file($path);
    $modules_to_enable = array();
    if (isset($info['dependencies'])) {
      foreach ($info['dependencies'] as $module) {
        if(!module_exists($module)) {
          //module_enable(array($module), FALSE);
          $modules_to_enable[] = $module;
        }
      }
    }
    // enable the bundle last
    if(!module_exists($bundle)) {
      //module_enable(array($bundle), FALSE);
      $modules_to_enable[] = $bundle;
    }

    // Size of modules to enable in each batch.
    $limit = 3;
    $module_chunks = array_chunk($modules_to_enable, $limit, TRUE);

    // Chunk modules into groups.
    $operations = array();
    foreach ($module_chunks as $chunk) {
      $operations[] = array('profile_module_manager_process_batch', array($chunk));
    }

    // Set batch operation and redirect to bundles list when done.
    $batch = array(
      'title' => t('Enabling Bundle'),
      'operations' => $operations,
      'finished' => 'profile_module_manager_batch_finished',
      'init_message' => t('Initializing...'),
      'progress_message' => t('Operation @current out of @total.'),
      'error_message' => t('Bundle failed to be enabled.'),
    );
    batch_set($batch);

    // Logout all users except currently using site if lgout flag is set.
    if (isset($info['bundle_logout']) && $info['bundle_logout'] == 1) {
      // Get all logged in users based on timestamp of being logged in since 900 seconds ago.
      $interval = REQUEST_TIME - 900;
      $items = db_query('SELECT u.uid, u.name, MAX(s.timestamp) AS max_timestamp FROM {users} u INNER JOIN {sessions} s ON u.uid = s.uid WHERE s.timestamp >= :interval AND s.uid > 0 GROUP BY u.uid, u.name ORDER BY max_timestamp DESC', array(':interval' => $interval))->fetchAll();
      $uids = array();
      foreach ($items as $item) {
        $uids[] = $item->uid;
      }
      // Log out all other users.
      $deleted_users = db_delete('sessions')
        ->condition('uid', $uids, 'NOT IN')
        ->execute();
    }
    batch_process('admin/settings/bundles/list');
  }
}
