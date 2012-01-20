<?php
/**
 * @file
 * a.context.inc
 */

/**
 * Implementation of hook_context_default_contexts().
 */
function context_field_context_default_contexts() {
  $export = array();

  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'context_field_default_context';
  $context->description = 'Default Context';
  $context->tag = 'Context Field Default';
  $context->conditions = array(
    'context_field' => array(
      'values' => array(
        1 => 1,
        2 => 2,
      ),
    ),
  );
  $context->reactions = array();
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Context Field Default');
  t('Default Context');
  $export['context_field_default_context'] = $context;

  return $export;
}
