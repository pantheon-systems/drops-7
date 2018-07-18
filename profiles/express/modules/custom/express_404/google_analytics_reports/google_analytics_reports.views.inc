<?php

/**
 * @file
 * Provide views data and handlers for Google Analytics Core Reporting API.
 */

/**
 * Implements hook_views_data().
 */
function google_analytics_reports_views_data() {
  $data['google_analytics']['table']['group']  = t('Google Analytics');

  $data['google_analytics']['table']['base'] = array(
    'title' => t('Google Analytics'),
    'query class' => 'google_analytics_reports_query',
    'help' => t('Views Google Analytics query builder'),
  );

  $data['google_analytics']['start_date'] = array(
    'title' => t('Start date of report'),
    'help' => t('Start date of report.'),
    'argument' => array(
      'handler' => 'google_analytics_reports_handler_argument',
    ),
    'filter' => array(
      'handler' => 'google_analytics_reports_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  $data['google_analytics']['end_date'] = array(
    'title' => t('End date of report'),
    'help' => t('End date of report.'),
    'argument' => array(
      'handler' => 'google_analytics_reports_handler_argument',
    ),
    'filter' => array(
      'handler' => 'google_analytics_reports_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  $data['google_analytics']['profile_id'] = array(
    'title' => t('Profile ID'),
    'help' => t('Profile ID'),
    'argument' => array(
      'handler' => 'google_analytics_reports_handler_argument',
    ),
    'filter' => array(
      'handler' => 'google_analytics_reports_handler_filter_string',
    ),
  );

  $fields = google_analytics_reports_get_fields();

  foreach ($fields as $field_name => $field) {
    // Description of filed from Google Analytics.
    $field->description = t($field->description) . '<br />' . t('Type: @type.', array('@type' => $field->type));
    if (isset($field->calculation)) {
      $field->description .= '<br />' . t('Calculation: <code>@formula</code>.',
          array('@formula' => $field->calculation));
    }
    $field->description .= '<br />' . t('API name: <code>@ga</code>.',
        array('@ga' => 'ga:' . $field_name));

    // Provide default handler.
    $field_handler = 'views_handler_field';
    $float = FALSE;

    if (google_analytics_reports_is_custom($field_name)) {
      $field_handler = 'google_analytics_reports_handler_field';
    }
    elseif (in_array($field->data_type, array('date', 'time'))) {
      $field_handler = 'views_handler_field_date';
    }
    elseif (in_array($field->data_type,
      array('integer', 'float', 'percent', 'currency'))) {

      $field_handler = 'views_handler_field_numeric';
      $float = TRUE;
    }
    $data['google_analytics'][$field_name] = array(
      'title' => t($field->ui_name),
      'help' => $field->description,
      'group' => t($field->column_group),
      'field' => array(
        'handler' => $field_handler,
        'click sortable' => TRUE,
        'float' => $float,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'argument' => array(
        'handler' => 'google_analytics_reports_handler_argument',
      ),
      'filter' => array(
        'handler' => ($field->type == 'metric') ? 'google_analytics_reports_handler_filter_numeric' : 'google_analytics_reports_handler_filter_string',
      ),
    );
  }

  return $data;
}

/**
 * Implements hook_views_plugins().
 */
function google_analytics_reports_views_plugins() {
  return array(
    'query' => array(
      'google_analytics_reports_query' => array(
        'title' => t('Google Analytics Query'),
        'handler' => 'google_analytics_reports_plugin_query_google_analytics',
      ),
    ),
    'argument default' => array(
      'google_analytics_reports_path' => array(
        'title' => t('Current page path (Google Analytics Reports)'),
        'handler' => 'google_analytics_reports_plugin_argument_default_google_analytics_reports_path',
      ),
    ),
  );
}
