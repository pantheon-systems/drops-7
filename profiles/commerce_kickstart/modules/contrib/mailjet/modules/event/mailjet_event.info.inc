<?php

/**
 * @file
 * Provides metadata for the mailjet event entity.
 */

/**
 * Implements hook_entity_property_info().
 */
function mailjet_event_entity_property_info() {
  $info = array();

  // Add meta-data about the basic commerce_product properties.
  $properties = &$info['mailjet_event']['properties'];

  $properties['id'] = array(
    'label' => t('Mailjet event ID'),
    'description' => t('The internal numeric ID of the mailjet event entity.'),
    'type' => 'integer',
    'schema field' => 'id',
  );

  $properties['uid'] = array(
    'label' => t('User ID'),
    'type' => 'user',
    'description' => t('The unique ID of the mailjet list entity creator.'),
    'schema field' => 'uid',
  );

  $properties['error'] = array(
    'label' => t('Error'),
    'type' => 'text',
    'options list' => 'mailjet_event_error_options',
    'getter callback' => 'mailjet_event_get_properties',
    'computed' => TRUE,
  );

  $properties['error_message'] = array(
    'label' => t('Error Message'),
    'type' => 'text',
    'getter callback' => 'mailjet_event_get_properties',
    'computed' => TRUE,
  );

  return $info;
}