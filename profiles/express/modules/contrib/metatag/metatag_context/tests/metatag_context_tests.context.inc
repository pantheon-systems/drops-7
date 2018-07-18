<?php

/**
 * @file
 * Context reaction for Metatag:Context tests.
 */

/**
 * Implements hook_context_default_contexts().
 */
function metatag_context_tests_context_default_contexts() {
  $defaults = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'metatag_context_test';
  $context->description = 'A default Metatag:Context definition for a test page.';
  $context->tag = 'Metatag';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'metatag-context-test' => 'metatag-context-test',
      ),
    ),
  );
  $context->reactions = array(
    'metatag_context_reaction' => array(
      'metatags' => array(
        'und' => array(
          'title' => array(
            'value' => 'Metatag:Context test page title tag',
          ),
          'description' => array(
            'value' => 'Metatag:Context test description tag.',
          ),
          'abstract' => array(
            'value' => '',
          ),
          'keywords' => array(
            'value' => 'Test, page, keywords',
          ),
        ),
      ),
      'metatag_admin' => 1,
    ),
  );
  $context->condition_mode = 1;
  $defaults[$context->name] = $context;

  // Translatables
  // Included for use with string extractors like potx.
  t('A default Metatag:Context definition for a test page.');
  t('Metatag');

  return $defaults;
}
