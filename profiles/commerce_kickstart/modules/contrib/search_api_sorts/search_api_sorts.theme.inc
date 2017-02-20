<?php

/**
 * Theme a list of sort options.
 *
 * @param array $variables
 *   An associative array containing:
 *   - items: The sort options
 *   - options: Various options to pass
 */
function theme_search_api_sorts_list(array $variables) {
  $items = array_map('render', $variables['items']);
  $options = $variables['options'];

  return $items ? theme('item_list', array('items' => $items) + $options) : '';
}

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
 * @return string
 */
function theme_search_api_sorts_sort(array $variables) {
  $name = $variables['name'];
  $path = $variables['path'];
  $options = $variables['options'] + array('attributes' => array());
  $options['attributes'] += array('class' => array());

  $order_options = $variables['order_options'] + array('query' => array(), 'attributes' => array(), 'html' => TRUE);
  $order_options['attributes'] += array('class' => array());

  if ($variables['active']) {
    $return_html = '<span class="search-api-sort-active">';
    $return_html .= l(t(check_plain($name)) . theme('tablesort_indicator', array('style' => $order_options['query']['order'])), $path, $order_options);
    $return_html .= '</span>';
  }
  else {
    $return_html = l($name, $path, $options);
  }

  return $return_html;
}
