<?php

/**
 * Theme a single sort item.
 *
 * @param array $variables
 *   An associative array containing:
 *   - name: The name to display for the item.
 *   - path: The destination path when the sort link is clicked.
 *   - options: An array of options to pass to l().
 *   - active: A boolean telling whether this sort filter is active or not.
 *   - order_options: If active, a set of options to reverse the order
 */
function theme_commerce_kickstart_search_sorts_sort(array $variables) {
  $name = $variables['name'];
  $path = $variables['path'];
  $options = $variables['options'] + array('attributes' => array());
  $options['attributes'] += array('class' => array());

  $order_options = $variables['order_options'] + array('query' => array(), 'attributes' => array());
  $order_options['attributes'] += array('class' => array());

  $return_html = l($name, $path, $options);
  if ($variables['active']) {
    $options['attributes']['class'][] = 'search-api-sort-remove';

    $return_html = '<span class="search-api-sort-active">';
    $return_html .= check_plain($name) . ' ';
    $return_html .= l(theme('tablesort_indicator', array('style' => $order_options['query']['order'])), $path, $order_options + array('html'=>TRUE));
    $return_html .= '</span>';
  }

  return $return_html;
}
