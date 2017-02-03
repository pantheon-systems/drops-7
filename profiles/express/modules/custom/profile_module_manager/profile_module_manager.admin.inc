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
  $form['profile_module_manager_ignore'] = array(
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('profile_module_manager_ignore'),
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

  return system_settings_form($form);

}

/**
 * Page callback to manage modules in code.
 */
function profile_module_manager_export($form, $form_state) {

  $form = array();
  $form['help'] = array(
    '#markup' => t('The Module Manager module stores this array in the module and compares what is enabled on each site to it.'),
  );
  $output = '';

  $result = db_query("SELECT name FROM {system} WHERE status = 1 AND type = 'module'  ORDER BY name ASC");

  $output .= "  return array(\n";

  foreach ($result as $module) {
    $output .= "      '$module->name' => '$module->name',\n";
  }
  $output .= " );\n";
  $lines = explode("\n", $output);

  $form['export'] = array(
    '#title' => t('Module array output -- update in YOUR_module_manager.module'),
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => count($lines),
    '#default_value' => $output,
    '#description' => t('Module .module file.'),
  );
  return $form;
}

/**
 * Page callback to generate roles and permissions in code.
 */
function profile_module_manager_bundle_form($form, $form_state) {
  $form = array();
  $form['help'] = array(
    '#markup' => t('<p>Add-on Bundles are additional features and functionality that you can enable. Before enabling a bundle, review the examples and documentation to see how it works. Once a bundle is enabled you cannot disable it. </p>'),
    '#prefix' => '<div class="module-bundle-description">',
    '#suffix' => '</div>',
  );

  $bundles = profile_module_manager_get_bundles('all');

  // Get hidden bundles to compare to $bundles array.
  $hidden_bundles = array();
  $hidden_bundles_list = variable_get('profile_module_manager_bundle_ignore');
  foreach ($hidden_bundles_list as $bundle) {
    if ($bundle !== 0) {
      $hidden_bundles[] = $bundle;
    }
  }

  // if there are no bundles that can be enabled, it should say something?  Show as locked checkbox?

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
  }
  $columns =5;
  $bundle_chunks = array_chunk($bundles, $columns, TRUE);
  foreach ($bundle_chunks as $key => $chunk) {
    $form['bundle_group_' . $key] = array(
      '#markup' => '',
      '#prefix' => '<div class="module-bundle-group">',
      '#suffix' => '</div>',
    );
    foreach ($chunk as $bundle) {
    // is it a bundle?

      //dsm($bundle);
      $info_file = str_replace(".module", ".info", $bundle->filename);
      $info = drupal_parse_info_file($info_file);
      //dsm($info);
      $enabled = module_exists($bundle->name) ? 1 : 0;

      $form['bundle_group_' . $key][$bundle->name] = array(
        '#type' => 'checkbox',
        '#default_value' => 0,
        '#title' => $info['name'],
        '#description' => $info['description'],
        '#default_value' => $enabled,
        '#disabled' => $enabled,
      );
    }
    for ($i = count($chunk); $i < $columns; $i++) {
      $form['bundle_group_' . $key]['bundle_group_spacer_' . $i] = array(
        '#markup' => '',
        '#prefix' => '<div class="module-bundle-group-spacer">',
        '#suffix' => '</div>',
      );
    }
  }


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#prefix' => '<div class="module-bundle-submit">',
    '#suffix' => '</div>',
  );

  return $form;
}

// ADD SUBMIT HANDLER THAT ENABLES MODULE AND DEPENDENCIES

/**
 * Submit handler for invite_form.
 *
 * Add entry to database and send email.
 */
function profile_module_manager_bundle_form_submit(&$form, &$form_state) {
  //@TODO: we should check the module_exits earlier
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, '_bundle') && $value) {
	    // look up depenencies & enable those first
      $path = drupal_get_path('module', $key) . '/' . $key . '.info';
      $info = drupal_parse_info_file($path);
      if (isset($info['dependencies'])) {
        foreach ($info['dependencies'] as $module) {

          if(!module_exists($module)) {
            module_enable(array($module), FALSE);
          }
        }
      }
      if(!module_exists($key)) {
	      module_enable(array($key), FALSE);
      }
	  }
  }
}
/**
 * Callback for admin/settings/bundles/list
 */

function profile_module_manager_bundle_list() {
  $output = array();

  if (variable_get('profile_module_manager_disable_enabling', 0) == 1) {
    $output = '<p>' . variable_get('profile_module_manager_disable_enabling_text', 'The ability to enable bundles has been turned off.') . '</p>';
    return $output;
  }
  $output['help'] = array(
    '#markup' => t('<p>Add-on Bundles are additional features and functionality that you can enable. Before enabling a bundle, review the examples and documentation to see how it works. Once a bundle is enabled you cannot disable it. </p>'),
    '#prefix' => '<div class="modules-bundle-description">',
    '#suffix' => '</div>',
  );

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

  // Wrapper markup for all bundles
  $output['bundles'] = array(
    '#prefix' => '<div class="module-bundles-wrapper block-column-container row">',
    '#suffix' => '</div>',
  );
  // Step through bundles
  $first_admin_bundle = TRUE;
  foreach ($bundles as $key => $bundle) {
    $enabled = module_exists($bundle->name) ? 1 : 0;
    $enabled_class = $enabled ? 'bundle-enabled' : 'bundle-disabled';
    // Add admin bundle break if admin bundle.
    if (strpos($bundle->name, '_admin_bundle') && $first_admin_bundle == TRUE) {
      $output['bundles']['bundle_group_' . $key] = array(
        '#markup' => '',
        '#prefix' => '</div><div class="row admin-bundle-separator"><h2>Admin Bundles</h2></div><div class="module-bundles-wrapper block-column-container row"><div class="col-lg-3 col-md-4 col-sm-6 col-xs-12 module-bundle-wrapper '. $enabled_class . '"><div class="module-bundle">',
        '#suffix' => '</div></div>',
      );
      $first_admin_bundle = FALSE;
    } else {
      $output['bundles']['bundle_group_' . $key] = array(
        '#markup' => '',
        '#prefix' => '<div class="col-lg-3 col-md-4 col-sm-6 col-xs-12 module-bundle-wrapper ' . $enabled_class . '"><div class="module-bundle">',
        '#suffix' => '</div></div>',
      );
    }
    $info_file = str_replace(".module", ".info", $bundle->filename);
    $info = drupal_parse_info_file($info_file);

    // Bundle title
    $output['bundles']['bundle_group_' . $key][$bundle->name]['title']['#markup'] = '<h2>' . $info['name']. '</h2>';

    // Bundle description
    $output['bundles']['bundle_group_' . $key][$bundle->name]['description']['#markup'] = $info['description'];
    $output['bundles']['bundle_group_' . $key][$bundle->name]['description']['#prefix'] = '<div class="module-bundle-description">';
    $output['bundles']['bundle_group_' . $key][$bundle->name]['description']['#suffix'] = '</div>';

    // Bundle actions
    $actions = array();
    if ($enabled) {
      $actions[] = '<span class="btn btn-default btn-disabled active-theme">Enabled</span>';
    }
    else {
      $actions[] = l('<i class="fa fa-plus-circle"></i> Enable', current_path() . '/confirm/' . $bundle->name, array('attributes' => array('class' => array('btn btn-primary')), 'html' => TRUE));
      if (isset($info['project_demo_url'])) {
        $actions[] = l('<i class="fa fa-info-circle"></i> More Information', $info['project_demo_url'], array('attributes' => array('class' => array('btn btn-default'), 'target' => '_blank'), 'html' => TRUE));
      }
    }
    $output['bundles']['bundle_group_' . $key][$bundle->name]['actions']= array(
      '#markup' => join('  ', $actions),
      '#prefix' => '<div class="bundle-actions">',
      '#suffix' => '</div>',
    );
  }
  return $output;
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

  if (variable_get('profile_module_manager_disable_enabling', 1) == 0) {
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
