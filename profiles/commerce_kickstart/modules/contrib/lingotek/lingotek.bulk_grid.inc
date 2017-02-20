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
  if(empty($entity_type)){
    $entity_type = 'node';
  }

  global $language;

  if (!isset($_SESSION['grid_entity_type'])) {
    $_SESSION['grid_entity_type'] = $entity_type;
  }
  elseif ($_SESSION['grid_entity_type'] != $entity_type) {
    $_SESSION['grid_entity_type'] = $entity_type;
  }

  /*
   * Here we store or retrieve the GET parameters so that the state of the table is maintained when leaving and coming back
   * Also makes it so the state is not lost when performing bulk actions
   */
  if (count($_GET) == 1 && isset($_SESSION['grid_custom_parameters'][$entity_type]) && !empty($_SESSION['grid_custom_parameters'][$entity_type])) {
    $_SESSION['grid_custom_parameters'][$entity_type]['preventloop'] = TRUE;
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type, array('query' => $_SESSION['grid_custom_parameters'][$entity_type]));
  }
  else {
    if (!isset($_SESSION['grid_custom_parameters'])) {
      $_SESSION['grid_custom_parameters'] = array();
    }
    $temp = array();
    foreach ($_GET as $key => $value){
        $temp[$key] = filter_xss($value);
    }
    $_SESSION['grid_custom_parameters'][$entity_type] = $temp;
    if ($_SESSION['grid_custom_parameters'][$entity_type]['q']) {
      unset($_SESSION['grid_custom_parameters'][$entity_type]['q']);
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
    '#attributes' => array('id' => 'entity-type')
  );
  $form['async_update'] = array(
    '#markup' => l('', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/async-update/' . $entity_type,
        array('html' => TRUE, 'attributes' => array('id' => 'async-update', 'type' => 'hidden')))
  );
  $form['auto_download'] = array(
    '#markup' => l('', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/auto-download/' . $entity_type,
        array('html' => TRUE, 'attributes' => array('id' => 'auto-download', 'type' => 'hidden')))
  );

  $page = pager_find_page() + 1; // Get current page from url
  $limit_select = (isset($_SESSION['limit_select']) ? (int)$_SESSION['limit_select'] : 0);

  // Run query to get table rows
  $table_data = $entity_type == 'config' ? lingotek_config_get_rows($entity_type, $form, $form_state) : lingotek_grid_get_rows($entity_type, $form, $form_state);

  $total_entity_rows = count($table_data);
  if ((int)($limit_select) < $total_entity_rows || $total_entity_rows == 0) {
    // reset the page to be the last set of results
    $page = 1;
    $_SESSION['grid_custom_parameters'][$entity_type]['page'] = 0;
    if (isset($_GET['page'])) {
      $_GET['page'] = 0; // used by PagerDefault class to get page number
    }

    /**
     *Run the query again with the new pages. If not then this code is not worth
     *having.
     *@author Unknown
     *
     */
    $table_data = $entity_type == 'config' ? lingotek_config_get_rows($entity_type, $form, $form_state) : lingotek_grid_get_rows($entity_type, $form, $form_state);
  }


  $filter_set = lingotek_filter_set_check($entity_type);




  $results_first = (($page - 1) * $limit_select) + 1;
  $results_last = $results_first + count($table_data) - 1;
  if($entity_type === 'config') {
    $form['lingotek_options'] = array(
      '#markup' => '<i title="More options" class="fa fa-chevron-left fa-2x lingotek-action ltk-options" id="more-options"></i>'
    );
    $form['lingotek_force_down_all'] = array(
    '#markup' => l('<i class="fa fa-sort-amount-asc fa-2x"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/force-down-all/true', array('html' => TRUE, 'attributes' => array('title' => t('Download all regardless of status'), 'class' => array('lingotek-action ltk-force-down'), 'id' => array('force-down')))),
  );
  }
  $form['lingotek_legend'] = array(
    '#markup' => l('<i class="fa fa-question-circle fa-2x ltk-legend"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/legend', array('html' => TRUE, 'attributes' => array('title' => t('Icon descriptions'), 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-large','lingotek-action'), 'id' => array('legend')))),
  );
  $form['customize'] = array(
    '#markup' => l('<i class="fa fa-list-alt fa-2x" ></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/customize/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Customize Table'), 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-large', 'lingotek-action')))),
  );
  $last_updated = variable_get('lingotek_pending_last_updated', NULL);
  $message = t('Check status of In-Progress translations (Last checked @time)', array('@time' => $last_updated ? lingotek_human_readable_timestamp($last_updated) . ' ' . t('ago') : t('Never')));

  $form['lingotek_update'] = array(
    '#markup' => l('<i class="fa fa-filter fa-1x notify-filtered-action"></i>'
        . '<i class="fa fa-check-square-o fa-1x notify-checked-action"></i>'
        . '<i class="fa fa-cloud-download fa-2x ltk-download"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/download-ready/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Download Ready translations'), 'class' => array('lingotek-action'), 'id' => array('download-ready')))),
  );

  $form['refresh'] = array(
    '#markup' => l('<i class="fa fa-refresh fa-2x ltk-refresh"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/update/' . $entity_type, array('html' => TRUE, 'attributes' => array('class' => 'lingotek-action', 'title' => $message, 'id' => array('refresh')))),
  );

  $form['lingotek_upload'] = array(
    '#markup' => l('<i class="fa fa-filter fa-1x notify-filtered-action"></i>'
        . '<i class="fa fa-check-square-o fa-1x notify-checked-action"></i>'
        . '<i class="fa fa-cloud-upload fa-2x ltk-upload"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/upload-edited/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Re-upload all edited source content'), 'class' => array('lingotek-action'), 'id' => array('upload-edited')))),
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
  $form['delete_translations'] = array(
    '#markup' => l(t('Delete local translations'), LINGOTEK_MENU_MAIN_BASE_URL . '/manage/delete-translations/' . $entity_type, array('attributes' => array('id' => 'delete-translations-link', 'class' => array('ctools-use-modal', 'ctools-modal-lingotek-small', 'ltk-hidden-modal-trigger')))),
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
      '#markup' => '<div style="">'
      . l('<i class="fa fa-plus" style="padding-right: 4px;"></i>'
          . t('Add content')
          . '<div style="margin: 15px 0;"></div>', 'node/add',
            array('html' => TRUE, 'attributes' => array('title' => t('Add content'))))
      . '</div>'
    );
  }

  $modal_size = $entity_type == 'config' ? 'ctools-modal-lingotek-small' : 'ctools-modal-lingotek-large';
  $modal_classes = array('ctools-use-modal', $modal_size, 'ltk-action');
  $search_title = l('<i class="fa fa-search"></i>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => $modal_classes)));

  $form['search'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['search']) ? $_SESSION['grid_filters'][$entity_type]['search'] : '',
    '#title' => filter_xss($search_title),
    '#size' => 30,
    '#attributes' => array('placeholder' => t('Search'))
  );
  $search_options = $entity_type == 'config' ? lingotek_config_search_options() : lingotek_grid_search_options($entity_type);

  $grid_term = isset($_SESSION['grid_filters'][$entity_type]['search_type']) ? $_SESSION['grid_filters'][$entity_type]['search_type'] : 'all';
  $config_term = isset($_SESSION['grid_filters'][$entity_type]['textgroup']) ? $_SESSION['grid_filters'][$entity_type]['textgroup'] : 'all';
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
    '#value' => decode_entities('&#xf002;'),
    '#attributes' => array('title' => 'Search'),
    '#submit' => $search_submit,
  );

  $form['advanced_link'] = array(
    '#markup' => l(t('Advanced') . '<span style="margin: 0 5px;"></span>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/filters/' . $entity_type, array('html' => TRUE, 'attributes' => array('title' => t('Advanced Search'), 'class' => $modal_classes)))
  );

  if ($filter_set) {
    $form['filter_message'] = array(
      '#markup' => '<span style="white-space:nowrap;">' . l('<i class="fa fa-times" style="margin: 0 5px;"></i>' . t('Clear Filters') . '</span>', LINGOTEK_MENU_MAIN_BASE_URL . '/manage/clear/filters/' . $entity_type, array('html' => TRUE, 'attributes' => array('id' => array('clear-filters')))) . '</span>',
    );
  }

  // Build actions selector
  $form['select_actions'] = array(
    '#type' => 'select',
    '#options' => $action_options,
  );

  $actions_submit_function = $entity_type == 'config' ? array('lingotek_config_action_submit') : array('lingotek_grid_action_submit');

  // Actions submit button
  $form['submit_actions'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Action'),
    '#name' => 'submit_actions',
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

function lingotek_filter_set_check($entity_type) {
  $filter_set = FALSE;
  if (isset($_SESSION['grid_filters'][$entity_type])) {
    foreach ($_SESSION['grid_filters'][$entity_type] as $key => $value) {
      if($key === 'filtered_ids' || $key === 'filtered_config_lids') {//these keys are filter metadata
        continue;
      }
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

  return $filter_set;
}

function lingotek_grid_action_options($entity_type) {
  $delete_text = $entity_type == 'config' ? t('Delete selected translations') : t('Delete selected content');
  $action_options = array(
    'select' => t('Actions...'),
    'upload' => t('Upload source for translation'),
    'sync' => t('Check progress of translations'),
    'reset' => t('Disassociate translations'),
    'delete' => $delete_text,
    'marked' => t('Mark content'),
    'unmarked' => t('Unmark content')
  );
	$api = LingotekApi::instance();
	$workflows = $api->listWorkflows();
	if (is_array($workflows) && count($workflows) > 1) {
		$action_options['workflow'] = t('Change workflow');
  }
  if ($entity_type == 'config') {
    $action_options[t('Download')] = array('download_all' => t('Download All Translations'));

  }
  else {
    $action_options['delete_translations'] = t('Delete local translations');
    $action_options['edit'] = t('Edit translation settings');
    $action_options[t('Download')] = array('download_all' => t('Download All Translations'));
  }

  $target_languages_raw = Lingotek::getLanguages();

  foreach ($target_languages_raw as $target_raw) {
    $action_options[t('Download')]['download_' . $target_raw->lingotek_locale] = t('Download') . ' ' . t($target_raw->name) . ' (' . $target_raw->lingotek_locale . ') ' . t('Translation');
  }
  return $action_options;
}

function lingotek_grid_search_options($entity_type) {
  $search_options = array(
    'title' => t('Title'),
  );

  if ($entity_type != 'fieldable_panels_pane') {
    $search_options['all'] = t('All');
    $search_options['body'] = t('Body');
  }

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

  if (variable_get('lingotek_translate_config_webform', 0)) {
    $search_options['webform'] = t('Webforms');
  }


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

  $form_state['values']['filters'] = lingotek_grid_get_filters($form_state['entity_type'], TRUE);

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

  $form_state['values']['columns'] = $form_state['entity_type'] == 'config' ? lingotek_grid_get_columns('config') : lingotek_grid_get_columns($_SESSION['grid_entity_type']);
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

function lingotek_grid_legend_form($form, $form_state) {
  $css_style = 'background-color:#80a49e; margin-right: 3px; padding:1px; padding-left:4px; padding-right:4px; color:#fff; white-space:nowrap; border:1px solid #fff; text-decoration:none; font-family: \'Roboto Mono\', monospace;';

  $form['lingotek_legend_header'] = array(
    '#markup' => '<h2 style="margin-top:0;">' . t('Icon descriptions') . '</h2> '
  );
  $legend_table = "<table class='legend-table'>"
      . "<tr>"
        . '<td><div><i class="fa fa-cloud-upload fa-2x ltk-upload" style="color:#444;"></i></div></td>'
        . '<td>'. t('Upload content to Lingotek for translation.') .'</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-refresh fa-2x ltk-refresh" style="color:#F37F16;"></i></div></td>'
        . '<td>' . t('Check the status of <i>In-Progress</i> translations.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-cloud-download fa-2x ltk-download" style="color:#2196F3;"></i></div></td>'
        . '<td>' . t('Download all translations with status <i>Ready for Download</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-list-alt fa-2x" style="color:#666;"></i></div></td>'
        . '<td>' . t('Choose which table columns to show or hide.</td>')
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-sort-amount-asc fa-2x" style="font-size: 150%; color:#666;"></i></div></td>'
        . '<td>' . t('(Config page only) Download all content regardless of current status.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-edit fa-2x" style="color: #0074BD"></i></div></td>'
        . '<td>' . t('Edit the content associated with this row.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-tasks fa-2x" style="color: #0074BD"></div></td>'
        . '<td>' . t('View current translation progress for content.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><i class="fa fa-gear fa-2x" style="color: #0074BD"></i></div></td>'
        . '<td>' . t('Adjust Lingotek module settings.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-none" style="' . $css_style . ' background-color: #FFFFFF; border: 1px solid #999999; color: #999999;">lang</a></div></td>'
        . '<td>' . t('This content has <i>No Tranlation</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-current" style="' . $css_style . ' background-color:#5FDC64;border: 1px solid #5FDC64;">lang</a></div></td>'
        . '<td>' . t('This content is current; no translations are <i>In-Progress</i> or <i>Ready for Download</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-edited" style="' . $css_style . ' background-color: #ffc107; border: 1px solid #ffc107; color: #575757;">lang</a></div></td>'
        . '<td>' . t('This content has been edited and is ready for <i>Re-upload</i> to Lingotek.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-pending" style="' . $css_style . ' background-color: #F37F16; border: 1px solid #F37F16;">lang</a></div></td>'
        . '<td>' . t('This content has been uploaded to Lingotek and the translations are <i>In-Progress</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-ready" style="' . $css_style . ' background-color:#2196F3; border: 1px solid #2196F3;">lang</a></div></td>'
        . '<td>' . t('This content has been translated and is <i>Ready for Download</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-ready-interim" style="' . $css_style . ' background-color: #FFFFFF; border: 1px solid #2196F3; color: #2196F3;">lang</a></div></td>'
        . '<td>' . t('This content is <i>Ready for Interim Download</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-interim" style="' . $css_style . ' background-color:#FFFFFF; border: 1px solid #4CAF50; color: #4CAF50">lang</a></div></td>'
        . '<td>' . t('This content has an interim <i>In-Progress</i> translation downloaded.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-error" style="' . $css_style . ' background-color: #B71C1C; border: 1px solid #B71C1C; color: #ffffff;">lang</a></div></td>'
        . '<td>' . t('This content has received an <i>Error</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-disabled" style="' . $css_style . ' background-color: #FFFFFF; border: 1px solid #999999; color: #999999; text-decoration: line-through;">lang</a></div></td>'
        . '<td>' . t('This content is <i>Disabled, cannot request translation</i>.') . '</td>'
      . "</tr>"

      . "<tr>"
        . '<td><div><a class="language-icon target-untracked" style="' . $css_style . ' background-color: #999999; border: 1px solid #999999;">lang</a></div></td>'
        . '<td>' . t('This content has translations, <i>but are not being tracked by Lingotek</i>.') . '</td>'
      . "</tr>"

      . "</table>";
  $form['lingotek_legend_table'] = array(
    '#markup' => $legend_table
  );

  return $form;
}

function lingotek_grid_legend() {
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
  );
  $output = ctools_modal_form_wrapper('lingotek_grid_legend_form', $form_state);

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
  $entity_type = $_SESSION['grid_entity_type'];
  $_SESSION['grid_filters'][$entity_type]['search_type'] = $form_state['values']['search_type'];
  if (!empty($form_state['values']['search'])) {
    $_SESSION['grid_filters'][$entity_type]['search'] = $form_state['values']['search'];
  }
  else {
    unset($_SESSION['grid_filters'][$entity_type]['search']);
    unset($_SESSION['grid_filters'][$entity_type]['search_type']);
  }
  unset($_SESSION['grid_filters'][$entity_type]['body']);
  unset($_SESSION['grid_filters'][$entity_type]['title']);
  if ($form_state['values']['search_type'] == 'title') {
    $_SESSION['grid_filters'][$entity_type]['title'] = $form_state['values']['search'];
  }
  elseif ($form_state['values']['search_type'] == 'body') {
    $_SESSION['grid_filters'][$entity_type]['body'] = $form_state['values']['search'];
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
      $nest = 'grid_filters'; //stored in $_SESSION['grid_filters'][$entity_type][$key]
    }
    elseif (strpos($key, '__custom')) {
      $add_key_to_session = TRUE;
      $nest = 'grid_custom'; //stored in $_SESSION['grid_custom'][$entity_type][$key]
    }
    // if we want this key, add it to the session
    if ($add_key_to_session) {
      if (is_null($nest)) {
        $_SESSION[$key] = $value;
      }
      else {
        if (!isset($_SESSION[$nest])) {
          $_SESSION[$nest] = array();
        }
        if (!isset($_SESSION[$nest][$form_state['entity_type']])) {
          $_SESSION[$nest][$form_state['entity_type']] = array();
        }
        $_SESSION[$nest][$form_state['entity_type']][str_replace('__filter', '', $key)] = $value;
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

  if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#name'] == 'submit_actions') { // If submitting an action
    foreach ($form_state['values']['the_grid'] as $value) {
      if ($value != 0) {
        $entity_ids[] = $value;
      }
    }

    if (isset($form_state['values']['select_actions'])) { // If an action was selected (which it would be, I don't know if this could ever NOT occur with normal use)
      $action = $form_state['values']['select_actions']; // Get the action
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
      elseif ($action == 'delete' || $action == 'reset' || $action == 'delete-translations') {
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
      elseif ($action == 'marked') {
        $batch = array(
          'title' => t('Updating Marked Entities'),
          'finished' => 'lingotek_update_marked_items_finished'
        );

        $operations = lingotek_get_marked_batch_elements($entity_type, $entity_ids);
        $batch['operations'] = $operations;
        $redirect = current_path();

        batch_set($batch);
        batch_process($redirect);
      }
      elseif ($action == 'unmarked') {
        $batch = array(
          'title' => t('Updating Unmarked Entities'),
          'finished' => 'lingotek_update_unmarked_items_finished'
        );

        $operations = lingotek_get_unmarked_batch_elements($entity_type, $entity_ids);
        $batch['operations'] = $operations;
        $redirect = current_path();

        batch_set($batch);
        batch_process($redirect);
      }
    }
  }
}

/**
* Updates marked value in the lingotek_entity_metadata table for one row
*/
function lingotek_update_marked_item_single($entity_id, $entity_type, $marked_value) {
  if ($entity_type === 'config') {
    $marked_offset = $entity_id + LingotekSync::MARKED_OFFSET;
    if ($marked_value == LingotekSync::MARKED) {
      LingotekSync::deleteConfigMarkedValue($entity_id, $marked_offset, 'marked');
    }
    else {
      LingotekSync::setConfigMarkedValue($entity_id, $marked_offset);
    }
    return;
  }

  if ($marked_value == LingotekSync::MARKED) {
    lingotek_keystore_delete($entity_type, $entity_id, 'marked');
  }
  else {
    lingotek_keystore($entity_type, $entity_id, 'marked', LingotekSync::MARKED);
  }
}

/**
* Updates marked value in the lingotek_entity_metadata table
*/
function lingotek_update_marked_items($entity_type, $entity_id, &$context) {
  if(empty($context['results'])) {
    $context['results']['marked'] = 1;
    if ($entity_type === 'config') {
      $context['results']['entity_type'] = 'config';
    }
  }
  else {
    $context['results']['marked'] += 1;
  }

  if ($entity_type === 'config') {
    $marked_offset = $entity_id + LingotekSync::MARKED_OFFSET;
    LingotekSync::setConfigMarkedValue($entity_id, $marked_offset, LingotekSync::MARKED);
  }
  else {
    lingotek_keystore($entity_type, $entity_id, 'marked', LingotekSync::MARKED);
  }
}

/**
* Updates unmarked value in the lingotek_entity_metadata table
*/
function lingotek_update_unmarked_items($entity_type, $entity_id, &$context) {
  if(empty($context['results'])) {
    $context['results']['unmarked'] = 1;
    if ($entity_type === 'config') {
      $context['results']['entity_type'] = 'config';
    }
  }
  else {
    $context['results']['unmarked'] += 1;
  }

  if ($entity_type === 'config') {
    $marked_offset = $entity_id + LingotekSync::MARKED_OFFSET;
    LingotekSync::deleteConfigMarkedValue($entity_id, $marked_offset, 'marked');
  }
  else {
    lingotek_keystore_delete($entity_type, $entity_id, 'marked');
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
  $e_grid_custom = isset($_SESSION['grid_custom'][$entity_type]) ? $_SESSION['grid_custom'][$entity_type] : array();

  $column_elements = array(
    'marked' => array(
      '#type' => 'checkbox',
      '#title' => t('Marked'),
      '#default_value' => isset($e_grid_custom[$prefix . 'marked' . $suffix]) ? $e_grid_custom[$prefix . 'marked' . $suffix] : in_array('marked', $columns['defaults']),
    ),
    'nid' => array(
      '#type' => 'checkbox',
      '#title' => t('ID'),
      '#default_value' => isset($e_grid_custom[$prefix . 'nid' . $suffix]) ? $e_grid_custom[$prefix . 'nid' . $suffix] : in_array('nid', $columns['defaults']),
    ),
    'content_type' => array(
      '#type' => 'checkbox',
      '#title' => t('Content Type'),
      '#default_value' => isset($e_grid_custom[$prefix . 'content_type' . $suffix]) ? $e_grid_custom[$prefix . 'content_type' . $suffix] : in_array('content_type', $columns['defaults']),
    ),
    'title' => array(
      '#type' => 'checkbox',
      '#title' => t('Title'),
      '#default_value' => isset($e_grid_custom[$prefix . 'title' . $suffix]) ? $e_grid_custom[$prefix . 'title' . $suffix] : in_array('title', $columns['defaults']),
    ),
    'label' => array(
      '#type' => 'checkbox',
      '#title' => t('Label'),
      '#default_value' => isset($e_grid_custom[$prefix . 'label' . $suffix]) ? $e_grid_custom[$prefix . 'label' . $suffix] : in_array('label', $columns['defaults']),
    ),
    'description' => array(
      '#type' => 'checkbox',
      '#disabled' => 'true',
      '#title' => t('Description'),
      '#default_value' => isset($e_grid_custom[$prefix . 'nid' . $suffix]) ? $e_grid_custom[$prefix . 'description' . $suffix] : in_array('description', $columns['defaults']),
    ),
    'language' => array(
      '#type' => 'checkbox',
      '#title' => t('Source'),
      '#default_value' => isset($e_grid_custom[$prefix . 'language' . $suffix]) ? $e_grid_custom[$prefix . 'language' . $suffix] : in_array('language', $columns['defaults']),
    ),
    'translations' => array(
      '#type' => 'checkbox',
      '#title' => t('Translations'),
      '#default_value' => isset($e_grid_custom[$prefix . 'translations' . $suffix]) ? $e_grid_custom[$prefix . 'translations' . $suffix] : in_array('translations', $columns['defaults']),
    ),
    'configuration' => array(
      '#type' => 'checkbox',
      '#title' => t('Profile'),
      '#default_value' => isset($e_grid_custom[$prefix . 'configuration' . $suffix]) ? $e_grid_custom[$prefix . 'configuration' . $suffix] : in_array('configuration', $columns['defaults']),
    ),
    'document_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Doc ID'),
      '#default_value' => isset($e_grid_custom[$prefix . 'document_id' . $suffix]) ? $e_grid_custom[$prefix . 'document_id' . $suffix] : in_array('document_id', $columns['defaults']),
    ),
    'workflow_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Workflow ID'),
      '#default_value' => isset($e_grid_custom[$prefix . 'workflow_id' . $suffix]) ? $e_grid_custom[$prefix . 'workflow_id' . $suffix] : in_array('workflow_id', $columns['defaults']),
    ),
    'workflow_name' => array(
      '#type' => 'checkbox',
      '#title' => t('Workflow Name'),
      '#default_value' => isset($e_grid_custom[$prefix . 'workflow_name' . $suffix]) ? $e_grid_custom[$prefix . 'workflow_name' . $suffix] : in_array('workflow_name', $columns['defaults']),
    ),
    'changed' => array(
      '#type' => 'checkbox',
      '#title' => t('Last Modified'),
      '#default_value' => isset($e_grid_custom[$prefix . 'changed' . $suffix]) ? $e_grid_custom[$prefix . 'changed' . $suffix] : in_array('changed', $columns['defaults']),
    ),
    'last_uploaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Last Uploaded'),
      '#default_value' => isset($e_grid_custom[$prefix . 'last_uploaded' . $suffix]) ? $e_grid_custom[$prefix . 'last_uploaded' . $suffix] : in_array('last_uploaded', $columns['defaults']),
    ),
    'locale_progress_percent' => array(
      '#type' => 'checkbox',
      '#title' => t('Target Progress Percentage'),
      '#default_value' => isset($e_grid_custom[$prefix . 'locale_progress_percent' . $suffix]) ? $e_grid_custom[$prefix . 'locale_progress_percent' . $suffix] : in_array('locale_progress_percent', $columns['defaults']),
    ),
    'progress_updated' => array(
      '#type' => 'checkbox',
      '#title' => t('Progress Last Updated'),
      '#default_value' => isset($e_grid_custom[$prefix . 'progress_updated' . $suffix]) ? $e_grid_custom[$prefix . 'progress_updated' . $suffix] : in_array('progress_updated', $columns['defaults']),
    ),
    'last_downloaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Time Last Downloaded'),
      '#default_value' => isset($e_grid_custom[$prefix . 'last_downloaded' . $suffix]) ? $e_grid_custom[$prefix . 'last_downloaded' . $suffix] : in_array('last_downloaded', $columns['defaults']),
    ),
    'actions' => array(
      '#type' => 'checkbox',
      '#title' => t('Actions'),
      '#default_value' => isset($e_grid_custom[$prefix . 'actions' . $suffix]) ? $e_grid_custom[$prefix . 'actions' . $suffix] : in_array('actions', $columns['defaults']),
    ),
  );
  if ($form_state['entity_type'] == 'taxonomy_term') {
    $column_elements['description']['#disabled'] = 'true';
    $column_elements['title']['#disabled'] = 'true';
  }
  if ($entity_type == 'paragraphs_item') {
    unset($column_elements['changed']);
  }
  $column_elements = array_intersect_key($column_elements, $columns['columns']); // Reduces the output columns to the defaults specified in 'lingotek_grid_define_columns'

  return lingotek_grid_process_elements($column_elements, $prefix, $suffix); // adds prefixes and suffixes to the elements
}

function lingotek_config_build_column_checkboxes($form_state) {
  $prefix = '';
  $suffix = '__custom'; // Suffix specified because the filter submit function differentiates based on this tag and puts the keys into the session variable as such
  $columns = lingotek_config_define_columns(); // Allowed columns and defaults for source and target grids are defined here
  $c_grid_custom = isset($_SESSION['grid_custom']['config']) ? $_SESSION['grid_custom']['config'] : array();
  $column_elements = array(
    'marked' => array(
      '#type' => 'checkbox',
      '#title' => t('Marked'),
      '#default_value' => isset($c_grid_custom[$prefix . 'marked' . $suffix]) ? $c_grid_custom[$prefix . 'marked' . $suffix] : in_array('marked', $columns['defaults']),
    ),
    'lid' => array(
      '#type' => 'checkbox',
      '#title' => t('ID'),
      '#default_value' => isset($c_grid_custom[$prefix . 'lid' . $suffix]) ? $c_grid_custom[$prefix . 'lid' . $suffix] : in_array('lid', $columns['defaults']),
    ),
    'config_set_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Config Set ID'),
      '#default_value' => isset($c_grid_custom[$prefix . 'config_set_id' . $suffix]) ? $c_grid_custom[$prefix . 'config_set_id' . $suffix] : in_array('config_set_id', $columns['defaults']),
    ),
    'set_name' => array(
      '#type' => 'checkbox',
      '#title' => t('Config Set Name'),
      '#default_value' => isset($c_grid_custom[$prefix . 'set_name' . $suffix]) ? $c_grid_custom[$prefix . 'set_name' . $suffix] : in_array('set_name', $columns['defaults']),
    ),
    'source' => array(
      '#type' => 'checkbox',
      '#title' => t('Source'),
      '#default_value' => isset($c_grid_custom[$prefix . 'source' . $suffix]) ? $c_grid_custom[$prefix . 'source' . $suffix] : in_array('source', $columns['defaults']),
    ),
    'source_uploaded' => array(
      '#type' => 'checkbox',
      '#title' => t('Source Uploaded'),
      '#default_value' => isset($c_grid_custom[$prefix . 'source_uploaded' . $suffix]) ? $c_grid_custom[$prefix . 'source_uploaded' . $suffix] : in_array('source_uploaded', $columns['defaults']),
    ),
    'translations' => array(
      '#type' => 'checkbox',
      '#title' => t('Translations'),
      '#default_value' => isset($c_grid_custom[$prefix . 'translations' . $suffix]) ? $c_grid_custom[$prefix . 'translations' . $suffix] : in_array('translations', $columns['defaults']),
    ),
    'location' => array(
      '#type' => 'checkbox',
      '#title' => t('Location'),
      '#default_value' => isset($c_grid_custom[$prefix . 'location' . $suffix]) ? $c_grid_custom[$prefix . 'location' . $suffix] : in_array('location', $columns['defaults']),
    ),
    'context' => array(
      '#type' => 'checkbox',
      '#title' => t('Context'),
      '#default_value' => isset($c_grid_custom[$prefix . 'context' . $suffix]) ? $c_grid_custom[$prefix . 'context' . $suffix] : in_array('context', $columns['defaults']),
    ),
    'doc_id' => array(
      '#type' => 'checkbox',
      '#title' => t('Document ID'),
      '#default_value' => isset($c_grid_custom[$prefix . 'doc_id' . $suffix]) ? $c_grid_custom[$prefix . 'doc_id' . $suffix] : in_array('doc_id', $columns['defaults']),
    ),
    'textgroup' => array(
      '#type' => 'checkbox',
      '#title' => t('Textgroup'),
      '#default_value' => isset($c_grid_custom[$prefix . 'textgroup' . $suffix]) ? $c_grid_custom[$prefix . 'textgroup' . $suffix] : in_array('textgroup', $columns['defaults']),
    ),
    'workflow' => array(
      '#type' => 'checkbox',
      '#title' => t('Workflow'),
      '#default_value' =>isset($c_grid_custom[$prefix . 'textgroup' . $suffix]) ? $c_grid_custom[$prefix . 'workflow' . $suffix] : in_array('workflow', $columns['defaults']),
    ),
  );

  $column_elements = array_intersect_key($column_elements, $columns['columns']); // Reduces the output columns to the defaults specified in 'lingotek_grid_define_columns'

  return lingotek_grid_process_elements($column_elements, $prefix, $suffix); // adds prefixes and suffixes to the elements
}

function lingotek_grid_update($entity_type) {
  if ($entity_type == 'config') {
    $lids = LingotekConfigSet::getLidsByStatus(LingotekSync::STATUS_PENDING);
    if (empty($lids)) {
      drupal_set_message(t('There are no <i>In-Progress</i> translations to check.'));
      drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
    }
    lingotek_config_update_selected($lids);
    return;
  }

  $active_locales = lingotek_get_target_locales();
  $entity_ids = LingotekSync::getEntityIdsByStatuses($entity_type, array(LingotekSync::STATUS_PENDING, LingotekSync::STATUS_READY_INTERIM, LingotekSync::STATUS_INTERIM), $active_locales);
  variable_set('lingotek_pending_last_updated', time());
  if (count($entity_ids) > 0) {
    $_SESSION['lingotek_sync_nodes'] = count($entity_ids);
    lingotek_update_target_progress_batch_create($entity_type, $entity_ids); // Run batch operations to sync all 'In Progress' nodes
  }
  else {
    drupal_set_message(t('There are no <i>In-Progress</i> translations to check.'));
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
  }
}

function lingotek_grid_query_status($entity_type, $comma_separated_ids = ''){
  if (strlen($comma_separated_ids) == 0) {
    return;
  }
  $ids = explode(',', $comma_separated_ids);
  $information = array();
  foreach($ids as $id) {
    $source_status = LingotekSync::getUploadStatus($entity_type, $id);
    $target_statuses = LingotekSync::getAllTargetStatusForEntity($entity_type, $id);
    $profile = LingotekSync::getProfileByEntityId($entity_type, $id);
    $last_sync_error = LingotekSync::getLastSyncError($entity_type, $id);

    $information[$id] = $target_statuses;
    $information[$id]['source_status'] = $source_status;
    $information[$id]['profile'] = $profile;
    if ($last_sync_error) {
      $information[$id]['last_upload_error'] = $last_sync_error;
    }

    if ($entity_type === 'node') {
      $query = db_select('node', 'n')
        ->fields('n', array('changed'))
        ->condition('n.nid', $id);
      $timestamp = $query->execute()->fetch();
      $information[$id]['last_modified'] = lingotek_human_readable_timestamp($timestamp->changed) . ' ago';
    }
  }
  echo json_encode($information);
}

function lingotek_grid_automatic_download($entity_type){
  $or = db_or()->condition('lem.value','READY')->condition('lem.value','READY_INTERIM');
  $query = db_select('lingotek_entity_metadata', 'lem');
  $query->addField('lem', 'entity_id');
  $query->addField('lem', 'entity_key');
  //$query->condition('lem.value', 'READY');
  $query->condition($or);
  $query->condition('lem.entity_type', $entity_type);
  $results = $query->execute();
  $ids_to_check = array();
  $translation_status = array();
  foreach($results as $result){
    $entity_id = $result->entity_id;
    $ids_to_check[] = $entity_id;
    $language_code = substr($result->entity_key,count($result->entity_key) - 6);
    $translation_status[$entity_id][$language_code] = $language_code;
  }
  $entities = entity_load($entity_type, $ids_to_check);

  foreach ($entities as $entity) {
    if($entity->lingotek['auto_download'] === 1){
      $id = $entity_type === 'node' ? $entity->nid : $entity->cid;
      foreach($translation_status[$id] as $lang_code){
        lingotek_entity_download($entity,$entity_type,$lang_code);
      }
    }
  }
}
function lingotek_grid_download_selected($entity_type, $entity_ids, $target_locales) {
  $operations = array();
  $entities = entity_load($entity_type, $entity_ids);
  $all_target_locales = $target_locales;
  foreach ($entities as $entity) {
    $target_locales = $all_target_locales; // must reset $target_locales to the original array or else some target languages may be skipped during download
    if (variable_get('lingotek_enable_language_specific_profiles')) {
      // Filter out the target languages that should not be included for this entity.
      $profile = LingotekProfile::loadByEntity($entity_type, $entity);
      $target_locales = array_keys($profile->filterTargetLocales($target_locales));
    }
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

function config_download_ready($force_download, $comma_separated_ids = NULL, $entity_type = NULL){
  if ($entity_type === NULL) {
    $entity_type = 'node';
  }
  if($force_download != NULL){
    $ready_lids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_READY);
    $current_lids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_CURRENT);
    $interim_current_lids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_INTERIM);
    $interim_ready_lids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_READY_INTERIM);
    $edited_lids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_EDITED);
    $set_ids = array_merge($ready_lids, $current_lids, $edited_lids, $interim_current_lids, $interim_ready_lids);
  }
  elseif(isset($_SESSION['grid_filters'][$entity_type]) || $comma_separated_ids !== NULL){
    if(isset($_SESSION['grid_filters'][$entity_type])){
      empty_array_check($_SESSION['grid_filters'][$entity_type]['filtered_config_lids'], $entity_type, "There are no translations ready for download for this filter.");
    }
    $selected_lids = $comma_separated_ids !== NULL ? explode(",", $comma_separated_ids)
        : $_SESSION['grid_filters'][$entity_type]['filtered_config_lids'];
    $set_ids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_READY, $selected_lids);
  }
  else {
    $set_ids = LingotekConfigSet::getSetIdsByStatus(LingotekSync::STATUS_READY);
  }
  lingotek_config_download_selected('download_all', $set_ids, true);
}

function lingotek_grid_download_ready($entity_type, $comma_separated_ids = NULL) {
  if ($entity_type === 'config') {
    config_download_ready(NULL, $comma_separated_ids, $entity_type);
    return;
  }
  $targets = LingotekSync::getTargetsByStatuses($entity_type, array(LingotekSync::STATUS_READY, LingotekSync::STATUS_READY_INTERIM));
  if (!empty($targets)) {
    if($comma_separated_ids !== NULL || (isset($_SESSION['grid_filters'][$entity_type]) && !empty($_SESSION['grid_filters'][$entity_type]))){
      if(isset($_SESSION['grid_filters'][$entity_type])){
        $filtered_ids = isset($_SESSION['grid_filters'][$entity_type]['filtered_ids']) ? isset($_SESSION['grid_filters'][$entity_type]['filtered_ids']) : NULL;
        empty_array_check($filtered_ids, $entity_type, "There are no translations ready for download for this filter.");
      }
      $entity_ids = $comma_separated_ids !== NULL ? explode(",", $comma_separated_ids)
          : $_SESSION['grid_filters'][$entity_type]['filtered_ids'];
    }
    else {
      $entity_ids = array();
      foreach ($targets as $target) {
        $entity_ids[] = $target['id'];
      }
    }
    $entities = entity_load($entity_type, $entity_ids);
    $operations = array();
    foreach ($targets as $target) {
      if (isset($entities[$target['id']]) &&
          $entities[$target['id']]->lingotek['profile'] != LingotekSync::PROFILE_DISABLED) { // exclude nodes with PROFILE_DISABLED
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
    drupal_set_message(t('There are no translations with status <i>Ready for Download</i>.'));
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
  }
}

function config_upload_edited($comma_separated_ids, $entity_type) {
    if($comma_separated_ids !== NULL){
      $lids = explode(",", $comma_separated_ids);
      lingotek_config_upload_selected($lids);
    }
    elseif(isset($_SESSION['grid_filters'][$entity_type])){
      empty_array_check($_SESSION['grid_filters'][$entity_type]['filtered_config_lids'], $entity_type, "There are no entities ready for upload for this filter");
      $selected_lids = $_SESSION['grid_filters'][$entity_type]['filtered_config_lids'];
      //check lid selection for dirty statuses
      $edited_lid_map = LingotekConfigSet::getLidsToUpdate(0, $selected_lids);
      $never_lid_map = LingotekConfigSet::findNeverUploadedLids($selected_lids);
      $lid_map = array();
      if(!empty($edited_lid_map) && !empty($never_lid_map)){
        foreach($edited_lid_map as $key => $textgroup){
          if(isset($never_lid_map[$key])){
            $lid_map[$key] = array_merge($never_lid_map[$key], $textgroup);
          }
        }
      }
      else {
        $lid_map = empty($edited_lid_map) ? $never_lid_map
            : $edited_lid_map;
      }
    }
    else {
      // Get all lids that are ready to upload
      $lid_map = LingotekConfigSet::getLidsToUpdate();
    }
    lingotek_config_upload_selected($lid_map);
}

function lingotek_grid_upload_edited($entity_type, $comma_separated_ids = NULL) {
  if ($entity_type === 'config') {
    config_upload_edited($comma_separated_ids, $entity_type);
    return;
  }
  if($comma_separated_ids !== NULL) {
    $entity_ids = explode(",", $comma_separated_ids);
  }
  elseif(isset($_SESSION['grid_filters'][$entity_type])){
    empty_array_check($_SESSION['grid_filters'][$entity_type]['filtered_ids'], $entity_type, "There are no entities ready for upload for this filter");
    $selected_ids = $_SESSION['grid_filters'][$entity_type]['filtered_ids'];
    //check lid selection for dirty statuses
    $entity_ids = LingotekSync::getEntityIdsToUpload($entity_type, $selected_ids);
    empty_array_check($entity_ids, $entity_type, "There are no entities ready for upload for this selection. Note: To upload disassociated content, select it using the checkboxes first.");
  }
  else {
    // Get all lids that are ready to upload
    $entity_ids = LingotekSync::getEntityIdsToUpload($entity_type);
  }

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
    drupal_set_message(t('There are no edited entities ready for <i>Re-upload</i>.'));
    drupal_goto('admin/settings/lingotek/manage/' . $entity_type);
  }
}
function empty_array_check($array, $entity_type, $message){
  if(empty($array)){
    drupal_set_message(t(check_plain($message)));
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
  if (!isset($_SESSION['grid_custom'][$grid_name])) { // If the columns do not exist yet in the session variable we get an error, so reset them here.
    lingotek_grid_reset_columns();
  }

  foreach ($_SESSION['grid_custom'][$grid_name] as $key => $value) {
    $columns[str_replace('__custom', '', $key)] = $value;
  }

  return $columns;
}

/**
 * Cleans up the grid_custom or config_custom session array, restoring the predefined defaults.
 */
function lingotek_grid_reset_columns() {
  $grid_name = $_SESSION['grid_entity_type'];
  if (isset($_SESSION['grid_custom'][$grid_name])) {
    unset($_SESSION['grid_custom'][$grid_name]);
  }
  $source_columns = $grid_name == 'config' ? lingotek_config_define_columns() : lingotek_grid_define_columns($grid_name);
  foreach ($source_columns['columns'] as $column) {
    $_SESSION['grid_custom'][$grid_name][$column . '__custom'] = in_array($column, $source_columns['defaults']);
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
    'marked' => 'marked',
    'nid' => 'nid',
    'content_type' => 'content_type',
    'title' => 'title',
    'language' => 'language',
    'translations' => 'translations',
    'configuration' => 'configuration',
    'document_id' => 'document_id',
    'workflow_id' => 'workflow_id',
    'workflow_name' => 'workflow_name',
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
  if ($entity_type == 'bean'){
    $columns['columns']['label'] = 'label';
  }

  return $columns;
}

function lingotek_config_define_columns() {
  $columns = array(
    'columns' => array(
      'marked' => 'marked',
      'lid' => 'lid',
      'config_set_id' => 'config_set_id',
      'set_name' => 'set_name',
      'source' => 'source',
      'source_uploaded' => 'source_uploaded',
      'translations' => 'translations',
      'location' => 'location',
      'context' => 'context',
      'doc_id' => 'doc_id',
      'workflow' => 'workflow',
      'textgroup' => 'textgroup',
    ),
    'defaults' => array(
      'lid' => 'lid',
      'set_name' => 'set_name',
      'source' => 'source',
      'source_uploaded' => 'source_uploaded',
      'translations' => 'translations',
      'textgroup' => 'textgroup',
      'workflow' => 'workflow',
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
  $profiles[LingotekSync::PROFILE_DISABLED] = t('Disabled');

  $profile_defaults = lingotek_get_profiles();
  foreach ($profile_defaults as $key => $p) {
    $profiles[$key] = $p->getName();
  }
  unset($profiles['CONFIG']);
  asort($profiles);

  $entity_type = $form_state['entity_type'];

  $filters = array(
    'nid' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['nid']) ? $_SESSION['grid_filters'][$entity_type]['nid'] : '',
      '#title' => t('Entity ID is'),
      '#size' => 8,
    ),
    'document_id' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['document_id']) ? $_SESSION['grid_filters'][$entity_type]['document_id'] : '',
      '#title' => t('Doc ID is'),
      '#size' => 10,
    ),
    'source_language' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['source_language']) ? $_SESSION['grid_filters'][$entity_type]['source_language'] : 'all',
      '#title' => t('Source Language'),
      '#options' => array('all' => t('All Languages')) + $source_languages,
    ),
    'profile' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['profile']) ? $_SESSION['grid_filters'][$entity_type]['profile'] : 'all',
      '#title' => t('Translation Profile'),
      '#options' => array('all' => 'All') + $profiles,
      '#multiple' => TRUE
    ),
    'upload_status' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['upload_status']) ? $_SESSION['grid_filters'][$entity_type]['upload_status'] : 'all',
      '#title' => t('Upload Status'),
      '#options' => array(
        'all' => t('All'),
        LingotekSync::STATUS_EDITED => t('Out of Sync'),
        LingotekSync::STATUS_CURRENT => t('In Sync'),
        'uploaded' => t('Uploaded'),
        'upload_failed' => t('Upload Failed'),
        'never' => t('Never Uploaded'),
        'disassoc' => t('Disassociated'),
      ),
      '#multiple' => FALSE,
    ),
    'marked_status' => array (
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['marked_status']) ? $_SESSION['grid_filters'][$entity_type]['marked_status'] : 'all',
      '#title' => t('Marked status'),
      '#options' => array (
        'all' => t('All'),
        'marked' => t('Marked'),
        'unmarked' => t('Not marked'),
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
    asort($grid_bundles);
    $filters['content_type'] = array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['content_type']) ? $_SESSION['grid_filters'][$entity_type]['content_type'] : 'all',
      '#title' => t('Content Type(s)'),
      '#options' => array('all' => t('All')) + $grid_bundles,
      '#multiple' => TRUE
    );
  }

  return lingotek_grid_process_elements($filters, '', '__filter'); // Add prefix and suffix to the name of each filter element
}

function lingotek_config_build_filters($form_state) {
  $entity_type = $_SESSION['grid_entity_type'];
  $languages = language_list();
  $search_languages = array('all' => t('All Languages'));
  foreach ($languages as $code => $language) {
    $search_languages[$code] = t($language->name) . ' (' . $language->lingotek_locale . ')';
  }
  $modules = array_map('ucfirst', module_list(FALSE, FALSE, TRUE));

  $filters = array(
    'lid' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['lid']) ? $_SESSION['grid_filters'][$entity_type]['lid'] : '',
      '#title' => t('Config Entity ID is'),
      '#size' => 8,
    ),
    'config_set_id' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['config_set_id']) ? $_SESSION['grid_filters'][$entity_type]['config_set_id'] : '',
      '#title' => t('Config Set ID is'),
      '#size' => 10,
    ),
    'document_id' => array(
      '#type' => 'textfield',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['document_id']) ? $_SESSION['grid_filters'][$entity_type]['document_id'] : '',
      '#title' => t('Doc ID is'),
      '#size' => 10,
    ),
    'location' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['location']) ? $_SESSION['grid_filters'][$entity_type]['location'] : '',
      '#title' => t('Module'),
      '#options' => array('' => '') + $modules,
    ),
    'upload_status' => array(
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['upload_status']) ? $_SESSION['grid_filters'][$entity_type]['upload_status'] : 'all',
      '#title' => t('Upload Status'),
      '#options' => array(
        'all' => t('All'),
        LingotekSync::STATUS_EDITED => t('Edited (needs reupload)'),
        LingotekSync::STATUS_CURRENT => t('Current (uploaded)'),
        'never' => t('Never uploaded'),
      ),
      '#multiple' => FALSE,
    ),
    'marked_status' => array (
      '#type' => 'select',
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['marked_status']) ? $_SESSION['grid_filters'][$entity_type]['marked_status'] : 'all',
      '#title' => t('Marked status'),
      '#options' => array (
        'all' => t('All'),
        'marked' => t('Marked'),
        'unmarked' => t('Not marked'),
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
      '#default_value' => isset($_SESSION['grid_filters'][$entity_type]['content_type']) ? $_SESSION['grid_filters'][$entity_type]['content_type'] : 'all',
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
function lingotek_grid_get_filters($entity_type, $source = TRUE) {
  $filters = array();
  if (isset($_SESSION['grid_filters'][$entity_type])) {
    foreach ($_SESSION['grid_filters'][$entity_type] as $key => $value) {
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
function lingotek_grid_clear_filters($entity_type = NULL) {
  if ($entity_type === NULL || is_array($entity_type)) {
    if (empty($_SESSION['grid_entity_type'])) {
      throw new LingotekException('Attempted to clear filters on an empty entity type!');
    }
    $entity_type = $_SESSION['grid_entity_type'];
  }
  if (isset($_SESSION['grid_filters'][$entity_type])) {
    unset($_SESSION['grid_filters'][$entity_type]);
    session_write_close();
  }
}

function lingotek_grid_clear_filters_page($entity_type) {
  lingotek_grid_clear_filters($entity_type);
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

  if ($entity_type == 'menu_link') {
    $label_col = 'link_title';
  }
  if ($entity_type == 'paragraphs_item') {
    $label_col = 'field_name';
  }

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
    'marked' => array('data' => t('Marked')),
    'nid' => array('data' => t('ID'), 'field' => 'n.' . $entity_id_key),
    'content_type' => array('data' => t('Content Type'), 'field' => 'type'),
    'title' => array('data' => t('Title'), 'field' => 'n.' . $label_col),
    'label' => array('data' => t('Label'), 'field' => 'n.label'),
    'description' => array('data' => t('Description')),
    'language' => array('data' => t('Source')), //, 'field' => 'upload_status'),
    'translations' => array('data' => t('Translations'), 'field' => 't_current_c'),
    'configuration' => array('data' => t('Profile')),
    'workflow_id' => array('data' => t('Workflow ID'), 'field' => 'workflow_id'),
    'workflow_name' => array('data' => t('Workflow Name'), 'field' => 'workflow_name'),
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

  if ($entity_type != 'bean') {
    unset($header['label']);
  }

  if (isset($entity_properties['changed'])) {
    $header['changed']['sort'] = 'desc';
  }
  $form_state['values']['grid_header'] = lingotek_bulk_grid_refine_source_header($header, $columns);
  $query = lingotek_bulk_grid_query($form_state, $count_only, $entity_id_key, $label_col, $entity_type, $limit, $bundle_col, $info, $entity_properties, $eid);
  //picked an arbitrarily large number to ensure all results are gotten
  // Initialize Query and extend paginator and tablesort if necessary
  lingotek_bulk_grid_filter_query($query, $entity_type, $eid, $label_col, $info);

  // If count only, return the count
  if ($count_only) {
    $result = $query->execute();
    return $result->rowCount();
  }

  // Execute the query
  $table_data_raw = $query->distinct()->execute()->fetchAllAssoc($entity_id_key);


  //save filtered id's to session for group download/upload
  if (lingotek_filter_set_check($entity_type)) {
    $no_limit_query = lingotek_bulk_grid_query($form_state, $count_only, $entity_id_key, $label_col, $entity_type, 500000, $bundle_col, $info, $entity_properties, $eid);
    lingotek_bulk_grid_filter_query($no_limit_query, $entity_type, $eid, $label_col, $info);
    $no_limit_query_raw = $no_limit_query->distinct()->execute();
    $all_filtered_entity_ids = array();
    foreach($no_limit_query_raw as $value){
      $all_filtered_entity_ids[$value->$entity_id_key] = $value->$entity_id_key;
    }
    $_SESSION['grid_filters'][$entity_type]['filtered_ids'] = $all_filtered_entity_ids;
  }

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
    $query->limit($limit);
    if($limit <= 5000){//anything greater than 500 is a filtered_id query for the batch filter actions
      $query->orderByHeader($form_state['values']['grid_header']);
    }
  }
  if ($base_table == 'node') {
    $query->where('n.tnid = 0 OR n.tnid = n.nid');
  }

  // Entity Title and Name of Content Type (type)
  $query->fields('n', array($entity_id_key));
  $query->addField('n', $entity_id_key, 'entity_id_key');

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
    $taxonomy_bundles_with_custom_fields = lingotek_get_advanced_vocabularies();

    if (empty($taxonomy_bundles_with_custom_fields)) {
      $taxonomy_bundles_with_custom_fields = array(-1);
    }
    $query->addField('n', 'description');
    $query->innerJoin('taxonomy_vocabulary', 'tv', 'n.vid = tv.vid');
    $query->addField('tv', 'name', 'tv_name');
    $query->addField('tv', 'machine_name', 'type');
    $query->addField('tv', 'i18n_mode', 'translation_mode');
    $query->condition('tv.machine_name', $taxonomy_bundles_with_custom_fields, 'IN');
  }
  elseif ($bundle_col) {
    if ($info['entity keys']['bundle'] != 'type') {
      $query->addField('n', $info['entity keys']['bundle']);
    }
    $query->addField('n', $info['entity keys']['bundle'], 'type');
    // beans have the $bundle_col, and also need the label
    if ($entity_type == 'bean'){
      $query->addField('n', 'label');
    }
    if ($entity_type == 'menu_link') {
      $min_query = db_select('menu_links', 'ml')
        ->condition('ml.i18n_tsid', 0, '!=')
        ->groupBy('i18n_tsid');
      $min_query->addExpression('MIN(mlid)', 'minimum');

      $ml_or = db_or();
      $ml_or->condition('n.i18n_tsid', 0);
      $ml_or->condition('n.mlid', $min_query, 'IN');

      $query->condition('n.language', LANGUAGE_NONE, '!=');
      $query->condition($ml_or);
    }
  }
}

function lingotek_bulk_grid_query_add_statuses($query, $entity_type, $eid) {
  $query->addExpression("(SELECT COUNT(entity_id) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='CURRENT')", 't_current_c');

  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_PENDING . "')", 't_pending');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_READY . "')", 't_ready');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_CURRENT . "')", 't_current');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_INTERIM . "')", 't_interim');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_READY_INTERIM . "')", 't_ready_interim');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_EDITED . "')", 't_edited');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_UNTRACKED . "')", 't_untracked');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_TARGET_LOCALIZE . "')", 't_target_localize');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_TARGET_EDITED . "')", 't_target_edited');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_ERROR . "')", 't_error');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(entity_key, 20, 10)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'target_sync_status_%' AND value='" . LingotekSync::STATUS_NONE . "')", 't_none');
  $query->addExpression("(SELECT GROUP_CONCAT(CONCAT(SUBSTRING(entity_key, 17, 10), ':' , value)) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key LIKE 'source_language_%')", 'lang_override');
}

function lingotek_bulk_grid_query_add_localized_title($query, $entity_type, $eid, $label_col) {
  $localized_label_table = 'field_data_' . $label_col . '_field';

  if ($label_col && db_table_exists($localized_label_table)) {
    $query->addExpression("(SELECT GROUP_CONCAT(" .  $label_col . '_field_value' . ") FROM {" . $localized_label_table . "} WHERE entity_id=" . $eid . " AND entity_type='" . $entity_type . "' AND language = '" . $GLOBALS['language']->language . "')", 'localized_title');
  }
}

function lingotek_bulk_grid_query_add_keys($query, $entity_type, $eid) {
  // Lingotek Document ID
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'document_id')", 'document_id');

  //Invalid xml
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'invalid_xml')", 'invalid_xml');

  // Entity Upload Status
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'upload_status' AND value <> '". LingotekSync::STATUS_TARGET . "' AND value <> '" . LingotekSync::STATUS_DELETED . "')", 'upload_status');

  // Profile Settings
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'profile')", 'profile');

  // Last Uploaded Timestamp
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'last_uploaded')", 'last_uploaded');

  // Any Upload Errors
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'last_sync_error')", 'last_sync_error');

  // Any specifically defined workflow id
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'workflow_id')", 'workflow_id');

  // Any specifically defined workflow name
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'workflow_name')", 'workflow_name');

  // Marked value
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE entity_type='" . $entity_type . "' AND entity_id=" . $eid . " AND entity_key = 'marked')", 'marked');

  // Original source language of the entity, in case of source overwriting option
  if ($entity_type == 'node') {
    $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_entity_metadata} WHERE'" . $eid . "' = entity_id and entity_type ='" . $entity_type . "' and entity_key = 'original_language')", 'original_lang');
  }
}

function lingotek_bulk_grid_filter_query($query, $entity_type, $eid, $label_col, $info) {
  if (!isset($_SESSION['grid_filters'][$entity_type])) {
    return;
  }
  $filters = $_SESSION['grid_filters'][$entity_type];

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
    'locale_progress_percent', 'marked_status');

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
    //never uploaded case
    if(in_array("never", $filters['upload_status'])){
      $notUploadedQuery = db_select('lingotek_entity_metadata','lingo_em');
      $notUploadedQuery->distinct();
      $notUploadedQuery->fields('lingo_em',array('entity_id'));
      $notUploadedQuery->condition('entity_key', 'last_uploaded');

      $query->havingCondition('entity_id_key', $notUploadedQuery, 'NOT IN');
    }
    else if(in_array("disassoc", $filters['upload_status'])){
      $disassocQuery = db_select('lingotek_entity_metadata','lingo_em');
      $disassocQuery->distinct();
      $disassocQuery->fields('lingo_em',array('entity_id'));
      $disassocQuery->condition('entity_key', 'last_uploaded');

      $query->havingCondition('entity_id_key', $disassocQuery, 'IN');
      $query->havingCondition('upload_status', 'UNTRACKED');
    }
    elseif(in_array("uploaded", $filters['upload_status'])) {
      $uploadedQuery = db_select('lingotek_entity_metadata','lingo_em');
      $uploadedQuery->distinct();
      $uploadedQuery->fields('lingo_em', array('entity_id'));
      $uploadedQuery->condition('entity_key', 'document_id');

      $query->havingCondition('entity_id_key', $uploadedQuery, 'IN');
    }
    elseif(in_array("upload_failed", $filters['upload_status'])) {
      $uploadFailedQuery = db_select('lingotek_entity_metadata','lingo_em');
      $uploadFailedQuery->distinct();
      $uploadFailedQuery->fields('lingo_em', array('entity_id'));
      $uploadFailedQuery->condition('entity_key', 'invalid_xml');

      $query->havingCondition('entity_id_key', $uploadFailedQuery, 'IN');
      $query->havingCondition('invalid_xml', LingotekSync::INVALID_XML_PRESENT);
    }
    //other cases
    else {
      $query->havingCondition('upload_status', $filters['upload_status'], 'IN');
    }
  }

  // Marked status
  if (isset($filters['marked_status']) && !in_array('all', $filters['marked_status'])) {
    // Marked
    if(in_array("marked", $filters['marked_status'])){
      $markedQuery = db_select('lingotek_entity_metadata','lingo_em');
      $markedQuery->distinct();
      $markedQuery->fields('lingo_em',array('entity_id'));
      $markedQuery->condition('entity_key', 'marked');
      $markedQuery->condition('value', LingotekSync::MARKED);

      $query->havingCondition('entity_id_key', $markedQuery, 'IN');
    }
    // Unmarked
    else if(in_array("unmarked", $filters['marked_status'])){
      $unMarkedQuery = db_select('lingotek_entity_metadata','lingo_em');
      $unMarkedQuery->distinct();
      $unMarkedQuery->fields('lingo_em',array('entity_id'));
      $unMarkedQuery->condition('entity_key', 'marked');
      $unMarkedQuery->condition('value', LingotekSync::MARKED);

      $query->havingCondition('entity_id_key', $unMarkedQuery, 'NOT IN');
    }
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
    $profiled_entity_ids = array(-1);

    foreach ($filters['profile'] as $p) {
      $profile_obj = LingotekProfile::loadById($p);
      $entities = $profile_obj->getEntities($entity_type);
      $profiled_entity_ids = array_merge($profiled_entity_ids, array_keys($entities));
    }
    foreach ($profiled_entity_ids as $p) {
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
      $query->havingCondition('document_id', NULL);
    }
    else {
      $query->havingCondition('document_id', $filters['document_id']);
    }
  }
}

function lingotek_bulk_grid_filter_last_uploaded($query, $filters) {
  if (isset($filters['last_uploaded']) && $filters['last_uploaded'] != 'all') {
    if ($filters['last_uploaded'] == '1 day') {
      $query->havingCondition('last_uploaded', strToTime($filters['last_uploaded']), '<');
    }
    elseif ($filters['last_uploaded'] == 'unknown') {
      $query->havingCondition('last_uploaded', NULL);
    }
    else {
      $params = explode(' ', $filters['last_uploaded'], 2); // string formatted like '< 1 week', so explode with a limit of two gives us array(0 => '<', 1 => '1 week')
      $query->havingCondition('last_uploaded', strToTime($params[1]), $params[0]);
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
  global $language;
  $languages = language_list();
  $profiles = lingotek_get_profiles();
  $api = LingotekApi::instance();
  $workflows = $api->listWorkflows();
  $table_data = array();
  $node_based_translations = array();

  foreach ($table_data_raw as $row) {
    if($row->language !== $language->language){
      $node_based_translations[] = $row->$entity_id_key;
    }

    $entity_id = $row->{$entity_id_key};
    $entity_type = $entity_properties['type'];

    $row->title = lingotek_truncate_grid_text($row->title, 55);

    $locales_statuses = lingotek_build_locales_statuses($row);

    ksort($locales_statuses);

    $disabled = $row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED;
    $disabled_class = $disabled ? ' ltk-disabled-icon' : '';
    $allow_source_overwriting = !empty($row->lingotek['allow_source_overwriting']);
    $allow_target_localization = !empty($row->lingotek['allow_target_localization']);

    $translation_icons = lingotek_lang_icons($entity_properties['type'], $languages, $entity_id, $locales_statuses, $disabled, $row->language, $allow_source_overwriting, $allow_target_localization); // show translation statuses
    if (!empty($translation_icons['source'])) {
      $row->overridden_source_target_icon = $translation_icons['source'];
      unset($translation_icons['source']);
    }
    $target_icons_str = implode('', array_values($translation_icons));
    $configuration = lingotek_render_configuration($row, $profiles, $disabled_class);
    $source = lingotek_render_source($entity_properties['type'], $row, $languages, $entity_properties['language_col'], $entity_properties);
    $marked_html = lingotek_render_marked_html($row, $entity_type, $entity_id);
    $actions = lingotek_render_actions($entity_properties, $entity_id, $disabled_class);
    $title = $entity_properties['label_col'] ? lingotek_render_title($row, $entity_properties) : '';
    $workflow_id = !empty($row->lingotek['workflow_id']) ? $row->lingotek['workflow_id'] : '';
    $workflow_name = array_key_exists($workflow_id, $workflows) ? $workflows[$workflow_id] : '';

    // Build the data to be output for this row
    $data = array(
      'marked' => $marked_html,
      'nid' => $row->{$entity_id_key} ? : t('??'),
      'title' => $title,
      'label' => isset($row->label) ? $row->label : '',
      'language' => $source,
      'translations' => $target_icons_str,
      'configuration' => $configuration,
      'document_id' => $row->document_id ? : t('N/A'),
      'content_type' => isset($entity_properties['info']['bundles'][$row->type]['label']) ? $entity_properties['info']['bundles'][$row->type]['label'] : $row->type,
      'last_uploaded' => $row->last_uploaded ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->last_uploaded))) : t('Never'),
      'workflow_id' => $workflow_id,
      'workflow_name' => $workflow_name,
      'actions' => '<span class="lingotek-node-actions">' . $actions . '</span>',
      'description' => isset($row->description) ? $row->description : '',
    );

    if (isset($entity_properties['changed'])) {
      $data['changed'] = $row->changed ? t('@time ago', array('@time' => lingotek_human_readable_timestamp($row->changed))) : t('Never');
    }
    $table_data[$entity_id] = $data;
  }
  $entity_type = $entity_properties['type'];
  if(!empty($node_based_translations)){
    if($entity_type === 'node') {
      $query = db_select('node', 'n');
      $query->addField('n', 'title');
      $query->addField('n', 'tnid');
      $query->addField('n', 'nid');
      $query->condition('n.tnid', $node_based_translations, 'IN');
      $query->condition('n.language', $language->language);
      $results = $query->execute();
      foreach($results as $result) {
         if(isset($table_data[$result->tnid])){
           $table_data[$result->tnid]['title'] = l(t($result->title), $entity_properties['type'] . "/$result->nid");
         }
      }
    }
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

function lingotek_build_locales_statuses($row, $t_prefix = TRUE) {
  $list_statuses = array('none', 'pending', 'ready', 'current', 'ready_interim', 'interim', 'edited', 'untracked', 'error');
  $locales_statuses = array();
  foreach ($list_statuses as $status) {
    $key = $t_prefix ? 't_' . $status : $status;
    foreach (explode(',', $row->$key) as $locale) {
      if (!empty($locale)) {
        $locales_statuses[$locale] = $status;
      }
    }
  }

  if (count($locales_statuses) == 0) {
    $source_language = $row->language;
    $languages = language_list();
    foreach ($languages as $language => $value) {
      if ($language != $source_language) {
        $lingotek_locale = Lingotek::convertDrupal2Lingotek($language);
        $locales_statuses[$lingotek_locale] = LingotekSync::STATUS_NONE;
      }
    }
  }
  return $locales_statuses;
}

function lingotek_render_title($row, $entity_properties) {
  $language = $GLOBALS['language'];
  $info = $entity_properties['info'];
  $entity_type = $entity_properties['type'];
  $no_localized_title = (language_default()->language != $language->language) && (!isset($row->localized_title) or $row->localized_title == '');

  // special handling of taxonomy terms for URI generation
  if ($entity_type == 'taxonomy_term') {
    $term_obj = lingotek_entity_load_single($entity_type, $row->tid);
    $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $term_obj) : '';
  }
  else {
    if ($entity_type == 'bean') {
      $row = lingotek_entity_load_single($entity_type, $row->bid);
    }
    elseif ($entity_type == 'group') {
      $row = lingotek_entity_load_single($entity_type, $row->gid);
    }
    elseif ($entity_type == 'paragraphs_item') {
      $row = lingotek_entity_load_single($entity_type, $row->item_id);
    }
    elseif ($entity_type == 'file') {
      $row = lingotek_entity_load_single($entity_type, $row->fid);
    }
    $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $row) : '';
  }

  $uri['options']['attributes'] = array('target' => '_blank');
  if ($entity_type == 'paragraphs_item') {
    $title_to_show = lingotek_clean_paragraphs_title($row->field_name);
  }
  elseif ($entity_type == 'file') {
    $title_to_show = $row->filename;
  }
  else {
    $title_to_show = (isset($row->localized_title) ? $row->localized_title : $row->title);
  }
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

function lingotek_clean_paragraphs_title($field_name) {
  $field_name = ucfirst($field_name);
  $field_name = str_replace('_', ' ', $field_name);

  return $field_name;
}

function lingotek_render_source_current_icon_link($row, $entity_properties, $profile) {
  $language = $GLOBALS['language'];
  $info = $entity_properties['info'];
  $language_col = $entity_properties['language_col'];
  $entity_type = $entity_properties['type'];
  $no_localized_title = (language_default()->language != $language->language) && (!isset($row->localized_title) or $row->localized_title == '');

  $icon_class = $profile === LingotekSync::PROFILE_DISABLED ? 'ltk-source-icon source-disabled' : 'ltk-source-icon source-current';

  $tip = $profile === LingotekSync::PROFILE_DISABLED ? 'Disabled, cannot request translation' : 'Source Uploaded';

  // special handling of taxonomy terms for URI generation
  if ($entity_type == 'taxonomy_term') {
    $term_obj = lingotek_entity_load_single($entity_type, $row->tid);
    $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $term_obj) : '';
  }
  else {
    if ($entity_type == 'bean') {
      $row = lingotek_entity_load_single($entity_type, $row->bid);
    }
    elseif ($entity_type == 'group') {
      $row = lingotek_entity_load_single($entity_type, $row->gid);
    }
    elseif ($entity_type == 'paragraphs_item') {
      // Paragraphs url callback doesn't work currently, so we build the link here
      $paragraphs_item = lingotek_entity_load_single($entity_type, $row->item_id);
      $bundle = $paragraphs_item->bundle;
      $real_path = $entity_properties['info']['bundles'][$bundle]['admin']['real path'];
      $edit_link = $real_path . '/edit';
      $paragraphs_language = ($language_col ? $row->language : '');
      $paragraphs_icon = l($paragraphs_language, $edit_link, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => $icon_class)));

      return $paragraphs_icon;
    }
    elseif ($entity_type == 'file') {
      $row = lingotek_entity_load_single($entity_type, $row->fid);
    }
    $uri = isset($info['uri callback']) ? call_user_func($info['uri callback'], $row) : '';
  }

  $uri['options']['attributes'] = array(
      'target' => '_blank',
      'class' => $icon_class,
      'title' => $tip
    );
  $title = '';
  $title_to_show = (isset($row->localized_title) ? $row->localized_title : $row->title);
  $source_str = ($language_col ? $row->language : '');

  if (isset($uri['path'])) {
    $title .= l($source_str, $uri['path'], $uri['options']);
  }
  else {
    $title .= $title_to_show;
  }

  return $title;
}

function lingotek_render_source($entity_type, $row, $languages, $language_col, $entity_properties = array()) {
  $profile = $row->lingotek['name'];
  if ($entity_type == 'node') {
    $id = $row->nid;
  }
  elseif ($entity_type == 'comment') {
    $id = $row->cid;
  }
  elseif ($entity_type == 'config') {
    $id = $row->lid;
  }
  elseif ($entity_type == 'fieldable_panels_pane'){
    $id = $row->fpid;
  }
  elseif ($entity_type == 'taxonomy_term') {
    $id = $row->tid;
    $row->language = isset($row->translation_mode) && $row->translation_mode == '1' ? 'en' : $row->language;
  }
  elseif ($entity_type == 'bean') {
    $id = $row->bid;
    $row->language = language_default()->language;
  }
  elseif ($entity_type == 'group') {
    $id = $row->gid;
    $language = lingotek_get_group_source($row->gid);
    $row->language = $language;
  }
  elseif ($entity_type == 'menu_link') {
    $id = $row->mlid;
  }
  elseif ($entity_type == 'paragraphs_item') {
    $id = $row->item_id;
  }
  elseif ($entity_type == 'file') {
    $id = $row->fid;
    $row->language = lingotek_get_file_source($row->fid);
  }
  elseif (!empty($row->id)) {
    $id = $row->id;
  }

  $source_str = ($language_col ? $row->language : '');

  if ($row->upload_status === LingotekSync::STATUS_NONE) {
    $source_html = '<a href="#" onclick="lingotek_perform_action(' . $id . ',\'upload\')" class="ltk-source-icon source-none" title="Upload">' . $source_str . '</a>';
  }
  elseif ($row->upload_status === LingotekSync::STATUS_EDITED) {
    $source_html = '<a href="#" onclick="lingotek_perform_action(' . $id . ',\'upload\')" class="ltk-source-icon source-edited" title="Re-upload (content has changed since last upload)">' . $source_str . '</a>';
  }
  elseif ($row->upload_status === LingotekSync::STATUS_CURRENT) {
    if ($entity_type === 'config') {
      $source_html = '<span class="ltk-source-icon-config-current" title="Source Uploaded">' . $source_str . '</span>';
    }
    else {
      $source_html = lingotek_render_source_current_icon_link($row, $entity_properties, $profile);
    }
  }
  elseif ($row->upload_status === LingotekSync::STATUS_ERROR) {
    if ($entity_type === 'config') {
      $error_message = (isset($row->upload_error)) ? $row->upload_error : '';
    }
    else {
      $error_message = (isset($row->last_sync_error)) ? $row->last_sync_error : '';
    }
    $source_html = '<a href="#" onclick="lingotek_perform_action(' . $id . ',\'upload\')" class="ltk-source-icon source-error" title="Error: ' . $error_message . '">' . $source_str . '</a>';
  }
  else {
    $source_html = '<a href="#" onclick="lingotek_perform_action(' . $id . ',\'upload\')" class="ltk-source-icon source-none" title="Upload">' . $source_str . '</a>';
  }

  // Config items aren't added to the lingotek_config_metadata table until uploaded.
  if ($entity_type === 'config' && $row->upload_status === NULL) {
    $source_html = '<a href="#" onclick="lingotek_perform_action(' . $id . ',\'upload\')" class="ltk-source-icon source-none" title="Upload">' . $source_str . '</a>';
  }

  if ($profile == LingotekSync::PROFILE_DISABLED && $entity_type != 'config') {
      $source_html = '<span class="ltk-target-disabled" title="Disabled, cannot request translation">' . $source_str . '</span>';
  }

  return $source_html;
}

function lingotek_render_marked_html($row, $entity_type, $entity_id) {
  global $base_url;
  $marked_value = (isset($row->marked) && $row->marked) ? 1 : 0;
  $title = $marked_value == 1 ? 'Unmark content' : 'Mark content';
  $marked_icon = $marked_value ? "fa-check-square" : "fa-square-o";
  $marked_html = '<span href="' . $base_url . '/lingotek/update-marked-value/' . $entity_id . '/' . $entity_type . '/' . $marked_value . '" class="ltk-marked-checkbox fa ' . $marked_icon . '" id="marked-' . $entity_type . '-' . $entity_id . '" title="' . $title . '"></span>';

  return $marked_html;
}

function lingotek_render_configuration($row, $profiles, $disabled_class) {
  $configuration = '';

  if (!empty($row->lingotek['auto_upload'])) {
    $configuration .= '<i class="fa fa-arrow-up' . $disabled_class . '" title="' . t('Automatic Upload') . '"></i> ';
  }
  if (!empty($row->lingotek['auto_download'])) {
    $configuration .= '<i class="fa fa-arrow-down' . $disabled_class . '" title="' . t('Automatic Download') . '"></i> ';
  }
  if (!empty($row->lingotek['allow_community_translation'])) {
    $configuration .= '<i class="fa fa-globe' . $disabled_class . '" title="' . t('Crowdsourcing Enabled') . '"></i> ';
  }

  if ($row->lingotek['profile'] == LingotekSync::PROFILE_DISABLED) {
    $configuration = t('Disabled');
  }
  else {
    $profile_key = $row->lingotek['profile'];
    if (array_key_exists($profile_key, $profiles)) {
      $configuration = t($profiles[$profile_key]->getName());
    }
    else {
      $configuration = t('Unknown');// profile id does not exist in profiles (this state should be unobtainable)
    }
  }

  return $configuration;
}

function lingotek_render_actions($entity_properties, $entity_id, $disabled_class) {
  $entity_type = $entity_properties['type'];
  $pm_icon = ($entity_type == 'node') ? ' <i class="fa fa-tasks' . $disabled_class . '"></i>' : '';
  $pm_link = ' ' . l($pm_icon, 'node/' . $entity_id . '/lingotek_pm', array('html' => TRUE, 'attributes' => array('title' => t('View Translations'), 'target' => '_blank')));

  $actions = '';
  $actions .= lingotek_get_entity_edit_link($entity_properties, $entity_id);
  $actions .= empty($disabled_class) ? $pm_link : $pm_icon;
  $actions .= ' ' . l('<i title="' . t('Set Lingotek Settings') . '" class="fa fa-gear"></i>', '', array('html' => TRUE, 'attributes' => array('onclick' => 'lingotek_perform_action(' . $entity_id . ',"edit"); return false;')));

  return $actions;
}

function lingotek_get_entity_edit_link($entity_properties, $entity_id, $url_only = FALSE) {
  $entity_type = $entity_properties['type'];
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
    case 'menu_link':
      $edit_link = 'admin/structure/menu/item/' . $entity_id;
      break;
    case 'paragraphs_item';
      $paragraphs_item = lingotek_entity_load_single($entity_type, $entity_id);
      $bundle = $paragraphs_item->bundle;
      $real_path = $entity_properties['info']['bundles'][$bundle]['admin']['real path'];
      $edit_link = $real_path . '/edit';
      break;
    default:
      $edit_link = $entity_type . '/' . $entity_id . '/edit';
      break;
  }
  return $url_only ? url($edit_link, array('query' => $query)) : l('<i title="' . t('Edit') . '" class="fa fa-edit"></i>', $edit_link, array('html' => TRUE, 'attributes' => array('target' => '_blank'), 'query' => $query));
}

function lingotek_lang_icons($entity_type, $languages, $entity_id, $locale_statuses = array(), $disabled = FALSE, $source_language = NULL, $allow_source_overwriting = FALSE, $allow_target_localization = FALSE, $non_lingotek_config_translations = array()) {

  if ($entity_type == 'group') {
    $source_language = lingotek_get_group_source($entity_id);
  }
  if ($entity_type == 'paragraphs_item') {
    $source_language = lingotek_get_paragraphs_item_source($entity_id);
  }

  $icons = array();
  $legend = array(
    LingotekSync::STATUS_NONE => t('No Translation'),
    LingotekSync::STATUS_PENDING => t('In-Progress'),
    LingotekSync::STATUS_READY => t('Ready for Download'),
    LingotekSync::STATUS_CURRENT => t('Current'),
    LingotekSync::STATUS_READY_INTERIM => t('Ready for Interim Download'),
    LingotekSync::STATUS_INTERIM => t('In-progress (interim translation downloaded)'),
    LingotekSync::STATUS_EDITED => t('Source Edited'),
    LingotekSync::STATUS_UNTRACKED => t('Translation exists, but it is not being tracked by Lingotek'),
    LingotekSync::STATUS_TARGET_LOCALIZE => t('Pending localization'),
    LingotekSync::STATUS_TARGET_EDITED => t('Needs to be uploaded'),
    LingotekSync::STATUS_NON_LINGOTEK => t('Non-Lingotek Translation'),
    LingotekSync::STATUS_ERROR => t('Error'),
  );
  $default_link = 'lingotek/workbench/' . $entity_type . '/' . $entity_id . '/';
  $manual_link = $entity_type . '/' . $entity_id . '/lingotek_pm/';
  $untracked_link = 'lingotek/view/' . $entity_type . '/' . $entity_id . '/';
  $untracked_config_link = 'admin/' . $entity_type . '/regional/translate/edit/' . $entity_id . '/';

  $link_map = array(
    LingotekSync::STATUS_PENDING => $default_link,
    LingotekSync::STATUS_READY => $default_link,
    LingotekSync::STATUS_CURRENT => $default_link,
    LingotekSync::STATUS_READY_INTERIM => $default_link,
    LingotekSync::STATUS_INTERIM => $default_link,
    LingotekSync::STATUS_EDITED => $default_link,
    LingotekSync::STATUS_UNTRACKED => $untracked_link,
    LingotekSync::STATUS_TARGET_LOCALIZE => $untracked_link,
    LingotekSync::STATUS_TARGET_EDITED => $untracked_link,
    LingotekSync::STATUS_NON_LINGOTEK => $untracked_config_link,
    LingotekSync::STATUS_ERROR => $default_link,
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
        // styling for the icons based on status and whether the translation is owned by Lingotek or not
        if ($entity_type == 'config' && (array_key_exists($entity_id, $non_lingotek_config_translations) && in_array($lang_code, explode(',', $non_lingotek_config_translations[$entity_id]->language_codes)))) {
          $css_classes = 'target-untracked';
          $status = LingotekSync::STATUS_UNTRACKED;
          $tip = $legend[$status];
        }
        else { //styling for all icons except Config/NON_LINGOTEK items
          $css_classes = 'target-' . strtolower($status);
          $tip = $legend[$status];
        }
        // link redirects to the translate interface for the config string rather than the Lingotek workbench if is an untracked config item
        if ($entity_type == 'config' && $status == LingotekSync::STATUS_UNTRACKED) {
          $current_icon = l($lang_code, $untracked_config_link . $lingotek_locale, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => array('language-icon', $css_classes))));
        }
        elseif ($status == LingotekSync::STATUS_NONE) {
          $none_css_classes = 'ltk-target-none';
          $none_tip = 'No Translation';
          $current_icon = '<span class = "' . $none_css_classes . '" title="' . $none_tip . '">' . $lang_code . '</span>';
        }
        else {
          $current_icon = l($lang_code, $link_map[$status] . $lingotek_locale, array('attributes' => array('target' => '_blank', 'title' => $tip, 'class' => array('language-icon', $css_classes))));
        }

        if ($disabled) {
          $current_icon = '<span class="ltk-target-disabled" title="Disabled, cannot request translation">' . $lang_code . '</span>';
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
function lingotek_change_workflow($entity_type, $entity_ids) {
  $entity_ids = explode(',', $entity_ids);
  if($entity_type === 'config'){
    foreach($entity_ids as $entity_id) {
      LingotekConfigSet::getSetId($entity_id);
    }
    $entity_ids = LingotekSync::getSetIdsFromLids($entity_ids);
  }
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'entity_type' => $entity_type,
    'nids' => $entity_ids,
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
    '#title' => $entity_type == 'config' ? t('Are you sure you want to delete these translations?') : t('Are you sure you want to delete these entities?'),
    '#description' => $entity_type == 'config' ? '<br/>'.filter_xss($content_details_string) : filter_xss($content_details_string),
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
 * Form constructor for the translations delete.
 */
function lingotek_entity_delete_translations_form($form, &$form_state, $nids = array(), $collapse_fieldset = TRUE) {
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
    '#title' => t('Are you sure you want to delete these translations?'),
    '#description' => '<br/>'.filter_xss($content_details_string),
    '#suffix' => '<i>' . t("Note: this action cannot be undone.") . '</i>',
    '#collapsible' => $collapse_fieldset,
    '#collapsed' => $collapse_fieldset,
  );

  $submit_function = 'lingotek_entity_delete_translations_form_submit';

  $form['confirm']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete Translations'),
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
    if ($entity_type == 'menu_link') {
      lingotek_delete_menu_links($entity_ids);
    }
    else {
      entity_delete_multiple($entity_type, $entity_ids);
    }
    $count = count($entity_ids);
    watchdog('content', format_plural($count, 'Deleted 1 entity.', 'Deleted @count entities.'));
    drupal_set_message(format_plural($count, 'Deleted 1 entity.', 'Deleted @count entities.'));
    $form_state['executed'] = TRUE;
  }
  $form_state['redirect'] = LINGOTEK_MENU_MAIN_BASE_URL . '/manage';
  return lingotek_grid_filter_submit($form, $form_state);
}

/**
 * Submit handler for the lingotek_entity_delete_translations form.
 */
function lingotek_entity_delete_translations_form_submit($form, &$form_state) {
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

  if ($entity_type == 'menu_link') {
    lingotek_delete_menu_link_translations($entity_ids);
  }
  else {
    // Deletes translations based on whether it is node based translation or field based translation
    if (!empty($entity_ids)) {
      $entities = entity_load($entity_type, $entity_ids);
      $count = 0;
      foreach ($entities as $entity) {
        $entity->lingotek_upload_override = FALSE;
        if ($entity_type == 'node' && lingotek_uses_node_translation($entity)) {
          $count += 1;
          lingotek_delete_node_translations($entity->nid);
        }
        if ($entity->tnid == 0) {
          $count += 1;
          lingotek_delete_field_translations($entity_type, $entity);
        }
      }
    }
  }

  watchdog('content', format_plural($count, 'Deleted translations for 1 entity.', 'Deleted translations for @count entities.'));
  drupal_set_message(format_plural($count, 'Deleted translations for 1 entity.', 'Deleted translations for @count entities.'));
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

  if (variable_get('lingotek_disassociate_delete_tms', FALSE)) {
    $form['disassociate_translations']['confirm'] = array(
      '#type' => 'item',
      '#markup' => t('Documents(s) will also be deleted from Lingotek TMS.'),
    );
  }
  else {
    $form['disassociate_translations']['confirm'] = array(
      '#type' => 'checkbox',
      '#title' => t('Also remove all document(s) from Lingotek TMS'),
      '#default_value' => FALSE,
    );
  }
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
  $global_remove_from_tms = variable_get('lingotek_disassociate_delete_tms', FALSE);
  if ($remove_from_tms || $global_remove_from_tms) { //disassociate on TMS
    $result = lingotek_batch_disassociate_content_worker($api, $doc_ids);
    if (!$result) {
      drupal_set_message(t('Failed to remove documents from Lingotek TMS.'), 'warning', FALSE);
      return;
    }
  }
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'document_id');
  LingotekSync::setAllUploadStatuses($entity_type, $entity_ids, LingotekSync::STATUS_NONE);
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'original_language');
  lingotek_keystore_delete_multiple($entity_type, $entity_ids, 'source_language_%', 'LIKE');
  foreach ($entity_ids as $entity_id) {
    // find which translations exist on Drupal for the entity
    $existing_translations = lingotek_get_languages($entity_type, $entity_id);
    // find which targets have a status within Lingotek
    $target_locales = LingotekSync::getAllTargetStatusForEntity($entity_type, $entity_id);
    // for each existing translation, set the target status to untracked
    foreach ($existing_translations as $language) {
      $locale = Lingotek::convertDrupal2Lingotek($language);
      if (isset($target_locales[$locale])) {
        LingotekSync::setTargetStatus($entity_type, $entity_id, $locale, LingotekSync::STATUS_UNTRACKED);
        unset($target_locales[$locale]);
      }
    }
    // for each non-existent translation, remove the target status
    foreach (array_keys($target_locales) as $locale) {
      LingotekSync::deleteTargetStatus($entity_type, $entity_id, $locale);
    }
  }

  $disassociated_entities = '(Entity ID\'s: ';

  for ($i = 0; $i < count($entity_ids); $i++) {
    if ($i == count($entity_ids) - 1) {
      $disassociated_entities .= $entity_ids[$i] . ')';
    }
    else {
      $disassociated_entities .= $entity_ids[$i] . ', ';
    }
  }

  drupal_set_message(format_plural(count($entity_ids), 'Translations disassociated for one entity. ' . $disassociated_entities, 'Translations disassociated for @count entities. ' . $disassociated_entities));
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
  $global_remove_from_tms = variable_get('lingotek_disassociate_delete_tms', FALSE);
  $set_ids = LingotekSync::getSetIdsFromLids($lids);
  $doc_ids = LingotekSync::getConfigDocIdsFromSetIds($set_ids);

  // Update the translation_agent_id in the locales_target table
  LingotekSync::updateLingotekTranslationAgentId($lids, LingotekSync::TRANSLATION_AGENT_ID_DRUPAL);

  if (($remove_from_tms || $global_remove_from_tms) && !empty($doc_ids)) { //disassociate on TMS
    $result = lingotek_batch_disassociate_content_worker($api, $doc_ids);
    //Deletes document data from lingotek_config_metadata table and lingotek_config_metadata table
    LingotekConfigSet::deleteConfigSetMetadataBySetId($set_ids);
    LingotekConfigSet::deleteConfigSetMapDataBySetId($set_ids);

    if (!$result) {
      drupal_set_message(t('Failed to remove documents from Lingotek TMS.'), 'warning', FALSE);
      return;
    }
  }

  if (!empty($lids)) {
    LingotekConfigSet::disassociateSegments($lids);
  }

  LingotekConfigSet::removeEmptyConfigSets($set_ids);
  $disassociated_config = '(Config ID\'s: ';

  for ($i = 0; $i < count($lids); $i++) {
    if ($i == count($lids) - 1) {
      $disassociated_config .= $lids[$i] . ')';
    }
    else {
      $disassociated_config .= $lids[$i] . ', ';
    }
  }

  drupal_set_message(format_plural(count($lids), 'Translations disassociated for one config item. ' . $disassociated_config, 'Translations disassociated for @count config items. ' . $disassociated_config));

}
