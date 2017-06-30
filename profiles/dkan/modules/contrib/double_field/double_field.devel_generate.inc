<?php

/**
 * @file
 * Functions needed for devel_generate integration.
 */

/**
 * Implements hook_devel_generate().
 */
function double_field_devel_generate($object, $field, $instance, $bundle) {

  list($widget['first'], $widget['second']) = explode('_&_', $instance['widget']['type']);

  foreach (array('first', 'second') as $subfield) {

    $subwidget_settings = $instance['widget']['settings'][$subfield];

    switch ($widget[$subfield]) {
      case 'checkbox':
        $object_field[$subfield] = $subwidget_settings['checkbox'][mt_rand(0, 1) ? 'on_value' : 'off_value'];
        break;

      case 'select':
        if ($subwidget_settings['select']['allowed_values']) {
          $object_field[$subfield] = array_rand($subwidget_settings['select']['allowed_values']);
        }
        break;

      default:
        // Other field types are not supported yet.
        switch ($field['settings'][$subfield]['type']) {
          case 'varchar':
            $maxlength = $field['settings'][$subfield]['maxlength'];
            if ($maxlength > 64) {
              // Average length of a word in devel_create_greeking()
              // is about 7 characters.
              $word_count = (int) ($maxlength / 7);
              $object_field[$subfield] = rtrim(substr(devel_create_greeking($word_count), 0, $maxlength - 1), ' .') . '.';
            }
            else {
              $object_field[$subfield] = user_password($field['settings'][$subfield]['maxlength']);
            }
            break;

          case 'text':
            $object_field[$subfield] = devel_create_greeking(mt_rand(1, 100));
            break;

          case 'int':
            $range = array(
              'tiny' => 1,
              'small' => 2,
              'medium' => 3,
              'normal' => 4,
              // Actually big is equal to 8.
              'big' => 4,
            );
            $limit = pow(2, 8 * $range[$field['settings'][$subfield]['size']] - 1);
            $object_field[$subfield] = mt_rand(-$limit, $limit - 1);
            break;

          case 'float':
            $limit = pow(2, 8 * 4 - 1);
            $object_field[$subfield] = mt_rand(-$limit, $limit - 1) / mt_rand(1, 10000);
            break;

          case 'decimal':
            require_once 'DrupalDebug.php';
            $limit = pow(10, $field['settings'][$subfield]['precision'] - $field['settings'][$subfield]['scale']);
            $object_field[$subfield] = round(mt_rand(-$limit, $limit - 1) / mt_rand(1, 10000), $field['settings'][$subfield]['scale']);
            break;

        }

    }

  }

  return $object_field;
}
