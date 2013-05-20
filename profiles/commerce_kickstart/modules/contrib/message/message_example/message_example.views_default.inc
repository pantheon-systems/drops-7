<?php
/**
 * @file
 * message_example.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function message_example_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'message_example';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'message';
  $view->human_name = 'Message example';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Message example';
  $handler->display->display_options['css_class'] = 'message-example';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['row_plugin'] = 'entity';
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['label'] = 'UAS (User activity stream)';
  $handler->display->display_options['header']['area']['content'] = 'Bellow is an example of a typical UAS (User activity stream), using the Message module.';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'No messages created yet';
  $handler->display->display_options['empty']['area']['content'] = 'You should create some activity (manually or with Drush + Devel generate module):

1) Register new users (<code>drush genu 10</code>)
2) Create new content and comments (<code>drush genc 10 5</code>)

';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
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
  /* Filter criterion: Message: Published (field_published) */
  $handler->display->display_options['filters']['field_published_value']['id'] = 'field_published_value';
  $handler->display->display_options['filters']['field_published_value']['table'] = 'field_data_field_published';
  $handler->display->display_options['filters']['field_published_value']['field'] = 'field_published_value';
  $handler->display->display_options['filters']['field_published_value']['value'] = array(
    1 => '1',
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['path'] = 'message-example';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Message example';
  $handler->display->display_options['menu']['description'] = 'Typical UAS (User activity stream).';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $export['message_example'] = $view;

  return $export;
}
