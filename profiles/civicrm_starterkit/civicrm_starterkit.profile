<?php
/**
 * @file
 * Enables modules and site configuration for a site installation with CiviCRM.
 */

/**
 * Implements hook_form_FORM_ID_alter() for install_configure_form().
 *
 * Allows the profile to alter the site configuration form.
 */
function civicrm_starterkit_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
}

/*
 * Implements hook_install_tasks_alter()
 */
function civicrm_starterkit_install_tasks_alter(&$tasks, $install_state) {

  // substitute our own finished step
  $tasks['install_finished'] = array(
    'display_name' => st('Finished Installation'),
    'display' => 1,
    'function' => 'civicrm_starterkit_install_finished',
  );
    
} 

function civicrm_starterkit_install_finished(&$install_state) {
  //admin/reports/communitymedia-checklist
  drupal_set_title(st('@drupal installation complete', array('@drupal' => drupal_install_profile_distribution_name())), PASS_THROUGH);
  $messages = drupal_set_message();
  $output = '<p>' . st('Congratulations, you installed @drupal!', array('@drupal' => drupal_install_profile_distribution_name())) . '</p>';
  $output .= '<p>' . (isset($messages['error']) ? st('Review the messages above before <a href="@url">using the CiviCRM Checklist to begin configuring your new site</a>.', array('@url' => url('civicrm/admin/configtask'))) : st('<a href="@url">Use the CiviCRM Checklist to begin configuring your new site</a>.', array('@url' => url('civicrm/admin/configtask')))) . '</p>';   
  
   //move CiviCRM menu to Admin Toolbar 
  // @TODO:  Is it safe to assume admin will be plid 2? 
  $mlid = db_query("SELECT mlid FROM {menu_links} WHERE link_path = 'civicrm/dashboard'")->fetchField();
  
  if ($mlid) {
    db_update('menu_links')
    ->fields(array('menu_name' => 'management', 'plid' => 2, 'weight' => '99', 'depth' => 2, 'p1' => 2, 'p2' => $mlid))
    ->condition('link_path', 'civicrm/dashboard')
    ->condition('router_path', 'civicrm')
    ->execute();
  }
  
  variable_set('civicrmtheme_theme_admin', 'seven');
  
  // Update the menu router information.
  menu_rebuild(); 
  
  // Flush all caches to ensure that any full bootstraps during the installer
  // do not leave stale cached data, and that any content types or other items
  // registered by the install profile are registered correctly.
  drupal_flush_all_caches();

  // Remember the profile which was used.
  variable_set('install_profile', drupal_get_profile());

  // Install profiles are always loaded last
  db_update('system')
    ->fields(array('weight' => 1000))
    ->condition('type', 'module')
    ->condition('name', drupal_get_profile())
    ->execute();

  // Cache a fully-built schema.
  drupal_get_schema(NULL, TRUE);

  // Run cron to populate update status tables (if available) so that users
  // will be warned if they've installed an out of date Drupal version.
  // Will also trigger indexing of profile-supplied content or feeds.
  drupal_cron_run();

  return $output;

} 

// Functions Borrowed from Commerce Kickstarter 

/**
 * Implements hook_update_projects_alter().
 */
function civicrm_starterkit_update_projects_alter(&$projects) {
  // Enable update status for the profile.
  $modules = system_rebuild_module_data();
  // The module object is shared in the request, so we need to clone it here.
  $profile = clone $modules['civicrm_starterkit'];
  $profile->info['hidden'] = FALSE;
  _update_process_info_list($projects, array('civicrm_starterkit' => $profile), 'module', TRUE);
}

/**
 * Implements hook_update_status_alter().
 *
 * Disable reporting of projects that are in the distribution, but only
 * if they have not been updated manually.
 *
 * Projects with insecure / revoked / unsupported releases are only shown
 * after two days, which gives enough time to prepare a new Kickstart release
 * which the users can install and solve the problem.
 */
function civicrm_starterkit_update_status_alter(&$projects) {
  $bad_statuses = array(
    UPDATE_NOT_SECURE,
    UPDATE_REVOKED,
    UPDATE_NOT_SUPPORTED,
  );

  $make_filepath = drupal_get_path('module', 'civicrm_starterkit') . '/drupal-org.make';
  if (!file_exists($make_filepath)) {
    return;
  }

  $make_info = drupal_parse_info_file($make_filepath);
  foreach ($projects as $project_name => $project_info) {
    // Never unset the drupal project to avoid hitting an error with
    // _update_requirement_check(). See http://drupal.org/node/1875386.
    if ($project_name == 'drupal') {
      continue;
    }
    // Hide cm_ projects, they have no update status of their own.
    //if (strpos($project_name, 'cm_') !== FALSE) {
      //unset($projects[$project_name]);
    //}
    // Hide bad releases (insecure, revoked, unsupported) if they are younger
    // than 7 days (giving distribution time to prepare an update).
    if (isset($project_info['status']) && in_array($project_info['status'], $bad_statuses)) {
      $days_ago = strtotime('7 days ago');
      if ($project_info['releases'][$project_info['recommended']]['date'] < $days_ago) {
        unset($projects[$project_name]);
      }
    }
    // Hide projects shipped w/ distro if they haven't been manually updated.
    elseif (isset($make_info['projects'][$project_name])) {
      $version = $make_info['projects'][$project_name]['version'];
      if (strpos($version, 'dev') !== FALSE || (DRUPAL_CORE_COMPATIBILITY . '-' . $version == $project_info['info']['version'])) {
        unset($projects[$project_name]);
      }
    }
  }
}