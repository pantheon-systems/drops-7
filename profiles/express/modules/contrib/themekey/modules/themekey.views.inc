<?php

/**
 * @file themekey.views.inc
 * Provides some views properties
 */

function themekey_views_themekey_properties() {
  // Attributes of properties
  $attributes = array();

  if (module_exists('views')) {

    $attributes['views:vid'] = array(
      'description' => t('Views: VID - The vid of a view (vid)'),
      'validator' => 'themekey_validator_ctype_digit',
      'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
    );

    // Mapping functions
    $maps = array();
    $maps[] = array(
      'src' => 'drupal:get_q',
      'dst' => 'views:vid',
      'callback' => 'themekey_views_vid2getq',
    );

    $return = array('attributes' => $attributes, 'maps' => $maps);

  }

  return $return;
}


/**
 * get the views id (vid) by given path
 * @param unknown_type $get_q
 * @param unknown_type $parameters
 */
function themekey_views_vid2getq($get_q, $parameters) {

  $vid = NULL;

  $all_views = views_get_all_views();

  if (!empty($all_views)) {
    foreach ($all_views as $views_name => $view) {

      // IF VIEW HAS A PATH WHICH IS EQUAL
      if ($view->get_url() == $get_q) {
        $vid = $view->vid;
        break;
      }

    }
  }

  return $vid;
}