<?php
/**
 * @file
 * openpublish_general.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_general_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'responsive_extra_blocks';
  $context->description = 'Repeat the addition of certain blocks for use in responsive layouts';
  $context->tag = 'sitewide';
  $context->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'search-form' => array(
          'module' => 'search',
          'delta' => 'form',
          'region' => 'footer_first',
          'weight' => 0,
        ),
        'system-user-menu' => array(
          'module' => 'system',
          'delta' => 'user-menu',
          'region' => 'footer_first',
          'weight' => 2,
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Repeat the addition of certain blocks for use in responsive layouts');
  t('sitewide');
  $export['responsive_extra_blocks'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'responsive_extra_blocks_2';
  $context->description = 'Repeat the addition of certain blocks for use in responsive layouts';
  $context->tag = 'sitewide';
  $context->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'system-user-menu' => array(
          'module' => 'system',
          'delta' => 'user-menu',
          'region' => 'user_first',
          'weight' => 1,
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Repeat the addition of certain blocks for use in responsive layouts');
  t('sitewide');
  $export['responsive_extra_blocks_2'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'sitewide';
  $context->description = 'Adds content to the proper region as required by Omega themes.';
  $context->tag = 'sitewide';
  $context->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'search-form' => array(
          'module' => 'search',
          'delta' => 'form',
          'region' => 'user_first',
          'weight' => 0,
        ),
        'menu-menu-utility-nav' => array(
          'module' => 'menu',
          'delta' => 'menu-utility-nav',
          'region' => 'user_first',
          'weight' => 2,
        ),
        'system-user-menu' => array(
          'module' => 'system',
          'delta' => 'user-menu',
          'region' => 'user_second',
          'weight' => 0,
        ),
        'system-main' => array(
          'module' => 'system',
          'delta' => 'main',
          'region' => 'content',
          'weight' => 1,
        ),
        'menu-menu-footer-links' => array(
          'module' => 'menu',
          'delta' => 'menu-footer-links',
          'region' => 'footer_first',
          'weight' => 1,
        ),
        'boxes-ad_728x90_1' => array(
          'module' => 'boxes',
          'delta' => 'ad_728x90_1',
          'region' => 'footer_second',
          'weight' => 0,
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Adds content to the proper region as required by Omega themes.');
  t('sitewide');
  $export['sitewide'] = $context;

  return $export;
}
