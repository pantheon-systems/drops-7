<?php

/**
 * @file
 * Provides metadata for the mailjet list entity.
 */

/**
 * Implements hook_entity_property_info().
 */
function mailjet_list_entity_property_info() {
  $info = array();

  // Add meta-data about the basic commerce_product properties.
  $properties = &$info['mailjet_list']['properties'];

  $properties['id'] = array(
    'label' => t('Mailjet list ID'),
    'description' => t('The internal numeric ID of the mailjet list entity.'),
    'type' => 'integer',
    'schema field' => 'id',
  );

  $properties['label'] = array(
    'label' => t('Label'),
    'description' => t('Title of your list (max length 60 characters).'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer mailjet_list entities',
    'schema field' => 'label',
  );

  $properties['status'] = array(
    'label' => t('Status'),
    'description' => t('Boolean indicating whether the mailjet list entity is active or disabled.'),
    'type' => 'boolean',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer mailjet_list entities',
    'schema field' => 'status',
  );

  $properties['uid'] = array(
    'label' => t('Creator ID'),
    'type' => 'user',
    'description' => t('The unique ID of the mailjet list entity creator.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer mailjet_list entities',
    'schema field' => 'uid',
  );

  return $info;
}

