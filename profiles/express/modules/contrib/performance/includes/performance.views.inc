<?php

/**
 * @file
 * Views integration for the Performance module.
 */

/**
 * Implements hook_views_data().
 */
function performance_views_data() {
  // The 'group' index will be used as a prefix in the UI for any of this
  // table's fields, sort criteria, etc. so it's easy to tell where they came
  // from.
  // Find it a bit silly in the performance logging context but it's mandatory.
  $data['performance_detail']['table']['group'] = t('Performance logging details');

  $data['performance_detail']['table']['base'] = array(
    'field' => 'pid',
    'title' => t('Performance logging details'),
    'weight' => -10,
  );

  // Path ID field.
  $data['performance_detail']['pid'] = array(
    'title' => t('Path ID'),
    'help' => t('Serialised ID field.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Last access field.
  $data['performance_detail']['timestamp'] = array(
    'title' => t('Timestamp'),
    'help' => t('Time when this data was recorded.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Bytes field.
  $data['performance_detail']['bytes'] = array(
    'title' => t('Bytes'),
    'help' => t('Amount of memory consumed in bytes when rendering the page.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Bytes field.
  $data['performance_detail']['ms'] = array(
    'title' => t('Milliseconds'),
    'help' => t('Amount of milliseconds needed to render this page.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Query count field.
  $data['performance_detail']['query_count'] = array(
    'title' => t('Query count'),
    'help' => t('Amount of queries executed when rendering this page.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Query timer field.
  $data['performance_detail']['query_timer'] = array(
    'title' => t('Query timer'),
    'help' => t('Amount of milliseconds spent on queries for this page.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Anonymous user field.
  $data['performance_detail']['anon'] = array(
    'title' => t('Anonymous user?'),
    'help' => t('Whether the access was by an anonymous user or not.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Anonymous?'),
      'type' => 'yes-no',
      // use boolean_field = 1 instead of boolean_field <> 0 in WHERE statment
      'use equal' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Path field.
  $data['performance_detail']['path'] = array(
    'title' => t('Path'),
    'help' => t('The path of the page for which detailed performance logging was stored.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}