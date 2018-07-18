<?php
/**
 * @file
 * cu_article.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_article_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'article_feature';
  $bean_type->label = 'Article Feature';
  $bean_type->options = '';
  $bean_type->description = 'A display of the latest articles where the first article is displayed with a large image and summary, while the remaining articles are include only the title and thumbnail.';
  $export['article_feature'] = $bean_type;

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'article_grid';
  $bean_type->label = 'Article Grid';
  $bean_type->options = '';
  $bean_type->description = 'Articles formatted in a grid layout';
  $export['article_grid'] = $bean_type;

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'article_slider';
  $bean_type->label = 'Article Slider';
  $bean_type->options = '';
  $bean_type->description = 'A content slider of articles. For articles to appear in the slider, they must have a thumbnail image attached.';
  $export['article_slider'] = $bean_type;

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'articles';
  $bean_type->label = 'Article List';
  $bean_type->options = '';
  $bean_type->description = 'A list of articles with configuration options such as filters, templates and items to display';
  $export['articles'] = $bean_type;

  return $export;
}
