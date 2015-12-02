<?php
/**
 * @file
 * people_content_type.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function people_content_type_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'people_list';
  $view->description = 'Views for people list pages';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'People List';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';
  $handler->display->display_options['row_options']['links'] = FALSE;
  /* Relationship: Content: Taxonomy terms on node */
  $handler->display->display_options['relationships']['term_node_tid']['id'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid']['table'] = 'node';
  $handler->display->display_options['relationships']['term_node_tid']['field'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid']['vocabularies'] = array(
    'person_type' => 'person_type',
    'department' => 0,
    'people_filter_1' => 0,
    'people_filter_2' => 0,
    'people_filter_3' => 0,
    'tags' => 0,
  );
  /* Relationship: Content: Taxonomy terms on node */
  $handler->display->display_options['relationships']['term_node_tid_1']['id'] = 'term_node_tid_1';
  $handler->display->display_options['relationships']['term_node_tid_1']['table'] = 'node';
  $handler->display->display_options['relationships']['term_node_tid_1']['field'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid_1']['label'] = 'department term';
  $handler->display->display_options['relationships']['term_node_tid_1']['vocabularies'] = array(
    'department' => 'department',
    'person_type' => 0,
    'people_filter_1' => 0,
    'people_filter_2' => 0,
    'people_filter_3' => 0,
    'tags' => 0,
  );
  /* Relationship: Content: Taxonomy terms on node */
  $handler->display->display_options['relationships']['term_node_tid_2']['id'] = 'term_node_tid_2';
  $handler->display->display_options['relationships']['term_node_tid_2']['table'] = 'node';
  $handler->display->display_options['relationships']['term_node_tid_2']['field'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid_2']['label'] = 'filter 1 term';
  $handler->display->display_options['relationships']['term_node_tid_2']['vocabularies'] = array(
    'people_filter_1' => 'people_filter_1',
    'department' => 0,
    'person_type' => 0,
    'people_filter_2' => 0,
    'people_filter_3' => 0,
    'tags' => 0,
  );
  /* Relationship: Content: Taxonomy terms on node */
  $handler->display->display_options['relationships']['term_node_tid_3']['id'] = 'term_node_tid_3';
  $handler->display->display_options['relationships']['term_node_tid_3']['table'] = 'node';
  $handler->display->display_options['relationships']['term_node_tid_3']['field'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid_3']['label'] = 'filter 2 term';
  $handler->display->display_options['relationships']['term_node_tid_3']['vocabularies'] = array(
    'people_filter_2' => 'people_filter_2',
    'department' => 0,
    'person_type' => 0,
    'people_filter_1' => 0,
    'people_filter_3' => 0,
    'tags' => 0,
  );
  /* Relationship: Content: Taxonomy terms on node */
  $handler->display->display_options['relationships']['term_node_tid_4']['id'] = 'term_node_tid_4';
  $handler->display->display_options['relationships']['term_node_tid_4']['table'] = 'node';
  $handler->display->display_options['relationships']['term_node_tid_4']['field'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid_4']['vocabularies'] = array(
    'people_filter_3' => 'people_filter_3',
    'department' => 0,
    'person_type' => 0,
    'people_filter_1' => 0,
    'people_filter_2' => 0,
    'tags' => 0,
  );
  /* Field: Content: Photo */
  $handler->display->display_options['fields']['field_person_photo']['id'] = 'field_person_photo';
  $handler->display->display_options['fields']['field_person_photo']['table'] = 'field_data_field_person_photo';
  $handler->display->display_options['fields']['field_person_photo']['field'] = 'field_person_photo';
  $handler->display->display_options['fields']['field_person_photo']['label'] = '';
  $handler->display->display_options['fields']['field_person_photo']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_photo']['element_class'] = 'person-photo';
  $handler->display->display_options['fields']['field_person_photo']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_photo']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_photo']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_photo']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['field_person_photo']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_person_photo']['settings'] = array(
    'image_style' => 'small_thumbnail',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_person_photo']['field_api_classes'] = TRUE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_type'] = 'div';
  $handler->display->display_options['fields']['title']['element_class'] = 'people-table-name';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['field_person_title']['id'] = 'field_person_title';
  $handler->display->display_options['fields']['field_person_title']['table'] = 'field_data_field_person_title';
  $handler->display->display_options['fields']['field_person_title']['field'] = 'field_person_title';
  $handler->display->display_options['fields']['field_person_title']['label'] = '';
  $handler->display->display_options['fields']['field_person_title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_title']['element_type'] = 'div';
  $handler->display->display_options['fields']['field_person_title']['element_class'] = 'people-table-title';
  $handler->display->display_options['fields']['field_person_title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_title']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_title']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_person_title']['separator'] = ' &bull; ';
  $handler->display->display_options['fields']['field_person_title']['field_api_classes'] = TRUE;
  /* Field: Content: Department */
  $handler->display->display_options['fields']['field_person_department']['id'] = 'field_person_department';
  $handler->display->display_options['fields']['field_person_department']['table'] = 'field_data_field_person_department';
  $handler->display->display_options['fields']['field_person_department']['field'] = 'field_person_department';
  $handler->display->display_options['fields']['field_person_department']['label'] = '';
  $handler->display->display_options['fields']['field_person_department']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_department']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_person_department']['alter']['text'] = 'Department(s): [field_person_department]';
  $handler->display->display_options['fields']['field_person_department']['element_type'] = 'div';
  $handler->display->display_options['fields']['field_person_department']['element_class'] = 'people-table-department';
  $handler->display->display_options['fields']['field_person_department']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_department']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_department']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_department']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_person_department']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_person_department']['separator'] = ' &bull; ';
  $handler->display->display_options['fields']['field_person_department']['field_api_classes'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Name';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '<strong>[title]</strong>
<div class="person-title">[field_person_title]</div>
<div class="person-department">[field_person_department]</div>';
  /* Field: Content: Email */
  $handler->display->display_options['fields']['field_person_email']['id'] = 'field_person_email';
  $handler->display->display_options['fields']['field_person_email']['table'] = 'field_data_field_person_email';
  $handler->display->display_options['fields']['field_person_email']['field'] = 'field_person_email';
  $handler->display->display_options['fields']['field_person_email']['label'] = 'Contact';
  $handler->display->display_options['fields']['field_person_email']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_email']['element_class'] = 'contact';
  $handler->display->display_options['fields']['field_person_email']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_email']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_email']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_email']['field_api_classes'] = TRUE;
  /* Field: Content: Phone */
  $handler->display->display_options['fields']['field_person_phone']['id'] = 'field_person_phone';
  $handler->display->display_options['fields']['field_person_phone']['table'] = 'field_data_field_person_phone';
  $handler->display->display_options['fields']['field_person_phone']['field'] = 'field_person_phone';
  $handler->display->display_options['fields']['field_person_phone']['label'] = '';
  $handler->display->display_options['fields']['field_person_phone']['element_type'] = 'div';
  $handler->display->display_options['fields']['field_person_phone']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_phone']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_phone']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_phone']['field_api_classes'] = TRUE;
  /* Field: Content: Job Type */
  $handler->display->display_options['fields']['field_person_job_type']['id'] = 'field_person_job_type';
  $handler->display->display_options['fields']['field_person_job_type']['table'] = 'field_data_field_person_job_type';
  $handler->display->display_options['fields']['field_person_job_type']['field'] = 'field_person_job_type';
  $handler->display->display_options['fields']['field_person_job_type']['label'] = '';
  $handler->display->display_options['fields']['field_person_job_type']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_job_type']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_job_type']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_job_type']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_job_type']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_job_type']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_person_job_type']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_person_job_type']['field_api_classes'] = TRUE;
  /* Field: Content: Filter 3 */
  $handler->display->display_options['fields']['field_person_filter_3']['id'] = 'field_person_filter_3';
  $handler->display->display_options['fields']['field_person_filter_3']['table'] = 'field_data_field_person_filter_3';
  $handler->display->display_options['fields']['field_person_filter_3']['field'] = 'field_person_filter_3';
  $handler->display->display_options['fields']['field_person_filter_3']['label'] = '';
  $handler->display->display_options['fields']['field_person_filter_3']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_filter_3']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_filter_3']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_filter_3']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_filter_3']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_filter_3']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_person_filter_3']['field_api_classes'] = TRUE;
  /* Sort criterion: Taxonomy term: Weight */
  $handler->display->display_options['sorts']['weight']['id'] = 'weight';
  $handler->display->display_options['sorts']['weight']['table'] = 'taxonomy_term_data';
  $handler->display->display_options['sorts']['weight']['field'] = 'weight';
  $handler->display->display_options['sorts']['weight']['relationship'] = 'term_node_tid';
  /* Sort criterion: Content: Last Name (field_person_last_name) */
  $handler->display->display_options['sorts']['field_person_last_name_value']['id'] = 'field_person_last_name_value';
  $handler->display->display_options['sorts']['field_person_last_name_value']['table'] = 'field_data_field_person_last_name';
  $handler->display->display_options['sorts']['field_person_last_name_value']['field'] = 'field_person_last_name_value';
  /* Contextual filter: Department */
  $handler->display->display_options['arguments']['tid']['id'] = 'tid';
  $handler->display->display_options['arguments']['tid']['table'] = 'taxonomy_index';
  $handler->display->display_options['arguments']['tid']['field'] = 'tid';
  $handler->display->display_options['arguments']['tid']['ui_name'] = 'Department';
  $handler->display->display_options['arguments']['tid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['tid']['default_argument_type'] = 'taxonomy_tid';
  $handler->display->display_options['arguments']['tid']['default_argument_options']['term_page'] = FALSE;
  $handler->display->display_options['arguments']['tid']['default_argument_options']['node'] = TRUE;
  $handler->display->display_options['arguments']['tid']['default_argument_options']['vocabularies'] = array(
    'department' => 'department',
  );
  $handler->display->display_options['arguments']['tid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['tid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['tid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['tid']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['tid']['validate']['type'] = 'taxonomy_term';
  $handler->display->display_options['arguments']['tid']['validate_options']['vocabularies'] = array(
    'department' => 'department',
  );
  $handler->display->display_options['arguments']['tid']['validate_options']['type'] = 'tids';
  $handler->display->display_options['arguments']['tid']['break_phrase'] = TRUE;
  $handler->display->display_options['arguments']['tid']['add_table'] = TRUE;
  /* Contextual filter: Type */
  $handler->display->display_options['arguments']['tid_1']['id'] = 'tid_1';
  $handler->display->display_options['arguments']['tid_1']['table'] = 'taxonomy_index';
  $handler->display->display_options['arguments']['tid_1']['field'] = 'tid';
  $handler->display->display_options['arguments']['tid_1']['ui_name'] = 'Type';
  $handler->display->display_options['arguments']['tid_1']['default_action'] = 'default';
  $handler->display->display_options['arguments']['tid_1']['default_argument_type'] = 'taxonomy_tid';
  $handler->display->display_options['arguments']['tid_1']['default_argument_options']['term_page'] = FALSE;
  $handler->display->display_options['arguments']['tid_1']['default_argument_options']['node'] = TRUE;
  $handler->display->display_options['arguments']['tid_1']['default_argument_options']['vocabularies'] = array(
    'person_type' => 'person_type',
  );
  $handler->display->display_options['arguments']['tid_1']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['tid_1']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['tid_1']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['tid_1']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['tid_1']['validate']['type'] = 'taxonomy_term';
  $handler->display->display_options['arguments']['tid_1']['validate_options']['vocabularies'] = array(
    'person_type' => 'person_type',
  );
  $handler->display->display_options['arguments']['tid_1']['validate_options']['type'] = 'tids';
  $handler->display->display_options['arguments']['tid_1']['break_phrase'] = TRUE;
  $handler->display->display_options['arguments']['tid_1']['add_table'] = TRUE;
  /* Contextual filter: Filter 1 */
  $handler->display->display_options['arguments']['tid_2']['id'] = 'tid_2';
  $handler->display->display_options['arguments']['tid_2']['table'] = 'taxonomy_index';
  $handler->display->display_options['arguments']['tid_2']['field'] = 'tid';
  $handler->display->display_options['arguments']['tid_2']['ui_name'] = 'Filter 1';
  $handler->display->display_options['arguments']['tid_2']['default_action'] = 'default';
  $handler->display->display_options['arguments']['tid_2']['default_argument_type'] = 'taxonomy_tid';
  $handler->display->display_options['arguments']['tid_2']['default_argument_options']['term_page'] = FALSE;
  $handler->display->display_options['arguments']['tid_2']['default_argument_options']['node'] = TRUE;
  $handler->display->display_options['arguments']['tid_2']['default_argument_options']['vocabularies'] = array(
    'people_filter_1' => 'people_filter_1',
  );
  $handler->display->display_options['arguments']['tid_2']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['tid_2']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['tid_2']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['tid_2']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['tid_2']['validate']['type'] = 'taxonomy_term';
  $handler->display->display_options['arguments']['tid_2']['validate_options']['vocabularies'] = array(
    'people_filter_1' => 'people_filter_1',
  );
  $handler->display->display_options['arguments']['tid_2']['validate_options']['type'] = 'tids';
  $handler->display->display_options['arguments']['tid_2']['break_phrase'] = TRUE;
  $handler->display->display_options['arguments']['tid_2']['add_table'] = TRUE;
  /* Contextual filter: Filter 2 */
  $handler->display->display_options['arguments']['tid_3']['id'] = 'tid_3';
  $handler->display->display_options['arguments']['tid_3']['table'] = 'taxonomy_index';
  $handler->display->display_options['arguments']['tid_3']['field'] = 'tid';
  $handler->display->display_options['arguments']['tid_3']['ui_name'] = 'Filter 2';
  $handler->display->display_options['arguments']['tid_3']['default_action'] = 'default';
  $handler->display->display_options['arguments']['tid_3']['default_argument_type'] = 'taxonomy_tid';
  $handler->display->display_options['arguments']['tid_3']['default_argument_options']['term_page'] = FALSE;
  $handler->display->display_options['arguments']['tid_3']['default_argument_options']['node'] = TRUE;
  $handler->display->display_options['arguments']['tid_3']['default_argument_options']['vocabularies'] = array(
    'people_filter_2' => 'people_filter_2',
  );
  $handler->display->display_options['arguments']['tid_3']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['tid_3']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['tid_3']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['tid_3']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['tid_3']['validate']['type'] = 'taxonomy_term';
  $handler->display->display_options['arguments']['tid_3']['validate_options']['vocabularies'] = array(
    'people_filter_2' => 'people_filter_2',
  );
  $handler->display->display_options['arguments']['tid_3']['validate_options']['type'] = 'tids';
  $handler->display->display_options['arguments']['tid_3']['break_phrase'] = TRUE;
  $handler->display->display_options['arguments']['tid_3']['add_table'] = TRUE;
  /* Contextual filter: Filter 3 */
  $handler->display->display_options['arguments']['tid_4']['id'] = 'tid_4';
  $handler->display->display_options['arguments']['tid_4']['table'] = 'taxonomy_index';
  $handler->display->display_options['arguments']['tid_4']['field'] = 'tid';
  $handler->display->display_options['arguments']['tid_4']['ui_name'] = 'Filter 3';
  $handler->display->display_options['arguments']['tid_4']['default_action'] = 'default';
  $handler->display->display_options['arguments']['tid_4']['default_argument_type'] = 'taxonomy_tid';
  $handler->display->display_options['arguments']['tid_4']['default_argument_options']['term_page'] = FALSE;
  $handler->display->display_options['arguments']['tid_4']['default_argument_options']['node'] = TRUE;
  $handler->display->display_options['arguments']['tid_4']['default_argument_options']['vocabularies'] = array(
    'people_filter_3' => 'people_filter_3',
  );
  $handler->display->display_options['arguments']['tid_4']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['tid_4']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['tid_4']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['tid_4']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['tid_4']['validate']['type'] = 'taxonomy_term';
  $handler->display->display_options['arguments']['tid_4']['validate_options']['vocabularies'] = array(
    'people_filter_3' => 'people_filter_3',
  );
  $handler->display->display_options['arguments']['tid_4']['validate_options']['type'] = 'tids';
  $handler->display->display_options['arguments']['tid_4']['break_phrase'] = TRUE;
  $handler->display->display_options['arguments']['tid_4']['add_table'] = TRUE;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'person' => 'person',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;
  /* Filter criterion: Content: Department (field_person_department) */
  $handler->display->display_options['filters']['field_person_department_tid']['id'] = 'field_person_department_tid';
  $handler->display->display_options['filters']['field_person_department_tid']['table'] = 'field_data_field_person_department';
  $handler->display->display_options['filters']['field_person_department_tid']['field'] = 'field_person_department_tid';
  $handler->display->display_options['filters']['field_person_department_tid']['group'] = 1;
  $handler->display->display_options['filters']['field_person_department_tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_person_department_tid']['expose']['operator_id'] = 'field_person_department_tid_op';
  $handler->display->display_options['filters']['field_person_department_tid']['expose']['label'] = 'Department';
  $handler->display->display_options['filters']['field_person_department_tid']['expose']['operator'] = 'field_person_department_tid_op';
  $handler->display->display_options['filters']['field_person_department_tid']['expose']['identifier'] = 'field_person_department_tid';
  $handler->display->display_options['filters']['field_person_department_tid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['field_person_department_tid']['type'] = 'select';
  $handler->display->display_options['filters']['field_person_department_tid']['vocabulary'] = 'department';
  $handler->display->display_options['filters']['field_person_department_tid']['hierarchy'] = 1;
  /* Filter criterion: Content: Job Type (field_person_job_type) */
  $handler->display->display_options['filters']['field_person_job_type_tid']['id'] = 'field_person_job_type_tid';
  $handler->display->display_options['filters']['field_person_job_type_tid']['table'] = 'field_data_field_person_job_type';
  $handler->display->display_options['filters']['field_person_job_type_tid']['field'] = 'field_person_job_type_tid';
  $handler->display->display_options['filters']['field_person_job_type_tid']['group'] = 1;
  $handler->display->display_options['filters']['field_person_job_type_tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_person_job_type_tid']['expose']['operator_id'] = 'field_person_job_type_tid_op';
  $handler->display->display_options['filters']['field_person_job_type_tid']['expose']['label'] = 'Job Type';
  $handler->display->display_options['filters']['field_person_job_type_tid']['expose']['operator'] = 'field_person_job_type_tid_op';
  $handler->display->display_options['filters']['field_person_job_type_tid']['expose']['identifier'] = 'field_person_job_type_tid';
  $handler->display->display_options['filters']['field_person_job_type_tid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['field_person_job_type_tid']['type'] = 'select';
  $handler->display->display_options['filters']['field_person_job_type_tid']['vocabulary'] = 'person_type';
  $handler->display->display_options['filters']['field_person_job_type_tid']['hierarchy'] = 1;
  /* Filter criterion: Content: Filter 1 (field_person_filter_1) */
  $handler->display->display_options['filters']['field_person_filter_1_tid']['id'] = 'field_person_filter_1_tid';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['table'] = 'field_data_field_person_filter_1';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['field'] = 'field_person_filter_1_tid';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['group'] = 1;
  $handler->display->display_options['filters']['field_person_filter_1_tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_person_filter_1_tid']['expose']['operator_id'] = 'field_person_filter_1_tid_op';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['expose']['label'] = 'Person Filter 1';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['expose']['operator'] = 'field_person_filter_1_tid_op';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['expose']['identifier'] = 'field_person_filter_1_tid';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['field_person_filter_1_tid']['type'] = 'select';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['vocabulary'] = 'people_filter_1';
  $handler->display->display_options['filters']['field_person_filter_1_tid']['hierarchy'] = 1;
  /* Filter criterion: Content: Filter 2 (field_person_filter_2) */
  $handler->display->display_options['filters']['field_person_filter_2_tid']['id'] = 'field_person_filter_2_tid';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['table'] = 'field_data_field_person_filter_2';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['field'] = 'field_person_filter_2_tid';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['group'] = 1;
  $handler->display->display_options['filters']['field_person_filter_2_tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_person_filter_2_tid']['expose']['operator_id'] = 'field_person_filter_2_tid_op';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['expose']['label'] = 'Filter 2 (field_person_filter_2)';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['expose']['operator'] = 'field_person_filter_2_tid_op';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['expose']['identifier'] = 'field_person_filter_2_tid';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['field_person_filter_2_tid']['type'] = 'select';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['vocabulary'] = 'people_filter_2';
  $handler->display->display_options['filters']['field_person_filter_2_tid']['hierarchy'] = 1;
  /* Filter criterion: Content: Filter 3 (field_person_filter_3) */
  $handler->display->display_options['filters']['field_person_filter_3_tid']['id'] = 'field_person_filter_3_tid';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['table'] = 'field_data_field_person_filter_3';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['field'] = 'field_person_filter_3_tid';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['group'] = 1;
  $handler->display->display_options['filters']['field_person_filter_3_tid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_person_filter_3_tid']['expose']['operator_id'] = 'field_person_filter_3_tid_op';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['expose']['label'] = 'Filter 3 (field_person_filter_3)';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['expose']['operator'] = 'field_person_filter_3_tid_op';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['expose']['identifier'] = 'field_person_filter_3_tid';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['field_person_filter_3_tid']['type'] = 'select';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['vocabulary'] = 'people_filter_3';
  $handler->display->display_options['filters']['field_person_filter_3_tid']['hierarchy'] = 1;

  /* Display: People List */
  $handler = $view->new_display('page', 'People List', 'list_page');
  $handler->display->display_options['path'] = 'people-list/list';

  /* Display: People Table */
  $handler = $view->new_display('page', 'People Table', 'table_page');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'field_person_photo' => 'field_person_photo',
    'title' => 'title',
    'field_person_title' => 'title',
    'field_person_department' => 'title',
    'nothing' => 'nothing',
    'field_person_email' => 'field_person_email',
    'field_person_phone' => 'field_person_email',
    'field_person_job_type' => 'field_person_job_type',
    'field_person_filter_3' => 'field_person_filter_3',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'field_person_photo' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 1,
    ),
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_title' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_department' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_email' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_phone' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_job_type' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_filter_3' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['path'] = 'people-list/table';

  /* Display: People Grid */
  $handler = $view->new_display('page', 'People Grid', 'grid_page');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['style_options']['row_class'] = 'grid-person';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'node';
  $handler->display->display_options['row_options']['view_mode'] = 'grid';
  $handler->display->display_options['row_options']['links'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['path'] = 'people-list/grid';

  /* Display: People Table Row */
  $handler = $view->new_display('page', 'People Table Row', 'table_row_page');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'field_person_photo' => 'field_person_photo',
    'rendered_entity' => 'rendered_entity',
    'field_person_email' => 'field_person_email',
    'field_person_phone' => 'field_person_email',
    'field_person_job_type' => 'field_person_job_type',
    'field_person_filter_3' => 'field_person_filter_3',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'field_person_photo' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 1,
    ),
    'rendered_entity' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_email' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 1,
    ),
    'field_person_phone' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_job_type' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_person_filter_3' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Photo */
  $handler->display->display_options['fields']['field_person_photo']['id'] = 'field_person_photo';
  $handler->display->display_options['fields']['field_person_photo']['table'] = 'field_data_field_person_photo';
  $handler->display->display_options['fields']['field_person_photo']['field'] = 'field_person_photo';
  $handler->display->display_options['fields']['field_person_photo']['label'] = '';
  $handler->display->display_options['fields']['field_person_photo']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_photo']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_photo']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_photo']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_photo']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['field_person_photo']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_person_photo']['settings'] = array(
    'image_style' => 'small_thumbnail',
    'image_link' => 'content',
  );
  $handler->display->display_options['fields']['field_person_photo']['field_api_classes'] = TRUE;
  /* Field: Content: Rendered Node */
  $handler->display->display_options['fields']['rendered_entity']['id'] = 'rendered_entity';
  $handler->display->display_options['fields']['rendered_entity']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['rendered_entity']['field'] = 'rendered_entity';
  $handler->display->display_options['fields']['rendered_entity']['label'] = '';
  $handler->display->display_options['fields']['rendered_entity']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['rendered_entity']['link_to_entity'] = 1;
  $handler->display->display_options['fields']['rendered_entity']['display'] = 'view';
  $handler->display->display_options['fields']['rendered_entity']['view_mode'] = 'table';
  $handler->display->display_options['fields']['rendered_entity']['bypass_access'] = 0;
  /* Field: Content: Email */
  $handler->display->display_options['fields']['field_person_email']['id'] = 'field_person_email';
  $handler->display->display_options['fields']['field_person_email']['table'] = 'field_data_field_person_email';
  $handler->display->display_options['fields']['field_person_email']['field'] = 'field_person_email';
  $handler->display->display_options['fields']['field_person_email']['label'] = '';
  $handler->display->display_options['fields']['field_person_email']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_person_email']['alter']['text'] = '<div class="person-email"><i class="fa fa-envelope"></i> <a href="mailto:[field_person_email]">Email</a></div>';
  $handler->display->display_options['fields']['field_person_email']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_email']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_email']['element_wrapper_type'] = 'div';
  $handler->display->display_options['fields']['field_person_email']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['field_person_email']['type'] = 'email_plain';
  $handler->display->display_options['fields']['field_person_email']['field_api_classes'] = TRUE;
  /* Field: Content: Phone */
  $handler->display->display_options['fields']['field_person_phone']['id'] = 'field_person_phone';
  $handler->display->display_options['fields']['field_person_phone']['table'] = 'field_data_field_person_phone';
  $handler->display->display_options['fields']['field_person_phone']['field'] = 'field_person_phone';
  $handler->display->display_options['fields']['field_person_phone']['label'] = '';
  $handler->display->display_options['fields']['field_person_phone']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_person_phone']['alter']['text'] = '<div class="person-phone"><i class="fa fa-phone"></i> [field_person_phone]</div>';
  $handler->display->display_options['fields']['field_person_phone']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_phone']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_phone']['element_wrapper_type'] = 'div';
  $handler->display->display_options['fields']['field_person_phone']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_phone']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['field_person_phone']['field_api_classes'] = TRUE;
  /* Field: Content: Job Type */
  $handler->display->display_options['fields']['field_person_job_type']['id'] = 'field_person_job_type';
  $handler->display->display_options['fields']['field_person_job_type']['table'] = 'field_data_field_person_job_type';
  $handler->display->display_options['fields']['field_person_job_type']['field'] = 'field_person_job_type';
  $handler->display->display_options['fields']['field_person_job_type']['label'] = '';
  $handler->display->display_options['fields']['field_person_job_type']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_job_type']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_job_type']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_job_type']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_job_type']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_job_type']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_person_job_type']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_person_job_type']['field_api_classes'] = TRUE;
  /* Field: Content: Filter 3 */
  $handler->display->display_options['fields']['field_person_filter_3']['id'] = 'field_person_filter_3';
  $handler->display->display_options['fields']['field_person_filter_3']['table'] = 'field_data_field_person_filter_3';
  $handler->display->display_options['fields']['field_person_filter_3']['field'] = 'field_person_filter_3';
  $handler->display->display_options['fields']['field_person_filter_3']['label'] = '';
  $handler->display->display_options['fields']['field_person_filter_3']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_filter_3']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_filter_3']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_filter_3']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_filter_3']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_filter_3']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_person_filter_3']['field_api_classes'] = TRUE;
  /* Field: Content: Department */
  $handler->display->display_options['fields']['field_person_department']['id'] = 'field_person_department';
  $handler->display->display_options['fields']['field_person_department']['table'] = 'field_data_field_person_department';
  $handler->display->display_options['fields']['field_person_department']['field'] = 'field_person_department';
  $handler->display->display_options['fields']['field_person_department']['label'] = '';
  $handler->display->display_options['fields']['field_person_department']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_person_department']['element_type'] = '0';
  $handler->display->display_options['fields']['field_person_department']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_person_department']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_person_department']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_person_department']['group_rows'] = FALSE;
  $handler->display->display_options['fields']['field_person_department']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_person_department']['field_api_classes'] = TRUE;
  $handler->display->display_options['path'] = 'people-list/list';
  $export['people_list'] = $view;

  return $export;
}
