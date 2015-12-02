<?php
/**
 * @file
 * cu_test_content.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function cu_test_content_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'test_content_context';
  $context->description = 'A context for test content';
  $context->tag = '';
  $context->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-text_block_two' => array(
          'module' => 'bean',
          'delta' => 'text_block_two',
          'region' => 'after_content',
          'weight' => '-10',
        ),
        'bean-text_block_three' => array(
          'module' => 'bean',
          'delta' => 'text_block_three',
          'region' => 'after_content',
          'weight' => '-9',
        ),
        'bean-text_block_four' => array(
          'module' => 'bean',
          'delta' => 'text_block_four',
          'region' => 'after_content',
          'weight' => '-8',
        ),
        'bean-text_block_five' => array(
          'module' => 'bean',
          'delta' => 'text_block_five',
          'region' => 'lower',
          'weight' => '-9',
        ),
        'bean-text_block_six' => array(
          'module' => 'bean',
          'delta' => 'text_block_six',
          'region' => 'lower',
          'weight' => '-8',
        ),
        'bean-footer_block_one' => array(
          'module' => 'bean',
          'delta' => 'footer_block_one',
          'region' => 'footer',
          'weight' => '-10',
        ),
        'bean-footer_block_two' => array(
          'module' => 'bean',
          'delta' => 'footer_block_two',
          'region' => 'footer',
          'weight' => '-9',
        ),
        'bean-footer_block_three' => array(
          'module' => 'bean',
          'delta' => 'footer_block_three',
          'region' => 'footer',
          'weight' => '-8',
        ),
        'bean-footer_block_four' => array(
          'module' => 'bean',
          'delta' => 'footer_block_four',
          'region' => 'footer',
          'weight' => '-7',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('A context for test content');
  $export['test_content_context'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'test_content_context_2';
  $context->description = 'A context for test content for non-homepages';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '~<front>' => '~<front>',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-events_block' => array(
          'module' => 'bean',
          'delta' => 'events_block',
          'region' => 'sidebar_second',
          'weight' => '0',
        ),
        'bean-text_block_one' => array(
          'module' => 'bean',
          'delta' => 'text_block_one',
          'region' => 'sidebar_second',
          'weight' => '5',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('A context for test content for non-homepages');
  $export['test_content_context_2'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'test_content_context_home';
  $context->description = 'Context for home page test content';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-slider_example' => array(
          'module' => 'bean',
          'delta' => 'slider_example',
          'region' => 'slider',
          'weight' => '-10',
        ),
        'bean-twitter_block' => array(
          'module' => 'bean',
          'delta' => 'twitter_block',
          'region' => 'content',
          'weight' => '10',
        ),
        'bean-latest_news' => array(
          'module' => 'bean',
          'delta' => 'latest_news',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Context for home page test content');
  $export['test_content_context_home'] = $context;

  return $export;
}
