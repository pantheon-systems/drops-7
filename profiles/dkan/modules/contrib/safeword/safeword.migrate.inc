<?php

/**
 * @file
 * Support for migrate module.
 *
 * With Migrate 2.4 or later, you can use the subfield syntax to set the
 * machine name:
 *
 * @code
 * $this->addFieldMapping('field_my_field', 'human_text');
 * $this->addFieldMapping('field_my_field:machine', 'machine_text');
 * @endcode
 *
 * With earlier versions of Migrate, you must pass an arguments array:
 *
 * @code
 * $arguments = array(
 *   'machine' => array('source_field' => 'machine_text'),
 * );
 * $this->addFieldMapping('field_my_field', 'human_text')
 *      ->arguments($arguments);
 * @endcode
 *
 * For multiple value fields, both human_text and machine_text need to be
 * arrays.
 */

/**
 * Implements hook_migrate_api().
 */
function safeword_migrate_api() {
  $api = array(
    'api' => 2,
    'field handlers' => array('MigrateSafewordFieldHandler'),
  );
  return $api;
}

/**
 * Custom extended MigrateFieldHandler class for the Safeword module.
 */
class MigrateSafewordFieldHandler extends MigrateFieldHandler {

  public function __construct() {
    $this->registerTypes(array('safeword_title'));
  }

  public function prepare($entity, array $field_info, array $instance, array $values) {
    // Change the format of the arguments for getFieldLanguage().
    $arguments = array();
    if (isset($values['arguments'])) {
      $arguments = array_filter($values['arguments']);
      unset($values['arguments']);
    }
    $language = $this->getFieldLanguage($entity, $field_info, $arguments);

    // Setup the standard Field API array for saving.
    foreach (array_values($values) as $delta => $value) {
      $item = array();
      if (isset($arguments['machine'])) {
        $item['machine'] = $arguments['machine'];
      }
      $item['human'] = $value;
      $return[$language][$delta] = $item;
    }

    return isset($return) ? $return : NULL;
  }
}
