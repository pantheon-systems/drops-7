<?php

/**
 * @file
 * Sync and management
 */
// ----  Other Functions

/**
 * Generate and return the site callback url.
 * This URL will be called when a document translation is complete, and can be downloaded.
 * Format:  ?doc_id={document_id}&target_code={target_language}&project_id={project_id}&completed={completed}
 * */
function lingotek_notify_url_generate($security_token = NULL) {
  global $base_url;
  if (is_null($security_token)) {
    $security_token = variable_get('lingotek_notify_security_token', NULL);
    if (is_null($security_token)) {
      $security_token = md5(time());
      variable_set('lingotek_notify_security_token', $security_token);
    }
  }
  return $base_url . '/?q=' . LINGOTEK_NOTIFY_URL . '&doc_id={document_uuid}&doc_idx={document_id}&target_code={target_language}&completed={completed}&project_id={project_id}&security_token=' . $security_token;
}

/**
 * Update the Notify URL (via API)
 */
function lingotek_notify_url_update($show_messages = TRUE) {
  $integration_id = variable_get('lingotek_integration_id', NULL);
  $success = LingotekSync::updateNotifyUrl();
  if ($show_messages) {
    if ($success) {
      drupal_set_message(t("Notification callback URL successfully updated."));
    }
    else {
      drupal_set_message(t("Notification callback URL was not successfully updated."), 'error');
    }
  }
  return $success;
}

/**
 * Notify URL Check - call lingotek_notify_url_update when lingotek_notify_url no longer matches lingotek_notify_url_generate
 */
function lingotek_notify_url_check() {
  if (strcasecmp(variable_get('lingotek_notify_url', ''), lingotek_notify_url_generate()) != 0) {
    lingotek_notify_url_update(FALSE);
  }
}

function lingotek_get_trans_obj($document_id, $document_idx) {
  // Adding a delay in the update.  Without the delay all the different language updates hit at once, causing node lock issues as multiple languages try to update the same node at once.
  $min = 0;
  $max = 3;
  $sleep = rand($min, $max);
  sleep($sleep);
  $trans_obj = NULL;

  // try 3 times if by chance we don't find the document in the database yet..
  $attempts = 0;
  while ($attempts < 3) {
    list($id, $type) = LingotekSync::getEntityIdFromDocId($document_id);
    if (!$id) { // check for the old style document_id
      list($id, $type) = LingotekSync::getEntityIdFromDocId($document_idx);
      if ($id) {
        $document_id = $document_idx;
      }
    }
    
    if ($id) {
      $entity = lingotek_entity_load_single($type, $id);
      $trans_obj = LingotekEntity::load($entity, $type);
      break;
    }

    if ($trans_obj = LingotekConfigChunk::loadByLingotekDocumentId($document_id)) {
      break;
    }

    LingotekLog::info('Did not find doc ID @doc_id yet on attempt #@attempt, retrying...', 
        array('@attempt' => $attempts, '@doc_id' => $document_id));
    sleep(2);
    $attempts++;
  }
  if (!$trans_obj) {
    LingotekLog::error('Did not find doc ID @doc_id after all attempts.  Giving up.' , array('@doc_id' => $document_id));
  }
  return $trans_obj;
}

/**
 * Registers the site translation notfication callback.
 */
function lingotek_notifications() {

  drupal_page_is_cacheable(FALSE);
  LingotekLog::trace('Received <pre>@data</pre>', array('@data' => var_export($_GET, TRUE)), 'callback');

  $document_id = ( isset($_GET['doc_id']) ) ? $_GET['doc_id'] : NULL;// uuid
  $document_idx = ( isset($_GET['doc_idx']) ) ? $_GET['doc_idx'] : NULL;// this is the deprecated document number
  $lingotek_locale = ( isset($_GET['target_code']) ) ? $_GET['target_code'] : NULL;
  $project_id = ( isset($_GET['project_id']) ) ? $_GET['project_id'] : NULL;
  $completed = ( isset($_GET['completed']) ) ? $_GET['completed'] : 1;
  $security_token = ( isset($_GET['security_token']) ) ? $_GET['security_token'] : NULL;

  $stored_security_token = variable_get('lingotek_notify_security_token', NULL);

  if (!is_null($stored_security_token)) { // only enforce security token matching if set as variable
    if (strcmp($security_token, $stored_security_token) != 0) {
      return drupal_json_output(array("message" => "Invalid security token"));
    }
  }

  if ((!isset($document_id) && !isset($document_idx)) || !isset($lingotek_locale)) {
    return drupal_json_output(array("message" => "Missing Required Parameter(s).  Required: doc_id, target_code"));
  }

  include_once('lingotek.batch.inc');

  $target_drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
  
  $trans_obj = lingotek_get_trans_obj($document_id, $document_idx);

  $downloaded = FALSE;

  if ($trans_obj) {
    $trans_obj->preDownload($lingotek_locale, $completed);
    
    $replacements = array(
      '@trans_type' => get_class($trans_obj),
      '@document' => $document_id,
      '@language_code' => $lingotek_locale,
      '@project_id' => $project_id,
      '@id' => $trans_obj->getId(),
    );
    
    if ($downloaded = $trans_obj->downloadTriggered($lingotek_locale)) {
      LingotekLog::trace('Updated local content for <strong>@trans_type</strong> @id based on hit
            from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, 'api');
    }
    else {
      LingotekLog::trace('Unable to update local content for <strong>@trans_type</strong> @id based on hit
            from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, 'api');
    }
    
    $trans_obj->postDownload($lingotek_locale, $completed);
    
  }
  else {
    LingotekLog::error('Lingotek document ID (@doc_id) not found.', array('@doc_id' => $document_id));
    return drupal_json_output(array("message" => "The doc_id was not found on the site."));
  }

  LingotekLog::info('[notify] <br/><b>code:</b> @lingotek_locale <br/><b>doc_id:</b> @document_id<br/><b>project:</b> @project_id <br/><b>entity:</b> @entity_type #@entity_id (@target_drupal_language_code) <br/><b>completed</b>: @completed', array(
    '@document_id' => $document_id,
      '@lingotek_locale' => $lingotek_locale,
      '@project_id' => $project_id,
      '@target_drupal_language_code' => $target_drupal_language_code,
      '@entity_type' => isset($trans_obj) ? $trans_obj->getEntityType() : '',
      '@entity_id' => isset($trans_obj) ? $trans_obj->getId() : '',
      '@completed' => $completed
        ), 'callback');

  $found = (isset($trans_obj) && $trans_obj);

  $response = array_merge($_GET, array(
        'target_drupal_language_code' => $target_drupal_language_code,
        'type' => isset($trans_obj) ? $trans_obj->getEntityType() : '',
        'id' => isset($trans_obj) ? $trans_obj->getId() : '',
        'found' => $found,
        'download' => $downloaded,
      ));

  return drupal_json_output($response);
}

/**
 * The API endpoint for bulk translation management
 */
function lingotek_sync_endpoint() {
  $parameters = array();
  $method = $_SERVER['REQUEST_METHOD'];
  $status = "200";
  $request = array(
    'method' => $method,
  );
  $response = array();

  switch ($method) {
    case 'GET':
      $request['parameters'] = $parameters = $_GET;
      /* $request['doc_ids'] = $document_ids = isset($parameters['doc_ids']) ? array_map(function($val) {
        return trim($val);
        }, explode(',', $parameters['doc_ids'])) : array(); */
      $response = LingotekSync::getReport();
      break;
    case 'POST': case 'PUT': case 'DELETE':
    default:
      parse_str(file_get_contents("php://input"), $parameters);
      $status = "405 Method Not Allowed";
      break;
  }

  return lingotek_json_output_cors($response, $status, array('methods_allowed' => 'GET'));
}

function lingotek_form_bulk_sync($form, $form_state) {

  global $base_url;
  module_load_include('admin.inc', 'lingotek');
  $submit_functions = array('lingotek_form_bulk_sync_submit');
  $report = LingotekSync::getReport();
  $upload_count_total = isset($report['upload_config_count']) ? $report['upload_config_count'] : 0;
  // Upload
  $form['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload config for translation'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#group' => 'administrative_settings',
    '#prefix' => '<div id="upload_wrapper">',
    '#suffix' => '</div>',
  );


  if (variable_get('lingotek_translate_config') && $upload_count_total > 0) {
    // pre-build include/exclude messages based on enabled config types
    $config_types = array(
      'lingotek_translate_config_builtins' => t('Include built-in interface data'),
      'lingotek_translate_config_blocks' => t('blocks'),
      'lingotek_translate_config_menus' => t('menus'),
      'lingotek_translate_config_views' => t('views'),
      'lingotek_translate_config_taxonomies' => t('taxonomy vocabularies & terms'),
      'lingotek_translate_config_fields' => t('field labels'),
      'lingotek_translate_config_misc' => t('other miscellaneous strings'),
    );
    $config_types_included = array();
    $config_types_excluded = array();
    foreach ($config_types as $k => $v) {
      if (variable_get($k, 0)) {
        $config_types_included[$k] = $v;
      }
      else {
        $config_types_excluded[$k] = $v;
      }
    }
    $cf_included_str = implode(', ', $config_types_included);
    if (count($config_types_included) > 1) {
      $cf_included_str = substr_replace($cf_included_str, ' ' . t('and') . ' ' . substr($cf_included_str, strrpos($cf_included_str, ', ') + 2), strrpos($cf_included_str, ', '));
    }
    $cf_excluded_str = implode(', ', $config_types_excluded);
    if (count($config_types_excluded) > 1) {
      $cf_excluded_str = substr_replace($cf_excluded_str, ' ' . t('and') . ' ' . substr($cf_excluded_str, strrpos($cf_excluded_str, ', ') + 2), strrpos($cf_excluded_str, ', '));
    }
    $form['upload']['upload_config_json'] = array('#type' => 'hidden', '#value' => json_encode($report['upload_config']));
    $form['upload']['upload_config'] = array(
      '#type' => 'checkbox',
      '#title' => $cf_included_str 
          . ' (<span id="upload-config-total">' 
          . $report['upload_config_count'] . ' ' 
          . format_plural($report['upload_config_count'], 'set', 'sets') 
          . ' ' . t('of additions/changes') . '</span>)',
      '#default_value' => $report['upload_config_count'] == 0 ? 0 : 1,
      '#disabled' => $report['upload_config_count'] == 0,
    );
    $additional_desc = '';
    if (count($config_types_excluded)) {
      $additional_desc .= t('(You can also enable translation for')
          . ' ' . $cf_excluded_str . ' '
          . t('within') . ' ' . t('Translation Configuration in') . ' '
          . l(t('Lingotek Settings'), 'admin/settings/lingotek/settings')
          . ')<br/>';
    }
    if (variable_get('lingotek_use_translation_from_drupal')) {
      $modules = lingotek_admin_get_enabled_modules();
      $updates = lingotek_check_for_l10n_translations($modules);
      if (!empty($updates)) {
        $additional_desc .= format_plural(
            count($updates), 
          'You have 1 pending localization import from Drupal included in this step.', 
          'You have @count pending localization imports from Drupal included in this step.');
      }
    }
    if ($additional_desc) {
      $form['upload']['upload_config']['#description'] = $additional_desc;
    }
  }
  else {
    $form['upload']['none'] = array(
      '#markup' => t('There are currently no new/modified items to be uploaded.'),
    );
  }

  // Download  
  $form['download'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download config translations'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#group' => 'vertical_tab',
  );
  $form['download']['download_targets_workflow_complete_json'] = array('#type' => 'hidden', '#value' => json_encode($report['download_targets_workflow_complete']));
  $form['download']['download_targets_workflow_incomplete_json'] = array('#type' => 'hidden', '#value' => json_encode($report['download_targets_workflow_incomplete']));

  $download_count_total = $report['download_targets_workflow_complete_count'] + (isset($report['download_targets_workflow_incomplete_count']) ? $report['download_targets_workflow_incomplete_count'] : 0);

  $available_targets = Lingotek::getLanguages();
  $options_complete = array(); // locale => label
  $options_incomplete = array();

  foreach ($available_targets as $target) {
    $label = $target->name . ' / ' . $target->native . ' (' . $target->lingotek_locale . ')';
    $locale_complete_count = 0;
    foreach ($report['download_targets_workflow_complete'] as $download_target) {
      if ($target->lingotek_locale == $download_target['locale']) {
        $locale_complete_count++;
      }
    }
    $locale_incomplete_count = 0;
    foreach ($report['download_targets_workflow_incomplete'] as $download_target) {
      if ($target->lingotek_locale == $download_target['locale']) {
        $locale_incomplete_count++;
      }
    }
    if ($locale_complete_count > 0) {
      $row = array(
        array('data' => $label, 'width' => '20%'),
        array('data' => $locale_complete_count, 'width' => '80%'),
      );
      $options_complete[$target->lingotek_locale] = $row;
    }
    if ($locale_incomplete_count > 0) {
      $row = array(
        array('data' => $label, 'width' => '20%'),
        array('data' => $locale_incomplete_count, 'width' => '80%'),
      );
      $options_incomplete[$target->lingotek_locale] = $row;
    }
  }

  if (empty($options_complete) && empty($options_incomplete)) {
    $form['download']['none'] = array(
      '#markup' => t('There are currently no pending translations to be downloaded.'),
    );
  }


  if (!empty($options_complete)) {
    $form['download']['download_locales_complete'] = array(
      '#type' => 'tableselect',
      '#prefix' => t('Completed Translations'),
      '#header' => array(
        t('Language'),
        t('Translations')
      ),
      '#options' => $options_complete,
      '#default_value' => array_fill_keys(array_keys($options_complete), 1)
    );
  }
  if (!empty($options_incomplete)) {
    /* $form['download']['download_locales_incomplete_check'] = array(
      '#type' => 'checkbox',
      '#title' => t('Download Incomplete Translations'),
      '#default_value' => 0,
      ); */
    $form['download']['download_locales_incomplete'] = array(
      '#type' => 'tableselect',
      '#prefix' => t('Incomplete Translations'),
      '#header' => array(
        t('Language'),
        t('Translations')
      ),
      '#options' => $options_incomplete,
      '#default_value' => array_fill_keys(array_keys($options_complete), 0),
        /* '#states' => array(
          'visible' => array(
          ':input[name="download_locales_incomplete_check"]' => array('checked' => TRUE),
          ),
          ), */
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Sync'),
    '#disabled' => ($upload_count_total == 0 && $download_count_total == 0),
    '#submit' => $submit_functions,
  );

  $form['force_download'] = array(
    '#type' => 'submit',
    '#value' => t('Force Download All'),
    '#submit' => $submit_functions,
  );

  return $form;
}

/**
 * Submit handler for the lingotek_form_bulk_sync form.
 * Calls the function that creates a batch job to do bulk sync.
 *
 * @param array $form
 *   The FAPI form array.
 * @param array $form_state
 *   The FAPI form state array.
 */
function lingotek_form_bulk_sync_submit($form, $form_state) {

  $extra_operations = array();
  $upload_config = array();
  $download_targets_incomplete = array();
  $vals = $form_state['values'];

  if ($vals['op'] == 'Force Download All') {
    $document_ids = LingotekConfigChunk::getAllDocumentIds();
    $available_targets = Lingotek::getLanguages();
    unset($available_targets['en_US']); // all locales_source is assumed by i18n modules to be English
    $download_targets = array();
    foreach ($document_ids as $doc_id) {
      foreach (array_keys($available_targets) as $lang) {
        $target = new stdClass();
        $target->document_id = $doc_id;
        $target->locale = $lang;
        $download_targets[] = $target;
      }
    }
    lingotek_sync_batch_create($upload_config, $download_targets, $download_targets_incomplete, NULL, $extra_operations);
    return;
  }

  // upload config
  $upload_config = array();
  if (isset($vals['upload_config']) && $vals['upload_config'] && isset($vals['upload_config_json'])) {
    $upload_config = json_decode($vals['upload_config_json']);
  }

  //download - complete
  $download_locales_complete = array();
  if (isset($vals['download_locales_complete'])) {
    foreach ($vals['download_locales_complete'] as $locale => $enabled) {
      if ($enabled) {
        $download_locales_complete[] = $locale;
      }
    }
  }
  $document_ids = array();
  $download_targets = array();
  if (!empty($download_locales_complete) && isset($vals['download_targets_workflow_complete_json'])) {
    $all_download_targets = json_decode($vals['download_targets_workflow_complete_json']);
    foreach ($all_download_targets as $download_target) {
      if (in_array($download_target->locale, $download_locales_complete)) {
        $document_ids[] = $download_target->document_id;
        $download_targets[] = $download_target;
      }
    }
  }

  //download - incomplete
  $download_locales_incomplete = array();
  if (isset($vals['download_locales_incomplete'])) {
    foreach ($vals['download_locales_incomplete'] as $locale => $enabled) {
      if ($enabled) {
        $download_locales_incomplete[] = $locale;
      }
    }
  }
  $download_targets_incomplete = array();
  if (!empty($download_locales_incomplete) && isset($vals['download_targets_workflow_incomplete_json'])) {
    $all_download_targets = json_decode($vals['download_targets_workflow_incomplete_json']);
    foreach ($all_download_targets as $download_target) {
      if (in_array($download_target->locale, $download_locales_incomplete)) {
        $document_ids[] = $download_target->document_id;
        $download_targets_incomplete[] = $download_target;
      }
    }
  }

  $document_ids = array_unique($document_ids);

  lingotek_sync_batch_create($upload_config, $download_targets, $download_targets_incomplete, NULL, $extra_operations);
}

/**
* Updates the 'target_sync_status_[lang-code]' field for every target in the lingotek table
* with the overall progress returned by TMS
*
* @param int array $document_ids
*    array of Document IDs that you want to update
*
*/
function lingotek_get_and_update_target_progress($entity_type, $document_ids) {
  $api = LingotekApi::Instance();
  if (empty($document_ids)) {
    return;
  }
  if (!is_array($document_ids)) {
    $document_ids = array($document_ids);
  }
  $project_id = variable_get('lingotek_project', NULL);

  $progress_report = $api->getProgressReport($project_id, $document_ids);

  $targets_count = LingotekSync::getTargetCountByDocumentIds($document_ids);
  
  if (isset($progress_report) && $progress_report->results == 'success') {
    $delete_nids_maybe = array();
    $entity_values = array();
    $trans_obj = NULL;

    if (!empty($progress_report->errors)) {
      foreach (get_object_vars($progress_report->errors) as $doc_id => $error) {
        list($entity_id, $entity_type) = LingotekSync::getEntityIdFromDocId($doc_id, $entity_type);
        switch ($error->status) {
          case 'IN_QUEUE':
            lingotek_keystore($entity_type, $entity_id, 'node_sync_status', LingotekSync::STATUS_PENDING);
            break;
          case 'NOT_FOUND':
          default:
            lingotek_keystore($entity_type, $entity_id, 'node_sync_status', LingotekSync::STATUS_FAILED);
            lingotek_keystore($entity_type, $entity_id, 'last_sync_error', substr($error, 0, 255));
            LingotekLog::error('Received unexpected error status (@status) from Lingotek for @entity_type #@id: <pre>@error</pre>', array('@status' => $error->status, '@entity_type' => $entity_type, '@id' => $entity_id, '@error' => $error));
        }
      }
    }
    foreach ($progress_report->byDocumentIdAndTargetLocale as $doc_id => $completion) {
      list($entity_id, $entity_type_from_table) = LingotekSync::getEntityIdFromDocId($doc_id);
      if (!$entity_id) {
        LingotekLog::error("Lingotek doc ID '@doc_id' not found", array('@doc_id' => $doc_id));
        continue;
      }
      else {
        $delete_nids_maybe[] = $entity_id;
        $target_number = isset($targets_count[$entity_id]->targets) ? $targets_count[$entity_id]->targets : 0;
      }
      foreach ($completion as $language => $percent) {
        $status = LingotekSync::getTargetStatus($doc_id, $language);
        if (isset($progress_report->workflowCompletedByDocumentIdAndTargetLocale->$doc_id->$language)) {
          if ($progress_report->workflowCompletedByDocumentIdAndTargetLocale->$doc_id->$language) { // If the workflow is complete
            if ($status != LingotekSync::STATUS_CURRENT) { // If the status is not current
              $to_status = LingotekSync::STATUS_READY; // Set it to ready
            }
            else {
              $to_status = LingotekSync::STATUS_CURRENT; // Otherwise keep it at current
            }
          }
          else { // If the workflow is not complete
            $to_status = LingotekSync::STATUS_PENDING; // Set it to pending
          }
          $entity_values[] = array($entity_type, $entity_id, 'target_sync_status_' . $language, $to_status);
        }
      }
      // update source status when necessary
      $entity_source_status = lingotek_keystore($entity_type, $entity_id, 'node_sync_status');
      if ($entity_source_status == LingotekSync::STATUS_FAILED || $entity_source_status == LingotekSync::STATUS_PENDING) {
        lingotek_keystore($entity_type, $entity_id, 'node_sync_status', LingotekSync::STATUS_CURRENT);
      }
    }

    // merge status info for entities
    foreach ($entity_values as $record) {
      $entity_id = (isset($record['entity_id']) ? $record['entity_id'] : $record[1]);
      $entity_key = (isset($record['entity_key']) ? $record['entity_key'] : $record[2]);
      $value = (isset($record['value']) ? $record['value'] : $record[3]);
      $query = db_merge('{lingotek_entity_metadata}')
          ->key(array('entity_id' => $entity_id, 'entity_type' => $entity_type, 'entity_key' => $entity_key))
        ->fields(array(
            'entity_type' => $entity_type,
            'entity_id' => $entity_id,
            'entity_key' => $entity_key,
            'value' => $value,
          ))
        ->execute();
    }
    return $progress_report;
  }
  else {
    $error_message = t("API Error(s):") . " <ul>";
    if (is_array($progress_report->errors)) {
      foreach ($progress_report->errors as $error) {
        $error_message .= "<li>" . $error . "</li>";
      }
    }
    $error_message .= "</ul><i>" . t('For additional information, check your <a href="@link">recent log messages</a>', array('@link' => url('admin/reports/dblog'))) . "</i>";
    drupal_set_message($error_message, 'error');
  }
}


/**
* Updates the 'target_sync_status_[lang-code]' field for every target in the
* lingotek_config_metadata table with the overall progress returned by TMS
*
* @param int array $document_ids
*    array of Document IDs to be updated
*
*/
function lingotek_update_config_progress($document_ids) {
  $api = LingotekApi::Instance();
  if (empty($document_ids)) {
    return;
  }
  if (!is_array($document_ids)) {
    $document_ids = array($document_ids);
  }
  $project_id = variable_get('lingotek_project', NULL);

  $progress_report = $api->getProgressReport($project_id, $document_ids);

  if (isset($progress_report) && $progress_report->results == 'success') {
    $cids = array();
    $cfg_values = array();
    $trans_obj = NULL;
    
    if (isset($progress_report->errors)) {
      foreach (get_object_vars($progress_report->errors) as $doc_id => $error) {
        $chunk = LingotekConfigChunk::loadByLingotekDocumentId($doc_id);
        switch ($error->status) {
          case 'IN_QUEUE':
            $chunk->setMetadataValue('chunk_sync_status', LingotekSync::STATUS_PENDING);
            break;
          case 'NOT_FOUND':
          default:
            $chunk->setMetadataValue('chunk_sync_status', LingotekSync::STATUS_FAILED);
            LingotekLog::error('Received unexpected error status (@status) from Lingotek for config chunk #@id: <pre>@error</pre>', array('@status' => $error->status, '@id' => $chunk, '@error' => $error));
        }
      }
    }
    
    foreach ($progress_report->byDocumentIdAndTargetLocale as $doc_id => $completion) {
      $trans_obj = LingotekConfigChunk::loadByLingotekDocumentId($doc_id);
      if (!$trans_obj) {
        LingotekLog::error("Lingotek doc ID '@doc_id' not found", array('@doc_id' => $doc_id));
        continue;
      }
   
      foreach ($completion as $language => $percent) {
        $status = LingotekSync::getTargetStatus($doc_id, $language);
        if (isset($progress_report->workflowCompletedByDocumentIdAndTargetLocale->$doc_id->$language)) {
          if ($progress_report->workflowCompletedByDocumentIdAndTargetLocale->$doc_id->$language) { // If the workflow is complete
            if ($status != LingotekSync::STATUS_CURRENT) { // If the status is not current
              $to_status = LingotekSync::STATUS_READY; // Set it to ready
            }
            else {
              $to_status = LingotekSync::STATUS_CURRENT; // Otherwise keep it at current
            }
          }
          else { // If the workflow is not complete
            $to_status = LingotekSync::STATUS_PENDING; // Set it to pending
          }
          $cfg_values[] = array($trans_obj->cid, 'target_sync_status_' . $language, $to_status);
        }
      }
    }

    // insert status info for config
    foreach ($cfg_values as $record) {
      $query = db_merge('{lingotek_config_metadata}')
          ->key(array('id' => $record[0], 'config_key' => $record[1]))
        ->fields(array(
            'id' => $record[0],
            'config_key' => $record[1],
            'value' => $record[2]
          ))
        ->execute();
    }
    return $progress_report;
  }
  else {
    $error_message = t("API Error(s):") . " <ul>";
    if (is_array($progress_report->errors)) {
      foreach ($progress_report->errors as $error) {
        $error_message .= "<li>" . $error . "</li>";
      }
    }
    $error_message .= "</ul><i>" . t('For additional information, check your <a href="@link">recent log messages</a>', array('@link' => url('admin/reports/dblog'))) . "</i>";
    drupal_set_message($error_message, 'error');
  }

}
