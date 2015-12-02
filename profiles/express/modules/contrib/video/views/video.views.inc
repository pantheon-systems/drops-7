<?php

/**
 * @file
 * Provide views data and handlers for statistics.module
 */
/**
 * @defgroup views_statistics_module statistics.module handlers
 *
 * Includes the ability to create views of just the statistics table.
 * @{
 */

/**
 * Implements hook_views_data().
 */
function video_views_data() {
  // Basic table information.
  // ----------------------------------------------------------------
  // video_output table

  $data['video_queue']['table']['group'] = t('Video');

  $data['video_queue']['table']['join'] = array(
    // ...to the node table
    'node' => array(
      'left_field' => 'nid',
      'field' => 'entity_id',
    ),
    // ...to the file_managed table
    'file_managed' => array(
      'left_field' => 'fid',
      'field' => 'fid',
    ),
  );

  // dimensions
  $data['video_queue']['dimensions'] = array(
    'title' => t('Dimensions'),
    'help' => t('The widthXheight of the video.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // status
  $data['video_queue']['status'] = array(
    'title' => t('Status'),
    'help' => t('Video conversion status.'),
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

  // duration
  $data['video_queue']['duration'] = array(
    'title' => t('Duration'),
    'help' => t('Video playtime.'),
    'field' => array(
      'handler' => 'video_handler_field_video_duration',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // started
  $data['video_queue']['started'] = array(
    'title' => t('Video conversion start'),
    'help' => t('The time that conversion process started.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // completed
  $data['video_queue']['completed'] = array(
    'title' => t('Video conversion end'),
    'help' => t('The time that the conversion process completed.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}

/**
 * Implements hook_field_views_data().
 *
 * Views integration for video fields. Adds a video relationship to the default
 * field data.
 *
 * @see field_views_field_default_views_data()
 */
function video_field_views_data($field) {
  $data = field_views_field_default_views_data($field);
  foreach ($data as $table_name => $table_data) {
    // Add the relationship only on the fid field.
    $data[$table_name][$field['field_name'] . '_fid']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => 'file_managed',
      'base field' => 'fid',
      'label' => t('video from !field_name', array('!field_name' => $field['field_name'])),
    );
    $data[$table_name][$field['field_name'] . '_thumbnail']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => 'file_managed',
      'base field' => 'fid',
      'label' => t('video thumbnail from !field_name', array('!field_name' => $field['field_name'])),
    );
  }

  return $data;
}

/**
 * Implements hook_field_views_data_views_data_alter().
 *
 * Views integration to provide reverse relationships on video fields.
 */
function video_field_views_data_views_data_alter(&$data, $field) {
  foreach ($field['bundles'] as $entity_type => $bundles) {
    $entity_info = entity_get_info($entity_type);
    $pseudo_field_name = 'reverse_' . $field['field_name'] . '_' . $entity_type;

    list($label, $all_labels) = field_views_field_label($field['field_name']);
    $entity = $entity_info['label'];
    if ($entity == t('Node')) {
      $entity = t('Content');
    }

    $data['file_managed'][$pseudo_field_name]['relationship'] = array(
      'title' => t('@entity using @field', array('@entity' => $entity, '@field' => $label)),
      'help' => t('Relate each @entity with a @field set to the video.', array('@entity' => $entity, '@field' => $label)),
      'handler' => 'views_handler_relationship_entity_reverse',
      'field_name' => $field['field_name'],
      'field table' => _field_sql_storage_tablename($field),
      'field field' => $field['field_name'] . '_fid',
      'base' => $entity_info['base table'],
      'base field' => $entity_info['entity keys']['id'],
      'label' => t('!field_name', array('!field_name' => $field['field_name'])),
      'join_extra' => array(
        0 => array(
          'field' => 'entity_type',
          'value' => $entity_type,
        ),
        1 => array(
          'field' => 'deleted',
          'value' => 0,
          'numeric' => TRUE,
        ),
      ),
    );
  }
}
