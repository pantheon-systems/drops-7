<?php

/**
 * @file
 * Insert support for generic files.
 */

/**
 * Implementation of hook_insert_styles().
 */
function insert_insert_styles() {
  $insert_styles = array();
  $insert_styles['auto'] = array('label' => t('Automatic'), 'weight' => -20);
  $insert_styles['link'] = array('label' => t('Link to file'), 'weight' => -12);
  $insert_styles['icon_link'] = array('label' => t('Link to file (with icon)'), 'weight' => -11);
  $insert_styles['image'] = array('label' => t('Original image'), 'weight' => -10);
  return $insert_styles;
}

/**
 * Implementation of hook_insert_content().
 */
function insert_insert_content($item, $style, $widget) {
  $style_name = $style['name'];

  if (empty($item['fid'])) {
    return '';
  }

  if ($style_name == 'auto') {
    $file = file_load($item['fid']);
    $info = @pathinfo($file->uri);
    if (in_array(strtolower($info['extension']), array('png', 'jpg', 'jpeg', 'gif'))) {
      $style_name = 'image';
    }
  }

  if ($style_name == 'image') {
    return theme('insert_image', array('item' => $item, 'widget' => $widget));
  }
  if ($style_name == 'icon_link') {
    return theme('insert_icon_link', array('item' => $item, 'widget' => $widget));
  }
  else {
    return theme('insert_link', array('item' => $item, 'widget' => $widget));
  }
}

/**
 * Preprocess variables for the insert-image.tpl.php file.
 */
function template_preprocess_insert_image(&$vars) {
  $vars['file'] = file_load($vars['item']['fid']);
  $absolute = isset($vars['widget']['settings']['insert_absolute']) ? $vars['widget']['settings']['insert_absolute'] : NULL;
  $vars['url'] = insert_create_url($vars['file']->uri, $absolute);
  $vars['class'] = !empty($vars['widget']['settings']['insert_class']) ? $vars['widget']['settings']['insert_class'] : '';
  $image_info = @image_get_info($vars['file']->uri);
  $vars['width'] = isset($image_info['width']) ? $image_info['width'] : '';
  $vars['height'] = isset($image_info['height']) ? $image_info['height'] : '';
}

/**
 * Preprocess variables for the insert-link.tpl.php file.
 */
function template_preprocess_insert_link(&$vars) {
  $vars['file'] = file_load($vars['item']['fid']);
  $absolute = isset($vars['widget']['settings']['insert_absolute']) ? $vars['widget']['settings']['insert_absolute'] : NULL;
  $vars['url'] = insert_create_url($vars['file']->uri, $absolute);
  $vars['class'] = !empty($vars['widget']['settings']['insert_class']) ? $vars['widget']['settings']['insert_class'] : '';
  $vars['name'] = $vars['file']->filename;
}

/**
 * Preprocess variables for the insert-icon-link.tpl.php file.
 */
function template_preprocess_insert_icon_link(&$vars) {
  $vars['file'] = file_load($vars['item']['fid']);
  $absolute = isset($vars['widget']['settings']['insert_absolute']) ? $vars['widget']['settings']['insert_absolute'] : NULL;
  $vars['url'] = insert_create_url($vars['file']->uri, $absolute);
  $vars['class'] = !empty($vars['widget']['settings']['insert_class']) ? $vars['widget']['settings']['insert_class'] : '';
  $vars['name'] = $vars['file']->filename;
  $vars['type'] = $vars['file']->filemime .'; length='. $vars['file']->filesize;
  $vars['icon'] = theme('file_icon', array('file' => $vars['file']));
}
