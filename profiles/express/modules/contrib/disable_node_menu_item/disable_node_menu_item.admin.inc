<?php
/**
 * @file
 * This file provides admin functionality for the disable node menu item module
 */

/**
 * Admin settings form callback
 */
function disable_node_menu_item_settings($form, &$form_state) {
  $options = array();
  foreach (node_type_get_types() as $type) {
    $options[$type->type] = $type->type;
  }

  $form['dnmi_content_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content Type Selection'),
    '#default_value' => variable_get('dnmi_content_types', array()),
    '#options' => $options,
    '#description' => t('This setting determines what content types the disable node menu item functionality will be included on')
  );

  return system_settings_form($form);
}
