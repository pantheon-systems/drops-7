<?php

/**
 * Class MigratePhysicalWeightFieldHandler
 *
 * Example:
 *   $this->addFieldMapping('field_weight', 'source_weight);
 *   $this->addFieldMapping('field_weight:unit')
 *     ->defaultValue('g');
 */
class MigratePhysicalWeightFieldHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('physical_weight'));
  }

  static function arguments($unit = NULL) {
    $arguments = array();
    if (!is_null($unit)) {
      $arguments['unit'] = $unit;
    }
    return $arguments;
  }

  /**
   * Implementation of MigrateFieldHandler::fields().
   *
   * @param $type
   *   The field type.
   * @param $instance
   *   Instance info for the field.
   * @param Migration $migration
   *   The migration context for the parent field. We can look at the mappings
   *   and determine which subfields are relevant.
   * @return array
   */
  public function fields($type, $instance, $migration = NULL) {
    return array(
      'unit' => t('Subfield: The unit of measure for the weight'),
    );
  }

  public function prepare($entity, array $field_info, array $instance, array $values) {
    if (isset($values['arguments'])) {
      $arguments = $values['arguments'];
      unset($values['arguments']);
    }
    else {
      $arguments = array();
    }

    $language = $this->getFieldLanguage($entity, $field_info, $arguments);

    foreach ($values as $delta => $value) {
      $item = array();
      if (isset($arguments['unit'])) {
        if (!is_array($arguments['unit'])) {
          $item['unit'] = $arguments['unit'];
        }
        else if (isset($arguments['unit'][$delta])) {
          $item['unit'] = $arguments['unit'][$delta];
        }
      }
      $item['weight'] = $value;
      $return[$language][$delta] = $item;
    }

    return isset($return) ? $return : NULL;
  }
}

