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
function apps_profile_install_tasks($install_state, $apps_server) {
  // Need to include the apps.module file because on installs the profile
  // collects all install tasks before any modules are enabled
  module_load_include('module', 'apps');

  // Only use apps forms during interactive installs.
  $tasks = array();
  $apps_server_name = $apps_server['machine name'];

  $task_screen = 'apps_profile_apps_select_form_' . $apps_server_name;
  $_SESSION['apps_servers'][$task_screen] = $apps_server;
  $tasks = array(
    // Setup an initial task to verify capability to run apps.
    'apps_install_verify' => array(
      'display_name' => t('Verify Apps support'),
      'type' => 'form',
      'function' => 'apps_install_verify',
    ),
    $task_screen => array(
      'display_name' => apps_profile_get_server_name($apps_server),
      'type' => 'form',
      'function' => 'apps_profile_apps_select_form',
    ),
    'apps_profile_download_app_modules_' . $apps_server_name => array(
      'display' => FALSE,
      'type' => 'batch',
      // If this is not an interactive install, we can download apps only if we have write access.
      'run' => (!empty($_SESSION['apps_downloads']) && ($install_state['interactive'] || apps_installer_has_write_access())) ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP,
      'function' => 'apps_profile_download_app_modules',
    ),
    // Only need this if using filetransfer authorization.
    'apps_profile_authorize_transfer_' . $apps_server_name => array(
      'display' => FALSE,
      'type' => 'form',
      'run' => (!empty($_SESSION['apps_downloads']) && $install_state['interactive'] && !apps_installer_has_write_access()) ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP,
      'function' => 'apps_profile_authorize_transfer',
    ),
    'apps_profile_install_app_modules_' . $apps_server_name => array(
      'display' => FALSE,
      'type' => 'batch',
      'run' => (!empty($_SESSION['apps_downloads']) && ($install_state['interactive'] || apps_installer_has_write_access())) ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP,
      'function' => 'apps_profile_install_app_modules',
    ),
    'apps_profile_enable_app_modules_' . $apps_server_name => array(
      'display' => FALSE,
      'type' => 'batch',
      'run' => (isset($_SESSION['apps'])) ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP,
      'function' => 'apps_profile_enable_app_modules',
    ),
  );
  return $tasks;
}

/**
 * Apps install form
 */
function apps_profile_apps_select_form($form, $form_state, &$install_state) {
  drupal_set_title(t('Install Apps'));
  // Get and cache the apps manifest file.
  apps_include('manifest');

  // If there is no internet things get in an unfixable state. Use try->catch
  try {
    $apps_server = $_SESSION['apps_servers'][$install_state['active_task']];

    $_SESSION['apps_server'] = $apps_server;
    $_SESSION['apps_manifest'] = apps_apps($apps_server['machine name']);
  }
  catch (Exception $e) {
    $form['info'] = array(
      '#markup' => t('<h2>Error</h2><p>Unable to connect to Apps Server.</p><p>Click "Continue" to finish the installation. You can either fix your internet connection and try the installation again or install apps later from the apps config page.</p>'),
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
    );
    return $form;
  }

  // Set a message if no manifest received.
  if (!isset($_SESSION['apps_manifest'])) {
    drupal_set_message('<b>Unable to contact Apps Server.</b><br> For some reason we were unable to contact the apps server.', 'error');
  }
  else {
    drupal_set_title(apps_profile_get_server_name($_SESSION['apps_server']));
    $form['actions'] = array('#type' => 'actions', '#weight' => 3);
    foreach($_SESSION['apps_manifest'] as $name => $app) {
      if ($name != '#theme') {
        $options[$name] = '<strong>' . $app['name'] . '</strong>';
        if (isset($app['description'])) {
          $options[$name] .= '<br>' . $app['description'];
        }
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
      '#default_value' => $_SESSION['apps_server']['default apps'],
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
  $_SESSION['apps'] = array();
  // Checking to make sure that skip was not clicked. Proceed if op is empty
  // to cover the non-interactive case where there is no op.
  if (empty($form_state['values']['op']) || $form_state['values']['op'] == t('Install Apps')) {
    $_SESSION['apps'] = array_filter($form_state['values']['apps']);
    // Determine if any apps need to be downloaded.
    apps_include('manifest');
    $installed_apps = apps_apps($_SESSION['apps_server']['machine name'], array('installed' => TRUE), TRUE);
    $_SESSION['apps_downloads'] = array_diff_key($_SESSION['apps'], $installed_apps) ? TRUE: FALSE;
    $_SESSION['apps_default_content'] = $form_state['values']['default_content'];
  }
}

/**
 * Batch process apps download.
 */
function apps_profile_download_app_modules(&$install_state) {
  apps_include('installer');
  $apps = array();
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
    $_SESSION['update_manager_update_projects'] = isset($results['projects']) ? $results['projects'] : NULL;
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
  // If this is not an interactive install, we may have apps that were selected
  // but not installed. Filter them out.
  if (!$install_state['interactive']) {
    apps_include('manifest');
    // If not interactive it is one big request, clear some static caches
    // so we recognized modules that have been downloaded just prior.
    system_list_reset();
    drupal_static_reset('apps_manifest');
    $installed_apps = apps_apps($_SESSION['apps_server']['machine name'], array('installed' => TRUE), TRUE);
    $_SESSION['apps'] = array_intersect_key($_SESSION['apps'], $installed_apps);
    // If no selected apps are installed, we have nothing to enable.
    if (empty($_SESSION['apps'])) {
      return;
    }
  }

  $modules = array_keys($_SESSION['apps']);

  // Allow profiles to add default content.
  if (isset($_SESSION['apps_default_content']) && isset($_SESSION['apps_server']['default content callback'])) {
    $function = $_SESSION['apps_server']['default content callback'];
    if (function_exists($function)) {
      $function($modules);
    }
  }
  // Do dependency checking so everything doesn't break from one missing dependency.
  foreach($modules as $id => $module) {
    if (!apps_profile_check_dependencies(array($module => $module))) {
      // Something went wrong. Remove from queue and add error.
      unset($modules[$id]);
      $module_info = system_get_info('module', $module);
      $module_name = isset($module_info['name']) ? $module_info['name'] : $module;
      drupal_set_message(t('There was an error installing @module app.', array('@module' => $module_name)), 'error');
    }
  }
  if (!empty($modules)) {
    // Setup the batch comments
    $enable_commands = array();
    foreach($modules as $module) {
      $module_info = system_get_info('module', $module);
      $module_name = isset($module_info['name']) ? $module_info['name'] : $module;
      $enable_commands[] = array('app_profile_enable_module', array($module, $module_name));
    }

    // Setup the batch operation
    $batch = array(
      'operations' => $enable_commands,
      'file' => drupal_get_path('module', 'apps') . '/apps.profile.inc',
      'title' => t('Enabling apps'),
      'init_message' => t('Preparing to enable the needed apps'),
      'finished' => 'apps_profile_enable_app_modules_finished',
    );
    return $batch;
  }
}

function app_profile_enable_module($module, $module_name, &$context) {
  // This is here to show the user that we are in the process of enabling
  $context['message'] = t('Enabled %module app', array('%module' => $module_name));

  // Enable the module and record any errors
  if (!module_enable(array($module))) {
    $context['results']['errors'][$module] = t('There was an error enabling @module app.', array('@module' => $module_name));
  }

  // Successful outcome
  $context['finished'] = TRUE;
}

/**  
 * Batch callback invoked when enable batch is completed.
 */
function apps_profile_enable_app_modules_finished($success, $results) {
  // Only display errors, on success do nothing since on success any
  // message we print will show up in a weird context (next page)
  if (!empty($results['errors'])) {
    $error_list = array(
      'title' => t('Enabling apps failed:'),
      'items' => $results['errors'],
    );
    drupal_set_message(theme('item_list', $error_list), 'error');
  }
  elseif(!$success) {
    // Ideally we're catching all Exceptions, so they should never see this,
    // but just in case, we have to tell them something.
    drupal_set_message(t('Fatal error trying to enable apps.'), 'error');
  }

  // Do a little cleanup
  // We cannot unset $_SESSION['apps'] here because it is used in the
  // run check for this task and can cause weird interactions with
  // the batch operations.
  // unset($_SESSION['apps']);
  unset($_SESSION['apps_default_content']);
  unset($_SESSION['apps_server']);
  unset($_SESSION['apps_manifest']);
  unset($_SESSION['apps_downloads']);
}

/**
 * Function to check and make sure all dependencies are available. This will stop stray apps
 * or broken downloads from stopping the install process for everything.
 * 
 * Copied from module_enable() in module.inc
 */
function apps_profile_check_dependencies($module_list) {
  // Get all module data so we can find dependencies and sort.
  $module_data = system_rebuild_module_data();
  // Create an associative array with weights as values.
  $module_list = array_flip(array_values($module_list));

  while (list($module) = each($module_list)) {
    if (!isset($module_data[$module])) {
      // This module is not found in the filesystem, abort.
      return FALSE;
    }
    if ($module_data[$module]->status) {
      // Skip already enabled modules.
      unset($module_list[$module]);
      continue;
    }
    $module_list[$module] = $module_data[$module]->sort;

    // Add dependencies to the list, with a placeholder weight.
    // The new modules will be processed as the while loop continues.
    foreach (array_keys($module_data[$module]->requires) as $dependency) {
      if (!isset($module_list[$dependency])) {
        $module_list[$dependency] = 0;
      }
    }
  }
  
  // If we make it this far then everything is good.
  return TRUE;
}

/**
 * Show instructions and check system services to help guide direction
 * for Apps installs.
 */
function apps_install_verify() {
  $form = array();

  $form['opening'] = array(
    '#markup' => '<h1>' . t('Verify Apps support') . '</h1>',
  );

  $form['openingtext'] = array(
    '#markup' => '<p>' . t('Installation of Apps requires that modules are installed on your site using the same mechanism as the Update module in Drupal core. This functionality depends on certain PHP extensions being enabled on your server. Below is the documentation & verification for the various methods of installing. <strong>Note that you need only <em>one</em> of these methods enabled in order to install apps.</strong>') . '</p>',
    '#weight' => -10,
  );

  // Verify FTP support
  $ftp_installed = extension_loaded('ftp');
  $form['ftp'] = array(
    '#type' => 'fieldset',
    '#title' => t('FTP (!status)', array('!status' => $ftp_installed ? t('Enabled!') : t('Not enabled'))),
    '#description' => '',
    '#collapsible' => !$ftp_installed,
    '#collapsed' => !$ftp_installed,
    '#weight' => $ftp_installed ? 0 : 10,
  );
  if (!$ftp_installed) {
    $form['ftp']['#description'] .= t('Your server does not have the FTP PHP extension. You will need to install it or use an alternative method. See <a href="http://us2.php.net/manual/en/book.ftp.php">http://us2.php.net/manual/en/book.ftp.php</a> for how to install the FTP PHP extension.') . '<br /><br />';
  }
  $form['ftp']['#description'] .= t('To install with FTP, you will need an FTP username and password that has permissions to write to your site directory on your server. Be aware that FTP is not an encrypted protocol and your credentials will be transmitted in the clear.');

  // Verify SSH support
  $ssh_installed = extension_loaded('ssh2');
  $form['ssh'] = array(
    '#type' => 'fieldset',
    '#title' => t('SSH (!status)', array('!status' => $ssh_installed ? t('Enabled!') : t('Not enabled'))),
    '#description' => '',
    '#collapsible' => !$ssh_installed,
    '#collapsed' => !$ssh_installed,
    '#weight' => $ssh_installed ? 1 : 11,
  );
  if (!$ssh_installed) {
    $form['ssh']['#description'] .= t('Your server does not have ssh2 installed. You will need to install it or use an alternative method. See <a href="http://us2.php.net/manual/en/book.ssh2.php">http://us2.php.net/manual/en/book.ssh2.php</a> for how to install the ssh2 php extension.') . '<br /><br />';
  }
  $form['ssh']['#description'] .= t('To install with SSH, you will need a username and password of a user that can SSH into the server and has write permissions to your site directory on your server.');

  // Verify web server write permissions
  $install_permissions = apps_installer_has_write_access();
  $vars = array('@install_path' => APPS_INSTALL_PATH);
  $form['webserver'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webserver direct install (!status)', array('!status' => $install_permissions ? t('Enabled!') : t('Not enabled'))),
    '#description' => '',
    '#collapsible' => !$install_permissions,
    '#collapsed' => !$install_permissions,
    '#weight' => $install_permissions ? 2 : 12,
  );
  $form['webserver']['#description'] .= $install_permissions
    ? t('You have write permissions to @install_path.', $vars) . '<br /><br />'
    : t('You do not have sufficient permissions to install by webserver direct install. In order to assign these permissions, go to the root of your drupal install and type <br/><br/><strong>sudo chmod -R 777 @install_path</strong>', $vars) . '<br /><br />';
  $form['webserver']['#description'] .= t('Be aware that there are security issues with leaving your site in this state.');

  // When altering this form and adding an install option, set this value to
  // TRUE if your option is enabled (available).
  $app_enabled = $ftp_installed || $ssh_installed || $install_permissions;
  $form['app_enabled'] = array(
    '#type' => 'value',
    '#value' => $app_enabled,
  );

  $form['#process'] = array('apps_install_verify_process');

  $form['continue'] = array(
    '#type' => 'submit',
    '#value' => t('Continue'),
    '#weight' => 20,
  );

  return $form;
}

/**
 * Process callback for apps_install_verify form.
 *
 * If no options are enabled, show all of them and a message.
 */
function apps_install_verify_process($form) {
  if (!$form['app_enabled']['#value']) {
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && $form[$key]['#type'] == 'fieldset') {
        $form[$key]['#collapsible'] = FALSE;
        $form[$key]['#collapsed'] = FALSE;
      }
    }
    drupal_set_message(t('No app install methods were found. <a href="!url">Retry verification</a>', array('!url' => check_url(drupal_current_script_url()))), 'warning');
  }
  return $form;
}


/**
 * Generate the title of the Apps install screen
 */
function apps_profile_get_server_name($server) {
  $t = get_t();
  return isset($server['title'])
             ? $t('Install @name Apps', array('@name' => $server['title']))
             : $t('Install Apps');

}
