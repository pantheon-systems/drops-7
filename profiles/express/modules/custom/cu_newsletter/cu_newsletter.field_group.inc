<?php
/**
 * @file
 * cu_newsletter.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_newsletter_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_newsletter_ads|node|newsletter|form';
  $field_group->group_name = 'group_newsletter_ads';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'newsletter';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_newsletter';
  $field_group->data = array(
    'label' => 'Ads/Promos',
    'weight' => '22',
    'children' => array(
      0 => 'field_newsletter_ad_promo',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-newsletter-ads field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $field_groups['group_newsletter_ads|node|newsletter|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_newsletter_blocks|node|newsletter|form';
  $field_group->group_name = 'group_newsletter_blocks';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'newsletter';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_newsletter';
  $field_group->data = array(
    'label' => 'Blocks',
    'weight' => '21',
    'children' => array(
      0 => 'field_newsletter_text_block',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-newsletter-blocks field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $field_groups['group_newsletter_blocks|node|newsletter|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_newsletter_intro|node|newsletter|form';
  $field_group->group_name = 'group_newsletter_intro';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'newsletter';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_newsletter';
  $field_group->data = array(
    'label' => 'Intro',
    'weight' => '19',
    'children' => array(
      0 => 'body',
      1 => 'field_newsletter_intro_image',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-newsletter-intro field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $field_groups['group_newsletter_intro|node|newsletter|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_newsletter_sections|node|newsletter|form';
  $field_group->group_name = 'group_newsletter_sections';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'newsletter';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_newsletter';
  $field_group->data = array(
    'label' => 'Sections',
    'weight' => '20',
    'children' => array(
      0 => 'field_newsletter_section',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-newsletter-sections field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $field_groups['group_newsletter_sections|node|newsletter|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_newsletter|node|newsletter|form';
  $field_group->group_name = 'group_newsletter';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'newsletter';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Elements',
    'weight' => '5',
    'children' => array(
      0 => 'group_newsletter_ads',
      1 => 'group_newsletter_blocks',
      2 => 'group_newsletter_intro',
      3 => 'group_newsletter_sections',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-newsletter field-group-htabs',
      ),
    ),
  );
  $field_groups['group_newsletter|node|newsletter|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_nl_section_articles|field_collection_item|field_newsletter_section_content|form';
  $field_group->group_name = 'group_nl_section_articles';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_newsletter_section_content';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_nl_section_contents';
  $field_group->data = array(
    'label' => 'Articles',
    'weight' => '1',
    'children' => array(
      0 => 'field_newsletter_articles',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-nl-section-articles field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_nl_section_articles|field_collection_item|field_newsletter_section_content|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_nl_section_contents|field_collection_item|field_newsletter_section_content|form';
  $field_group->group_name = 'group_nl_section_contents';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_newsletter_section_content';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Section Contents',
    'weight' => '0',
    'children' => array(
      0 => 'group_nl_section_articles',
      1 => 'group_nl_section_custom',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-nl-section-contents field-group-htabs',
        'id' => '',
      ),
    ),
  );
  $field_groups['group_nl_section_contents|field_collection_item|field_newsletter_section_content|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_nl_section_custom|field_collection_item|field_newsletter_section_content|form';
  $field_group->group_name = 'group_nl_section_custom';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_newsletter_section_content';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_nl_section_contents';
  $field_group->data = array(
    'label' => 'Custom',
    'weight' => '2',
    'children' => array(
      0 => 'field_nl_section_content_body',
      1 => 'field_nl_section_content_image',
      2 => 'field_nl_section_content_title',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-nl-section-custom field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_nl_section_custom|field_collection_item|field_newsletter_section_content|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Ads/Promos');
  t('Articles');
  t('Blocks');
  t('Custom');
  t('Elements');
  t('Intro');
  t('Section Contents');
  t('Sections');

  return $field_groups;
}
