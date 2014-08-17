<?php
/**
 * Implements hook_form_FORM_ID_alter().
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 */
function bizreview_form_system_theme_settings_alter(&$form, &$form_state) {
    
    $form['st_settings'] = array(
        '#type' => 'fieldset',
        '#title' => t('ST bizreview Theme Settings'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
    );

    $form['st_settings']['tabs'] = array(
        '#type' => 'vertical_tabs',
    );

    $form['st_settings']['tabs']['basic_settings'] = array(
        '#type' => 'fieldset',
        '#title' => t('Basic Settings'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
    );

    $form['st_settings']['tabs']['basic_settings']['scrolltop_display'] = array(
        '#type' => 'checkbox',
        '#title' => t('Show scroll-to-top button'),
        '#description'   => t('Use the checkbox to enable or disable scroll-to-top button.'),
        '#default_value' => theme_get_setting('scrolltop_display', 'bizreview'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
    );

    $form['st_settings']['tabs']['basic_settings']['bootstrap_js_include'] = array(
        '#type' => 'checkbox',
        '#title' => t('Bootstrap 3 Framework Javascript file'),
        '#description'   => t('Use the checkbox to enable or disable bootstrap.min.js file.'),
        '#default_value' => theme_get_setting('bootstrap_js_include', 'bizreview'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
    );

    $form['st_settings']['tabs']['ie8_support'] = array(
        '#type' => 'fieldset',
        '#title' => t('IE8 support'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['st_settings']['tabs']['ie8_support']['responsive_respond'] = array(
        '#type' => 'checkbox',
        '#title' => t('Add Respond.js [<em>bizreview/js/respond.min.js</em>] JavaScript to add basic CSS3 media query support to IE 6-8.'),
        '#default_value' => theme_get_setting('responsive_respond','bizreview'),
        '#description'   => t('IE 6-8 require a JavaScript polyfill solution to add basic support of CSS3 media queries. Note that you should enable <strong>Aggregate and compress CSS files</strong> through <em>/admin/config/development/performance</em>.'),
    );
	
	// Theme Color
  $form['st_settings']['tabs']['them_color_config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Color setting'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['st_settings']['tabs']['them_color_config']['theme_color'] = array(
    '#type' => 'select',
    '#title' => t('Color'),
    '#default_value' => theme_get_setting('theme_color'),
    '#options'  => array(
        'blue'              => t('Blue - Default'),
        'light-blue'        => t('Light Blue'),
        'green'             => t('Green'),
        'light-green'       => t('Light Green'),
        'red'           	=> t('Red'),
        'yellow'            => t('Yellow'),
        'purple'            => t('Purple'),
        'magenta'           => t('Magenta')
    ),
  );
}
