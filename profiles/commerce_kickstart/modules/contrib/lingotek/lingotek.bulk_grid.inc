<?php

/**
 * @file
 * Bulk Grid form
 */
include_once('lingotek.config.inc');

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
  lingotek_notify_if_no_languages_added();

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
  $form_state['values']['columns'] = lingotek_grid_get_columns($entity_type);
  $form_state['values']['grid_header'] = array();

  // Define source actions - keys are used to decide what action to do in the 'lingotek_grid_action_submit' function
  $action_options = lingotek_grid_action_options($entity_type);

  $form['lingotek-console'] = array(
    '#markup' => '<div id="lingotek-console"></div>',
  );

  $form['entity_type'] = array(
    '#type' => 'hidden',
    '#value' => $entity_type,
  );

  $page = pager_find_page() + 1; // Get current page from url
  $limit_select = (isset($_SESSION['limit_select']) ? (int)$_SESSION['limit_select'] : 0);

  if ($entity_type == 'config') {
    $total_entity_rows = lingotek_config_get_rows($entity_type, $form, $form_state, TRUE);
  }
  else {
    $total_entity_rows = lingotek_grid_get_rows($entity_type, $form, $form_state, TRUE);
  }

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
  $table_data = $entity_type == 'config' ? lingotek_config_get_rows($entity_type, $form, $form_state) : lingotek_grid_get_rows($entity_type, $form, $form_state);

  $results_first = (($page - 1) * $limit_select) + 1;
  $results_last = $results_first + count($table_data) - 1;

  $form['customize'] = array(
    '#markup' => l('<i class="fa fa-list-alt fa-2x" ></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/customize/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Customize Table'), 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-large', 'lingotek-action')))),
  );
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
    '#markup' => l(t('Edit Settings'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/edit/' . $entity_type, array('attributes' => array('id' => 'edit-settings-link', 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-large')))),

  );

  $form['disassociate_translations'] = array(
    '#markup' => l(t('Disassociate translations'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/reset/' . $entity_type, array('attributes' => array('id' => 'reset-link', 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-small', 'ltk-hidden-modal-trigger')))),
  );
  $form['delete'] = array(
    '#markup' => l(t('Delete selected content'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/delete/' . $entity_type, array('attributes' => array('id' => 'delete-link', 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-small', 'ltk-hidden-modal-trigger')))),
  );
  $form['pop_up_link'] = array(
    '#markup' => l(t('Hidden pop-up'), '', array('attributes' => array('id' => 'popup-link', 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-large', 'ltk-hidden-modal-trigger')))),
  );

  $api = LingotekApi::instance();
  $workflows = $api->listWorkflows();
  if (is_array($workflows) && count($workflows) > 1) {
    $form['change_workflow'] = array(
      '#markup' => l(t('Change Workflow'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/change-workflow/' . $entity_type, array('attributes' => array('id' => 'change-workflow-link', 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-large')))),
    );
  }

  if ($entity_type == 'node') {
    $form['node/add'] = array(
      '#markup' => l('<i class="fa fa-plus" style="margin-left: 15px; padding-right: 4px;"></i>' . t('Add content') . '<div style="margin: 15px 0;"></div>', 'node/add', array('html' => TRUE, 'attributes' => array('title' => t('Add content'))))
    );
  }

  $modal_size = $entity_type == 'config' ? 'ctools-modal-lingotek-small' : 'ctools-modal-lingotek-large';
  $modal_classes = array('ctools-use-modal', $modal_size, 'ltk-action');
  $search_title = l('<i class="fa fa-search"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => $modal_classes))) . ' ' . t('Search') . ': ';

  $form['search'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($_SESSION['grid_filters']['search']) ? $_SESSION['grid_filters']['search'] : '',
    '#title' => filter_xss($search_title),
    '#size' => 30,
  );

  $search_options = $entity_type == 'config' ? lingotek_config_search_options() : lingotek_grid_search_options();

  $grid_term = isset($_SESSION['grid_filters']['search_type']) ? $_SESSION['grid_filters']['search_type'] : 'all';
  $config_term = isset($_SESSION['grid_filters']['textgroup']) ? $_SESSION['grid_filters']['textgroup'] : 'all';
  $search_term = $entity_type == 'config' ? $config_term : $grid_term;

  $form['search_type'] = array(
    '#type' => 'select',
    '#options' => $search_options,
    '#default_value' => $search_term,
  );

  $config_search_submit = array('lingotek_config_filter_inline_submit');
  $grid_search_submit = array('lingotek_grid_filter_inline_submit');
  $search_submit = $entity_type == 'config' ? $config_search_submit : $grid_search_submit;

  $form['search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
    '#submit' => $search_submit,
  );

  $form['advanced_link'] = array(
    '#markup' => l(t('Advanced') . '<span style="margin: 0 5px;"></span>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => $modal_classes)))
  );

  if ($filter_set) {
    $form['filter_message'] = array(
      '#markup' => '<span style="white-space:nowrap;">' . l('<i class="fa fa-times" style="margin: 0 5px;"></i>' . t('Clear Filters') . '</span>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/clear/filters/' . $entity_type, array('html' => TRUE)) . '</span>',
    );
  }

  // Build actions selector
  $form['actions_select'] = array(
    '#type' => 'select',
    '#options' => $action_options,
    '#title' => '<i class="fa fa-asterisk ltk-muted"></i>' . ' ' . t('Actions') . ': ',
  );

  $actions_submit_function = $entity_type == 'config' ? array('lingotek_config_action_submit') : array('lingotek_grid_action_submit');

  // Actions submit button
  $form['actions_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Action'),
    '#name' => 'actions_submit',
    '#submit' => $actions_submit_function,
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
    'lingotek-small' => array(
      'modalSize' => array(
        'type' => 'fixed',
        'width' => 450,
        'height' => 450,
      ),
      'closeImage' => theme('image', array('path' => drupal_get_path('module', 'lingotek') . '/images/close.png', 'alt' => t('Close window'), 'title' => t('Close window'))),
      'closeText' => '',
      'animation' => 'fadeIn',
    ),
  ), 'setting');

  drupal_add_js(array(
    'lingotek-large' => array(
      'modalSize' => array(
        'type' => 'scale',
        'width' => .6,
        'height' => .8,
      ),
      'closeImage' => theme('image', array('path' => drupal_get_path('module', 'lingotek') . '/images/close.png', 'alt' => t('Close window'), 'title' => t('Close window'))),
      'closeText' => '',
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

function lingotek_grid_action_options($entity_type) {
  $delete_text = $entity_type == 'config' ? t('Delete selected translations') : t('Delete selected content');
  $action_options = array(
    'select' => t('Select an action'),
    'upload' => t('Upload source for translation'),
    'sync' => t('Check progress of translations'),
    'reset' => t('Disassociate translations'),
    'delete' => $delete_text,
  );

  if ($entity_type == 'config') {
    $action_options[t('Download')] = array('download_all' => t('Download All Translations'));
  }
  else {
    $action_options['edit'] = t('Edit translation settings');

    $api = LingotekApi::instance();
    $workflows = $api->listWorkflows();
    if (is_array($workflows) && count($workflows) > 1) {
      $action_options['workflow'] = t('Change workflow');
    }

    $action_options[t('Download')] = array('download_all' => t('Download All Translations'));
  }

  $target_languages_raw = Lingotek::getLanguages();

  foreach ($target_languages_raw as $target_raw) {
    $action_options[t('Download')]['download_' . $target_raw->lingotek_locale] = t('Download') . ' ' . t($target_raw->name) . ' (' . $target_raw->lingotek_locale . ') ' . t('Translation');
  }
  return $action_options;
}

function lingotek_grid_search_options() {
  $search_options = array(
    'all' => t('All'),
    'title' => t('Title'),
    'body' => t('Body'),
  );

  return $search_options;
}

function lingotek_config_search_options() {
  $search_options = array(
    'all' => t('All text groups'),
    'blocks' => t('Blocks'),
    'interface' => t('Built-in interface'),
    'field' => t('Field labels'),
    'menu' => t('Menu'),
    'misc' => t('Miscellaneous'),
    'taxonomy' => t('Taxonomy'),
    'views' => t('Views'),
  );

  return $search_options;
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

  $form['filter_fieldset']['filters'] += $form_state['entity_type'] == 'config' ? lingotek_config_build_filters($form_state) : lingotek_grid_build_filters($form_state);

  return $form;
}

function lingotek_filters_popup($entity_type) {
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form = array();
  $form_state = array(
    'ajax' => TRUE,
    'entity_type' => $entity_type,
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

  $form_state['values']['columns'] = $form_state['entity_type'] == 'config' ? lingotek_grid_get_columns('config') : lingotek_grid_get_columns();
  $form['customize_table_fieldset']['custom_columns'] += $form_state['entity_type'] == 'config' ? lingotek_config_build_column_checkboxes($form_state) : lingotek_grid_build_column_checkboxes($form_state);

  return $form;
}

function lingotek_grid_customize($entity_type) {
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'entity_type' => $entity_type,
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
  // we have to add some of these keys to the session because otherwise they are not saved after the form submission
  if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#name'] != 'op') {
    $_SESSION['button'] = $form_state['clicked_button']['#name'];
  }
  if (!isset($form_state['values'])) {
    return;
  }
  foreach ($form_state['values'] as $key => $value) {
    $add_key_to_session = FALSE;
    $nest = NULL;
    if (strpos($key, '__filter')) {
      $add_key_to_session = TRUE;
      $nest = 'grid_filters'; //stored in $_SESSION['grid_filters'][$key]
    }
    elseif (strpos($key, '__custom')) {
      $add_key_to_session = TRUE;
      $nest = $form_state['entity_type'] . '_custom'; //stored in $_SESSION['grid_custom'][$key]
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
  $entity_type = $form_state['entity_type'];
  $columns = lingotek_grid_define_columns($entity_type); // Allowed columns and defaults for source and target grids are defined here
  $column_elements = array(
    'nid' => array(
      '#type' => 'checkbox',
      '#title' => t('ID'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'nid' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'nid' . $suffix] : in_array('nid', $columns['defaults']),
    ),
    'content_type' => array(
      '#type' => 'checkbox',
      '#title' => t('Content Type'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'content_type' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'content_type' . $suffix] : in_array('content_type', $columns['defaults']),
    ),
    'title' => array(
      '#type' => 'checkbox',
      '#title' => t('Title'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'title' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'title' . $suffix] : in_array('title', $columns['defaults']),
    ),
    'description' => array(
      '#type' => 'checkbox',
      '#disabled' => 'true',
      '#title' => t('Description'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'nid' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'description' . $suffix] : in_array('description', $columns['defaults']),
    ),
    'language' => array(
      '#type' => 'checkbox',
      '#title' => t('Source Uploaded'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'language' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'language' . $suffix] : in_array('language', $columns['defaults']),
    ),
    'translations' => array(
      '#type' => 'checkbox',
      '#title' => t('Translations'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'translations' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'translations' . $suffix] : in_array('translations', $columns['defaults']),
    ),
    'configuration' => array(
      '#type' => 'checkbox',
      '#title' => t('Profile'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'configuration' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'configuration' . $suffix] : in_array('configuration', $columns['defaults']),
    ),
    'document_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Doc ID'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'document_id' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'document_id' . $suffix] : in_array('document_id', $columns['defaults']),
    ),
    'workflow' => array(
      '#type' => 'checkbox',
      '#title' => t('Workflow'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'workflow' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'workflow' . $suffix] : in_array('workflow', $columns['defaults']),
    ),
    'changed' => array(
      '#type' => 'checkbox',
      '#title' => t('Last Modified'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'changed' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'changed' . $suffix] : in_array('changed', $columns['defaults']),
    ),
    'last_uploaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Last Uploaded'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'last_uploaded' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'last_uploaded' . $suffix] : in_array('last_uploaded', $columns['defaults']),
    ),
    'locale_progress_percent' => array(
      '#type' => 'checkbox',
      '#title' => t('Target Progress Percentage'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'locale_progress_percent' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'locale_progress_percent' . $suffix] : in_array('locale_progress_percent', $columns['defaults']),
    ),
    'progress_updated' => array(
      '#type' => 'checkbox',
      '#title' => t('Progress Last Updated'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'progress_updated' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'progress_updated' . $suffix] : in_array('progress_updated', $columns['defaults']),
    ),
    'last_downloaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Time Last Downloaded'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'last_downloaded' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'last_downloaded' . $suffix] : in_array('last_downloaded', $columns['defaults']),
    ),
    'actions' => array(
      '#type' => 'checkbox',
      '#title' => t('Actions'),
      '#default_value' => isset($_SESSION[$entity_type . '_custom'][$prefix . 'actions' . $suffix]) ? $_SESSION[$entity_type . '_custom'][$prefix . 'actions' . $suffix] : in_array('actions', $columns['defaults']),
    ),
  );
  if ($form_state['entity_type'] == 'taxonomy_term') {
    $column_elements['description']['#disabled'] = 'true';
    $column_elements['title']['#disabled'] = 'true';
  }
  $column_elements = array_intersect_key($column_elements, $columns['columns']); // Reduces the output columns to the defaults specified in 'lingotek_grid_define_columns'

  return lingotek_grid_process_elements($column_elements, $prefix, $suffix); // adds prefixes and suffixes to the elements
}

function lingotek_config_build_column_checkboxes($form_state) {
  $prefix = '';
  $suffix = '__custom'; // Suffix specified because the filter submit function differentiates based on this tag and puts the keys into the session variable as such
  $columns = lingotek_config_define_columns(); // Allowed columns and defaults for source and target grids are defined here
  $column_elements = array(
    'lid' => array(
      '#type' => 'checkbox',
      '#title' => t('ID'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'lid' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'lid' . $suffix] : in_array('lid', $columns['defaults']),
    ),
    'set_name' => array(
      '#type' => 'checkbox',
      '#title' => t('Config Set Name'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'set_name' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'set_name' . $suffix] : in_array('set_name', $columns['defaults']),
    ),
    'source' => array(
      '#type' => 'checkbox',
      '#title' => t('Source'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'source' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'source' . $suffix] : in_array('source', $columns['defaults']),
    ),
    'source_uploaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Source Uploaded'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'source_uploaded' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'source_uploaded' . $suffix] : in_array('source_uploaded', $columns['defaults']),
    ),
    'translations' => array(
      '#type' => 'checkbox',
      '#title' => t('Translations'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'translations' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'translations' . $suffix] : in_array('translations', $columns['defaults']),
    ),
    'location' => array(
      '#type' => 'checkbox',
      '#title' => t('Location'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'location' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'location' . $suffix] : in_array('location', $columns['defaults']),
    ),
    'context' => array(
      '#type' => 'checkbox',
      '#title' => t('Context'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'context' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'context' . $suffix] : in_array('context', $columns['defaults']),
    ),
    'doc_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Document ID'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'doc_id' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'doc_id' . $suffix] : in_array('doc_id', $columns['defaults']),
    ),
    'textgroup' => array(
      '#type' => 'checkbox',
      '#title' => t('Textgroup'),
      '#default_value' => isset($_SESSION['config_custom'][$prefix . 'textgroup' . $suffix]) ? $_SESSION['config_custom'][$prefix . 'textgroup' . $suffix] : in_array('textgroup', $columns['defaults']),
    ),
  );
  $column_elements = array_intersect_key($column_elements, $columns['columns']); // Reduces the output columns to the defaults specified in 'lingotek_grid_define_columns'

  return lingotek_grid_process_elements($column_elements, $prefix, $suffix); // adds prefixes and suffixes to the elements
}

function lingotek_grid_update($entity_type) {
  if ($entity_type == 'config') {
    $lids = LingotekConfigSet::getLidsByStatus(LingotekSync::STATUS_PENDING);
    if (empty($lids)) {
      drupal_set_message(t('There are no in-progress translations to check.'));
      drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
    }
    lingotek_config_update_selected($lids);
    return;
  }
  $active_locales = lingotek_get_target_locales();
  $pending_nids = LingotekSync::getEntityIdsByTargetStatus($entity_type, LingotekSync::STATUS_PENDING, $active_locales);
  $disabled_nids = LingotekSync::getEntityIdsByProfileStatus($entity_type, LingotekSync::PROFILE_DISABLED);
  $nids = array_diff($pending_nids, $disabled_nids);
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
      // Skip language neutral taxonomy terms because their source is really English.
      if ($entity_type == 'taxonomy_term' && $entity->language == LANGUAGE_NONE && $locale == 'en_US') {
        continue;
      }
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
  if ($entity_type === 'config') {
    // Looks clean but actually gets set_ids, convert those to lids, then the next function converts back to set_ids.
    $lids = LingotekConfigSet::getLidsByStatus(LingotekSync::STATUS_READY);
    lingotek_config_download_selected('download_all', $lids);
    return;
  }
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
  if ($entity_type === 'config') {
    // Only get the ones that need to be uploaded.
    $lids = LingotekConfigSet::getLidsToUpdate();
    lingotek_config_upload_selected($lids);
    return;
  }
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
 * @param string $grid_name
 *    Changes whether this will be for the normal grids or for config
 *
 * @return array $columns
 *    Associative array keyed by column name with prefix and suffix removed
 *    Keys point to a bool which specifies if the column should be shown or not
 */
function lingotek_grid_get_columns($grid_name = 'grid') {
  $filters = array();

  if (!isset($_SESSION[$grid_name . '_custom'])) { // If the columns do not exist yet in the session variable we get an error, so reset them here.
    lingotek_grid_reset_columns();
  }

  foreach ($_SESSION[$grid_name . '_custom'] as $key => $value) {
    $columns[str_replace('__custom', '', $key)] = $value;
  }

  return $columns;
}

/**
 * Cleans up the grid_custom or config_custom session array, restoring the predefined defaults.
 */
function lingotek_grid_reset_columns() {
  $grid_name = $_SESSION['grid_entity_type'];
  if (isset($_SESSION[$grid_name . '_custom'])) {
    unset($_SESSION[$grid_name . '_custom']);
  }
  $source_columns = $grid_name == 'config' ? lingotek_config_define_columns() : lingotek_grid_define_columns($grid_name);
  foreach ($source_columns['columns'] as $column) {
    $_SESSION[$grid_name . '_custom'][$column . '__custom'] = in_array($column, $source_columns['defaults']);
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
function lingotek_grid_define_columns($entity_type) {
  $columns = array();
  $columns['columns'] = array(
    'nid' => 'nid',
    'content_type' => 'content_type',
    'title' => 'title',
    'language' => 'language',
    'translations' => 'translations',
    'configuration' => 'configuration',
    'document_id' => 'document_id',
    'workflow' => 'workflow',
    'changed' => 'changed',
    'last_uploaded' => 'last_uploaded',
    'actions' => 'actions',
  );
  $columns['defaults'] = array(
    'title' => 'title',
    'language' => 'language',
    'translations' => 'translations',
    'configuration' => 'configuration',
    'content_type' => 'content_type',
    'actions' => 'actions',
    'changed' => 'changed',
  );
  if ($entity_type == 'taxonomy_term') {
    $columns['columns']['description'] = 'description';
    $columns['defaults']['description'] = 'description';
  }
  return $columns;
}

function lingotek_config_define_columns() {
  $columns = array(
    'columns' => array(
      'lid' => 'lid',
      'set_name' => 'set_name',
      'source' => 'source',
      'source_uploaded' => 'source_uploaded',
      'translations' => 'translations',
      'location' => 'location',
      'context' => 'context',
      'doc_id' => 'doc_id',
      'textgroup' => 'textgroup',
    ),
    'defaults' => array(
      'lid' => 'lid',
      'set_name' => 'set_name',
      'source' => 'source',
      'source_uploaded' => 'source_uploaded',
      'translations' => 'translations',
      'textgroup' => 'textgroup',
    ),
  );
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
  $source_languages = array();
  foreach ($languages as $code => $language) {
    $source_languages[$code] = t($language->name) . ' (' . $language->lingotek_locale . ')';
  }
  asort($source_languages);

  $profiles = array();
  $profiles[LingotekSync::PROFILE_CUSTOM] = t('Custom');
  $profiles[LingotekSync::PROFILE_DISABLED] = t('Disabled');

  $profile_defaults = lingotek_get_profiles();
  foreach ($profile_defaults as $key => $p) {
    $profiles[$key] = $p['name'];
  }
  unset($profiles['CONFIG']);
  asort($profiles);

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
      '#options' => array('all' => t('All Languages')) + $source_languages,
    ),
    'profile' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['profile']) ? $_SESSION['grid_filters']['profile'] : 'all',
      '#title' => t('Translation Profile'),
      '#options' => array('all' => 'All') + $profiles,
      '#multiple' => TRUE
    ),
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
    asort($grid_bundles);
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

function lingotek_config_build_filters($form_state) {
  $languages = language_list();
  $search_languages = array('all' => t('All Languages'));
  foreach ($languages as $code => $language) {
    $search_languages[$code] = t($language->name) . ' (' . $language->lingotek_locale . ')';
  }
  $modules = array_map('ucfirst', module_list(FALSE, FALSE, TRUE));

  $filters = array(
    'lid' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['lid']) ? $_SESSION['grid_filters']['lid'] : '',
      '#title' => t('Config Entity ID is'),
      '#size' => 8,
    ),
    'document_id' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters']['document_id']) ? $_SESSION['grid_filters']['document_id'] : '',
      '#title' => t('Doc ID is'),
      '#size' => 10,
    ),
    'location' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['location']) ? $_SESSION['grid_filters']['location'] : '',
      '#title' => t('Module'),
      '#options' => array('' => '') + $modules,
      ),
    'upload_status' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters']['upload_status']) ? $_SESSION['grid_filters']['upload_status'] : 'all',
      '#title' => t('Upload Status'),
      '#options' => array(
        'all' => t('All'),
        LingotekSync::STATUS_EDITED => t('Edited (needs reupload)'),
        LingotekSync::STATUS_CURRENT => t('Current (uploaded)'),
        'never' => t('Never uploaded'),
      ),
      '#multiple' => FALSE,
    ),
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

  $entity_properties = array_flip($info['schema_fields_sql']['base table']);

  $label_col  = isset($info['entity keys']['label']) && $info['entity keys']['label'] ? $info['entity keys']['label'] : NULL;
  $bundle_col = isset($info['entity keys']['bundle']) && $info['entity keys']['bundle'] && isset($entity_properties[$info['entity keys']['bundle']]) ? $info['entity keys']['bundle'] : NULL;
  // All managed entity types should have a language column.
  // Field collection entities have one, but it is called 'langcode'.
  // Message type entities have one, and it is called 'language'; but currently it does not appear in their entity keys.
  // So, the $language_col looks for the correct language field, and then guesses it is 'language' if it doesn't find one.
  $language_col = !empty($info['entity keys']['language']) ? $info['entity keys']['language'] : 'language';
  $entity_properties['type'] = $entity_type;
  $entity_properties['label_col'] = $label_col;
  $entity_properties['info'] = $info;
  $entity_properties['language_col'] = $language_col;

  $limit = isset($_SESSION['limit_select']) ? $_SESSION['limit_select'] : 10;
  $columns = isset($form_state['values']['columns']) ? $form_state['values']['columns'] : array();

  $header = array(// Define the tentative source header
    'nid' => array('data' => t('ID'), 'field' => 'n.' . $entity_id_key),
    'content_type' => array('data' => t('Content Type'), 'field' => 'type'),
    'title' => array('data' => t('Title'), 'field' => 'n.' . $label_col),
    'description' => array('data' => t('Description')),
    'language' => array('data' => t('Source Uploaded')), //, 'field' => 'upload_status'),
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
  $form_state['values']['grid_header'] = lingotek_bulk_grid_refine_source_header($header, $columns);
  $query = lingotek_bulk_grid_query($form_state, $count_only, $entity_id_key, $label_col, $entity_type, $limit, $bundle_col, $info, $entity_properties, $eid);
  // Initialize Query and extend paginator and tablesort if necessary
  lingotek_bulk_grid_filter_query($query, $entity_type, $eid, $label_col, $info);

  // If count only, return the count
  if ($count_only) {
    $result = $query->execute();
    return $result->rowCount();
  }

  // Execute the query
  $table_data_raw = $query->distinct()->execute()->fetchAllAssoc($entity_id_key);
  lingotek_entity_load($table_data_raw, $entity_type);

  // Parse returned objects and make them arrays keyed by the Node ID for clean use in The Grid.
  $table_data = lingotek_bulk_grid_parse_table_data($table_data_raw, $entity_properties, $entity_id_key);

  return $table_data;
}

function lingotek_bulk_grid_refine_source_header($header, $columns) {
  $grid_header = array();
  foreach ($header as $title => $data) { // Refine the source header using the selected columns
    if (array_key_exists($title, $columns) && $columns[$title]) {
      $grid_header[$title] = $data;
    }
  }
  return $grid_header;
}

function lingotek_bulk_grid_query($form_state, $count_only, $entity_id_key, $label_col, $entity_type, $limit, $bundle_col, $info, $entity_properties, $eid) {
  $base_table = $info['base table'];

  if ($count_only) {
    $query = db_select('' . $base_table . '', 'n');
  }
  else {
    $query = db_select('' . $base_table . '', 'n')
        ->extend('PagerDefault')
        ->extend('TableSort');
    $query->limit($limit)
        ->orderByHeader($form_state['values']['grid_header']);
  }
  if ($base_table == 'node') {
    $query->innerJoin('node', 'node2', '(n.nid = node2.nid) AND (node2.tnid = 0 OR node2.tnid = node2.nid)');
  }

  // Entity Title and Name of Content Type (type)
  $query->fields('n', array($entity_id_key));

  if ($label_col) {
    $query->addField('n', $label_col, 'title');
  }

  $query->addField('n', $entity_properties['language_col'], 'language');

  if (isset($entity_properties['changed'])) {
    $query->addField('n', 'changed');
  }

  lingotek_bulk_grid_query_add_entity_specifics($query, $entity_type, $bundle_col, $info);
  lingotek_bulk_grid_query_add_statuses($query, $entity_type, $eid);
  lingotek_bulk_grid_query_add_localized_title($query, $entity_type, $eid, $label_col);
  lingotek_bulk_grid_query_add_keys($query, $entity_type, $eid);

  return $query;
}

function lingotek_bulk_grid_query_add_entity_specifics($query, $entity_type, $bundle_col, $info) {
  if ($entity_type == 'comment') {
    $query->join('node', 'nn', 'nn.nid = n.nid');
    $query->addExpression("CONCAT('comment_node_',nn.type)", 'type');
    $query->addExpression("CONCAT('comment_node_',nn.type)", 'node_type');
  }
  elseif ($entity_type == 'taxonomy_term') {
    $query->addField('n', 'description');

    $query->innerJoin('taxonomy_vocabulary', 'tv', 'n.vid = tv.vid');
    $query->addField('tv', 'name', 'tv_name');
    $query->addField('tv', 'machine_name', 'type');
    $query->addField('tv', 'i18n_mode', 'translation_mode');

    // Remove taxonomy terms handled by config manage page.
    $query->leftJoin('field_config_instance', 'fields', 'fields.bundle = tv.machine_name');
    $or = db_or();
    // Has no custom fields
    $or->isNotNull('fields.bundle');
    $or->condition('tv.i18n_mode', LINGOTEK_TAXONOMY_LOCALIZE_VALUE, '<>');
    $query->condition($or);
  }
  elseif ($bundle_col) {
    if ($info['entity keys']['bundle'] != 'type') {
      $query->addField('n', $info['entity keys']['bundle']);
    }
    $query->addField('n', $info['entity keys']['bundle'], 'type');
  }
}

function lingotek_bulk_grid_query_add_statuses($query, $entity_type, $eid) {
  $query->addExpression("(SELECT COUNT(entity_id) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='CURRENT')", 't_current_c');

  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_PENDING . "')", 't_pending');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_READY . "')", 't_ready');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_CURRENT . "')", 't_current');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_EDITED . "')", 't_edited');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_UNTRACKED . "')", 't_untracked');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_TARGET_LOCALIZE . "')", 't_target_localize');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_TARGET_EDITED . "')", 't_target_edited');
  $query->addExpression("(SELECT GROUP_CONCAT(CONCAT(SUBSTRING(entity_key, 17, 10), ':' , value)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'source_language_%')", 'lang_override');
}

function lingotek_bulk_grid_query_add_localized_title($query, $entity_type, $eid, $label_col) {
  $localized_label_table = 'field_data_' . $label_col . '_field';

  if ($label_col && db_table_exists($localized_label_table)) {
    $query->leftJoin('' . $localized_label_table . '', 't_title', 't_title.entity_id = ' . $eid . ' and t_title.entity_type = \'' . $entity_type . '\' and t_title.language=\'' . $GLOBALS['language']->language . '\'');
    $query->addField('t_title', $label_col . '_field_value', 'localized_title');
  }
}

function lingotek_bulk_grid_query_add_keys($query, $entity_type, $eid) {
  // left joins are necessary here because some lingotek table keys might not exist
  // Lingotek Document ID
  $query->leftJoin('lingotek_entity_metadata', 'lingo_document_id', 'lingo_document_id.entity_type =\'' . $entity_type . '\' AND lingo_document_id.entity_id = ' . $eid . ' and lingo_document_id.entity_key = \'document_id\'');
  $query->addField('lingo_document_id', 'value', 'document_id');

  // Entity Upload Status
  $query->leftJoin('lingotek_entity_metadata', 'lingo_upload_status', 'lingo_upload_status.entity_type =\'' . $entity_type . '\' AND lingo_upload_status.entity_id = ' . $eid . ' and lingo_upload_status.entity_key = \'upload_status\' and lingo_upload_status.value <> \'' . LingotekSync::STATUS_TARGET . '\'');
  $query->addField('lingo_upload_status', 'value', 'upload_status');

  // Profile Settings
  $query->leftJoin('lingotek_entity_metadata', 'lingo_profile', 'lingo_profile.entity_type =\'' . $entity_type . '\' AND lingo_profile.entity_id = ' . $eid . ' and lingo_profile.entity_key = \'profile\'');
  $query->addField('lingo_profile', 'value', 'profile');

  // Last Uploaded Timestamp
  $query->leftJoin('lingotek_entity_metadata', 'lingo_last_uploaded', 'lingo_last_uploaded.entity_type =\'' . $entity_type . '\' AND lingo_last_uploaded.entity_id = ' . $eid . ' and lingo_last_uploaded.entity_key = \'last_uploaded\'');
  $query->addField('lingo_last_uploaded', 'value', 'last_uploaded');

  // Any Upload Errors
  $query->leftJoin('lingotek_entity_metadata', 'lingo_last_sync_error', 'lingo_last_sync_error.entity_type =\'' . $entity_type . '\' AND lingo_last_sync_error.entity_id = ' . $eid . ' and lingo_last_sync_error.entity_key = \'last_sync_error\'');
  $query->addField('lingo_last_sync_error', 'value', 'last_sync_error');

  // Any specifically defined workflow
  $query->leftJoin('lingotek_entity_metadata', 'lingo_workflow', '' . $eid . ' = lingo_workflow.entity_id and lingo_workflow.entity_type =\'' . $entity_type . '\' and lingo_workflow.entity_key = \'workflow_id\'');
  $query->addField('lingo_workflow', 'value', 'workflow');

  // Original source language of the entity, in case of source overwriting option
  if ($entity_type == 'node') {
    $query->leftJoin('lingotek_entity_metadata', 'lingo_orig_lang', $eid . ' = lingo_orig_lang.entity_id and lingo_orig_lang.entity_type =\'' . $entity_type . '\' and lingo_orig_lang.entity_key = \'original_language\'');
    $query->addField('lingo_orig_lang', 'value', 'original_lang');
  }
}

function lingotek_bulk_grid_filter_query($query, $entity_type, $eid, $label_col, $info) {
  if (!isset($_SESSION['grid_filters'])) {
    return;
  }
  $filters = $_SESSION['grid_filters'];

  lingotek_bulk_grid_filter_search_box($query, $filters, $eid, $label_col, $entity_type);
  lingotek_bulk_grid_filter_popup_options($query, $filters, $eid, $info, $entity_type);
  lingotek_bulk_grid_filter_last_uploaded($query, $filters);
  lingotek_bulk_grid_filter_last_downloaded($query, $filters);
}

function lingotek_bulk_grid_filter_search_box($query, $filters, $eid, $label_col, $entity_type) {
  if (isset($filters['search_type']) && $filters['search_type'] == 'all') {
    $filters['title'] = $filters['body'] = $filters['search'];
  }

  $title_query = $body_query = array(-1);

  if ($entity_type == 'comment') {
    $title_field_table = 'field_data_subject_field';
    $title_field_col = 'subject_field_value';
    $body_field_table = 'field_data_comment_body';
    $body_field_col = 'comment_body_value';
  }
  else {
    $title_field_table = 'field_data_title_field';
    $title_field_col = 'title_field_value';
    $body_field_table = 'field_data_body';
    $body_field_col = 'body_value';
  }

  if (isset($filters['title']) && db_table_exists($title_field_table)) {
    $title_query = db_select($title_field_table, 'tf')
        ->distinct()
        ->fields('tf', array('entity_id'))
        ->condition('tf.' . $title_field_col, '%' . $filters['title'] . '%', 'LIKE');
  }

  if (isset($filters['body']) && db_table_exists($body_field_table)) {
    $body_query = db_select($body_field_table, 'tb')
        ->distinct()
        ->fields('tb', array('entity_id'))
        ->condition('tb.' . $body_field_col, '%' . $filters['body'] . '%', 'LIKE');
  }

  //  Search
  if (isset($filters['search_type']) && $filters['search_type'] == 'all' && isset($filters['search']) && strlen($filters['search'])) {
    $or = db_or();

    if (!empty($label_col)) {
      $or->condition('n.' . $label_col, '%' . $filters['search'] . '%', 'LIKE');
    }
    $or->condition('' . $eid . '', $title_query, 'IN');
    $or->condition('' . $eid . '', $body_query, 'IN');

    $query->condition($or);
  }
  else {
    //  Title Field
    if (isset($filters['title']) && $filters['title'] != '') {
      $or = db_or();
      $or->condition('' . $eid . '', $title_query, 'IN');
      if (!empty($label_col)) {
        $or->condition('n.' . $label_col, '%' . $filters['search'] . '%', 'LIKE');
      }
      $query->condition($or);
    }

    // Body Field
    if (isset($filters['body']) && $filters['body'] != '') {
      $query->condition('' . $eid . '', $body_query, 'IN');
    }
  }
}

function lingotek_bulk_grid_filter_popup_options($query, $filters, $eid, $info, $entity_type) {
  //  Entity ID
  if (isset($filters['nid']) && $filters['nid'] != '') {
    $query->condition('' . $eid . '', $filters['nid']);
  }

  lingotek_filter_by_document_id($query, $filters);

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
    elseif ($entity_type == 'comment') {
      $or = db_or();
      foreach ($filters['content_type'] as $type_alias) {
        $content_type = substr($type_alias, strlen('comment_node_'));
        $or->condition('nn.type', $content_type);
      }
      $query->condition($or);
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
}

function lingotek_filter_by_document_id($query, $filters) {
  if (isset($filters['document_id']) && $filters['document_id'] != '') {
    if ($filters['document_id'] == 'None') {
      $query->condition('lingo_document_id.value', NULL);
    }
    else {
      $query->condition('lingo_document_id.value', $filters['document_id']);
    }
  }
}

function lingotek_bulk_grid_filter_last_uploaded($query, $filters) {
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
}

function lingotek_bulk_grid_filter_last_downloaded($query, $filters) {
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
}

function lingotek_bulk_grid_parse_table_data($table_data_raw, $entity_properties, $entity_id_key) {
  $languages = language_list();
  $profiles = lingotek_get_profiles();

  $api = LingotekApi::instance();
  $workflows = $api->listWorkflows();

  $table_data = array();

  foreach ($table_data_raw as $row) {
    $entity_id = $row->{$entity_id_key};

    $row->title = lingotek_truncate_grid_text($row->title, 55);
    $icon = lingotek_source_uploaded_icon($row);
    $locales_statuses = lingotek_build_locales_statuses($row);

    ksort($locales_statuses);

    $disabled = $row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED;
    $disabled_class = $disabled ? ' ltk-disabled-icon' : '';
    $allow_source_overwriting = !empty($row->lingotek['allow_source_overwriting']);
    $allow_target_localization = !empty($profiles[$row->lingotek['profile']]['allow_target_localization']);

    $translation_icons = lingotek_lang_icons($entity_properties['type'], $languages, $entity_id, $locales_statuses, $disabled, $row->language, $allow_source_overwriting, $allow_target_localization); // show translation statuses
    if (!empty($translation_icons['source'])) {
      $row->overridden_source_target_icon = $translation_icons['source'];
      unset($translation_icons['source']);
    }
    $target_icons_str = implode('', array_values($translation_icons));
    $configuration = lingotek_render_configuration($row, $profiles, $disabled_class);
    $source = lingotek_render_source($entity_properties['type'], $row, $icon, $languages, $entity_properties['language_col']);
    $actions = lingotek_render_actions($entity_properties['type'], $entity_id, $disabled_class);
    $title = $entity_properties['label_col'] ? lingotek_render_title($row, $entity_properties, $GLOBALS['language']) : '';

    // Build the data to be output for this row
    $data = array(
      'nid' => $row->{$entity_id_key} ? : t('??'),
      'title' => $title,
      'language' => $source,
      'translations' => $target_icons_str,
      'configuration' => $configuration,
      'document_id' => $row->document_id ? : t('N/A'),
      'content_type' => isset($entity_properties['info']['bundles'][$row->type]['label']) ? $entity_properties['info']['bundles'][$row->type]['label'] : $row->type,
      'last_uploaded' => $row->last_uploaded ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->last_uploaded))) : t('Never'),
      'workflow' => ($row->lingotek['workflow_id'] ? (isset($workflows[$row->lingotek['workflow_id']]) ? check_plain($workflows[$row->lingotek['workflow_id']]) : $row->lingotek['workflow_id']) : ''),
      'actions' => '<span class="lingotek-node-actions">' . $actions . '</span>',
      'description' => isset($row->description) ? $row->description : '',
    );

    if (isset($entity_properties['changed'])) {
      $data['changed'] = $row->changed ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->changed))) : t('Never');
    }

    $table_data[$entity_id] = $data;
  }
  return $table_data;
}

function lingotek_truncate_grid_text($string, $truncate_length) {
  if (strlen($string) > $truncate_length) {
    $dots = '...';
    $dots_length = strlen($dots);
    $string = substr($string, 0, $truncate_length - $dots_length) . $dots;
  }
  return $string;
}

function lingotek_source_uploaded_icon($row) {
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
  return $icon;
}

function lingotek_build_locales_statuses($row, $t_prefix = TRUE) {
  $list_statuses = array('pending', 'ready', 'current', 'edited', 'untracked');
  $locales_statuses = array();
  foreach ($list_statuses as $status) {
    $key = $t_prefix ? 't_' . $status : $status;
    foreach (explode(',', $row->$key) as $locale) {
      if (!empty($locale)) {
        $locales_statuses[$locale] = $status;
      }
    }
  }

  return $locales_statuses;
}

function lingotek_render_title($row, $entity_properties, $language) {
  $info = $entity_properties['info'];
  $entity_type = $entity_properties['type'];
  $no_localized_title = (language_default()->language != $language->language) && (!isset($row->localized_title) or $row->localized_title == '');

  // special handling of taxonomy terms for URI generation
  if ($entity_type == 'taxonomy_term') {
    $term_obj = lingotek_entity_load_single($entity_type, $row->tid);
    $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $term_obj) : '';
  }
  else {
    $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $row) : '';
  }

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

  return $title;
}

function lingotek_render_source($entity_type, $row, $icon, $languages, $language_col) {
  if ($entity_type == 'node') {
    $id = $row->nid;
  }
  elseif ($entity_type == 'comment') {
    $id = $row->cid;
  }
  elseif ($entity_type == 'config') {
    $id = $row->lid;
  }
  elseif ($entity_type == 'taxonomy_term') {
    $id = $row->tid;
    $row->language = isset($row->translation_mode) && $row->translation_mode == '1' ? 'en' : $row->language;
  }
  elseif (!empty($row->id)) {
    $id = $row->id;
  }
  $source = '<span class="lingotek-language-source" style="white-space: nowrap;">' . $icon . ' ';
  $source .= ($language_col ? lingotek_get_upload_string($row, $languages) : '');
  $source .= '</span>';
  $linked_statuses = array(LingotekSync::STATUS_EDITED, LingotekSync::STATUS_FAILED);
  $needs_upload = (in_array($row->upload_status, $linked_statuses) || empty($row->upload_status)) && $row->lingotek['profile'] != LingotekSync::PROFILE_DISABLED;
  if ($needs_upload) { // add link for upload
    $source = '<span title="Upload Now" class="ltk-upload-button lingotek-language-source" onclick="lingotek_perform_action(' . $id . ',\'upload\')">' . $source . '</span>';
  }
  return $source;
}

function lingotek_render_configuration($row, $profiles, $disabled_class) {
  $configuration = '';

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

  return $configuration;
}

function lingotek_render_actions($entity_type, $entity_id, $disabled_class) {
  $pm_icon = ($entity_type == 'node') ? ' <i class="fa fa-tasks' . $disabled_class . '"></i>' : '';
  $pm_link = ' ' . l($pm_icon, 'node/' . $entity_id . '/lingotek_pm', array('html' => TRUE, 'attributes' => array('title' => t('View Translations'), 'target' => '_blank')));

  $actions = '';
  $actions .= lingotek_get_entity_edit_link($entity_type, $entity_id);
  $actions .= empty($disabled_class) ? $pm_link : $pm_icon;
  $actions .= ' ' . l('<i title="' . t('Set Lingotek Settings') . '" class="fa fa-gear"></i>', '', array('html' => TRUE, 'attributes' => array('onclick' => 'lingotek_perform_action(' . $entity_id . ',"edit"); return false;')));

  return $actions;
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

function lingotek_lang_icons($entity_type, $languages, $entity_id, $locale_statuses = array(), $disabled = FALSE, $source_language = NULL, $allow_source_overwriting = FALSE, $allow_target_localization = FALSE, $non_lingotek_config_translations = array()) {
  $icons = array();

  $legend = array(
    LingotekSync::STATUS_PENDING => t('In progress'),
    LingotekSync::STATUS_READY => t('Ready to download'),
    LingotekSync::STATUS_CURRENT => t('Current'),
    LingotekSync::STATUS_EDITED => t('Not current'),
    LingotekSync::STATUS_UNTRACKED => t('Untracked'),
    LingotekSync::STATUS_TARGET_LOCALIZE => t('Pending localization'),
    LingotekSync::STATUS_TARGET_EDITED => t('Needs to be uploaded'),
    LingotekSync::STATUS_NON_LINGOTEK => t('Non-Lingotek Translation'),
    ''
  );
  $default_link = 'lingotek/workbench/' . $entity_type . '/' . $entity_id . '/';
  $manual_link = $entity_type . '/' . $entity_id . '/lingotek_pm/';
  $untracked_link = 'lingotek/view/' . $entity_type . '/' . $entity_id . '/';
  $untracked_config_link = 'admin/' . $entity_type . '/regional/translate/edit/' . $entity_id . '/';

  $link_map = array(
    LingotekSync::STATUS_PENDING => $default_link,
    LingotekSync::STATUS_READY => $default_link,
    LingotekSync::STATUS_CURRENT => $default_link,
    LingotekSync::STATUS_EDITED => $default_link,
    LingotekSync::STATUS_UNTRACKED => $untracked_link,
    LingotekSync::STATUS_TARGET_LOCALIZE => $untracked_link,
    LingotekSync::STATUS_TARGET_EDITED => $untracked_link,
    LingotekSync::STATUS_NON_LINGOTEK => $untracked_config_link,
  );

  $lingotek_languages = Lingotek::getLanguages();

  foreach ($locale_statuses as $locale => $locale_status) {
    // if it's lingotek enabled
    if (array_key_exists($locale, $lingotek_languages)) {
      // could this ever be false? I thought all $lingotek_languages would be enabled
      $locale_enabled = $lingotek_languages[$locale]->lingotek_enabled; //array_key_exists($locale, $lingotek_languages) ? $lingotek_languages[$locale]->lingotek_enabled : FALSE;
      $lang_code = $lingotek_languages[$locale]->language;
      $lingotek_locale = $lingotek_languages[$locale]->lingotek_locale;
      if (!$allow_source_overwriting && !is_null($source_language) && $lang_code === $source_language) {
        continue; // hide source language targets (shouldn't exist anyways)
      }

      if ($locale_enabled) { // exclude language translations that are not lingotek_enabled (we may consider showing everything later)
        $status = strtoupper($locale_status);
        $status = $disabled && strtoupper($status) != LingotekSync::STATUS_CURRENT ? LingotekSync::STATUS_UNTRACKED : $status;

        // styling for the icons based on status and whether the translation is owned by Lingotek or not
        if ($entity_type == 'config' && (array_key_exists($entity_id, $non_lingotek_config_translations) && in_array($lang_code, explode(',', $non_lingotek_config_translations[$entity_id]->language_codes)))) {
          $css_classes = 'target-non-lingotek';
          $status = 'NON_LINGOTEK';
          $tip = $languages[$lang_code]->name . ' - ' . $legend[$status];
        }
        else { //styling for all icons except Config/NON_LINGOTEK items
          $css_classes = 'target-' . strtolower($status);
          $css_classes .= $disabled ? ' target-disabled' : '';
          $tip = $languages[$lang_code]->name . ' - ' . $legend[$status];
          $status = $disabled ? LingotekSync::STATUS_UNTRACKED : $status; // all disabled entities should link to view page (not workbench)
        }

        // link redirects to the translate interface for the config string rather than the Lingotek workbench if is an untracked config item
        if ($entity_type == 'config' && $status == LingotekSync::STATUS_UNTRACKED) {
          $current_icon = l($lang_code, $untracked_config_link . $lingotek_locale, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => array('language-icon', $css_classes))));
        }
        else {
          $current_icon = l($lang_code, $link_map[$status] . $lingotek_locale, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => array('language-icon', $css_classes))));
        }

        if (!is_null($source_language) && $lang_code === $source_language) {
          $icons['source'] = $current_icon;
        }
        else {
          $icons[$lang_code] = $current_icon;
        }
      }
    }
  }

  return $icons;
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
    '#description' => filter_xss($content_details_string),
    '#suffix' => '<i>' . t("Note: this action cannot be undone.") . '</i>',
    '#collapsible' => $collapse_fieldset,
    '#collapsed' => $collapse_fieldset,
  );

  $submit_function = ($entity_type === 'config') ? 'lingotek_config_delete_form_submit' : 'lingotek_entity_delete_form_submit';

  $form['confirm']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array($submit_function),
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

function lingotek_config_delete_form_submit($form, &$form_state) {
  if (isset($form_state['values']['entity_ids'])) {
    $lids = json_decode($form_state['values']['entity_ids']);
  }
  elseif (isset($form_state['entity_ids'])) {
    $lids = json_decode($form_state['entity_ids']);
  }

  if (!empty($lids)) {
    LingotekConfigSet::deleteSegmentTranslations($lids);
    $count = count($lids);
    watchdog('content', format_plural($count, 'Deleted translations for 1 entity.', 'Deleted translations for @count entities.'));
    drupal_set_message(format_plural($count, 'Deleted translations for 1 entity.', 'Deleted translations for @count entities.'));
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

  $entity_type = $form_state['entity_type'];

  $form['disassociate_translations'] = array(
    '#type' => 'fieldset',
    '#prefix' => format_plural(count($entity_ids), '', '<div class="messages warning">' . t('This action will apply to @count entities.') . '</div>'),
    '#title' => t('Disassociate Translations'),
    '#description' => t("Disassociates the entity translations on Lingotek's servers from the copies downloaded to Drupal. Additional translation using Lingotek will require re-uploading the entity to restart the translation process. <br/><br/>"),
    '#suffix' => $note = ($entity_type === 'config') ? '<span style="color:red"><b><i>' . t("Note: Deleting documents on the TMS will affect all strings in the associated config sets.") . '<b><i></span>' : '<i>' . t("Note: This action should only be used to change the Lingotek project or TM vault associated with the entities.") . '<i>',
    '#collapsible' => $collapse_fieldset,
    '#collapsed' => $collapse_fieldset,
  );

  $form['disassociate_translations']['confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('Also remove all document(s) from Lingotek TMS'),
    '#default_value' => FALSE,
  );

  $submit_function = ($entity_type === 'config') ? 'lingotek_config_disassociate_form_submit' : 'lingotek_entity_disassociate_form_submit';

  $form['disassociate_translations']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Disassociate Translations'),
    '#submit' => array($submit_function),
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
      drupal_set_message(t('Failed to remove documents from Lingotek TMS.'), 'warning', FALSE);
      return;
    }
  }
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'document_id');
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'upload_status');
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'original_language');
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'source_language_%', 'LIKE');
  foreach ($entity_ids as $entity_id) {
    LingotekSync::setAllTargetStatus($entity_type, $entity_id, LingotekSync::STATUS_UNTRACKED);
  }

  drupal_set_message(format_plural(count($entity_ids), 'Translations disassociated for one entity.', 'Translations disassociated for @count entities.'));
}


/**
 * Display the language string (including overridden language, if any)
 */
function lingotek_get_upload_string($row, $languages) {
  $actual_source_lang = lingotek_row_source_language($row);
  $lang_overridden = !empty($row->lang_override) ? TRUE : FALSE;
  $marked_language = !empty($languages[$row->language]->name) ? t($languages[$row->language]->name) : t('Unnamed (@language_code)', array('@language_code' => $row->language));
  $actual_language = !empty($languages[$actual_source_lang]->name) ? t($languages[$actual_source_lang]->name) : t('Unnamed (@language_code)', array('@language_code' => $actual_source_lang));
  $original_language = !empty($row->original_lang) && !empty($languages[$row->original_lang]->name) ? t($languages[$row->original_lang]->name) : $actual_language;
  if ($lang_overridden) {
    if ($actual_source_lang != $row->language) {
      $span_title = t('Language Override: This source content is marked as @marked_language in Drupal but is written in @actual_language.', array('@marked_language' => $marked_language, '@actual_language' => $actual_language));
    }
    else {
      $span_title = t('Language Override: This source content was uploaded as @original_language to Lingotek but is now @marked_language. Re-uploading this in its current state may corrupt the translation for this document.', array('@marked_language' => $marked_language, '@original_language' => $original_language));
    }
    $response = '<span title="' . $span_title . '">' . t($original_language) . (!empty($row->overridden_source_target_icon) ? '&nbsp;&nbsp;&nbsp;' . $row->overridden_source_target_icon : '') . '</span>';
  }
  else {
    $response = (empty($row->language) || $row->language == LANGUAGE_NONE) ? t('Language Neutral') : t($languages[$row->language]->name);
  }
  return $response;
}

/**
 * Return source language code to be used by Lingotek
 */
function lingotek_row_source_language($row) {
  if (!empty($row->lang_override)) {
    $overrides = explode(',', $row->lang_override);
    $override_map = array();
    foreach ($overrides as $o) {
      list($k, $v) = explode(':', $o);
      $override_map[$k] = $v;
    }
    $drupal_source_locale = Lingotek::convertDrupal2Lingotek($row->language);
    $overridden_source_lang = !empty($override_map[$drupal_source_locale]) ? $override_map[$drupal_source_locale] : NULL;
    if (!empty($overridden_source_lang)) {
      return $overridden_source_lang;
    }
  }
  return $row->language;
}

function lingotek_config_disassociate_form_submit($form, $form_state) {
  if (isset($form_state['values']['entity_ids'])) {
    $lids = json_decode($form_state['values']['entity_ids']);
  }
  elseif (isset($form_state['lids'])) {
    $lids = json_decode($form_state['entity_ids']);
  }

  $api = LingotekApi::instance();
  $remove_from_tms = $form_state['values']['confirm'];
  $set_ids = LingotekSync::getSetIdsFromLids($lids);
  $doc_ids = LingotekSync::getConfigDocIdsFromSetIds($set_ids);

  if ($remove_from_tms) { //disassociate on TMS
    $result = lingotek_batch_disassociate_content_worker($api, $doc_ids);
    //Deletes document data from lingotek_config_metadata table
    $query1 = db_delete('lingotek_config_metadata')
      ->condition('id', $set_ids)
      ->execute();
    $query2 = db_delete('lingotek_config_map')
      ->condition('set_id', $set_ids)
      ->execute();

    if (!$result) {
      drupal_set_message(t('Failed to remove documents from Lingotek TMS.'), 'warning', FALSE);
      return;
    }
  }

  if (!empty($lids)) {
    LingotekConfigSet::disassociateSegments($lids);
  }
}
