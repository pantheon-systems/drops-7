<?php
/**
 * @file
 * cu_block_selector.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function cu_block_selector_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_block_selector';
  $context->description = '';
  $context->tag = 'locked';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'node/*' => 'node/*',
        '~node/*/edit' => '~node/*/edit',
        '~node/*/delete' => '~node/*/delete',
        '~node/*/blocks' => '~node/*/blocks',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'cu_block_selector-cu_block_header' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_header',
          'region' => 'content',
          'weight' => '-10',
        ),
        'cu_block_selector-cu_block_footer' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_footer',
          'region' => 'content',
          'weight' => '10',
        ),
        'cu_block_selector-cu_block_inner_content_left' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_inner_content_left',
          'region' => 'content_sidebar_left',
          'weight' => '0',
        ),
        'cu_block_selector-cu_block_inner_content_right' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_inner_content_right',
          'region' => 'content_sidebar_right',
          'weight' => '0',
        ),
        'cu_block_selector-cu_block_sidebar_first' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_sidebar_first',
          'region' => 'sidebar_first',
          'weight' => '0',
        ),
        'cu_block_selector-cu_block_sidebar_second' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_sidebar_second',
          'region' => 'sidebar_second',
          'weight' => '0',
        ),
        'cu_block_selector-cu_block_intro' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_intro',
          'region' => 'intro',
          'weight' => '0',
        ),
        'cu_block_selector-cu_block_slider' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_slider',
          'region' => 'slider',
          'weight' => '0',
        ),
        'cu_block_selector-cu_block_wide_2' => array(
          'module' => 'cu_block_selector',
          'delta' => 'cu_block_wide_2',
          'region' => 'wide_2',
          'weight' => '0',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('locked');
  $export['cu_block_selector'] = $context;

  return $export;
}
