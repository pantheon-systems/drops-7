<?php

/**
 * @file
 * Bulk Grid form
 */

function lingotek_bulk_grid_form($form, $form_state) {
  
  global $language;
  
  /*
   * Here we store or retrieve the GET parameters so that the state of the table is maintained when leaving and coming back
   * Also makes it so the state is not lost when performing bulk actions
   */
  if (count($_GET) == 1 && isset($_SESSION['grid_custom_parameters']) && !empty($_SESSION['grid_custom_parameters'])) {
    $_SESSION['grid_custom_parameters']['preventloop'] = TRUE;
    drupal_goto('admin/settings/lingotek/manage/list', array('query' => $_SESSION['grid_custom_parameters']));
  }
  else {
    $_SESSION['grid_custom_parameters'] = $_GET;
    if ($_SESSION['grid_custom_parameters']['q']) {
      unset($_SESSION['grid_custom_parameters']['q']);
    }
  }
  
  $path_to_lingotek = drupal_get_path('module', 'lingotek');
  lingotek_is_module_setup();
  
  // Output success messages for actions
  if (isset($_SESSION['lingotek_edit_nodes'])) {
    drupal_set_message(format_plural($_SESSION['lingotek_edit_nodes'], 'Settings changed for one node.', 'Settings changed for @count nodes.'));
    unset($_SESSION['lingotek_edit_nodes']);
  }
  if (isset($_SESSION['lingotek_sync_nodes'])) {
    drupal_set_message(format_plural($_SESSION['lingotek_sync_nodes'], 'Target translations progress checked and updated for one node.', 'Target translations progress checked and updated for @count nodes.'));
    unset($_SESSION['lingotek_sync_nodes']);
  }
  
  // Populate form_state with filter values so the query can use them
  $form_state['values']['columns'] = lingotek_grid_get_columns(TRUE);
  $form_state['values']['grid_header'] = array();
  
  // Define source actions - keys are used to decide what action to do in the 'lingotek_grid_action_submit' function
  $action_options = array(
    'select' => t('Select an action'),
    'upload' => t('Upload Source for Translation'),
    'sync' => t('Check Progress of Translations'),
    'reset' => t('Disassociate Translations'),
//    'delete' => t('Delete Nodes'),
    'edit' => t('Edit Translation Settings'),
    'workflow' => t('Change Workflow'),
    'Download' => array('download_all' => t('Download All Translations')),
  );

  $target_languages_raw = language_list('language');
    
  foreach ($target_languages_raw as $target_raw) {
    $action_options['Download']['download_' . $target_raw->lingotek_locale] = 'Download ' . $target_raw->name . ' (' . $target_raw->lingotek_locale . ') Translation';
  }
  
  $form['lingotek-console'] = array(
    '#markup' => '<div id="lingotek-console"></div>',
  );
  
  // Run query to get table rows
  $table_data = lingotek_grid_get_rows($form, $form_state);
  
  // Count of returned results
  $page = pager_find_page(); // Get current page from url
  
  $form['customize'] = array(
    '#markup' => l('<i class="fa fa-list-alt fa-2x" ></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/customize', array('html' => TRUE, 'attributes' => array('title' => t('Customize Table'), 'class' => array('ctools-use-modal', 'ctools-modal-customize-table', 'lingotek-action')))),
  );
  /*$form['filter_popup'] = array(
    '#markup' => l('<i class="fa-search fa-2x"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters', array('html' => TRUE, 'attributes' => array('title' => 'Set Filters', 'class' => array('ctools-use-modal', 'ctools-modal-set-filters', 'lingotek-action')))),
  );*/
  $last_updated = variable_get('lingotek_pending_last_updated', NULL);
  $message = t('Check in-progress translation statuses (Last checked @time)', array('@time' => $last_updated ? lingotek_human_readable_timestamp($last_updated) . ' ago' : 'Never'));
  $form['refresh'] = array(
    '#markup' => l('<i class="fa fa-refresh fa-2x ltk-refresh"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/update', array('html' => TRUE, 'attributes' => array('class' => 'lingotek-action', 'title' => $message))),
  );
  $form['lingotek_update'] = array(
    '#markup' => l('<i class="fa fa-cloud-download fa-2x ltk-download"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/download-ready', array('html' => TRUE, 'attributes' => array('title' => t('Download complete translations'), 'class' => array('lingotek-action')))),
  );
  
  $form['edit_settings'] = array(
    '#markup' => l(t('Edit Settings'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/edit', array('attributes' => array('id' => 'edit-settings-link', 'class' => array('ctools-use-modal', 'ctools-modal-set-filters')))),

  );
  $form['disassociate_translations'] = array(
    '#markup' => l(t('Disassociate Translations'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/reset', array('attributes' => array('id' => 'reset-translations-link', 'class' => array('ctools-use-modal', 'ctools-modal-set-filters')))),
  );

  if (LingotekAccount::instance()->isEnterprise()) {
    $form['change_workflow'] = array(
      '#markup' => l(t('Change Workflow'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/change-workflow', array('attributes' => array('id' => 'change-workflow-link', 'class' => array('ctools-use-modal', 'ctools-modal-set-filters')))),
    );
  }
  
  $form['search'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($_SESSION['grid_filters']['search']) ? $_SESSION['grid_filters']['search'] : '',
    '#title' => l('<i class="fa fa-search"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters', array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => array('ctools-use-modal', 'ctools-modal-set-filters', 'ltk-action')))) . ' ' . t('Search') . ': ',
    '#size' => 30,
  );

  $search_options = array(
    'all' => 'All',
    'title' => 'Title',
    'body' => 'Body',
  );
  
  $form['search_type'] = array(
    '#type' => 'select',
    '#options' => $search_options,
    '#default_value' => isset($_SESSION['grid_filters']['search_type']) ? $_SESSION['grid_filters']['search_type'] : 'all',
  );
  
  $form['search_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Go',
    '#submit' => array('lingotek_grid_filter_inline_submit'),
  );
    
  $filter_set = FALSE;
  
  if (isset($_SESSION['grid_filters'])) {  
    foreach ($_SESSION['grid_filters'] as $key => $value) {
      if (is_array($value)) {
        $keys = array_keys($value);
        if (count($keys) == 1) {
          if ($keys[0] != ''  && $keys[0]  != 'all') {
            $filter_set = TRUE;
          }
        }
        elseif (count($keys) > 1) {
          $filter_set = TRUE;
        }
      }
      else {
        if (!empty($value) && $value != 'all') {
          $filter_set = TRUE;
        }
      }
    }
  }
  
  if ($filter_set) {
    $form['filter_message'] = array(
      '#markup' => ' ' . l('<i class="fa fa-times" style="margin-right: 5px;"></i>' . t('Clear Filters'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters/clear', array('html' => TRUE)) . '',
    );
  }
  
  // Build actions selector
  $form['actions_select'] = array(
    '#type' => 'select',
    '#options' => $action_options,
    '#title' => '<i class="fa fa-asterisk ltk-muted"></i>' . ' ' . t('Actions') . ': ',
  );

  // Actions submit button
  $form['actions_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Action'),
    '#name' => 'actions_submit',
    '#submit' => array('lingotek_grid_action_submit'),
  );
  
  // div container for the table and pager
  $form['grid_container'] = array(
    '#type' => 'container',
    '#attached' => array(
      'css' => array(
        '//netdna.bootstrapcdn.com/font-awesome/4.0.3/css/font-awesome.css' => array(
          'type' => 'external',
        ),
      ),
      'js' => array( // We get php errors when TableSort and AJAX are combined (see https://drupal.org/node/1833746).             // So we are faking it with a hidden submit button and some jQuery.
        $path_to_lingotek . '/js/lingotek.bulk_grid.js',
      ),
    ),
  );

  ctools_include('modal');
  ctools_modal_add_js();

  drupal_add_js(array(
    'customize-table' => array(
      'modalSize' => array(
        //'type' => 'fixed',
        //'width' => 200,
        //'height' => 200,
      ),
    ),
    'set-filters' => array(
      'modalSize' => array(
        'type' => 'fixed',
        'width' => 500,
        'height' => 500,
      ),
    ),
  ), 'setting');
  
  if (!empty($table_data)) { // If results, render the table.  Otherwise, output 'No results were returned.'
    // Calculate and output the number of results shown
    // The actual table
    $form['grid_container']['the_grid'] = array(
      '#type' => 'tableselect',
      '#header' => $form_state['values']['grid_header'],
      '#options' => $table_data,
    );
    // The pager
    $form['grid_container']['pager'] = array(
      '#theme' => 'pager',
    );
  }
  else {
    $form['grid_container']['none'] = array(
      '#markup' => '<div class="grid-empty">' . t('No results found.') . '</div>',
    );
  }
  
  // process limit_select to correctly limit the query and pager
  $limit = 10;
  if (isset($_SESSION['limit_select'])) {
    $limit = $_SESSION['limit_select'];
  }
  $form_state['values']['limit_select'] = $limit;
  
  $results_first = ($page * $form_state['values']['limit_select']) + 1;
  $results_last = $results_first + count($table_data) - 1;
  if ($results_last > 0) {
    $form['count'] = array(
      '#markup' => '<span id="grid-result-summary">'
      . t('Displaying @first - @last', array('@first' => $results_first, '@last' => $results_last))
      . ($filter_set ? (' (' . t('filtered results') . ')') : '') . '</span>',
    );
  }

  if (count($table_data) >= 10) {
    $form['limit_select'] = array(
      '#type' => 'select',
      '#prefix' => '<div id="page-limit">',
      '#suffix' =>  ' ' . t('results per page') . '</div>',
      '#options' => array(
        10 => '10',
        25 => '25',
        50 => '50',
        100 => '100',
        250 => '250',
        500 => '500',
      ),
      '#default_value' => $limit,
    );
  }

  return $form;
}

function lingotek_filters_popup_form($form = array(), $form_state = array()) {  
  
  // Container to create styleable div class
  $form['filter_fieldset']['filters'] = array(
    '#type' => 'container',
  );

  // Container to create styleable div class
  $form['filter_fieldset']['filter_buttons'] = array(
    '#type' => 'container',
  );
  
  // Filter submit button
  $form['filter_fieldset']['filter_buttons']['filter_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Filters'),
    '#submit' => array('lingotek_grid_filter_submit'),
  );

  // Reset filter defaults button
  $form['filter_fieldset']['filter_buttons']['filter_reset'] = array(
    '#type' => 'submit',
    '#value' => t('Clear Filters'),
    '#submit' => array('lingotek_grid_clear_filters'),
  );
  
  $form_state['values']['filters'] = lingotek_grid_get_filters(TRUE);
  
  $form['filter_fieldset']['filters'] += lingotek_grid_build_filters($form_state);
  
  return $form;
}

function lingotek_filters_popup() {
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');
  
  $form = array();
  $form_state = array(
    'ajax' => TRUE,
  );
  $output = ctools_modal_form_wrapper('lingotek_filters_popup_form', $form_state);
  
  if (!empty($form_state['executed'])) {
    lingotek_grid_filter_submit($form, $form_state);
    // Create ajax command array, dismiss the modal window.
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    drupal_exit();
  }
    
  print ajax_render($output);
}

function lingotek_grid_customize_form($form, $form_state) {  
  
  // Container to create styleable div class
  $form['customize_table_fieldset']['custom_columns'] = array(
    '#type' => 'container',
  );

  // Container to create styleable div class
  $form['customize_table_fieldset']['custom_buttons'] = array(
    '#type' => 'container',
  );

  // Submit customized columns button
  $form['customize_table_fieldset']['custom_buttons']['custom_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('lingotek_grid_filter_submit'),
  );

  // Reset column defaults button
  $form['customize_table_fieldset']['custom_buttons']['custom_clear'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to Defaults'),
    '#submit' => array('lingotek_grid_reset_columns'),
  );
  
  $form_state['values']['columns'] = lingotek_grid_get_columns(TRUE);
  $form['customize_table_fieldset']['custom_columns'] += lingotek_grid_build_column_checkboxes($form_state);
  
  return $form;
}

function lingotek_grid_customize() {
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');
  
  $form_state = array(
    'ajax' => TRUE,
  );
  $output = ctools_modal_form_wrapper('lingotek_grid_customize_form', $form_state);
  
  if (!empty($form_state['executed'])) {

    $f = array();
    form_execute_handlers('submit', $f, $form_state);
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    exit;
  }
    
  print ajax_render($output);
}

function lingotek_grid_filter_inline_submit($form, $form_state) {
  $_SESSION['grid_filters']['search_type'] = $form_state['values']['search_type'];
  if (!empty($form_state['values']['search'])) {
    $_SESSION['grid_filters']['search'] = $form_state['values']['search'];
  }
  else {
    unset($_SESSION['grid_filters']['search']);
    unset($_SESSION['grid_filters']['search_type']);
  }
  unset($_SESSION['grid_filters']['body']);
  unset($_SESSION['grid_filters']['title']);
  if ($form_state['values']['search_type'] == 'title') {
    $_SESSION['grid_filters']['title'] = $form_state['values']['search'];
  }
  elseif ($form_state['values']['search_type'] == 'body') {
    $_SESSION['grid_filters']['body'] = $form_state['values']['search'];
  }

  if (isset($form_state['values']['limit_select'])) {
    $_SESSION['limit_select'] = $form_state['values']['limit_select'];
  }
}

/**
 * Submit function for The Grid's filters (header, column, and filter fieldsets)
 * Adds filters to the session variable so the query can use them after the page load
 */
function lingotek_grid_filter_submit($form, $form_state) {
  $stselected = FALSE;
  $lselected = FALSE;
  // we have to add some of these keys to the session because otherwise they are not saved after the form submission
  if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#name'] != 'op') {
    $_SESSION['button'] = $form_state['clicked_button']['#name'];
  }
  if (isset($form_state['values'])) {
    foreach ($form_state['values'] as $key => $value) {
      $add_key_to_session = FALSE;
      $nest = NULL;
      if ($key == 'limit_select') {
        $add_key_to_session = TRUE;
        $lselected = TRUE;
      }
      elseif (strpos($key, '__filter')) {
        $add_key_to_session = TRUE;
        $nest = 'grid_filters'; //stored in $_SESSION['grid_filters'][$key]
      }
      elseif (strpos($key, '__custom')) {
        $add_key_to_session = TRUE;
        $nest = 'grid_custom'; //stored in $_SESSION['grid_custom'][$key]
      }
      // if we want this key, add it to the session
      if ($add_key_to_session) {
        if (is_null($nest)) {
          $_SESSION[$key] = $value;
        }
        else {
          $_SESSION[$nest][str_replace('__filter', '', $key)] = $value;
        }

      }
    }
  }
  if (!$lselected) {
    $_SESSION['limit_select'] = 10;
  }
}

/**
 * Submit function for The Grid's actions
 * The action corresponds to the key of the option selected
 * Often redirects to batch operations or to other pages entirely
 */
function lingotek_grid_action_submit($form, $form_state) {
  $nids = array();

  if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#name'] == 'actions_submit') { // If submitting an action
    foreach ($form_state['values']['the_grid'] as $value) {
      if ($value != 0) {
        $nids[] = $value;
      }
    }
    
    if (isset($form_state['values']['actions_select'])) { // If an action was selected (which it would be, I don't know if this could ever NOT occur with normal use)
      $action = $form_state['values']['actions_select']; // Get the action
      if (count($nids) <= 0) { // Select a node
        drupal_set_message(t('You must select at least one node to @action.', array('@action' => $action)), 'warning'); // Or pay the price
      }
      elseif ($action == 'upload') { // If uploading
        $batch = array(
          'title' => t('Uploading Nodes To Lingotek'),
          'finished' => 'lingotek_sync_upload_node_finished'
        );
        $operations = lingotek_get_sync_upload_batch_elements($nids);
        $batch['operations'] = $operations;
        $redirect = 'admin/settings/lingotek/manage';

        batch_set($batch);
        batch_process($redirect); // Run batch operations to upload all of the selected nodes to Lingotek
      }
      elseif (substr($action, 0, 8) == 'download') { // If downloading all targets
        $lang = substr($action, 9, 10);
        $target_locales = ($lang == 'all') ? lingotek_get_target_locales() : array($lang);
        $document_ids_map = LingotekSync::getDocIdsFromNodeIds($nids, $associate = TRUE);
        $complete_targets = array();
        $incomplete_targets = array();
        
        // We need to check all pending nodes to see if they are READY so that in the end the status is correct.
        $pending_document_ids = array();
        foreach ($document_ids_map as $nid => $nid_obj) {
          $status = lingotek_lingonode($nid, 'node_sync_status');
          if ($status != LingotekSync::STATUS_CURRENT && $status != LingotekSync::STATUS_READY) {
            $pending_document_ids[] = $nid_obj->doc_id;
          }
        }
        lingotek_get_and_update_target_progress($pending_document_ids);
        

        foreach ($target_locales as $language) { // Separate the nodes by target_sync_progress_[lang code] percentage
          $complete_nids = LingotekSync::getNodeIdSubsetByTargetStatusReady($nids, $language);
          $complete_doc_ids = array();
          if (!empty($complete_nids)) {
            $complete_doc_ids = LingotekSync::getDocIdsFromNodeIds($complete_nids);
          }
          foreach ($document_ids_map as $nid_obj) {
            $doc_id = $nid_obj->doc_id;
            if (in_array($doc_id, $complete_doc_ids)) { // If workflow completed, create target json
              $complete_targets[] = (object) array('document_id' => $doc_id, 'locale' => $language);
            }
            else {
              $incomplete_targets[] = (object) array('document_id' => $doc_id, 'locale' => $language);
            }
          }
        }
        if (!empty($complete_targets)) {
          $batch = array(
            'title' => t('Downloading Translations from Lingotek'),
            'operations' => array(),
            'finished' => 'lingotek_sync_download_target_finished'
          );
          $batch['operations'] = array_merge($batch['operations'], lingotek_get_sync_download_batch_elements($complete_targets, LingotekSync::STATUS_CURRENT)); // If target progress is at 100, download and set to current
          $batch['operations'] = array_merge($batch['operations'], lingotek_get_sync_download_batch_elements($incomplete_targets, LingotekSync::STATUS_PENDING)); // Otherwise, download and set to pending
          $redirect = 'admin/settings/lingotek/manage';

          batch_set($batch);
          batch_process($redirect); // Run the batch operations
        }
      }
      elseif ($action == 'delete') { // If resetting translations
        $_SESSION['lingotek_disassociate_nodes'] = $nids;
        drupal_goto('admin/settings/lingotek/manage/delete/'); // Redirect to the disassociate translations form to do so
      }
      elseif ($action == 'reset') { // If resetting translations
        $_SESSION['lingotek_disassociate_nodes'] = $nids;
        drupal_goto('admin/settings/lingotek/manage/reset/'); // Redirect to the disassociate translations form to do so
      }
      elseif ($action == 'edit') { // If editing node settings
        $_SESSION['lingotek_edit_nodes'] = $nids;
        drupal_goto('admin/settings/lingotek/manage/edit/'); // Redirect to the edit Lingotek node settings form - for multiple nodes, form defaults are global defaults
      }
      elseif ($action == 'workflow') { // If changing the workflow
        $_SESSION['lingotek_change_workflow'] = $nids;
        drupal_goto('admin/settings/lingotek/manage/change-workflow/'); // Redirect to the change-workflow settings form - for multiple nodes, form defaults are global defaults
      }
      elseif ($action == 'sync') { // If syncing the progress
        $_SESSION['lingotek_sync_nodes'] = count($nids);
        lingotek_update_target_progress_batch_create($nids); // Run batch operations to get the progress report from Lingotek
      }
    }
  }
}

/**
 * Builds the checkbox elements for customizing The Grid's columns
 * Uses predefined defaults specified in 'lingotek_grid_define_columns'
 */
function lingotek_grid_build_column_checkboxes($form_state) {
  $prefix = '';
  $suffix = '__custom'; // Suffix specified because the filter submit function differentiates based on this tag and puts the keys into the session variable as such
  $columns = lingotek_grid_define_columns(); // Allowed columns and defaults for source and target grids are defined here
  $column_elements = array(
    'nid' => array(
      '#type' => 'checkbox',
      '#title' => t('Node ID'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'nid' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'nid' . $suffix] : in_array('nid', $columns['defaults']),
    ),
    'content_type' => array(
      '#type' => 'checkbox',
      '#title' => t('Content Type'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'content_type' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'content_type' . $suffix] : in_array('content_type', $columns['defaults']),
    ),
    'title' => array(
      '#type' => 'checkbox',
      '#title' => t('Node Title'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'title' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'title' . $suffix] : in_array('title', $columns['defaults']),
    ),
    'language' => array(
      '#type' => 'checkbox',
      '#title' => t('Source Language'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'language' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'language' . $suffix] : in_array('language', $columns['defaults']),
    ),
    'translations' => array(
      '#type' => 'checkbox',
      '#title' => t('Translations'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'translations' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'translations' . $suffix] : in_array('translations', $columns['defaults']),
    ),
    'configuration' => array(
      '#type' => 'checkbox',
      '#title' => t('Configuration'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'configuration' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'configuration' . $suffix] : in_array('configuration', $columns['defaults']),
    ),
    'document_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Doc ID'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'document_id' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'document_id' . $suffix] : in_array('document_id', $columns['defaults']),
    ),
    'translation_progress' => array(
      '#type' => 'checkbox',
      '#title' => t('Translation Progress'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'translation_progress' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'translation_progress' . $suffix] : in_array('translation_progress', $columns['defaults']),
    ),
    'workflow' => array(
      '#type' => 'checkbox',
      '#title' => t('Workflow'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'workflow' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'workflow' . $suffix] : in_array('workflow', $columns['defaults']),
    ),
    'changed' => array(
      '#type' => 'checkbox',
      '#title' => t('Last Modified'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'changed' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'changed' . $suffix] : in_array('changed', $columns['defaults']),
    ),
    'last_uploaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Time Last Uploaded'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'last_uploaded' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'last_uploaded' . $suffix] : in_array('last_uploaded', $columns['defaults']),
    ),
    'locale_progress_percent' => array(
      '#type' => 'checkbox',
      '#title' => t('Target Progress Percentage'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'locale_progress_percent' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'locale_progress_percent' . $suffix] : in_array('locale_progress_percent', $columns['defaults']),
    ),
    'progress_updated' => array(
      '#type' => 'checkbox',
      '#title' => t('Progress Last Updated'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'progress_updated' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'progress_updated' . $suffix] : in_array('progress_updated', $columns['defaults']),
    ),
//    'translation_progress_percent' => array(
//      '#type' => 'checkbox',
//      '#title' => t('Translation Progress Percentage'),
//      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'translation_progress_percent' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'translation_progress_percent' . $suffix] : in_array('translation_progress_percent', $columns['defaults']),
//    ),
    'last_downloaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Time Last Downloaded'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'last_downloaded' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'last_downloaded' . $suffix] : in_array('last_downloaded', $columns['defaults']),
    ),
    'actions' => array(
      '#type' => 'checkbox',
      '#title' => t('Actions'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'actions' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'actions' . $suffix] : in_array('actions', $columns['defaults']),
    ),
  );
  $column_elements = array_intersect_key($column_elements, $columns['columns']); // Reduces the output columns to the defaults specified in 'lingotek_grid_define_columns'

  return lingotek_grid_process_elements($column_elements, $prefix, $suffix); // adds prefixes and suffixes to the elements
}

function lingotek_grid_update() {
  $nids = LingotekSync::getNodeIdsByStatusAndTarget(LingotekSync::STATUS_PENDING);
  variable_set('lingotek_pending_last_updated', time());
  if (count($nids) > 0) {
    $_SESSION['lingotek_sync_nodes'] = count($nids);
    lingotek_update_target_progress_batch_create($nids); // Run batch operations to sync all 'In Progress' nodes
  }
  else {
    drupal_set_message(t('There are no in-progress translations to check.'));
    drupal_goto('admin/settings/lingotek/manage/list');
  }
}

function lingotek_grid_download_ready() {
  $targets = LingotekSync::getTargetsByStatus(LingotekSync::STATUS_READY);
  $sync_success_target = LingotekSync::STATUS_CURRENT;

  if (!empty($targets)) {

    $nids = array();
    foreach ($targets as $target) {
      $nids[] = $target['nid'];
    }
    $nodes = node_load_multiple($nids);
    
    $operations = array();
    foreach ($targets as $target) {
      if ($nodes[$target['nid']]->lingotek['profile'] != LingotekSync::PROFILE_DISABLED) { // exclude nodes with PROFILE_DISABLED
        $operations[] = array('lingotek_sync_download_node_target', array($target['nid'], $target['locale'], $sync_success_target, FALSE));
      }
    }
    $redirect = 'admin/settings/lingotek/manage';
    $batch = array(
      'title' => t('Downloading Translations'),
      'finished' => 'lingotek_sync_download_target_finished',
    );
    $batch['operations'] = $operations;
    batch_set($batch);
    batch_process($redirect); // Run batch operations to upload all of the selected nodes to Lingotek
  }
  else {
    drupal_set_message(t('There are no translations ready for download.'));
    drupal_goto('admin/settings/lingotek/manage/list');
  }
}

/**
 * Gets the columns that will be shown from the session variable
 *
 * @param bool $source
 *    Changes whether settings for source or target variables are output
 *
 * @return array $columns
 *    Associative array keyed by column name with prefix and suffix removed
 *    Keys point to a bool which specifies if the column should be shown or not
 */
function lingotek_grid_get_columns() {
  $filters = array();

  if (!isset($_SESSION['grid_custom'])) { // If the columns do not exist yet in the session variable we get an error, so reset them here.
    lingotek_grid_reset_columns();
  }

  foreach ($_SESSION['grid_custom'] as $key => $value) {
    $columns[str_replace('__custom', '', $key)] = $value;
  }

  return $columns;
}

/**
 * Cleans up the grid_custom session array, restoring the predefined defaults.
 */
function lingotek_grid_reset_columns() {
  if (isset($_SESSION['grid_custom'])) {
    unset($_SESSION['grid_custom']);
  }
  $source_columns = lingotek_grid_define_columns();
  foreach ($source_columns['columns'] as $column) {
    $_SESSION['grid_custom'][$column . '__custom'] = in_array($column, $source_columns['defaults']);
  }
}

/**
 * Defines which columns should be shown for source and target tables, and what the defaults should be.
 *
 * @param bool $source
 *    Changes whether settings for source or target variables are output
 *
 * @return array $columns
 *    Associative array with two important inner arrays:
 *      1: 'columns' points to the allowed columns,
 *      2: 'defaults' points to the base defaults to use when resetting the customization
 *    The 'columns' and 'defaults' arrays are output with the keys and the values being the same
 */
function lingotek_grid_define_columns() {
  $columns = array(
    'columns' => array(
      'nid',
      'content_type',
      'title',
      'language',
      'translations',
      'configuration',
      'document_id',
      'workflow',
      'changed',
      'last_uploaded',
      'translation_progress_percent',
      'actions',
    ),
    'defaults' => array(
      'title',
      'language',
      'translations',
      'configuration',
      'content_type',
      'actions',
      'changed',
    ),
  );
  $columns['columns'] = array_combine($columns['columns'], $columns['columns']);
  $columns['defaults'] = array_combine($columns['defaults'], $columns['defaults']);
  return $columns;
}

/**
 * Builds the form elements for the filters.
 *
 * @return array
 *    The full list of filters is later filtered so that there is exactly one column for every filter
 */
function lingotek_grid_build_filters($form_state) {
  $languages = language_list();
  $source_languages = array('all' => 'All Languages');
  foreach ($languages as $code => $language) {
    $source_languages[$code] = $language->name . ' (' . $language->lingotek_locale . ')';
  }
  
  $profiles = array();
  $profiles['all'] = 'All';
  $profiles[LingotekSync::PROFILE_CUSTOM] = 'Custom';
  $profiles[LingotekSync::PROFILE_DISABLED] = 'Disabled';
  
  $profile_defaults = lingotek_get_profiles();
  foreach ($profile_defaults as $key => $p) {
    $profiles[$key] = $p['name'];
  }
  
  $filters = array(
    'nid' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['nid']) ? $_SESSION['grid_filters']['nid'] : '',
      '#title' => t('Node ID is'),
      '#size' => 8,
    ),
    'document_id' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['document_id']) ? $_SESSION['grid_filters']['document_id'] : '',
      '#title' => t('Doc ID is'),
      '#size' => 10,
    ),
    'source_language' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['source_language']) ? $_SESSION['grid_filters']['source_language'] : 'all',
      '#title' => t('Source Language'),
      '#options' => $source_languages,
    ),
    'profile' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['profile']) ? $_SESSION['grid_filters']['profile'] : 'all',
      '#title' => t('Translation Profile'),
      '#options' => $profiles,
    ),
    /*'title' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['title']) ? $_SESSION['grid_filters']['title'] : '',
      '#title' => t('Title Includes'),
      '#size' => 30,
    ),
    'body' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['body']) ? $_SESSION['grid_filters']['body'] : '',
      '#title' => t('Body Includes'),
      '#size' => 30,
    ),*/
    'upload_status' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['upload_status']) ? $_SESSION['grid_filters']['upload_status'] : 'all',
      '#title' => t('Upload Status'),
      '#options' => array(
        'all' => t('All'),
        LingotekSync::STATUS_EDITED => t('Out of Sync'),
        LingotekSync::STATUS_CURRENT => t('In Sync'),
        LingotekSync::STATUS_DISABLED => t('Disabled'),
      ),
      '#multiple' => FALSE,
    ),
    'content_type' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['content_type']) ? $_SESSION['grid_filters']['content_type'] : 'all',
      '#title' => t('Content Type(s)'),
      '#options' => array('all' => t('All')) + node_type_get_names(),
      '#multiple' => TRUE
    ),
    /*'translation_status' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['translation_status']) ? $_SESSION['grid_filters']['translation_status'] : 'all',
      '#title' => t('Translation Status'),
      '#options' => array(
        'all' => t('All'),
        'out_of_sync' => t('Out of Sync'),
        'in_sync' => t('In Sync'),
        'disabled' => t('Disabled'),
      ),
      '#multiple' => TRUE
    ),
    'translation_progress' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['translation_progress']) ? $_SESSION['grid_filters']['translation_progress'] : 'all',
      '#title' => t('Translation Progress'),
      '#options' => array(
        'all' => t('All'),
        'needs_upload' => t('Source Needs Upload'),
        'in_progress' => t('In Progress'),
        'download' => t('Ready for Download'),
        'complete' => t('Complete'),
        'disabled' => t('Disabled'),
      ),
      '#multiple' => TRUE
    ),*/
/*    'last_uploaded' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['last_uploaded']) ? $_SESSION['grid_filters']['last_uploaded'] : 'all',
      '#title' => t('Last Uploaded'),
      '#options' => array(
        'all' => t('All'),
        '1 day' => t('Less than 1 day ago'),
        '< 1 week' => t('Less than 1 week ago'),
        '>= 1 week' => t('1 week or more ago'),
        'unknown' => t('Unknown'),
      ),
      '#multiple' => FALSE
    ),
    'locale_progress_percent' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['locale_progress_percent']) ? $_SESSION['grid_filters']['locale_progress_percent'] : 'all',
      '#title' => t('Target Progress Percentage'),
      '#options' => array(
        'all' => t('All'),
        0 => t('0 Percent'),
        25 => t('Between 1 and 25 Percent'),
        50 => t('Between 26 and 50 Percent'),
        75 => t('Between 51 and 75 Percent'),
        100 => t('Between 76 and 100 Percent')
      ),
      '#multiple' => TRUE
    ),
    'progress_updated' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['progress_updated']) ? $_SESSION['grid_filters']['progress_updated'] : 'all',
      '#title' => t('Progress Updated'),
      '#options' => array(
        'all' => t('All'),
        '1 day' => t('Less than 1 day ago'),
        '< 1 week' => t('Less than 1 week ago'),
        '>= 1 week' => t('1 week or more ago'),
        'unknown' => t('Unknown'),
      ),
      '#multiple' => FALSE
    ),*/
//    'translation_progress_percent' => array(
//      '#type' => 'select',
//      '#default_value' => isset($_SESSION['grid_filters']['translation_progress_percent']) ? $_SESSION['grid_filters']['translation_progress_percent'] : 'all',
//      '#title' => t('Translation Progress Percentage'),
//      '#options' => array(
//        'all' => t('All'),
//        0 => t('0 Percent'),
//        25 => t('Between 1 and 25 Percent'),
//        50 => t('Between 26 and 50 Percent'),
//        75 => t('Between 51 and 75 Percent'),
//        100 => t('Between 76 and 100 Percent')
//      ),
//      '#multiple' => TRUE
//    ),
/*    'last_downloaded' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['last_downloaded']) ? $_SESSION['grid_filters']['last_downloaded'] : 'all',
      '#title' => t('Last Downloaded'),
      '#options' => array(
        'all' => t('All'),
        '1 day' => t('Less than 1 day ago'),
        '< 1 week' => t('Less than 1 week ago'),
        '>= 1 week' => t('1 week or more ago'),
        'unknown' => t('Unknown'),
      ),
      '#multiple' => FALSE
    ),*/
  );

  return lingotek_grid_process_elements($filters, '', '__filter'); // Add prefix and suffix to the name of each filter element
}

/**
 * Get the list of filters and their values from the session
 *
 * @param bool $source
 *    Changes whether filters for source or target are output
 *
 * @return array $filters
 *    Associative array keyed by filter name with prefix and suffix removed
 *    Keys point to the values to be filtered on
 */
function lingotek_grid_get_filters($source = TRUE) {
  $filters = array();
  if (isset($_SESSION['grid_filters'])) {
    foreach ($_SESSION['grid_filters'] as $key => $value) {
      $new_key = str_replace($source ? 'source_' : 'target_', '', $key, $replaced);
      if ($replaced > 0) {
        $filters[str_replace('__filter', '', $new_key)] = $value;
      }
    }
  }
  return $filters;
}

/**
 * Completely clears out any filters from the session variable
 * Filters will automatically revert to their defaults
 */
function lingotek_grid_clear_filters() {
  if (isset($_SESSION['grid_filters'])) {
    unset($_SESSION['grid_filters']);
    session_write_close();
  }
}

function lingotek_grid_clear_filters_page() {
  lingotek_grid_clear_filters();
  drupal_goto('admin/settings/lingotek/manage/list');
}

/**
 * Add $prefix and $suffix to each $element name
 */
function lingotek_grid_process_elements($elements, $prefix, $suffix) {
  $new_keys = array();
  foreach ($elements as $element => $innards) {
    $new_keys[$prefix . $element . $suffix] = $innards;
  }
  return $new_keys;
}

/**
 * Dynamic query processing function for the grid
 * Since the header defines which columns are shown, this query gets all possible values and refines the header using the columns selected in the UI
 * The filters are also processed here
 *
 * @return array $table_data
 *    Returns array of rows
 *    Populates The Grid
 */
function lingotek_grid_get_rows($form, &$form_state) {

  $table_data = array();
  $limit = isset($_SESSION['limit_select']) ? $_SESSION['limit_select'] : 10;
  $columns = isset($form_state['values']['columns']) ? $form_state['values']['columns'] : array();
  $source = isset($form_state['values']['source']) ? $form_state['values']['source'] : TRUE;

  $header = array(// Define the tentative source header
    'nid' => array('data' => t('Node ID'), 'field' => 'n.nid'),
    'title' => array('data' => t('Title'), 'field' => 'n.title'),
    'content_type' => array('data' => t('Content Type'), 'field' => 'n.type'),
    'language' => array('data' => t('Source Uploaded'), 'field' => 'upload_status'),
    'translations' => array('data' => t('Translations'), 'field' => 't_current_c'),
    'configuration' => array('data' => t('Profile')),
    'workflow' => array('data' => t('Workflow'), 'field' => 'workflow'),
    'document_id' => array('data' => t('Doc ID'), 'field' => 'document_id'),
    'changed' => array('data' => t('Last Modified'), 'field' => 'changed', 'sort' => 'desc'),
    'last_uploaded' => array('data' => t('Last Uploaded'), 'field' => 'last_uploaded'),
//      'translation_progress_percent' => array('data' => t('Workflow Progress'), 'field' => 'translation_progress_percent'),
    'actions' => array('data' => t('Actions')),
    'translation_status' => array('data' => t('Translation Status')),
    'translation_progress' => array('data' => t('Translation Progress')),
    'locale_progress_percent' => array('data' => t('Target Progress'), 'field' => 'locale_progress_percent'),
    'progress_updated' => array('data' => t('Progress Updated'), 'field' => 'progress_updated'),
    'last_downloaded' => array('data' => t('Last Downloaded'), 'field' => 'last_downloaded'),
    'translate_link' => array('data' => t('Translate')),
  );

  foreach ($header as $title => $data) { // Refine the source header using the selected columns
    if (array_key_exists($title, $columns) && $columns[$title]) {
      $form_state['values']['grid_header'][$title] = $data;
    }
  }

  // Initialize Query and extend paginator and tablesort
  $query = db_select('node', 'n')
      ->extend('PagerDefault')
      ->extend('TableSort');

  $query->innerJoin('node', 'node2', '(n.nid = node2.nid) AND (node2.tnid = 0 OR node2.tnid = node2.nid)');

  $query->limit($limit);
  $query->orderByHeader($form_state['values']['grid_header']);

  // Node Title and Name of Content Type (type)
  $query->fields('n', array('nid', 'title', 'type', 'language', 'changed'));

  $query->addExpression("(SELECT COUNT(nid) FROM lingotek WHERE nid=n.nid AND lingokey LIKE 'target_sync_status_%' AND lingovalue='CURRENT')", 't_current_c');

  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(lingokey, 20, 10)) FROM lingotek WHERE nid=n.nid AND lingokey LIKE 'target_sync_status_%' AND lingovalue='PENDING')", 't_pending');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(lingokey, 20, 10)) FROM lingotek WHERE nid=n.nid AND lingokey LIKE 'target_sync_status_%' AND lingovalue='READY')", 't_ready');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(lingokey, 20, 10)) FROM lingotek WHERE nid=n.nid AND lingokey LIKE 'target_sync_status_%' AND lingovalue='CURRENT')", 't_current');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(lingokey, 20, 10)) FROM lingotek WHERE nid=n.nid AND lingokey LIKE 'target_sync_status_%' AND lingovalue='EDITED')", 't_edited');

  $filters = isset($_SESSION['grid_filters']) ? $_SESSION['grid_filters'] : array();

  global $language;

  if (language_default()->language != $language->language) {
    $query->leftJoin('field_data_title_field', 't_title', 't_title.entity_id = n.nid and t_title.entity_type = \'node\' and t_title.language=\'' . $language->language . '\'');
    $query->addField('t_title', 'title_field_value', 'localized_title');
  }

  // left joins are necessary here because some lingotek table keys might not exist
  // Lingotek Document ID
  $query->leftJoin('lingotek', 'lingo_document_id', 'lingo_document_id.nid = n.nid and lingo_document_id.lingokey = \'document_id\'');
  $query->addField('lingo_document_id', 'lingovalue', 'document_id');

  // Node Upload Status
  $query->leftJoin('lingotek', 'lingo_upload_status', 'lingo_upload_status.nid = n.nid and lingo_upload_status.lingokey = \'node_sync_status\' and lingo_upload_status.lingovalue <> \'' . LingotekSync::STATUS_TARGET . '\'');
  //$query->leftJoin('lingotek', 'lingo_upload_status', 'lingo_upload_status.nid = n.nid and lingo_upload_status.lingokey = \'node_sync_status\'');
  //$query->condition('lingo_upload_status.lingovalue', LingotekSync::STATUS_TARGET, '<>');
  $query->addField('lingo_upload_status', 'lingovalue', 'upload_status');

  // Profile Settings
  $query->leftJoin('lingotek', 'lingo_profile', 'lingo_profile.nid = n.nid and lingo_profile.lingokey = \'profile\'');
  $query->addField('lingo_profile', 'lingovalue', 'profile');

  // Last Uploaded Timestamp
  $query->leftJoin('lingotek', 'lingo_last_uploaded', 'lingo_last_uploaded.nid = n.nid and lingo_last_uploaded.lingokey = \'last_uploaded\'');
  $query->addField('lingo_last_uploaded', 'lingovalue', 'last_uploaded');

  // Translation Progress Percent (Average of all target workflow progresses)
//    $query->leftJoin('lingotek', 'lingo_translation_progress_percent', 'lingo_translation_progress_percent.nid = n.nid and lingo_translation_progress_percent.lingokey = \'translation_progress\'');
//      $query->addField('lingo_translation_progress_percent', 'lingovalue', 'translation_progress_percent');
  // Add translation status to the query
//    $query->leftJoin('lingotek', 'lingo_translation_status', 'n.nid = lingo_translation_status.nid and lingo_translation_status.lingokey LIKE \'target_sync_status_' . $language_filter . '\'');
//      $query->addField('lingo_translation_status', 'lingovalue', 'translation_status');
  // Add workflow to the query
  $query->leftJoin('lingotek', 'lingo_workflow', 'n.nid = lingo_workflow.nid and lingo_workflow.lingokey = \'workflow_id\'');
  $query->addField('lingo_workflow', 'lingovalue', 'workflow');

//    // Target Workflow Progress
//    $query->leftJoin('lingotek', 'lingo_locale_progress_percent', 'n.nid = lingo_locale_progress_percent.nid and lingo_locale_progress_percent.lingokey LIKE \'target_sync_progress_' . $language_filter . '\'');
//      $query->addField('lingo_locale_progress_percent', 'lingovalue', 'locale_progress_percent');
//
//    // Target Progress Last Updated Timestamp
//    $query->leftJoin('lingotek', 'lingo_progress_updated', 'n.nid = lingo_progress_updated.nid and lingo_progress_updated.lingokey LIKE \'target_sync_last_progress_updated_' . $language_filter . '\'');
//      $query->addField('lingo_progress_updated', 'lingovalue', 'progress_updated');
//
//    // Last Downloaded Timestamp
//    $query->leftJoin('lingotek', 'lingo_last_downloaded', 'n.nid = lingo_last_downloaded.nid and lingo_last_downloaded.lingokey LIKE \'target_last_downloaded_' . $language_filter . '\'');
//      $query->addField('lingo_last_downloaded', 'lingovalue', 'last_downloaded');

  if (isset($filters['search_type']) && $filters['search_type'] == 'all') {
    $filters['title'] = $filters['body'] = $filters['search'];
  }

  if (isset($filters['title'])) {
    $title_query = db_select('field_data_title_field', 'tf')
        ->distinct()
        ->fields('tf', array('entity_id'))
        ->condition('tf.title_field_value', '%' . $filters['title'] . '%', 'LIKE');
  }

  if (isset($filters['body'])) {
    $body_query = db_select('field_data_body', 'tb')
        ->distinct()
        ->fields('tb', array('entity_id'))
        ->condition('tb.body_value', '%' . $filters['body'] . '%', 'LIKE');
  }

  // START FILTERS
  //  Search
  if (isset($filters['search_type']) && $filters['search_type'] == 'all' && isset($filters['search']) && strlen($filters['search'])) {
    $or = db_or();

    $or->condition('n.title', '%' . $filters['search'] . '%', 'LIKE'); // is this redundant when the following line is added? 
    $or->condition('n.nid', $title_query, 'IN');
    $or->condition('n.nid', $body_query, 'IN');

    $query->condition($or);
  }
  else {
    //  Title Field
    if (isset($filters['title']) && $filters['title'] != '') {
      $query->condition('n.nid', $title_query, 'IN');
    }

    // Body Field
    if (isset($filters['body']) && $filters['body'] != '') {
      $query->condition('n.nid', $body_query, 'IN');
    }
  }

  //  Node ID
  if (isset($filters['nid']) && $filters['nid'] != '') {
    $query->condition('n.nid', $filters['nid']);
  }



  // Lingotek Document ID
  if (isset($filters['document_id']) && $filters['document_id'] != '') {
    if ($filters['document_id'] == 'None') {
      $query->condition('lingo_document_id.lingovalue', NULL);
    }
    else {
      $query->condition('lingo_document_id.lingovalue', $filters['document_id']);
    }
  }

  $array_fix = array('upload_status', 'content_type', 'auto_upload', 'auto_download',
    'crowdsourcing', 'url_alias', 'translation_status',
    'locale_progress_percent');

  foreach ($array_fix as $value) {
    if (isset($filters[$value]) && !is_array($filters[$value])) {
      $filters[$value] = array($filters[$value]);
    }
  }
  // Source Language
  if (isset($filters['source_language']) && $filters['source_language'] != 'all') {
    $query->condition('n.language', $filters['source_language']);
  }

  // Upload Status
  if (isset($filters['upload_status']) && !in_array('all', $filters['upload_status'])) {
    $query->condition('lingo_upload_status.lingovalue', $filters['upload_status'], 'IN');
  }

  //  Content Type
  if (isset($filters['content_type']) && !in_array('all', $filters['content_type'])) {
    $query->condition('n.type', $filters['content_type'], 'IN');
  }

  if (isset($filters['profile']) && $filters['profile'] != 'all') {
    $or = lingotek_profile_condition('n', 'lingo_auto_upload', 'lingo_profile', $filters['profile']);
    $query->condition($or);
  }

  // Last Uploaded
  if (isset($filters['last_uploaded']) && $filters['last_uploaded'] != 'all') {
    if ($filters['last_uploaded'] == '1 day') {
      $query->condition('lingo_last_uploaded.lingovalue', strToTime($filters['last_uploaded']), '<');
    }
    elseif ($filters['last_uploaded'] == 'unknown') {
      $query->condition('lingo_last_uploaded.lingovalue', NULL);
    }
    else {
      $params = explode(' ', $filters['last_uploaded'], 2); // string formatted like '< 1 week', so explode with a limit of two gives us array(0 => '<', 1 => '1 week')
      $query->condition('lingo_last_uploaded.lingovalue', strToTime($params[1]), $params[0]);
    }
  }

  // Translation Progress Percentage
//      if (isset($columns['translation_progress_percent']) && $columns['translation_progress_percent'] && isset($filters['translation_progress_percent']) && !in_array('all', $filters['translation_progress_percent'])) {
//        $or = db_or();
//        foreach ($filters['translation_progress_percent'] as $percent) {
//          if ($percent == 0) {
//            $or->condition('lingo_translation_progress_percent.lingovalue', $percent);
//            $or->condition('lingo_translation_progress_percent.lingovalue', NULL);
//          }
//          else {
//            $range = array((int)$percent - 24, (int)$percent);
//            $or->where('CAST(lingo_translation_progress_percent.lingovalue as UNSIGNED) BETWEEN ' . $range[0] . '  AND ' . $range[1]);
//          }
//        }
//        $query->condition($or);
//      }
  // Translation Status
//      if (isset($filters['translation_status']) && !empty($filters['translation_status']) && !in_array('all', $filters['translation_status'])) {
//        $or = db_or();
//        if (in_array('out_of_sync', $filters['translation_status'])) {
//          $or->condition('lingo_upload_status.lingovalue', LingotekSync::STATUS_EDITED);
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_PENDING);
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_READY);
//        }
//        if (in_array('in_sync', $filters['translation_status'])) {
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_CURRENT);
//        }
//        if (in_array('disabled', $filters['translation_status'])) {
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_DISABLED);
//        }
//        $query->condition($or);
//      }
  // Translation Progress
//      if (isset($filters['translation_progress']) && !empty($filters['translation_progress']) && !in_array('all', $filters['translation_progress'])) {
//        $or = db_or();
//        if (in_array('needs_upload', $filters['translation_progress'])) {
//          $or->condition('lingo_upload_status.lingovalue', LingotekSync::STATUS_EDITED);
//        }
//        if (in_array('in_progress', $filters['translation_progress'])) {
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_PENDING);
//        }
//        if (in_array('download', $filters['translation_progress'])) {
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_READY);
//        }
//        if (in_array('complete', $filters['translation_progress'])) {
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_CURRENT);
//        }
//        if (in_array('disabled', $filters['translation_progress'])) {
//          $or->condition('lingo_translation_status.lingovalue', LingotekSync::STATUS_DISABLED);
//        }
//        $query->condition($or);
//      }
  // Target Progress Percentage
//      if (isset($filters['locale_progress_percent']) && !in_array('all', $filters['locale_progress_percent'])) {
//        $or = db_or();
//        foreach ($filters['locale_progress_percent'] as $percent) {
//          if ($percent == 0) {
//            $or->condition('lingo_locale_progress_percent.lingovalue', $percent);
//            $or->condition('lingo_locale_progress_percent.lingovalue', NULL);
//          }
//          else {
//            $range = array((int)$percent - 24, (int)$percent);
//            $or->where('CAST(lingo_locale_progress_percent.lingovalue as UNSIGNED) BETWEEN ' . $range[0] . '  AND ' . $range[1]);
//          }
//        }
//        $query->condition($or);
//      }
  // Progress Last Updated
  if (isset($filters['progress_updated']) && $filters['progress_updated'] != 'all') {
    if ($filters['progress_updated'] == '1 day') {
      $query->condition('lingo_progress_updated.lingovalue', strToTime($filters['progress_updated']), '<');
    }
    elseif ($filters['progress_updated'] == 'unknown') {
      $query->condition('lingo_progress_updated.lingovalue', NULL);
    }
    else {
      $params = explode(' ', $filters['progress_updated'], 2); // string formatted like '< 1 week', so explode with a limit of two gives us array(0 => '<', 1 => '1 week')
      $query->condition('lingo_progress_updated.lingovalue', strToTime($params[1]), $params[0]);
    }
  }

  // Last Downloaded
  if (isset($filters['last_downloaded']) && $filters['last_downloaded'] != 'all') {
    if ($filters['last_downloaded'] == '1 day') {
      $query->condition('lingo_last_downloaded.lingovalue', strToTime($filters['last_downloaded']), '<');
    }
    elseif ($filters['last_downloaded'] == 'unknown') {
      $query->condition('lingo_last_downloaded.lingovalue', NULL);
    }
    else {
      $params = explode(' ', $filters['last_downloaded'], 2); // string formatted like '< 1 week', so explode with a limit of two gives us array(0 => '<', 1 => '1 week')
      $query->condition('lingo_last_downloaded.lingovalue', strToTime($params[1]), $params[0]);
    }
  }
  // END FILTERS
  // Execute the query
  $table_data_raw = $query->execute()->fetchAllAssoc('nid');
  lingotek_node_load($table_data_raw, array());

  $languages = language_list();
  $profiles = lingotek_get_profiles();
  
  $api = LingotekApi::instance();
  $workflows = $api->listWorkflows();
  
  $types = _node_types_build()->types;

  // Parse returned objects and make them arrays keyed by the Node ID for clean use in The Grid.
  foreach ($table_data_raw as $row) {
    // RENAMING
    $title_truncate_length = 55;
    if (strlen($row->title) > $title_truncate_length) { // very long title names make The Grid look messy, so we truncate them.
      $dots = '...';
      $dots_length = strlen($dots);
      $row->title = substr($row->title, 0, $title_truncate_length - $dots_length) . $dots;
    }
    $icon = '';
    if ($row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED) {
      $row->upload_status = LingotekSync::STATUS_DISABLED;
    }
    switch ($row->upload_status) {
      case LingotekSync::STATUS_EDITED:
        $icon = '<i class="fa fa-square-o" title="Needs to be uploaded"></i>';
        break;
      case LingotekSync::STATUS_CURRENT:
        $icon = '<i class="fa fa-check-square" title="Uploaded to Lingotek"></i>';
        break;
      case LingotekSync::STATUS_DISABLED:
        $icon = '<i class="fa fa-minus-square" style="color: #999;" title="Lingotek is disabled"></i>';
        break;
      default:
        $icon = '<i class="fa fa-minus-square" style="color: #999;" title="Unknown upload status"></i>';
        break;
    }

    $list_statuses = array('pending', 'ready', 'current', 'edited');

    $locales = array();
    $statuses = array();

    foreach ($list_statuses as $status) {
      $key = 't_' . $status;
      foreach (explode(',', $row->$key) as $l) {
        if (!empty($l)) {
          $locales[] = $l;
          $statuses[] = $status;
        }
      }
    }

    array_multisort($locales, SORT_ASC, $statuses);

    $translations = lingotek_lang_icons($languages, $locales, $statuses, $row->nid, TRUE);

    $configuration = '';

    $disabled = $row->upload_status == LingotekSync::STATUS_DISABLED ? ' lingotek-disabled' : '';

    if ($row->lingotek['create_lingotek_document']) {
      $configuration .= '<i class="fa fa-arrow-up' . $disabled . '" title="Automatic Upload"></i> ';
    }
    if ($row->lingotek['sync_method']) {
      $configuration .= '<i class="fa fa-arrow-down' . $disabled . '" title="Automatic Download"></i> ';
    }
    if ($row->lingotek['allow_community_translation']) {
      $configuration .= '<i class="fa fa-globe' . $disabled . '" title="Crowdsourcing Enabled"></i> ';
    }

    if ($row->lingotek['profile'] == LingotekSync::PROFILE_CUSTOM) {
      $configuration = t('Custom') . ' <span class="node-configuration">' . $configuration . '</span>';
    }
    elseif ($row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED) {
      $configuration = t('Disabled');
    }
    else {
      $profile_key = $row->lingotek['profile']; //dpm($profile_key); dpm($profiles);
      if (array_key_exists($profile_key, $profiles)) {
        $configuration = $profiles[$profile_key]['name'];
      }
      else {
        $configuration = t('Unknown');// profile id does not exist in profiles (this state should be unobtainable)
      }
    }


    $source = '<span class="lingotek-language-source" style="white-space: nowrap;">' . $icon . ' ' . ($row->language == 'und' ? t('None') : $languages[$row->language]->name) . '</span>';

    if (empty($row->upload_status) || $row->upload_status == LingotekSync::STATUS_EDITED) {
      $source = '<span title="Upload Now" class="ltk-upload-button lingotek-language-source" onclick="lingotek_perform_action(' . $row->nid . ',\'upload\')">' . $source . '</span>';
    }

    $pm_icon = ' <i title="' . t('View Translations') . '" class="fa fa-tasks' . $disabled . '"></i>';
    $pm_link = ' ' . l($pm_icon, 'node/' . $row->nid . '/lingotek_pm', array('html' => TRUE, 'attributes' => array('target' => '_blank')));

    $actions = '';
    $actions .= l('<i title="' . t('Edit Node') . '" class="fa fa-edit"></i>', 'node/' . $row->nid . '/edit', array('html' => TRUE, 'attributes' => array('target' => '_blank')));
    $actions .= ' ' . l('<i title="' . t('Set Lingotek Settings') . '" class="fa fa-gear"></i>', '', array('html' => TRUE, 'attributes' => array('onclick' => 'lingotek_perform_action(' . $row->nid . ',"edit"); return false;')));
    $actions .= empty($disabled) ? $pm_link : $pm_icon;

    $no_localized_title = (language_default()->language != $language->language) && $row->localized_title == '';

    // Build the data to be output for this row
    $data = array(
      'nid' => $row->nid ? : t('??'),
      'title' => $row->nid ? ($no_localized_title ? '<span class="no-localized-title">' : '') . l((isset($row->localized_title) ? $row->localized_title : $row->title), 'node/' . $row->nid, array('attributes' => array('target' => '_blank'))) . ($no_localized_title ? '</span>' : '') : t('N/A'),
      'language' => $source,
      'translations' => $translations,
      'configuration' => $configuration,
      'document_id' => $row->document_id ? : t('N/A'),
      'content_type' => $types[$row->type]->name ? : t('??'),
//        'translation_status' => $row->upload_status ?: t('N/A'),
//        'translation_progress' => $translation_progress ?: t('N/A'),
      'changed' => $row->changed ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->changed))) : t('Never'),
      'last_uploaded' => $row->last_uploaded ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->last_uploaded))) : t('Never'),
//        'locale_progress_percent' => $row->locale_progress_percent ? lingotek_grid_create_progress_bar($row->locale_progress_percent) : lingotek_grid_create_progress_bar(0),
//        'progress_updated' => $row->progress_updated ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->progress_updated))) : t('Never'),
//        'last_downloaded' => $row->last_downloaded ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->last_downloaded))) : t('Never'),
      'workflow' => ($row->workflow ? check_plain($workflows[$row->workflow]) : ''),
      'actions' => '<span class="lingotek-node-actions">' . $actions . '</span>',
    );

    $table_data[$row->nid] = $data;
  }

  return $table_data;
}

function lingotek_lang_icons($languages, $locales, $statuses, $nid = NULL, $convert = FALSE) {
  $html = "";

  $legend = array(
    LingotekSync::STATUS_PENDING => 'In Progress',
    LingotekSync::STATUS_READY => 'Ready to Download',
    LingotekSync::STATUS_CURRENT => 'Current',
    LingotekSync::STATUS_EDITED => 'Source needs to be uploaded',
    LingotekSync::STATUS_DISABLED => 'Disabled',
  );
  $default_link = 'node/' . $nid . '/lingotekworkbench/';
  $manual_link = 'node/' . $nid . '/lingotek_pm/';
  $link_map = array(
    LingotekSync::STATUS_PENDING => $default_link,
    LingotekSync::STATUS_READY => $default_link,
    LingotekSync::STATUS_CURRENT => $default_link,
    LingotekSync::STATUS_EDITED => $manual_link,
    LingotekSync::STATUS_DISABLED => $manual_link,
  );

  foreach ($locales as $key => $original_locale) {
    $locale = $convert ? Lingotek::convertLingotek2Drupal($original_locale) : $original_locale;
    if ($locale) { // exclude language translations that are not lingotek_enabled (we may consider showing everything later)
      $status = $statuses[$key];
      $css_class = 'target-' . $status;
      $tip = $languages[$locale]->name . ' - ' . $legend[strtoupper($status)];

      $html .= l($locale, $link_map[strtoupper($status)] . $original_locale, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => array('language-icon', $css_class))));
    }
  }
  
  return $html;
}

/**
 * Helper function for creating the HTML for a progress bar
 *
 * @param int $progress
 *    Percentage complete for the progress bar
 *
 * @return string $html
 *    Raw HTML for a progress bar themed in style/base.css
 */
function lingotek_grid_create_progress_bar($progress) {
  $style = "width:" . round($progress) . "%;";
  $html = '<div class="lingotek-progress"><div class="bar" style="' . $style . '"></div><div class="percent">' . $progress . '%' . '</div></div>';
  return $html;
}

/**
 * Callback function to disassociate translations for multiple nodes at a time
 *
 * Node IDs are passed through the $_SESSION variable at $_SESSION['lingotek_disassociate_nodes']
 *
 * Returns a fully rendered html form
 */
function lingotek_disassociate_nodes($nids) {
  $second_run = !empty($form_state['executed']);
  
  $nids = explode(',', $nids);
  
  if (count($nids) > 1 && !$second_run) {
    drupal_set_message(t('You will be disassociating translations for @number nodes.', array('@number' => count($nids))), 'warning');
  }
  
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');
  
  $form_state = array(
    'ajax' => TRUE,
    'nids' => $nids,
  );
  $output = ctools_modal_form_wrapper('lingotek_node_disassociate_form', $form_state);
  
  if (!empty($form_state['executed'])) {
    // Create ajax command array, dismiss the modal window.
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    drupal_exit();
  }
    
  print ajax_render($output);
}

/**
 * Callback function to edit settings for multiple nodes at a time
 *
 * Node IDs are passed through the $_SESSION variable at $_SESSION['lingotek_edit_nodes']
 *
 * Returns a fully rendered html form
 */
function lingotek_edit_nodes($nids) {
  $nids = explode(',', $nids);

  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');
    
  $form_state = array(
    'ajax' => TRUE,
    'nids' => $nids,
  );
  $output = ctools_modal_form_wrapper('lingotek_get_node_settings_form', $form_state);
  
  if (!empty($form_state['executed'])) {
    // Create ajax command array, dismiss the modal window.
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    drupal_exit();
  }
    
  print ajax_render($output);
}

/**
 * Callback function to change workflow for multiple nodes at a time
 *
 * Node IDs are passed through the $_SESSION variable at $_SESSION['lingotek_change_workflow']
 *
 * Returns a fully rendered html form
 */
function lingotek_change_workflow($nids) {
  $nids = explode(',', $nids);

  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');
    
  $form_state = array(
    'ajax' => TRUE,
    'nids' => $nids,
  );
  $output = ctools_modal_form_wrapper('lingotek_get_change_workflow_form', $form_state);
  
  if (!empty($form_state['executed'])) {
    // Create ajax command array, dismiss the modal window.
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    drupal_exit();
  }
    
  print ajax_render($output);
}
