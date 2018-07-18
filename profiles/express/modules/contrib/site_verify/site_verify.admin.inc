<?php

function site_verify_list() {
  $engines = site_verify_get_engines();
  $destination = drupal_get_destination();

  $header = array(
    array('data' => t('Engine'), 'field' => 'engine'),
    array('data' => t('Meta tag'), 'field' => 'meta'),
    array('data' => t('File'), 'field' => 'file'),
    array('data' => t('Operations')),
  );

  $query = db_select('site_verify');
  $query->fields('site_verify');
  $query->extend('TableSort')->orderByHeader($header);
  //$query->extend('PagerDefault')->limit(50);
  $verifications = $query->execute();

  $rows = array();
  foreach ($verifications as $verification) {
    $row = array('data' => array());
    $row['data'][] = $engines[$verification->engine]['name'];
    $row['data'][] = $verification->meta ? '<span title="' . check_plain(truncate_utf8($verification->meta, 48)) . '">' . t('Yes') . '</spam>' : t('No');
    $row['data'][] = $verification->file ? l($verification->file, $verification->file) : t('None');
    $operations = array();
    $operations['edit'] = array(
      'title' => t('Edit'),
      'href' => "admin/config/search/verifications/{$verification->svid}/edit",
      'query' => $destination,
    );
    $operations['delete'] = array(
      'title' => t('Delete'),
      'href' => "admin/config/search/verifications/{$verification->svid}/delete",
      'query' => $destination,
    );
    $row['data']['operations'] = array(
      'data' => array(
        '#theme' => 'links',
        '#links' => $operations,
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
    $rows[] = $row;
  }

  $build['verification_tabe'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No verifications available. <a href="@add">Add verification</a>.', array('@add' => url('admin/config/search/verifications/add'))),
  );
  //$build['verification_pager'] = array('#theme' => 'pager');
  return $build;
}

/**
 * Form to add/edit a verification record.
 *
 * Step 1: Choose ENGINE (Bing, Google, etc).
 * Step 2: Enter verification details (meta-tag or verification file).
 */
function site_verify_edit_form($form, &$form_state, $record = array(), $engine = NULL) {
  if (!isset($form_state['storage']['step'])) {
    // Add default placeholders.
    $record += array(
      'svid' => NULL,
      'file' => '',
      'file_contents' => t('This is a verification page.'),
      'meta' => '',
      'engine' => $engine,
    );
    $form_state['storage']['record'] = $record;
    $form_state['storage']['step'] = $record['engine'] ? 2 : 1;
  }
  else {
    $record = $form_state['storage']['record'];
  }

  $form['actions'] = array('#type' => 'actions');

  switch ($form_state['storage']['step']) {
    case 1:
      // Build  list of possible verification engines.
      $engines = site_verify_get_engines();
      $options = array();
      foreach ($engines as $key => $engine) {
        $options[$key] = $engine['name'];
      }
      asort($options);

      $form['engine'] = array(
        '#type' => 'select',
        '#title' => t('Search engine'),
        '#options' => $options,
      );
      $form['actions']['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
      );
      break;

    case 2:
      $form['svid'] = array(
        '#type' => 'value',
        '#value' => $record['svid'],
      );
      $form['engine'] = array(
        '#type' => 'value',
        '#value' => $record['engine']['key'],
      );
      $form['engine_name'] = array(
        '#type' => 'item',
        '#title' => t('Search engine'),
        '#value' => $record['engine']['name'],
      );
      $form['#engine'] = $record['engine'];

      $form['meta'] = array(
        '#type' => 'textfield',
        '#title' => t('Verification META tag'),
        '#default_value' => $record['meta'],
        '#description' => t('This is the full meta tag provided for verification. Note that this meta tag will only be visible in the source code of your <a href="@frontpage">front page</a>.', array('@frontpage' => url('<front>'))),
        '#element_validate' => $record['engine']['meta_validate'],
        '#access' => $record['engine']['meta'],
        '#maxlength' => NULL,
      );

      //@TODO: display errror or lock form if upload exists, but user doesn't have perm
      if (user_access('administer site verify uploads')) {
        $form['file_upload'] = array(
          '#type' => 'file',
          '#title' => t('Upload an existing verification file'),
          '#description' => t('If you have been provided with an actual file, you can simply upload the file.'),
          '#access' => $record['engine']['file'],
        );
        $form['file'] = array(
          '#type' => 'textfield',
          '#title' => t('Verification file'),
          '#default_value' => $record['file'],
          '#description' => t('The name of the HTML verification file you were asked to upload.'),
          '#element_validate' => $record['engine']['file_validate'],
          '#access' => $record['engine']['file'],
        );
        $form['file_contents'] = array(
          '#type' => 'textarea',
          '#title' => t('Verification file contents'),
          '#default_value' => $record['file_contents'],
          '#element_validate' => $record['engine']['file_contents_validate'],
          '#wysiwyg' => FALSE,
          '#access' => $record['engine']['file_contents'],
        );
      }

      // Add a warning message if Clean URLs is not available.
      if (!variable_get('clean_url', 0)) {
        drupal_set_message(t('Using verification files will not work if <a href="@clean-urls">clean URLs</a> are disabled.', array('@clean-urls' => url('admin/settings/clean-url'))), 'error', FALSE);
        $form['file']['#disabled'] = TRUE;
        $form['file_contents']['#disabled'] = TRUE;
        $form['file_upload']['#disabled'] = TRUE;
      }

      //@TODO: $record['engine']['file'] is always TRUE
      if ($record['engine']['file']) {
        $form['#attributes'] = array('enctype' => 'multipart/form-data');
      }

      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
        '#weight' => 10,
      );
      break;
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/search/verifications',
    '#title' => t('Cancel'),
    '#weight' => 15,
  );

  return $form;
}

/**
 * Validation callback; sanitize metatag and save the uploaded file
 */
function site_verify_edit_form_validate($form, &$form_state) {
  if ($form_state['storage']['step']) {
    // validate metatag
    if (isset($form_state['values']['meta']) && $form_state['values']['meta']) {
      $valid_metatag_set = FALSE;

      $html = $form_state['values']['meta'];
      $dom = new DOMDocument;
      $dom->loadHTML($html);
      //@TODO: If we only care about the first metatag... why foreach?  why not [0]?
      foreach ($dom->getElementsByTagName('meta') as $tag) {
        if ($tag->getAttribute('name') && $tag->getAttribute('content')) {
          $form_state['values']['meta'] = '<meta name="' . $tag->getAttribute('name') . '" content="' . $tag->getAttribute('content') . '" />';
          $valid_metatag_set = TRUE;
        }
      }

      if (!$valid_metatag_set) {
        form_set_error('meta', t('A valid metatag was not found'));
      }
    }

    //
    if (isset($form_state['values']['file']) && $form_state['values']['file']) {
      $values = &$form_state['values'];
      // Import the uploaded verification file.
      $validators = array('file_validate_extensions' => array());
      if ($file = file_save_upload('file_upload', $validators, FALSE, FILE_EXISTS_REPLACE)) {
        $contents = @file_get_contents($file->uri);
        file_delete($file);
        if ($contents === FALSE) {
          drupal_set_message(t('The verification file import failed, because the file %filename could not be read.', array('%filename' => $file->filename)), 'error');
        }
        else {
          $values['file'] = $file->filename;
          $values['file_contents'] = $contents;
          //drupal_set_message(t('The verification file <a href="@filename">@filename</a> was successfully imported.', array('@filename' => $file->filename)));
        }
      }

      // Confirm that the desired filename isn't already in use by another
      // verification.
      if ($values['file']) {
        $existing_file = db_query("SELECT svid FROM {site_verify} WHERE LOWER(file) = LOWER(:file) AND svid <> :svid", array(':file' => $values['file'], ':svid' => $values['svid']))->fetchField();
        if ($existing_file) {
          form_set_error('file', t('The file %filename is already being used in another verification.', array('%filename' => $values['file'])));
        }
      }
    }
    else {
      $form_state['values']['file_contents'] = NULL;
    }
  }

}

/**
 * Submission callback; send form to the next step or save the verification.
 */
function site_verify_edit_form_submit($form, &$form_state) {
  if ($form_state['storage']['step'] == 1) {
    // Send the form to step 2 (verification details).
    $form_state['storage']['record']['engine'] = site_verify_engine_load($form_state['values']['engine']);
    $form_state['storage']['step']++;
    $form_state['rebuild'] = TRUE;
  }
  else {
    // Save the verification to the database.
    if ($form_state['values']['svid']) {
      drupal_write_record('site_verify', $form_state['values'], array('svid'));
    }
    else {
      drupal_write_record('site_verify', $form_state['values']);
    }

    drupal_set_message(t('Verification saved.'));
    $form_state['storage'] = $form_state['rebuild'] = NULL;
    $form_state['redirect'] = 'admin/config/search/verifications';

    // Clear front page caches and set the menu to be rebuilt.
    cache_clear_all(url('<front>', array('absolute' => TRUE)), 'cache_page');
    cache_clear_all(url(variable_get('site_frontpage', 'node'), array('absolute' => TRUE)), 'cache_page');
    variable_set('menu_rebuild_needed', TRUE);
  }
}

/**
 * Delete form: found at admin/config/search/verifications/xxx/delete
 */
function site_verify_delete_form($form, $form_state, $record) {
  $form['record'] = array(
    '#type' => 'value',
    '#value' => $record,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the site verification for %engine?', array('%engine' => $record['engine']['name'])),
    'admin/config/search/verifications',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function site_verify_delete_form_submit($form, &$form_state) {
  $record = $form_state['values']['record'];
  db_delete('site_verify')->condition('svid', $record['svid'])->execute();
  drupal_set_message(t('Verification for %engine has been deleted.', array('%engine' => $record['engine']['name'])));
  watchdog('site_verify', 'Verification for %engine deleted.', array('%engine' => $record['engine']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/config/search/verifications';

  // Clear front page caches and set the menu to be rebuilt.
  cache_clear_all(url('<front>', array('absolute' => TRUE)), 'cache_page');
  cache_clear_all(url(variable_get('site_frontpage', 'node'), array('absolute' => TRUE)), 'cache_page');
  variable_set('menu_rebuild_needed', TRUE);
}


/**
 * Validate that the 'meta' property uses the correct pattern for Google.
 */
function site_verify_validate_meta_google($element, &$form_state) {
  $value = strtolower(trim($element['#value']));
  if ($value != '' && !preg_match('%\A<meta name="verify-v1" content="[\S]+" />\Z%', $value)) {
    form_error($element, t('Invalid verification meta tag.'));
  }
}

function site_verify_validate_page_google($element, &$form_state) {
  $value = strtolower(trim($element['#value']));
  if ($value != '' && !preg_match('%\Agoogle[\da-f]+\.html\Z%', $value)) {
    form_error($element, t('Invalid verification file.'));
  }
}
