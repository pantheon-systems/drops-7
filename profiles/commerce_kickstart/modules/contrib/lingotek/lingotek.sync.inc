<?php

/**
 * @file
 * Sync and management
 */
// ----  Other Functions

/**
 * Returns the site callback url.
 * */
function lingotek_get_notifications_url() {
  global $base_url;
  return $base_url . '/' . LINGOTEK_NOTIFICATIONS_URL;
}

/**
 * Registers the site translation notfication callback.
 * This URL will be called when a document translation is complete, and can be downloaded.
 *
 * Format:  ?doc_id={document_id}&target_code={target_language}&project_id={project_id}
 * */
function lingotek_notifications() {

  $document_id = ( isset($_GET['doc_id']) ) ? $_GET['doc_id'] : NULL;
  $lingotek_locale = ( isset($_GET['target_code']) ) ? $_GET['target_code'] : NULL;
  $project_id = ( isset($_GET['project_id']) ) ? $_GET['project_id'] : NULL;

  if (!isset($document_id) || !isset($lingotek_locale)) {
    return drupal_json_output(array("message" => "Missing Required Parameter(s).  Required: doc_id, target_code"));
  }

  // Adding a delay in the update.  Without the delay all the different language updates hit at once, causing node lock issues as multiple languages try to update the same node at once.
  $min = 0;
  $max = 3;
  $sleep = rand($min, $max);
  sleep($sleep);

  include_once('lingotek.batch.inc');
  $context = '';

  $target_drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
  $nid = LingotekSync::getNodeIdFromDocId($document_id);

  LingotekLog::trace('<h2>node: @node_id (@language_code)</h2>
      <strong>doc:</strong> @document_id
      <br /><strong>code:</strong> @language_code
      <br /><strong>project:</strong> @project_id', array(
    '@node_id' => $nid,
    '@document_id' => $document_id,
    '@language_code' => $lingotek_locale,
    '@project_id' => $project_id
      ), 'callback');

  if (!$nid) {
    // Look for and sync a comment if one is associated with the passed Lingotek Document ID.
    if (class_exists('LingotekComment')) {

      $source_language = lingotek_get_source_language(); //TO-DO: use the source_language of the comment?
      if ($comment = LingotekComment::loadByLingotekDocumentId($document_id, $source_language, $project_id)) {
        $replacements = array(
          '@id' => $comment->id,
          '@document' => $document_id,
          '@language_code' => $lingotek_locale,
          '@project_id' => $project_id,
        );

        if ($comment->updateLocalContent()) {
          LingotekLog::trace('Updated local content for <strong>comment</strong> @id based on hit
                from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, 'api');
        }
        else {
          LingotekLog::trace('Unable to update local content for <strong>comment</strong> @id based on hit
                from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, 'api');
        }
      }
    }
    else {
      LingotekLog::error('LingotekComment class not found. Please clear the Drupal cache to refresh the autoload registry');
    }
  }
  else {
    $node = node_load($nid);
    $source_language = $node->language;
    $node_setting = lingotek_lingonode($nid, 'sync_method');
    $auto_download = $node_setting !== FALSE ? $node_setting : variable_get('lingotek_sync', TRUE);

    if ($auto_download) { // download only when automatic download is enabled
      lingotek_sync_download_node_target($nid, $lingotek_locale, LingotekSync::STATUS_CURRENT, $context);
    }
  }

  $found = $nid || (isset($comment) && $comment) ? TRUE : FALSE;

  //print_r($comment);
  //Response
  $response = $found ? array_merge($_GET, array(
        'target_drupal_language_code' => $target_drupal_language_code,
        'source_language' => $source_language,
        'type' => isset($comment) ? 'comment' : 'node',
        'id' => isset($comment) ? $comment->cid : $nid,
        'found' => $found,
        'download' => $found && isset($comment) ? TRUE : (isset($auto_download) && $auto_download == TRUE)
      )) : array_merge($_GET, array(
        'found' => $found
      ));

  drupal_json_output($response);
}

/**
 * The main function responsible for syncing node/document translation.
 * 
 * */
function lingotek_sync() {
  $parameters = $_GET;
  $method = $_SERVER['REQUEST_METHOD'];
  $status = "200";
  $test = isset($parameters['test']) && $parameters['test'] && strcmp($parameters['test'], 'false') != 0 ? TRUE : FALSE;
  $request = array(
    'test' => $test,
    'method' => $method,
    'parameters' => $parameters
  );
  //$request['upload'] = $upload = isset($parameters['upload']) && ($parameters['upload'] == 0 || strcmp($parameters['upload'], 'false') == 0) ? false : true;
  //$request['download'] = $download = isset($parameters['download']) && ($parameters['download'] == 0 || strcmp($parameters['download'], 'false') == 0) ? false : true;
  $request['upload_nids_as_json'] = $upload_nids = isset($parameters['upload_nids']) ? json_decode($parameters['upload_nids']) : NULL;
  $request['download_targets_as_json'] = $download_targets = isset($parameters['download_targets_as_json']) ? json_decode($parameters['download_targets_as_json']) : NULL;

  if ($test) {
    lingotek_json_output_cors($request, $status, array('methods_allowed' => 'GET,POST'));
  }
  else {
    lingotek_sync_batch_create($download_targets, $upload_nids);
  }
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

function lingotek_form_bulk_sync() {
  $form = array();

  $report = LingotekSync::getReport();

  // Upload
  $form['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload content for translation'),
    //'#description' => t('Translation management defaults used when creating new nodes. At the node level, these settings can be adjusted.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#group' => 'administrative_settings',
  );
  $form['upload']['upload_nids_json'] = array('#type' => 'hidden', '#value' => json_encode($report['upload_nids']));

  $upload_count_total = $report['upload_nids_count'] + (isset($report['upload_nids_et_count']) ? $report['upload_nids_et_count'] : 0);
  if ($upload_count_total > 0) {
    $form['upload']['upload'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include new/modified nodes (<span id="upload-total">@count</span>)', array('@count' => $report['upload_nids_count'])),
      '#default_value' => $report['upload_nids_count'] == 0 ? 0 : 1,
      '#disabled' => $report['upload_nids_count'] == 0
    );
    if (module_exists('entity_translation')) {
      $form['upload']['upload_nids_et_json'] = array('#type' => 'hidden', '#value' => json_encode($report['upload_nids_et']));
      $form['upload']['upload_et'] = array(
        '#type' => 'checkbox',
        '#title' => t('Include nodes managed by Entity Translation (<span id="upload-et-total">@count</span>)', array('@count' => $report['upload_nids_et_count'])),
        '#default_value' => 0,
        '#disabled' => $report['upload_nids_et_count'] == 0,
        '#description' => t('These nodes will now be managed by Lingotek.')
      );
    }
  }
  else {
    $form['upload']['none'] = array(
      '#markup' => 'There are currently no new/modified nodes to be uploaded.'
    );
  }

  // Download  
  $form['download'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download translations'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#group' => 'vertical_tab',
  );
  $form['download']['download_targets_workflow_complete_json'] = array('#type' => 'hidden', '#value' => json_encode($report['download_targets_workflow_complete']));
  $form['download']['download_targets_workflow_incomplete_json'] = array('#type' => 'hidden', '#value' => json_encode($report['download_targets_workflow_incomplete']));

  $download_count_total = $report['download_targets_workflow_complete_count'] + (isset($report['download_targets_workflow_incomplete_count']) ? $report['download_targets_workflow_incomplete_count'] : 0);

  $available_targets = Lingotek::availableLanguageTargets(); //array_combine(Lingotek::availableLanguageTargets("lingotek_locale"),Lingotek::availableLanguageTargets("name"));
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
        $label,
        $locale_complete_count
      );
      $options_complete[$target->lingotek_locale] = $row;
    }
    if ($locale_incomplete_count > 0) {
      $row = array(
        $label,
        $locale_incomplete_count
      );
      $options_incomplete[$target->lingotek_locale] = $row;
    }
  }

  if (empty($options_complete) && empty($options_incomplete)) {
    $form['download']['none'] = array(
      '#markup' => 'There are currently no pending translations to be downloaded.'
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
    $form['download']['download_locales_incomplete'] = array(
      '#type' => 'tableselect',
      '#prefix' => t('Incomplete Translations'),
      '#header' => array(
        t('Language'),
        t('Translations')
      ),
      '#options' => $options_incomplete,
      '#default_value' => array_fill_keys(array_keys($options_complete), 0)
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Sync'),
    '#disabled' => ($upload_count_total == 0 && $download_count_total == 0),
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
  //dpm($form); //dpm($form_state);
  $vals = $form_state['values'];

  // upload
  $upload_nids = array();
  if (isset($vals['upload']) && $vals['upload'] && isset($vals['upload_nids_json'])) {
    $upload_nids = array_merge($upload_nids, json_decode($vals['upload_nids_json']));
  }
  if (isset($vals['upload_et']) && $vals['upload_et'] && isset($vals['upload_nids_et_json'])) {
    $upload_nids = array_merge($upload_nids, json_decode($vals['upload_nids_et_json']));
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
  $download_targets = array();
  if (!empty($download_locales_complete) && isset($vals['download_targets_workflow_complete_json'])) {
    $all_download_targets = json_decode($vals['download_targets_workflow_complete_json']);
    foreach ($all_download_targets as $download_target) {
      if (in_array($download_target->locale, $download_locales_complete)) {
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
        $download_targets_incomplete[] = $download_target;
      }
    }
  }
  //dpm($download_locales); dpm($download_targets); dpm($upload_nids); dpm($vals);
  lingotek_sync_batch_create($download_targets, $upload_nids, $download_targets_incomplete);
}