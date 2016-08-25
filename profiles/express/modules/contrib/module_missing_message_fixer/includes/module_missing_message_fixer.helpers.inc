<?php

/**
 * @file
 * Helper file.
 */


/**
 * Helper function to check for modules to fix.
 */
function _module_missing_message_fixer_check_modules() {
  $rows = array();

  // These are special modules that have their own patches already.
  // This will help eliminate some of the brute force of this module.
  $special = array(
    'adminimal_theme' => 'https://www.drupal.org/node/2763581',
    'content' => 'https://www.drupal.org/node/2763555',
    'field_collection_table' => 'https://www.drupal.org/node/2764331',
  );

  // Grab all the modules in the system table.
  $query = db_query("SELECT filename, type, name FROM {system}");

  // Go through the query and check to see if the module exists in the directory.
  foreach ($query->fetchAll() as $record) {
    // Grab the checker.
    $check = drupal_get_filename($record->type, $record->name, $record->filename, FALSE);

    // If drupal_get_filename returns null = we got problems.
    if (is_null($check) &&
        $record->name != 'default' &&
        !array_key_exists($record->name, $special)) {
      // Go ahead and set the row if all is well.
      $rows[$record->name] = array(
        'name' => $record->name,
        'type' => $record->type,
        'filename' => $record->filename,
      );
    }
    // Go ahead and print out a special message for the user.
    elseif (array_key_exists($record->name, $special)) {
      // Add exception to this since content module seems to be ubercart sad only.
      if ($record->name == 'content' && !module_exists('ubercart')) {
        $rows[$record->name] = array(
          'name' => $record->name,
          'type' => $record->type,
          'filename' => $record->filename,
        );
      }
      // Everyone else fails into here.
      else {
        // Set the message.
        $msg =  t('The @module module has a patch. See <a href="@link" target="_blank">this issue</a> for more information.  It <strong>WILL NOT</strong> be removed by Module Missing Message Fixer.', array(
          '@module' => $record->name,
          '@link' => $special[$record->name],
        ));
        // Now print it!
        drupal_set_message($msg, 'status', FALSE);
      }
    }
  }

  return $rows;
}
