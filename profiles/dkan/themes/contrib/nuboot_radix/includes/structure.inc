<?php
/**
 * @file
 * Theme and preprocess functions for breadcrumbs, messages, tabs..etc.
 */

/**
 * Implements hook_css_alter().
 */
function nuboot_radix_css_alter(&$css) {
  $radix_path = drupal_get_path('theme', 'radix');

  // Radix now includes compiled stylesheets for demo purposes.
  // We remove these from our subtheme since they are already included
  // in compass_radix.
  unset($css[$radix_path . '/assets/stylesheets/radix-style.css']);
  unset($css[$radix_path . '/assets/stylesheets/radix-print.css']);
  // Remove dkan button styles so we can use our own.
  unset($css[drupal_get_path('module', 'dkan_dataset') . '/dkan_dataset_btn.css']);
  unset($css[drupal_get_path('module', 'dkan_dataset') . '/dkan_dataset.css']);
}

/**
 * Implements hook_js_alter().
 */
function nuboot_radix_js_alter(&$javascript) {
  $radix_path = drupal_get_path('theme', 'radix');
  // Remove radix-script.js.
  // Hash usage in radix-script.js breaks with use of a hash map in URL. https://www.drupal.org/node/2426513.
  unset($javascript[$radix_path . '/assets/javascripts/radix-script.js']);
}

/**
 * Implements template_preprocess_page().
 */
function nuboot_radix_preprocess_page(&$variables) {
  // Add copyright to theme.
  if ($copyright = theme_get_setting('copyright')) {
    $variables['copyright'] = isset($copyright['value']) ? check_markup($copyright['value'], $copyright['format']) : '';
  }
  if (drupal_is_front_page()) {
    $vars['title'] = '';
    unset($vars['page']['content']['system_main']['default_message']);
  }
  if (arg(0) == 'admin' && arg(1) == 'appearance') {
    global $base_url;
    drupal_set_message(t('For Colorizer to work you must set the Administration theme to <em>Default theme</em> at the bottom of the <a href="' . $base_url . '/admin/appearance">theme list</a> page.'), 'warning');
  }
  // Remove title on dataset edit and creation pages.
  if (!empty($vars['node']) && in_array($vars['node']->type, array('dataset', 'resource')) || arg(1) == 'add') {
    $vars['title'] = '';
  }
  // Add external css files.
  $external_css = array(
    '#attached' => array(
      'css' => array(
        '//fonts.googleapis.com/css?family=Droid+Sans:400,700|Open+Sans:300italic,400italic,600italic,700italic,800italic,400,300,600,700,800' => array(
          'type' => 'external'),
      ),
    ),
  );
  drupal_render($external_css);
}
