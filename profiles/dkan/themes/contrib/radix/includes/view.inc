<?php
/**
 * @file
 * Theme and preprocess functions for views.
 */

/**
 * Implements template_preprocess_views_view_table().
 */
function radix_preprocess_views_view_table(&$variables) {
  // Add Bootstrap table classes.
  $variables['classes_array'] = array_merge($variables['classes_array'], array(
    'table',
    'table-striped',
    'table-bordered',
  ));
}

/**
 * Implements template_preprocess_views_view_unformatted().
 */
//function radix_preprocess_views_view_unformatted(&$variables) {
//  // Add col-md-12 class to views-row is a col class is not provided.
//  foreach ($variables['classes_array'] as &$classes) {
//    if (!preg_match("/col\-[xs|sm|md|lg]*\-[0-9]{1,2}/", $classes)) {
//      $classes .= ' col-md-12';
//    }
//  }
//}

/**
 * Implements theme_views_mini_pager().
 */
function radix_views_mini_pager($vars) {
  global $pager_page_array, $pager_total;

  $tags = $vars['tags'];
  $element = $vars['element'];
  $parameters = $vars['parameters'];

  // current is the page we are currently paged to
  $pager_current = $pager_page_array[$element] + 1;
  // max is the maximum page number
  $pager_max = $pager_total[$element];
  // End of marker calculations.

  if ($pager_total[$element] > 1) {

    $li_previous = theme('pager_previous', array(
      'text' => (isset($tags[1]) ? $tags[1] : t('‹‹')),
      'element' => $element,
      'interval' => 1,
      'parameters' => $parameters,
    ));
    if (empty($li_previous)) {
      $li_previous = "&nbsp;";
    }

    $li_next = theme('pager_next', array(
      'text' => (isset($tags[3]) ? $tags[3] : t('››')),
      'element' => $element,
      'interval' => 1,
      'parameters' => $parameters,
    ));

    if (empty($li_next)) {
      $li_next = "&nbsp;";
    }

    $items[] = array(
      'class' => array('pager-previous'),
      'data' => $li_previous,
    );

    $items[] = array(
      'class' => array('pager-current'),
      'data' => '<span>' . t('@current of @max', array('@current' => $pager_current, '@max' => $pager_max)) . '</span>',
    );

    $items[] = array(
      'class' => array('pager-next'),
      'data' => $li_next,
    );
    return theme('item_list', array(
      'items' => $items,
      'title' => NULL,
      'type' => 'ul',
      'attributes' => array('class' => array('pagination')),
    ));
  }
}