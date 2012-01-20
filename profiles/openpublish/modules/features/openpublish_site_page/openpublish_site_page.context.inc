<?php
/**
 * @file
 * openpublish_site_page.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_site_page_context_default_contexts() {
  $export = array();

  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'site_page_detail_page';
  $context->description = 'Used for placing blocks on all Site Pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'openpublish_site_page' => 'openpublish_site_page',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
  );
  $context->reactions = array();
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used for placing blocks on all Site Pages.');
  $export['site_page_detail_page'] = $context;

  return $export;
}
