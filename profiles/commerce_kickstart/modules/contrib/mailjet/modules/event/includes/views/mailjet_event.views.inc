<?php
/**
 * @file
 *   Adds views support for mailjet events
 */

/**
 * Implements hook_views_data()
 */
function mailjet_event_views_data() {
  $data = array();
  $data['mailjet_event']['event'] = array(
    'field' => array(
      'title' => t('Event'),
      'help' => t('The Mailjet Event.'),
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  return $data;
}
