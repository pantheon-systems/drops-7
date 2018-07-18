<?php

function express_404_views_default_views() {
  if (module_exists('google_analytics_reports')) {
    $view = new view();
    $view->name = '404_pages';
    $view->description = 'Pageviews are from the last 30 days.';
    $view->tag = 'default';
    $view->base_table = 'google_analytics';
    $view->human_name = '404 Pages';
    $view->core = 7;
    $view->api_version = '3.0';
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

    /* Display: Master */
    $handler = $view->new_display('default', 'Master', 'default');
    $handler->display->display_options['title'] = '404 Pages';
    $handler->display->display_options['use_more_always'] = FALSE;
    $handler->display->display_options['access']['type'] = 'perm';
    $handler->display->display_options['access']['perm'] = 'access google analytics reports';
    $handler->display->display_options['cache']['type'] = 'none';
    $handler->display->display_options['query']['type'] = 'views_query';
    $handler->display->display_options['exposed_form']['type'] = 'basic';
    $handler->display->display_options['pager']['type'] = 'some';
    $handler->display->display_options['pager']['options']['items_per_page'] = '5';
    $handler->display->display_options['style_plugin'] = 'default';
    $handler->display->display_options['row_plugin'] = 'fields';
    /* Footer: Global: Text area */
    $handler->display->display_options['footer']['area']['id'] = 'area';
    $handler->display->display_options['footer']['area']['table'] = 'views';
    $handler->display->display_options['footer']['area']['field'] = 'area';
    $handler->display->display_options['footer']['area']['content'] = '<p>Views are from the last 30 days.</p>
  <hr>
  <p>The pages listed above should be redirected to the appropriate, published pages to prevent visitors arriving on "not found" pages. This can be done by creating a redirect by clicking the button below.</p>
  <h4><a class="btn btn-primary" href="admin/config/search/redirect/add" target="_blank">Create a redirect</a></h4>';
    $handler->display->display_options['footer']['area']['format'] = 'wysiwyg';
    /* Field: Page Tracking: Pageviews */
    $handler->display->display_options['fields']['pageviews']['id'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['pageviews']['field'] = 'pageviews';
    $handler->display->display_options['fields']['pageviews']['exclude'] = TRUE;
    $handler->display->display_options['fields']['pageviews']['precision'] = '0';
    /* Field: Page Tracking: Page */
    $handler->display->display_options['fields']['pagePath']['id'] = 'pagePath';
    $handler->display->display_options['fields']['pagePath']['table'] = 'google_analytics';
    $handler->display->display_options['fields']['pagePath']['field'] = 'pagePath';
    $handler->display->display_options['fields']['pagePath']['label'] = '';
    $handler->display->display_options['fields']['pagePath']['alter']['alter_text'] = TRUE;
    $handler->display->display_options['fields']['pagePath']['alter']['text'] = '<p>[pagePath]: [pageviews] view(s)</p>';
    $handler->display->display_options['fields']['pagePath']['element_label_colon'] = FALSE;
    /* Sort criterion: Page Tracking: Pageviews */
    $handler->display->display_options['sorts']['pageviews']['id'] = 'pageviews';
    $handler->display->display_options['sorts']['pageviews']['table'] = 'google_analytics';
    $handler->display->display_options['sorts']['pageviews']['field'] = 'pageviews';
    $handler->display->display_options['sorts']['pageviews']['order'] = 'DESC';
    /* Filter criterion: Google Analytics: Start date of report */
    $handler->display->display_options['filters']['start_date']['id'] = 'start_date';
    $handler->display->display_options['filters']['start_date']['table'] = 'google_analytics';
    $handler->display->display_options['filters']['start_date']['field'] = 'start_date';
    $handler->display->display_options['filters']['start_date']['value']['value'] = '-31 day';
    $handler->display->display_options['filters']['start_date']['value']['type'] = 'offset';
    $handler->display->display_options['filters']['start_date']['group'] = 1;
    $handler->display->display_options['filters']['start_date']['expose']['operator'] = FALSE;
    /* Filter criterion: Google Analytics: End date of report */
    $handler->display->display_options['filters']['end_date']['id'] = 'end_date';
    $handler->display->display_options['filters']['end_date']['table'] = 'google_analytics';
    $handler->display->display_options['filters']['end_date']['field'] = 'end_date';
    $handler->display->display_options['filters']['end_date']['value']['value'] = '-1 day';
    $handler->display->display_options['filters']['end_date']['value']['type'] = 'offset';
    $handler->display->display_options['filters']['end_date']['group'] = 1;
    $handler->display->display_options['filters']['end_date']['expose']['operator'] = FALSE;
    /* Filter criterion: Page Tracking: Page Title */
    $handler->display->display_options['filters']['pageTitle']['id'] = 'pageTitle';
    $handler->display->display_options['filters']['pageTitle']['table'] = 'google_analytics';
    $handler->display->display_options['filters']['pageTitle']['field'] = 'pageTitle';
    $handler->display->display_options['filters']['pageTitle']['operator'] = 'contains';
    $handler->display->display_options['filters']['pageTitle']['value'] = 'not found';

    /* Display: Block */
    $handler = $view->new_display('block', 'Block', 'block');

    $views[$view->name] = $view;
  }

  return $views;
}