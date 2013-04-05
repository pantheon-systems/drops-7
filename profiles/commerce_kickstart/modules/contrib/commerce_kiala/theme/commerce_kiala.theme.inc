<?php

/**
 * @file
 * Contains the theme callbacks for the Kiala module.
 */

/**
 * Implements template_preprocess_hook()
 * - theme_commerce_kiala_point_details
 */
function template_preprocess_commerce_kiala_point_details(&$vars) {
  // store raw values
  $vars['point_details_raw'] = $vars['point_details'];
  $vars['point_title_raw'] = isset($vars['point_title']) ? $vars['point_title'] : '';

  // reference details for altering
  $point = &$vars['point_details'];

  // get settings
  $settings = commerce_kiala_settings(array(
    'include_passwords' => FALSE,
    'decrypt' => FALSE,
  ));

  // sanitize point values
  $point = commerce_kiala_sanitize_point_details($point);

  // sanitize point title
  if (!empty($vars['point_title'])) {
    $vars['point_title'] = check_plain($vars['point_title']);
  }

  // set available and status message
  $vars['available'] = FALSE;
  $vars['status_message'] = '';
  if (isset($point['status'])) {
    $vars['available'] = !empty($point['status']['available']);
    if (!empty($point['status']['value'])) {
      $vars['status_message'] = $point['status']['value'];
    }
  }

  // render an image tag
  $vars['picture_image'] = '';
  if (isset($point['picture']['href'])) {
    if (!empty($settings['ls_ws_details_image_style'])) {
      $vars['picture_image'] = theme('image_style', array(
        'style_name' => $settings['ls_ws_details_image_style'],
        'path' => $point['picture']['href'],
      ));
    }
    else {
      $vars['picture_image'] = theme('image', array(
        'path' => $point['picture']['href'],
      ));
    }
  }

  // add id specific vars
  if (!empty($point['shortId'])) {
    if (!empty($vars['show_more_link'])) {
      $vars['more_link'] = '';
      $vars['more_link_path'] = 'kiala-points/' . $point['shortId'];
      $vars['more_link_url'] = url('kiala-points/' . $point['shortId']);
      $vars['more_link_text'] = t('View more');

      if ($settings['ls_map_details_embed_type'] != 'inline') {
        $more_link_render = commerce_kiala_render_embedded_details_map($point['shortId'], array(
          'display_text' => $vars['more_link_text'],
        ));
        if (!empty($more_link_render)) {
          $vars['more_link'] = drupal_render($more_link_render);
        }
      }

      if (empty($vars['more_link'])) {
        $vars['more_link'] = l($vars['more_link_text'], $vars['more_link_path']);
      }
    }

    // add classes for id
    $vars['classes_array'][] = drupal_html_class('commerce-kiala-point-details-' . $point['shortId']);
  }

  // add css files
  drupal_add_css(drupal_get_path('module', 'commerce_kiala') . '/theme/commerce_kiala.css');
}

/**
 * Implements template_preprocess_hook()
 * - theme_commerce_kiala_point_search_map
 */
function template_preprocess_commerce_kiala_point_search_map(&$vars) {
  $zip = $vars['zip'];
  $options = !empty($vars['options']) ? $vars['options'] : array();

  $api_handler = commerce_kiala_get_handler_ls_map_search($zip, $options);
  $vars['search_map_url'] = $api_handler->url;
  $vars['search_map_width'] = $api_handler->settings['ls_map_search_width'];
  $vars['search_map_height'] = $api_handler->settings['ls_map_search_height'];
}

/**
 * Implements template_preprocess_hook()
 * - theme_commerce_kiala_point_details_map
 */
function template_preprocess_commerce_kiala_point_details_map(&$vars) {
  $point_id = $vars['point_id'];
  $options = !empty($vars['options']) ? $vars['options'] : array();

  $api_handler = commerce_kiala_get_handler_ls_map_details($point_id, $options);
  $vars['map_url'] = $api_handler->url;
  $vars['map_width'] = $api_handler->settings['ls_map_details_width'];
  $vars['map_height'] = $api_handler->settings['ls_map_details_height'];

  $vars['classes_array'][] = drupal_html_class('commerce-kiala-point-details-map-' . $point_id);
}
