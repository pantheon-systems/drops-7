<?php
/**
 * @file
 * openpublish_blog_post.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_blog_post_context_default_contexts() {
  $export = array();

  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'blog_post_detail_page';
  $context->description = 'Used to place blocks on Blog Post node pages.';
  $context->tag = 'Node Detail Pages';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'openpublish_blog_post' => 'openpublish_blog_post',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-5ce78ee8d7030d935fd275a05708281b' => array(
          'module' => 'views',
          'delta' => '5ce78ee8d7030d935fd275a05708281b',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Node Detail Pages');
  t('Used to place blocks on Blog Post node pages.');
  $export['blog_post_detail_page'] = $context;

  return $export;
}
