<?php
/**
 * @file
 * openpublish_most_popular.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function openpublish_most_popular_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'op_most_popular';
  $view->description = 'Some GA Stats powered views that display the most popular content for specific types, sections and so on.';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'OpenPublish - Most Popular';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'OpenPublish - Most popular today';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Sort criterion: Statistics: Page Views in the past 48 hours */
  $handler->display->display_options['sorts']['count']['id'] = 'count';
  $handler->display->display_options['sorts']['count']['table'] = 'ga_stats_count_pageviews_2days';
  $handler->display->display_options['sorts']['count']['field'] = 'count';
  /* Sort criterion: Statistics: Page Views in the past 24 hours */
  $handler->display->display_options['sorts']['count_1']['id'] = 'count_1';
  $handler->display->display_options['sorts']['count_1']['table'] = 'ga_stats_count_pageviews_today';
  $handler->display->display_options['sorts']['count_1']['field'] = 'count';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Taxonomy Filter: Node Taxonomy Filter */
  $handler->display->display_options['filters']['nid']['id'] = 'nid';
  $handler->display->display_options['filters']['nid']['table'] = 'views_node_taxonomy_filter';
  $handler->display->display_options['filters']['nid']['field'] = 'nid';
  $handler->display->display_options['filters']['nid']['value'] = array(
    0 => array(
      'sections' => 'sections',
    ),
    1 => 1,
  );
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'openpublish_article' => 'openpublish_article',
    'openpublish_blog_post' => 'openpublish_blog_post',
    'openpublish_photo_gallery' => 'openpublish_photo_gallery',
    'openpublish_video' => 'openpublish_video',
  );

  /* Display: Popular in... (24 hours) */
  $handler = $view->new_display('block', 'Popular in... (24 hours)', 'block_popular_section_24h');
  $handler->display->display_options['display_description'] = 'Lists the most popular content in this section for a 24h period.';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Sort criterion: Statistics: Page Views in the past 24 hours */
  $handler->display->display_options['sorts']['count_1']['id'] = 'count_1';
  $handler->display->display_options['sorts']['count_1']['table'] = 'ga_stats_count_pageviews_today';
  $handler->display->display_options['sorts']['count_1']['field'] = 'count';
  $handler->display->display_options['sorts']['count_1']['order'] = 'DESC';

  /* Display: Popular in... (7 days) */
  $handler = $view->new_display('block', 'Popular in... (7 days)', 'block_popular_section_7d');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'OpenPublish - Most popular this week';
  $handler->display->display_options['display_description'] = 'Lists the most popular content in this section for a 7d period.';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Statistics: Page Views in the past 7 days */
  $handler->display->display_options['sorts']['count']['id'] = 'count';
  $handler->display->display_options['sorts']['count']['table'] = 'ga_stats_count_pageviews_week';
  $handler->display->display_options['sorts']['count']['field'] = 'count';
  $handler->display->display_options['sorts']['count']['order'] = 'DESC';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';

  /* Display: Popular in... (48 hours) */
  $handler = $view->new_display('block', 'Popular in... (48 hours)', 'block_1');
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Sort criterion: Statistics: Page Views in the past 48 hours */
  $handler->display->display_options['sorts']['count']['id'] = 'count';
  $handler->display->display_options['sorts']['count']['table'] = 'ga_stats_count_pageviews_2days';
  $handler->display->display_options['sorts']['count']['field'] = 'count';
  $handler->display->display_options['sorts']['count']['order'] = 'DESC';
  $export['op_most_popular'] = $view;

  return $export;
}
