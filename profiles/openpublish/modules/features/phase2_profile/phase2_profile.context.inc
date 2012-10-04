<?php
/**
 * @file
 * phase2_profile.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function phase2_profile_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'profile_detail_page';
  $context->description = 'Used to place blocks on Author pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'profile' => 'profile',
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
        'views-op_recent_content-block_2' => array(
          'module' => 'views',
          'delta' => 'op_recent_content-block_2',
          'region' => 'sidebar_second',
          'weight' => '-9',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used to place blocks on Author pages.');
  $export['profile_detail_page'] = $context;

  return $export;
}
