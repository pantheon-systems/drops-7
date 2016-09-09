<?php
/**
 * @file
 * cu_class_views.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function cu_class_views_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'cu_classes';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'cu_class';
  $view->human_name = 'Classes';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Course Search';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
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
  $handler->display->display_options['empty']['area']['content'] = 'There are no courses that match your search.';
  $handler->display->display_options['empty']['area']['format'] = 'wysiwyg';
  /* Field: Class: Class ID */
  $handler->display->display_options['fields']['class_id']['id'] = 'class_id';
  $handler->display->display_options['fields']['class_id']['table'] = 'cu_class';
  $handler->display->display_options['fields']['class_id']['field'] = 'class_id';
  /* Field: Class: CLASS_NBR */
  $handler->display->display_options['fields']['field_class_class_nbr']['id'] = 'field_class_class_nbr';
  $handler->display->display_options['fields']['field_class_class_nbr']['table'] = 'field_data_field_class_class_nbr';
  $handler->display->display_options['fields']['field_class_class_nbr']['field'] = 'field_class_class_nbr';
  $handler->display->display_options['fields']['field_class_class_nbr']['label'] = '';
  $handler->display->display_options['fields']['field_class_class_nbr']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_nbr']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_class_nbr']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_nbr']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_class_nbr']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 1,
  );
  /* Field: Class: CLASS_SECTION */
  $handler->display->display_options['fields']['field_class_class_section']['id'] = 'field_class_class_section';
  $handler->display->display_options['fields']['field_class_class_section']['table'] = 'field_data_field_class_class_section';
  $handler->display->display_options['fields']['field_class_class_section']['field'] = 'field_class_class_section';
  $handler->display->display_options['fields']['field_class_class_section']['label'] = '';
  $handler->display->display_options['fields']['field_class_class_section']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_section']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_section']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_class_section']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_class_section']['element_default_classes'] = FALSE;
  /* Field: Class: COURSE_TITLE_LONG */
  $handler->display->display_options['fields']['field_class_course_title_long']['id'] = 'field_class_course_title_long';
  $handler->display->display_options['fields']['field_class_course_title_long']['table'] = 'field_data_field_class_course_title_long';
  $handler->display->display_options['fields']['field_class_course_title_long']['field'] = 'field_class_course_title_long';
  $handler->display->display_options['fields']['field_class_course_title_long']['label'] = '';
  $handler->display->display_options['fields']['field_class_course_title_long']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_course_title_long']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_class_course_title_long']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_course_title_long']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_course_title_long']['element_default_classes'] = FALSE;
  /* Field: Class: INSTRUCTION_MODE */
  $handler->display->display_options['fields']['field_class_instruction_mode']['id'] = 'field_class_instruction_mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['table'] = 'field_data_field_class_instruction_mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['field'] = 'field_class_instruction_mode';
  $handler->display->display_options['fields']['field_class_instruction_mode']['label'] = '';
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_instruction_mode']['element_default_classes'] = FALSE;
  /* Field: Class: SSR_INSTR_LONG */
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['id'] = 'field_class_ssr_instr_long';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['table'] = 'field_data_field_class_ssr_instr_long';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['field'] = 'field_class_ssr_instr_long';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['label'] = '';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_instr_long']['field_api_classes'] = TRUE;
  /* Field: Class: END_DT */
  $handler->display->display_options['fields']['field_class_end_dt']['id'] = 'field_class_end_dt';
  $handler->display->display_options['fields']['field_class_end_dt']['table'] = 'field_data_field_class_end_dt';
  $handler->display->display_options['fields']['field_class_end_dt']['field'] = 'field_class_end_dt';
  $handler->display->display_options['fields']['field_class_end_dt']['label'] = '';
  $handler->display->display_options['fields']['field_class_end_dt']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_end_dt']['element_label_type'] = 'div';
  $handler->display->display_options['fields']['field_class_end_dt']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_end_dt']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_end_dt']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_end_dt']['settings'] = array(
    'format_type' => 'cu_medium_date',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
    'show_remaining_days' => 0,
  );
  /* Field: Class: Last Updated */
  $handler->display->display_options['fields']['field_class_last_updated']['id'] = 'field_class_last_updated';
  $handler->display->display_options['fields']['field_class_last_updated']['table'] = 'field_data_field_class_last_updated';
  $handler->display->display_options['fields']['field_class_last_updated']['field'] = 'field_class_last_updated';
  $handler->display->display_options['fields']['field_class_last_updated']['label'] = '';
  $handler->display->display_options['fields']['field_class_last_updated']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_last_updated']['element_label_type'] = 'div';
  $handler->display->display_options['fields']['field_class_last_updated']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_last_updated']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_last_updated']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_last_updated']['settings'] = array(
    'format_type' => 'cu_short_date',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
    'show_remaining_days' => 0,
  );
  /* Field: Class: START_DT */
  $handler->display->display_options['fields']['field_class_start_dt']['id'] = 'field_class_start_dt';
  $handler->display->display_options['fields']['field_class_start_dt']['table'] = 'field_data_field_class_start_dt';
  $handler->display->display_options['fields']['field_class_start_dt']['field'] = 'field_class_start_dt';
  $handler->display->display_options['fields']['field_class_start_dt']['label'] = '';
  $handler->display->display_options['fields']['field_class_start_dt']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_start_dt']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_class_start_dt']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_start_dt']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_start_dt']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_start_dt']['settings'] = array(
    'format_type' => 'cu_medium_date',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
    'show_remaining_days' => 0,
  );
  /* Field: Class: ENRL_CAP */
  $handler->display->display_options['fields']['field_class_enrl_cap']['id'] = 'field_class_enrl_cap';
  $handler->display->display_options['fields']['field_class_enrl_cap']['table'] = 'field_data_field_class_enrl_cap';
  $handler->display->display_options['fields']['field_class_enrl_cap']['field'] = 'field_class_enrl_cap';
  $handler->display->display_options['fields']['field_class_enrl_cap']['label'] = '';
  $handler->display->display_options['fields']['field_class_enrl_cap']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_enrl_cap']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_class_enrl_cap']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_enrl_cap']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_enrl_cap']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_enrl_cap']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 1,
  );
  /* Field: Class: ENRL_TOT */
  $handler->display->display_options['fields']['field_class_enrl_tot']['id'] = 'field_class_enrl_tot';
  $handler->display->display_options['fields']['field_class_enrl_tot']['table'] = 'field_data_field_class_enrl_tot';
  $handler->display->display_options['fields']['field_class_enrl_tot']['field'] = 'field_class_enrl_tot';
  $handler->display->display_options['fields']['field_class_enrl_tot']['label'] = '';
  $handler->display->display_options['fields']['field_class_enrl_tot']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_enrl_tot']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_class_enrl_tot']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_enrl_tot']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_enrl_tot']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_enrl_tot']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 1,
  );
  /* Field: Class: SSR_DESCRLONG */
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['id'] = 'field_class_ssr_descrlong';
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['table'] = 'field_data_field_class_ssr_descrlong';
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['field'] = 'field_class_ssr_descrlong';
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['label'] = '';
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_descrlong']['field_api_classes'] = TRUE;
  /* Field: Class: CRSE_ID */
  $handler->display->display_options['fields']['field_class_crse_id']['id'] = 'field_class_crse_id';
  $handler->display->display_options['fields']['field_class_crse_id']['table'] = 'field_data_field_class_crse_id';
  $handler->display->display_options['fields']['field_class_crse_id']['field'] = 'field_class_crse_id';
  $handler->display->display_options['fields']['field_class_crse_id']['label'] = '';
  $handler->display->display_options['fields']['field_class_crse_id']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_crse_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_crse_id']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_crse_id']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_crse_id']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 1,
  );
  /* Field: Class: SUBJECT */
  $handler->display->display_options['fields']['field_class_subject']['id'] = 'field_class_subject';
  $handler->display->display_options['fields']['field_class_subject']['table'] = 'field_data_field_class_subject';
  $handler->display->display_options['fields']['field_class_subject']['field'] = 'field_class_subject';
  $handler->display->display_options['fields']['field_class_subject']['label'] = '';
  $handler->display->display_options['fields']['field_class_subject']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_subject']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_subject']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_subject']['element_default_classes'] = FALSE;
  /* Field: Class: SESSION_CODE */
  $handler->display->display_options['fields']['field_class_session_code']['id'] = 'field_class_session_code';
  $handler->display->display_options['fields']['field_class_session_code']['table'] = 'field_data_field_class_session_code';
  $handler->display->display_options['fields']['field_class_session_code']['field'] = 'field_class_session_code';
  $handler->display->display_options['fields']['field_class_session_code']['label'] = '';
  $handler->display->display_options['fields']['field_class_session_code']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_session_code']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_session_code']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_session_code']['element_default_classes'] = FALSE;
  /* Field: Class: UNITS_ACAD_PROG */
  $handler->display->display_options['fields']['field_class_units_acad_prog']['id'] = 'field_class_units_acad_prog';
  $handler->display->display_options['fields']['field_class_units_acad_prog']['table'] = 'field_data_field_class_units_acad_prog';
  $handler->display->display_options['fields']['field_class_units_acad_prog']['field'] = 'field_class_units_acad_prog';
  $handler->display->display_options['fields']['field_class_units_acad_prog']['label'] = '';
  $handler->display->display_options['fields']['field_class_units_acad_prog']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_units_acad_prog']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_units_acad_prog']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_units_acad_prog']['element_default_classes'] = FALSE;
  /* Field: Class: CATALOG_NBR */
  $handler->display->display_options['fields']['field_class_catalog_nbr']['id'] = 'field_class_catalog_nbr';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['table'] = 'field_data_field_class_catalog_nbr';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['field'] = 'field_class_catalog_nbr';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['label'] = '';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_catalog_nbr']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_catalog_nbr']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 0,
  );
  /* Field: Class: SSR_MTG_SCHED_LONG */
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['id'] = 'field_class_ssr_mtg_sched_long';
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['table'] = 'field_data_field_class_ssr_mtg_sched_long';
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['field'] = 'field_class_ssr_mtg_sched_long';
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['label'] = '';
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_ssr_mtg_sched_long']['element_default_classes'] = FALSE;
  /* Field: Class: CRS_TOPIC_DESCR */
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['id'] = 'field_class_crs_topic_descr';
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['table'] = 'field_data_field_class_crs_topic_descr';
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['field'] = 'field_class_crs_topic_descr';
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['label'] = '';
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_crs_topic_descr']['element_default_classes'] = FALSE;
  /* Field: Class: College/School */
  $handler->display->display_options['fields']['field_class_subject_top_level']['id'] = 'field_class_subject_top_level';
  $handler->display->display_options['fields']['field_class_subject_top_level']['table'] = 'field_data_field_class_subject_top_level';
  $handler->display->display_options['fields']['field_class_subject_top_level']['field'] = 'field_class_subject_top_level';
  $handler->display->display_options['fields']['field_class_subject_top_level']['label'] = '';
  $handler->display->display_options['fields']['field_class_subject_top_level']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_subject_top_level']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_subject_top_level']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_subject_top_level']['element_default_classes'] = FALSE;
  /* Field: Class: INSTRUCTION_MODE_LOVDescr */
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['id'] = 'field_class_instruction_mode_des';
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['table'] = 'field_data_field_class_instruction_mode_des';
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['field'] = 'field_class_instruction_mode_des';
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['label'] = '';
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_instruction_mode_des']['element_default_classes'] = FALSE;
  /* Field: Class: INSTITUTION */
  $handler->display->display_options['fields']['field_class_institution']['id'] = 'field_class_institution';
  $handler->display->display_options['fields']['field_class_institution']['table'] = 'field_data_field_class_institution';
  $handler->display->display_options['fields']['field_class_institution']['field'] = 'field_class_institution';
  $handler->display->display_options['fields']['field_class_institution']['label'] = '';
  $handler->display->display_options['fields']['field_class_institution']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_institution']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_institution']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_institution']['element_default_classes'] = FALSE;
  /* Field: Class: STRM */
  $handler->display->display_options['fields']['field_class_strm']['id'] = 'field_class_strm';
  $handler->display->display_options['fields']['field_class_strm']['table'] = 'field_data_field_class_strm';
  $handler->display->display_options['fields']['field_class_strm']['field'] = 'field_class_strm';
  $handler->display->display_options['fields']['field_class_strm']['label'] = '';
  $handler->display->display_options['fields']['field_class_strm']['element_type'] = '0';
  $handler->display->display_options['fields']['field_class_strm']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_strm']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_class_strm']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_class_strm']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 0,
  );
  /* Sort criterion: Class: SUBJECT (field_class_subject) */
  $handler->display->display_options['sorts']['field_class_subject_value']['id'] = 'field_class_subject_value';
  $handler->display->display_options['sorts']['field_class_subject_value']['table'] = 'field_data_field_class_subject';
  $handler->display->display_options['sorts']['field_class_subject_value']['field'] = 'field_class_subject_value';
  /* Sort criterion: Class: CATALOG_NBR (field_class_catalog_nbr) */
  $handler->display->display_options['sorts']['field_class_catalog_nbr_value']['id'] = 'field_class_catalog_nbr_value';
  $handler->display->display_options['sorts']['field_class_catalog_nbr_value']['table'] = 'field_data_field_class_catalog_nbr';
  $handler->display->display_options['sorts']['field_class_catalog_nbr_value']['field'] = 'field_class_catalog_nbr_value';
  /* Sort criterion: Class: CLASS_SECTION (field_class_class_section) */
  $handler->display->display_options['sorts']['field_class_class_section_value']['id'] = 'field_class_class_section_value';
  $handler->display->display_options['sorts']['field_class_class_section_value']['table'] = 'field_data_field_class_class_section';
  $handler->display->display_options['sorts']['field_class_class_section_value']['field'] = 'field_class_class_section_value';
  $handler->display->display_options['filter_groups']['groups'] = array(
    1 => 'AND',
    2 => 'AND',
  );
  /* Filter criterion: Global: Combine fields filter */
  $handler->display->display_options['filters']['combine']['id'] = 'combine';
  $handler->display->display_options['filters']['combine']['table'] = 'views';
  $handler->display->display_options['filters']['combine']['field'] = 'combine';
  $handler->display->display_options['filters']['combine']['operator'] = 'contains';
  $handler->display->display_options['filters']['combine']['group'] = 1;
  $handler->display->display_options['filters']['combine']['exposed'] = TRUE;
  $handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
  $handler->display->display_options['filters']['combine']['expose']['label'] = 'Keyword Search';
  $handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
  $handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
  $handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    6 => 0,
    4 => 0,
    7 => 0,
    3 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['combine']['group_info']['label'] = 'Combine fields filter';
  $handler->display->display_options['filters']['combine']['group_info']['identifier'] = 'combine';
  $handler->display->display_options['filters']['combine']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['combine']['group_info']['group_items'] = array(
    1 => array(),
    2 => array(),
    3 => array(),
  );
  $handler->display->display_options['filters']['combine']['fields'] = array(
    'field_class_class_nbr' => 'field_class_class_nbr',
    'field_class_course_title_long' => 'field_class_course_title_long',
    'field_class_ssr_instr_long' => 'field_class_ssr_instr_long',
    'field_class_ssr_descrlong' => 'field_class_ssr_descrlong',
    'field_class_subject' => 'field_class_subject',
    'field_class_catalog_nbr' => 'field_class_catalog_nbr',
  );
  /* Filter criterion: Class: Status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'cu_class';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value']['value'] = '1';
  $handler->display->display_options['filters']['status']['group'] = 1;
  /* Filter criterion: Class: CANCEL_DT (field_class_cancel_dt) */
  $handler->display->display_options['filters']['field_class_cancel_dt_value']['id'] = 'field_class_cancel_dt_value';
  $handler->display->display_options['filters']['field_class_cancel_dt_value']['table'] = 'field_data_field_class_cancel_dt';
  $handler->display->display_options['filters']['field_class_cancel_dt_value']['field'] = 'field_class_cancel_dt_value';
  $handler->display->display_options['filters']['field_class_cancel_dt_value']['operator'] = 'empty';
  $handler->display->display_options['filters']['field_class_cancel_dt_value']['group'] = 1;
  /* Filter criterion: Class: STRM (field_class_strm) */
  $handler->display->display_options['filters']['field_class_strm_value']['id'] = 'field_class_strm_value';
  $handler->display->display_options['filters']['field_class_strm_value']['table'] = 'field_data_field_class_strm';
  $handler->display->display_options['filters']['field_class_strm_value']['field'] = 'field_class_strm_value';
  $handler->display->display_options['filters']['field_class_strm_value']['group'] = 2;
  $handler->display->display_options['filters']['field_class_strm_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_strm_value']['expose']['operator_id'] = 'field_class_strm_value_op';
  $handler->display->display_options['filters']['field_class_strm_value']['expose']['label'] = 'Term';
  $handler->display->display_options['filters']['field_class_strm_value']['expose']['operator'] = 'field_class_strm_value_op';
  $handler->display->display_options['filters']['field_class_strm_value']['expose']['identifier'] = 'field_class_strm_value';
  $handler->display->display_options['filters']['field_class_strm_value']['expose']['remember_roles'] = array(
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
  /* Filter criterion: Class: INSTITUTION (field_class_institution) */
  $handler->display->display_options['filters']['field_class_institution_value']['id'] = 'field_class_institution_value';
  $handler->display->display_options['filters']['field_class_institution_value']['table'] = 'field_data_field_class_institution';
  $handler->display->display_options['filters']['field_class_institution_value']['field'] = 'field_class_institution_value';
  $handler->display->display_options['filters']['field_class_institution_value']['group'] = 2;
  $handler->display->display_options['filters']['field_class_institution_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_institution_value']['expose']['operator_id'] = 'field_class_institution_value_op';
  $handler->display->display_options['filters']['field_class_institution_value']['expose']['label'] = 'Campus';
  $handler->display->display_options['filters']['field_class_institution_value']['expose']['operator'] = 'field_class_institution_value_op';
  $handler->display->display_options['filters']['field_class_institution_value']['expose']['identifier'] = 'field_class_institution_value';
  $handler->display->display_options['filters']['field_class_institution_value']['expose']['remember_roles'] = array(
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
  /* Filter criterion: Class: College/School (field_class_subject_top_level) */
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['id'] = 'field_class_subject_top_level_value';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['table'] = 'field_data_field_class_subject_top_level';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['field'] = 'field_class_subject_top_level_value';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['group'] = 2;
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['expose']['operator_id'] = 'field_class_subject_top_level_value_op';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['expose']['label'] = 'College/School';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['expose']['operator'] = 'field_class_subject_top_level_value_op';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['expose']['identifier'] = 'field_class_subject_top_level_value';
  $handler->display->display_options['filters']['field_class_subject_top_level_value']['expose']['remember_roles'] = array(
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
  /* Filter criterion: Class: SUBJECT (field_class_subject) */
  $handler->display->display_options['filters']['field_class_subject_value']['id'] = 'field_class_subject_value';
  $handler->display->display_options['filters']['field_class_subject_value']['table'] = 'field_data_field_class_subject';
  $handler->display->display_options['filters']['field_class_subject_value']['field'] = 'field_class_subject_value';
  $handler->display->display_options['filters']['field_class_subject_value']['group'] = 2;
  $handler->display->display_options['filters']['field_class_subject_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['operator_id'] = 'field_class_subject_value_op';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['label'] = 'Subject';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['operator'] = 'field_class_subject_value_op';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['identifier'] = 'field_class_subject_value';
  $handler->display->display_options['filters']['field_class_subject_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    14 => 0,
    7 => 0,
    11 => 0,
    5 => 0,
    9 => 0,
    20 => 0,
    22 => 0,
  );
  /* Filter criterion: Class: INSTRUCTION_MODE_LOVDescr (field_class_instruction_mode_des) */
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['id'] = 'field_class_instruction_mode_des_value';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['table'] = 'field_data_field_class_instruction_mode_des';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['field'] = 'field_class_instruction_mode_des_value';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['group'] = 2;
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['expose']['operator_id'] = 'field_class_instruction_mode_des_value_op';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['expose']['label'] = 'Format';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['expose']['operator'] = 'field_class_instruction_mode_des_value_op';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['expose']['identifier'] = 'field_class_instruction_mode_des_value';
  $handler->display->display_options['filters']['field_class_instruction_mode_des_value']['expose']['remember_roles'] = array(
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
  $handler->display->display_options['path'] = 'courses/search';

  $export['cu_classes'] = $view;
  return $export;
}
