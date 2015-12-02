<?php


/**
 * @file
 *   Validating functions of ThemeKey Example.
 */


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "=", "!", "<", "<=", ">", ">="
 * Allowed values: string of digits (numbers)
 *
 *
 * @param $rule
 *   A Theme Switching Rule as associative array:
 *   - property: ThemeKey property as string (e.g., "drupal:path")
 *   - wildcard: optional string, only used if property is "drupal:path:wildcard"
 *   - operator: ThemeKey operator as string ("=", "!", "<". "<=", ">", ">=", "~")
 *   - value: ThemeKey property value as string
 *
 * @return
 *   An associative array of errors:
 *   - property: translated error message as string
 *     describing a problem with the property
 *   - wildcard: translated error message as string
 *     describing a problem with the wildcard
 *   - operator: translated error message as string
 *     describing a problem with the operator
 *   - value: translated error message as string
 *     describing a problem with the value
 *   If no errors detected the array is empty.
 */
function themekey_example_validator_number_one($rule) {
  $errors = array();

  switch ($rule['operator']) {
    case '~':
      $errors['operator'] = t('Possible operators are "=", "!", "<", "<=", ">" and ">="');
      break;

    case '=':
      if ('1' !== $rule['value']) {
        $errors['value'] = t('No other value than "1" makes sense for this operator');
      }
      break;

    default:
      if (!ctype_digit($rule['value'])) {
        $errors['value'] = t('Value must be a number');
      }
      break;
  }

  return $errors;
}
