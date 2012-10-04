<?php
/**
 * @file
 * openpublish_section_fronts.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_section_fronts_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'section_front_page';
  $context->description = 'Used for placing blocks on all Section Front Pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'section_front' => 'section_front',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'delta_blocks-page-title' => array(
          'module' => 'delta_blocks',
          'delta' => 'page-title',
          'region' => 'header_second',
          'weight' => 0,
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used for placing blocks on all Section Front Pages.');
  $export['section_front_page'] = $context;

  return $export;
}
