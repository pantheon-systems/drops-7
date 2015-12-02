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
function cuminimal_preprocess_html(&$vars) {
  $vars['attributes_array']['class'][]='banner-white';
  $layout = theme_get_setting('layout_style', 'cuminimal') ? theme_get_setting('layout_style', 'cuminimal') : 'layout-wide';
  $vars['attributes_array']['class'][]=$layout;
}

/**
 * Implements hook_process_region).
 */
function cuminimal_preprocess_region(&$vars) {
  $theme = alpha_get_theme();
  switch ($vars['elements']['#region']) {         
    case 'site_info':
      $vars['beboulder']['color'] = 'white';
      break;
  }
}