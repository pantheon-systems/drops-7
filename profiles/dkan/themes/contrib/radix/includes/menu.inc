<?php
/**
 * @file
 * Theme and preprocess functions for menus
 */

/**
 * Implements theme_menu_tree().
 */
function radix_menu_tree(&$variables) {
  return '<ul class="menu nav">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__menu_block().
 */
function radix_menu_tree__menu_block(&$variables) {
  return '<ul class="menu nav nav-pills nav-stacked">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__navbar_nav().
 */
function radix_menu_tree__navbar_nav(&$variables) {
  return '<ul class="menu nav navbar-nav">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__navbar_left().
 */
function radix_menu_tree__navbar_left(&$variables) {
  return '<ul class="menu nav navbar-nav navbar-left">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__navbar_right().
 */
function radix_menu_tree__navbar_right(&$variables) {
  return '<ul class="menu nav navbar-nav navbar-right">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__nav_tabs().
 */
function radix_menu_tree__nav_tabs(&$variables) {
  return '<ul class="nav nav-tabs">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__nav_tabs_justified().
 */
function radix_menu_tree__nav_tabs_justified(&$variables) {
  return '<ul class="nav nav-tabs nav-justified">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__nav_pills().
 */
function radix_menu_tree__nav_pills(&$variables) {
  return '<ul class="nav nav-pills">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__nav_pills_stacked().
 */
function radix_menu_tree__nav_pills_stacked(&$variables) {
  return '<ul class="nav nav-pills nav-stacked">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__nav_pills_justified().
 */
function radix_menu_tree__nav_pills_justified(&$variables) {
  return '<ul class="nav nav-pills nav-justified">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_tree__nav_expanded().
 */
function radix_menu_tree__nav_expanded(&$variables) {
  return '<ul class="nav nav-expanded nav-justified">' . $variables['tree'] . '</ul>';
}

/**
 * Implements theme_menu_link().
 */
function radix_menu_link($variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if (!empty($element['#below'])) {
    $sub_menu = drupal_render($element['#below']);
    $element['#localized_options']['html'] = TRUE;
    $element['#title'] .= '<span class="caret"></span>';
  }

  // Fix for active class.
  if (($element['#href'] == current_path() || ($element['#href'] == '<front>' && drupal_is_front_page())) && (empty($element['#localized_options']['language']) || $element['#localized_options']['language']->language == $language_url->language)) {
    $element['#attributes']['class'][] = 'active';
  }

  // Add active class to li if active trail.
  if (in_array('active-trail', $element['#attributes']['class'])) {
    $element['#attributes']['class'][] = 'active';
  }

  // Add a unique class using the title.
  $title = strip_tags($element['#title']);
  $element['#attributes']['class'][] = 'menu-link-' . drupal_html_class($title);

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Implements theme_menu_link().
 */
function radix_menu_link__dropdown($variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if (!empty($element['#below'])) {
    // Wrap in dropdown-menu.
    unset($element['#below']['#theme_wrappers']);
    $sub_menu = '<ul class="dropdown-menu">' . drupal_render($element['#below']) . '</ul>';
    $element['#localized_options']['attributes']['class'][] = 'dropdown-toggle';
    $element['#localized_options']['html'] = TRUE;

    // Check if element is nested.
    if ((!empty($element['#original_link']['depth'])) && ($element['#original_link']['depth'] > 1)) {
      $element['#attributes']['class'][] = 'dropdown-submenu';
      $element['#title'] .= ' <span class="fa fa-caret-right"></span>';
    }
    else {
      $element['#attributes']['class'][] = 'dropdown';
      $element['#title'] .= ' <span class="fa fa-caret-down"></span>';

      $element['#localized_options']['attributes']['data-toggle'] = 'dropdown';
      $element['#localized_options']['attributes']['data-target'] = '#';
    }
  }

  // Fix for active class.
  if (($element['#href'] == current_path() || ($element['#href'] == '<front>' && drupal_is_front_page())) && (empty($element['#localized_options']['language']) || $element['#localized_options']['language']->language == $language_url->language)) {
    $element['#attributes']['class'][] = 'active';
  }

  // Add active class to li if active trail.
  if (in_array('active-trail', $element['#attributes']['class'])) {
    $element['#attributes']['class'][] = 'active';
  }

  // Add a unique class using the title.
  $title = strip_tags($element['#title']);
  $element['#attributes']['class'][] = 'menu-link-' . drupal_html_class($title);

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Implements theme_menu_link__management.
 *
 * We render the management links as defaults to support the navbar module
 * out of the box.
 */
function radix_menu_link__management($variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }
  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Implements theme_link().
 */
function radix_link($variables) {
  $icons = '';
  $variables['options']['attributes'] = !empty($variables['options']['attributes']) ? $variables['options']['attributes'] : array();
  $variables['options']['html'] = !empty($variables['options']['html']) ? $variables['options']['html'] : FALSE;
  if (isset($variables['options']['attributes']['class']) && is_array($variables['options']['attributes']['class'])) {
    $css_classes = $variables['options']['attributes']['class'];
    if ($icon_classes = preg_grep('/^icon\-.*/', $css_classes)) {
      // Render an icon for each class.
      foreach ($icon_classes as $icon_class) {
        $icons .= '<i class="' . $icon_class . '"></i>';
      }
      // Unset icon classes for attributes to prevent double rendering.
      $variables['options']['attributes']['class'] = array_diff($css_classes, $icon_classes);
    }
  }
  return '<a href="' . check_plain(url($variables['path'], $variables['options'])) . '"' . drupal_attributes($variables['options']['attributes']) . '>' . $icons . ($variables['options']['html'] ? $variables['text'] : check_plain($variables['text'])) . '</a>';
}
