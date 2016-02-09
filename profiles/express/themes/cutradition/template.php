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
function cutradition_preprocess_html(&$vars) {



  // SET BANNER COLOR (banner-white, banner-light, banner-cumodern, banner-black)
  $banner_color = theme_get_setting('banner_color', 'cutradition') ? theme_get_setting('banner_color', 'cutradition') : 'black';
  //$banner_color = 'light';
  $vars['classes_array'][]='banner-' . $banner_color;
  $layout = theme_get_setting('layout_style', 'cutradition') ? theme_get_setting('layout_style', 'cutradition') : 'layout-wide';
  $vars['classes_array'][]=$layout;
}

function cutradition_preprocess_page(&$vars) {
  $vars['theme_hook_suggestions'][] = 'page__title';
}

function cutradition_breadcrumb($vars) {
  $breadcrumb = $vars['breadcrumb'];
  if (!empty($breadcrumb)) {
    // Replace the Home breadcrumb with a Home icon
    //$breadcrumb[0] = str_replace('Home','<i class="fa fa-home"></i> <span class="home-breadcrumb element-invisible">Home</span>',$breadcrumb[0]);
    // Get current page title and add to breadcrumb array
    $breadcrumb[] = '<span class="current-crumb"><span></span>' . drupal_get_title() . '</span>';
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    $output .= '<div class="breadcrumb">' . implode('<span class="breadcumb-divider element-invisible">/</span>', $breadcrumb) . '</div>';
    return $output;
  }
}
