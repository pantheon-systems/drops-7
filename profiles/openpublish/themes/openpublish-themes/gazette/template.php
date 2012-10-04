<?php

/**
 * @file
 * This file is empty by default because the base theme chain (Alpha & Omega) provides
 * all the basic functionality. However, in case you wish to customize the output that Drupal
 * generates through Alpha & Omega this file is a good place to do so.
 * 
 * Alpha comes with a neat solution for keeping this file as clean as possible while the code
 * for your subtheme grows. Please read the README.txt in the /preprocess and /process subfolders
 * for more information on this topic.
 */

/**
 * Implements hook_preprocess_html().
 */
function gazette_preprocess_html(&$vars) {
  //add IE CSS
  drupal_add_css(path_to_theme() . '/css/ie8.css', array('group' => 2001, 'weight' => 100, 'browsers' => array('IE' => 'lte IE 8', '!IE' => FALSE), 'preprocess' => FALSE));
}

function gazette_preprocess_views_view_unformatted(&$vars) {
  foreach($vars['classes'] as &$rowclasses) {
    $rowclasses[] = 'clearfix';
  }
  foreach($vars['classes_array'] as &$rowclasses) {
    $rowclasses .= ' clearfix';
  }
  foreach($vars['attributes_array']['class'] as &$rowclasses) {
    $rowclasses .= ' clearfix';
  }
}