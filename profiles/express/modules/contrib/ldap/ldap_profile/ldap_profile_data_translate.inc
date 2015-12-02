<?php

/**
 * @file
 * translate file for ldap_profile module
 */

/**
 * Modify this file to indicate any types of custom data translations that you need.
 */
function ldap_profile_custom_translate($field, $value) {
  $result = '';
  switch ($field) {
    /**
     * the following is an example of how this is used
     * if your ldap account has an ldap field of telephone_number with format '(xxx) xxx-xxx'
     * and your drupal account has field_phone with format 'xxx-xxx-xxxx'
     *
     * case 'telephone_number': $result = strtr($value, array('(' => '', ')' => '', ' ' => '-'));
     */
    default: $result = $value;
  }
  return $result;
}
