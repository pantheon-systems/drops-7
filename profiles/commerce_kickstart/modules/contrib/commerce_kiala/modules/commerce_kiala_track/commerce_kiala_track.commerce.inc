<?php

/**
 * @file
 * Implementations of Commerce Hooks
 */


/**
 * Implements hook_commerce_kiala_settings_info().
 */
function commerce_kiala_track_commerce_kiala_settings_info() {
  return array(
    'countryid' => array(
      'default' => 'FR'
    ),
    'tracktrace_ws_url' => array(
      'group' => 'tracktrace_ws',
      'default' => 'https://trackandtrace-ws.kiala.com/tracker/rest/ParcelData',
    ),
    'tracktrace_ws_params' => array(
      'group' => 'tracktrace_ws',
      'default' => array(),
    ),
    /*'tracktrace_display_method' => array(
      'group' => 'tracktrace',
      'default' => 'form'
    ),*/
    'tracktrace_iframe_url' => array(
      'group' => 'tracktrace_iframe',
      'default' => 'https://be.kiala.com/tnt'
    ),
    'tracktrace_embed_type' => array(
      'group' => 'tracktrace_iframe',
      'default' => 'inline'
    ),
    'tracktrace_modal_link_text' => array(
      'group' => 'tracktrace_iframe',
      'default' => 'View Tracking',
    ),
    'tracktrace_iframe_height' => array(
      'group' => 'tracktrace_iframe',
      'default' => 480
    ),
    'tracktrace_iframe_width' => array(
      'group' => 'tracktrace_iframe',
      'default' => 640
    )
  );
}
