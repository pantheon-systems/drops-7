<?php

/**
 * @file
 * theme-settings.php provides the custom theme settings
 *
 * Provides the checkboxes for the CSS overrides functionality
 * as well as the serif/sans-serif style option.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function bamboo_form_system_theme_settings_alter(&$form, &$form_state) {

  $form['bamboo_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bamboo Theme Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['bamboo_settings']['breadcrumbs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show breadcrumbs in a page'),
    '#default_value' => theme_get_setting('breadcrumbs', 'bamboo'),
    '#description' => t("Check this option to show breadcrumbs in page. Uncheck to hide."),
  );

  $form['bamboo_settings']['general_settings']['bamboo_themelogo'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use theme Logo?'),
    '#default_value' => theme_get_setting('bamboo_themelogo', 'bamboo'),
    '#description' => t("Check for yes, uncheck to upload your own logo!"),
  );

  $form['bamboo_settings']['general_settings']['theme_bg_config']['theme_bg'] = array(
    '#type' => 'select',
    '#title' => t('Choose a background'),
    '#default_value' => theme_get_setting('theme_bg'),
    '#options' => array(
      'gray_gradient' => t('Gray gradient'),
      'green_gradient' => t('Green gradient'),
      'purple_gradient' => t('Purple gradient'),
      'rust_gradient' => t('Rust gradient'),
      'light_fabric' => t('Light fabric'),
      'dark_linen' => t('Dark linen'),
      'light_cloth' => t('Light cloth'),
      'tiles' => t('Tiles'),
      'retro1' => t('Retro 1'),
      'retro2' => t('Retro 2'),
      'retro3' => t('Retro 3'),
      'retro4' => t('Retro 4'),
      'retro5' => t('Retro 5'),
      'abstract1' => t('Abstract pattern 1'),
      'abstract2' => t('Abstract pattern 2'),
      'abstract3' => t('Abstract pattern 3'),
      'abstract4' => t('Abstract pattern 4'),
      'abstract5' => t('Abstract pattern 5'),
    ),
  );

  $form['bamboo_settings']['general_settings']['theme_color_config']['theme_color_palette'] = array(
    '#type' => 'select',
    '#title' => t('Choose a color palette'),
    '#default_value' => theme_get_setting('theme_color_palette'),
    '#options' => array(
      'green_bamboo' => t('Green bamboo'),
      'warm_purple' => t('Warm purple'),
      'dark_rust' => t('Dark rust'),
    ),
  );

  $form['bamboo_settings']['general_settings']['header_font_style'] = array(
    '#type' => 'select',
    '#title' => t('Choose a header font style'),
    '#default_value' => theme_get_setting('header_font_style'),
    '#options' => array(
      'sans_serif' => t('Sans-Serif'),
      'serif' => t('Serif'),
    ),
  );

  $form['bamboo_settings']['general_settings']['body_font_style'] = array(
    '#type' => 'select',
    '#title' => t('Choose a body font style'),
    '#default_value' => theme_get_setting('body_font_style'),
    '#options' => array(
      'sans_serif' => t('Sans-Serif'),
      'serif' => t('Serif'),
    ),
  );

  $form['bamboo_settings']['general_settings']['theme_sidebar_location'] = array(
    '#type' => 'select',
    '#title' => t('Sidebar location'),
    '#default_value' => theme_get_setting('theme_sidebar_location'),
    '#description' => t("Choose where you would like your sidebar, left or right. Either way for mobile, it
      will flow underneath the content area."),
    '#options' => array(
      'sidebar_right' => t('Right'),
      'sidebar_left' => t('Left'),
    ),
  );

  $form['additional_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Bamboo Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['additional_settings']['other_settings']['bamboo_localcss'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use local.css?'),
    '#default_value' => theme_get_setting('bamboo_localcss', 'bamboo'),
    '#description' => t("This setting allows you to use your own custom css file within the Bamboo
    theme folder. Only check this box if you have renamed local.sample.css to local.css.
    You must clear the Drupal cache after doing this."),
  );

  $form['additional_settings']['other_settings']['bamboo_tertiarymenu'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use tertiary drop down menus?'),
    '#default_value' => theme_get_setting('bamboo_tertiarymenu', 'bamboo'),
    '#description' => t("Check this box if you are going to have tertiary (third level drop down menus)"),
  );

  $form['additional_settings']['other_settings']['bamboo_viewport'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Touch device pinch and zoom?'),
    '#default_value' => theme_get_setting('bamboo_viewport', 'bamboo'),
    '#description' => t("** Check this box ONLY if you want to enable touch device users to be able to pinch and zoom.
    Note this is purely experimental and if you enable this, there is no support for layouts breaking."),
  );

  $form['additional_settings']['other_settings']['bamboo_imagecaptions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Image Captions?'),
    '#default_value' => theme_get_setting('bamboo_imagecaptions', 'bamboo'),
    '#description' => t("Check this box if you would like captions for imagefield images."),
  );

  $form['additional_settings']['other_settings']['bamboo_tablehover'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use experimental table hover effect?'),
    '#default_value' => theme_get_setting('bamboo_tablehover', 'bamboo'),
    '#description' => t("Check this box for an experimental hover effect on table cells."),
  );

  $form['custom_css_path_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom CSS Path Settings'),
    '#description' => t("<strong style='color: #cc0000;'>Note only use this feature if you know what you are doing!</strong>"),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['custom_css_path_settings']['custom_css_path']['bamboo_custom_css_location'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only check the box if you want to specify a custom path below to your local css file.'),
    '#default_value' => theme_get_setting('bamboo_custom_css_location', 'bamboo'),
  );

  $form['custom_css_path_settings']['custom_css_path']['bamboo_custom_css_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to Custom Stylesheet'),
    '#description' => t('Specify a custom path to the local.css file without the leading slash:
    e.g.: sites/default/files/custom-css/local.css you must check the box above for this to work.'),
    '#default_value' => theme_get_setting('bamboo_custom_css_path', 'bamboo'),
  );

}
