<?php

/**
 * Main hook used to add dashboards to the admin/dashboard page.
 *
 * @return array $variables
 *   An array of configuration options used to integrate custom dashboards into
 *   the dashboard page.
 *
 *   'title' - Title that is displayed as tab title and page title when the tab
 *     is selected.
 *
 *   'callback' - Function that returns the output for the dashboard.
 *
 *   'weight' - Used to determine where the tab appears on the dashboards page.
 *     Lower weights appear more to the left of the screen.
 *
 *   'access arguments' - Permissions that control who can see/use a dashboard.
 */
function hook_express_dashboard($variables) {
  $variables['user'] = array(
    'title' => 'User Dashboard',
    'callback' => 'express_dashboard_user_content',
    'weight' => -99,
    'access arguments' => array('use user dashboard'),
  );
  $variables['seo'] = array(
    'title' => 'SEO Dashboard',
    'callback' => 'express_dashboard_seo_content',
    'access arguments' => array('use seo dashboard'),
  );

  return $variables;
}
