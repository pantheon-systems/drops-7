<?php

/**
 * @file
 * Form functions for embed_templates module.
 */

// Include confirm form file.
module_load_include('inc', 'embed_templates', 'forms/embed_templates.confirm_form');

/**
 * Form for adding embeds.
 *
 * @param array $form
 *   Empty form passed in to be built upon.
 * @param array $form_state
 *   State of form which might be in confirmation step.
 * @param int|null $embed_templates_id
 *   ID of embed if being edited or deleted.
 *
 * @return array
 *   Fully built form ready to send to user.
 */
function embed_templates_form($form, &$form_state, $embed_templates_id = NULL) {

  // If set to delete embeds, then return that form.
  if (isset($form_state['storage']['confirm'])) {
    return embed_templates_confirm($form, $form_state);
  }

  // Initialize default form variables.
  $form = array();
  $label = $path = '';
  $status = NULL;
  $data = array();
  $embed_templates = NULL;
  $id = NULL;
  $size = variable_get('et_form_field_size', 128);

  // Load embed if it exists.
  if ($embed_templates_id != NULL && $embed_templates = entity_load_single('embed_templates', $embed_templates_id)) {
    // Grab existing values to place in form.
    $label = $embed_templates->name;
    $path = $embed_templates->path;
    $status = $embed_templates->status;
    $type = $embed_templates->type;
    $data = unserialize($embed_templates->data);
    $id = $embed_templates_id;

    // Store embed in form for form submission processing.
    $form_state['embed_templates'] = $embed_templates;

    // @todo Deal with embed templates entity not loading?
  }
  else {
    // Get type.
    $path_parts = explode('/', current_path());
    $type = array_pop($path_parts);

    // Get type definition.
    $type_definition = embed_templates_get_embed_type($type);
    $status = $type_definition['default_status'];
  }

  $form['embed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embed'),
    '#weight' => -10,
  );

  $form['embed']['label'] = array(
    '#title' => t('Label'),
    '#description' => t('Administrative label used in overview screen.'),
    '#type' => 'textfield',
    '#default_value' => $label,
    '#required' => TRUE,
    '#size' => $size,
  );

  // Search field for existing paths.
  $form['embed']['path'] = array(
    '#title' => t('Path'),
    '#description' => 'The page you will add the embed to. To add embeds to multiple pages, please leave blank and use the Context module.',
    '#type' => 'textfield',
    '#autocomplete_path' => 'titles/autocomplete',
    '#default_value' => $path,
    '#size' => $size,
  );

  // Get all status options.
  $all_operations = embed_templates_get_operations();

  // Remove operations that user doesn't have permissions to perform.
  $operations = array();
  foreach ($all_operations as $key => $value) {
    if ($value['add_permission'] != NULL && user_access($value['add_permission'])) {
      $operations[$key] = $value['label'];
    }

    // Add statuses that don't require a permission.
    if ($value['add_permission'] == NULL) {
      $operations[$key] = $value['label'];
    }
  }

  // If user doesn't have access to operations, then only include the default status.
  // @todo What if the status is set to something I don't have permissions for?
  $disabled_status = FALSE;
  $status_description = '';
  if (empty($operations)) {
    $disabled_status = TRUE;
    // Add label from above. Need to declare label before operations are unset.
    $operations = array($status => $all_operations[$status]['label']);
    // Add a nice description to tell user why the field is disabled.
    $status_description = 'You do not have sufficient permissions to edit this status. The declared or default status will remain the same when this form is saved.';
  }

  // Add options for publishing.
  $form['publish_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Workflow Options'),
    '#weight' => 11,
  );
  $form['publish_settings']['status'] = array(
    '#title' => t('Status'),
    '#type' => 'select',
    '#disabled' => $disabled_status,
    '#options' => $operations,
    '#default_value' => $status,
    '#description' => $status_description,
  );

  // Add options for save and cancel.
  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('embed_code_form_cancel'),
    '#limit_validation_errors' => array(),
  );

  // Add delete button only if editing an embed.
  if (isset($embed_templates) && user_access('delete embeds')) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#submit' => array('embed_templates_form_submit'),
      '#value' => t('Delete'),
    );
  }

  // @todo possibly disable parts of form if no "Publishing Options" are available
  // to the user.

  // Get the specific parts of the embed form.
  if (!isset($form['confirm'])) {
    call_user_func_array(embed_templates_get_form_callback($type), array(
      &$form,
      &$form_state,
      $id,
      $data
    ));
  }

  return $form;
}

/**
 * Submit function for adding or editing an embed.
 *
 * @param array $form
 *   Fully built form shown to user.
 * @param array $form_state
 *   Ste of form with user-entered values.
 *
 * @return mixed
 *   Ends up redirecting user so nothing is really returned.
 */
function embed_templates_form_submit(&$form, &$form_state) {

  // If the user has chosen the delete operation, then try to delete embed.
  // Otherwise, go about trying to save embed information.
  if ($form_state['input']['op'] == 'Delete') {
    // If the user hasn't confirmed, they want to delete items, send them to confirm form.
    if (!isset($form_state['storage']['confirm'])) {
      // @todo Don't have recursive $form_state. Probably only store part of the array I need.
      $form_state['storage']['form_state'] = $form_state;
      $form_state['storage']['form'] = $form;
      // This will cause the form to be rebuilt returning to the confirm part of the form.
      $form_state['storage']['confirm'] = TRUE;
      $form_state['rebuild'] = TRUE;
      return $form_state;
    }
    else {
      // Call submission callback in case embeds need to do something for deletion.
      if ($submission_callback = embed_templates_get_submission_callback($form_state['embed_templates']->type)) {
        call_user_func($submission_callback, $form_state['embed_templates'], 'delete');
      }

      // Finally delete embed and redirect to the embed codes overview form.
      entity_delete('embed_templates', $form_state['embed_templates']->id);
      $form_state['redirect'] = 'admin/content/embeds';
    }
  }
  else {

    // Take type form URL or embed entity.
    if (isset($form_state['embed_templates'])) {
      $type = $form_state['embed_templates']->type;
      $renderer = $form_state['embed_templates']->renderer;
    }
    else {
      $path_parts = explode('/', current_path());
      $type = array_pop($path_parts);
      $renderer = embed_templates_get_renderer($type);
    }

    // Build data array for serialization by grabbing form values for embed type.
    $data = array();
    $options = array();

    foreach ($form['options'] as $key => $value) {
      if (strpos($key, '#') === FALSE) {
        $options[] = $key;
      }
    }

    // @todo Deal with possibility of form elements with same keys. Input values are overwritten if they have the same keys.
    foreach ($form_state['values'] as $key => $value) {
      if (in_array($key, $options)) {
        $data[$key] = $value;
      }
    }

    // Account for embed with no paths. Used for Context and Beans.
    $path = !empty($form_state['values']['path']) ? $form_state['values']['path'] : 'N/A';

    // Need user for embed overview page.
    global $user;

    // If editing an embed, add data to existing embed object.
    // Otherwise, make a new embed entity.
    if (isset($form_state['embed_templates']->id)) {
      $embed_templates = $form_state['embed_templates'];
      $embed_templates->name = $form_state['values']['label'];
      $embed_templates->author = $user->name  ?: 'N/A';
      $embed_templates->updated = REQUEST_TIME;
      $embed_templates->path = $path;
      $embed_templates->type = $type;
      $embed_templates->renderer = $renderer;
      $embed_templates->status = $form_state['values']['status'];
      $embed_templates->data = serialize($data);
    }
    else {
      $values = array(
        'name' => $form_state['values']['label'],
        'author' => $user->name ?: 'N/A',
        'updated' => REQUEST_TIME,
        'path' => $path,
        'type' => $type,
        'renderer' => $renderer,
        'status' => $form_state['values']['status'],
        'data' => serialize($data),
      );
      $embed_templates = entity_create('embed_templates', $values);
    }

    // Call submission callback in case embeds need to do something for creation/save.
    if ($submission_callback = embed_templates_get_submission_callback($type)) {
      call_user_func($submission_callback, $embed_templates, 'save');
    }

    // @todo make sure that the type + name is unique and send form error if not. Probably best to do in validation function.
    $success = $embed_templates->save();

    // Redirect to overview page if successful.
    // Otherwise, show error to user.
    if ($success) {
      $form_state['redirect'] = 'admin/content/embeds';
    }
    else {
      drupal_set_message('Embed code could not be saved. Please try again.', 'error');
    }
  }
}
