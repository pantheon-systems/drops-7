<?php

/**
 * Implements hook_views_data_alter().
 */
function commerce_message_views_data_alter(&$data) {
  $data['message']['add'] = array(
    'title' => t('Add new message'),
    'help' => t('Display a form to add a new message.'),
    'area' => array(
      'handler' => 'commerce_message_handler_area_add_message',
    ),
  );
}
