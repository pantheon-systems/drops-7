<?php

/**
 * @file
 * csvImporter.module
 * Let's import a file.  Subsequent uploads will overwrite said file.
 */

/**
 * Form builder, validate, and submit functions for the importer form.
 *
 * @return output
 *   The form output object.
 */
function csvImporter_form() {
  $help  = "<p>" . t('This tool uploads your CSV file to the server -- for subsequent loading into Drupal. The file you upload must always have the same file name.') . "</p>";
  $help .= "<p>" . t('After uploading your CSV file, please proceed !link to import your data to the live site.', array('!link' => l(t('here'), 'admin/content/migrate'))) . "</p>";
  $form['help']['#markup'] = $help;

  $form['#attributes'] = array(
    'enctype' => 'multipart/form-data',
  );
  $description = ($max_size = parse_size(ini_get('upload_max_filesize'))) ?
    t(
      'Due to server restrictions, the <strong>maximum upload file size is !max_size</strong>. Files that exceed this size will be disregarded.',
      array('!max_size' => format_size($max_size))) :
    '';
  $form['csvfile'] = array(
    '#title' => t('CSV File'),
    '#type'  => 'file',
    '#description' => $description,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Commence Import'),
  );
  $form['#validate'] = array(
    'csvImporter_validate_fileupload',
    'csvImporter_form_validate',
  );
  return $form;
}

/**
 * Validate the file upload.
 *
 * It must be a CSV, and we must
 * successfully save it to our import directory.
 *
 * @param string $form
 *   The form being used to edit the node.
 * @param array $form_state
 *   The form state array.
 *
 * @return boolen
 *   True/False.
 */
function csvImporter_validate_fileupload(&$form, &$form_state) {
  $validators = array('file_validate_extensions' => array('csv'));
  $path = 'public://' . (variable_get('csvimportpeople', 'csvimportpeople'));
  if (file_prepare_directory($path, FILE_CREATE_DIRECTORY)) {
    if ($file = file_save_upload(
        'csvfile',
        $validators,
        $path,
        FILE_EXISTS_REPLACE)) {
      $form_state['values']['csvupload'] = $file->destination;
    }
    else {
      form_set_error(
        'csvImporter',
        t(
          'Unable to copy upload file to !dest',
          array('!dest' => $destination)));
    }
  }
}

/**
 * Page callback function for the menu item This just returns our $form.
 *
 * To indicate a validation error, use form_set_error().
 *
 * @param string $form
 *   The form being used to edit the node.
 * @param array $form_state
 *   The form state array.
 *
 * @return boolen
 *   True/False.
 */
function csvImporter_form_validate(&$form, &$form_state) {
  if (isset($form_state['values']['csvupload'])) {
    if ($handle = fopen($form_state['values']['csvupload'], 'r')) {
      $line_count = 1;
      $first = TRUE;
      if ($line = fgetcsv($handle, 4096)) {
        // This is where you can validate aspects of the file itself, like the
        // number of columns.
        // if(count($line) != 3){ form_set_error('csvfile', t('This file has
        // the incorrect number of columns. Expecting 3')); }
      }
      fclose($handle);
    }
    else {
      form_set_error(
        'csvfile',
        t(
          'Unable to read uploaded file !filepath',
          array('!filepath' => $form_state['values']['csvupload'])));
    }
  }
}

/**
 * Page callback function for the menu item This just returns our $form.
 *
 * To indicate a validation error, use form_set_error().
 *
 * @param string $form
 *   The form being used to edit the node.
 * @param array $form_state
 *   The form state array.
 *
 * @return boolen
 *   True/False.
 */
function csvImporter_form_submit(&$form, &$form_state) {
  $batch = array(
    'title' => t('Importing CSV ...'),
    'operations' => array(),
    'init_message' => t('Commencing'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('An error occurred during processing'),
    'finished' => 'csvImporter_import_finished',
  );
  $file_name = $form_state['values']['csvupload'];
  if (isset($file_name)) {
    if ($handle = fopen($file_name, 'r')) {
      $batch['operations'][] = array(
        '_csvImporter_remember_filename',
        array($file_name));
      $line_count = 1;
      $first = TRUE;
      $line = fgetcsv($handle, 4096);
      while ($line = fgetcsv($handle, 4096)) {
        // We use base64_encode to ensure we don't overload the batch
        // processor by stuffing complex objects into it.
        $batch['operations'][] = array(
          '_csvImporter_import_line',
          array(array_map('base64_encode', $line)));
        $line_count++;
      }
      fclose($handle);
      $message = t('The !file has been uploaded with !lines including table header.',
                 array('!file' => $file_name, '!lines' => format_plural($line_count, '1 line', '@count lines')));
      drupal_set_message($message);
    }
  }
  batch_set($batch);
}

/**
 * Finished callback.
 *
 * @param string $success
 *   The form being used to edit the node.
 * @param string $results
 *   The form being used to edit the node.
 * @param string $operations
 *   The form being used to edit the node.
 *
 * @return string
 *   The batch message.
 */
function csvImporter_import_finished($success, $results, $operations) {
  if (!empty($results['failed_rows'])) {
    $dir  = file_directory_path() . '/csvImporter/';
    if (file_check_directory($dir, FILE_CREATE_DIRECTORY)) {
      // We validated extension on upload.
      $csv_filename = 'failed_rows-' . basename($results['uploaded_filename']);
      $csv_filepath = $dir . '/' . $csv_filename;
      $targs = array(
        '!csv_url' => l(check_plain($csv_filename), $csv_filepath),
        '%csv_filename' => $csv_filename,
        '%csv_filepath' => $csv_filepath,
      );
      if ($handle = fopen($csv_filepath, 'w+')) {
        foreach ($results['failed_rows'] as $failed_row) {
          fputcsv($handle, $failed_row);
        }
        fclose($handle);
        drupal_set_message(
          t(
            'Some rows failed to import. You may download a CSV of these rows: !csv_url',
            $targs),
          'error');
      }
      else {
        drupal_set_message(
          t(
            'Some rows failed to import, but unable to write error CSV to %csv_filepath',
            $targs),
          'error');
      }
    }
    else {
      drupal_set_message(
        t(
          'Some rows failed to import, but unable to create directory for error CSV at %csv_directory',
          $targs),
        'error');
    }
  }
  return t('The CSV import has completed.');
}

/**
 * Set associative array to remember filename.
 *
 * @param string $filename
 *   The form being used to edit the node.
 * @param string $context
 *   The form being used to edit the node.
 *
 * @return string
 *   The batch message.
 */
function _csvImporter_remember_filename($filename, &$context) {
  $context['results']['uploaded_filename'] = $filename;
}

/**
 * Batch helper.
 *
 * @param string $line
 *   The form being used to edit the node.
 * @param string $session_nid
 *   The form being used to edit the node.
 * @param array $context
 *   The form being used to edit the node.
 *
 * @return boolen
 *   True/False.
 */
function _csvImporter_import_line($line, $session_nid, &$context) {
  $context['results']['rows_imported']++;
  $line = $cleaned_line = array_map('base64_decode', $line);

  // Give feedback to the importer about which operation we're preforming.
  // Show the row count by default.
  $context['message'] = t(
    'Importing row !c',
    array('!c' => $context['results']['rows_imported']));

  // Or, Provide some feedback about the row currently being processed.
  $context['message'] = t('Importing %title', array('%title' => $line[0]));
  // End giving feedback to the importer.  This is where you would convert the
  // line of the CSV file into a new object, be it a node, user, etc.
  // drupal_set_message("Parsed line {$line[0]}");
  // End converting the line to a drupal object.
  // You can capture and report on failed lines by doing something like this.
  if ($line[1] == 'ROW' && $line[2] == 'FAILS') {
    $context['results']['failed_rows'][] = $line;
  }
  // End capturing and reporting on failed lines.
}
