<?php

/**
 * @file
 * Views wizard for Google Analytics views.
 */

$plugin = array(
  'name' => 'google_analytics',
  'base_table' => 'google_analytics',
  'title' => t('Google Analytics'),
  'available_sorts' => array(
    'sessions:DESC' => t('Sessions'),
    'users:DESC' => t('Users'),
    'pageviews:DESC' => t('Pageviews'),
    'date:DESC' => t('Date'),
  ),
  'form_wizard_class' => array(
    'file' => 'google_analytics_reports_google_analytics_views_wizard.class.php',
    'class' => 'GoogleAnalyticsReportsGoogleAnalyticsViewsWizard',
  ),
  'path_field' => array(
    'id' => 'pagePath',
    'table' => 'google_analytics',
    'field' => 'pagePath',
    'exclude' => TRUE,
  ),
);
