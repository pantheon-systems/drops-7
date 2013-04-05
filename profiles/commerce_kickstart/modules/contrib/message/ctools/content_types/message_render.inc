<?php

/**
 * CTools Plugin definition.
 */
$plugin = array(
  'title' => t('Message render'),
  'description' => t('Render a message entity.'),
  'required context' => new ctools_context_required(t('Message'), 'entity:message'),
  'category' => t('Message'),
  'defaults' => array(
    'field_name' => MESSAGE_FIELD_MESSAGE_TEXT,
    'partials' => FALSE,
    'partial_delta' => 0,
  ),
);

/**
 * Render callback; Show a partial message.
 */
function message_message_render_content_type_render($subtype, $conf, $args, $context) {
  if (empty($context->data)) {
    return FALSE;
  }

  $message = $context->data;

  $block = new stdClass();
  $block->module = 'message';
  $block->title = '';

  $options = array(
    'field name' => $conf['field_name'],
    'partials' => $conf['partials'],
    'partial delta' => $conf['partials_delta'],
  );

  $block->content = $message->getText(NULL, $options);
  return $block;
}

/**
 * Edit form; Allow selecting a partial.
 */
function message_message_render_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $options = array();
  foreach (field_info_instances('message_type') as $bundle => $instances) {
    foreach ($instances as $field_name => $instance) {
      if (!empty($options[$field_name])) {
        continue;
      }
      $field = field_info_field($field_name);
      if (!in_array($field['type'], array('text_long', 'text'))) {
        continue;
      }
      $options[$field_name] = $instance['label'];
    }
  }
  // Get all the text fields attached to a message-type.
  $form['field_name'] = array(
    '#type' => 'select',
    '#title' => t('Field name'),
    '#description' => t('Select the field name to render.'),
    '#options' => $options,
    '#default_value' => $conf['field_name'],
    '#required' => TRUE,
  );

  // Get all the text fields attached to a message-type.
  $form['partials'] = array(
    '#type' => 'checkbox',
    '#title' => t('Partial'),
    '#description' => t('Render only a single delta out of the whole message (in case it is separated in multiple deltas).'),
    '#default_value' => $conf['partials'],
  );

  $form['partials_delta'] = array(
    '#type' => 'select',
    '#title' => t('Partial delta'),
    '#description' => t('The delta to use for partial rendering.'),
    '#default_value' => $conf['partials_delta'],
    '#options' => range(0, 20),
  );
  return $form;
}

/**
 * Submit form; Save field and partial values.
 */
function message_message_render_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['field_name'] = $form_state['values']['field_name'];
  $form_state['conf']['partials'] = $form_state['values']['partials'];
  $form_state['conf']['partials_delta'] = $form_state['values']['partials_delta'];
}
