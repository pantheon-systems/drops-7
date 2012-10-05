<?php
/**
 * @file
 * openpublish_article.box.inc
 */

/**
 * Implements hook_default_box().
 */
function openpublish_article_default_box() {
  $export = array();

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'ad_300x250_1';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Ad: 300x250';
  $box->options = array(
    'body' => array(
      'value' => '<p><img alt="&quot;We help you be awesome!&quot; - Phase2 Technology" src="http://jcaldwell.openpublish.dev6.fayze2.com/profiles/openpublish/images/banner_phase2.jpg" /></p>',
      'format' => 'full_html',
    ),
    'additional_classes' => '',
  );
  $export['ad_300x250_1'] = $box;

  return $export;
}
