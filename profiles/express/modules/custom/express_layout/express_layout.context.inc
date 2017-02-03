<?php
/**
 * @file
 * express_layout.context.inc
 */

/**
 * Context condition for entity layouts.
 */
class express_layout_layout_context_condition extends context_condition_node {
  function condition_values() {
    return _express_layout_form_options();
  }
  function execute($node) {
    $layout = entity_load_single('express_layout', $node->nid);
    foreach ($this->get_contexts($layout->field_feature_layout_toggle[LANGUAGE_NONE][0]['value']) as $context) {
      $this->condition_met($context, $layout->field_feature_layout_toggle[LANGUAGE_NONE][0]['value']);
    }
  }
}

/**
 * Implements hook_context_default_contexts().
 */
function express_layout_context_default_contexts() {

  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'express_layout_blocks';
  $context->description = 'Places blocks on a page according to Express Layout settings';
  $context->tag = 'locked';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'node/*' => 'node/*',
        '~node/*/edit' => '~node/*/edit',
        '~node/*/layout' => '~node/*/layout',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'express_layout-el_block_header' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_header',
          'region' => 'content',
          'weight' => '-10',
        ),
        'express_layout-el_block_footer' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_footer',
          'region' => 'content',
          'weight' => '10',
        ),
        'express_layout-el_block_sidebar_first' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_sidebar_first',
          'region' => 'sidebar_first',
          'weight' => '-10',
        ),
        'express_layout-el_block_sidebar_second' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_sidebar_second',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'express_layout-el_block_intro' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_intro',
          'region' => 'intro',
          'weight' => '-10',
        ),
        'express_layout-el_block_slider' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_slider',
          'region' => 'slider',
          'weight' => '-10',
        ),
        'express_layout-el_block_wide_2' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_wide_2',
          'region' => 'wide_2',
          'weight' => '-10',
        ),
        'express_layout-el_block_inner_content_right' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_inner_content_right',
          'region' => 'content_sidebar_right',
          'weight' => '-10',
        ),
        'express_layout-el_block_inner_content_left' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_inner_content_left',
          'region' => 'content_sidebar_left',
          'weight' => '-10',
        ),
        'express_layout-el_block_inner_content_bottom' => array(
          'module' => 'express_layout',
          'delta' => 'el_block_inner_content_bottom',
          'region' => 'content_bottom',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

// Translatables
// Included for use with string extractors like potx.
  t('Places blocks on a page according to Express Layout settings');

  $export['express_layout_blocks'] = $context;

  return $export;
}
