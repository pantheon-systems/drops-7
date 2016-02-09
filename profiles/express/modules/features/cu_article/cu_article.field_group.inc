<?php
/**
 * @file
 * cu_article.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_article_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_byline|node|article|form';
  $field_group->group_name = 'group_article_byline';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_article_extras';
  $field_group->data = array(
    'label' => 'Byline',
    'weight' => '15',
    'children' => array(
      0 => 'field_article_byline',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-byline field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_article_byline|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_categories|node|article|form';
  $field_group->group_name = 'group_article_categories';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_article_extras';
  $field_group->data = array(
    'label' => 'Categories',
    'weight' => '13',
    'children' => array(
      0 => 'field_article_categories',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-categories field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_article_categories|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_external|node|article|form';
  $field_group->group_name = 'group_article_external';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_article_extras';
  $field_group->data = array(
    'label' => 'External Link',
    'weight' => '17',
    'children' => array(
      0 => 'field_article_external_url',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-external field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_article_external|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_extras|node|article|form';
  $field_group->group_name = 'group_article_extras';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Article Extras',
    'weight' => '9',
    'children' => array(
      0 => 'group_article_images',
      1 => 'group_article_categories',
      2 => 'group_article_tags',
      3 => 'group_article_external',
      4 => 'group_article_byline',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-article-extras field-group-htabs',
      ),
    ),
  );
  $export['group_article_extras|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_images|node|article|form';
  $field_group->group_name = 'group_article_images';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_article_extras';
  $field_group->data = array(
    'label' => 'Images',
    'weight' => '12',
    'children' => array(
      0 => 'field_article_thumbnail',
      1 => 'field_image',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-images field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_article_images|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_tags|node|article|form';
  $field_group->group_name = 'group_article_tags';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_article_extras';
  $field_group->data = array(
    'label' => 'Tags',
    'weight' => '14',
    'children' => array(
      0 => 'field_tags',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-tags field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_article_tags|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_settings|bean|article_feature|form';
  $field_group->group_name = 'group_settings';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'article_feature';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Settings',
    'weight' => '3',
    'children' => array(
      0 => 'field_article_feature_display',
      1 => 'field_article_feature_image_size',
      2 => 'field_article_link',
      3 => 'field_article_feature_show_cat',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-settings field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_settings|bean|article_feature|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_settings|bean|article_grid|form';
  $field_group->group_name = 'group_settings';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'article_grid';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Settings',
    'weight' => '3',
    'children' => array(
      0 => 'field_article_grid_items',
      1 => 'field_article_grid_more_link',
      2 => 'field_article_grid_summary',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-settings field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_settings|bean|article_grid|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_settings|bean|articles|form';
  $field_group->group_name = 'group_settings';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'articles';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Settings',
    'weight' => '3',
    'children' => array(
      0 => 'field_article_display',
      1 => 'field_article_items_display',
      2 => 'field_article_link',
      3 => 'field_article_pager',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-settings field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_settings|bean|articles|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_taxonomy_filters|bean|article_feature|form';
  $field_group->group_name = 'group_taxonomy_filters';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'article_feature';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Filters',
    'weight' => '2',
    'children' => array(
      0 => 'field_article_feature_filter',
      1 => 'field_article_feature_category',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_taxonomy_filters|bean|article_feature|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_taxonomy_filters|bean|article_grid|form';
  $field_group->group_name = 'group_taxonomy_filters';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'article_grid';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Filters',
    'weight' => '2',
    'children' => array(
      0 => 'field_article_grid_filter',
      1 => 'field_article_grid_category',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_taxonomy_filters|bean|article_grid|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_taxonomy_filters|bean|article_slider|form';
  $field_group->group_name = 'group_taxonomy_filters';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'article_slider';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Filters',
    'weight' => '2',
    'children' => array(
      0 => 'field_article_slider_filter',
      1 => 'field_article_slider_category',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_taxonomy_filters|bean|article_slider|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_taxonomy_filters|bean|articles|form';
  $field_group->group_name = 'group_taxonomy_filters';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'articles';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Filters',
    'weight' => '2',
    'children' => array(
      0 => 'field_article_term',
      1 => 'field_article_list_category',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_taxonomy_filters|bean|articles|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_taxonomy_filters|node|article_list_page|form';
  $field_group->group_name = 'group_taxonomy_filters';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article_list_page';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Filters',
    'weight' => '4',
    'children' => array(
      0 => 'field_article_page_filter',
      1 => 'field_article_page_category',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_taxonomy_filters|node|article_list_page|form'] = $field_group;

  return $export;
}
