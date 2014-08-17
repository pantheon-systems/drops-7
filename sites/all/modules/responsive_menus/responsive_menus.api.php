<?php

/**
 * @file
 * Hooks provided by the Responsive Menus module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * To add a new style to Responsive Menus, you are most likely creating at least
 * 3 functions:
 * hook_responsive_menus_style_info().
 * A form name callback to add form elements to Responsive Menu's admin form.
 * A js_settings() callback to pass extra variables to javascript.
 *
 * Explained below.
 */

/**
 * Example implementation of hook_responsive_menus_style_info().
 *
 * You can use js/css_folder to load a whole directory's files, or
 * js/css_files to load individual files.
 *
 * The parameters 'form' & 'js_settings' are callbacks to functions.
 *
 * Params when declaring hook_responsive_menus_style_info():
 * name            :string:  Name displayed when choosing style.
 * form            :string:  Drupal FAPI callback for admin form.
 * js_folder       :string:  Folder to recursively include .js files from.
 * css_folder      :string:  Folder to recursively include .css files from.
 * js_files        :array:   Individual JS files to include.
 * css_files       :array:   Individual CSS files to include.
 * js_settings     :string:  Function to generate settings to pass to JS.
 * use_libraries   :boolean: TRUE if uses libraries module to load.
 * library         :string:  Name that the libraries module uses to load.
 * jquery_version  :float:   Minimum jQuery version required for this style.
 */
function hook_responsive_menus_style_info() {
  $path = drupal_get_path('module', 'responsive_menus');
  $styles = array(
    'example_style' => array(
      'name' => t('Example Responsive Menus style'),
      'form' => 'example_style_settings',
      'js_folder' => drupal_get_path('module', 'responsive_menus') . '/js',
      'css_folder' => drupal_get_path('module', 'responsive_menus') . '/css',
      'js_files' => array(
        $path . '/js/example1.js',
        $path . '/js/example2.js',
      ),
      'css_files' => array($path . '/css/example.css'),
      'js_settings' => 'example_style_js_settings',
      'use_libraries' => TRUE,
      'library' => 'ExampleLibraryFolderName',
      'jquery_version' => 1.7,
    ),
  );

  return $styles;
}

/**
 * Additional style settings for the Responsive Menus admin form.
 *
 * You aren't returning an entire form, just some additional options that go
 * within the style settings fieldset of the Responsive Menus admin form.
 *
 * You are in charge of your own #default_values.
 *
 * @return array
 *   Drupal FAPI formatted array.
 */
function example_style_settings() {
  $form['responsive_menus_css_selectors'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS selectors for which menus to responsify'),
    '#default_value' => variable_get('responsive_menus_css_selectors', '.main-menu'),
    '#description' => t('Enter CSS selectors of menus to responsify.  Comma separated or 1 per line'),
  );
  $form['responsive_menus_simple_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to display for menu toggle button'),
    '#default_value' => variable_get('responsive_menus_simple_text', '☰ Menu'),
  );
  $form['responsive_menus_media_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Screen width to respond to'),
    '#size' => 10,
    '#default_value' => variable_get('responsive_menus_media_size', 768),
    '#description' => t('Width in pixels when we swap out responsive menu e.g. 768'),
  );

  return $form;
}

/**
 * Callback to generate settings to pass to javascript.
 *
 * @return array
 *   Array of settings to pass to javascript, identified by their key.
 *   They can be accessed in javascript by: settings.responsive_menus.your_key.
 */
function example_style_js_settings() {
  $js_settings = array();
  $js_settings['selectors'] = responsive_menus_build_selectors();
  $js_settings['toggler_text'] = variable_get('responsive_menus_simple_text', '☰ Menu');
  $js_settings['media_size'] = variable_get('responsive_menus_media_size', 768);

  return $js_settings;
}

/**
 * Alter any of the styles registered with Responsive Menus.
 *
 * This is a very powerful hook that can allow you to:
 * -Bypass a library's requirements.
 * --e.g. Remove a style's requirement on jquery_update or libraries module.
 * -Provide your own libraries or files.
 * -Include additional files to load with a style.
 * -Use a different form function for a style's settings.
 * -Use a different function for building javascript settings.
 *
 * @param array $styles
 *   Array of all the currently known styles.
 * Options:
 * name: string - Style's name.
 * form: string - Function name to return Drupal FAPI array.
 * js_files: array - Array of paths to JS files to load for this style.
 * css_files: array - Array of paths to CSS files to load for this style.
 * js_settings: string - Function name to build JS settings passed to drupal_add_js().
 * jquery_version: float - Minimum required jQuery version for this style.
 * -- Note:  This setting will require jquery_update module enabled unless the
 * -- user checks "I will provide my own jQuery Library".
 * use_libraries: boolean - TRUE / FALSE to use the Libraries module.
 * library: string - Name of the library, used for Libraries module.
 * selector: string - Text for the admin page describing which selector to use.
 *
 * Other notes:
 *   If you want to bypass the requirement on the Libraries module for a style,
 *   you can set 'use_libraries' => FALSE, and then use js_files & css_files to
 *   provide the path(s) to the files.
 *
 * See the 2 examples below.
 * First, showing all the available options.  Note including js_files, css_files
 * AND specifying use_libraries => TRUE would result in the Libraries module
 * first trying to load the library, then Responsive Menus would drupal_add_[type]
 * the files in js_files & css_files settings.
 *
 * Second example is showing how to override the sidr style to lift Libraries
 * module requirement to include your own files.
 *
 */
function hook_responsive_menus_styles_alter(&$styles) {
  // Example showing all of the currently available fields.
  // Note, js_folder & css_folder are excluded until an alternative to glob() is
  // built into RM.
  $path = drupal_get_path('module', 'my_style_module') . '/styles';
  $styles['my_style'] = array(
    'name' => t('My Style'),
    'form' => 'responsive_menus_my_style_settings',
    'js_files' => array(
      $path . '/my_style/my_style.js',
      $path . '/my_style/my_other_style.js',
    ),
    'css_files' => array($path . '/my_style/my_style.css'),
    'js_settings' => 'responsive_menus_my_style_js_settings',
    'jquery_version' => 1.7,
    'use_libraries' => TRUE,
    'library' => 'my_style',
    'selector' => t('Text describing what selector to use.  e.g. ul'),
  );

  // In this next example, I will override the Sidr style to bypass the Libraries
  // module and provide my own files in my_module/responsive_menus_alter/sidr/.
  $path = drupal_get_path('module', 'my_module') . '/responsive_menus_alter';
  $styles['sidr'] = array(
    'use_libraries' => FALSE,
    'js_files' => array($path . '/sidr/my_sidr.js',),
    'css_files' => array($path . '/sidr/my_sidr.css'),
  );
}
