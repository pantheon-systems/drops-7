<?php
/**
 * @file
 * Context for environment readonly
 */

/**
 * Implements hook_context_default_contexts().
 */
function environment_readonly_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'environment_readonly_alert';
  $context->description = 'Displays a message to authenticated users when the site is in read only mode';
  $context->tag = 'Environment';
  $context->conditions = array(
    'environment' => array(
      'values' => array(
        'readonly' => 'readonly',
      ),
    ),
    'user' => array(
      'values' => array(
        'authenticated user' => 'authenticated user',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'environment_readonly-environment_readonly_alert' => array(
          'module' => 'environment_readonly',
          'delta' => 'environment_readonly_alert',
          'region' => 'content',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('Displays a message to authenticated users when the site is in read only mode');
  t('Environment');
  $export['environment_readonly_alert'] = $context;

  return $export;
}
