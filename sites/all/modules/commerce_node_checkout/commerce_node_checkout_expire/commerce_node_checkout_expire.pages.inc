<?php

/**
 * @file
 *  Page callbacks and related functions.
 */

/**
 * The node expiration adjustment form.
 */
function commerce_node_checkout_expire_adjust_expiration_form($form, &$form_state, $node) {
  // Get the last line item for this node
  $line_item = commerce_node_checkout_expire_get_node_last_line_item($node);
  // Extract the entity language
  $language = entity_language('commerce_line_item', $line_item);
  // Set the field options for the form
  $options = array('field_name' => 'commerce_node_checkout_expires');
  // Attach the expiration field for the line item
  field_attach_form('commerce_line_item', $line_item, $form, $form_state, $language, $options);
  // Add a warning about unpublished nodes
  $form['warning'] = array(
    '#type' => 'fieldset',
    '#title' => t('Warning'),
    '#access' => ($node->status == NODE_NOT_PUBLISHED),
    'message' => array(
      '#markup' => t('This node is currently unpublished. Changing the expiration date will not publish it.'),
    ),
  );
  // Provide a submit button
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 1000,
  );
  return $form;
}

/**
 * Submit handler for the node expiration adjustment form.
 */
function commerce_node_checkout_expire_adjust_expiration_form_submit($form, &$form_state) {
  // Update and save the line item
  $line_item = $form['#entity'];
  $entity_type = 'commerce_line_item';
  entity_form_submit_build_entity($entity_type, $line_item, $form, $form_state);
  entity_save($entity_type, $line_item);

  // Alert the user
  drupal_set_message(t('The expiration date has been updated.'));
}
