<?php

/**
 * @file
 * Token module integration.
 */

/**
 * Implements hook_token_info().
 */
function addressfield_token_info() {
  $type = array(
    'name' => t('Address field'),
    'description' => t('Tokens related to address field values and their components.'),
    'needs-data' => 'address-field',
    'field' => TRUE,
  );

  // Define tokens for the various components of addresses supported through the
  // user interface along with two helper tokens for country and administrative
  // area to distinguish between names and abbreviations.
  $info['country'] = array(
    'name' => t('Country name'),
    'description' => t('The full name of the country.'),
  );
  $info['country-code'] = array(
    'name' => t('Country code'),
    'description' => t('The two letter ISO country code.'),
  );
  $info['administrative-area'] = array(
    'name' => t('Administrative area (i.e. State/Province)'),
    'description' => t('The administrative area value, expanded to the full name if applicable.'),
  );
  $info['administrative-area-raw'] = array(
    'name' => t('Administrative area (raw value)'),
    'description' => t('The raw administrative area value.'),
  );
  $info['locality'] = array(
    'name' => t('Locality (i.e. City)'),
    'description' => t('The locality value.'),
  );
  $info['postal-code'] = array(
    'name' => t('Postal code'),
    'description' => t('The postal code value.'),
  );
  $info['thoroughfare'] = array(
    'name' => t('Thoroughfare (i.e. Street address)'),
    'description' => t('The thoroughfare value.'),
  );
  $info['premise'] = array(
    'name' => t('Premise (i.e. Street address)'),
    'description' => t('The premise value.'),
  );
  $info['sub_premise'] = array(
    'name' => t('Sub Premise (i.e. Suite, Apartment, Floor, Unknown.)'),
    'description' => t('The sub premise value.'),
  );
  $info['organisation'] = array(
    'name' => t('Organisation'),
    'description' => t('The organisation name value.'),
  );
  $info['name-line'] = array(
    'name' => t('Full name'),
    'description' => t('The name line value of the address.'),
  );
  $info['first-name'] = array(
    'name' => t('First name'),
    'description' => t('The first name value.'),
  );
  $info['last-name'] = array(
    'name' => t('Last name'),
    'description' => t('The last name value.'),
  );

  // Add a helper token to format addresses as expected by MailChimp.
  $info['format-mailchimp'] = array(
    'name' => t('Address formatted for MailChimp'),
    'description' => t('The full address formatted for import into MailChimp.'),
  );

  return array(
    'types' => array('address-field' => $type),
    'tokens' => array('address-field' => $info),
  );
}

/**
 * Implements hook_token_info_alter().
 */
function addressfield_token_info_alter(&$data) {
  // Loop over every address field on the site.
  foreach (array_filter(field_info_field_map(), 'addressfield_field_map_filter') as $field_name => $field) {
    foreach ($data['tokens'] as $group => $token){
      if (isset($data['tokens'][$group][$field_name]) && is_array($data['tokens'][$group][$field_name])) {
        // Set the token type for the field to use the addressfield child tokens.
        $data['tokens'][$group][$field_name]['type'] = 'address-field';
      }
    }
  }
}

/**
 * Implements hook_tokens().
 */
function addressfield_tokens($type, $tokens, array $data = array(), array $options = array()) {
  if (isset($options['language'])) {
    $language_code = $options['language']->language;
  }
  else {
    $language_code = LANGUAGE_NONE;
  }

  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  // If we're generating tokens for an address field, extract the address data
  // from the field value array and generate the necessary replacements.
  if ($type == 'address-field' && !empty($data['address-field'][$language_code]) && is_array($data['address-field'][$language_code])) {
    $address = reset($data['address-field'][$language_code]);

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'country':
          $countries = _addressfield_country_options_list();
          $replacements[$original] = $sanitize ? check_plain($countries[$address['country']]) : $countries[$address['country']];
          break;

        case 'country-code':
          $replacements[$original] = $sanitize ? check_plain($address['country']) : $address['country'];
          break;

        case 'administrative-area':
          // If we received format handlers in the data array, generate the form
          // for the address field to see if the administrative area should be
          // expanded from an abbreviation to a related name.
          $administrative_area = $address['administrative_area'];

          if (!empty($data['format_handlers'])) {
            $form = addressfield_generate($address, $data['format_handlers'], array('mode' => 'form'));

            if (!empty($form['locality_block']['administrative_area']['#options'][$administrative_area])) {
              $administrative_area = $form['locality_block']['administrative_area']['#options'][$administrative_area];
            }
          }

          $replacements[$original] = $sanitize ? check_plain($administrative_area) : $administrative_area;
          break;

        case 'administrative-area-raw':
          $replacements[$original] = $sanitize ? check_plain($address['administrative_area']) : $address['administrative_area'];
          break;

        case 'locality':
          $replacements[$original] = $sanitize ? check_plain($address['locality']) : $address['locality'];
          break;

        case 'postal-code':
          $replacements[$original] = $sanitize ? check_plain($address['postal_code']) : $address['postal_code'];
          break;

        case 'thoroughfare':
          $replacements[$original] = $sanitize ? check_plain($address['thoroughfare']) : $address['thoroughfare'];
          break;

        case 'premise':
          $replacements[$original] = $sanitize ? check_plain($address['premise']) : $address['premise'];
          break;

        case 'sub_premise':
          $replacements[$original] = $sanitize ? check_plain($address['sub_premise']) : $address['sub_premise'];
          break;

        case 'organisation':
          $replacements[$original] = $sanitize ? check_plain($address['organisation_name']) : $address['organisation_name'];
          break;

        case 'name-line':
          $replacements[$original] = $sanitize ? check_plain($address['name_line']) : $address['name_line'];
          break;

        case 'first-name':
          $replacements[$original] = $sanitize ? check_plain($address['first_name']) : $address['first_name'];
          break;

        case 'last-name':
          $replacements[$original] = $sanitize ? check_plain($address['last_name']) : $address['last_name'];
          break;

        // See: http://kb.mailchimp.com/article/how-do-i-format-my-list-fields-to-import-them
        case 'format-mailchimp':
          $components = array();

          foreach (array('thoroughfare', 'premise', 'locality', 'administrative_area', 'postal_code', 'country') as $component) {
            if (!empty($address[$component])) {
              $components[] = $address[$component];
            }
          }

          $format_mailchimp = implode('  ', $components);
          $replacements[$original] = $sanitize ? check_plain($format_mailchimp) : $format_mailchimp;
          break;
      }
    }
  }

  // The Token module extends direct token generation by using a generic entity
  // token generation process. Since we intend to overwrite the default Token
  // module implementation of address field tokens, we use this generic token
  // generation process to find and replace address field tokens on relevant
  // entities. This ensures our tokens aren't overwritten by the Token module
  // and helps us avoid having to do the entity detection ourselves.
  if ($type == 'entity') {
    // Loop over the address fields defined on the site.
    foreach (array_filter(field_info_field_map(), 'addressfield_field_map_filter') as $field_name => $field) {
      // If there are any address field tokens in the token list...
      if ($addressfield_tokens = token_find_with_prefix($tokens, $field_name)) {
        // If the current field is on the matching entity type...
        if (!empty($field['bundles'][$data['entity_type']])) {
          // Extract the format handlers selected in a representative instance
          // settings form for use in formatting tokens.
          $instance = field_info_instance($data['entity_type'], $field_name, reset($field['bundles'][$data['entity_type']]));
          $format_handlers = $instance['widget']['settings']['format_handlers'];
        }

        // Generate the necessary address field tokens for the entity.
        $replacements += token_generate('address-field', $addressfield_tokens, array('address-field' => $data['entity']->$field_name, 'format_handlers' => $format_handlers), $options);
      }
    }
  }

  return $replacements;
}
