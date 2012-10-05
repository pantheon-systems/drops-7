<?php

/**
 * Download all modules and library to a temp location
 *
 * Find all downloadables needed (which we do not have) and sets a batch for download
 * Then proccess that batch and returns to the current apps_install_next page
 *
 * PARAM $app: an app array
 *
 */
function apps_download_apps($app) {
  $download_batch = apps_download_apps_batch(array($app));
  batch_set($download_batch);
  batch_process($_SESSION['apps_install_next']);
}

/** 
 * Make the batch for downloading modules for app.
 * 
 * Split out for use in apps.profile.inc
 */
function apps_download_apps_batch($apps) {
  $download_commands = array();
  foreach ($apps as $app) {
    $downloads = array();
    // find all downloads needed for dependencies
    foreach($app['dependencies'] as $dep) {
      if(!$dep['installed']) {
        $downloads[$dep['downloadable']]['for'][] = $dep['version']['name'];
        $downloads[$dep['downloadable']]['type'] = 'module';
      }
    }
    foreach($app['libraries'] as $lib) {
      if(!$lib['installed']) {
        $downloads[$lib['downloadable']]['for'][] = $lib['version']['name'];
        $downloads[$lib['downloadable']]['type'] = 'library';
      }
    }
    // add our core modules download
    if(!$app['installed']) {
      $downloads[$app['downloadable']]['for'][] = $app['machine_name'];
      $downloads[$app['downloadable']]['type'] = 'app';
    }
    //foreach download find the url
    foreach($downloads as $key => $download) {
      $downloads[$key]['url'] = $app['downloadables'][$key];
      // do a quick dirty pull of the name from the key
      $downloads[$key]['name'] = ($e = strpos($key, " ")) ? substr($key, 0, $e) : $key;
    }
    foreach($downloads as $download) {
      $download_commands[] = array(
        'apps_download_batch',
        array(
          $download['name'],
          $download['url'],
          $download['type']
        ),
      );
    }
  }
  $batch = array(
    'operations' => $download_commands,
    'file' => drupal_get_path('module', 'apps') . '/apps.installer.inc',
    'title' => t('Downloading modules'),
    'init_message' => t('Preparing to download needed modules'),
    'finished' => 'apps_download_batch_finished',
  );
  return $batch;
}

/**
 * Batch callback invoked when the download batch is completed.
 *
 * A pass though to update_manager_download_batch_finished
 * but we set $_GET['destination'] to control the drupal_goto that is 
 * in that function
 */
function apps_download_batch_finished($success, $results) {
  module_load_include("inc", "update", "update.manager");
  $_GET['destination'] = $_SESSION['apps_install_next'];
  update_manager_download_batch_finished($success, $results);
}

/*
 * Pass though to update_manager_batch_project_get
 * we need this becuase in a batch set the file param is for bouth the operations as well
 * as the other callbackes
 */
function apps_download_batch($project, $url, $type, &$context) {
  //$args = func_get_args();
  module_load_include("inc", "update", "update.manager");
  //return call_user_func_array('update_manager_batch_project_get', $args);

  // This is here to show the user that we are in the process of downloading.
  if (!isset($context['sandbox']['started'])) {
    $context['sandbox']['started'] = TRUE;
    $context['message'] = t('Downloading %project', array('%project' => $project));
    $context['finished'] = 0;
    return;
  }

  // Actually try to download the file.
  if (!($local_cache = update_manager_file_get($url))) {
    $context['results']['errors'][$project] = t('Failed to download %project from %url', array('%project' => $project, '%url' => $url));
    return;
  }

  // Extract it.
  $extract_directory = apps_extract_directory($type);
  try {
    update_manager_archive_extract($local_cache, $extract_directory);
  }
  catch (Exception $e) {
    $context['results']['errors'][$project] = $e->getMessage();
    return;
  }

  // Verify it.
  $archive_errors = update_manager_archive_verify($project, $local_cache, $extract_directory);
  $archive_errors = array();
  if (!empty($archive_errors)) {
    // We just need to make sure our array keys don't collide, so use the
    // numeric keys from the $archive_errors array.
    foreach ($archive_errors as $key => $error) {
      $context['results']['errors']["$project-$key"] = $error;
    }
    return;
  }

  // Yay, success.
  $context['results']['projects'][$type][$project] = $url;
  $context['finished'] = 1;
}

/**
 * Wrapper for _update_manager_extract_directory()
 * 
 * Since libraries and modules can live in the same location, we need to 
 * namespace the types so they don't collide like the colorbox module.
 */
function apps_extract_directory($type = '') {
  $directory = _update_manager_extract_directory();
  if ($type) {
    $directory .= '/' . $type;
    if (!file_exists($directory)) {
      mkdir($directory);
    }
  }
  return $directory;

}

/**
 *  Move modules from there temp location in to the drupal tree
 *
 * taken from update_manager_update_ready_form_submit
 * we are using apps_run_install instead of update_authorize_run_update
 *
 * TODO: Get the install to work when we do not own sites OPIC-377
 */
function apps_install_downloads() {
  module_load_include("inc", "update", "update.manager");
  if (!empty($_SESSION['update_manager_update_projects'])) {
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

    // If the owner of the last directory we extracted is the same as the
    // owner of our configuration directory (e.g. sites/default) where we're
    // trying to install the code, there's no need to prompt for FTP/SSH
    // credentials. Instead, we instantiate a FileTransferLocal and invoke
    // update_authorize_run_update() directly.
    //if (fileowner($project_real_location) == fileowner(conf_path())) {
    if (apps_installer_has_write_access()) {
      module_load_include('inc', 'update', 'update.authorize');
      $filetransfer = new FileTransferLocal(DRUPAL_ROOT);
      apps_run_install($filetransfer, $updates); //this is our change
      unset($_SESSION['update_manager_update_projects']);
    }
    // Otherwise, go through the regular workflow to prompt for FTP/SSH
    // credentials and invoke update_authorize_run_update() indirectly with
    // whatever FileTransfer object authorize.php creates for us.
    else {
      // Set the $_SESSION variables so that authorize form knows what to do after authorization.
      system_authorized_init('apps_run_install', drupal_get_path('module', 'apps') . '/apps.installer.inc', array($updates), t('Update manager'));
      require_once DRUPAL_ROOT . '/includes/authorize.inc';
      // Get the authorize form.
      $form = drupal_get_form('authorize_filetransfer_form');
      return $form;
    }
  }
}


/**
 * The batch builder and processor for moving files to drupal 
 *
 * taken from update_authorize_run_update
 * builds a batch and process it for installing modules from the templocation
 */
function apps_run_install($filetransfer, $projects) {
  $operations = array();
  foreach ($projects as $project => $project_info) {
    $operations[] = array(
      'update_authorize_batch_copy_project',
      array(
        $project_info['project'],
        $project_info['updater_name'],
        $project_info['local_url'],
        $filetransfer,
      ),
    );
  }

  $batch = array(
    'title' => t('Installing updates'),
    'init_message' => t('Preparing to update your site'),
    'operations' => $operations,
    'finished' => 'update_authorize_update_batch_finished',
    'file' => drupal_get_path('module', 'update') . '/update.authorize.inc',
  );

  batch_set($batch);
  // Invoke the batch via authorize.php.

  batch_process($_SESSION['apps_install_next']);
}
