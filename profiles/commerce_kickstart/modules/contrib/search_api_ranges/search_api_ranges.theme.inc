<?php

/**
 * @file
 * Theme functions for Min/Max UI Slider widget
 */

/**
 * Theme a jQuery slider.
 *
 * @param array $variables
 */
function theme_search_api_ranges_slider(array $variables) {
  $output = '';
  $output .= '<div class="search-api-ranges-widget">';
  $output .= render($variables['slider']);
  $output .= '</div>';

  return $output;
}
