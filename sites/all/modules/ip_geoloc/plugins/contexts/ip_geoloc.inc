<?php

/**
 * @file
 * ip_geoloc/plugins/contexts/ip_geoloc.inc
 * CTools context plugin.
 */

$plugin = array(
  'title' => t('IP geolocation'),
  'description' => t('The geolocation information for the current user'),
  'context' => 'ip_geoloc_ip_geoloc_context',
  'keyword' => 'ip_geoloc',
  'context name' => 'ip_geoloc',
  'convert list' => 'ip_geoloc_ip_geoloc_context_convert_list',
  'convert' => 'ip_geoloc_ip_geoloc_context_convert',
);

/**
 * Implements hook_context().
 */
function ip_geoloc_ip_geoloc_context($empty, $data = NULL, $conf = FALSE) {
  $context = new ctools_context('ip_geoloc');
  $context->plugin = 'ip_geoloc';
  if ($empty) {
    return $context;
  }
  $context->data = _ip_geoloc_get_session_value();
  return $context;
}

/**
 * Provide a list of sub-keywords.
 *
 * This is used to provide keywords from the context for use in a content type,
 * pane, etc.
 */
function ip_geoloc_ip_geoloc_context_convert_list() {
  return array(
    'provider' => t('Provider'),
    'ip_address' => t('IP Address'),
    'latitude' => t('Latitude'),
    'longitude' => t('Longitude'),
    'accuracy' => t('Accuracy'),
    'formatted_address' => t('Formatted Address'),
    'street_number' => t('Street Number'),
    'route' => t('Route'),
    'locality' => t('Locality'),
    'administrative_area_level_1' => t('Administrative Area Level 1'),
    'administrative_area_level_2' => t('Administrative Area Level 2'),
    'country' => t('Country'),
    'country_code' => t('Country Code'),
    'postal_code' => t('Postal Code'),
  );
}

/**
 * Convert a context into string to be used as keyword by content types, etc.
 */
function ip_geoloc_ip_geoloc_context_convert($context, $type) {
  if (isset($context->data['location'][$type])) {
    return $context->data['location'][$type];
  }
  return NULL;
}
