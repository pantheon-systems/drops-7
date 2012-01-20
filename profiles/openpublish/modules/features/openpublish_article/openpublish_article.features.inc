<?php
/**
 * @file
 * openpublish_article.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_article_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function openpublish_article_image_default_styles() {
  $styles = array();

  // Exported image style: 400px_wide
  $styles['400px_wide'] = array(
    'name' => '400px_wide',
    'effects' => array(
      2 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => '400',
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => '1',
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function openpublish_article_node_info() {
  $items = array(
    'openpublish_article' => array(
      'name' => t('Article'),
      'base' => 'node_content',
      'description' => t('Used to create individual stories as would appear in News, Sport, or Business content areas.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
