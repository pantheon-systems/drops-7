<?php

/*
 * @file
 * Devel Generate for addressfield module
 *
 */

/*
 * Implements hook_devel_generate();
 */
function addressfield_devel_generate($object, $field, $instance, $bundle) {
  if (field_behaviors_widget('multiple values', $instance) == FIELD_BEHAVIOR_CUSTOM) {
    return devel_generate_multiple('_addressfield_devel_generate', $object, $field, $instance, $bundle);
  }
  else {
    return _addressfield_devel_generate($object, $field, $instance, $bundle);
  }
}

/*
 * Helper function to generate a greeky US-based address
 */
function _addressfield_devel_generate($object, $field, $instance, $bundle) {
  // Set default value
  $object_field = addressfield_default_values();
  $addresses = _addressfield_sample_addresses();
  $object_field = array_merge($object_field, $addresses[array_rand($addresses)]);
  unset($object_field['data']);
  return $object_field;
}

/**
 * Returns an array of sample addresses.
 *
 * @return
 *   An array of addressfield arrays
 */
function _addressfield_sample_addresses() {
  $fields = &drupal_static(__FUNCTION__);
  if (!isset($fields)) {
    $filepath = DRUPAL_ROOT . '/' . drupal_get_path('module', 'addressfield');
    $fields = array();
    if ($handle = @fopen("$filepath/addresses.txt",'r')) {
      if (is_resource($handle)) {
        $addresses = array();
        while (($buffer = fgets($handle)) !== false) {
          list($country, $administrative_area, $sub_administrative_area, $locality, $dependent_locality, $postal_code, $thoroughfare, $premise, $sub_premise) = explode("\t", $buffer);
          $fields[] = array(
            'country' => ($country == 'NULL') ? NULL : trim($country),
            'administrative_area' => ($administrative_area == 'NULL') ? NULL : trim($administrative_area),
            'sub_administrative_area' => ($sub_administrative_area == 'NULL') ? NULL : trim($sub_administrative_area),
            'locality' => ($locality == 'NULL') ? NULL : trim($locality),
            'dependent_locality' => ($dependent_locality == 'NULL') ? NULL : trim($dependent_locality),
            'postal_code' => ($postal_code == 'NULL') ? NULL : trim($postal_code),
            'thoroughfare' => ($thoroughfare == 'NULL') ? NULL : trim($thoroughfare),
            'premise' => ($premise == 'NULL') ? NULL : trim($premise),
            'sub_premise' => ($sub_premise == 'NULL') ? NULL : trim($sub_premise),
          );
        }
      }
      fclose($handle);
    }
  }
  return $fields;
}
