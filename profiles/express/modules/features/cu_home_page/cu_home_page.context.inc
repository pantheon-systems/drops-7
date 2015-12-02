<?php
/**
 * @file
 * cu_home_page.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function cu_home_page_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'homepage';
  $context->description = 'Active only on the homepage of the site.';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
  );
  $context->reactions = array();
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Active only on the homepage of the site.');
  $export['homepage'] = $context;

  return $export;
}
