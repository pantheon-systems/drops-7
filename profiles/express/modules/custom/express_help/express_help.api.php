<?php
/**
 * @file
 * Hooks for express_help.
 */

/**
 * Hook for providing page/screen level help links.
 */
function hook_express_help($variables) {
  $variables['admin/content/add'][] = array(
    'title' => 'Adding Content Help',
    'short_title' => 'Adding Content',
    'url' => 'http://www.colorado.edu/webcentral/tutorials/adding-content',
  );
  return $variables = array();
}

/**
 * Hook for providing field level help links.
 */
function hook_express_help_fields($variables) {
  $variables['article_node_form']['fields'] = array(
    'field_article_external_url' => array(
      'title' => 'External URL Help',
      'short_title' => 'External URL',
      'url' => 'http://www.colorado.edu/webcentral/tutorials/article/external_url',
    ),
    'metatags' => array(
      'title' => 'Metatags Help',
      'short_title' => 'Metatags',
      'url' => 'http://www.colorado.edu/webcentral/tutorials/article/external_url',
    ),
  );
  return $variables;
}

/**
 * Hook for providing custom paths which inlcude wildcards for help links.
 */
function hook_express_help_custom_paths($variables) {
  $variables['path/to/custom/%/page'] = 'callback-function';

  return $variables;
}
