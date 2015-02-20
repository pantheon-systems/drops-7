<?php
/**
 * Implements hook_form_alter().
 */
function nuboot_radix_form_alter(&$form, &$form_state, $form_id) {
  //drupal_set_message('<pre>' . print_r($form, TRUE) . '</pre>');
  switch ($form_id) {
    case 'colorizer_admin_settings':
      $form['colorizer_global']['colorizer_cssfile']['#default_value'] = 'colorizer/colorizer.css';
      $form['colorizer_global']['colorizer_incfile']['#default_value'] = 'colorizer/colorizer.inc';
      break;
  }
}
