<?php


/**
 * @file
 * General database dump/restore code for Backup and Migrate.
 */

/**
 * Restore from a previously backed up files. File must be a decompressed SQL file.
 */
function backup_migrate_db_restore($source, $file, $settings) {
  $num = 0;

  if ($type = _backup_migrate_db_get_db_type($source->dest_url['scheme'])) {
    // Switch to a different db if specified.
    _backup_migrate_db_switch_db($source->get_location());

    backup_migrate_filters_invoke_all('pre_restore', $file, $settings);

    // Open the file using the file wrapper. Check that the dump is of the right type (allow .sql for legacy reasons).
    if ($file->type_id() !== $type && $file->type_id() !== 'sql') {
      _backup_migrate_message("Unable to restore from file %file because it is of an unknown file type.", array("%file" => $file->filepath()), 'error');
    }
    else {
      // Dump the database.
      $function = 'backup_migrate_restore_db_from_file_'. $type;
      if (function_exists($function)) {
        $num = $function($file, $settings);
      }
    }

    backup_migrate_filters_invoke_all('post_restore', $file, $settings, $num);

    // Switch back to the previous db.
    _backup_migrate_db_switch_db();
  }
  return $num;
}

/**
 * Build the database dump file. Takes a list of tables to exclude and some formatting options.
 */
function backup_migrate_db_backup($source, $file, $settings) {
  $success = FALSE;

  if ($type = _backup_migrate_db_get_db_type($source->dest_url['scheme'])) {
    $file->push_type($type);

    // Switch to a different db if specified.
    _backup_migrate_db_switch_db($source->get_location());

    backup_migrate_filters_invoke_all('pre_backup', $source, $file, $settings);

    // Dump the database.
    $function = 'backup_migrate_backup_db_to_file_'. $type;
    if (function_exists($function)) {
      $success = $function($file, $settings);
    }

    backup_migrate_filters_invoke_all('post_backup', $source, $file, $settings, $success);

    // Switch back to the previous db.
    _backup_migrate_db_switch_db();
  }
  return $success ? $file : FALSE;
}

/**
 * Get the list of table names.
 */
function _backup_migrate_get_table_names($source) {
  $out = array();
  if ($source && $type = _backup_migrate_db_get_db_type($source->dest_url['scheme'])) {
    $function = '_backup_migrate_get_table_names_'. $type;
    if (function_exists($function)) {
      $out = $function();
    }
  }
  return $out;
}

/**
 * Tables to ingore altogether. None by default.
 */
function _backup_migrate_default_exclude_tables() {
  return array();
}

/**
 * Return the default tables whose data can be ignored. These tables mostly contain
 *  info which can be easily reproducted (such as cache or search index)
 *  but also tables which can become quite bloated but are not necessarily extremely
 *  important to back up or migrate during development (such ass access log and watchdog)
 */
function _backup_migrate_default_structure_only_tables() {
  $core = array(
    'cache',
    'cache_admin_menu',
    'cache_browscap',
    'cache_content',
    'cache_filter',
    'cache_calendar_ical',
    'cache_location',
    'cache_menu',
    'cache_page',
    'cache_reptag',
    'cache_views',
    'cache_views_data',
    'cache_block',
    'cache_update',
    'cache_form',
    'cache_bootstrap',
    'cache_field',
    'cache_image',
    'cache_path',
    'sessions',
    'search_dataset',
    'search_index',
    'search_keywords_log',
    'search_total',
    'watchdog',
    'accesslog',
    'devel_queries',
    'devel_times',
  );
  $alltables = array_merge($core, module_invoke_all('devel_caches'));
  return $alltables;
}

/**
 * Switch to the db described by the DB URL, or back to previous if none selected.
 */
function _backup_migrate_db_switch_db($in_url = NULL) {
  static $db_stack = array();
  global $db_url;

  // If no DB URL is specified, pop the previous one and set to it.
  if ($in_url === NULL && $db_stack) {
    db_set_active(array_pop($db_stack));
  }

  // If there is a valid DB URL, switch to it.
  if ($in_url) {
    // Make the db_url into an array if needed.
    if (!is_array($db_url)) {
      $db_url = array('default' => $db_url);
    }
    // Add the new db to the db_url array.
    $db_url[$in_url] = $in_url;
    // Switch to the new db and push the old one on the stack
    $db_stack[] = db_set_active($in_url);
  }
}

/**
 * Get the appropriate db type for file inclusion and calling the right function.
 */
function _backup_migrate_db_get_db_type($scheme) {
  static $type = NULL;
  if ($type === NULL) {
    switch ($scheme) {
      case 'mysql':
      case 'mysqli':
        $type = 'mysql';
        backup_migrate_include('db.mysql');
      break;
      default: 
        $type = false;
      break;
    }
    if (!$type) {
      if ($scheme) {
        _backup_migrate_message("Backup and migrate does not support @type databases.", array("@type" => $scheme), 'error');
      }
      else {
        _backup_migrate_message("Backup and migrate does not support this database type.", array(), 'error');
      }
    }
  }
  return $type;
}
