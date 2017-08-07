<?php
/**
 * @file
 * cu_linkchecker.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function cu_linkchecker_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_action_status_code_301';
  $strongarm->value = '0';
  $export['linkchecker_action_status_code_301'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_action_status_code_404';
  $strongarm->value = '0';
  $export['linkchecker_action_status_code_404'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_check_connections_max';
  $strongarm->value = '8';
  $export['linkchecker_check_connections_max'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_check_library';
  $strongarm->value = 'core';
  $export['linkchecker_check_library'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_check_links_interval';
  $strongarm->value = '86400';
  $export['linkchecker_check_links_interval'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_check_links_types';
  $strongarm->value = '0';
  $export['linkchecker_check_links_types'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_check_useragent';
  $strongarm->value = 'Drupal (+http://drupal.org/)';
  $export['linkchecker_check_useragent'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_disable_cron_link_check';
  $strongarm->value = 0;
  $export['linkchecker_disable_cron_link_check'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_disable_link_check_for_urls';
  $strongarm->value = 'example.com
example.net
example.org';
  $export['linkchecker_disable_link_check_for_urls'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_a';
  $strongarm->value = 1;
  $export['linkchecker_extract_from_a'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_audio';
  $strongarm->value = 0;
  $export['linkchecker_extract_from_audio'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_embed';
  $strongarm->value = 0;
  $export['linkchecker_extract_from_embed'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_iframe';
  $strongarm->value = 0;
  $export['linkchecker_extract_from_iframe'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_img';
  $strongarm->value = 1;
  $export['linkchecker_extract_from_img'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_object';
  $strongarm->value = 0;
  $export['linkchecker_extract_from_object'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_extract_from_video';
  $strongarm->value = 0;
  $export['linkchecker_extract_from_video'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_filter_blacklist';
  $strongarm->value = array(
    'filter_autop' => 'filter_autop',
    'ckeditor_link_filter' => 0,
    'filter_url' => 0,
    'transliteration' => 0,
    'pathologic' => 0,
    'filter_htmlcorrector' => 0,
    'filter_html_escape' => 0,
    'filter_html' => 0,
    'shortcode' => 0,
    'shortcode_text_corrector' => 0,
    'video_embed_field' => 0,
    'video_filter' => 0,
    'wysiwyg' => 0,
  );
  $export['linkchecker_filter_blacklist'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_ignore_response_codes';
  $strongarm->value = '200
206
301
302
304
401
403';
  $export['linkchecker_ignore_response_codes'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_log_level';
  $strongarm->value = '6';
  $export['linkchecker_log_level'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_scan_beans';
  $strongarm->value = array(
    'block' => 'block',
    'block_row' => 'block_row',
    'block_section' => 'block_section',
    'content_list' => 'content_list',
    'cu_events_calendar_block' => 'cu_events_calendar_block',
    'events_calendar_grid' => 'events_calendar_grid',
    'facebook_activity' => 'facebook_activity',
    'facebook_like_button' => 'facebook_like_button',
    'feature_callout' => 'feature_callout',
    'hero_unit' => 'hero_unit',
    'rss' => 'rss',
    'slider' => 'slider',
    'social_links' => 'social_links',
    'twitter_block' => 'twitter_block',
    'video_reveal' => 'video_reveal',
    'quicktab' => 'quicktab',
    'article_feature' => 'article_feature',
    'article_grid' => 'article_grid',
    'article_slider' => 'article_slider',
    'articles' => 'articles',
    'people_list_block' => 'people_list_block',
  );
  $export['linkchecker_scan_beans'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_scan_blocks';
  $strongarm->value = 1;
  $export['linkchecker_scan_blocks'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_scan_comments';
  $strongarm->value = 0;
  $export['linkchecker_scan_comments'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'linkchecker_scan_nodetypes';
  $strongarm->value = array(
    'article' => 'article',
    'article_list_page' => 'article_list_page',
    'page' => 'page',
    'content_list_page' => 'content_list_page',
    'faqs' => 'faqs',
    'people_list_page' => 'people_list_page',
    'person' => 'person',
    'photo_gallery' => 'photo_gallery',
    'webform' => 'webform',
    'file' => 0,
    'video' => 0,
  );
  $export['linkchecker_scan_nodetypes'] = $strongarm;

  return $export;
}
