<?php
/**
 * @file
 * Video filter embedded code to add video as filter and using WYSIWYG
 */

/**
 * Implements hook_wysiwyg_include_directory().
 */
function video_wysiwyg_include_directory($type) {
  switch ($type) {
    case 'plugins':
      return 'wysiwyg_plugins';
      break;
  }
}

/**
 * Implements hook_filter_info().
 *
 * @todo For performance, should allow to cache by default.
 */
function video_filter_info() {
  $filters['video_filter'] = array(
    'title' => t('Converts video tags to embedded code'),
    'description' => t('This filter will convert [content:video] tag into markup.'),
    'process callback' => 'video_filter',
    'tips callback' => 'video_filter_tips', // @TODO not implemented
  );

  // If the WYSIWYG module is enabled, add additional help.
  if (module_exists('wysiwyg')) {
    $filters['video_filter']['description'] .= ' ' . t('This must be enabled for the WYSIWYG integration to work correctly with this text filter.');
  }

  return $filters;
}

/**
 * advertisement_filter callbase from video_filter_info()
 */
function video_filter($text) {
  $text = ' ' . $text . ' ';
  $text = preg_replace_callback("/\[.*?]/s", 'video_token_to_markup', $text);
  return $text;
}

/**
 * Create markup from tokens
 */
function video_token_to_markup($match) {
  $match = str_replace("[", "", $match);
  $match = str_replace("]", "", $match);
  $tag = $match[0];
  $markup = '';
  switch ($tag) {
    case 'content:video':
      $markup = '<img src="http://placehold.it/250x128&text=WYSYWIG+plugin+under+development--+250x128"/>';
      break;
  }
  return $markup;
}
