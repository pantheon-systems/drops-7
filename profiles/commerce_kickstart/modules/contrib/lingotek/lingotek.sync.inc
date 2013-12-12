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

/**
 * Registers the site translation notfication callback.
 */
function lingotek_notifications() {

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

  // Adding a delay in the update.  Without the delay all the different language updates hit at once, causing node lock issues as multiple languages try to update the same node at once.
  $min = 0;
  $max = 3;
  $sleep = rand($min, $max);
  sleep($sleep);

  include_once('lingotek.batch.inc');
  $context = '';

  $target_drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
  
  // try 3 times if by chance we don't find the document in the database yet..
  $attempts = 0;
  while ($attempts < 3) {
    $nid = LingotekSync::getNodeIdFromDocId($document_id);
    if (!$nid) { // check for the old style document_id
      $nid = LingotekSync::getNodeIdFromDocId($document_idx);
      if ($nid) {
        $document_id = $document_idx;
      }
    }

    LingotekLog::info('<b>code:</b> @lingotek_locale <br/><b>doc_id:</b> @document_id<br/><b>project:</b> @project_id <br/><b>node:</b> @node_id (@target_drupal_language_code) <br/><b>completed</b>: @completed', array(
        '@document_id' => $document_id,
        '@lingotek_locale' => $lingotek_locale,
        '@project_id' => $project_id,
        '@node_id' => $nid ? $nid : "NA",
        '@target_drupal_language_code' => $target_drupal_language_code,
        '@completed' => $completed
          ), 'callback');

    // If there is not a node ID associated with this document, check for config chunks and comments
    // associated with the document ID
    if (!$nid) {
      $source_language = lingotek_get_source_language(); //TO-DO: use the source_language of the object?
      $replacements = array(
        '@document' => $document_id,
        '@language_code' => $lingotek_locale,
        '@project_id' => $project_id,
      );

      // Look for and sync a config chunk if one is associated with the passed Lingotek Document ID.
      if ($trans_obj = LingotekConfigChunk::loadByLingotekDocumentId($document_id, $source_language, $project_id)) {
        $replacements['@id'] = $trans_obj->cid;
        $replacements['@trans_type'] = "ConfigChunk";
      }
      // Look for and sync a comment if one is associated with the passed Lingotek Document ID.
      elseif ($trans_obj = LingotekComment::loadByLingotekDocumentId($document_id, $source_language, $project_id)) {
        $replacements['@id'] = $trans_obj->id;
        $replacements['@trans_type'] = "Comment";
      }
      else {
        if ($attempts < 10) {
          LingotekLog::info('Did not find doc ID @doc_id yet on attempt #@attempt, retrying...', 
              array('@attempt' => $attempts, '@doc_id' => $document_id));
          sleep(2);
          $attempts++;
          continue;
        }
        LingotekLog::error('Lingotek document ID (@doc_id) not found.', array('@doc_id' => $document_id));
        return drupal_json_output(array("message" => "The doc_id was not found on the site."));
      }

      if ($trans_obj->updateLocalContentByTarget($lingotek_locale)) {
        LingotekLog::trace('Updated local content for <strong>@trans_type</strong> @id based on hit
              from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, 'api');
      }
      else {
        LingotekLog::trace('Unable to update local content for <strong>@trans_type</strong> @id based on hit
              from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, 'api');
      }
    }
    else {
      $node = lingotek_node_load_default($nid);
      $source_language = $node->language;
      $node_setting = $node->lingotek['sync_method'];
      $auto_download = $node_setting !== FALSE ? $node_setting : variable_get('lingotek_sync', TRUE);
      if ($completed) {
        LingotekSync::setTargetStatus($nid, $lingotek_locale, LingotekSync::STATUS_READY);
      }

      if ($auto_download) { // download only when automatic download is enabled
        $status = $completed ? LingotekSync::STATUS_CURRENT : LingotekSync::STATUS_PENDING;
        lingotek_sync_download_node_target($nid, $lingotek_locale, $status, TRUE, $context);

        // if workbench moderation is enabled for this node, and node should be moderated, moderate it based on the options
        $target_statuses = LingotekSync::getAllTargetStatusNotCurrent($nid);
        if (empty($target_statuses)) {
          lingotek_lingonode($nid, 'workbench_moderate', 1);
        }
        if (module_exists('workbench_moderation') && isset($node->workbench_moderation) && lingotek_lingonode($nid, 'workbench_moderate') == 1) {
          $options_index = $node->lingotek['sync_method_workbench_moderation'];
          $trans_options = lingotek_get_workbench_moderation_transitions();
          if ($options_index == 1) {
            $from_state = $node->workbench_moderation['current']->state;
            $mult_trans = variable_get('lingotek_sync_wb_select_' . $from_state, NULL);
            if ($mult_trans) {
              $trans_options[$from_state] = $mult_trans;
            }
          }
          lingotek_workbench_moderation_moderate($nid, $options_index, $trans_options); // moderate if all languages have been downloaded
          lingotek_lingonode_variable_delete($nid, 'workbench_moderate');
        }
      }
      
      if (!$completed) {
        lingotek_get_and_update_target_progress($document_id);
      }
      else {
        lingotek_lingonode($nid, 'target_sync_progress_' . $lingotek_locale, 100);
        
        $query = db_select('lingotek');
        $query->addExpression('AVG(lingovalue)');
        
        $total = $query->condition('lingokey', 'target_sync_progress_%', 'LIKE')
          ->condition('nid', $nid)
          ->execute()->fetchField();
        lingotek_lingonode($nid, 'translation_progress', $total);
          
        lingotek_lingonode($nid, 'target_sync_last_progress_updated_' . $lingotek_locale, time());
      }
      // clear any caching from entitycache module to allow the new translation to show immediately
      if (module_exists('entitycache')) {
        cache_clear_all($nid, 'cache_entity_node');
      }
    }
    break; // if the function did not continue, then the doc ID was found.
  }

  $found = $nid || (isset($trans_obj) && $trans_obj) ? TRUE : FALSE;

  $response = $found ? array_merge($_GET, array(
        'target_drupal_language_code' => $target_drupal_language_code,
        'source_language' => $source_language,
        'type' => isset($trans_obj) ? $trans_obj->getEntityType() : 'node',
        'id' => isset($trans_obj) ? $trans_obj->getId() : $nid,
        'found' => $found,
        'download' => $found && isset($trans_obj) && $trans_obj->getEntityType() == 'comment' ? TRUE : (isset($auto_download) && $auto_download == TRUE)
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
  $request['upload_nids_as_json'] = $upload_nids = isset($parameters['upload_nids']) ? json_decode($parameters['upload_nids']) : NULL;
  $request['upload_config_as_json'] = $upload_config = isset($parameters['upload_config']) ? json_decode($parameters['upload_config']) : NULL;
  $request['download_targets_as_json'] = $download_targets = isset($parameters['download_targets_as_json']) ? json_decode($parameters['download_targets_as_json']) : NULL;

  if ($test) {
    lingotek_json_output_cors($request, $status, array('methods_allowed' => 'GET,POST'));
  }
  else {
    lingotek_sync_batch_create($upload_nids, $upload_config, $download_targets);
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

function lingotek_form_bulk_sync($form, $form_state) {

  global $base_url;
  module_load_include('admin.inc', 'lingotek');
  $submit_functions = array('lingotek_form_bulk_sync_submit');
  $report = LingotekSync::getReport();
  $upload_count_total = $report['upload_nids_count'] + (isset($report['upload_nids_et_count']) ? $report['upload_nids_et_count'] : 0) + (isset($report['upload_config_count']) ? $report['upload_config_count'] : 0);
  // Upload
  $form['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload content for translation'),
    //'#description' => t('Translation management defaults used when creating new nodes. At the node level, these settings can be adjusted.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#group' => 'administrative_settings',
    '#prefix' => '<div id="upload_wrapper">',
    '#suffix' => '</div>',
  );


  if ($upload_count_total > 0) {
    if (module_exists('workbench_moderation')) {

      // Get nodes that correspond to the upload state in $form['upload']['upload_workbench_moderation']
      lingotek_workbench_moderation_bulk_select($form, $form_state, $report);

      $options_upload = array();
      foreach ($report['workbench_moderation_states'] as $state) {
        if (isset($report['upload_nids_wb_count_' . $state]) && $report['upload_nids_wb_count_' . $state] > 0) {
          $form['upload']['upload_nids_wb_json_' . $state] = array('#type' => 'hidden', '#value' => json_encode($report['upload_nids_wb_' . $state]));
          $options_upload[$state] = array(
            array('data' => $state, 'width' => '20%'),
            array('data' => $report['upload_nids_wb_count_' . $state], 'width' => '80%')
          );
        }
      }
      if (!empty($options_upload)) {
        $form['upload']['upload_wb'] = array(
          '#type' => 'tableselect',
          '#prefix' => t('Select nodes to upload based on their Workbench Moderation state'),
          '#header' => array(
            t('State'),
            t('New / Modified Nodes'),
          ),
          '#options' => $options_upload,
          '#default_value' => array_fill_keys(array_keys($options_upload), 0),
        );
      }
      $form['upload']['upload_nids_nowb_json'] = array('#type' => 'hidden', '#value' => json_encode($report['upload_nids_nowb']));
      if ($report['upload_nids_nowb_count'] > 0) {
        $form['upload']['upload_nowb'] = array(
          '#type' => 'checkbox',
          '#title' => t('Include new/modified nodes not using Workbench Moderation (<span id="upload-total-nowb">@count</span>)', array('@count' => $report['upload_nids_nowb_count'])),
          '#default_value' => $report['upload_nids_nowb_count'] == 0 ? 0 : 1,
          '#disabled' => $report['upload_nids_nowb_count'] == 0,
        );
      }
    }
    else {
      $form['upload']['upload_nids_json'] = array('#type' => 'hidden', '#value' => json_encode($report['upload_nids']));
      if ($report['upload_nids_count'] > 0) {
        $form['upload']['upload'] = array(
          '#type' => 'checkbox',
          '#title' => t('Include new/modified nodes (<span id="upload-total">@count</span>)', array('@count' => $report['upload_nids_count'])),
          '#default_value' => $report['upload_nids_count'] == 0 ? 0 : 1,
          '#disabled' => $report['upload_nids_count'] == 0,
        );
      }
    }
    if (variable_get('lingotek_translate_config')) {
      // pre-build include/exclude messages based on enabled config types
      $config_types = array(
        'lingotek_translate_config_builtins' => t('Include built-in interface data'),
        'lingotek_translate_config_blocks' => t('blocks'),
        'lingotek_translate_config_menus' => t('menus'),
        'lingotek_translate_config_views' => t('views'),
        'lingotek_translate_config_taxonomies' => t('taxonomy vocabularies & terms'),
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
  }
  else {
    $form['upload']['none'] = array(
      '#markup' => t('There are currently no new/modified items to be uploaded.'),
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

  $available_targets = Lingotek::availableLanguageTargets();
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
  if (!empty($options_complete) || !empty($options_incomplete)) {
    if (module_exists('workbench_moderation')) {
      $transition_select = lingotek_workbench_moderation_get_mult_transitions();

      $form['download']['download_locales_wb_options'] = array(
        '#type' => 'select',
        '#title' => 'WORKBENCH MODERATION',
        '#field_suffix' => 'after translations have downloaded.',
        '#options' => lingotek_get_workbench_moderation_options(),
        '#default_value' => variable_get('lingotek_sync_workbench_moderation', 'no_moderation'),
        '#description' => t("Transitions will not occur until <i>all</i> of a node's translations have downloaded."),
      );

      if (!empty($transition_select)) { // select options for multiple possible transitions
        $form['download']['download_locales_wb'] = array(
          '#type' => 'container',
          '#states' => array(
            'visible' => array(
              'select[name="download_locales_wb_options"]' => array('value' => 'increment'),
            ),
          ),
        );
        $default_override = variable_get('lingotek_sync', 0) ? 0 : 1;
        $form['download']['download_locales_wb']['wb_override_defaults'] = array(
          '#type' => 'checkbox',
          '#prefix' => t('<b>Your configuration of the Workbench Moderation module has multiple transition paths.</b>'),
          '#title' => 'Override defaults',
          '#description' => t('Recommended if defaults have not been set in admin->lingotek->settings->content defaults'),
          '#default_value' => $default_override,
        );
        $form['download']['download_locales_wb']['wb_select'] = array(
          '#type' => 'container',
          '#states' => array(
            'invisible' => array(
              ':input[name="wb_override_defaults"]' => array('checked' => FALSE),
            ),
          ),
        );
        $header = array('INITIAL STATE', 'TRANSITIONS TO');
        $form['download']['download_locales_wb']['wb_select']['wb_table'] = array(
          '#input' => TRUE,
          '#prefix' => t('Choose the state that you would like the translations to transition to below.'),
          '#type' => 'container',
          '#theme' => 'table',
          '#header' => $header,
          '#rows' => array(),
        );
        foreach ($transition_select as $from_state => $to_states) {
          $selected = variable_get('lingotek_sync_wb_select_' . $from_state); // get default
          $select_str = '<div class="form-item form-type-select form-item-download-locales-wb-' . $from_state . '">';
          $select_str .= '<select id="edit_download_locales_wb_' . $from_state . '" name="download_locales_wb_' . $from_state . '"';
          $select_str .= ' class="form-select">';
          foreach ($to_states as $state) {
            $select_str .= '<option ';
            $select_str .= 'value="' . $state . '"';
            if ($selected && $selected == $state) { // select default
              $select_str .= ' selected="selected"';
            }
            $select_str .= '>' . $state . '</option>';
          }
          // foreach option add to string
          $select_str .= '</select></div>';
          $form['download']['download_locales_wb']['wb_select']['wb_table']['#rows'][] = array(
            array('data' => $from_state, 'width' => '20%'),
            array('data' => $select_str, 'width' => '80%'),
          ); // Get out of $form_state['input']
        }
      }
    } // end workbench_moderation form elements
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Sync'),
    '#disabled' => ($upload_count_total == 0 && $download_count_total == 0),
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
  if (isset($vals['upload_wb']) && $vals['upload_wb']) {
    foreach ($vals['upload_wb'] as $state) {
      if (isset($vals['upload_nids_wb_json_' . $state])) {
        $upload_nids = array_merge($upload_nids, json_decode($vals['upload_nids_wb_json_' . $state]));
      }
    }
  }
  if (isset($vals['upload_nowb']) && $vals['upload_nowb'] && isset($vals['upload_nids_nowb_json'])) {
    $upload_nids = array_merge($upload_nids, json_decode($vals['upload_nids_nowb_json']));
  }
  $upload_nids = array_unique($upload_nids);

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

  $extra_operations = array();
  $document_ids = array_unique($document_ids);

  // if workbench moderation is enabled
  if (module_exists('workbench_moderation')) {
    $transitions = lingotek_get_workbench_moderation_transitions();
    if (isset($form_state['values']['download_locales_wb_options'])) {
      if ($form_state['values']['download_locales_wb_options'] == 'increment') {
        if (isset($form_state['values']['wb_override_defaults']) && $form_state['values']['wb_override_defaults'] == 1) { // if we are overriding
          $trans_keys = array_keys($transitions);
          foreach ($trans_keys as $trans_key) {
            if (isset($form_state['input']['download_locales_wb_' . $trans_key])) { // if there is a form variable for this transition
              $transitions[$trans_key] = $form_state['input']['download_locales_wb_' . $trans_key]; // override the defaults
            }
          }
        }
      }
      $extra_operations = array_merge($extra_operations, lingotek_post_download_workbench_moderation($form_state['values']['download_locales_wb_options'], $document_ids, $transitions, TRUE));
    }
  }

  //dpm($download_locales); dpm($download_targets); dpm($upload_nids); dpm($vals);
  lingotek_sync_batch_create($upload_nids, $upload_config, $download_targets, $download_targets_incomplete, NULL, $extra_operations);
}

/*
 * Function called when loading bulk_sync form to update upload_nids
 *
 * @param array $sync_report
 *    Contains information returned by LingotekSync::getReport() about edited nodes ready for translation
 */

function lingotek_workbench_moderation_bulk_select($form, $form_state, &$sync_report) {

  if (!isset($form_state['values']['upload_wb'])) {
    $wb_index = variable_get('lingotek_create_documents_by_default_workbench_moderation', 0);
  }
  else {
    $wb_index = $form_state['values']['upload_wb'];
  }
  $states = lingotek_get_workbench_moderation_states();
  $sync_report['workbench_moderation_states'] = $states;

  if ($sync_report['upload_nids_count'] > 0) {
    foreach ($states as $state) {
      $result = lingotek_get_workbench_moderation_state_select($state, $sync_report['upload_nids']);
      $sync_report['upload_nids_wb_count_' . $state] = count($result); // update nid count
      $sync_report['upload_nids_wb_' . $state] = $result; // update nid list
    }
  }
  /*
    if (module_exists('entity_translation') && $sync_report['upload_nids_et_count'] > 0) {
    $result = lingotek_get_workbench_moderation_state_select($state, $sync_report['upload_nids_et']);
    $sync_report['upload_nids_et_count'] = count($result);
    $sync_report['upload_nids_et'] = $result;
    } */
}

/*
 * Get nids of all nodes using workbench_moderation that are in moderation state $wb_state
 *
 * @param string $wb_state
 *    moderation state to search for
 * @param array $nids
 *    node ids to search through
 * @return array $result
 *    a numbered array containing the matching nids
 */

function lingotek_get_workbench_moderation_state_select($wb_state, $nids = array()) {

  $sub_query = db_select('workbench_moderation_node_history', 'wb1') // populate a numbered array with nids whose most recent moderation state has been selected
      ->condition('nid', $nids, 'IN')
      ->groupBy('nid');
  $sub_query->addExpression('MAX(hid)', 'hid');
  $query = db_select('workbench_moderation_node_history', 'wb2')
      ->fields('wb2', array('nid'))
      ->condition('hid', $sub_query, 'IN')
      ->condition('state', $wb_state);
  $result = $query->execute()->fetchCol(0);
  return $result;
}

/*
 * Gets associative array of duplicate workbench moderation transitions
 *  i.e. needs_review transitions to draft AND needs_review transitions to published
 *
 * @return array $dup_trans
 *    associative array with array(transitions_from => array(transitions_to_1, transitions_to_2, etc.), transitions_from => etc.)
 *    ONLY returns transitions with duplicate from_name
 *    Returns empty if option is not "Increment to next state" or if no from_state has multiple transitions
 *
 */

function lingotek_workbench_moderation_get_mult_transitions() {
  $dup_trans = array();
  $transitions = workbench_moderation_transitions();

  foreach ($transitions as $transition) { // insert transitions into a keyed array
    $dup_trans[$transition->from_name][$transition->to_name] = $transition->to_name;
  }
  foreach ($transitions as $transition) { // remove transitions without duplicate from_name
    if (count($dup_trans[$transition->from_name]) < 2) {
      unset($dup_trans[$transition->from_name]);
    }
  }
  return $dup_trans; // otherwise return NULL
}

/*
 * Build operations array with batch worker functions to moderate workbench moderation
 *
 * @param int $options_index
 *    Index for array returned by lingotek_get_workbench_moderation_options()
 *    Specifies the state to which we should moderate
 *
 * @param array $document_ids
 *    Document IDs of downloaded documents
 *    Used to get corresponding Node IDs for nodes which need moderation
 */

function lingotek_post_download_workbench_moderation($options_index, $document_ids = array(), $trans_options = array(), $automatic_moderate = FALSE) {
  $operations = array();
  if ($automatic_moderate) {
    foreach ($document_ids as $document_id) {
      $nid = LingotekSync::getNodeIdFromDocId($document_id);
      $operations[] = array('lingotek_workbench_moderation_moderate', array($nid, $options_index, $trans_options));
    }
  }
  return $operations;
}

/**
* Updates the 'target_sync_progress_[lang-code]' field for every target in the lingotek table
* with the overall progress returned by TMS
*
* @param int array $document_ids
*    array of Document IDs that you want to update
*
*/
function lingotek_get_and_update_target_progress($document_ids) {
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
    $delete_cids_maybe = array();
    $node_values = array();
    $cfg_values = array();
    $trans_obj = NULL;
    foreach ($progress_report->byDocumentIdAndTargetLocale as $doc_id => $completion) {
      $nid = LingotekSync::getNodeIdFromDocId($doc_id);
      // if no $nid is found for the doc ID, it must be config translation
      if (!$nid) {
        $trans_obj = LingotekConfigChunk::loadByLingotekDocumentId($doc_id, NULL, $project_id);
        if (!$trans_obj) {
          LingotekLog::error("Lingotek doc ID '@doc_id' not found", array('@doc_id' => $doc_id));
          continue;
        }
        // re-assign the chunk ID to be a node ID for now...
        $nid = $trans_obj->cid;
        $delete_cids_maybe[] = $nid;
      }
      else {
        $delete_nids_maybe[] = $nid;
        $target_number = $targets_count[$nid]->targets;
      }      
      $total_percentage = 0;
      foreach ($completion as $language => $percent) {
        if ($trans_obj) {
          $values = &$cfg_values;
        }
        else {
          $values = &$node_values;
        }
        $values[] = array($nid, 'target_sync_progress_' . $language, $percent);
        $values[] = array($nid, 'target_sync_last_progress_updated_' . $language, time());
        $total_percentage += $percent;

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
          $values[] = array($nid, 'target_sync_status_' . $language, $to_status);
        }
      }
      // for now just report on node translation progress...
      if (!$trans_obj) {
        $node_values[] = array(
          'nid' => $nid,
          'lingokey' => 'translation_progress',
          'lingovalue' => $total_percentage / $target_number,
        );
      }
    }

    if ($delete_nids_maybe) {
      lingotek_lingonode_variable_delete_multiple($delete_nids_maybe, 'target_sync_progress_%', 'LIKE');
      lingotek_lingonode_variable_delete_multiple($delete_nids_maybe, 'target_sync_last_progress_updated_%', 'LIKE');
      lingotek_lingonode_variable_delete_multiple($delete_nids_maybe, 'translation_progress');
    }
    if ($delete_cids_maybe) {
      foreach ($delete_cids_maybe as $cid) {
        if ($trans_obj = LingotekConfigChunk::loadById($cid)) {
          $trans_obj->deleteMetadataValue('target_sync_progress_%');
          $trans_obj->deleteMetadataValue('target_sync_last_progress_updated_%');
          $trans_obj->deleteMetadataValue('translation_progress');
        }
      }
    }

    // merge status info for nodes
    foreach ($node_values as $record) {
      $nid = (isset($record['nid']) ? $record['nid'] : $record[0]);
      $lingokey = (isset($record['lingokey']) ? $record['lingokey'] : $record[1]);
      $lingovalue = (isset($record['lingovalue']) ? $record['lingovalue'] : $record[2]);
      $query = db_merge('lingotek')
        ->key(array('nid' => $nid, 'lingokey' => $lingokey))
        ->fields(array(
            'nid' => $nid,
            'lingokey' => $lingokey,
            'lingovalue' => $lingovalue,
          ))
        ->execute();
    }
    
    // insert status info for config
    foreach ($cfg_values as $record) {
      $query = db_merge('lingotek_config_metadata')
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
    $error_message .= "</ul><i>For additional information, check your recent log messages</i>";
    drupal_set_message(t('@error_message', array('@error_message' => $error_message)), 'error');
  }

}
