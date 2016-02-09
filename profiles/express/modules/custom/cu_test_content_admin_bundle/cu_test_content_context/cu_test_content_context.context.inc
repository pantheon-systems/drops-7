<?php
/**
 * @file
 * cu_test_content_context.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function cu_test_content_context_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_article_lists';
  $context->description = '';
  $context->tag = 'testing';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'article_list_page' => 'article_list_page',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-twitter_block' => array(
          'module' => 'bean',
          'delta' => 'twitter_block',
          'region' => 'sidebar_second',
          'weight' => '9',
        ),
        'bean-events_list_block' => array(
          'module' => 'bean',
          'delta' => 'events_list_block',
          'region' => 'sidebar_second',
          'weight' => '10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('testing');
  $export['cu_test_content_article_lists'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_articles';
  $context->description = '';
  $context->tag = 'testing';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'article' => 'article',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-article_list_block' => array(
          'module' => 'bean',
          'delta' => 'article_list_block',
          'region' => 'sidebar_second',
          'weight' => '7',
        ),
        'bean-twitter_block' => array(
          'module' => 'bean',
          'delta' => 'twitter_block',
          'region' => 'sidebar_second',
          'weight' => '9',
        ),
        'bean-events_list_block' => array(
          'module' => 'bean',
          'delta' => 'events_list_block',
          'region' => 'sidebar_second',
          'weight' => '10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('testing');
  $export['cu_test_content_articles'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_blocks';
  $context->description = '';
  $context->tag = 'testing';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'blocks' => 'blocks',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-twitter_block' => array(
          'module' => 'bean',
          'delta' => 'twitter_block',
          'region' => 'content',
          'weight' => '-10',
        ),
        'bean-events_list_block' => array(
          'module' => 'bean',
          'delta' => 'events_list_block',
          'region' => 'content',
          'weight' => '-9',
        ),
        'bean-text_block_one' => array(
          'module' => 'bean',
          'delta' => 'text_block_one',
          'region' => 'content',
          'weight' => '-8',
        ),
        'bean-text_block_two' => array(
          'module' => 'bean',
          'delta' => 'text_block_two',
          'region' => 'content',
          'weight' => '-7',
        ),
        'bean-text_block_three' => array(
          'module' => 'bean',
          'delta' => 'text_block_three',
          'region' => 'content',
          'weight' => '-6',
        ),
        'bean-text_block_four' => array(
          'module' => 'bean',
          'delta' => 'text_block_four',
          'region' => 'content',
          'weight' => '-5',
        ),
        'bean-text_block_five' => array(
          'module' => 'bean',
          'delta' => 'text_block_five',
          'region' => 'content',
          'weight' => '-4',
        ),
        'bean-text_block_six' => array(
          'module' => 'bean',
          'delta' => 'text_block_six',
          'region' => 'content',
          'weight' => '-3',
        ),
        'bean-text_block_seven' => array(
          'module' => 'bean',
          'delta' => 'text_block_seven',
          'region' => 'content',
          'weight' => '-2',
        ),
        'bean-text_block_eight' => array(
          'module' => 'bean',
          'delta' => 'text_block_eight',
          'region' => 'content',
          'weight' => '-1',
        ),
        'bean-text_block_nine' => array(
          'module' => 'bean',
          'delta' => 'text_block_nine',
          'region' => 'content',
          'weight' => '0',
        ),
        'bean-text_block_ten' => array(
          'module' => 'bean',
          'delta' => 'text_block_ten',
          'region' => 'content',
          'weight' => '1',
        ),
        'bean-text_block_eleven' => array(
          'module' => 'bean',
          'delta' => 'text_block_eleven',
          'region' => 'content',
          'weight' => '2',
        ),
        'bean-text_block_twelve' => array(
          'module' => 'bean',
          'delta' => 'text_block_twelve',
          'region' => 'content',
          'weight' => '3',
        ),
        'bean-map' => array(
          'module' => 'bean',
          'delta' => 'map',
          'region' => 'content',
          'weight' => '4',
        ),
        'bean-article_list_block' => array(
          'module' => 'bean',
          'delta' => 'article_list_block',
          'region' => 'content',
          'weight' => '5',
        ),
        'bean-article_grid_block' => array(
          'module' => 'bean',
          'delta' => 'article_grid_block',
          'region' => 'content',
          'weight' => '6',
        ),
        'bean-hero_unit' => array(
          'module' => 'bean',
          'delta' => 'hero_unit',
          'region' => 'content',
          'weight' => '7',
        ),
        'bean-social_links' => array(
          'module' => 'bean',
          'delta' => 'social_links',
          'region' => 'content',
          'weight' => '8',
        ),
        'bean-content_grid' => array(
          'module' => 'bean',
          'delta' => 'content_grid',
          'region' => 'content',
          'weight' => '9',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('testing');
  $export['cu_test_content_blocks'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_home';
  $context->description = 'Home page test content';
  $context->tag = 'testing';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-content_grid' => array(
          'module' => 'bean',
          'delta' => 'content_grid',
          'region' => 'content',
          'weight' => '8',
        ),
        'bean-map' => array(
          'module' => 'bean',
          'delta' => 'map',
          'region' => 'content',
          'weight' => '9',
        ),
        'bean-article_grid_block' => array(
          'module' => 'bean',
          'delta' => 'article_grid_block',
          'region' => 'content',
          'weight' => '10',
        ),
        'bean-hero_unit' => array(
          'module' => 'bean',
          'delta' => 'hero_unit',
          'region' => 'intro',
          'weight' => '-10',
        ),
        'bean-text_block_four' => array(
          'module' => 'bean',
          'delta' => 'text_block_four',
          'region' => 'after_content',
          'weight' => '-10',
        ),
        'bean-text_block_five' => array(
          'module' => 'bean',
          'delta' => 'text_block_five',
          'region' => 'after_content',
          'weight' => '-9',
        ),
        'bean-text_block_six' => array(
          'module' => 'bean',
          'delta' => 'text_block_six',
          'region' => 'after_content',
          'weight' => '-8',
        ),
        'bean-text_block_one' => array(
          'module' => 'bean',
          'delta' => 'text_block_one',
          'region' => 'lower',
          'weight' => '-10',
        ),
        'bean-text_block_two' => array(
          'module' => 'bean',
          'delta' => 'text_block_two',
          'region' => 'lower',
          'weight' => '-9',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Home page test content');
  t('testing');
  $export['cu_test_content_home'] = $context;

  $context = new stdClass();
  $context->disabled = TRUE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_home_2';
  $context->description = '';
  $context->tag = 'testing';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-article_feature' => array(
          'module' => 'bean',
          'delta' => 'article_feature',
          'region' => 'content',
          'weight' => '10',
        ),
        'bean-events_list_block' => array(
          'module' => 'bean',
          'delta' => 'events_list_block',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'bean-slider' => array(
          'module' => 'bean',
          'delta' => 'slider',
          'region' => 'slider',
          'weight' => '-10',
        ),
        'bean-video_reveal' => array(
          'module' => 'bean',
          'delta' => 'video_reveal',
          'region' => 'wide_2',
          'weight' => '-10',
        ),
        'bean-text_block_one' => array(
          'module' => 'bean',
          'delta' => 'text_block_one',
          'region' => 'after_content',
          'weight' => '-10',
        ),
        'bean-text_block_two' => array(
          'module' => 'bean',
          'delta' => 'text_block_two',
          'region' => 'after_content',
          'weight' => '-9',
        ),
        'bean-text_block_three' => array(
          'module' => 'bean',
          'delta' => 'text_block_three',
          'region' => 'lower',
          'weight' => '-10',
        ),
        'bean-text_block_four' => array(
          'module' => 'bean',
          'delta' => 'text_block_four',
          'region' => 'lower',
          'weight' => '-9',
        ),
        'bean-text_block_five' => array(
          'module' => 'bean',
          'delta' => 'text_block_five',
          'region' => 'lower',
          'weight' => '-8',
        ),
      ),
    ),
    'column_override' => array(
      'column_override_after_content_columns' => '2',
      'column_override_after_content_2_columns' => '3',
      'column_override_footer_columns' => 'no-change',
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('testing');
  $export['cu_test_content_home_2'] = $context;

  $context = new stdClass();
  $context->disabled = TRUE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_home_3';
  $context->description = '';
  $context->tag = 'testing';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-article_slider' => array(
          'module' => 'bean',
          'delta' => 'article_slider',
          'region' => 'content',
          'weight' => '9',
        ),
        'bean-video_reveal' => array(
          'module' => 'bean',
          'delta' => 'video_reveal',
          'region' => 'content',
          'weight' => '10',
        ),
        'bean-twitter_block' => array(
          'module' => 'bean',
          'delta' => 'twitter_block',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'bean-hero_unit' => array(
          'module' => 'bean',
          'delta' => 'hero_unit',
          'region' => 'intro',
          'weight' => '-10',
        ),
        'bean-content_grid' => array(
          'module' => 'bean',
          'delta' => 'content_grid',
          'region' => 'after_content',
          'weight' => '-6',
        ),
        'bean-text_block_one' => array(
          'module' => 'bean',
          'delta' => 'text_block_one',
          'region' => 'lower',
          'weight' => '-10',
        ),
        'bean-text_block_two' => array(
          'module' => 'bean',
          'delta' => 'text_block_two',
          'region' => 'lower',
          'weight' => '-9',
        ),
        'bean-text_block_three' => array(
          'module' => 'bean',
          'delta' => 'text_block_three',
          'region' => 'lower',
          'weight' => '-8',
        ),
        'bean-text_block_four' => array(
          'module' => 'bean',
          'delta' => 'text_block_four',
          'region' => 'lower',
          'weight' => '-7',
        ),
      ),
    ),
    'column_override' => array(
      'column_override_after_content_columns' => '1',
      'column_override_after_content_2_columns' => '2',
      'column_override_footer_columns' => 'no-change',
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('testing');
  $export['cu_test_content_home_3'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_pages';
  $context->description = '';
  $context->tag = 'testing';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'page' => 'page',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
    'path' => array(
      'values' => array(
        '~<front>' => '~<front>',
        '~blocks' => '~blocks',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'bean-text_block_eleven' => array(
          'module' => 'bean',
          'delta' => 'text_block_eleven',
          'region' => 'sidebar_second',
          'weight' => '1',
        ),
        'bean-text_block_twelve' => array(
          'module' => 'bean',
          'delta' => 'text_block_twelve',
          'region' => 'sidebar_second',
          'weight' => '2',
        ),
        'bean-events_list_block' => array(
          'module' => 'bean',
          'delta' => 'events_list_block',
          'region' => 'sidebar_second',
          'weight' => '0',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('testing');
  $export['cu_test_content_pages'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'cu_test_content_sitewide';
  $context->description = 'Sitewide test content';
  $context->tag = 'testing';
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
        'bean-text_block_seven' => array(
          'module' => 'bean',
          'delta' => 'text_block_seven',
          'region' => 'footer',
          'weight' => '-10',
        ),
        'bean-text_block_eight' => array(
          'module' => 'bean',
          'delta' => 'text_block_eight',
          'region' => 'footer',
          'weight' => '-9',
        ),
        'bean-text_block_nine' => array(
          'module' => 'bean',
          'delta' => 'text_block_nine',
          'region' => 'footer',
          'weight' => '-8',
        ),
        'bean-text_block_ten' => array(
          'module' => 'bean',
          'delta' => 'text_block_ten',
          'region' => 'footer',
          'weight' => '-7',
        ),
        'bean-social_links' => array(
          'module' => 'bean',
          'delta' => 'social_links',
          'region' => 'site_info',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Sitewide test content');
  t('testing');
  $export['cu_test_content_sitewide'] = $context;

  return $export;
}
