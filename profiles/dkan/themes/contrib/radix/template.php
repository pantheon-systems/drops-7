<?php
/**
 * @file
 * Theme hooks for Radix.
 */

require_once dirname(__FILE__) . '/includes/utilities.inc';
require_once dirname(__FILE__) . '/includes/theme.inc';
require_once dirname(__FILE__) . '/includes/maintenance.inc';
require_once dirname(__FILE__) . '/includes/structure.inc';
require_once dirname(__FILE__) . '/includes/field.inc';
require_once dirname(__FILE__) . '/includes/form.inc';
require_once dirname(__FILE__) . '/includes/menu.inc';
require_once dirname(__FILE__) . '/includes/comment.inc';
require_once dirname(__FILE__) . '/includes/panel.inc';
require_once dirname(__FILE__) . '/includes/view.inc';
require_once dirname(__FILE__) . '/includes/admin.inc';
require_once dirname(__FILE__) . '/includes/contrib.inc';

/**
 * Implements template_preprocess_html().
 */
function radix_preprocess_html(&$variables) {
  global $base_url;

  // Add Bootstrap JS from CDN if bootstrap library is not installed.
  if (!module_exists('bootstrap_library')) {
    $base = parse_url($base_url);
    $url = $base['scheme'] . '://netdna.bootstrapcdn.com/bootstrap/3.1.1/js/bootstrap.min.js';
    drupal_add_js($url, 'external');
  }

  // Add support for the Modenizr module.
  // Load modernizr.js only if modernizr module is not present.
  if (!module_exists('modernizr')) {
    drupal_add_js(drupal_get_path('theme', 'radix') . '/assets/javascripts/modernizr.js');
  }

  // Add meta for Bootstrap Responsive.
  // <meta name="viewport" content="width=device-width, initial-scale=1.0">
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      'name' => 'viewport',
      'content' => 'width=device-width, initial-scale=1.0',
    ),
  );
  drupal_add_html_head($element, 'bootstrap_responsive');

  // Add some custom classes for panels pages.
  if (module_exists('page_manager') && count(page_manager_get_current_page())) {
    $variables['is_panel'] = TRUE;

    // Get the current panel display and add some classes to body.
    if ($display = panels_get_current_page_display()) {
      $variables['classes_array'][] = 'panel-layout-' . $display->layout;

      // Add a custom class for each region that has content.
      $regions = array_keys($display->panels);
      foreach ($regions as $region) {
        $variables['classes_array'][] = 'panel-region-' . $region;
      }
    }
  }
}

/**
 * Implements hook_css_alter().
 */
function radix_css_alter(&$css) {
  // Unset some panopoly css.
  if (module_exists('panopoly_admin')) {
    $panopoly_admin_path = drupal_get_path('module', 'panopoly_admin');
    if (isset($css[$panopoly_admin_path . '/panopoly-admin.css'])) {
      unset($css[$panopoly_admin_path . '/panopoly-admin.css']);
    }
  }

  if (module_exists('panopoly_magic')) {
    $panopoly_magic_path = drupal_get_path('module', 'panopoly_magic');
    if (isset($css[$panopoly_magic_path . '/css/panopoly-modal.css'])) {
      unset($css[$panopoly_magic_path . '/css/panopoly-modal.css']);
    }
  }

  // Unset some core css.
  unset($css['modules/system/system.menus.css']);
}

/**
 * Implements hook_js_alter().
 */
function radix_js_alter(&$javascript) {
  // Add radix-modal when required.
  if (module_exists('ctools')) {
    $ctools_modal = drupal_get_path('module', 'ctools') . '/js/modal.js';
    $radix_modal = drupal_get_path('theme', 'radix') . '/assets/javascripts/radix-modal.js';
    if (!empty($javascript[$ctools_modal]) && empty($javascript[$radix_modal])) {
      $javascript[$radix_modal] = array_merge(
        drupal_js_defaults(), array('group' => JS_THEME, 'data' => $radix_modal));
    }
  }

  // Add radix-field-slideshow when required.
  if (module_exists('field_slideshow')) {
    $field_slideshow = drupal_get_path('module', 'field_slideshow') . '/field_slideshow.js';
    $radix_field_slideshow = drupal_get_path('theme', 'radix') . '/assets/javascripts/radix-field-slideshow.js';
    if (!empty($javascript[$field_slideshow]) && empty($javascript[$radix_field_slideshow])) {
      $javascript[$radix_field_slideshow] = array_merge(
        drupal_js_defaults(), array('group' => JS_THEME, 'data' => $radix_field_slideshow));
    }
  }

  // Add radix-progress when required.
  $progress = 'misc/progress.js';
  $radix_progress = drupal_get_path('theme', 'radix') . '/assets/javascripts/radix-progress.js';
  if (!empty($javascript[$progress]) && empty($javascript[$radix_progress])) {
    $javascript[$radix_progress] = array_merge(
      drupal_js_defaults(), array('group' => JS_THEME, 'data' => $radix_progress));
  }
}

/**
 * Implements template_preprocess_page().
 */
function radix_preprocess_page(&$variables) {
  // Determine if the page is rendered using panels.
  $variables['is_panel'] = FALSE;
  if (module_exists('page_manager') && count(page_manager_get_current_page())) {
    $variables['is_panel'] = TRUE;
  }

  // Make sure tabs is empty.
  if (empty($variables['tabs']['#primary']) && empty($variables['tabs']['#secondary'])) {
    $variables['tabs'] = '';
  }

  // Theme action links as buttons.
  if (!empty($variables['action_links'])) {
    foreach (element_children($variables['action_links']) as $key) {
      $variables['action_links'][$key]['#link']['localized_options']['attributes'] = array(
        'class' => array('btn', 'btn-primary', 'btn-sm'),
      );
    }
  }

  // Add search_form to theme.
  $variables['search_form'] = '';
  if (module_exists('search') && user_access('search content')) {
    $search_box_form = drupal_get_form('search_form');
    $search_box_form['basic']['keys']['#title'] = 'Search';
    $search_box_form['basic']['keys']['#title_display'] = 'invisible';
    $search_box_form['basic']['keys']['#size'] = 20;
    $search_box_form['basic']['keys']['#attributes'] = array('placeholder' => 'Search');
    $search_box_form['basic']['keys']['#attributes']['class'][] = 'form-control';
    $search_box_form['basic']['submit']['#value'] = t('Search');
    $search_box_form['#attributes']['class'][] = 'navbar-form';
    $search_box_form['#attributes']['class'][] = 'navbar-right';
    $search_box = drupal_render($search_box_form);
    $variables['search_form'] = (user_access('search content')) ? $search_box : NULL;
  }

  // Format and add main menu to theme.
  $main_menu_data = menu_build_tree(variable_get('menu_main_links_source', 'main-menu'), array(
    'min_depth' => 1,
    'max_depth' => 2,
  ));
  $variables['main_menu'] = menu_tree_output($main_menu_data);
  $variables['main_menu']['#theme_wrappers'] = array();

  // Add a copyright message.
  $variables['copyright'] = t('Drupal is a registered trademark of Dries Buytaert.');

  // Display a message if Sass has not been compiled.
//  $theme_path = drupal_get_path('theme', $GLOBALS['theme']);
//  $stylesheet_path = $theme_path . '/assets/stylesheets/screen.css';
//  if (_radix_current_theme() == 'radix') {
//    $stylesheet_path = $theme_path . '/assets/stylesheets/radix-style.css';
//  }
//  if (!file_exists($stylesheet_path)) {
//    drupal_set_message(t('It looks like %path has not been created yet. Run <code>@command</code> in your theme directory to create it.', array(
//      '%path' => $stylesheet_path,
//      '@command' => 'compass watch',
//    )), 'error');
//  }
}
