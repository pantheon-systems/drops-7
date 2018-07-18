<?php

/**
 * Implements hook_views_default_views().
 */
function google_analytics_reports_views_default_views() {

  // Google Analytics Reports Summary view.
  $view = new view();
  $view->name = 'google_analytics_reports_summary';
  $view->description = t('Display summary statistic information about website from Google Analytics.');
  $view->tag = t('Google Analytics Reports');
  $view->base_table = 'google_analytics';
  $view->human_name = t('Google Analytics Reports Summary');
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = t('Google Analytics Summary');
  $handler->display->display_options['css_class'] = 'google-analytics-reports-summary';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access google analytics reports';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  /* Field: Session metrics: Sessions */
  $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['element_type'] = 'strong';
  $handler->display->display_options['fields']['sessions']['empty'] = '-';
  $handler->display->display_options['fields']['sessions']['precision'] = '0';
  $handler->display->display_options['fields']['sessions']['separator'] = ' ';
  /* Field: User metrics: Users */
  $handler->display->display_options['fields']['users']['id'] = 'users';
  $handler->display->display_options['fields']['users']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['users']['field'] = 'users';
  $handler->display->display_options['fields']['users']['element_type'] = 'strong';
  $handler->display->display_options['fields']['users']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['users']['empty'] = '-';
  $handler->display->display_options['fields']['users']['precision'] = '0';
  $handler->display->display_options['fields']['users']['separator'] = ' ';
  /* Field: Page tracking metrics: Pageviews */
  $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['element_type'] = 'strong';
  $handler->display->display_options['fields']['pageviews']['empty'] = '-';
  $handler->display->display_options['fields']['pageviews']['precision'] = '0';
  $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
  /* Field: Page tracking calculated metrics: Pages / Session */
  $handler->display->display_options['fields']['pageviewsPerSession']['id'] = 'pageviewsPerSession';
  $handler->display->display_options['fields']['pageviewsPerSession']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageviewsPerSession']['field'] = 'pageviewsPerSession';
  $handler->display->display_options['fields']['pageviewsPerSession']['element_type'] = 'strong';
  $handler->display->display_options['fields']['pageviewsPerSession']['empty'] = '-';
  $handler->display->display_options['fields']['pageviewsPerSession']['set_precision'] = TRUE;
  $handler->display->display_options['fields']['pageviewsPerSession']['precision'] = '2';
  $handler->display->display_options['fields']['pageviewsPerSession']['separator'] = ' ';
  /* Field: Session calculated metrics: Avg. Session Duration */
  $handler->display->display_options['fields']['avgSessionDuration']['id'] = 'avgSessionDuration';
  $handler->display->display_options['fields']['avgSessionDuration']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['avgSessionDuration']['field'] = 'avgSessionDuration';
  $handler->display->display_options['fields']['avgSessionDuration']['element_type'] = 'strong';
  $handler->display->display_options['fields']['avgSessionDuration']['empty'] = '-';
  $handler->display->display_options['fields']['avgSessionDuration']['date_format'] = 'custom';
  $handler->display->display_options['fields']['avgSessionDuration']['custom_date_format'] = 'i:s';
  $handler->display->display_options['fields']['avgSessionDuration']['second_date_format'] = 'long';
  /* Field: Session calculated metrics: Bounce Rate */
  $handler->display->display_options['fields']['bounceRate']['id'] = 'bounceRate';
  $handler->display->display_options['fields']['bounceRate']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['bounceRate']['field'] = 'bounceRate';
  $handler->display->display_options['fields']['bounceRate']['element_type'] = 'strong';
  $handler->display->display_options['fields']['bounceRate']['empty'] = '-';
  $handler->display->display_options['fields']['bounceRate']['set_precision'] = TRUE;
  $handler->display->display_options['fields']['bounceRate']['precision'] = '2';
  $handler->display->display_options['fields']['bounceRate']['separator'] = '';
  $handler->display->display_options['fields']['bounceRate']['suffix'] = '%';
  /* Field: User calculated metrics: % New Sessions */
  $handler->display->display_options['fields']['percentNewSessions']['id'] = 'percentNewSessions';
  $handler->display->display_options['fields']['percentNewSessions']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['percentNewSessions']['field'] = 'percentNewSessions';
  $handler->display->display_options['fields']['percentNewSessions']['element_type'] = 'strong';
  $handler->display->display_options['fields']['percentNewSessions']['empty'] = '-';
  $handler->display->display_options['fields']['percentNewSessions']['set_precision'] = TRUE;
  $handler->display->display_options['fields']['percentNewSessions']['precision'] = '2';
  $handler->display->display_options['fields']['percentNewSessions']['separator'] = '';
  $handler->display->display_options['fields']['percentNewSessions']['suffix'] = '%';
  /* Filter criterion: Google Analytics: Start date of report */
  $handler->display->display_options['filters']['start_date']['id'] = 'start_date';
  $handler->display->display_options['filters']['start_date']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['start_date']['field'] = 'start_date';
  $handler->display->display_options['filters']['start_date']['value']['value'] = '-31 day';
  $handler->display->display_options['filters']['start_date']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['start_date']['group'] = 1;
  /* Filter criterion: Google Analytics: End date of report */
  $handler->display->display_options['filters']['end_date']['id'] = 'end_date';
  $handler->display->display_options['filters']['end_date']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['end_date']['field'] = 'end_date';
  $handler->display->display_options['filters']['end_date']['value']['value'] = '-1 day';
  $handler->display->display_options['filters']['end_date']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['end_date']['group'] = 1;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'sessions' => 'sessions',
    'users' => 'users',
    'pageviews' => 'pageviews',
    'pageviewsPerSession' => 'pageviewsPerSession',
    'avgSessionDuration' => 'avgSessionDuration',
    'bounceRate' => 'bounceRate',
    'percentNewSessions' => 'percentNewSessions',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'sessions' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'users' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'pageviews' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'pageviewsPerSession' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'avgSessionDuration' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'bounceRate' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'percentNewSessions' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['path'] = 'admin/reports/google-analytics-reports/summary';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = t('Google Analytics Summary');
  $handler->display->display_options['menu']['description'] = t('View a traffic report for your site.');
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  /* Display: Top Pages */
  $handler = $view->new_display('attachment', 'Top Pages', 'attachment_top_pages');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'google-analytics-reports-summary google-analytics-reports-summary-top-pages';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['type'] = 'ol';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['inline'] = array(
    'pageTitle' => 'pageTitle',
    'pageviews' => 'pageviews',
  );
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('Top Pages');
  $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Top Pages') . '</h2>';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('Pages not found.');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t('Pages not found.');
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Page tracking dimensions: Page */
  $handler->display->display_options['fields']['pagePath']['id'] = 'pagePath';
  $handler->display->display_options['fields']['pagePath']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pagePath']['field'] = 'pagePath';
  $handler->display->display_options['fields']['pagePath']['label'] = '';
  $handler->display->display_options['fields']['pagePath']['exclude'] = TRUE;
  $handler->display->display_options['fields']['pagePath']['element_label_colon'] = FALSE;
  /* Field: Page tracking dimensions: Page Title */
  $handler->display->display_options['fields']['pageTitle']['id'] = 'pageTitle';
  $handler->display->display_options['fields']['pageTitle']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageTitle']['field'] = 'pageTitle';
  $handler->display->display_options['fields']['pageTitle']['label'] = '';
  $handler->display->display_options['fields']['pageTitle']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['pageTitle']['alter']['text'] = '<a href="[pagePath]">[pageTitle]</a>';
  $handler->display->display_options['fields']['pageTitle']['element_label_colon'] = FALSE;
  /* Field: Page tracking metrics: Pageviews */
  $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['label'] = '';
  $handler->display->display_options['fields']['pageviews']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['pageviews']['precision'] = '0';
  $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
  $handler->display->display_options['fields']['pageviews']['prefix'] = '- ';
  $handler->display->display_options['fields']['pageviews']['suffix'] = ' views';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Page tracking metrics: Pageviews */
  $handler->display->display_options['sorts']['pageviews']['id'] = 'pageviews';
  $handler->display->display_options['sorts']['pageviews']['table'] = 'google_analytics';
  $handler->display->display_options['sorts']['pageviews']['field'] = 'pageviews';
  $handler->display->display_options['sorts']['pageviews']['order'] = 'DESC';
  $handler->display->display_options['displays'] = array(
    'page' => 'page',
    'default' => 0,
  );
  $handler->display->display_options['attachment_position'] = 'after';

  /* Display: Top Sources */
  $handler = $view->new_display('attachment', 'Top Sources', 'attachment_top_sources');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'google-analytics-reports-summary google-analytics-reports-summary-top-sources';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['type'] = 'ol';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['inline'] = array(
    'source' => 'source',
    'medium' => 'medium',
    'sessions' => 'sessions',
  );
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('Top Sources');
  $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Top Sources') . '</h2>';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('Sources not found.');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t('Sources not found.');
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Traffic sources dimensions: Source */
  $handler->display->display_options['fields']['source']['id'] = 'source';
  $handler->display->display_options['fields']['source']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['source']['field'] = 'source';
  $handler->display->display_options['fields']['source']['label'] = '';
  $handler->display->display_options['fields']['source']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['source']['alter']['text'] = '<strong>[source]</strong> - ';
  $handler->display->display_options['fields']['source']['element_label_colon'] = FALSE;
  /* Field: Traffic sources dimensions: Medium */
  $handler->display->display_options['fields']['medium']['id'] = 'medium';
  $handler->display->display_options['fields']['medium']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['medium']['field'] = 'medium';
  $handler->display->display_options['fields']['medium']['label'] = '';
  $handler->display->display_options['fields']['medium']['element_label_colon'] = FALSE;
  /* Field: Session metrics: Sessions */
  $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['label'] = '';
  $handler->display->display_options['fields']['sessions']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sessions']['precision'] = '0';
  $handler->display->display_options['fields']['sessions']['separator'] = ' ';
  $handler->display->display_options['fields']['sessions']['prefix'] = '- ';
  $handler->display->display_options['fields']['sessions']['suffix'] = ' sessions';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Session metrics: Sessions */
  $handler->display->display_options['sorts']['sessions']['id'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['sorts']['sessions']['field'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['order'] = 'DESC';
  $handler->display->display_options['displays'] = array(
    'page' => 'page',
    'default' => 0,
  );
  $handler->display->display_options['attachment_position'] = 'after';

  /* Display: Top Keywords */
  $handler = $view->new_display('attachment', 'Top Keywords', 'attachment_top_keywords');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'google-analytics-reports-summary google-analytics-reports-summary-top-keywords';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['type'] = 'ol';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['inline'] = array(
    'keyword' => 'keyword',
    'sessions' => 'sessions',
  );
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('Top Keywords');
  $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Top Keywords') . '</h2>';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('Keywords not found.');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t('Keywords not found.');
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Traffic sources dimensions: Keyword */
  $handler->display->display_options['fields']['keyword']['id'] = 'keyword';
  $handler->display->display_options['fields']['keyword']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['keyword']['field'] = 'keyword';
  $handler->display->display_options['fields']['keyword']['label'] = '';
  $handler->display->display_options['fields']['keyword']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['keyword']['alter']['text'] = '<strong>[keyword]</strong>';
  $handler->display->display_options['fields']['keyword']['element_label_colon'] = FALSE;
  /* Field: Session metrics: Sessions */
  $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['label'] = '';
  $handler->display->display_options['fields']['sessions']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sessions']['precision'] = '0';
  $handler->display->display_options['fields']['sessions']['separator'] = ' ';
  $handler->display->display_options['fields']['sessions']['prefix'] = '- ';
  $handler->display->display_options['fields']['sessions']['suffix'] = ' sessions';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Session metrics: Sessions */
  $handler->display->display_options['sorts']['sessions']['id'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['sorts']['sessions']['field'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['order'] = 'DESC';
  $handler->display->display_options['displays'] = array(
    'page' => 'page',
    'default' => 0,
  );
  $handler->display->display_options['attachment_position'] = 'after';

  if (module_exists('charts')) {
    /* Display: Chart */
    $handler = $view->new_display('attachment', 'Chart', 'attachment_chart');
    $handler->display->display_options['defaults']['title'] = FALSE;
    $handler->display->display_options['defaults']['css_class'] = FALSE;
    $handler->display->display_options['css_class'] = 'google-analytics-reports-summary google-analytics-reports-summary-chart';
    $handler->display->display_options['pager']['type'] = 'none';
    $handler->display->display_options['pager']['options']['offset'] = '0';
    $handler->display->display_options['defaults']['style_plugin'] = FALSE;
    $handler->display->display_options['style_plugin'] = 'chart';
    $handler->display->display_options['style_options']['type'] = 'area';
    $handler->display->display_options['style_options']['library'] = '';
    $handler->display->display_options['style_options']['label_field'] = 'date';
    $handler->display->display_options['style_options']['data_fields'] = array(
      'sessions' => 'sessions',
      'pageviews' => 'pageviews',
      'date' => 0,
    );
    $handler->display->display_options['style_options']['field_colors'] = array(
      'date' => '#2f7ed8',
      'sessions' => '#058dc7',
      'pageviews' => '#aadff3',
    );
    $handler->display->display_options['style_options']['title_position'] = '';
    $handler->display->display_options['style_options']['width'] = '';
    $handler->display->display_options['style_options']['height'] = '300';
    $handler->display->display_options['style_options']['xaxis_title'] = t('Date');
    $handler->display->display_options['style_options']['xaxis_labels_rotation'] = '30';
    $handler->display->display_options['style_options']['yaxis_title'] = t('Sessions and Pageviews');
    $handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
    $handler->display->display_options['defaults']['style_options'] = FALSE;
    $handler->display->display_options['defaults']['row_plugin'] = FALSE;
    $handler->display->display_options['defaults']['row_options'] = FALSE;
    $handler->display->display_options['defaults']['header'] = FALSE;
    /* Header: Global: Unfiltered text */
    $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
    $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
    $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
    $handler->display->display_options['header']['area_text_custom']['label'] = t('Sessions and Pageviews over the past 30 days');
    $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
    $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Sessions and Pageviews over the past 30 days') . '</h2>';
    $handler->display->display_options['defaults']['empty'] = FALSE;
    /* No results behavior: Global: Unfiltered text */
    $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
    $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['label'] = t('No data to display.');
    $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
    $handler->display->display_options['empty']['area_text_custom']['content'] = t('No data to display.');
    $handler->display->display_options['defaults']['fields'] = FALSE;
    /* Field: Time dimensions: Date */
    $handler->display->display_options['fields']['date']['id'] = 'date';
    $handler->display->display_options['fields']['date']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['date']['field'] = 'date';
    $handler->display->display_options['fields']['date']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['date']['date_format'] = 'custom';
    $handler->display->display_options['fields']['date']['custom_date_format'] = 'j M';
    $handler->display->display_options['fields']['date']['second_date_format'] = 'long';
    /* Field: Session metrics: Sessions */
    $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
    $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
    $handler->display->display_options['fields']['sessions']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['sessions']['precision'] = '0';
    $handler->display->display_options['fields']['sessions']['separator'] = ' ';
    /* Field: Page tracking metrics: Pageviews */
    $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['pageviews']['precision'] = '0';
    $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
    $handler->display->display_options['defaults']['sorts'] = FALSE;
    /* Sort criterion: Time dimensions: Date */
    $handler->display->display_options['sorts']['date']['id'] = 'date';
    $handler->display->display_options['sorts']['date']['table'] = 'google_analytics';
    $handler->display->display_options['sorts']['date']['field'] = 'date';
    $handler->display->display_options['displays'] = array(
      'page' => 'page',
      'default' => 0,
      'block_1' => 0,
    );
  }

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = t('Sessions and Pageviews over the last 30 days');
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'google-analytics-reports-summary-block';
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = TRUE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'Full report';
  $handler->display->display_options['defaults']['link_display'] = FALSE;
  $handler->display->display_options['link_display'] = 'page';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['default_row_class'] = FALSE;
  $handler->display->display_options['style_options']['row_class_special'] = FALSE;
  $handler->display->display_options['style_options']['type'] = 'ol';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['inline'] = array(
    'pageTitle' => 'pageTitle',
    'pageviews' => 'pageviews',
  );
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  if (module_exists('charts')) {
    /* Header: Global: View area */
    $handler->display->display_options['header']['view']['id'] = 'view';
    $handler->display->display_options['header']['view']['table'] = 'views';
    $handler->display->display_options['header']['view']['field'] = 'view';
    $handler->display->display_options['header']['view']['label'] = t('Block Chart');
    $handler->display->display_options['header']['view']['view_to_insert'] = 'google_analytics_reports_summary:attachment_block_chart';
  }
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('Top Pages');
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h3>' . t('Top Pages') . '</h3>';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t("Site doesn't have any statistics.");
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t("Site doesn't have any statistics.");
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Page tracking dimensions: Page */
  $handler->display->display_options['fields']['pagePath']['id'] = 'pagePath';
  $handler->display->display_options['fields']['pagePath']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pagePath']['field'] = 'pagePath';
  $handler->display->display_options['fields']['pagePath']['label'] = '';
  $handler->display->display_options['fields']['pagePath']['exclude'] = TRUE;
  $handler->display->display_options['fields']['pagePath']['element_label_colon'] = FALSE;
  /* Field: Page tracking dimensions: Page Title */
  $handler->display->display_options['fields']['pageTitle']['id'] = 'pageTitle';
  $handler->display->display_options['fields']['pageTitle']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageTitle']['field'] = 'pageTitle';
  $handler->display->display_options['fields']['pageTitle']['label'] = '';
  $handler->display->display_options['fields']['pageTitle']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['pageTitle']['alter']['text'] = '<a href="[pagePath]">[pageTitle]</a>';
  $handler->display->display_options['fields']['pageTitle']['element_label_colon'] = FALSE;
  /* Field: Page tracking metrics: Pageviews */
  $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['label'] = '';
  $handler->display->display_options['fields']['pageviews']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['pageviews']['precision'] = '0';
  $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
  $handler->display->display_options['fields']['pageviews']['prefix'] = '- ';
  $handler->display->display_options['fields']['pageviews']['suffix'] = ' views';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Page tracking metrics: Pageviews */
  $handler->display->display_options['sorts']['pageviews']['id'] = 'pageviews';
  $handler->display->display_options['sorts']['pageviews']['table'] = 'google_analytics';
  $handler->display->display_options['sorts']['pageviews']['field'] = 'pageviews';
  $handler->display->display_options['sorts']['pageviews']['order'] = 'DESC';
  $handler->display->display_options['block_description'] = t('Google Analytics Reports Summary Block');

  if (module_exists('charts')) {
    /* Display: Block Chart */
    $handler = $view->new_display('attachment', 'Block Chart', 'attachment_block_chart');
    $handler->display->display_options['defaults']['title'] = FALSE;
    $handler->display->display_options['defaults']['css_class'] = FALSE;
    $handler->display->display_options['css_class'] = 'google-analytics-reports-summary-block google-analytics-reports-summary-block-chart';
    $handler->display->display_options['pager']['type'] = 'none';
    $handler->display->display_options['pager']['options']['offset'] = '0';
    $handler->display->display_options['defaults']['style_plugin'] = FALSE;
    $handler->display->display_options['style_plugin'] = 'chart';
    $handler->display->display_options['style_options']['type'] = 'area';
    $handler->display->display_options['style_options']['library'] = '';
    $handler->display->display_options['style_options']['label_field'] = 'date';
    $handler->display->display_options['style_options']['data_fields'] = array(
      'sessions' => 'sessions',
      'pageviews' => 'pageviews',
      'date' => 0,
    );
    $handler->display->display_options['style_options']['field_colors'] = array(
      'date' => '#2f7ed8',
      'sessions' => '#058dc7',
      'pageviews' => '#aadff3',
    );
    $handler->display->display_options['style_options']['title_position'] = '';
    $handler->display->display_options['style_options']['legend_position'] = '';
    $handler->display->display_options['style_options']['width'] = '';
    $handler->display->display_options['style_options']['height'] = '200';
    $handler->display->display_options['style_options']['xaxis_title'] = t('Date');
    $handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
    $handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
    $handler->display->display_options['defaults']['style_options'] = FALSE;
    $handler->display->display_options['defaults']['row_plugin'] = FALSE;
    $handler->display->display_options['defaults']['row_options'] = FALSE;
    $handler->display->display_options['defaults']['empty'] = FALSE;
    /* No results behavior: Global: Unfiltered text */
    $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
    $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['label'] = t('No data to display.');
    $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
    $handler->display->display_options['empty']['area_text_custom']['content'] = t('No data to display.');
    $handler->display->display_options['defaults']['fields'] = FALSE;
    /* Field: Time dimensions: Date */
    $handler->display->display_options['fields']['date']['id'] = 'date';
    $handler->display->display_options['fields']['date']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['date']['field'] = 'date';
    $handler->display->display_options['fields']['date']['date_format'] = 'custom';
    $handler->display->display_options['fields']['date']['custom_date_format'] = 'j M';
    $handler->display->display_options['fields']['date']['second_date_format'] = 'long';
    /* Field: Session metrics: Sessions */
    $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
    $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
    $handler->display->display_options['fields']['sessions']['precision'] = '0';
    $handler->display->display_options['fields']['sessions']['separator'] = '';
    /* Field: Page tracking metrics: Pageviews */
    $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['precision'] = '0';
    $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
    $handler->display->display_options['defaults']['sorts'] = FALSE;
    /* Sort criterion: Time dimensions: Date */
    $handler->display->display_options['sorts']['date']['id'] = 'date';
    $handler->display->display_options['sorts']['date']['table'] = 'google_analytics';
    $handler->display->display_options['sorts']['date']['field'] = 'date';
    $handler->display->display_options['displays'] = array(
      'default' => 0,
      'page' => 0,
      'block_1' => 0,
    );
  }

  $views[$view->name] = $view;

  // Google Analytics Reports Page view.
  $view = new view();
  $view->name = 'google_analytics_reports_page';
  $view->description = t('Display statistic information about website pages from Google Analytics.');
  $view->tag = t('Google Analytics Reports');
  $view->base_table = 'google_analytics';
  $view->human_name = t('Google Analytics Reports Page');
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = t('Google Analytics Reports Page');
  $handler->display->display_options['css_class'] = 'google-analytics-reports-pages';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access google analytics reports';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: Page tracking metrics: Pageviews */
  $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
  $handler->display->display_options['fields']['pageviews']['element_type'] = 'strong';
  $handler->display->display_options['fields']['pageviews']['empty'] = '-';
  $handler->display->display_options['fields']['pageviews']['precision'] = '0';
  $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
  /* Field: Page tracking metrics: Unique Pageviews */
  $handler->display->display_options['fields']['uniquePageviews']['id'] = 'uniquePageviews';
  $handler->display->display_options['fields']['uniquePageviews']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['uniquePageviews']['field'] = 'uniquePageviews';
  $handler->display->display_options['fields']['uniquePageviews']['element_type'] = 'strong';
  $handler->display->display_options['fields']['uniquePageviews']['empty'] = '-';
  $handler->display->display_options['fields']['uniquePageviews']['precision'] = '0';
  $handler->display->display_options['fields']['uniquePageviews']['separator'] = ' ';
  /* Field: Page tracking calculated metrics: Avg. Time on Page */
  $handler->display->display_options['fields']['avgTimeOnPage']['id'] = 'avgTimeOnPage';
  $handler->display->display_options['fields']['avgTimeOnPage']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['avgTimeOnPage']['field'] = 'avgTimeOnPage';
  $handler->display->display_options['fields']['avgTimeOnPage']['element_type'] = 'strong';
  $handler->display->display_options['fields']['avgTimeOnPage']['empty'] = '-';
  $handler->display->display_options['fields']['avgTimeOnPage']['date_format'] = 'custom';
  $handler->display->display_options['fields']['avgTimeOnPage']['custom_date_format'] = 'i:s';
  $handler->display->display_options['fields']['avgTimeOnPage']['second_date_format'] = 'long';
  /* Field: Page tracking metrics: Entrances */
  $handler->display->display_options['fields']['entrances']['id'] = 'entrances';
  $handler->display->display_options['fields']['entrances']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['entrances']['field'] = 'entrances';
  $handler->display->display_options['fields']['entrances']['element_type'] = 'strong';
  $handler->display->display_options['fields']['entrances']['empty'] = '-';
  $handler->display->display_options['fields']['entrances']['precision'] = '0';
  $handler->display->display_options['fields']['entrances']['separator'] = ' ';
  /* Field: Session calculated metrics: Bounce Rate */
  $handler->display->display_options['fields']['bounceRate']['id'] = 'bounceRate';
  $handler->display->display_options['fields']['bounceRate']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['bounceRate']['field'] = 'bounceRate';
  $handler->display->display_options['fields']['bounceRate']['element_type'] = 'strong';
  $handler->display->display_options['fields']['bounceRate']['empty'] = '-';
  $handler->display->display_options['fields']['bounceRate']['set_precision'] = TRUE;
  $handler->display->display_options['fields']['bounceRate']['precision'] = '2';
  $handler->display->display_options['fields']['bounceRate']['separator'] = '';
  $handler->display->display_options['fields']['bounceRate']['suffix'] = '%';
  /* Field: Page tracking calculated metrics: % Exit */
  $handler->display->display_options['fields']['exitRate']['id'] = 'exitRate';
  $handler->display->display_options['fields']['exitRate']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['exitRate']['field'] = 'exitRate';
  $handler->display->display_options['fields']['exitRate']['label'] = t('Exit Rate');
  $handler->display->display_options['fields']['exitRate']['element_type'] = 'strong';
  $handler->display->display_options['fields']['exitRate']['empty'] = '-';
  $handler->display->display_options['fields']['exitRate']['set_precision'] = TRUE;
  $handler->display->display_options['fields']['exitRate']['precision'] = '2';
  $handler->display->display_options['fields']['exitRate']['separator'] = '';
  $handler->display->display_options['fields']['exitRate']['suffix'] = '%';
  /* Field: Page tracking metrics: Page Value */
  $handler->display->display_options['fields']['pageValue']['id'] = 'pageValue';
  $handler->display->display_options['fields']['pageValue']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['pageValue']['field'] = 'pageValue';
  $handler->display->display_options['fields']['pageValue']['element_type'] = 'strong';
  $handler->display->display_options['fields']['pageValue']['empty'] = '-';
  $handler->display->display_options['fields']['pageValue']['set_precision'] = TRUE;
  $handler->display->display_options['fields']['pageValue']['precision'] = '2';
  $handler->display->display_options['fields']['pageValue']['separator'] = '';
  $handler->display->display_options['fields']['pageValue']['prefix'] = '$';
  /* Contextual filter: Page tracking dimensions: Page */
  $handler->display->display_options['arguments']['pagePath']['id'] = 'pagePath';
  $handler->display->display_options['arguments']['pagePath']['table'] = 'google_analytics';
  $handler->display->display_options['arguments']['pagePath']['field'] = 'pagePath';
  $handler->display->display_options['arguments']['pagePath']['default_action'] = 'default';
  $handler->display->display_options['arguments']['pagePath']['title_enable'] = TRUE;
  $handler->display->display_options['arguments']['pagePath']['title'] = t('Google Analytics details for "%1" page');
  $handler->display->display_options['arguments']['pagePath']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['pagePath']['default_argument_options']['argument'] = '/';
  $handler->display->display_options['arguments']['pagePath']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['pagePath']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['pagePath']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Google Analytics: Start date of report */
  $handler->display->display_options['filters']['start_date']['id'] = 'start_date';
  $handler->display->display_options['filters']['start_date']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['start_date']['field'] = 'start_date';
  $handler->display->display_options['filters']['start_date']['value']['value'] = '-31 day';
  $handler->display->display_options['filters']['start_date']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['start_date']['group'] = 1;
  /* Filter criterion: Google Analytics: End date of report */
  $handler->display->display_options['filters']['end_date']['id'] = 'end_date';
  $handler->display->display_options['filters']['end_date']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['end_date']['field'] = 'end_date';
  $handler->display->display_options['filters']['end_date']['value']['value'] = '-1 day';
  $handler->display->display_options['filters']['end_date']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['end_date']['group'] = 1;

  /* Display: Page - Front */
  $handler = $view->new_display('page', 'Page - Front', 'page_front');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'pageviews' => 'pageviews',
    'uniquePageviews' => 'uniquePageviews',
    'avgTimeOnPage' => 'avgTimeOnPage',
    'entrances' => 'entrances',
    'bounceRate' => 'bounceRate',
    'exitRate' => 'exitRate',
    'pageValue' => 'pageValue',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'pageviews' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'uniquePageviews' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'avgTimeOnPage' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'entrances' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'bounceRate' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'exitRate' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'pageValue' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  if (module_exists('charts')) {
    /* Header: Global: View area */
    $handler->display->display_options['header']['view']['id'] = 'view';
    $handler->display->display_options['header']['view']['table'] = 'views';
    $handler->display->display_options['header']['view']['field'] = 'view';
    $handler->display->display_options['header']['view']['label'] = t('Chart');
    $handler->display->display_options['header']['view']['empty'] = TRUE;
    $handler->display->display_options['header']['view']['view_to_insert'] = 'google_analytics_reports_page:attachment_chart';
    $handler->display->display_options['header']['view']['inherit_arguments'] = TRUE;
  }
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('This page was viewed');
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('This page was viewed [pageviews] times') . '</h2>';
  $handler->display->display_options['header']['area_text_custom']['tokenize'] = TRUE;
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('This page was not viewed');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = '<h2>' . t('This page was not viewed') . '</h2>';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Page tracking dimensions: Page */
  $handler->display->display_options['arguments']['pagePath']['id'] = 'pagePath';
  $handler->display->display_options['arguments']['pagePath']['table'] = 'google_analytics';
  $handler->display->display_options['arguments']['pagePath']['field'] = 'pagePath';
  $handler->display->display_options['arguments']['pagePath']['default_action'] = 'default';
  $handler->display->display_options['arguments']['pagePath']['title_enable'] = TRUE;
  $handler->display->display_options['arguments']['pagePath']['title'] = t('Google Analytics details for front page');
  $handler->display->display_options['arguments']['pagePath']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['pagePath']['default_argument_options']['argument'] = '/';
  $handler->display->display_options['arguments']['pagePath']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['pagePath']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['pagePath']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['path'] = 'admin/reports/google-analytics-reports/page';

  /* Display: Page - Others */
  $handler = $view->new_display('page', 'Page - Others', 'page');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'pageviews' => 'pageviews',
    'uniquePageviews' => 'uniquePageviews',
    'avgTimeOnPage' => 'avgTimeOnPage',
    'entrances' => 'entrances',
    'bounceRate' => 'bounceRate',
    'exitRate' => 'exitRate',
    'pageValue' => 'pageValue',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'pageviews' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'uniquePageviews' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'avgTimeOnPage' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'entrances' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'bounceRate' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'exitRate' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
    'pageValue' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => 'views-align-center',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  if (module_exists('charts')) {
    /* Header: Global: View area */
    $handler->display->display_options['header']['view']['id'] = 'view';
    $handler->display->display_options['header']['view']['table'] = 'views';
    $handler->display->display_options['header']['view']['field'] = 'view';
    $handler->display->display_options['header']['view']['label'] = t('Chart');
    $handler->display->display_options['header']['view']['empty'] = TRUE;
    $handler->display->display_options['header']['view']['view_to_insert'] = 'google_analytics_reports_page:attachment_chart';
    $handler->display->display_options['header']['view']['inherit_arguments'] = TRUE;
  }
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('This page was viewed');
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' .  t('This page was viewed [pageviews] times') . '</h2>';
  $handler->display->display_options['header']['area_text_custom']['tokenize'] = TRUE;
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('This page was not viewed');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = '<h2>' . t('This page was not viewed') . '</h2>';
  $handler->display->display_options['path'] = 'admin/reports/google-analytics-reports/page/%';

  /* Display: Top Referrals */
  $handler = $view->new_display('attachment', 'Top Referrals', 'attachment_top_referrals');
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'google-analytics-reports-pages google-analytics-reports-pages-top-referrals';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['type'] = 'ol';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('Top Referrals');
  $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Top Referrals') . '</h2>';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('Referrals not found.');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t('Referrals not found.');
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Traffic sources dimensions: Source */
  $handler->display->display_options['fields']['source']['id'] = 'source';
  $handler->display->display_options['fields']['source']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['source']['field'] = 'source';
  $handler->display->display_options['fields']['source']['label'] = '';
  $handler->display->display_options['fields']['source']['element_type'] = 'strong';
  $handler->display->display_options['fields']['source']['element_label_type'] = '0';
  $handler->display->display_options['fields']['source']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['source']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['source']['element_default_classes'] = FALSE;
  /* Field: Session metrics: Sessions */
  $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['label'] = '';
  $handler->display->display_options['fields']['sessions']['element_type'] = '0';
  $handler->display->display_options['fields']['sessions']['element_label_type'] = '0';
  $handler->display->display_options['fields']['sessions']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sessions']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['sessions']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['sessions']['precision'] = '0';
  $handler->display->display_options['fields']['sessions']['separator'] = ' ';
  $handler->display->display_options['fields']['sessions']['prefix'] = '- ';
  $handler->display->display_options['fields']['sessions']['suffix'] = ' sessions';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Session metrics: Sessions */
  $handler->display->display_options['sorts']['sessions']['id'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['sorts']['sessions']['field'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['order'] = 'DESC';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Google Analytics: Start date of report */
  $handler->display->display_options['filters']['start_date']['id'] = 'start_date';
  $handler->display->display_options['filters']['start_date']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['start_date']['field'] = 'start_date';
  $handler->display->display_options['filters']['start_date']['value']['value'] = '-31 day';
  $handler->display->display_options['filters']['start_date']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['start_date']['group'] = 1;
  /* Filter criterion: Google Analytics: End date of report */
  $handler->display->display_options['filters']['end_date']['id'] = 'end_date';
  $handler->display->display_options['filters']['end_date']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['end_date']['field'] = 'end_date';
  $handler->display->display_options['filters']['end_date']['value']['value'] = '-1 day';
  $handler->display->display_options['filters']['end_date']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['end_date']['group'] = 1;
  /* Filter criterion: Traffic sources dimensions: Medium */
  $handler->display->display_options['filters']['medium']['id'] = 'medium';
  $handler->display->display_options['filters']['medium']['table'] = 'google_analytics';
  $handler->display->display_options['filters']['medium']['field'] = 'medium';
  $handler->display->display_options['filters']['medium']['value'] = 'referral';
  $handler->display->display_options['displays'] = array(
    'page_front' => 'page_front',
    'page' => 'page',
    'default' => 0,
    'block' => 0,
  );
  $handler->display->display_options['attachment_position'] = 'after';

  /* Display: Top Keywords */
  $handler = $view->new_display('attachment', 'Top Keywords', 'attachment_top_keywords');
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'google-analytics-reports-pages google-analytics-reports-pages-top-keywords';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['type'] = 'ol';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  /* Header: Global: Unfiltered text */
  $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['header']['area_text_custom']['label'] = t('Top Keywords');
  $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Top Keywords') . '</h2>';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t('Keywords not found.');
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t('Keywords not found.');
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Traffic sources dimensions: Keyword */
  $handler->display->display_options['fields']['keyword']['id'] = 'keyword';
  $handler->display->display_options['fields']['keyword']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['keyword']['field'] = 'keyword';
  $handler->display->display_options['fields']['keyword']['label'] = '';
  $handler->display->display_options['fields']['keyword']['element_type'] = 'strong';
  $handler->display->display_options['fields']['keyword']['element_label_type'] = '0';
  $handler->display->display_options['fields']['keyword']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['keyword']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['keyword']['element_default_classes'] = FALSE;
  /* Field: Session metrics: Sessions */
  $handler->display->display_options['fields']['sessions']['id'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['fields']['sessions']['field'] = 'sessions';
  $handler->display->display_options['fields']['sessions']['label'] = '';
  $handler->display->display_options['fields']['sessions']['element_type'] = '0';
  $handler->display->display_options['fields']['sessions']['element_label_type'] = '0';
  $handler->display->display_options['fields']['sessions']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sessions']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['sessions']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['sessions']['precision'] = '0';
  $handler->display->display_options['fields']['sessions']['separator'] = ' ';
  $handler->display->display_options['fields']['sessions']['prefix'] = ' - ';
  $handler->display->display_options['fields']['sessions']['suffix'] = ' sessions';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Session metrics: Sessions */
  $handler->display->display_options['sorts']['sessions']['id'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['table'] = 'google_analytics';
  $handler->display->display_options['sorts']['sessions']['field'] = 'sessions';
  $handler->display->display_options['sorts']['sessions']['order'] = 'DESC';
  $handler->display->display_options['displays'] = array(
    'page_front' => 'page_front',
    'page' => 'page',
    'default' => 0,
    'block' => 0,
  );
  $handler->display->display_options['attachment_position'] = 'after';

  if (module_exists('charts')) {
    /* Display: Chart */
    $handler = $view->new_display('attachment', 'Chart', 'attachment_chart');
    $handler->display->display_options['defaults']['title'] = FALSE;
    $handler->display->display_options['defaults']['css_class'] = FALSE;
    $handler->display->display_options['css_class'] = 'google-analytics-reports-pages google-analytics-reports-pages-chart';
    $handler->display->display_options['pager']['type'] = 'none';
    $handler->display->display_options['pager']['options']['offset'] = '0';
    $handler->display->display_options['defaults']['style_plugin'] = FALSE;
    $handler->display->display_options['style_plugin'] = 'chart';
    $handler->display->display_options['style_options']['type'] = 'area';
    $handler->display->display_options['style_options']['library'] = '';
    $handler->display->display_options['style_options']['label_field'] = 'date';
    $handler->display->display_options['style_options']['data_fields'] = array(
      'pageviews' => 'pageviews',
      'date' => 0,
    );
    $handler->display->display_options['style_options']['field_colors'] = array(
      'date' => '#0d233a',
      'pageviews' => '#058dc7',
    );
    $handler->display->display_options['style_options']['title_position'] = '';
    $handler->display->display_options['style_options']['width'] = '';
    $handler->display->display_options['style_options']['height'] = '300';
    $handler->display->display_options['style_options']['xaxis_title'] = t('Date');
    $handler->display->display_options['style_options']['xaxis_labels_rotation'] = '30';
    $handler->display->display_options['style_options']['yaxis_title'] = t('Pageviews');
    $handler->display->display_options['style_options']['yaxis_min'] = '0';
    $handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
    $handler->display->display_options['defaults']['style_options'] = FALSE;
    $handler->display->display_options['defaults']['row_plugin'] = FALSE;
    $handler->display->display_options['defaults']['row_options'] = FALSE;
    $handler->display->display_options['defaults']['header'] = FALSE;
    /* Header: Global: Unfiltered text */
    $handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
    $handler->display->display_options['header']['area_text_custom']['table'] = 'views';
    $handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
    $handler->display->display_options['header']['area_text_custom']['label'] = t('Pageviews over the past 30 days');
    $handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
    $handler->display->display_options['header']['area_text_custom']['content'] = '<h2>' . t('Pageviews over the past 30 days') . '</h2>';
    $handler->display->display_options['defaults']['empty'] = FALSE;
    /* No results behavior: Global: Unfiltered text */
    $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
    $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['label'] = t('No data to display.');
    $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
    $handler->display->display_options['empty']['area_text_custom']['content'] = t('No data to display.');
    $handler->display->display_options['defaults']['fields'] = FALSE;
    /* Field: Time dimensions: Date */
    $handler->display->display_options['fields']['date']['id'] = 'date';
    $handler->display->display_options['fields']['date']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['date']['field'] = 'date';
    $handler->display->display_options['fields']['date']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['date']['date_format'] = 'custom';
    $handler->display->display_options['fields']['date']['custom_date_format'] = 'j M';
    $handler->display->display_options['fields']['date']['second_date_format'] = 'long';
    /* Field: Page tracking metrics: Pageviews */
    $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['element_label_colon'] = FALSE;
    $handler->display->display_options['fields']['pageviews']['empty'] = '0';
    $handler->display->display_options['fields']['pageviews']['precision'] = '0';
    $handler->display->display_options['fields']['pageviews']['separator'] = '';
    $handler->display->display_options['defaults']['sorts'] = FALSE;
    /* Sort criterion: Time dimensions: Date */
    $handler->display->display_options['sorts']['date']['id'] = 'date';
    $handler->display->display_options['sorts']['date']['table'] = 'google_analytics';
    $handler->display->display_options['sorts']['date']['field'] = 'date';
    $handler->display->display_options['displays'] = array(
      'default' => 0,
      'page' => 0,
      'block' => 0,
    );
  }

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = t('Page traffic over the past 30 days');
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = TRUE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = t('Full report for this page');
  $handler->display->display_options['defaults']['link_display'] = FALSE;
  $handler->display->display_options['link_display'] = 'custom_url';
  $handler->display->display_options['link_url'] = 'admin/reports/google-analytics-reports/page!1';
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = t("Page doesn't have any statistics.");
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = t("Page doesn't have any statistics.");
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Page tracking dimensions: Page */
  $handler->display->display_options['arguments']['pagePath']['id'] = 'pagePath';
  $handler->display->display_options['arguments']['pagePath']['table'] = 'google_analytics';
  $handler->display->display_options['arguments']['pagePath']['field'] = 'pagePath';
  $handler->display->display_options['arguments']['pagePath']['default_action'] = 'default';
  $handler->display->display_options['arguments']['pagePath']['default_argument_type'] = 'google_analytics_reports_path';
  $handler->display->display_options['arguments']['pagePath']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['pagePath']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['pagePath']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['block_description'] = t('Google Analytics Reports Page Block');

  if (module_exists('charts')) {
    /* Display: Block Chart */
    $handler = $view->new_display('attachment', 'Block Chart', 'attachment_block_chart');
    $handler->display->display_options['pager']['type'] = 'none';
    $handler->display->display_options['pager']['options']['offset'] = '0';
    $handler->display->display_options['defaults']['style_plugin'] = FALSE;
    $handler->display->display_options['style_plugin'] = 'chart';
    $handler->display->display_options['style_options']['type'] = 'area';
    $handler->display->display_options['style_options']['library'] = '';
    $handler->display->display_options['style_options']['label_field'] = 'date';
    $handler->display->display_options['style_options']['data_fields'] = array(
      'pageviews' => 'pageviews',
      'date' => 0,
    );
    $handler->display->display_options['style_options']['field_colors'] = array(
      'date' => '#2f7ed8',
      'pageviews' => '#058dc7',
    );
    $handler->display->display_options['style_options']['title_position'] = '';
    $handler->display->display_options['style_options']['legend_position'] = 'top';
    $handler->display->display_options['style_options']['width'] = '';
    $handler->display->display_options['style_options']['height'] = '200';
    $handler->display->display_options['style_options']['xaxis_title'] = t('Date');
    $handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
    $handler->display->display_options['style_options']['yaxis_min'] = '0';
    $handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
    $handler->display->display_options['defaults']['style_options'] = FALSE;
    $handler->display->display_options['defaults']['row_plugin'] = FALSE;
    $handler->display->display_options['defaults']['row_options'] = FALSE;
    $handler->display->display_options['defaults']['empty'] = FALSE;
    /* No results behavior: Global: Unfiltered text */
    $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
    $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
    $handler->display->display_options['empty']['area_text_custom']['label'] = t('No data to display.');
    $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
    $handler->display->display_options['empty']['area_text_custom']['content'] = t('No data to display.');
    $handler->display->display_options['defaults']['fields'] = FALSE;
    /* Field: Time dimensions: Date */
    $handler->display->display_options['fields']['date']['id'] = 'date';
    $handler->display->display_options['fields']['date']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['date']['field'] = 'date';
    $handler->display->display_options['fields']['date']['date_format'] = 'custom';
    $handler->display->display_options['fields']['date']['custom_date_format'] = 'j M';
    $handler->display->display_options['fields']['date']['second_date_format'] = 'long';
    /* Field: Page tracking metrics: Pageviews */
    $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['element_type'] = 'strong';
    $handler->display->display_options['fields']['pageviews']['empty'] = '-';
    $handler->display->display_options['fields']['pageviews']['precision'] = '0';
    $handler->display->display_options['fields']['pageviews']['separator'] = ' ';
    $handler->display->display_options['defaults']['sorts'] = FALSE;
    /* Sort criterion: Time dimensions: Date */
    $handler->display->display_options['sorts']['date']['id'] = 'date';
    $handler->display->display_options['sorts']['date']['table'] = 'google_analytics';
    $handler->display->display_options['sorts']['date']['field'] = 'date';
    $handler->display->display_options['defaults']['arguments'] = FALSE;
    /* Contextual filter: Page tracking dimensions: Page */
    $handler->display->display_options['arguments']['pagePath']['id'] = 'pagePath';
    $handler->display->display_options['arguments']['pagePath']['table'] = 'google_analytics';
    $handler->display->display_options['arguments']['pagePath']['field'] = 'pagePath';
    $handler->display->display_options['arguments']['pagePath']['default_action'] = 'default';
    $handler->display->display_options['arguments']['pagePath']['title_enable'] = TRUE;
    $handler->display->display_options['arguments']['pagePath']['title'] = t('Google Analytics details for "%1" page');
    $handler->display->display_options['arguments']['pagePath']['default_argument_type'] = 'google_analytics_reports_path';
    $handler->display->display_options['arguments']['pagePath']['summary']['number_of_records'] = '0';
    $handler->display->display_options['arguments']['pagePath']['summary']['format'] = 'default_summary';
    $handler->display->display_options['arguments']['pagePath']['summary_options']['items_per_page'] = '25';
    $handler->display->display_options['displays'] = array(
      'block' => 'block',
      'default' => 0,
      'page' => 0,
    );
  }

  $views[$view->name] = $view;

  return $views;
}
