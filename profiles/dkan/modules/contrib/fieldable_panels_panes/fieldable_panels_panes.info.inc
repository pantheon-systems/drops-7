<?php

/**
 * @file
 * Provides info about the node entity.
 */

/**
 * Implements hook_entity_property_info().
 *
 * @see entity_entity_property_info()
 */
function fieldable_panels_panes_entity_property_info() {
  $info = array();
  $properties = &$info['fieldable_panels_pane']['properties'];

  $default_properties = array(
    'setter callback' => 'entity_property_verbatim_set',
  );

  $properties['fpid'] = array(
    'label' => t('ID'),
    'type' => 'integer',
    'description' => t('The unique ID of the entity.'),
    'schema field' => 'fpid',
  );
  $properties['vid'] = array(
    'label' => t('Revision ID'),
    'type' => 'integer',
    'description' => t("The unique ID of the entity's revision."),
    'schema field' => 'vid',
  );
  $properties['is_new'] = array(
    'label' => t('Is new'),
    'type' => 'boolean',
    'description' => t('Whether the FPP is new and not saved to the database yet.'),
  ) + $default_properties;
  $properties['bundle'] = array(
    'label' => t('Bundle'),
    'type' => 'token',
    'description' => t('The type of the Fieldable Panels Pane.'),
    'setter permission' => 'administer fieldable panels panes',
    'options list' => 'fieldable_panels_panes_get_bundle_labels',
    'required' => TRUE,
    'schema field' => 'bundle',
  ) + $default_properties;
  $properties['title'] = array(
    'label' => t('Title'),
    'description' => t('The title of the entity.'),
    'schema field' => 'title',
    'required' => TRUE,
  ) + $default_properties;
  $properties['admin_title'] = array(
    'label' => t('Admin title'),
    'description' => t("The title as it will appear in the Add Content dialog."),
    'getter callback' => 'fieldable_panels_panes_entity_title',
    'schema field' => 'admin_title',
  ) + $default_properties;
  $properties['admin_description'] = array(
    'label' => t("Admin description"),
    'description' => t('The description as it will appear in the Add Content dialog.'),
    'schema field' => 'admin_description',
  ) + $default_properties;
  $properties['category'] = array(
    'label' => t('Category'),
    'description' => t('The category under which it will appear in the Add Content dialog.'),
    'schema field' => 'category',
  ) + $default_properties;
  $properties['language'] = array(
    'label' => t('Language'),
    'type' => 'token',
    'description' => t('The language the entity is written in.'),
    'options list' => 'entity_metadata_language_list',
    'schema field' => 'language',
    'setter permission' => 'administer fieldable panels panes',
  ) + $default_properties;
  $properties['link'] = array(
    'label' => t('Link?'),
    'description' => t('Whether or not this entity title will link to another page.'),
    'type' => 'boolean',
  ) + $default_properties;
  $properties['path'] = array(
    'label' => t('Path'),
    'description' => t('The path the title should link to, uses alias if one available.'),
    'getter callback' => 'fieldable_panels_panes_metadata_fpp_get_properties',
    'type' => 'uri',
  ) + $default_properties;
  $properties['path_raw'] = array(
    'label' => t('Path raw'),
    'description' => t('The path the title should link to, excluding the alias.'),
    'schema field' => 'path',
    'type' => 'uri',
  ) + $default_properties;
  $properties['reusable'] = array(
    'label' => t('Reusable?'),
    'description' => t('Whether or not this entity will appear in the Add Content dialog.'),
    'type' => 'boolean',
  ) + $default_properties;
  $properties['view_access'] = array(
    'label' => t('View access'),
    'type' => 'text',
    'description' => t('The universally uniquely identification string.'),
    'schema field' => 'view_access',
  ) + $default_properties;
  $properties['edit_access'] = array(
    'label' => t('Edit access'),
    'type' => 'text',
    'description' => t('The universally uniquely identification string.'),
    'schema field' => 'edit_access',
  ) + $default_properties;
  $properties['uuid'] = array(
    'label' => t('UUID'),
    'type' => 'text',
    'description' => t('The universally uniquely identification string.'),
    'schema field' => 'uuid',
  ) + $default_properties;
  $properties['vuuid'] = array(
    'label' => t('Revision UUID'),
    'type' => 'text',
    'description' => t('The universally uniquely identification string.'),
    'schema field' => 'uuid',
  ) + $default_properties;
  $properties['created'] = array(
    'label' => t('Date created'),
    'type' => 'date',
    'description' => t('The date the FPP was posted.'),
    'schema field' => 'created',
  ) + $default_properties;
  $properties['changed'] = array(
    'label' => t('Date changed'),
    'type' => 'date',
    'schema field' => 'changed',
    'description' => t('The date the FPP was most recently updated.'),
  ) + $default_properties;
  $properties['log'] = array(
    'label' => t('Revision log message'),
    'type' => 'text',
    'description' => t('In case a new revision is to be saved, the log entry explaining the changes for this version.'),
  ) + $default_properties;
  $properties['revision'] = array(
    'label' => t('Creates revision'),
    'type' => 'boolean',
    'description' => t('Whether saving this FPP creates a new revision.'),
  ) + $default_properties;

  return $info;
}
