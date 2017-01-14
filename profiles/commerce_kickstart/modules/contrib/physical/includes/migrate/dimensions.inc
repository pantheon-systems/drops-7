<?php

/**
 * Class MigratePhysicalDimensionsFieldHandler
 *
 * Primary value is not used and subfields are used for dimensions.
 * Example:
 *   $this->addFieldMapping('field_dimensions')
 *     ->defaultValue(1);
 *   $this->addFieldMapping('field_dimensions:length', 'source_length');
 *   $this->addFieldMapping('field_dimensions:width', 'source_width');
 *   $this->addFieldMapping('field_dimensions:height', 'source_height');
 *   $this->addFieldMapping('field_dimensions:unit')
 *     ->defaultValue('mm');
 */
class MigratePhysicalDimensionsFieldHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('physical_dimensions'));
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
   *  The field type.
   * @param $instance
   *  Instance info for the field.
   * @param Migration $migration
   *  The migration context for the parent field. We can look at the mappings
   *  and determine which subfields are relevant.
   * @return array
   */
  public function fields($type, $instance, $migration = NULL) {
    return array(
      'length' => t('Subfield: !field', array('!field' => t('Length'))),
      'width' => t('Subfield: !field', array('!field' => t('Width'))),
      'height' => t('Subfield: !field', array('!field' => t('Height'))),
      'unit' => t('Subfield: The unit of measure for the dimensions'),
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

    if(empty($values)) {
      $values = array(0 => '');
    }

    $language = $this->getFieldLanguage($entity, $field_info, $arguments);

    foreach ($values as $delta => $value) {
      $item = array();
      foreach (array_keys(self::fields('', '')) as $subfield) {
        if (isset($arguments[$subfield])) {
          if (!is_array($arguments[$subfield])) {
            $item[$subfield] = $arguments[$subfield];
          }
          else if (isset($arguments[$subfield][$delta])) {
            $item[$subfield] = $arguments[$subfield][$delta];
          }
        }
      }
      $return[$language][$delta] = $item;
    }

    return isset($return) ? $return : NULL;
  }
}

