<?php
/**
 * @file
 * people_content_type.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function people_content_type_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'additional_settings__active_tab_people_list_page';
  $strongarm->value = 'edit-display';
  $export['additional_settings__active_tab_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ant_pattern_person';
  $strongarm->value = '';
  $export['ant_pattern_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ant_person';
  $strongarm->value = '0';
  $export['ant_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'attachment_links_create_alias_people_list_page';
  $strongarm->value = 0;
  $export['attachment_links_create_alias_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'attachment_links_selection_people_list_page';
  $strongarm->value = '0';
  $export['attachment_links_selection_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'auto_entitylabel_node_person';
  $strongarm->value = '1';
  $export['auto_entitylabel_node_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'auto_entitylabel_pattern_node_person';
  $strongarm->value = '[node:field-person-first-name] [node:field-person-last-name]';
  $export['auto_entitylabel_pattern_node_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'content_type_category_people_list_page';
  $strongarm->value = 'People';
  $export['content_type_category_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'content_type_category_person';
  $strongarm->value = 'People';
  $export['content_type_category_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'default_menu_link_enabled_people_list_page';
  $strongarm->value = 1;
  $export['default_menu_link_enabled_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'default_menu_link_people_list_page_enabled';
  $strongarm->value = 1;
  $export['default_menu_link_people_list_page_enabled'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'diff_enable_revisions_page_node_people_list_page';
  $strongarm->value = 1;
  $export['diff_enable_revisions_page_node_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'diff_show_preview_changes_node_people_list_page';
  $strongarm->value = 1;
  $export['diff_show_preview_changes_node_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'diff_view_mode_preview_node_people_list_page';
  $strongarm->value = 'full';
  $export['diff_view_mode_preview_node_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'field_bundle_settings_node__people_list_page';
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
      'diff_standard' => array(
        'custom_settings' => FALSE,
      ),
      'token' => array(
        'custom_settings' => FALSE,
      ),
      'promo' => array(
        'custom_settings' => FALSE,
      ),
      'grid' => array(
        'custom_settings' => FALSE,
      ),
      'sidebar' => array(
        'custom_settings' => FALSE,
      ),
      'title' => array(
        'custom_settings' => FALSE,
      ),
      'revision' => array(
        'custom_settings' => FALSE,
      ),
      'print' => array(
        'custom_settings' => FALSE,
      ),
      'table' => array(
        'custom_settings' => FALSE,
      ),
    ),
    'extra_fields' => array(
      'form' => array(
        'title' => array(
          'weight' => '0',
        ),
        'path' => array(
          'weight' => '1',
        ),
        'redirect' => array(
          'weight' => '2',
        ),
        'metatags' => array(
          'weight' => '40',
        ),
      ),
      'display' => array(
        'people_list_entity_view_1' => array(
          'default' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
          'sidebar' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
          'title' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
        ),
        'people_list_entity_view_1_form' => array(
          'default' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
          'sidebar' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
          'title' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
        ),
        'people_list_entity_view_2' => array(
          'default' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
          'sidebar' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
          'title' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
        ),
        'people_list_entity_view_2_form' => array(
          'default' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
          'sidebar' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
          'title' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
        ),
        'people_list_entity_view_3' => array(
          'default' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
          'sidebar' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
          'title' => array(
            'weight' => '10',
            'visible' => TRUE,
          ),
        ),
        'people_list_entity_view_3_form' => array(
          'default' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
          'sidebar' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
          'title' => array(
            'weight' => '9',
            'visible' => TRUE,
          ),
        ),
      ),
    ),
  );
  $export['field_bundle_settings_node__people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'field_bundle_settings_node__person';
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
        'custom_settings' => TRUE,
      ),
      'diff_standard' => array(
        'custom_settings' => FALSE,
      ),
      'title' => array(
        'custom_settings' => TRUE,
      ),
      'revision' => array(
        'custom_settings' => FALSE,
      ),
      'grid' => array(
        'custom_settings' => TRUE,
      ),
      'promo' => array(
        'custom_settings' => FALSE,
      ),
      'table' => array(
        'custom_settings' => TRUE,
      ),
      'print' => array(
        'custom_settings' => FALSE,
      ),
      'embed' => array(
        'custom_settings' => TRUE,
      ),
    ),
    'extra_fields' => array(
      'form' => array(
        'title' => array(
          'weight' => '0',
        ),
        'path' => array(
          'weight' => '1',
        ),
        'redirect' => array(
          'weight' => '2',
        ),
        'metatags' => array(
          'weight' => '40',
        ),
      ),
      'display' => array(),
    ),
  );
  $export['field_bundle_settings_node__person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_options_people_list_page';
  $strongarm->value = array(
    0 => 'menu-footer-menu',
    1 => 'main-menu',
    2 => 'menu-secondary-menu',
  );
  $export['menu_options_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_options_person';
  $strongarm->value = array(
    0 => 'main-menu',
    1 => 'menu-footer-menu',
    2 => 'menu-secondary-menu',
  );
  $export['menu_options_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_parent_people_list_page';
  $strongarm->value = 'main-menu:0';
  $export['menu_parent_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_parent_person';
  $strongarm->value = 'main-menu:0';
  $export['menu_parent_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_people_list_page';
  $strongarm->value = array(
    0 => 'status',
    1 => 'revision',
  );
  $export['node_options_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_person';
  $strongarm->value = array(
    0 => 'status',
    1 => 'revision',
  );
  $export['node_options_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_preview_people_list_page';
  $strongarm->value = '0';
  $export['node_preview_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_preview_person';
  $strongarm->value = '0';
  $export['node_preview_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_submitted_people_list_page';
  $strongarm->value = 0;
  $export['node_submitted_people_list_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_submitted_person';
  $strongarm->value = 0;
  $export['node_submitted_person'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_node_people_list_page_pattern';
  $strongarm->value = '[node:menu-link:parents:join-path]/[node:title]';
  $export['pathauto_node_people_list_page_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_node_person_pattern';
  $strongarm->value = '[node:menu-link:parents:join-path]/[node:title]';
  $export['pathauto_node_person_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_taxonomy_term_department_pattern';
  $strongarm->value = '';
  $export['pathauto_taxonomy_term_department_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_taxonomy_term_people_filter_1_pattern';
  $strongarm->value = '';
  $export['pathauto_taxonomy_term_people_filter_1_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_taxonomy_term_people_filter_2_pattern';
  $strongarm->value = '';
  $export['pathauto_taxonomy_term_people_filter_2_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_taxonomy_term_people_filter_3_pattern';
  $strongarm->value = '';
  $export['pathauto_taxonomy_term_people_filter_3_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_taxonomy_term_person_type_pattern';
  $strongarm->value = '';
  $export['pathauto_taxonomy_term_person_type_pattern'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'url_people_list_page';
  $strongarm->value = '';
  $export['url_people_list_page'] = $strongarm;

  return $export;
}
