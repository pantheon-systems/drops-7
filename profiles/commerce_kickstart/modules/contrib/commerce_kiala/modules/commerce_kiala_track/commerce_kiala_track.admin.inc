<?php

/*
 * Admin track and trace form
 */
function commerce_kiala_track_admin_trackandtrace_form($form, &$form_state, $order) {
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $tracking_object = $wrapper->kiala_tracking_object->value();
  $settings = commerce_kiala_settings();

  $form['tracking_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Tracking number'),
    '#default_value' => !empty($tracking_object) ? $tracking_object->tracking_number : '',
    '#required' => TRUE
  );

  if ($tracking_object) {

    // Cannot display secure iFrame content in non-secure page
    if ($GLOBALS['is_https']) {
      $embed_type = $settings['tracktrace_embed_type'];
      $form['modal_trackback_iframe_link'] = commerce_kiala_track_render_embedded_trackback_content($tracking_object->tracking_number, $embed_type);
    }
    else {
      drupal_set_message(t('Enable HTTPS on this page to display Kiala tracking information.'), 'warning', FALSE);
    }

    $form['#kiala_tracking_object'] = $tracking_object;
  }

  $form['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#description' => t('This will save the tracking number for this order and look for updates to the status'),
    '#submit' => array('commerce_kiala_track_admin_trackandtrace_form_submit')
  );

  $form['#order'] = $order;
  return $form;
}

/*
 * Form callback: submit track-and-trace admin form
 */
function commerce_kiala_track_admin_trackandtrace_form_submit(&$form, &$form_state) {

  // Update or insert a tracking object record
  if (empty($form['#kiala_tracking_object'])) {
    $tracking_object = commerce_kiala_track_create_tracking_object($form_state['values']['tracking_number'], $form['#order']->order_id);
  }
  else {
    $tracking_object = $form['#kiala_tracking_object'];
    $tracking_object->tracking_number = $form_state['values']['tracking_number'];
  }
  commerce_kiala_track_write_tracking_record($tracking_object);
  drupal_set_message(t('Tracking number updated'));
}
