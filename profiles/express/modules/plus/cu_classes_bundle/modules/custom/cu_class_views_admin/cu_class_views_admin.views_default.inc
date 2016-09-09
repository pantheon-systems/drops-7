<?php
/**
 * @file
 * cu_class_views_admin.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function cu_class_views_admin_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'cu_classes_admin';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'cu_class';
  $view->human_name = 'CU Class Admin';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Classes';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'class_id' => 'class_id',
    'field_class_class_section' => 'field_class_class_section',
    'field_class_catalog_nbr' => 'field_class_catalog_nbr',
    'field_class_subject' => 'field_class_subject',
    'title' => 'title',
    'field_class_ssr_instr_long' => 'field_class_ssr_instr_long',
    'field_class_last_updated' => 'field_class_last_updated',
    'field_class_instruction_mode' => 'field_class_instruction_mode',
    'status' => 'status',
    'course_id' => 'course_id',
  );
  $handler->display->display_options['style_options']['default'] = 'class_id';
  $handler->display->display_options['style_options']['info'] = array(
    'class_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_class_class_section' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_class_catalog_nbr' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_class_subject' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_class_ssr_instr_long' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_class_last_updated' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_class_instruction_mode' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'course_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Result summary */
  $handler->display->display_options['header']['result']['id'] = 'result';
  $handler->display->display_options['header']['result']['table'] = 'views';
  $handler->display->display_options['header']['result']['field'] = 'result';
  $handler->display->display_options['header']['result']['content'] = '<div class="results-counter">Your search has returned @total classes.</div>';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no classes that match your search.';
  $handler->display->display_options['empty']['area']['format'] = 'wysiwyg';
  /* Field: Class: Class ID */
  $handler->display->display_options['fields']['class_id']['id'] = 'class_id';
  $handler->display->display_options['fields']['class_id']['table'] = 'cu_class';
  $handler->display->display_options['fields']['class_id']['field'] = 'class_id';
  $handler->display->display_options['fields']['class_id']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['class_id']['alter']['path'] = 'class/[class_id]';
  $handler->display->display_options['fields']['class_id']['separator'] = '';
  /* Field: Class: CLASS_SECTION */
  $handler->display->display_options['fields']['field_class_class_section']['id'] = 'field_class_class_section';
  $handler->display->display_options['fields']['field_class_class_section']['table'] = 'field_data_field_class_class_section';
  $handler->display->display_options['fields']['field_class_class_section']['field'] = 'field_class_class_section';
  $handler->display->display_options['fields']['field_class_class_section']['label'] = '';
  $handler->display->display_options['fields']['field_class_class_section']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_class_class_section']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_section']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_class_section']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_section']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_class_section']['field_api_classes'] = TRUE;
  /* Field: Class: CATALOG_NBR */
  $handler->display->display_options['fields']['field_class_catalog_nbr']['id'] = 'field_class_catalog_nbr';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['table'] = 'field_data_field_class_catalog_nbr';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['field'] = 'field_class_catalog_nbr';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['label'] = '';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_catalog_nbr']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 1,
  );
  $handler->display->display_options['fields']['field_class_catalog_nbr']['field_api_classes'] = TRUE;
  /* Field: Class: SUBJECT */
  $handler->display->display_options['fields']['field_class_subject']['id'] = 'field_class_subject';
  $handler->display->display_options['fields']['field_class_subject']['table'] = 'field_data_field_class_subject';
  $handler->display->display_options['fields']['field_class_subject']['field'] = 'field_class_subject';
  $handler->display->display_options['fields']['field_class_subject']['label'] = 'Catalog Number';
  $handler->display->display_options['fields']['field_class_subject']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_class_subject']['alter']['text'] = '[field_class_subject] [field_class_catalog_nbr] - [field_class_class_section]';
  $handler->display->display_options['fields']['field_class_subject']['alter']['trim_whitespace'] = TRUE;
  $handler->display->display_options['fields']['field_class_subject']['alter']['strip_tags'] = TRUE;
  $handler->display->display_options['fields']['field_class_subject']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_subject']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_subject']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_subject']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_subject']['field_api_classes'] = TRUE;
  /* Field: Class: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'cu_class';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Field: Class: SSR_INSTR_LONG */
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['id'] = 'field_class_ssr_instr_long';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['table'] = 'field_data_field_class_ssr_instr_long';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['field'] = 'field_class_ssr_instr_long';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['label'] = 'Instructor';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['field_api_classes'] = TRUE;
  /* Field: Class: Last Updated */
  $handler->display->display_options['fields']['field_class_last_updated']['id'] = 'field_class_last_updated';
  $handler->display->display_options['fields']['field_class_last_updated']['table'] = 'field_data_field_class_last_updated';
  $handler->display->display_options['fields']['field_class_last_updated']['field'] = 'field_class_last_updated';
  $handler->display->display_options['fields']['field_class_last_updated']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_last_updated']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_last_updated']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_last_updated']['settings'] = array(
    'format_type' => 'cu_short_date_time',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
    'show_remaining_days' => 0,
  );
  $handler->display->display_options['fields']['field_class_last_updated']['field_api_classes'] = TRUE;
  /* Field: Class: INSTRUCTION_MODE */
  $handler->display->display_options['fields']['field_class_instruction_mode']['id'] = 'field_class_instruction_mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['table'] = 'field_data_field_class_instruction_mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['field'] = 'field_class_instruction_mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['label'] = 'Mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_instruction_mode']['field_api_classes'] = TRUE;
  /* Field: Class: Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'cu_class';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['separator'] = '';
  /* Field: Class: Course_id */
  $handler->display->display_options['fields']['course_id']['id'] = 'course_id';
  $handler->display->display_options['fields']['course_id']['table'] = 'cu_class';
  $handler->display->display_options['fields']['course_id']['field'] = 'course_id';
  $handler->display->display_options['fields']['course_id']['label'] = 'Course ID';
  $handler->display->display_options['fields']['course_id']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['course_id']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['course_id']['alter']['path'] = 'course/[course_id]';
  $handler->display->display_options['fields']['course_id']['separator'] = '';
  /* Filter criterion: Class: SUBJECT (field_class_subject) */
  $handler->display->display_options['filters']['field_class_subject_value']['id'] = 'field_class_subject_value';
  $handler->display->display_options['filters']['field_class_subject_value']['table'] = 'field_data_field_class_subject';
  $handler->display->display_options['filters']['field_class_subject_value']['field'] = 'field_class_subject_value';
  $handler->display->display_options['filters']['field_class_subject_value']['group'] = 1;
  $handler->display->display_options['filters']['field_class_subject_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['operator_id'] = 'field_class_subject_value_op';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['label'] = 'Subject';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['operator'] = 'field_class_subject_value_op';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['identifier'] = 'field_class_subject_value';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
    8 => 0,
    9 => 0,
  );
  /* Filter criterion: Class: CATALOG_NBR (field_class_catalog_nbr) */
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['id'] = 'field_class_catalog_nbr_value';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['table'] = 'field_data_field_class_catalog_nbr';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['field'] = 'field_class_catalog_nbr_value';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['group'] = 1;
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['expose']['operator_id'] = 'field_class_catalog_nbr_value_op';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['expose']['label'] = 'Catalog Number';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['expose']['operator'] = 'field_class_catalog_nbr_value_op';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['expose']['identifier'] = 'field_class_catalog_nbr_value';
  $handler->display->display_options['filters']['field_class_catalog_nbr_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
    8 => 0,
    9 => 0,
  );
  /* Filter criterion: Class: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'cu_class';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
    8 => 0,
    9 => 0,
  );
  /* Filter criterion: Class: CLASS_NBR (field_class_class_nbr) */
  $handler->display->display_options['filters']['field_class_class_nbr_value']['id'] = 'field_class_class_nbr_value';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['table'] = 'field_data_field_class_class_nbr';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['field'] = 'field_class_class_nbr_value';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['group'] = 1;
  $handler->display->display_options['filters']['field_class_class_nbr_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_class_nbr_value']['expose']['operator_id'] = 'field_class_class_nbr_value_op';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['expose']['label'] = 'Class ID';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['expose']['operator'] = 'field_class_class_nbr_value_op';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['expose']['identifier'] = 'field_class_class_nbr_value';
  $handler->display->display_options['filters']['field_class_class_nbr_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
    8 => 0,
    9 => 0,
  );
  /* Filter criterion: Class: CRSE_ID (field_class_crse_id) */
  $handler->display->display_options['filters']['field_class_crse_id_value']['id'] = 'field_class_crse_id_value';
  $handler->display->display_options['filters']['field_class_crse_id_value']['table'] = 'field_data_field_class_crse_id';
  $handler->display->display_options['filters']['field_class_crse_id_value']['field'] = 'field_class_crse_id_value';
  $handler->display->display_options['filters']['field_class_crse_id_value']['group'] = 1;
  $handler->display->display_options['filters']['field_class_crse_id_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_crse_id_value']['expose']['operator_id'] = 'field_class_crse_id_value_op';
  $handler->display->display_options['filters']['field_class_crse_id_value']['expose']['label'] = 'Course ID';
  $handler->display->display_options['filters']['field_class_crse_id_value']['expose']['operator'] = 'field_class_crse_id_value_op';
  $handler->display->display_options['filters']['field_class_crse_id_value']['expose']['identifier'] = 'field_class_crse_id_value';
  $handler->display->display_options['filters']['field_class_crse_id_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
    8 => 0,
    9 => 0,
  );
  /* Filter criterion: Class: Status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'cu_class';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Published';
  $handler->display->display_options['filters']['status']['expose']['description'] = '1 = published';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
    8 => 0,
    9 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['display_description'] = 'Admin page for classes';
  $handler->display->display_options['path'] = 'admin/content/classes';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Classes';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $export['cu_classes_admin'] = $view;

  return $export;
}
