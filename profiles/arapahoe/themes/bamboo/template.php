<?php

/**
 * @file
 * Template.php provides theme functions & overrides
 */

/**
 * Implements hook_preprocess_html().
 */
function bamboo_preprocess_html(&$vars) {
$vars['rdf'] = new stdClass;

  if (module_exists('rdf')) {
    $vars['doctype'] = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML+RDFa 1.1//EN">' . "\n";
    $vars['rdf']->version = ' version="HTML+RDFa 1.1"';
    $vars['rdf']->namespaces = $vars['rdf_namespaces'];
    $vars['rdf']->profile = ' profile="' . $vars['grddl_profile'] . '"';
  }
  else {
    $vars['doctype'] = '<!DOCTYPE html>' . "\n";
    $vars['rdf']->version = '';
    $vars['rdf']->namespaces = '';
    $vars['rdf']->profile = '';
  }

  drupal_add_css('//fonts.googleapis.com/css?family=Open+Sans:400italic,600italic,700italic,400,600,700',array('type' => 'external'));
  drupal_add_css('//fonts.googleapis.com/css?family=Rosarivo:400,400italic&subset=latin,latin-ext',array('type' => 'external'));

  // Add a body class is the site name is hidden.
  if (theme_get_setting('toggle_name') == FALSE) {
    $vars['classes_array'][] = 'site-name-hidden';
  }

  // Add IE 8 fixes style sheet.
  drupal_add_css(path_to_theme() . '/css/ie8-fixes.css',
    array(
      'group' => CSS_THEME,
      'browsers' =>
      array(
        'IE' => 'lte IE 8',
        '!IE' => FALSE),
      'preprocess' => FALSE));

  // Add IE 9 fixes style sheet.
  drupal_add_css(path_to_theme() . '/css/ie9-fixes.css',
    array(
      'group' => CSS_THEME,
      'browsers' =>
      array(
        'IE' => 'IE 9',
        '!IE' => FALSE),
      'preprocess' => FALSE));

  // Extra body classes for theme variables.
  // The background.
  // $file = theme_get_setting('theme_bg') . '-style';
  $file = theme_get_setting('theme_bg');
  /* drupal_add_css(path_to_theme() . '/css/'.
  $file, array('group' => CSS_THEME, 'weight' =>
  115,'browsers' => array(), 'preprocess' => FALSE)); */
  $vars['classes_array'][] = drupal_html_class('bg-' . $file);

  // The Color Palette.
  $file = theme_get_setting('theme_color_palette');
  $vars['classes_array'][] = drupal_html_class('color-palette-' . $file);

  // The header font style.
  $file = theme_get_setting('header_font_style');
  $vars['classes_array'][] = drupal_html_class('header-font-' . $file);

  // The body font style.
  $file = theme_get_setting('body_font_style');
  $vars['classes_array'][] = drupal_html_class('body-font-' . $file);

  // Sidebar location.
  $file = theme_get_setting('theme_sidebar_location');
  $vars['classes_array'][] = drupal_html_class($file);

  // If imagecaption is true, add class.
  if (theme_get_setting('bamboo_imagecaptions') == TRUE) {
    $vars['classes_array'][] = drupal_html_class('imagecaption');
  }

  // If tablehover is true, add class.
  if (theme_get_setting('bamboo_tablehover') == TRUE) {
    $vars['classes_array'][] = drupal_html_class('tablehover');
  }

  // Local css within theme folder if checked.
  if (theme_get_setting('bamboo_localcss') == TRUE) {
     drupal_add_css(path_to_theme() . '/css/local.css',
      array(
        'group' => CSS_THEME,
        'media' => 'screen',
        'preprocess' => TRUE,
        'weight' => '9998',
      )
    );
  }

  // Custom css file path if checked and file exists.
  if (theme_get_setting('bamboo_custom_css_location') == TRUE) {
    $path =  theme_get_setting('bamboo_custom_css_path');
    if (file_exists($path)) {
      drupal_add_css("$path",
        array(
          'group' => CSS_THEME,
          'preprocess' => TRUE,
          'weight' => 9999
        )
      );
    }
  }

  // Add general JS.
  drupal_add_js(drupal_get_path('theme', 'bamboo') . '/js/scripts.js',
    array(
      'group' => JS_THEME,
      'preprocess' => TRUE,
      'weight' => '9999',
      )
    );
  $vars['scripts'] = drupal_get_js();

  // Use tertiary menus = true.
  if (theme_get_setting('bamboo_tertiarymenu') == TRUE) {
    $vars['classes_array'][] = drupal_html_class('tertiarymenu');
  }

  if (!$vars['is_front']) {
    // Add unique class for each page.
    $path = drupal_get_path_alias($_GET['q']);
    // Add unique class for each website section.
    list($section,) = explode('/', $path, 2);
    $arg = explode('/', $_GET['q']);
    if ($arg[0] == 'node' && isset($arg[1])) {
      if ($arg[1] == 'add') {
        $section = 'node-add';
      }
      elseif (isset($arg[2]) && is_numeric($arg[1]) && ($arg[2] == 'edit' || $arg[2] == 'delete')) {
        $section = 'node-' . $arg[2];
      }
    }
    $vars['classes_array'][] = drupal_html_class('section-' . $section);
  }

  // Test if page is a node or not and then add a body class.
  if ($node = menu_get_object()) {
    $vars['classes_array'][] = 'is-node';
  }
  else {
    $vars['classes_array'][] = 'not-node';
  }

  // Add a body class is the theme logo is hidden or not.
  if (theme_get_setting('bamboo_themelogo') == TRUE) {
    $vars['classes_array'][] = 'theme-logo';
  }
  else {
    $vars['classes_array'][] = 'theme-logo-none';
  }

}

/**
 * Custom functions for the theme
 */

/**
 * Implements hook_html_head_alter().
 */
function bamboo_html_head_alter(&$head_elements) {
  // Overwrite default meta character tag with HTML5 version.
  $head_elements['system_meta_content_type']['#attributes'] = array(
    'charset' => 'utf-8',
  );
}

/**
 * Preprocesses variables for theme_username().
 *
 * Modules that make any changes to variables like 'name' or 'extra' must insure
 * that the final string is safe to include directly in the output by using
 * check_plain() or filter_xss().
 *
 * @see template_process_username()
 */
function bamboo_preprocess_username(&$vars) {

  // Update the username so it's the full name of the user.
  $account = $vars['account'];

  // Revise the name trimming done in template_preprocess_username.
  $name = $vars['name_raw'] = format_username($account);

  // Trim the altered name as core does, but with a higher character limit.
  if (drupal_strlen($name) > 35) {
    $name = drupal_substr($name, 0, 18) . '...';
  }

  // Assign the altered name to $vars['name'].
  $vars['name'] = check_plain($name);

}

/**
 * Insert themed breadcrumb page navigation at top of the node content.
 */
function bamboo_breadcrumb($vars) {

  // Show breadcrumbs if checked.
  if (theme_get_setting('breadcrumbs') == 1) {

    // Theme the breadcrumbs.
    $breadcrumb = $vars['breadcrumb'];
    if (!empty($breadcrumb)) {
      // Use CSS to hide titile .element-invisible.
      $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
      // Comment below line to hide current page to breadcrumb.
      $breadcrumb[] = drupal_get_title();
      $output .= '<nav class="breadcrumb">' . implode(' » ', $breadcrumb) . '</nav>';
      return $output;
    }
  }
}

/**
 * Override or insert variables into the page template.
 */
function bamboo_preprocess_page(&$vars, $hook) {
  // Set variable for theme native main menu with sub links.
  if (!empty($vars['main_menu'])) {
  // Get the entire main menu tree.
  $main_menu_tree = menu_tree_all_data('main-menu');
  // Add the rendered output to the $primary_nav variable.
  $vars['primary_nav'] = menu_tree_output($main_menu_tree);
  }

  else {
    // Don't show the menu if unchecked in the theme settings.
    $vars['primary_nav'] = FALSE;
  }

  // If the default logo is used, then determine which color and set the path.
  $file = theme_get_setting('theme_color_palette');
  if (theme_get_setting('bamboo_themelogo') == TRUE) {
    $vars['logo'] = base_path() . path_to_theme() . '/images/' . $file . '-logo.png';
  }

  // Check if it's a node and set a variable.
  $vars['is_node'] = false;
  if ($node = menu_get_object()) {
    $vars['is_node'] = true;
  }

}

/**
 * Duplicate of theme_menu_local_tasks() but adds clearfix to tabs.
 */
function bamboo_menu_local_tasks(&$vars) {
  $output = '';

  if (!empty($vars['primary'])) {
    $vars['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $vars['primary']['#prefix'] .= '<ul class="tabs primary clearfix">';
    $vars['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($vars['primary']);
  }
  if (!empty($vars['secondary'])) {
    $vars['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $vars['secondary']['#prefix'] .= '<ul class="tabs secondary clearfix">';
    $vars['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($vars['secondary']);
  }
  return $output;
}

/**
 * Override or insert variables into the node template.
 */
function bamboo_preprocess_node(&$vars) {

  // Global node.
  $node = $vars['node'];
  if ($vars['view_mode'] == 'full' && node_is_page($node)) {
    $vars['classes_array'][] = 'node-full';
  }

  if ($vars['view_mode'] == 'teaser' && node_is_page($node)) {
    $vars['classes_array'][] = 'node-teaser';
  }

  // Some nice expanded classes for Nodes.
  $vars['attributes_array']['role'][] = 'article';
  $vars['title_attributes_array']['class'][] = 'article-title';
  $vars['content_attributes_array']['class'][] = 'article-content';

  // Show only the username in submitted, the date is handled by node.tpl.php.
  $vars['submitted'] = t('Submitted by !username',
    array('!username' => $vars['name']));

  if ($blocks  = block_get_blocks_by_region('node_block')) {
    $vars['node_block'] = $blocks;
  }
  
  if ($blocks_node_block = block_get_blocks_by_region('node_block')) {
        $vars['node_block'] = $blocks_node_block;
        $vars['node_block']['#theme_wrappers'] = array('region');
        $vars['node_block']['#region'] = 'node_block';
    }
    else {
      $vars['node_block'] = '';
    }

    // Set date variables using drupal's format_date function
    // Based on <?php echo format_date($node->created, "custom", "M");.
    $vars['thedate'] = format_date($node->created, "custom", "j");
    $vars['themonth'] = format_date($node->created, "custom", "M");
    $vars['theyear'] = format_date($node->created, "custom", "Y");

}

/**
 * Implements hook_page_alter().
 */
function bamboo_page_alter($page) {
  /* add the viewport meta tag which will render as:
   * see: https://developer.mozilla.org/en-US/docs/Mobile/Viewport_meta_tag
   * for docs
   * <meta name="viewport" content="width=device-width, initial-scale=1.0
   * maximum-scale=1, user-scalable=no" />
   */

  if (theme_get_setting('bamboo_viewport') == FALSE) {

    // No pinch and zoom
    $viewport = array(
      '#type' => 'html_tag',
      '#tag' => 'meta',
      '#attributes' => array(
        'name' => 'viewport',
        'content' => 'width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no',
      ),
    );
    drupal_add_html_head($viewport, 'viewport');
  }
  else {

  /*
   * user-scalable=yes;
   * width=device-width;
   * initial-scale=0.31; maximum-scale=1.0; minimum-scale=0.25
   */

    // Pinch and Zoom enabled.
    $viewport = array(
      '#type' => 'html_tag',
      '#tag' => 'meta',
      '#attributes' => array(
        'name' => 'viewport',
        'content' => 'width=device-width, initial-scale=1.0, maximum-scale=2.0, minimum-scale=0.55, user-scalable=yes',
      ),
    );
    drupal_add_html_head($viewport, 'viewport');

  }

}
