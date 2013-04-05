<?php

/**
 * @file
 * Hide then country when only one country is available.
 */

$plugin = array(
  'title' => t('Hide the country when only one is available'),
  'format callback' => 'addressfield_format_address_hide_country',
  'type' => 'address',
  'weight' => -80,
);

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_hide_country(&$format, $address, $context = array()) {
  $format['country']['#access'] = (count($format['country']['#options']) > 1);
}
