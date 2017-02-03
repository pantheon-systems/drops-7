<?php

/**
 * @file
 * Views include file.
 */

/**
 * Implements hook_views_data_alter().
 */
function administerusersbyrole_views_data_alter(&$data) {
  // Change the Views core handler that provides user edit and cancel links
  // so that they take account of our new permissions.
  $data['users']['edit_node']['field']['handler'] = 'administerusersbyrole_handler_field_user_link_edit';
  $data['users']['cancel_node']['field']['handler'] = 'administerusersbyrole_handler_field_user_link_cancel';
}
