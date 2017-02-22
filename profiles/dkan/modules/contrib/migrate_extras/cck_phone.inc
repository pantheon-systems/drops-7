<?php

/**
 * Primary value passed to this field must be the two letter ISO country code of
 * the phone number.
 *
 * Subfields are used to specify all the other values:
 *   'number' - The actual phone number.
 *   'extension' - The extension.
 *
 * @code
 *   // The country should be passed in as the primary value.
 *   $this->addFieldMapping('field_user_phone', 'profile_country');
 *   $this->addFieldMapping('field_user_phone:number', 'profile_number');
 *   $this->addFieldMapping('field_user_phone:extension', 'profile_extension');
 * @endcode
 */
class MigrateCckPhoneHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('phone_number'));
  }

  public function fields($type) {
    return array(
      'number' => t('Subfield: The phone number itself'),
      'extension' => t('Extension: An optional extension'),
    );
  }

  public function prepare($entity, array $field_info, array $instance, array $values) {
    $arguments = array();
    if (isset($values['arguments'])) {
      $arguments = array_filter($values['arguments']);
      unset($values['arguments']);
    }
    $language = $this->getFieldLanguage($entity, $field_info, $arguments);

    // Setup the standard Field API array for saving.
    $delta = 0;
    foreach ($values as $value) {
      $return[$language][$delta] = array('country_codes' => drupal_strtolower($value));
      if (isset($arguments['number'])) {
        if (is_array($arguments['number'])) {
          $return[$language][$delta]['number'] = $arguments['number'][$delta];
        }
        else {
          $return[$language][$delta]['number'] = $arguments['number'];
        }
      }
      if (isset($arguments['extension'])) {
        if (is_array($arguments['extension'])) {
          $return[$language][$delta]['extension'] = $arguments['extension'][$delta];
        }
        else {
          $return[$language][$delta]['extension'] = $arguments['extension'];
        }
      }
      $delta++;
    }

    return isset($return) ? $return : NULL;
  }
}
