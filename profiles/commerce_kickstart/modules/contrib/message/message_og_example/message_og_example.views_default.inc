<?php
/**
 * @file
 * message_og_example.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function message_og_example_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'message_og_example';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'message';
  $view->human_name = 'Message OG example';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Message OG example';
  $handler->display->display_options['css_class'] = 'message-example';
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'panels_fields';
  $handler->display->display_options['row_options']['regions'] = array(
    'message_render' => 'left',
    'message_render_1' => 'right',
    'message_render_2' => 'right',
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['label'] = 'UAS (User activity stream) with OG';
  $handler->display->display_options['header']['area']['empty'] = FALSE;
  $handler->display->display_options['header']['area']['content'] = 'Bellow is an example of a typical UAS (User activity stream) integrated with Organic groups, using the Message module.';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  $handler->display->display_options['header']['area']['tokenize'] = 0;
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'No messages created yet';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'You should create some activity (manually or with Drush + Devel generate module):

1) Register new users (<code>drush genu 10</code>)
2) Create new content and comments (<code>drush genc 10 0 --types=group,post</code>)

';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  $handler->display->display_options['empty']['area']['tokenize'] = 0;
  /* Field: User picture - (Partial 0) */
  $handler->display->display_options['fields']['message_render']['id'] = 'message_render';
  $handler->display->display_options['fields']['message_render']['table'] = 'message';
  $handler->display->display_options['fields']['message_render']['field'] = 'message_render';
  $handler->display->display_options['fields']['message_render']['ui_name'] = 'User picture - (Partial 0)';
  $handler->display->display_options['fields']['message_render']['label'] = '';
  $handler->display->display_options['fields']['message_render']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['message_render']['partials'] = 1;
  $handler->display->display_options['fields']['message_render']['partials_delta'] = '0';
  /* Field: Operation info - (Partial 1) */
  $handler->display->display_options['fields']['message_render_1']['id'] = 'message_render_1';
  $handler->display->display_options['fields']['message_render_1']['table'] = 'message';
  $handler->display->display_options['fields']['message_render_1']['field'] = 'message_render';
  $handler->display->display_options['fields']['message_render_1']['ui_name'] = 'Operation info - (Partial 1)';
  $handler->display->display_options['fields']['message_render_1']['label'] = '';
  $handler->display->display_options['fields']['message_render_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['message_render_1']['partials'] = 1;
  $handler->display->display_options['fields']['message_render_1']['partials_delta'] = '1';
  /* Field: Teaser - (Partial 2) */
  $handler->display->display_options['fields']['message_render_2']['id'] = 'message_render_2';
  $handler->display->display_options['fields']['message_render_2']['table'] = 'message';
  $handler->display->display_options['fields']['message_render_2']['field'] = 'message_render';
  $handler->display->display_options['fields']['message_render_2']['ui_name'] = 'Teaser - (Partial 2)';
  $handler->display->display_options['fields']['message_render_2']['label'] = '';
  $handler->display->display_options['fields']['message_render_2']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['message_render_2']['partials'] = 1;
  $handler->display->display_options['fields']['message_render_2']['partials_delta'] = '2';
  /* Sort criterion: Message: Timestamp */
  $handler->display->display_options['sorts']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['table'] = 'message';
  $handler->display->display_options['sorts']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['order'] = 'DESC';
  /* Contextual filter: OG groups of the current user */
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['id'] = 'field_node_groups_ref_target_id';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['table'] = 'field_data_field_node_groups_ref';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['field'] = 'field_node_groups_ref_target_id';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['ui_name'] = 'OG groups of the current user';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['default_argument_type'] = 'og_user_groups';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['break_phrase'] = 1;
  $handler->display->display_options['arguments']['field_node_groups_ref_target_id']['not'] = 0;
  /* Filter criterion: Message: Published (field_published) */
  $handler->display->display_options['filters']['field_published_value']['id'] = 'field_published_value';
  $handler->display->display_options['filters']['field_published_value']['table'] = 'field_data_field_published';
  $handler->display->display_options['filters']['field_published_value']['field'] = 'field_published_value';
  $handler->display->display_options['filters']['field_published_value']['value'] = array(
    1 => '1',
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'message-og-example';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Messasge OG example';
  $handler->display->display_options['menu']['description'] = 'Typical UAS (User activity stream) related to Organic groups.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $export['message_og_example'] = $view;

  return $export;
}
