<?php

/**
 * @file
 * Ctools integration.
 */

define('ELYSIA_CRON_CTOOLS_LINEBREAK', "\n");

/**
 * Get default cron jobs.
 *
 * @return array
 *   Cron jobs.
 */
function elysia_cron_get_ctools_defaults() {
  if (module_exists('ctools') && function_exists('ctools_include')) {
    ctools_include('export');
    if (function_exists('ctools_export_get_schema') && function_exists('_ctools_export_get_defaults') && ($schema = ctools_export_get_schema('elysia_cron'))) {
      $export = $schema['export'];
      return _ctools_export_get_defaults('elysia_cron', $export);
    }
  }

  return array();
}

/**
 * Ctools load callback.
 *
 * Ctools does not support override of PARTIAL record,
 * this is an elysia cron specific replacement to support it.
 *
 * @param string $name
 *   Cron job name.
 *
 * @return object|null
 *   Object to export or NULL if nothing found.
 */
function elysia_cron_ctools_export_load($name) {
  $schema = ctools_export_get_schema('elysia_cron');
  if (!empty($schema)) {
    $export = $schema['export'];

    $object = db_select('elysia_cron', 'ec')
      ->fields('ec', _elysia_cron_columns())
      ->condition('name', $name)
      ->execute()
      ->fetch();
    $default_objects = _ctools_export_get_defaults('elysia_cron', $export);

    if ($object) {
      if (isset($default_objects[$name])) {
        return _elysia_cron_ctools_export_load_object_db_and_code($object, $default_objects[$name], $export);
      }
      else {
        return _elysia_cron_ctools_export_load_object_db($object, $export);
      }
    }
    elseif (isset($default_objects[$name])) {
      return _elysia_cron_ctools_export_load_object_code($default_objects[$name], $export);
    }
  }
}

/**
 * Ctools load all callback.
 *
 * Ctools does not support override of PARTIAL record,
 * this is an elysia cron specific replacement to support it.
 *
 * @return array
 *   Array of object to export.
 */
function elysia_cron_ctools_export_load_all() {
  $result = array();
  $schema = ctools_export_get_schema('elysia_cron');

  if (empty($schema)) {
    return $result;
  }

  $export = $schema['export'];

  $objects = db_select('elysia_cron', 'ec')
    ->fields('ec', _elysia_cron_columns())
    ->execute()
    ->fetchAll();
  $default_objects = _ctools_export_get_defaults('elysia_cron', $export);

  foreach ($objects as $object) {
    $key = $object->{$export['key']};
    if (isset($default_objects[$key])) {
      $result[$key] = _elysia_cron_ctools_export_load_object_db_and_code($object, $default_objects[$key], $export);
      unset($default_objects[$key]);
    }
    else {
      $result[$key] = _elysia_cron_ctools_export_load_object_db($object, $export);
    }
  }

  foreach ($default_objects as $key => $object) {
    $result[$key] = _elysia_cron_ctools_export_load_object_code($object, $export);
  }

  return $result;
}

/**
 * @param object $object
 * @param object $code_object
 * @param array $export
 *
 * @return object
 */
function _elysia_cron_ctools_export_load_object_db_and_code($object, $code_object, array $export) {
  $overridden = FALSE;

  foreach ($code_object as $keyd => $value) {
    if (!isset($object->$keyd) || is_null($object->$keyd)) {
      $object->$keyd = $value;
    }
    else {
      if ($object->$keyd !== $value) {
        $overridden = TRUE;
      }
    }
  }

  $object->table = 'elysia_cron';
  $object->export_type = EXPORT_IN_DATABASE | EXPORT_IN_CODE;
  if (!empty($export['export type string'])) {
    $object->{$export['export type string']} = $overridden ? t('Overridden') : t('Normal');
  }

  return $object;
}

/**
 * @param object $object
 * @param array $export
 *
 * @return object
 */
function _elysia_cron_ctools_export_load_object_db($object, $export) {
  $object->table = 'elysia_cron';
  $object->export_type = EXPORT_IN_DATABASE;
  if (!empty($export['export type string'])) {
    $object->{$export['export type string']} = t('Normal');
  }
  return $object;
}

/**
 * @param object $object
 * @param array $export
 *
 * @return object
 */
function _elysia_cron_ctools_export_load_object_code($object, $export) {
  $object->table = 'elysia_cron';
  $object->export_type = EXPORT_IN_CODE;
  if (!empty($export['export type string'])) {
    $object->{$export['export type string']} = t('Default');
  }
  $object->in_code_only = TRUE;
  return $object;
}

/**
 * Ctools export object factory.
 *
 * Original ctools export factory (_ctools_export_unpack_object)
 * does not handle NULL values correctly.
 * This function does not support $schema['join'].
 *
 * @param array $schema
 * @param object $data
 *
 * @return object
 */
function elysia_cron_ctools_export_object_factory(array $schema, $data) {
  $object = new stdClass();

  foreach ($schema['fields'] as $field => $info) {
    $object->$field = isset($data->$field) && !is_null($data->$field) ? (empty($info['serialize']) ? $data->$field : unserialize($data->$field)) : NULL;
  }

  return $object;
}

/**
 * Ctools export callback.
 *
 * Handles NULL value (it's not possible to do this with "field"
 * export callback, because null values are rewritten before its call).
 *
 * @param object $object
 * @param string $indent
 *
 * @return string
 */
function elysia_cron_ctools_export_callback($object, $indent) {
  $table = 'elysia_cron';
  $schema = ctools_export_get_schema($table);
  $identifier = $schema['export']['identifier'];

  $output = $indent . '$' . $identifier . ' = new ' . get_class($object) . '();' . ELYSIA_CRON_CTOOLS_LINEBREAK;

  if ($schema['export']['can disable']) {
    $output .= $indent . '$' . $identifier . '->disabled = FALSE; /* Edit this to true to make a default ' . $identifier . ' disabled initially */' . ELYSIA_CRON_CTOOLS_LINEBREAK;
  }
  if (!empty($schema['export']['api']['current_version'])) {
    $output .= $indent . '$' . $identifier . '->api_version = ' . $schema['export']['api']['current_version'] . ';' . ELYSIA_CRON_CTOOLS_LINEBREAK;
  }

  $fields = $schema['fields'];

  foreach ($fields as $field => $info) {
    if (!empty($info['no export'])) {
      continue;
    }
    $value = isset($object->$field) ? $object->$field : (isset($info['default']) ? $info['default'] : NULL);
    if (!is_null($value) && $info['type'] == 'int') {
      $value = (isset($info['size']) && $info['size'] == 'tiny') ? (bool) $value : (int) $value;
    }
    $output .= $indent . '$' . $identifier . '->' . $field . ' = ' . ctools_var_export($value, $indent) . ';' . ELYSIA_CRON_CTOOLS_LINEBREAK;
  }

  return $output;
}

/**
 * Ctools export to hook code callback.
 *
 * Original "to hook code" callback does not support the replacement
 * of "load/load all" callback (it simply ignores them,
 * even if defined and supported elsewhere)
 * This code is equal to the original ctools one,
 * but uses specific load callback.
 *
 * @param array $names
 * @param string $name
 *
 * @return string
 */
function elysia_cron_ctools_to_hook_code(array $names, $name) {
  $output = '';

  $table = 'elysia_cron';
  $schema = ctools_export_get_schema($table);
  $export = $schema['export'];

  $objects = elysia_cron_ctools_export_load_all();
  $objects = array_intersect_key($objects, array_flip($names));

  if ($objects) {
    $output = '/**' . ELYSIA_CRON_CTOOLS_LINEBREAK;
    $output .= " * Implementation of hook_{$export['default hook']}()" . ELYSIA_CRON_CTOOLS_LINEBREAK;
    $output .= ' */' . ELYSIA_CRON_CTOOLS_LINEBREAK;
    $output .= "function " . $name . "_{$export['default hook']}() {" . ELYSIA_CRON_CTOOLS_LINEBREAK;
    $output .= "  \${$export['identifier']}s = array();" . ELYSIA_CRON_CTOOLS_LINEBREAK . ELYSIA_CRON_CTOOLS_LINEBREAK;

    foreach ($objects as $object) {
      $output .= ctools_export_crud_export($table, $object, '  ');
      $output .= "  \${$export['identifier']}s['" . check_plain($object->{$export['key']}) . "'] = \${$export['identifier']};" . ELYSIA_CRON_CTOOLS_LINEBREAK . ELYSIA_CRON_CTOOLS_LINEBREAK;
    }

    $output .= "  return \${$export['identifier']}s;" . ELYSIA_CRON_CTOOLS_LINEBREAK;
    $output .= '}' . ELYSIA_CRON_CTOOLS_LINEBREAK;
  }

  return $output;
}
