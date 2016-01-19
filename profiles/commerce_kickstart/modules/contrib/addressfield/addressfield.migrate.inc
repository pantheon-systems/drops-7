<?php

/**
 * @file
 * Base integration with the Migrate API class.
 */

/**
 * Implements hook_migrate_api().
 */
function addressfield_migrate_api() {
  $api = array(
    'api' => 2,
    'field handlers' => array('MigrateAddressFieldHandler'),
  );
  return $api;
}

/**
 * Primary value passed to this field must be the two letter ISO country code of
 * the address.
 *
 * Arguments are used to specify all the other values:
 *   'administrative_area' - The administrative area of this address. (i.e. State/Province)
 *   'sub_administrative_area' - The sub administrative area of this address.
 *   'locality' - The locality of this address. (i.e. City)
 *   'dependent_locality' - The dependent locality of this address.
 *   'postal_code' - The postal code of this address.
 *   'thoroughfare' - The thoroughfare of this address. (i.e. Street address)
 *   'premise' - The premise of this address. (i.e. Apartment / Suite number)
 *   'sub_premise' - The sub_premise of this address.
 *   'organisation_name' - Contents of a primary OrganisationName element in the xNL XML.
 *   'name_line' - Contents of a primary NameLine element in the xNL XML.
 *   'first_name' - Contents of the FirstName element of a primary PersonName element in the xNL XML.
 *   'last_name' - Contents of the LastName element of a primary PersonName element in the xNL XML.
 *   'data' - Additional data for this address.
 *
 * Add the source field mappings to the argument array then add null mappings to
 * avoid having fields flagged as as unmapped:
 * @code
 *   // The country should be passed in as the primary value.
 *   $this->addFieldMapping('field_address', 'profile_country');
 *   $this->addFieldMapping('field_address:thoroughfare', 'profile_address');
 *   $this->addFieldMapping('field_address:locality', 'profile_city');
 *   $this->addFieldMapping('field_address:administrative_area', 'profile_state');
 * @endcode
 */
class MigrateAddressFieldHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('addressfield'));
  }

  /**
   * Provide subfields for the addressfield columns.
   */
  public function fields() {
    // Declare our arguments to also be available as subfields.
    $fields = array(
      'administrative_area' => t('<a href="@doc">The administrative area of ' .
          'this address (i.e. State/Province)</a>',
        array('@doc' => 'http://drupal.org/node/1996546#administrative_area')),
      'sub_administrative_area' => t('<a href="@doc">The sub administrative ' .
          'area of this address</a>',
        array('@doc' => 'http://drupal.org/node/1996546#sub_administrative_area')),
      'locality' => t('<a href="@doc">The locality of this address (i.e. ' .
          'City)</a>',
        array('@doc' => 'http://drupal.org/node/1996546#locality')),
      'dependent_locality' => t('<a href="@doc">The dependent locality of ' .
          'this address</a>',
        array('@doc' => 'http://drupal.org/node/1996546#dependent_locality')),
      'postal_code' => t('<a href="@doc">The postal code of this address</a>',
        array('@doc' => 'http://drupal.org/node/1996546#postal_code')),
      'thoroughfare' => t('<a href="@doc">The thoroughfare of this address ' .
          '(i.e. Street address)</a>',
        array('@doc' => 'http://drupal.org/node/1996546#thoroughfare')),
      'premise' => t('<a href="@doc">The premise of this address (i.e. Apartment / Suite number)</a>',
        array('@doc' => 'http://drupal.org/node/1996546#premise')),
      'sub_premise' => t('<a href="@doc">The sub_premise of this address</a>',
        array('@doc' => 'http://drupal.org/node/1996546#sub_premise')),
      'organisation_name' => t('<a href="@doc">Contents of a primary ' .
          'OrganisationName element in the xNL XML</a>',
        array('@doc' => 'http://drupal.org/node/1996546#organisation_name')),
      'name_line' => t('<a href="@doc">Contents of a primary NameLine element ' .
          'in the xNL XML</a>',
        array('@doc' => 'http://drupal.org/node/1996546#name_line')),
      'first_name' => t('<a href="@doc">Contents of the FirstName element of ' .
          'a primary PersonName element in the xNL XML</a>',
        array('@doc' => 'http://drupal.org/node/1996546#first_name')),
      'last_name' => t('<a href="@doc">Contents of the LastName element of a ' .
          'primary PersonName element in the xNL XML</a>',
        array('@doc' => 'http://drupal.org/node/1996546#last_name')),
      'data' => t('<a href="@doc">Additional data for this address</a>',
        array('@doc' => 'http://drupal.org/node/1996546#data')),
    );
    return $fields;
  }

  /**
   * Implements MigrateFieldHandler::prepare().
   *
   * @param $entity
   * @param array $field_info
   * @param array $instance
   * @param array $values
   *
   * @return null
   */
  public function prepare($entity, array $field_info, array $instance,
                          array $values) {
    $arguments = array();
    if (isset($values['arguments'])) {
      $arguments = array_filter($values['arguments']);
      unset($values['arguments']);
    }
    $language = $this->getFieldLanguage($entity, $field_info, $arguments);

    // Setup the standard Field API array for saving.
    $delta = 0;
    foreach ($values as $value) {
      $return[$language][$delta] = array('country' => $value)
        + $this->prepareArguments($arguments, $field_info, $delta);
      $delta++;
    }

    return isset($return) ? $return : NULL;
  }

  /**
   * Builds an array with additional data for the current $delta.
   *
   * @param  array $arguments
   * @param  array $field_info
   * @param  $delta
   *
   * @return array
   */
  protected function prepareArguments(array $arguments, array $field_info, $delta) {
    $result = array();
    $data = array();

    foreach ($arguments as $column_key => $column_value) {
      $value = NULL;

      if (is_array($arguments[$column_key])) {
        if (!empty($arguments[$column_key][$delta])) {
          $value = $arguments[$column_key][$delta];
        }
      }
      else {
        $value = $arguments[$column_key];
      }

      if ($value) {
        if (isset($field_info['columns'][$column_key])) {
          // Store the data in a separate column.
          $result[$column_key] = $value;
        }
        else {
          // Add the data to the 'data' column.
          $data[$column_key] = $value;
        }
      }
    }

    // Store all the other data as a serialized array in the data field.
    if (!empty($data)) {
      $result['data'] = serialize($data);
    }

    return $result;
  }
}
