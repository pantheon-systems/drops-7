<?php

function mailjet_event_views_default() {
  $views = array();

  $view = new view();
  $view->name = 'mailjet_events_log';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'mailjet_event';
  $view->human_name = 'Mailjet Events Log';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Mailjet Events Log';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer mailjet module';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  /* Relationship: Mailjet Event: User ID */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'mailjet_event';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  /* Field: Mailjet Event: Mailjet event ID */
  $handler->display->display_options['fields']['id']['id'] = 'id';
  $handler->display->display_options['fields']['id']['table'] = 'mailjet_event';
  $handler->display->display_options['fields']['id']['field'] = 'id';
  /* Field: User: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'users';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'uid';
  /* Field: Mailjet Event: Event */
  $handler->display->display_options['fields']['event']['id'] = 'event';
  $handler->display->display_options['fields']['event']['table'] = 'mailjet_event';
  $handler->display->display_options['fields']['event']['field'] = 'event';
  /* Sort criterion: Mailjet Event: Mailjet event ID */
  $handler->display->display_options['sorts']['id']['id'] = 'id';
  $handler->display->display_options['sorts']['id']['table'] = 'mailjet_event';
  $handler->display->display_options['sorts']['id']['field'] = 'id';
  $handler->display->display_options['sorts']['id']['order'] = 'DESC';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/reports/mailjet';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Mailjet Events';
  $handler->display->display_options['menu']['description'] = 'A log of Mailjet event callbacks revived by the site';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;


  $views[$view->name] = $view;

  return $views;
}