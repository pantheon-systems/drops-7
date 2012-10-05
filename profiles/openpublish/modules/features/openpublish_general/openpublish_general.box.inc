<?php
/**
 * @file
 * openpublish_general.box.inc
 */

/**
 * Implements hook_default_box().
 */
function openpublish_general_default_box() {
  $export = array();

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'ad_728x90_1';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Ad: 728x90 Bottom';
  $box->options = array(
    'body' => array(
      'value' => '<p><img alt="OpenPublish: A smart solution for today\'s publishers." src="http://jcaldwell.openpublish.dev6.fayze2.com/profiles/openpublish/images/banner_openpublish.jpg" style="width: 728px; height: 90px; " /></p>',
      'format' => 'full_html',
    ),
    'additional_classes' => '',
  );
  $export['ad_728x90_1'] = $box;

  return $export;
}
