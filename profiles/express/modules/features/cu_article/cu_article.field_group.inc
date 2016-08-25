<?php
/**
 * @file
 * cu_article.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_article_field_group_info() {
  $field_groups = array();

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
  $field_groups['group_article_byline|node|article|form'] = $field_group;

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
  $field_groups['group_article_categories|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_expose_filters|node|article_list_page|form';
  $field_group->group_name = 'group_article_expose_filters';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article_list_page';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Expose FIlters',
    'weight' => '9',
    'children' => array(
      0 => 'field_article_page_cat_expose',
      1 => 'field_article_page_tag_expose',
      2 => 'field_article_page_childterms',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Expose FIlters',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => 'group-article-expose-filters field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $field_groups['group_article_expose_filters|node|article_list_page|form'] = $field_group;

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
    'weight' => '16',
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
  $field_groups['group_article_external|node|article|form'] = $field_group;

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
    'weight' => '5',
    'children' => array(
      0 => 'group_article_byline',
      1 => 'group_article_categories',
      2 => 'group_article_external',
      3 => 'group_article_images',
      4 => 'group_article_tags',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-article-extras field-group-htabs',
      ),
    ),
  );
  $field_groups['group_article_extras|node|article|form'] = $field_group;

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
  $field_groups['group_article_images|node|article|form'] = $field_group;

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
  $field_groups['group_article_tags|node|article|form'] = $field_group;

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
      2 => 'field_article_feature_show_cat',
      3 => 'field_article_link',
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
  $field_groups['group_settings|bean|article_feature|form'] = $field_group;

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
  $field_groups['group_settings|bean|article_grid|form'] = $field_group;

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
  $field_groups['group_settings|bean|articles|form'] = $field_group;

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
      0 => 'field_article_feature_category',
      1 => 'field_article_feature_filter',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => 'Choose which articles will appear by selecting from the categories or tags below.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $field_groups['group_taxonomy_filters|bean|article_feature|form'] = $field_group;

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
      0 => 'field_article_grid_category',
      1 => 'field_article_grid_filter',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => 'Choose which articles will appear by selecting from the categories or tags below.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $field_groups['group_taxonomy_filters|bean|article_grid|form'] = $field_group;

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
      0 => 'field_article_slider_category',
      1 => 'field_article_slider_filter',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => 'Choose which articles will appear by selecting from the categories or tags below.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $field_groups['group_taxonomy_filters|bean|article_slider|form'] = $field_group;

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
      0 => 'field_article_list_category',
      1 => 'field_article_term',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => 'Choose which articles will appear by selecting from the categories or tags below.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $field_groups['group_taxonomy_filters|bean|articles|form'] = $field_group;

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
      0 => 'field_article_page_category',
      1 => 'field_article_page_filter',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-taxonomy-filters field-group-fieldset',
        'description' => 'Choose which articles will appear in the list by selecting from the categories or tags below.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $field_groups['group_taxonomy_filters|node|article_list_page|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Article Extras');
  t('Byline');
  t('Categories');
  t('Expose FIlters');
  t('External Link');
  t('Filters');
  t('Images');
  t('Settings');
  t('Tags');

  return $field_groups;
}
