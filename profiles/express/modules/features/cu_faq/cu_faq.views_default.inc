<?php
/**
 * @file
 * cu_faq.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function cu_faq_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'cu_faq';
  $view->description = 'FAQ EVA.';
  $view->tag = 'faq';
  $view->base_table = 'node';
  $view->human_name = 'CU FAQ';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'views_accordion';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'field_qa_collection_title',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['collapsible'] = 1;
  $handler->display->display_options['style_options']['row-start-open'] = 'none';
  $handler->display->display_options['style_options']['autoheight'] = 0;
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Relationship: Content: FAQ Collection (field_qa_collection) */
  $handler->display->display_options['relationships']['field_qa_collection_value']['id'] = 'field_qa_collection_value';
  $handler->display->display_options['relationships']['field_qa_collection_value']['table'] = 'field_data_field_qa_collection';
  $handler->display->display_options['relationships']['field_qa_collection_value']['field'] = 'field_qa_collection_value';
  $handler->display->display_options['relationships']['field_qa_collection_value']['label'] = 'top level collection';
  $handler->display->display_options['relationships']['field_qa_collection_value']['delta'] = '-1';
  /* Relationship: Field collection item: Questions and Answers (field_qa) */
  $handler->display->display_options['relationships']['field_qa_value']['id'] = 'field_qa_value';
  $handler->display->display_options['relationships']['field_qa_value']['table'] = 'field_data_field_qa';
  $handler->display->display_options['relationships']['field_qa_value']['field'] = 'field_qa_value';
  $handler->display->display_options['relationships']['field_qa_value']['relationship'] = 'field_qa_collection_value';
  $handler->display->display_options['relationships']['field_qa_value']['label'] = 'questions and answers';
  $handler->display->display_options['relationships']['field_qa_value']['delta'] = '-1';
  /* Field: Field collection item: Question */
  $handler->display->display_options['fields']['field_qa_question']['id'] = 'field_qa_question';
  $handler->display->display_options['fields']['field_qa_question']['table'] = 'field_data_field_qa_question';
  $handler->display->display_options['fields']['field_qa_question']['field'] = 'field_qa_question';
  $handler->display->display_options['fields']['field_qa_question']['relationship'] = 'field_qa_value';
  $handler->display->display_options['fields']['field_qa_question']['label'] = '';
  $handler->display->display_options['fields']['field_qa_question']['element_type'] = '0';
  $handler->display->display_options['fields']['field_qa_question']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_qa_question']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_qa_question']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_qa_question']['field_api_classes'] = TRUE;
  /* Field: Field collection item: Answer */
  $handler->display->display_options['fields']['field_qa_answer']['id'] = 'field_qa_answer';
  $handler->display->display_options['fields']['field_qa_answer']['table'] = 'field_data_field_qa_answer';
  $handler->display->display_options['fields']['field_qa_answer']['field'] = 'field_qa_answer';
  $handler->display->display_options['fields']['field_qa_answer']['relationship'] = 'field_qa_value';
  $handler->display->display_options['fields']['field_qa_answer']['label'] = '';
  $handler->display->display_options['fields']['field_qa_answer']['element_type'] = '0';
  $handler->display->display_options['fields']['field_qa_answer']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_qa_answer']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_qa_answer']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_qa_answer']['field_api_classes'] = TRUE;
  /* Field: Field collection item: Title */
  $handler->display->display_options['fields']['field_qa_collection_title']['id'] = 'field_qa_collection_title';
  $handler->display->display_options['fields']['field_qa_collection_title']['table'] = 'field_data_field_qa_collection_title';
  $handler->display->display_options['fields']['field_qa_collection_title']['field'] = 'field_qa_collection_title';
  $handler->display->display_options['fields']['field_qa_collection_title']['relationship'] = 'field_qa_collection_value';
  $handler->display->display_options['fields']['field_qa_collection_title']['label'] = '';
  $handler->display->display_options['fields']['field_qa_collection_title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_qa_collection_title']['element_type'] = 'h2';
  $handler->display->display_options['fields']['field_qa_collection_title']['element_class'] = 'faq-section-title';
  $handler->display->display_options['fields']['field_qa_collection_title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_qa_collection_title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_qa_collection_title']['element_wrapper_class'] = 'faq-section-title-wrapper';
  $handler->display->display_options['fields']['field_qa_collection_title']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_qa_collection_title']['field_api_classes'] = TRUE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'faqs' => 'faqs',
  );

  /* Display: EVA Field */
  $handler = $view->new_display('entity_view', 'EVA Field', 'entity_view_1');
  $handler->display->display_options['entity_type'] = 'node';
  $handler->display->display_options['bundles'] = array(
    0 => 'faqs',
  );
  $export['cu_faq'] = $view;

  return $export;
}
