<?php

/**
 * Implements hook_context_default_contexts().
 */
function cu_share_context_default_contexts() {
  $export = array();
  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'social_sharing_links';
  $context->description = 'Decide which pages Social Sharing links should appear on.';
  $context->tag = 'social_sharing_links';
  $context->conditions = array(
    'sitewide_public' => array(
      'values' => array(
        1 => 1,
      ),
    ),
    'path' => array(
      'values' => array(
        '~<front>' => '~<front>',
      ),
    ),
  );
  $context->reactions = array(
    'cu_share' => array(
      'cu_share' => 1,
    ),
  );
  $context->condition_mode = 1;
// Translatables
// Included for use with string extractors like potx.
  t('Decide which pages Social Sharing links should appear on');
  $export['cu_share'] = $context;
  return $export;
}
