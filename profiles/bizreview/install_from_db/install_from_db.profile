<?php

/**
 * @file
 * Install profile helper to add option for importing from database.
 */

/**
 * CALL THIS from your profile_install_tasks_alter() hook function.
 */
function install_from_db_install_tasks_alter(&$tasks, $install_state) {
  // prevent cron from running when finished
  $tasks['install_finished']['function'] = 'install_from_db_install_finished';
  // redirect the drupal install_profile_modules step to our own function
  $tasks['install_profile_modules']['function'] = 'install_from_db_install_profile_modules';

  // add the new quickstart step to the installer
  $quickstart = array(
      'display_name' => st('Choose installation method'),
      'type' => 'form',
      'run' => INSTALL_TASK_RUN_IF_NOT_COMPLETED,
    );
  $tasks = _install_from_db_insert_before_key($tasks, 'install_profile_modules',
    'install_from_db_form', $quickstart);
}

/**
 * Helper function to insert a key/value pair before an existing key
 * Used to re-order the install tasks array
 */
function _install_from_db_insert_before_key( $originalArray, $originalKey, $insertKey, $insertValue ) {
  $newArray = array();
  $inserted = false;
  foreach ($originalArray as $key => $value) {
    if (!$inserted && ($key === $originalKey)) {
      $newArray[$insertKey] = $insertValue;
      $inserted = true;
    }
    $newArray[$key] = $value;
  }
  return $newArray;
}

/**
 * Prompt user to select normal or quick installation method
 * TODO: Check db to be sure it is mysql, or provide mechanism for
 * using dumps of other database types.
 *
 * @see install_from_db_form_validate()
 * @see install_from_db_form_submit()
 */
function install_from_db_form($form, &$form_state, &$install_state) {
  $profile = $install_state['parameters']['profile'];
  // find database dump in the /db folder within the profile
  // TODO: support different database dump formats
  $filename = DRUPAL_ROOT . '/profiles/' . $profile . '/db/' . $profile . '.mysql';

  $conn = Database::getConnection('default');
  if (!file_exists($filename) || !$conn || ($conn->driver() !== 'mysql')) {
    // can't do quickstart if no db dump exists
    // also only allow quickstart for mysql databases currently
    unset($install_state['parameters']['quickstart']);
    $form_state['executed'] = TRUE;
    return;
  }
  $install_state['parameters']['db_import_filename'] = $filename;

  if ($install_state['interactive'] && !empty($install_state['parameters']['quickstart'])) {
    // if url argument is already specified, then just use it
    $form_state['input']['quickstart'] = $install_state['parameters']['quickstart'];
    $form_state['executed'] = TRUE;
    return;
  }

  if (!$install_state['interactive'] && !isset($form_state['values']['quickstart'])) {
    // default non-interactive to quickstart
    $install_state['parameters']['quickstart'] = 'quick';
    $form_state['input']['quickstart'] = 'quick';
  }

  $form['quickstart']['quick'] = array(
    '#type' => 'radio',
    '#value' => 'quick',
    '#return_value' => 'quick',
    '#title' => st('Quick installation'),
    '#description' => st('Quickly install using a predefined database dump. (recommended)'),
    '#parents' => array('quickstart'),
  );
  $form['quickstart']['standard'] = array(
    '#type' => 'radio',
    '#value' => 'quick',
    '#return_value' => 'standard',
    '#title' => st('Standard installation'),
    '#description' => st('Install using the full Drupal process.  This is slower.'),
    '#parents' => array('quickstart'),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] =  array(
    '#type' => 'submit',
    '#value' => st('Save and continue'),
  );
  return $form;
}

/**
 * Form validation handler for install_from_db_form().
 *
 * @see install_from_db_form_submit()
 */
function install_from_db_form_validate($form, &$form_state) {
}

/**
 * Form submission handler for install_from_db_form().
 *
 * @see install_from_db_form_validate()
 */
function install_from_db_form_submit($form, &$form_state) {
  global $install_state;
  $install_state['parameters']['quickstart'] = $form_state['input']['quickstart'];
}


/**
 * overrides install_profile_modules()
 * For quick install, load database from sql dump
 * otherwise install normally
 */
function install_from_db_install_profile_modules(&$install_state) {
  if (!empty($install_state['parameters']['quickstart']) && ($install_state['parameters']['quickstart'] === 'quick') && !empty($install_state['parameters']['db_import_filename'])) {
    if (!$install_state['interactive']) {
      watchdog('install', 'Installing from database import.', array(), WATCHDOG_INFO);
      print "Installing from database\n";
    }
    // bypass normal module installation
    // load database dump instead
    // uses batch because normal module install task uses batch
    // and it's also good for this potentially time consuming db restore
    $operations = array();
    $defer_operations = array();
    $system_sql = '';
    $filename = $install_state['parameters']['db_import_filename'];
    if (file_exists($filename)) {
      $file = fopen($filename, 'rb');
      if ($file) {
        while (($line = _install_from_db_read_sql_batch($file, $table)) !== false) {
          if (!empty($line)) {
            // don't process blank lines
            if (in_array($table, array('system'))) {
              $defer_operations[] = array('_install_from_db_install_db_import', array($line, $table));
            }
            else {
              $operations[] = array('_install_from_db_install_db_import', array($line, $table));
            }
          }
        }
      }
      fclose($file);
      $operations = array_merge($operations, $defer_operations);
    }
    $batch = array(
      'operations' => $operations,
      'title' => st('Importing site database'),
      'error_message' => st('The database import has encountered an error.'),
      'finished' => '_install_from_db_install_db_import_finished',
    );
    return $batch;
  }
  if (!$install_state['interactive']) {
    watchdog('install', 'Performing standard Drupal install.', array(), WATCHDOG_INFO);
    print "Standard Drupal install\n";
  }
  return install_profile_modules($install_state);
}

/**
 * 'Finished' callback for module installation batch.
 */
function _install_from_db_install_db_import_finished($success, $results, $operations) {
  // remove any field data added to user entity
  // since we did not import any users
  $fields_info = field_info_instances('user', 'user');
  foreach ($fields_info as $field_name => $info) {
    db_delete('field_data_' . $field_name)
      ->condition('entity_type', 'user')
      ->condition('bundle', 'user')
      ->execute();
    db_delete('field_revision_' . $field_name)
      ->condition('entity_type', 'user')
      ->condition('bundle', 'user')
      ->execute();
  }
}

/**
 * Aggressively clear the cache so new database will take affect
 * Do this right after importing the new system module as the last step
 */
function _install_from_db_cc_all() {
  // load the module files as if we just installed them
  $module_list = module_list(TRUE);
  foreach ($module_list as $module) {
    drupal_load('module', $module);
  }

  drupal_static_reset();
  drupal_flush_all_caches();
}

/**
 * Batch callback for batch database import.
 */
function _install_from_db_install_db_import($line, $table, &$context) {
  global $conf;

  if ($table === 'variable') {
    // save/restore some variables needed by installer or unique to new site
    $saved_vars = array('install_task', 'install_current_batch', 'cron_key', 'drupal_private_key');
    foreach ($saved_vars as $var) {
      $saved_values[$var] = variable_get($var, '');
    }
  }

  // Do NOT use db_query here as it will mess with the query data, such as the {} in
  // serialized data fields.
  $conn = Database::getConnection('default');
  $stmt = $conn->prepare($line);
  $stmt->execute();
  $stmt->closeCursor();

  if ($table === 'variable') {
    // restore saved variables
    foreach ($saved_vars as $var) {
      variable_set($var, $saved_values[$var]);
    }
  }
  else if ($table === 'system') {
    // Flush all caches to complete the module installation process. Subsequent
    // installation tasks will now have full access to the profile's modules.
    _install_from_db_cc_all();
  }
}

/**
 * Read a batch of sql commands (ending in commit)
 * @param $file - name of file to read from
 * @param $table - name of table referenced in sql statements is returned
 * @return - string containing sql commands for a single table.
 */
function _install_from_db_read_sql_batch($file, &$table) {
  $conn = Database::getConnection('default');
  $line = '';
  $table = '';
  $skip = FALSE;
  $skip_tables = array('batch', 'cache', 'sessions', 'queue', 'semaphore', 'users',
    'advagg_aggregates', 'advagg_aggregates_hashes',
    'advagg_aggregates_versions', 'advagg_files');
  while (($newline = _install_from_db_read_sql_command_from_file($file)) !== false) {
    // process the line read.
    $newline_prefix = _install_from_db_replace_prefix($newline, $conn);
    // block of SQL starts with a Table structure comment
    if (preg_match('/Table structure for table `([^`]+)`/', $newline, $matches)) {
      $new_table = $matches[1];
      if (!empty($table) && ($new_table !== $table)) {
        // close out the previous table
        // put this line back into the read buffer for the next time
        _install_from_db_read_sql_command_from_file($file, $newline);
        break;
      }
      $table = $new_table;
      if (!$skip) {
        if (in_array($table, $skip_tables) || (strpos($table, 'cache_') === 0)) {
          // skip listed tables, along with any cache_* table
          $skip = TRUE;
        }
      }
    }
    if (preg_match('/\A(CREATE TABLE )`([^`]+)`/', $newline, $matches)) {
      // always check first before creating tables
      $newline_prefix = preg_replace('/\ACREATE TABLE/', 'CREATE TABLE IF NOT EXISTS', $newline_prefix);
      if ($skip) {
        // make sure skipped tables are still created
        $line .= $newline_prefix;
      }
    }
    if (!empty($newline) && substr($newline, 0, 2) == '--') {
      // otherwise skip comments
      $newline_prefix = '';
    }
    if (!$skip && !empty($newline_prefix)) {
      $line .= $newline_prefix;
    }
    // block of SQL ends with a commit command.
    if ($newline === 'commit;') {
      // be sure to turn autocommit back on for Drupal batch system and other database
      // queries to work properly
      $line .= 'set autocommit=1;';
    }
  }
  if (empty($line) && ($newline === FALSE)) {
    return FALSE;
  }
  return $line;
}

/**
 * Read a multiline sql command from a file.
 *
 * Supports the formatting created by mysqldump, but won't handle multiline comments.
 * Taken from backup_migrate module
 */
function _install_from_db_read_sql_command_from_file($file, $save_line = '') {
  static $save_for_later = '';

  if (!empty($save_for_later)) {
    // check if a previous line was saved
    $out = $save_for_later;
    $save_for_later = '';
    return trim($out);
  }

  if (!empty($save_line)) {
    // save this text for the next time we read from file
    // used to stuff the previous line back into the read buffer
    $save_for_later = $save_line;
    return;
  }

  $out = '';
  while (($line = fgets($file)) !== false) {
    $line = trim($line);
    if (empty($out) && !empty($line) && substr($line, 0, 2) == '--') {
      // return single line comments
      return trim($line);
    }
    // Otherwise, ignore single line comments within a sql statement.
    if (!empty($line) && substr($line, 0, 2) != '--') {
      $out .= ' ' . $line;
      // If a line ends in ; or */ it is a sql command.
      if (substr($out, strlen($out) - 1, 1) == ';' || substr($out, strlen($out) - 2, 2) == '*/') {
        return trim($out);
      }
    }
  }
  if (empty($out) && ($line === FALSE)) {
    return FALSE;
  }
  return trim($out);
}

/**
 * Apply the correct prefix to tables in the SQL statement
 */
function _install_from_db_replace_prefix($line, $conn) {
  $patterns = array(
    'DROP TABLE IF EXISTS',
    'CREATE TABLE',
    'LOCK TABLES',
    'ALTER TABLE',
    'INSERT INTO',
    'UPDATE',
  );
  foreach ($patterns as $pattern) {
    // handle lines like this:  /*!40000 ALTER TABLE `actions` ENABLE KEYS */;
    $find = '#\A((/\*\!\d+ )?' . $pattern . ' )`([^`]+)`#';
    // NOTE: We need to replace `tablename` with the new prefixed table name
    // We cannot run $conn->prefixQuery on the entire $line because it will mess
    // with the {} characters within serialized data.
    // So we ONLY match and replace the table name here.
    if (preg_match($find, $line, $matches)) {
      $new_table = $conn->prefixTables('{' . $matches[3] . '}');
      $replace = '$1`' . $new_table . '`';
      $line = preg_replace($find, $replace, $line);
    }
  }
  return $line;
}

/**
 * Finishes importing files at end of installation.
 * COPIED from install.core.inc but no longer calls drupal_run_cron
 * since we just installed the correct module versions for the distro
 * that we desire.
 *
 * @param $install_state
 *   An array of information about the current installation state.
 *
 * @return
 *   A message informing the user that the installation is complete.
 */
function install_from_db_install_finished(&$install_state) {
  drupal_set_title(st('@drupal installation complete', array('@drupal' => drupal_install_profile_distribution_name())), PASS_THROUGH);
  $messages = drupal_set_message();
  $output = '<p>' . st('Congratulations, you installed @drupal!', array('@drupal' => drupal_install_profile_distribution_name())) . '</p>';
  $output .= '<p>' . (isset($messages['error']) ? st('Review the messages above before visiting <a href="@url">your new site</a>.', array('@url' => url(''))) : st('<a href="@url">Visit your new site</a>.', array('@url' => url('')))) . '</p>';

  // Flush all caches to ensure that any full bootstraps during the installer
  // do not leave stale cached data, and that any content types or other items
  // registered by the installation profile are registered correctly.
  drupal_flush_all_caches();

  // Remember the profile which was used.
  variable_set('install_profile', drupal_get_profile());

  // Installation profiles are always loaded last
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
  //
  // NO!  Don't do this after installing profile from DB since we have the
  // module versions we want and the Update module is potentially enabled
  //
  // drupal_cron_run();
  //
  // Mark cron as run so it doesn't run on next page load either
  variable_set('cron_last', REQUEST_TIME);

  return $output;
}
