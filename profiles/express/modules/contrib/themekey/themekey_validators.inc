<?php

/**
 * @file
 * Provides set of validators which can be used to validate
 * ThemeKey Theme Switching Rules.
 * @see themekey_admin.inc
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author Carsten Müller | Cocomore AG
 *   @see http://drupal.org/user/124707
 */


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "=", "!"
 * Allowed values: "true", "false" as string
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
function themekey_validator_string_boolean($rule) {
  $errors = array();

  switch ($rule['operator']) {
    case '=':
    case '!':
      if ('true' !== $rule['value'] && 'false' !== $rule['value']) {
        $errors['value'] = t('Possible values are "true" and "false"');
      }
      break;

    default:
      $errors['operator'] = t('Possible operators are "=" and "!"');
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "=", "!"
 * Allowed values: "1", "0"
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
function themekey_validator_nummeric_boolean($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      if (!ctype_digit($rule['value']) || 1 < $rule['value']) {
        $errors['value'] = t('Possible values are "0" and "1"');
      }
      break;

    default:
      $errors['operator'] = t('Possible operators are "=" and "!"');
  }

  return $errors;
}


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
function themekey_validator_ctype_digit($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      $errors['operator'] = t('Possible operators are "=", "!", "<", "<=", ">" and ">="');
      break;
  }

  if (!ctype_digit($rule['value'])) {
    $errors['value'] = t('Value must be a number');
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed values:
 * - configured Drupal language as string or 'und' if operator is "="
 * - valid regular expression if operator is "~"
 * - any string for different operators
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
function themekey_validator_language($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
      $languages = language_list();
      // add 'neutral' to the list of languages
      $languages['neutral'] = TRUE;
      if (!array_key_exists($rule['value'], $languages)) {
        $errors['value'] = t('Possible values are %languages', array('%languages' => '"' . implode('", "', array_keys($languages)) . '"'));
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "<", "<=", ">", ">=" and "~"
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - string formatted like "2009-12-24 23:56:17" for different operators
 *
 *
 * @param $rule
 *   A Theme Switching Rule as associative array:
 *   - property: ThemeKey property as string (e.g. "drupal:path")
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
function themekey_validator_date_time($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      // It seems senseless to switch a theme for one second
      $errors['operator'] = t('Possible operators are "<", "<=", ">", ">=" and "~"');
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9]{4}[0-9\- :]*$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable for checks against dates formatted like \"2009-12-24 23:56:17\"");
      }
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - a valid date like "2009-12-24" if operator is "=" or "!"
 * - fragment of a date which contains at least the year as four digits
 *   for different operators
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
function themekey_validator_date($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      if (!preg_match("/^[0-9]{4}-[0-1]{1}[0-9]{1}-[0-3]{1}[0-9]{1}$/", $rule['value'])) {
        $errors['value'] = t('Not a valid date string. Format should look like "2009-12-24"');
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9]{4}[0-9\-]*$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable for checks against dates formatted like \"2009-12-24\"");
      }
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - a valid time like "23:16:56" if operator is "=" or "!"
 * - fragment of a time which contains at least the hour as two digits
 *   for different operators
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
function themekey_validator_time($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      if (!preg_match("/^[0-2][0-9]:[0-5][0-9]:[0-5][0-9]$/", $rule['value'])) {
        $errors['value'] = t('Not a valid date string. Format should look like "23:16:56"');
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-2][0-9][0-9:]*$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable for checks against dates formatted like \"23:16:56\"");
      }
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - an existing content type if operator is "=" or "!"
 * - must contain only lowercase letters, numbers, and underscores
 *   for different operators
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
function themekey_validator_node_type($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      $node_types = node_type_get_types();
      if (!array_key_exists($rule['value'], $node_types)) {
        $errors['value'] = t('Possible values are %node_types', array('%node_types' => '"' . implode('", "', array_keys($node_types)) . '"'));
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9a-z_]+$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable. It can only contain lowercase letters, numbers, and underscores");
      }
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - must contain only lowercase letters, numbers, and hyphens
 *   for different operators
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
function themekey_validator_http_host($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9a-z\-.]+$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable. It can only contain lowercase letters, numbers, and hyphens");
      }
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: all
 * Allowed values: any string without whitespace
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
function themekey_validator_no_whitespace($rule) {
  $errors = array();

  if (preg_match("/\s/", $rule['value'])) {
    $errors['value'] = t('Value must not contain whitespace characters');
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed wildcards: any string without whitespace and not starting with "#" or "%"
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
function themekey_validator_wildcard($rule) {
  $errors = themekey_validator_no_whitespace($rule);

  if (preg_match("/^[#%]/", $rule['wildcard'])) {
    $errors['wildcard'] = t('Wildcard must not start with type identifier "#" or "%" at this point');
  }
  elseif (preg_match("/\s/", $rule['wildcard'])) {
    $errors['wildcard'] = t('Wildcard must not contain whitespace characters');
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "~"
 * Allowed values: valid regular expression
 *
 * @see http://php.net/manual/en/pcre.pattern.php
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
function themekey_validator_regex($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      if (FALSE === @preg_match($rule['value'], 'dummy')) {
        $errors['value'] = t('Regular expression seems to be malformed. See !link for details', array('!link' => l(t('PHP Manual'), 'http://php.net/manual/en/pcre.pattern.php')));
      }
      break;

    default:
      $errors['operator'] = t('The only possible operator is "~"');
      break;
  }

  return $errors;
}
/**
 * @file
 * Provides set of validators which could be used to validate
 * ThemeKey Theme Switching Rules.
 * @see themekey_admin.inc
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author Carsten Müller | Cocomore AG
 *   @see http://drupal.org/user/124707
 */


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "=", "!"
 * Allowed values: paths without whitespace
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
function themekey_validator_drupal_path($rule) {
  $errors = themekey_validator_no_whitespace($rule);

  switch ($rule['operator']) {
    case '=':
    case '!':
      if (strpos($rule['value'], '/') === 0) {
        $errors['value'] = t('A Drupal path must not start with "/"');
      }
      if (preg_match("@[^/][%#]@", $rule['value'])) {
        $errors['value'] = t('A wildcard can only be used to replace part(s) of the path, but not for parts of a word');
      }
      if (strpos($rule['value'], '?') !== FALSE) {
        $errors['value'] = t('Query strings will be stripped before a Drupal path is processed. Maybe you want to chain drupal:path and system:query_string or system:query_param (both provided by additional module ThemeKey Properties).');
      }
      break;

    default:
      $errors['operator'] = t('Possible operators are "=" and "!"');
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: any
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - a valid IPv4 address like "123.123.123.123" if operator is "=" or "!"
 * - fragment of an IPv4 address which contains at least one digit
 *   for different operators
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
function themekey_validator_ip_address($rule) {
  $errors = array();

  switch ($rule['operator']) {
    case '=':
    case '!':
      // TODO add for support IPv6
      // TODO improve regex for IPv4
      if (!preg_match("/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/", $rule['value'])) {
        $errors['value'] = t('Not a valid IPv4 address. Format should look like "123.123.123.123"');
      }
      break;

    case '~':
      // CHECK THE REGULAR EXPRESSION
      $errors = themekey_validator_regex($rule);
      break;

    default:
      // TODO add for support IPv6
      // TODO improve regex for IPv4
      if (!preg_match("/^[1-2][0-9\.]*$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable for checks against dates formatted like \"123.123.123.123\"");
      }
      break;
  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "=", "!", "~"
 * Allowed values:
 * - valid regular expression if operator is "~"
 * - a valid Drupal role if operator is "=" or "!"
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
function themekey_validator_role($rule) {
  $errors = array();

  switch ($rule['operator']) {
    case '=':
    case '!':
      $roles = array();
      $result = db_select('role', 'r')
      ->fields('r')
      ->orderBy('name', 'ASC')
      ->execute();
      foreach ($result as $role) {
        $roles[$role->rid] = $role->name;
      }

      if (!in_array($rule['value'], $roles)) {
        $errors['value'] = t('The entered user role %value is not valid. Possible roles are "%roles".', array('%value' => $rule['value'], '%roles' => implode('", "', $roles)));
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      $errors['operator'] = t('Possible operators are "=", "!", "~"');
      break;

  }

  return $errors;
}


/**
 * Validates a Theme Switching Rule.
 * Allowed Operators: "=", "!"
 * Allowed values:
 * - a three letter day String if operator is "=" or "!"
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
function themekey_validator_day_of_week($rule) {
  static $days_of_week = array('Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun');

  $errors = array();

  switch ($rule['operator']) {
    case '=':
    case '!':
      if (!in_array($rule['value'], $days_of_week)) {
        $errors['value'] = t('The day %value is not valid. Possible days are "%days".', array('%value' => $rule['value'], '%days' => implode('", "', $days_of_week)));
      }
      break;

    default:
      $errors['operator'] = t('Possible operators are "=" and "!"');
      break;

  }

  return $errors;
}


