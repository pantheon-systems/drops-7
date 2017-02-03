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

    case 'system_themes_admin_form':
      $admin_theme = variable_get('admin_theme');
      //if (arg(0) == 'admin' && arg(1) == 'appearance' && $admin_theme !== 0) {
      if ($admin_theme <> 0) {
        global $base_url;
        drupal_set_message(t('For Colorizer to work you must set the Administration theme to <em>Default theme</em> at the bottom of the <a href="' . $base_url . '/admin/appearance">theme list</a> page. ' . $admin_theme), 'warning');
    	}
    	break;
  }
}
