<?php
/**
 * @file
 * openpublish_media.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_media_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'photo_gallery_detail_page';
  $context->description = 'Used to place blocks on Photo Gallery node pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'openpublish_photo_gallery' => 'openpublish_photo_gallery',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-ec0f800b3db8af0ee17f07e48a1161df' => array(
          'module' => 'views',
          'delta' => 'ec0f800b3db8af0ee17f07e48a1161df',
          'region' => 'content',
          'weight' => '1',
        ),
        'boxes-ad_300x250_1' => array(
          'module' => 'boxes',
          'delta' => 'ad_300x250_1',
          'region' => 'sidebar_second',
          'weight' => '0',
        ),
        'views-5ce78ee8d7030d935fd275a05708281b' => array(
          'module' => 'views',
          'delta' => '5ce78ee8d7030d935fd275a05708281b',
          'region' => 'sidebar_second',
          'weight' => '1',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used to place blocks on Photo Gallery node pages.');
  $export['photo_gallery_detail_page'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'video_detail_page';
  $context->description = 'Used for placing blocks on all Video node pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'openpublish_video' => 'openpublish_video',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'boxes-ad_300x250_1' => array(
          'module' => 'boxes',
          'delta' => 'ad_300x250_1',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'views-5ce78ee8d7030d935fd275a05708281b' => array(
          'module' => 'views',
          'delta' => '5ce78ee8d7030d935fd275a05708281b',
          'region' => 'sidebar_second',
          'weight' => '-9',
        ),
        'views-op_recent_content-block_3' => array(
          'module' => 'views',
          'delta' => 'op_recent_content-block_3',
          'region' => 'sidebar_second',
          'weight' => '-8',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used for placing blocks on all Video node pages.');
  $export['video_detail_page'] = $context;

  return $export;
}
