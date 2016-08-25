<?php

/**
 * Form for adding / editing a video entity.
 */
function tracking_pixel_form($form, &$form_state, $tracking_pixel = NULL) {
  field_attach_form('tracking_pixel', $tracking_pixel, $form, $form_state);
  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save Tracking Pixel'),
    ),
  );
  $form['actions']['cancel'] = array(
    '#type'   => 'submit',
    '#value'  => t('Cancel'),
    '#submit' => array('trackingpixel_form_cancel'),
    '#limit_validation_errors' => array(),
  );
  if (empty($tracking_pixel->is_new)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#submit' => array('cu_tracking_pixel_delete_submit'),
      '#value' => t('Delete'),
    );
  }
  // Decode embed code
  if (!empty($form['field_tracking_code'][LANGUAGE_NONE][0]['value']['#default_value'])) {
    $code = $form['field_tracking_code'][LANGUAGE_NONE][0]['value']['#default_value'];
    $form['field_tracking_code'][LANGUAGE_NONE][0]['value']['#default_value'] = base64_decode($code);
  }
  $options = array(
    'top' => 'Top',
    'bottom' => 'Bottom',
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#weight' => 10,
  );
  $form['settings']['position'] = array(
    '#title' => t('Location'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Choose where on the page the pixel is placed. Look at the tracking pixel instructions to determine where it should be placed.'),
    '#default_value' => isset($tracking_pixel->position) ? $tracking_pixel->position : 'bottom',
  );
  $form['publish_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Publishing Options'),
    '#weight' => 11,
  );

  $form['publish_settings']['status'] = array(
    '#title' => t('Published'),
    '#type' => 'checkbox',

    '#default_value' => isset($tracking_pixel->status) ? $tracking_pixel->status : '1',
  );
  return $form;
}

function trackingpixel_form_cancel() {
  drupal_goto('admin/content/trackingpixels');
}

function cu_tracking_pixel_delete_submit($form, &$form_state) {
  $id = $form['#entity']->id;
  $form_state['redirect'] = array('admin/content/trackingpixels/' . $id . '/delete');
}

/**
 * Validation for video entity form.
 */
function tracking_pixel_form_validate($form, &$form_state) {

}

/**
 * Submit handler for video entity form.
 */
function tracking_pixel_form_submit($form, &$form_state) {
  // Values saved to tracking_pixel table
  $values = array(
    'name' => $form_state['values']['field_tracking_name'][LANGUAGE_NONE][0]['value'],
    'path' => $form_state['values']['field_tracking_path'][LANGUAGE_NONE][0]['url'],
    'position' => $form_state['values']['position'],
    'status' => $form_state['values']['status'],
  );
  // See if the entity exists
  if ($tracking_pixel = entity_load_single('tracking_pixel', $form['#entity']->id)) {
    // Entity exists, add values just in case they have changed
    foreach ($values as $key => $value) {
      $tracking_pixel->$key = $value;
    }
  }
  else {
    // create new entity with values
    $tracking_pixel = entity_create('tracking_pixel', $values);
  }
  // Update attached fields
  $tracking_pixel->field_tracking_name = $form_state['values']['field_tracking_name'];

  $tracking_pixel->field_tracking_path = $form_state['values']['field_tracking_path'];

  // Encode the tracking pixel code before it's stored
  $code = $form_state['values']['field_tracking_code'][LANGUAGE_NONE][0]['value'];
  $form_state['values']['field_tracking_code'][LANGUAGE_NONE][0]['value'] = base64_encode($code);

  $tracking_pixel->field_tracking_code = $form_state['values']['field_tracking_code'];

  // Save entity and return to overview
  $tracking_pixel->save();
  drupal_goto('admin/content/trackingpixels');
}

function cu_tracking_pixel_view_display() {
  $output = array();

  return $output;
}
