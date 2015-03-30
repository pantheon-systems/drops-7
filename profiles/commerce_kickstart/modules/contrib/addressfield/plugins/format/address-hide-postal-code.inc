<?php

/**
 * @file
 * Hide the postal code field.
 */

$plugin = array(
  'title' => t('Hide the postal code'),
  'format callback' => 'addressfield_format_address_hide_postal_code',
  'type' => 'address',
  'weight' => -85,
);

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_hide_postal_code(&$format, $address, $context = array()) {
  if (isset($format['locality_block']['postal_code'])) {
    $format['locality_block']['postal_code']['#access'] = FALSE;
  }
}
