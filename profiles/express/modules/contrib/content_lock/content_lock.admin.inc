<?php /* -*- mode: php; indent-tabs-mode: nil; tab-width: 2; -*- */

# Copyright (c) 2010 Impressive.media
# Author: Eugen Mayer

/**
 * Providing an administration interface for content_lock.
 */
function content_lock_admin_settings() {
  $form = array();

  $form['#attached'] = array(
    'js' => array(
      drupal_get_path('module','content_lock') . '/content_lock.admin.js',
    ),
  );

  $form['content_lock_unload_js'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use javascript to detect leaving the node form'),
      '#description' => t('If you disable this, nodes will remain locked if the user navigates away from the node without saving it or clicking the cancel button.'),
      '#default_value' => variable_get('content_lock_unload_js', true),
  );
  $form['content_lock_unload_js_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Javascript detection options.'),
  );
  $form['content_lock_unload_js_fieldset']['content_lock_unload_js_message_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Confirm that the user really wants to leave the node form with a Javascript popup'),
    '#description' => t('This will cause a Javascript popup to ask the user if navigating away from the node edit page is intentional, giving the user an opportunity to cancel navigation so that the changes to the node may be saved. This will not appear if the user clicks the cancel button.'),
    '#default_value' => variable_get('content_lock_unload_js_message_enable', TRUE),
  );
  $form['content_lock_unload_js_fieldset']['content_lock_unload_js_message'] = array(
      '#type' => 'textfield',
      '#title' => t('Javascript popup message text'),
      '#description' => t('This option allows you to modify the text the user sees when navigating away from an edited node.'),
      '#default_value' => variable_get('content_lock_unload_js_message', 'If you proceed, ALL of your changes will be lost.'),
  );

  $form['content_lock_admin_verbose'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show lock / unlock messages'),
      '#description' => t('This option controls whether or not content_lock inform the user that he has locked/unlocked a node. This does not control the javascript dialogues.'),
      '#default_value' => variable_get('content_lock_admin_verbose', true),
  );

  $form['content_lock_admin_cancelbutton'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add cancel button'),
      '#description' => t('Adds a cancel button to the node / user / comment form. This way a user can properly cancel the transaction.'),
      '#default_value' => variable_get('content_lock_admin_cancelbutton', true),
  );

  $node_types = array();
  foreach (node_type_get_types() as $node_type_name => $node_type) {
    $node_types[$node_type_name] = $node_type->name;
  }
  $form['content_lock_allowed_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Lockable content types'),
    '#description' => t('Apply lock to the selected content types. Leave blank for all node types to be lockable.'),
    '#options' => $node_types,
    '#default_value' => variable_get('content_lock_allowed_node_types', array()),
  );

  $formats = filter_formats();
  $format_options = array();
  foreach ($formats as $id => $format) {
    $format_options[$id] = $format->name;
  }

  $form['content_lock_allowed_formats'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Lockable text formats'),
    '#description' => t('Apply lock to selected formats only. Leave blank for all node input formats to be lockable.'),
    '#options' => $format_options,
    '#default_value' => variable_get('content_lock_allowed_formats', array()),
  );

  return system_settings_form($form);
}
