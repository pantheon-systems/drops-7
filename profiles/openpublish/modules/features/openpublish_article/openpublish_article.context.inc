<?php
/**
 * @file
 * openpublish_article.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_article_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'article_detail_page';
  $context->description = 'Used to place blocks on Article node pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'openpublish_article' => 'openpublish_article',
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
          'weight' => 0,
        ),
        'user-new' => array(
          'module' => 'user',
          'delta' => 'new',
          'region' => 'sidebar_second',
          'weight' => 1,
        ),
        'views-5ce78ee8d7030d935fd275a05708281b' => array(
          'module' => 'views',
          'delta' => '5ce78ee8d7030d935fd275a05708281b',
          'region' => 'sidebar_second',
          'weight' => 2,
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used to place blocks on Article node pages.');
  $export['article_detail_page'] = $context;

  return $export;
}
