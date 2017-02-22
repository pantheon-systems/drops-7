<?php
/**
 * @file
 * panopoly_widgets.linkit_profiles.inc
 */

/**
 * Implements hook_default_linkit_profiles().
 */
function panopoly_widgets_default_linkit_profiles() {
  $export = array();

  $linkit_profile = new LinkitProfile();
  $linkit_profile->disabled = FALSE; /* Edit this to true to make a default linkit_profile disabled initially */
  $linkit_profile->api_version = 1;
  $linkit_profile->name = 'content_fields';
  $linkit_profile->admin_title = 'Content Fields';
  $linkit_profile->admin_description = '';
  $linkit_profile->profile_type = '2';
  $linkit_profile->data = array(
    'search_plugins' => array(
      'entity:menu_link' => array(
        'enabled' => 0,
        'weight' => '-10',
      ),
      'entity:search_api_server' => array(
        'enabled' => 0,
        'weight' => '-10',
      ),
      'entity:search_api_index' => array(
        'enabled' => 0,
        'weight' => '-10',
      ),
      'entity:node' => array(
        'enabled' => 1,
        'weight' => '0',
      ),
      'entity:taxonomy_term' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
      'entity:user' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
      'entity:file' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
    ),
    'entity:node' => array(
      'result_description' => '[node:title]',
      'bundles' => array(
        'panopoly_page' => 0,
        'manual_testing' => 0,
        'panopoly_test_page' => 0,
      ),
      'group_by_bundle' => 0,
      'include_unpublished' => 0,
    ),
    'entity:search_api_server' => array(
      'result_description' => '',
      'include_unpublished' => 0,
    ),
    'entity:search_api_index' => array(
      'result_description' => '',
      'include_unpublished' => 0,
    ),
    'entity:file' => array(
      'result_description' => '',
      'bundles' => array(
        'image' => 0,
        'video' => 0,
        'audio' => 0,
        'document' => 0,
      ),
      'group_by_bundle' => 0,
      'show_scheme' => 0,
      'group_by_scheme' => 0,
      'image_extra_info' => array(
        'thumbnail' => 'thumbnail',
        'dimensions' => 'dimensions',
      ),
    ),
    'entity:taxonomy_term' => array(
      'result_description' => '',
    ),
    'entity:user' => array(
      'result_description' => '',
    ),
    'entity:menu_link' => array(
      'result_description' => '',
      'bundles' => array(
        'devel' => 0,
        'main-menu' => 0,
        'management' => 0,
        'navigation' => 0,
        'user-menu' => 0,
      ),
      'group_by_bundle' => 0,
    ),
    'insert_plugin' => array(
      'plugin' => 'raw_url',
      'url_method' => '1',
    ),
    'attribute_plugins' => array(
      'target' => array(
        'enabled' => 0,
        'weight' => '-10',
      ),
      'class' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
      'accesskey' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
      'rel' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
      'title' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
      'id' => array(
        'enabled' => 0,
        'weight' => '0',
      ),
    ),
    'autocomplete' => array(
      'charLimit' => '3',
      'wait' => '350',
      'remoteTimeout' => '10000',
    ),
  );
  $export['content_fields'] = $linkit_profile;

  return $export;
}
