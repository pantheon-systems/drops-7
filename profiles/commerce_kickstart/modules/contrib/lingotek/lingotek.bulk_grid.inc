<?php

/**
 * @file
 * Bulk Grid form
 */

function lingotek_manage_callback() {
  drupal_goto('admin/settings/lingotek/manage/node');
}

function lingotek_bulk_grid_form($form, $form_state) {
  $entity_type = arg(4);
  $entity_type = empty($entity_type)? 'node': $entity_type;

  global $language;

  if (isset($_SESSION['grid_entity_type']) && $_SESSION['grid_entity_type'] != $entity_type) {
    $_SESSION['grid_entity_type'] = $entity_type;
    lingotek_grid_clear_filters();
  }
  elseif (!isset($_SESSION['grid_entity_type'])) {
    $_SESSION['grid_entity_type'] = $entity_type;
  }

  /*
   * Here we store or retrieve the GET parameters so that the state of the table is maintained when leaving and coming back
   * Also makes it so the state is not lost when performing bulk actions
   */
  if (count($_GET) == 1 && isset($_SESSION['grid_custom_parameters']) && !empty($_SESSION['grid_custom_parameters'])) {
    $_SESSION['grid_custom_parameters']['preventloop'] = TRUE;
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type, array('query' => $_SESSION['grid_custom_parameters']));
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
    'upload' => t('Upload source for translation'),
    'sync' => t('Check progress of translations'),
    'reset' => t('Disassociate translations'),
    'delete' => t('Delete selected content'),
    'edit' => t('Edit translation settings'),
    'workflow' => t('Change workflow'),
    t('Download') => array('download_all' => t('Download All Translations')),
  );

  $target_languages_raw = Lingotek::getLanguages();

  foreach ($target_languages_raw as $target_raw) {
    $action_options[t('Download')]['download_' . $target_raw->lingotek_locale] = t('Download') . ' ' . t($target_raw->name) . ' (' . $target_raw->lingotek_locale . ') ' . t('Translation');
  }

  $form['lingotek-console'] = array(
    '#markup' => '<div id="lingotek-console"></div>',
  );

  $form['entity_type'] = array(
    '#type' => 'hidden',
    '#value' => $entity_type,
  );

  $page = pager_find_page() + 1; // Get current page from url
  $limit_select = (isset($_SESSION['limit_select']) ? (int)$_SESSION['limit_select'] : 0);

  $total_entity_rows = lingotek_grid_get_rows($entity_type, $form, $form_state, TRUE);

  if ((int)($page - 1) * $limit_select > $total_entity_rows) {
    // reset the page to be the last set of results
    $page = 1;
    $_SESSION['grid_custom_parameters']['page'] = 0;
    if (isset($_GET['page'])) {
      $_GET['page'] = 0; // used by PagerDefault class to get page number
    }
  }

  $filter_set = FALSE;

  if (isset($_SESSION['grid_filters'])) {
    foreach ($_SESSION['grid_filters'] as $key => $value) {
      if (is_array($value)) {
        $keys = array_keys($value);
        if (count($keys) == 1) {
          if ($keys[0] !== '' && $keys[0] !== 'all') {
            $filter_set = TRUE;
          }
        }
        elseif (count($keys) > 1) {
          $filter_set = TRUE;
        }
      }
      else {
        // $value === '0' accounts for the case of the automatic profile
        if ((!empty($value) && $value !== 'all') || $value === '0') {
          $filter_set = TRUE;
        }
      }
    }
  }

  // Run query to get table rows
  $table_data = lingotek_grid_get_rows($entity_type, $form, $form_state);

  $results_first = (($page - 1) * $limit_select) + 1;
  $results_last = $results_first + count($table_data) - 1;

  $form['customize'] = array(
    '#markup' => l('<i class="fa fa-list-alt fa-2x" ></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/customize', array('html' => TRUE, 'attributes' => array('title' => t('Customize Table'), 'class' => array('ctools-use-modal', 'ctools-modal-large', 'lingotek-action')))),
  );
  /*$form['filter_popup'] = array(
    '#markup' => l('<i class="fa-search fa-2x"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters', array('html' => TRUE, 'attributes' => array('title' => 'Set Filters', 'class' => array('ctools-use-modal', 'ctools-modal-large', 'lingotek-action')))),
  );*/
  $last_updated = variable_get('lingotek_pending_last_updated', NULL);
  $message = t('Check status of in-progress translations (Last checked @time)', array('@time' => $last_updated ? lingotek_human_readable_timestamp($last_updated) . ' ' . t('ago') : t('Never')));

  $form['lingotek_update'] = array(
    '#markup' => l('<i class="fa fa-cloud-download fa-2x ltk-download"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/download-ready/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Download complete translations'), 'class' => array('lingotek-action')))),
  );

  $form['refresh'] = array(
    '#markup' => l('<i class="fa fa-refresh fa-2x ltk-refresh"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/update/' . $entity_type, array('html' => TRUE, 'attributes' => array('class' => 'lingotek-action', 'title' => $message))),
  );

  $form['lingotek_upload'] = array(
    '#markup' => l('<i class="fa fa-cloud-upload fa-2x ltk-upload"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/upload-edited/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Upload all pending source content'), 'class' => array('lingotek-action')))),
  );

  $form['edit_settings'] = array(
    '#markup' => l(t('Edit Settings'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/edit/' . $entity_type, array('attributes' => array('id' => 'edit-settings-link', 'class' => array('ctools-use-modal', 'ctools-modal-large')))),

  );

  $form['disassociate_translations'] = array(
    '#markup' => l(t('Disassociate translations'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/reset/' . $entity_type, array('attributes' => array('id' => 'reset-link', 'class' => array('ctools-use-modal', 'ctools-modal-small', 'ltk-hidden-modal-trigger')))),
  );
  $form['delete'] = array(
    '#markup' => l(t('Delete selected content'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/delete/' . $entity_type, array('attributes' => array('id' => 'delete-link', 'class' => array('ctools-use-modal', 'ctools-modal-small', 'ltk-hidden-modal-trigger')))),
  );
  $form['pop_up_link'] = array(
    '#markup' => l(t('Hidden pop-up'), '', array('attributes' => array('id' => 'popup-link', 'class' => array('ctools-use-modal', 'ctools-modal-large', 'ltk-hidden-modal-trigger')))),
  );

  $api = LingotekApi::instance();
  $workflows = $api->listWorkflows();
  if (is_array($workflows) && count($workflows) > 1) {
    $form['change_workflow'] = array(
      '#markup' => l(t('Change Workflow'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/change-workflow/' . $entity_type, array('attributes' => array('id' => 'change-workflow-link', 'class' => array('ctools-use-modal', 'ctools-modal-large')))),
    );
  }

  if ($entity_type == 'node') {
    $form['node/add'] = array(
      '#markup' => l('<i class="fa fa-plus" style="margin-left: 15px; padding-right: 4px;"></i>' . t('Add content') . '<div style="margin: 15px 0;"></div>', 'node/add', array('html' => TRUE, 'attributes' => array('title' => t('Add content'))))
    );
  }

  $form['search'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($_SESSION['grid_filters']['search']) ? $_SESSION['grid_filters']['search'] : '',
    '#title' => l('<i class="fa fa-search"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters', array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => array('ctools-use-modal', 'ctools-modal-large', 'ltk-action')))) . ' ' . t('Search') . ': ',
    '#size' => 30,
  );

  $search_options = array(
    'all' => t('All'),
    'title' => t('Title'),
    'body' => t('Body'),
  );

  $form['search_type'] = array(
    '#type' => 'select',
    '#options' => $search_options,
    '#default_value' => isset($_SESSION['grid_filters']['search_type']) ? $_SESSION['grid_filters']['search_type'] : 'all',
  );

  $form['search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
    '#submit' => array('lingotek_grid_filter_inline_submit'),
  );

  $form['advanced_link'] = array(
    '#markup' => l(t('Advanced') . '<span style="margin: 0 5px;"></span>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters', array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => array('ctools-use-modal', 'ctools-modal-large', 'ltk-action'))))
  );

  if ($filter_set) {
    $form['filter_message'] = array(
      '#markup' => '<span style="white-space:nowrap;">' . l('<i class="fa fa-times" style="margin: 0 5px;"></i>' . t('Clear Filters') . '</span>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters/clear/' . $entity_type, array('html' => TRUE)) . '</span>',
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
    'small' => array(
      'modalSize' => array(
        'type' => 'fixed',
        'width' => 450,
        'height' => 400,
      ),
      'closeImage' => theme('image', array('path' => drupal_get_path('module', 'lingotek') . '/images/close.png', 'alt' => t('Close window'), 'title' => t('Close window'))),
      'animation' => 'fadeIn',
    ),
  ), 'setting');

  drupal_add_js(array(
    'large' => array(
      'modalSize' => array(
        'type' => 'scale',
        'width' => .6,
        'height' => .8,
      ),
      'closeImage' => theme('image', array('path' => drupal_get_path('module', 'lingotek') . '/images/close.png', 'alt' => t('Close window'), 'title' => t('Close window'))),
      'animation' => 'fadeIn',
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

  if ($results_last > 0) {
    $form['count'] = array(
      '#markup' => '<span class="grid-result-summary">'
      . t('Displaying @first - @last', array('@first' => $results_first, '@last' => $results_last))
      . ($filter_set ? (' (' . t('filtered results') . ')') : '') . '</span>',
    );
  }

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

function lingotek_popup($form_id, $entity_type = 'node', $entity_ids = array(), $extra = "") {
  $second_run = !empty($form_state['executed']);
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $entity_ids = !is_array($entity_ids) ? explode(',', $entity_ids) : $entity_ids;

  $form_state = array(
    'ajax' => TRUE,
    'entity_ids' => $entity_ids,
    'entity_type' => $entity_type,
  );

  $output = ctools_modal_form_wrapper('lingotek_' . $form_id . '_form', $form_state);

  if (!empty($form_state['executed'])) {
    lingotek_grid_action_submit(NULL, $form_state);
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
      if (strpos($key, '__filter')) {
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
}

/**
 * Submit function for The Grid's actions
 * The action corresponds to the key of the option selected
 * Often redirects to batch operations or to other pages entirely
 */
function lingotek_grid_action_submit($form, $form_state) {
  $entity_ids = array();
  $entity_type = isset($form_state['entity_type']) ? $form_state['entity_type'] : (isset($form_state['values']['entity_type']) ? $form_state['values']['entity_type'] : NULL);

  if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#name'] == 'actions_submit') { // If submitting an action
    foreach ($form_state['values']['the_grid'] as $value) {
      if ($value != 0) {
        $entity_ids[] = $value;
      }
    }

    if (isset($form_state['values']['actions_select'])) { // If an action was selected (which it would be, I don't know if this could ever NOT occur with normal use)
      $action = $form_state['values']['actions_select']; // Get the action
      if (count($entity_ids) <= 0) { // Select a node
        drupal_set_message(t('You must select at least one node to @action.', array('@action' => $action)), 'warning'); // Or pay the price
      }
      elseif ($action == 'upload') { // If uploading
        $batch = array(
          'title' => t('Uploading Content To Lingotek'),
          'finished' => 'lingotek_sync_upload_node_finished'
        );
        $operations = lingotek_get_sync_upload_batch_elements($entity_type, $entity_ids);
        $batch['operations'] = $operations;
        $redirect = current_path();

        batch_set($batch);
        batch_process($redirect); // Run batch operations to upload all of the selected nodes to Lingotek
      }
      elseif (substr($action, 0, 8) == 'download') { // If downloading all targets
        $locale = substr($action, 9, 10);
        $target_locales = ($locale == 'all') ? lingotek_get_target_locales() : array($locale);
        lingotek_grid_download_selected($entity_type, $entity_ids, $target_locales);
      }
      elseif ($action == 'delete' || $action == 'reset') {
        // ajax ctools modal employed (see lingotek_bulk_grid_form() and lingotek.bulk_grid.js)
      }
      elseif ($action == 'edit') { // If editing node settings
        drupal_goto(LINGOTEK_MENU_MAIN_BASE_URL . '/manage/edit/'); // Redirect to the edit Lingotek node settings form - for multiple nodes, form defaults are global defaults
      }
      elseif ($action == 'workflow') { // If changing the workflow
        drupal_goto(LINGOTEK_MENU_MAIN_BASE_URL . '/manage/change-workflow/'); // Redirect to the change-workflow settings form - for multiple nodes, form defaults are global defaults
      }
      elseif ($action == 'sync') { // If syncing the progress
        lingotek_update_target_progress_batch_create($entity_type, $entity_ids); // Run batch operations to get the progress report from Lingotek
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
      '#title' => t('ID'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'nid' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'nid' . $suffix] : in_array('nid', $columns['defaults']),
    ),
    'content_type' => array(
      '#type' => 'checkbox',
      '#title' => t('Content Type'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'content_type' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'content_type' . $suffix] : in_array('content_type', $columns['defaults']),
    ),
    'title' => array(
      '#type' => 'checkbox',
      '#title' => t('Title'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'title' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'title' . $suffix] : in_array('title', $columns['defaults']),
    ),
    'language' => array(
      '#type' => 'checkbox',
      '#title' => t('Source Uploaded'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'language' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'language' . $suffix] : in_array('language', $columns['defaults']),
    ),
    'translations' => array(
      '#type' => 'checkbox',
      '#title' => t('Translations'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'translations' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'translations' . $suffix] : in_array('translations', $columns['defaults']),
    ),
    'configuration' => array(
      '#type' => 'checkbox',
      '#title' => t('Profile'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'configuration' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'configuration' . $suffix] : in_array('configuration', $columns['defaults']),
    ),
    'document_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Doc ID'),
      '#default_value' => isset($_SESSION['grid_custom'][$prefix . 'document_id' . $suffix]) ? $_SESSION['grid_custom'][$prefix . 'document_id' . $suffix] : in_array('document_id', $columns['defaults']),
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
      '#title' => t('Last Uploaded'),
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

function lingotek_grid_update($entity_type) {
  $nids = LingotekSync::getEntityIdsByStatusAndTarget($entity_type, LingotekSync::STATUS_PENDING);
  variable_set('lingotek_pending_last_updated', time());
  if (count($nids) > 0) {
    $_SESSION['lingotek_sync_nodes'] = count($nids);
    lingotek_update_target_progress_batch_create($entity_type, $nids); // Run batch operations to sync all 'In Progress' nodes
  }
  else {
    drupal_set_message(t('There are no in-progress translations to check.'));
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
  }
}

function lingotek_grid_download_selected($entity_type, $entity_ids, $target_locales) {
  $operations = array();
  $entities = entity_load($entity_type, $entity_ids);
  foreach ($entities as $entity) {
    foreach ($target_locales as $locale) {
      $operations[] = array('lingotek_entity_download', array($entity, $entity_type, $locale));
    }
  }
  $redirect = 'admin/settings/lingotek/manage/' . $entity_type;
  $batch = array(
    'title' => t('Downloading Translations'),
    'finished' => 'lingotek_sync_download_target_finished',
  );
  $batch['operations'] = $operations;
  batch_set($batch);
  batch_process($redirect);
}

function lingotek_grid_download_ready($entity_type) {
  $targets = LingotekSync::getTargetsByStatus($entity_type, LingotekSync::STATUS_READY);

  if (!empty($targets)) {

    $entity_ids = array();
    foreach ($targets as $target) {
      $entity_ids[] = $target['id'];
    }
    $entities = entity_load($entity_type, $entity_ids);

    $operations = array();
    foreach ($targets as $target) {
      if ($entities[$target['id']]->lingotek['profile'] != LingotekSync::PROFILE_DISABLED) { // exclude nodes with PROFILE_DISABLED
        $operations[] = array('lingotek_entity_download', array($target['id'], $entity_type, $target['locale']));
      }
    }
    $redirect = 'admin/settings/lingotek/manage/' . $entity_type;
    $batch = array(
      'title' => t('Downloading Translations'),
      'finished' => 'lingotek_sync_download_target_finished',
    );
    $batch['operations'] = $operations;
    batch_set($batch);
    batch_process($redirect);
  }
  else {
    drupal_set_message(t('There are no translations ready for download.'));
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
  }
}

function lingotek_grid_upload_edited($entity_type) {
  $entity_ids = LingotekSync::getEntityIdsToUpload($entity_type);

  if (!empty($entity_ids)) {
    $operations = array();
    foreach ($entity_ids as $id) {
      $operations[] = array('lingotek_entity_upload', array($id, $entity_type));
    }
    $redirect = 'admin/settings/lingotek/manage/' . $entity_type;
    $batch = array(
      'title' => t('Uploading documents'),
      'finished' => 'lingotek_sync_upload_node_finished',
      'operations' => $operations,
    );
    batch_set($batch);
    batch_process($redirect);
  }
  else {
    drupal_set_message(t('There are no translations ready for upload.'));
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
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
  $source_languages = array('all' => t('All Languages'));
  foreach ($languages as $code => $language) {
    $source_languages[$code] = t($language->name) . ' (' . $language->lingotek_locale . ')';
  }

  $profiles = array();
  $profiles['all'] = 'All';
  $profiles[LingotekSync::PROFILE_CUSTOM] = t('Custom');
  $profiles[LingotekSync::PROFILE_DISABLED] = t('Disabled');

  $profile_defaults = lingotek_get_profiles();
  foreach ($profile_defaults as $key => $p) {
    $profiles[$key] = $p['name'];
  }

  $filters = array(
    'nid' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['nid']) ? $_SESSION['grid_filters']['nid'] : '',
      '#title' => t('Entity ID is'),
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
      '#multiple' => TRUE
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
      ),
      '#multiple' => FALSE,
    )
  );

  // Include a content-type filter if there are bundles by which to filter.
  $entity_info = entity_get_info($_SESSION['grid_entity_type']);
  if (!empty($entity_info['bundles'])) {
    $grid_bundles = array_map(function($val) {
      return $val['label'];
    }, $entity_info['bundles']);
    $filters['content_type'] = array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['content_type']) ? $_SESSION['grid_filters']['content_type'] : 'all',
      '#title' => t('Content Type(s)'),
      '#options' => array('all' => t('All')) + $grid_bundles,
      '#multiple' => TRUE
    );
  }

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

function lingotek_grid_clear_filters_page($entity_type) {
  lingotek_grid_clear_filters();
  drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
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
function lingotek_grid_get_rows($entity_type, $form, &$form_state, $count_only = FALSE) {
  $info = entity_get_info($entity_type);
  $entity_id_key = $info['entity keys']['id'];
  $eid = 'n.' . $entity_id_key;

  $base_table = $info['base table'];

  $entity_properties = array_flip($info['schema_fields_sql']['base table']);

  $label_col  = isset($info['entity keys']['label']) && $info['entity keys']['label'] ? $info['entity keys']['label'] : NULL;
  $bundle_col = isset($info['entity keys']['bundle']) && $info['entity keys']['bundle'] && isset($entity_properties[$info['entity keys']['bundle']]) ? $info['entity keys']['bundle'] : NULL;
  // All managed entity types should have a language column.
  // Field collection entities have one, but it is called 'langcode'.
  // Message type entities have one, and it is called 'language'; but currently it does not appear in their entity keys.
  // So, the $language_col looks for the correct language field, and then guesses it is 'language' if it doesn't find one.
  $language_col = isset($info['entity keys']['language']) && $info['entity keys']['language'] ? $info['entity keys']['language'] : 'language';

  $table_data = array();
  $limit = isset($_SESSION['limit_select']) ? $_SESSION['limit_select'] : 10;
  $columns = isset($form_state['values']['columns']) ? $form_state['values']['columns'] : array();
  $source = isset($form_state['values']['source']) ? $form_state['values']['source'] : TRUE;

  $header = array(// Define the tentative source header
    'nid' => array('data' => t('ID'), 'field' => 'n.' . $entity_id_key),
    'content_type' => array('data' => t('Content Type'), 'field' => 'type'),
    'title' => array('data' => t('Title'), 'field' => 'n.' . $label_col),
    'language' => array('data' => t('Source Uploaded')),//, 'field' => 'upload_status'),
    'translations' => array('data' => t('Translations'), 'field' => 't_current_c'),
    'configuration' => array('data' => t('Profile')),
    'workflow' => array('data' => t('Workflow'), 'field' => 'workflow'),
    'document_id' => array('data' => t('Doc ID'), 'field' => 'document_id'),
    'changed' => array('data' => t('Last Modified'), 'field' => 'changed'),
    'last_uploaded' => array('data' => t('Last Uploaded'), 'field' => 'last_uploaded'),
    'actions' => array('data' => t('Actions')),
    'translation_status' => array('data' => t('Translation Status')),
    'last_downloaded' => array('data' => t('Last Downloaded'), 'field' => 'last_downloaded'),
    'translate_link' => array('data' => t('Translate')),
  );

  if (!isset($entity_properties['changed'])) {
    unset($header['changed']);
  }
  if (!$label_col) {
    unset($header['title']);
  }
  // Taxonomy terms require special handling of bundles because they do not
  // have a bundle column in their table.
  if (!$bundle_col && $entity_type != 'taxonomy_term') {
    unset($header['content_type']);
  }

  if (isset($entity_properties['changed'])) {
    $header['changed']['sort'] = 'desc';
  }

  foreach ($header as $title => $data) { // Refine the source header using the selected columns
    if (array_key_exists($title, $columns) && $columns[$title]) {
      $form_state['values']['grid_header'][$title] = $data;
    }
  }

  // Initialize Query and extend paginator and tablesort if necessary
  if ($count_only) {
    $query = db_select('{' . $base_table . '}', 'n');
  }
  else {
    $query = db_select('{' . $base_table . '}', 'n')
        ->extend('PagerDefault')
        ->extend('TableSort');
    $query->limit($limit)
        ->orderByHeader($form_state['values']['grid_header']);
  }
  if ($base_table == 'node') {
    $query->innerJoin('{node}', 'node2', '(n.nid = node2.nid) AND (node2.tnid = 0 OR node2.tnid = node2.nid)');
  }

  // Entity Title and Name of Content Type (type)
  $query->fields('n', array($entity_id_key));

  if ($label_col) {
    $query->addField('n', $label_col, 'title');
  }

  if ($language_col) {
    $query->addField('n', $language_col, 'language');
  }

  if (isset($entity_properties['changed'])) {
    $query->addField('n', 'changed');
  }

  if ($entity_type == 'comment') {
    $query->leftJoin('{node}', 'nn', 'nn.nid = n.nid');
    $query->addExpression("CONCAT('comment_node_',nn.type)", 'type');
    $query->addExpression("CONCAT('comment_node_',nn.type)", 'node_type');
  }
  elseif ($entity_type == 'taxonomy_term') {
    $query->innerJoin('{taxonomy_vocabulary}', 'tv', 'n.vid = tv.vid');
    $query->addField('tv', 'name', 'tv_name');
    $query->addField('tv', 'machine_name', 'type');
  }
  elseif ($bundle_col) {
    if ($info['entity keys']['bundle'] != 'type') {
      $query->addField('n', $info['entity keys']['bundle']);
    }
    $query->addField('n', $info['entity keys']['bundle'], 'type');
  }

  $query->addExpression("(SELECT COUNT(entity_id) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='CURRENT')", 't_current_c');

  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='PENDING')", 't_pending');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='READY')", 't_ready');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='CURRENT')", 't_current');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='EDITED')", 't_edited');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='UNTRACKED')", 't_untracked');

  $filters = isset($_SESSION['grid_filters']) ? $_SESSION['grid_filters'] : array();

  global $language;
  $localized_label_table = 'field_data_' . $label_col . '_field';

  if ($label_col && db_table_exists($localized_label_table)) {
    $query->leftJoin('{' . $localized_label_table . '}', 't_title', 't_title.entity_id = ' . $eid . ' and t_title.entity_type = \'' . $entity_type . '\' and t_title.language=\'' . $language->language . '\'');
    $query->addField('t_title', $label_col . '_field_value', 'localized_title');
  }

  // left joins are necessary here because some lingotek table keys might not exist
  // Lingotek Document ID
  $query->leftJoin('{lingotek_entity_metadata}', 'lingo_document_id', 'lingo_document_id.entity_type =\'' . $entity_type . '\' AND lingo_document_id.entity_id = ' . $eid . ' and lingo_document_id.entity_key = \'document_id\'');
  $query->addField('lingo_document_id', 'value', 'document_id');

  // Entity Upload Status
  $query->leftJoin('{lingotek_entity_metadata}', 'lingo_upload_status', 'lingo_upload_status.entity_type =\'' . $entity_type . '\' AND lingo_upload_status.entity_id = ' . $eid . ' and lingo_upload_status.entity_key = \'node_sync_status\' and lingo_upload_status.value <> \'' . LingotekSync::STATUS_TARGET . '\'');
  //$query->leftJoin('{lingotek_entity_metadata}', 'lingo_upload_status', 'lingo_upload_status.nid = ' . $eid . ' and lingo_upload_status.entity_key = \'node_sync_status\'');
  //$query->condition('lingo_upload_status.value', LingotekSync::STATUS_TARGET, '<>');
  $query->addField('lingo_upload_status', 'value', 'upload_status');

  // Profile Settings
  $query->leftJoin('{lingotek_entity_metadata}', 'lingo_profile', 'lingo_profile.entity_type =\'' . $entity_type . '\' AND lingo_profile.entity_id = ' . $eid . ' and lingo_profile.entity_key = \'profile\'');
  $query->addField('lingo_profile', 'value', 'profile');

  // Last Uploaded Timestamp
  $query->leftJoin('{lingotek_entity_metadata}', 'lingo_last_uploaded', 'lingo_last_uploaded.entity_type =\'' . $entity_type . '\' AND lingo_last_uploaded.entity_id = ' . $eid . ' and lingo_last_uploaded.entity_key = \'last_uploaded\'');
  $query->addField('lingo_last_uploaded', 'value', 'last_uploaded');

  // Any Upload Errors
  $query->leftJoin('{lingotek_entity_metadata}', 'lingo_last_sync_error', 'lingo_last_sync_error.entity_type =\'' . $entity_type . '\' AND lingo_last_sync_error.entity_id = ' . $eid . ' and lingo_last_sync_error.entity_key = \'last_sync_error\'');
  $query->addField('lingo_last_sync_error', 'value', 'last_sync_error');

  $query->leftJoin('{lingotek_entity_metadata}', 'lingo_workflow', '' . $eid . ' = lingo_workflow.entity_id and lingo_workflow.entity_type =\'' . $entity_type . '\' and lingo_workflow.entity_key = \'workflow_id\'');
  $query->addField('lingo_workflow', 'value', 'workflow');

  if (isset($filters['search_type']) && $filters['search_type'] == 'all') {
    $filters['title'] = $filters['body'] = $filters['search'];
  }

  if (isset($filters['title'])) {
    $title_query = db_select('{field_data_title_field}', 'tf')
        ->distinct()
        ->fields('tf', array('entity_id'))
        ->condition('tf.title_field_value', '%' . $filters['title'] . '%', 'LIKE');
  }

  if (isset($filters['body'])) {
    $body_query = db_select('{field_data_body}', 'tb')
        ->distinct()
        ->fields('tb', array('entity_id'))
        ->condition('tb.body_value', '%' . $filters['body'] . '%', 'LIKE');
  }

  // START FILTERS
  //  Search
  if (isset($filters['search_type']) && $filters['search_type'] == 'all' && isset($filters['search']) && strlen($filters['search'])) {
    $or = db_or();

    if (!empty($label_col)) {
      $or->condition('n.' . $label_col, '%' . $filters['search'] . '%', 'LIKE'); // is this redundant when the  following line is added?
    }
    $or->condition('' . $eid . '', $title_query, 'IN');
    $or->condition('' . $eid . '', $body_query, 'IN');

    $query->condition($or);
  }
  else {
    //  Title Field
    if (isset($filters['title']) && $filters['title'] != '') {
      $query->condition('' . $eid . '', $title_query, 'IN');
    }

    // Body Field
    if (isset($filters['body']) && $filters['body'] != '') {
      $query->condition('' . $eid . '', $body_query, 'IN');
    }
  }

  //  Entity ID
  if (isset($filters['nid']) && $filters['nid'] != '') {
    $query->condition('' . $eid . '', $filters['nid']);
  }

  // Lingotek Document ID
  if (isset($filters['document_id']) && $filters['document_id'] != '') {
    if ($filters['document_id'] == 'None') {
      $query->condition('lingo_document_id.value', NULL);
    }
    else {
      $query->condition('lingo_document_id.value', $filters['document_id']);
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
    $query->condition('n.' . $info['entity keys']['language'], $filters['source_language']);
  }

  // Upload Status
  if (isset($filters['upload_status']) && !in_array('all', $filters['upload_status'])) {
    $query->condition('lingo_upload_status.value', $filters['upload_status'], 'IN');
  }

  //  Content Type
  if (isset($filters['content_type']) && !in_array('all', $filters['content_type'])) {
    // Special-case handling of taxonomy_term pseudo-entities
    if ($entity_type == 'taxonomy_term') {
      $query->condition('tv.machine_name', $filters['content_type'], 'IN');
    }
    else {
      $query->condition('n.' . $info['entity keys']['bundle'], $filters['content_type'], 'IN');
    }
  }

  if (isset($filters['profile']) && !in_array('all', $filters['profile'])) {
    $profiled_entities = lingotek_get_entities_by_profile_and_entity_type($filters['profile'], $entity_type);
    $profiled_entity_ids = array(-1);
    foreach ($profiled_entities as $p) {
      $profiled_entity_ids[] = $p['id'];
    }
    $query->condition('n.' . $info['entity keys']['id'], $profiled_entity_ids, 'IN');
    //$or = lingotek_profile_condition($base_table, 'n', 'lingo_profile', $filters['profile']);
    //$query->condition($or);
  }

  // Last Uploaded
  if (isset($filters['last_uploaded']) && $filters['last_uploaded'] != 'all') {
    if ($filters['last_uploaded'] == '1 day') {
      $query->condition('lingo_last_uploaded.value', strToTime($filters['last_uploaded']), '<');
    }
    elseif ($filters['last_uploaded'] == 'unknown') {
      $query->condition('lingo_last_uploaded.value', NULL);
    }
    else {
      $params = explode(' ', $filters['last_uploaded'], 2); // string formatted like '< 1 week', so explode with a limit of two gives us array(0 => '<', 1 => '1 week')
      $query->condition('lingo_last_uploaded.value', strToTime($params[1]), $params[0]);
    }
  }

  // Last Downloaded
  if (isset($filters['last_downloaded']) && $filters['last_downloaded'] != 'all') {
    if ($filters['last_downloaded'] == '1 day') {
      $query->condition('lingo_last_downloaded.value', strToTime($filters['last_downloaded']), '<');
    }
    elseif ($filters['last_downloaded'] == 'unknown') {
      $query->condition('lingo_last_downloaded.value', NULL);
    }
    else {
      $params = explode(' ', $filters['last_downloaded'], 2); // string formatted like '< 1 week', so explode with a limit of two gives us array(0 => '<', 1 => '1 week')
      $query->condition('lingo_last_downloaded.value', strToTime($params[1]), $params[0]);
    }
  }
  // END FILTERS

  // If count only, return the count
  if ($count_only) {
    $result = $query->execute();
    return $result->rowCount();
  }

  // Execute the query
  $table_data_raw = $query->execute()->fetchAllAssoc($entity_id_key);
  lingotek_entity_load($table_data_raw, $entity_type);

  $languages = language_list();
  $profiles = lingotek_get_profiles();

  $api = LingotekApi::instance();
  $workflows = $api->listWorkflows();

  $types = _node_types_build()->types;

  // Parse returned objects and make them arrays keyed by the Node ID for clean use in The Grid.
  foreach ($table_data_raw as $row) {
    $entity_id = $row->{$entity_id_key};

    // RENAMING
    $title_truncate_length = 55;
    if (strlen($row->title) > $title_truncate_length) { // very long title names make The Grid look messy, so we truncate them.
      $dots = '...';
      $dots_length = strlen($dots);
      $row->title = substr($row->title, 0, $title_truncate_length - $dots_length) . $dots;
    }
    $icon = '';
    if ($row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED) {
      $icon = '<i class="fa fa-minus-square" style="color: #999;" title="' . t('Lingotek is disabled') . '"></i>';
    }
    elseif (!is_null($row->upload_status)) {
      switch ($row->upload_status) {
        case LingotekSync::STATUS_EDITED:
          $icon = '<i class="fa fa-square-o" title="' . t('Needs to be uploaded') . '"></i>';
          break;
        case LingotekSync::STATUS_CURRENT:
          $icon = '<i class="fa fa-check-square" title="' . t('Uploaded to Lingotek') . '"></i>';
          break;
        case LingotekSync::STATUS_PENDING:
          $icon = '<i class="fa fa-square" title="' . t('Upload to Lingotek in progress') . '"></i>';
          break;
        case LingotekSync::STATUS_FAILED:
          $error = $row->last_sync_error ? $row->last_sync_error : '';
          $icon = '<i class="fa fa-warning" style="color: darkorange;" title="' . t('Lingotek processing failed @error', array('@error' => ': ' . $error)) . '"></i>';
          break;
        default:
          $icon = '<i class="fa fa-question-circle" style="color: darkorange;" title="' . t('Unknown upload status') . ': ' . check_plain($row->upload_status) . '"></i>';
          break;
      }
    }
    else {
      $icon = '<i class="fa fa-square-o" title="' . t('Needs to be uploaded') . '"></i>';
    }

    $list_statuses = array('pending', 'ready', 'current', 'edited', 'untracked');

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

    $disabled = $row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED;

    $translations = lingotek_lang_icons($entity_type, $languages, $locales, $statuses, $entity_id, $disabled, $row->language); // show translation statuses

    $configuration = '';

    $disabled_class = $disabled ? ' ltk-disabled-icon' : '';

    if ($row->lingotek['create_lingotek_document']) {
      $configuration .= '<i class="fa fa-arrow-up' . $disabled_class . '" title="' . t('Automatic Upload') . '"></i> ';
    }
    if ($row->lingotek['sync_method']) {
      $configuration .= '<i class="fa fa-arrow-down' . $disabled_class . '" title="' . t('Automatic Download') . '"></i> ';
    }
    if ($row->lingotek['allow_community_translation']) {
      $configuration .= '<i class="fa fa-globe' . $disabled_class . '" title="' . t('Crowdsourcing Enabled') . '"></i> ';
    }

    if ($row->lingotek['profile'] == LingotekSync::PROFILE_CUSTOM) {
      $configuration = t('Custom') . ' <span class="node-configuration">' . $configuration . '</span>';
    }
    elseif ($row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED) {
      $configuration = t('Disabled');
    }
    else {
      $profile_key = $row->lingotek['profile'];
      if (array_key_exists($profile_key, $profiles)) {
        $configuration = t($profiles[$profile_key]['name']);
      }
      else {
        $configuration = t('Unknown');// profile id does not exist in profiles (this state should be unobtainable)
      }
    }

    // Source

    $source = '<span class="lingotek-language-source" style="white-space: nowrap;">' . $icon . ' ';
    if ($language_col) {
      $source .= ($row->language == LANGUAGE_NONE ? t('Language Neutral') : t($languages[$row->language]->name));
    }
    $source .= '</span>';
    $linked_statuses = array(LingotekSync::STATUS_EDITED, LingotekSync::STATUS_FAILED);
    $needs_upload = (in_array($row->upload_status, $linked_statuses) || empty($row->upload_status)) && $row->lingotek['profile'] != LingotekSync::PROFILE_DISABLED;
    if ($needs_upload) { // add link for upload
      $source = '<span title="Upload Now" class="ltk-upload-button lingotek-language-source" onclick="lingotek_perform_action(' . $entity_id . ',\'upload\')">' . $source . '</span>';
    }

    $pm_icon = ($entity_type == 'node') ? ' <i class="fa fa-tasks' . $disabled_class . '"></i>' : '';
    $pm_link = ' ' . l($pm_icon, 'node/' . $entity_id . '/lingotek_pm', array('html' => TRUE, 'attributes' => array('title' => t('View Translations'), 'target' => '_blank')));


    $actions = '';
    $actions .= lingotek_get_entity_edit_link($entity_type, $entity_id);
    $actions .= empty($disabled_class) ? $pm_link : $pm_icon;
    $actions .= ' ' . l('<i title="' . t('Set Lingotek Settings') . '" class="fa fa-gear"></i>', '', array('html' => TRUE, 'attributes' => array('onclick' => 'lingotek_perform_action(' . $entity_id . ',"edit"); return false;')));

    $no_localized_title = (language_default()->language != $language->language) && (!isset($row->localized_title) or $row->localized_title == '');

    if ($label_col) {
      $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $row) : '';
      $uri['options']['attributes'] = array('target' => '_blank');
      $title_to_show = (isset($row->localized_title) ? $row->localized_title : $row->title);
      $title = ($no_localized_title ? '<span class="no-localized-title">' : '');
      if (isset($uri['path'])) {
        $title .= l($title_to_show, $uri['path'], $uri['options']);
      }
      else {
        $title .= $title_to_show;
      }
      $title .= ($no_localized_title ? '</span>' : '');
    }
    else {
      $title = '';
    }

    // Build the data to be output for this row
    $data = array(
      'nid' => $row->{$entity_id_key} ? : t('??'),
      'title' => $title,
      'language' => $source,
      'translations' => $translations,
      'configuration' => $configuration,
      'document_id' => $row->document_id ? : t('N/A'),
      'content_type' => isset($info['bundles'][$row->type]['label']) ? $info['bundles'][$row->type]['label'] : $row->type,
      'last_uploaded' => $row->last_uploaded ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->last_uploaded))) : t('Never'),
      'workflow' => ($row->lingotek['workflow_id'] ? (isset($workflows[$row->lingotek['workflow_id']]) ? check_plain($workflows[$row->lingotek['workflow_id']]) : $row->lingotek['workflow_id']) : ''),
      'actions' => '<span class="lingotek-node-actions">' . $actions . '</span>',
    );

    if (isset($entity_properties['changed'])) {
      $data['changed'] = $row->changed ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->changed))) : t('Never');
    }
    $table_data[$entity_id] = $data;
  }

  return $table_data;
}

function lingotek_get_entity_edit_link($entity_type, $entity_id, $url_only = FALSE) {
  $edit_link = "";
  $query = array();
  $query['destination'] = 'admin/settings/lingotek/manage/' . $entity_type;
  switch ($entity_type) {
    case 'fieldable_panels_pane':
      $edit_link = 'admin/structure/fieldable-panels-panes/view/' . $entity_id . '/edit';
      break;
    case 'taxonomy_term':
      $edit_link = 'taxonomy/term/' . $entity_id . '/edit';
      break;
    case 'message_type':
      return '';
    default:
      $edit_link = $entity_type . '/' . $entity_id . '/edit';
      break;
  }
  return $url_only ? url($edit_link, array('query' => $query)) : l('<i title="' . t('Edit') . '" class="fa fa-edit"></i>', $edit_link, array('html' => TRUE, 'attributes' => array('target' => '_blank'), 'query' => $query));
}

function lingotek_lang_icons($entity_type, $languages, $locales, $statuses, $entity_id = NULL, $disabled = FALSE, $source_language = NULL) {
  $html = "";

  $legend = array(
    LingotekSync::STATUS_PENDING => t('In Progress'),
    LingotekSync::STATUS_READY => t('Ready to Download'),
    LingotekSync::STATUS_CURRENT => t('Current'),
    LingotekSync::STATUS_EDITED => t('Not Current'),
    LingotekSync::STATUS_UNTRACKED => t('Untracked'),
    ''
  );
  $default_link = 'lingotek/workbench/' . $entity_type . '/' . $entity_id . '/';
  $manual_link = $entity_type . '/' . $entity_id . '/lingotek_pm/';
  $untracked_link = 'lingotek/view/' . $entity_type . '/' . $entity_id . '/';

  $link_map = array(
    LingotekSync::STATUS_PENDING => $default_link,
    LingotekSync::STATUS_READY => $default_link,
    LingotekSync::STATUS_CURRENT => $default_link,
    LingotekSync::STATUS_EDITED => $manual_link,
    LingotekSync::STATUS_UNTRACKED => $untracked_link,
  );

  $lingotek_languages = Lingotek::getLanguages();

  foreach ($locales as $key => $original_locale) {
    if (key_exists($original_locale, $lingotek_languages)) {
      $locale_enabled = $lingotek_languages[$original_locale]->lingotek_enabled; //array_key_exists($locale, $lingotek_languages) ? $lingotek_languages[$locale]->lingotek_enabled : FALSE;
      $lang_code = $lingotek_languages[$original_locale]->language;
      $lingotek_locale = $lingotek_languages[$original_locale]->lingotek_locale;
      if (!is_null($source_language) && $lang_code === $source_language) {
        continue; // hide source language targets (shouldn't exist anyways)
      }
      if ($locale_enabled) { // exclude language translations that are not lingotek_enabled (we may consider showing everything later)
        $status = strtoupper($statuses[$key]);
        $status = $disabled && strtoupper($status) != LingotekSync::STATUS_CURRENT ? LingotekSync::STATUS_UNTRACKED : $status;
        $css_classes = 'target-' . strtolower($status);
        $css_classes .= $disabled ? ' target-disabled' : '';
        $tip = $languages[$lang_code]->name . ' - ' . $legend[$status];
        $status = $disabled ? LingotekSync::STATUS_UNTRACKED : $status; // all disabled entities should link to view page (not workbench)
        $html .= l($lang_code, $link_map[$status] . $lingotek_locale, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => array('language-icon', $css_classes))));
      }
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
function lingotek_disassociate_nodes($entity_type, $entity_ids) {
  $second_run = !empty($form_state['executed']);

  $entity_ids = !is_array($entity_ids) ? explode(',', $entity_ids) : $entity_ids;

  if (count($entity_ids) > 1 && !$second_run) {
    drupal_set_message(format_plural(count($entity_ids), 'You will be disassociating translations for one entity.', 'You will be disassociating translations for @count entities'), 'warning');
  }

  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'entity_ids' => $entity_ids,
    'entity_type' => $entity_type,
  );
  $output = ctools_modal_form_wrapper('lingotek_entity_disassociate_form', $form_state);

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
function lingotek_edit_nodes($entity_type, $nids) {
  $nids = explode(',', $nids);

  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'nids' => $nids,
    'entity_type' => $entity_type,
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
function lingotek_change_workflow($entity_type, $nids) {
  $nids = explode(',', $nids);

  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'entity_type' => $entity_type,
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

/**
 * Form constructor for the entity delete.
 */
function lingotek_entity_delete_form($form, &$form_state, $nids = array(), $collapse_fieldset = TRUE) {

  if (isset($form_state['entity_ids'])) {
    $entity_ids = $form_state['entity_ids'];
    $collapse_fieldset = FALSE;
  }

  $form = array();
  if (!is_array($entity_ids)) {
    $entity_ids = array($entity_ids);
  }
  $entity_type = $form_state['entity_type'];

  $content_details_string = $entity_type . ' ' . t("entity ids") . ': ' . implode(", ", $entity_ids) . "<br/>";

  $form['confirm'] = array(
    '#type' => 'fieldset',
    '#prefix' => format_plural(count($entity_ids), '', '<div class="messages warning">' . t('This action will apply to @count entities.') . '</div>'),
    '#title' => t('Are you sure you want to delete these entities?'),
    '#description' => $content_details_string,
    '#suffix' => '<i>' . t("Note: this action cannot be undone.") . '</i>',
    '#collapsible' => $collapse_fieldset,
    '#collapsed' => $collapse_fieldset,
  );

  $form['confirm']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );

  $form['entity_ids'] = array(
    '#type' => 'hidden',
    '#value' => json_encode($entity_ids),
  );

  return $form;
}

/**
 * Submit handler for the lingotek_entity_delete form.
 */
function lingotek_entity_delete_form_submit($form, &$form_state) {
  if (isset($form_state['values']['entity_ids'])) {
    $drupal_entity_ids_json = json_decode($form_state['values']['entity_ids']);
  }
  elseif (isset($form_state['entity_ids'])) {
    $drupal_entity_ids_json = json_decode($form_state['entity_ids']);
  }

  $entity_type = isset($form_state['entity_type']) ? $form_state['entity_type'] : 'node';
  $entity_ids = array();
  foreach ($drupal_entity_ids_json as $id) {
    $entity_ids[] = $id;
  }

  if (!empty($entity_ids)) {
    entity_delete_multiple($entity_type, $entity_ids);
    $count = count($entity_ids);
    watchdog('content', format_plural($count, 'Deleted 1 entity.', 'Deleted @count entities.'));
    drupal_set_message(format_plural($count, 'Deleted 1 entity.', 'Deleted @count entities.'));
    $form_state['executed'] = TRUE;
  }
  $form_state['redirect'] = LINGOTEK_MENU_MAIN_BASE_URL . '/manage';
  return lingotek_grid_filter_submit($form, $form_state);
}

/**
 * Form constructor for the entity disassociate form. (Formerly "Reset Translations")
 */
function lingotek_entity_disassociate_form($form, $form_state, $entity_ids = array(), $collapse_fieldset = TRUE) {

  if (isset($form_state['entity_ids']) && empty($entity_ids)) {
    $entity_ids = $form_state['entity_ids'];
    $collapse_fieldset = FALSE;
  }

  $form = array();
  if (!is_array($entity_ids)) {
    $entity_ids = array($entity_ids);
  }

  $form['disassociate_translations'] = array(
    '#type' => 'fieldset',
    '#prefix' => format_plural(count($entity_ids), '', '<div class="messages warning">' . t('This action will apply to @count entities.') . '</div>'),
    '#title' => t('Disassociate Translations'),
    '#description' => t("Disassociates the entity translations on Lingotek's servers from the copies downloaded to Drupal. Additional translation using Lingotek will require re-uploading the entity to restart the translation process."),
    '#suffix' => '<i>' . t("Note: This action should only be used to change the Lingotek project or TM vault associated with the entities.") . '<i>',
    '#collapsible' => $collapse_fieldset,
    '#collapsed' => $collapse_fieldset,
  );

  $form['disassociate_translations']['confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('Also remove all document(s) from Lingotek TMS'),
    '#default_value' => FALSE,
    );

  $form['disassociate_translations']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Disassociate Translations'),
  );

  $form['entity_ids'] = array(
    '#type' => 'hidden',
    '#value' => json_encode($entity_ids),
  );

  return $form;
}

/**
 * Submit handler for the lingotek_entity_disassociate form.
 */
function lingotek_entity_disassociate_form_submit($form, $form_state) {

  if (isset($form_state['values']['entity_ids'])) {
    $drupal_entity_ids_json = json_decode($form_state['values']['entity_ids']);
  }
  elseif (isset($form_state['entity_ids'])) {
    $drupal_entity_ids_json = json_decode($form_state['entity_ids']);
  }
  $entity_type = isset($form_state['entity_type']) ? $form_state['entity_type'] : 'node';
  $entity_ids = array();
  foreach ($drupal_entity_ids_json as $id) {
    $entity_ids[] = $id;
  }
  $doc_ids = LingotekSync::getDocIdsFromEntityIds($entity_type, $entity_ids);

  $api = LingotekApi::instance();
  $remove_from_tms = $form_state['values']['confirm'];
  if ($remove_from_tms) { //disassociate on TMS
    $result = lingotek_batch_disassociate_content_worker($api, $doc_ids);
    if (!$result) {
      drupal_set_message(t('Failed to remove documents from Lingotek TMS.'), 'status', FALSE);
      return;
    }
  }
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'document_id');
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'node_sync_status');
  foreach ($entity_ids as $entity_id) {
    LingotekSync::setAllTargetStatus($entity_type, $entity_id, LingotekSync::STATUS_UNTRACKED);
  }

  drupal_set_message(format_plural(count($entity_ids), 'Translations disassociated for one entity.', 'Translations disassociated for @count entities.'));
}
