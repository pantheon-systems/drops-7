<?php
/**
 * @file
 * Menu Badges themes.
 */

/**
 * Preprocess the menu badge views display.
 */
function template_preprocess_menu_badges_views_view(&$vars) {
  $view = $vars['view'];

  $vars['rows']       = (!empty($view->result) || $view->style_plugin->even_empty()) ? $view->style_plugin->render($view->result) : '';
  if (!empty($vars['rows'])) {
    $vars['menu_result'] = trim(strip_tags($vars['rows']));
  }
  else {
    $vars['menu_result'] = NULL;
  }
}