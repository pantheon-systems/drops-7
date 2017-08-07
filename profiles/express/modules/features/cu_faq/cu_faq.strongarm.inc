<?php
/**
 * @file
 * cu_faq.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function cu_faq_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'additional_settings__active_tab_faqs';
  $strongarm->value = 'edit-menu';
  $export['additional_settings__active_tab_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'attachment_links_create_alias_faqs';
  $strongarm->value = 0;
  $export['attachment_links_create_alias_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'attachment_links_selection_faqs';
  $strongarm->value = '0';
  $export['attachment_links_selection_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'default_menu_link_enabled_faqs';
  $strongarm->value = 1;
  $export['default_menu_link_enabled_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'default_menu_link_faqs_enabled';
  $strongarm->value = 1;
  $export['default_menu_link_faqs_enabled'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'diff_enable_revisions_page_node_faqs';
  $strongarm->value = 1;
  $export['diff_enable_revisions_page_node_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'diff_show_preview_changes_node_faqs';
  $strongarm->value = 1;
  $export['diff_show_preview_changes_node_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'diff_view_mode_preview_node_faqs';
  $strongarm->value = 'full';
  $export['diff_view_mode_preview_node_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'field_bundle_settings_node__faqs';
  $strongarm->value = array(
    'view_modes' => array(
      'teaser' => array(
        'custom_settings' => TRUE,
      ),
      'full' => array(
        'custom_settings' => FALSE,
      ),
      'rss' => array(
        'custom_settings' => FALSE,
      ),
      'token' => array(
        'custom_settings' => FALSE,
      ),
      'sidebar' => array(
        'custom_settings' => FALSE,
      ),
      'article_blocks' => array(
        'custom_settings' => FALSE,
      ),
      'diff_standard' => array(
        'custom_settings' => FALSE,
      ),
      'title' => array(
        'custom_settings' => FALSE,
      ),
      'revision' => array(
        'custom_settings' => FALSE,
      ),
    ),
    'extra_fields' => array(
      'form' => array(
        'title' => array(
          'weight' => '0',
        ),
        'path' => array(
          'weight' => '3',
        ),
        'redirect' => array(
          'weight' => '2',
        ),
      ),
      'display' => array(
        'faqs_entity_view_1' => array(
          'default' => array(
            'weight' => '2',
            'visible' => TRUE,
          ),
        ),
        'faq_accordion_entity_view_1' => array(
          'default' => array(
            'weight' => '1',
            'visible' => TRUE,
          ),
        ),
        'cu_faq_entity_view_1' => array(
          'default' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
        ),
      ),
    ),
  );
  $export['field_bundle_settings_node__faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_options_faqs';
  $strongarm->value = array(
    0 => 'menu-footer-menu',
    1 => 'main-menu',
    2 => 'menu-secondary-menu',
  );
  $export['menu_options_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_parent_faqs';
  $strongarm->value = 'main-menu:0';
  $export['menu_parent_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_faqs';
  $strongarm->value = array(
    0 => 'status',
    1 => 'revision',
  );
  $export['node_options_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_preview_faqs';
  $strongarm->value = '0';
  $export['node_preview_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_submitted_faqs';
  $strongarm->value = 0;
  $export['node_submitted_faqs'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_node_faqs_pattern';
  $strongarm->value = '[node:menu-link:parent:title]/[node:title]';
  $export['pathauto_node_faqs_pattern'] = $strongarm;

  return $export;
}
