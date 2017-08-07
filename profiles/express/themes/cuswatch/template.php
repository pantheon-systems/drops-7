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
function cuswatch_preprocess_html(&$vars) {

  // SET BANNER COLOR (banner-white, banner-light, banner-dark, banner-black)
  $vars['classes_array'][]='banner-black';
  $layout = theme_get_setting('layout_style', 'cuswatch') ? theme_get_setting('layout_style', 'cuswatch') : 'layout-wide';
  $vars['classes_array'][]=$layout;
}

function cuswatch_preprocess_page(&$vars) {
  $vars['theme_hook_suggestions'][] = 'page__title';
}
