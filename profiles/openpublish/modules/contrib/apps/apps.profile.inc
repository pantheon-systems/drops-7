<?php
/**
 * This file handles all the functions for downloading, transferring and 
 * installing apps during the install process.
 */

/**
 * Add install tasks to profile install tasks.
 * 
 * See apps.api.php for use.
 *
 */
function apps_profile_install_tasks($install_state, $apps_server, $default_apps = array()) {
  // Only use apps forms during interactive installs.
  $tasks = array();
  if ($install_state['interactive']) {
    $_SESSION['apps_defaults'] = $default_apps;
    $_SESSION['apps_server'] = $apps_server;
    $tasks = array(
      'apps_profile_apps_select_form_' . $apps_server => array(
        'display_name' => st('Install Apps'),
        'type' => 'form',
        'function' => 'apps_profile_apps_select_form',
      ),
      'apps_profile_download_app_modules_' . $apps_server => array(
        'display' => FALSE,
        'type' => 'batch',
        'run' => (isset($_SESSION['apps']))?INSTALL_TASK_RUN_IF_NOT_COMPLETED:INSTALL_TASK_SKIP,
        'function' => 'apps_profile_download_app_modules',
      ),
      // Only need this if using filetransfer authorization.
      'apps_profile_authorize_transfer_' . $apps_server => array(
        'display' => FALSE,
        'type' => 'form',
        'run' => (!is_writeable(conf_path()) && isset($_SESSION['apps']))?INSTALL_TASK_RUN_IF_NOT_COMPLETED:INSTALL_TASK_SKIP,
        'function' => 'apps_profile_authorize_transfer',
      ),
      'apps_profile_install_app_modules_' . $apps_server => array(
        'display' => FALSE,
        'type' => 'batch',
        'run' => (isset($_SESSION['apps']))?INSTALL_TASK_RUN_IF_NOT_COMPLETED:INSTALL_TASK_SKIP,
        'function' => 'apps_profile_install_app_modules',
      ),
      'apps_profile_enable_app_modules_' . $apps_server => array(
        'display' => FALSE,
        'run' => (isset($_SESSION['apps']))?INSTALL_TASK_RUN_IF_NOT_COMPLETED:INSTALL_TASK_SKIP,
        'function' => 'apps_profile_enable_app_modules',
      ),
    );
  }
  return $tasks;
}

/**
 * Apps install form
 */
function apps_profile_apps_select_form($form, $form_state, &$install_state) {
  drupal_set_title(t('Install Apps'));
  // Get and cache the apps manifest file.
  apps_include('manifest');
  $_SESSION['apps_manifest'] = apps_apps($_SESSION['apps_server']);

  // Set a message if no manifest received.
  if (!isset($_SESSION['apps_manifest'])) {
    drupal_set_message('<b>Unable to contact Apps Server.</b><br> For some reason we were unable to contact the apps server.', 'error');
  }
  else {
    $form['actions'] = array('#type' => 'actions', '#weight' => 3);
    foreach($_SESSION['apps_manifest'] as $name => $app) {
      if ($name != '#theme') {
        $options[$name] = '<strong>' . $app['name'] . '</strong><br>' . $app['description'];
      }
    }
    $form = array();

    $form['apps_message'] = array(
      '#markup' => t('<h2>Apps</h2><p>Apps are the next generation in usability for Drupal. They contain bundles of functionality for your website. Select any apps you want to install right now. You can add more later on the apps page.</p></p>In order to install apps, you must be able to FTP or SSH into your server. This uses the same process as the update module.</p>'),
    );

    $form['apps_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Select Apps To Install'),
      '#collapsible' => FALSE,
    );
    $form['apps_fieldset']['apps'] = array(
      '#type' =>'checkboxes',
      '#title' => t('Apps'),
      '#default_value' => $_SESSION['apps_defaults'],
      '#options' => $options,
    );

    $form['default_content_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Default Content'),
      '#collapsible' => FALSE,
    );
    $form['default_content_fieldset']['default_content'] = array(
      '#type' => 'checkbox',
      '#title' => t('Install default content'),
      '#default_value' => TRUE,
      '#description' => t('By selecting this box default content will be installed for each app. Without default content the site may look empty before you start adding to it. You can remove the default content later by going to the apps config page.'),
    );

    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Install Apps'),
    );
  }
  $form['actions']['skip'] = array(
    '#type' => 'submit',
    '#value' => t('Skip this step'),
  );
  drupal_add_css("#apps-profile-apps-select-form .form-submit { display:inline; }", array('type' => 'inline'));

  return $form;
}

/**
 * Submit function for apps_profile_apps_select_form.
 */
function apps_profile_apps_select_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Install Apps')) {
    $_SESSION['apps'] = array_filter($form_state['values']['apps']);
    $_SESSION['apps_default_content'] = $form_state['values']['default_content'];
  }
}

/**
 * Batch process apps download.
 */
function apps_profile_download_app_modules(&$install_state) {
  apps_include('installer');
  $apps = $_SESSION['apps'];
  foreach ($_SESSION['apps'] as $id => $name) {
    $apps[] = $_SESSION['apps_manifest'][$name];
  }
  $batch = apps_download_apps_batch($apps);
  
  $batch['finished'] = 'apps_profile_download_batch_finished';
  return $batch;
}

/**
 * Batch callback invoked when the download batch is completed.
 *
 * This is a copy of update_manager_download_batch_finished without the goto
 * which messes up the batch during install.
 */
function apps_profile_download_batch_finished($success, $results) {
  if (!empty($results['errors'])) {
    $error_list = array(
      'title' => t('Downloading updates failed:'),
      'items' => $results['errors'],
    );
    drupal_set_message(theme('item_list', $error_list), 'error');
  }
  elseif ($success) {
    drupal_set_message(t('Updates downloaded successfully.'));
    $_SESSION['update_manager_update_projects'] = $results['projects'];
    //drupal_goto('admin/update/ready'); //This was removed.
  }
  else {
    // Ideally we're catching all Exceptions, so they should never see this,
    // but just in case, we have to tell them something.
    drupal_set_message(t('Fatal error trying to download.'), 'error');
  }
}

/**
 * Get filetransfer authorization form.
 */
function apps_profile_authorize_transfer($form, $form_state, &$install_state) {
  // Set the $_SESSION variables so that authorize form knows what to do after authorization.
  system_authorized_init('apps_profile_authorize_transfer_save', drupal_get_path('module', 'apps') . '/apps.profile.inc', array(), t('Apps Install Manager'));
  require_once DRUPAL_ROOT . '/includes/authorize.inc';
  // Get the authorize form.
  $form = drupal_retrieve_form('authorize_filetransfer_form', $form_state);
  // Add in the default form handlers.
  $form['#validate'][] = 'authorize_filetransfer_form_validate';
  $form['#submit'][] = 'authorize_filetransfer_form_submit';
  return $form;
}

/**
 * Callback after the authorize_filetransfer_form_submit. 
 * 
 * Save the file transfer protocol.
 */
function apps_profile_authorize_transfer_save($filetransfer, $nothing = array()) {
  $_SESSION['filetransfer'] = $filetransfer;
}

/**
 * Batch process apps install.
 */
function apps_profile_install_app_modules(&$install_state) {
  $batch = array();
  if (!empty($_SESSION['update_manager_update_projects'])) {
    apps_include('installer');
    
    // Make sure the Updater registry is loaded.
    drupal_get_updaters();

    $updates = array();
    $project_types = $_SESSION['update_manager_update_projects'];
    foreach($project_types as $type => $projects) {
      $directory = apps_extract_directory($type);
      foreach ($projects as $project => $url) {
        $project_location = $directory . '/' . $project;
        $updater = Updater::factory($project_location);
        $project_real_location = drupal_realpath($project_location);
        $updates[] = array(
          'project' => $project,
          'updater_name' => get_class($updater),
          'local_url' => $project_real_location,
        );
      }
    }
    
    if (isset($_SESSION['filetransfer'])) {
      // We have authenticated a filetransfer so use it.
      $filetransfer = $_SESSION['filetransfer'];
    }
    else {
      // This is a local transfer because the config_path is writeable.
      $filetransfer = new FileTransferLocal(DRUPAL_ROOT);
    }
    module_load_include('inc', 'update', 'update.authorize');
    $operations = array();
    foreach ($updates as $update => $update_info) {
      $operations[] = array(
        'update_authorize_batch_copy_project',
        array(
          $update_info['project'],
          $update_info['updater_name'],
          $update_info['local_url'],
          $filetransfer,
        ),
      );
    }

    $batch = array(
      'title' => t('Installing updates'),
      'init_message' => t('Preparing to update your site'),
      'operations' => $operations,
      'file' => drupal_get_path('module', 'update') . '/update.authorize.inc',
    );
    unset($_SESSION['update_manager_update_projects']);
  }
  return $batch;
}

/**
 * Install downloaded apps.
 */
function apps_profile_enable_app_modules(&$install_state) {
  $modules = array_keys($_SESSION['apps']);
  if ($_SESSION['apps_default_content']) {
    $modules[] = 'enterprise_content';
    $files = system_rebuild_module_data();
    foreach($_SESSION['apps'] as $app) {
      // Should probably check the app to see the proper way to do this.
      if (isset($files[$app . '_content'])) {
        $modules[] = $app . '_content';
      }
    }
  }
  if (!empty($modules)) {
    module_enable($modules);
  }
  
  // Do a little cleanup
  unset($_SESSION['apps']);
  unset($_SESSION['apps_default_content']);
  unset($_SESSION['apps_server']);
  unset($_SESSION['apps_manifest']);
}

