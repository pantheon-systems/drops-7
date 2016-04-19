<?php
/**
 * @file
 * Theme and preprocess functions for blocks.
 */

/**
 * Implements template_preprocess_block().
 */
function radix_preprocess_block(&$variables) {
  // Generate a custom class for the block from the title.
  $variables['classes_array'][] = 'block--' . drupal_html_class($variables['block']->title);
}
