<?php

/**
 * @file
 * Insert support for ImageField module.
 */

/**
 * Implementation of hook_insert_widgets().
 */
function image_insert_widgets() {
  return array(
    'image_image' => array(
      'element_type' => 'managed_file',
      'wrapper' => '.image-widget',
      'fields' => array(
        'alt' => 'input[name$="[alt]"], textarea[name$="[alt]"]',
        'title' => 'input[name$="[title]"], textarea[name$="[title]"]',
        'description' => 'input[name$="[description]"], textarea[name$="[description]"]',
      ),
    ),
  );
}

/**
 * Implementation of hook_insert_styles().
 */
function image_insert_styles() {
  $image_styles = image_styles();
  $insert_styles = array();
  foreach ($image_styles as $style) {
    $insert_styles['image_' . $style['name']] = array(
      'label' => t($style['name']),
    );
  }
  return $insert_styles;
}

/**
 * Implementation of hook_insert_content().
 */
function image_insert_content($item, $style, $widget) {
  $style_name = preg_replace('/^image_/', '', $style['name']);
  return theme(array('image_insert_image__' . str_replace('-', '_', $style_name), 'image_insert_image'), array('item' => $item, 'widget' => $widget, 'style_name' => $style_name));
}

/**
 * Theme the content that will be inserted for Image styles.
 */
function template_preprocess_image_insert_image(&$vars) {
  $vars['file'] = file_load($vars['item']['fid']);

  // Determine dimensions of the image after the image style transformations.
  $image_info = @image_get_info($vars['file']->uri);
  $vars['width'] = isset($image_info['width']) ? $image_info['width'] : NULL;
  $vars['height'] = isset($image_info['height']) ? $image_info['height'] : NULL;
  image_style_transform_dimensions($vars['style_name'], $vars);

  $vars['uri'] = image_style_path($vars['style_name'], $vars['file']->uri);
  $absolute = isset($vars['widget']['settings']['insert_absolute']) ? $vars['widget']['settings']['insert_absolute'] : NULL;
  $vars['url'] = insert_create_url($vars['uri'], $absolute, variable_get('clean_url'));

  // http://drupal.org/node/1923336
  if (function_exists('image_style_path_token')) {
    $token_query = array(IMAGE_DERIVATIVE_TOKEN => image_style_path_token($vars['style_name'], $vars['file']->uri));
    $vars['url'] .= (strpos($vars['url'], '?') !== FALSE ? '&' : '?') . drupal_http_build_query($token_query);
  }

  $vars['class'] = !empty($vars['widget']['settings']['insert_class']) ? $vars['widget']['settings']['insert_class'] : '';
}
