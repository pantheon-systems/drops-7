<?php

/**
 * Contains the predefined address formats.
 *
 * Derived from Google's dataset: https://i18napis.appspot.com/address.
 */

/**
 * Returns the address format for the given country code.
 *
 * @param $country_code
 *   The country code for which the address format should be returned.
 *
 * @return
 *   The address format array with the following keys:
 *   - used_fields: An array of fields used by this format. Possible values:
 *     'dependent_locality', 'locality', 'administrative_area', 'postal_code'.
 *   - required_fields: An array of required fields. See "used_fields".
 *   - dependent_locality_label: The label for the dependent locality field.
 *   - locality_label: The label for the locality field.
 *   - administrative_area_label: The label for the administrative area field.
 *   - postal_code_label: The label for the postal code field.
 *   - render_administrative_area_value: True if the value should be rendered
 *     instead of the ISO code. US example: California instead of CA.
 *     Only relevant for countries with predefined administrative areas.
 */
function addressfield_get_address_format($country_code) {
  $default_values = array(
    'used_fields' => array('locality'),
    'required_fields' => array('locality'),
    'dependent_locality_label' => t('Suburb'),
    'locality_label' => t('City'),
    'administrative_area_label' => t('Province'),
    'postal_code_label' => t('Postal code'),
    'render_administrative_area_value' => FALSE,
  );

  $address_formats = array();
  // These formats differ from the default only by the presence of the
  // postal code in 'used_fields'.
  $countries_with_optional_postal_code = array(
    'AC', 'AD', 'AL', 'AZ', 'BA', 'BB', 'BD', 'BG', 'BH', 'BM', 'BN', 'BT',
    'CR', 'CY', 'DO', 'DZ', 'EC', 'EH', 'ET', 'FO', 'GE', 'GN', 'GT',
    'GW', 'HR', 'HT', 'IL', 'IS', 'JO', 'KE', 'KG', 'KH', 'KW', 'LA',
    'LA', 'LB', 'LK', 'LR', 'LS',  'MA', 'MC', 'MD', 'ME', 'MG', 'MK', 'MM',
    'MT', 'MU', 'MV', 'NE', 'NP', 'OM', 'PK', 'PY', 'RO', 'RS', 'SA', 'SI',
    'SN', 'SZ', 'TA', 'TJ', 'TM', 'TN', 'VA', 'VC', 'VG', 'XK', 'ZM',
  );
  foreach ($countries_with_optional_postal_code as $code) {
    $address_formats[$code] = array(
      'used_fields' => array('locality', 'postal_code'),
    );
  }

  // These formats differ from the default only by the presence of the
  // postal code in 'used_fields' and 'required_fields'.
  $countries_with_required_postal_code = array(
    'AT', 'AX', 'BE', 'BL', 'CH', 'CZ', 'DE', 'DK', 'FI', 'FK', 'FR', 'GF', 'GG',
    'GL', 'GP', 'GR', 'GS', 'HU', 'IM', 'IO', 'JE', 'LI', 'LU', 'MF', 'MQ', 'NC',
    'NL', 'NO', 'PL', 'PM', 'PN', 'PT', 'RE', 'SE', 'SH', 'SJ', 'SK', 'TC', 'WF',
    'YT',
  );
  foreach ($countries_with_required_postal_code as $code) {
    $address_formats[$code] = array(
      'used_fields' => array('locality', 'postal_code'),
      'required_fields' => array('locality', 'postal_code'),
    );
  }

  $address_formats['AE'] = array(
    'used_fields' => array('administrative_area'),
    'administrative_area_label' => t('Emirate'),
  );
  $address_formats['AM'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['AR'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['AS'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['AU'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'locality_label' => t('City/Suburb'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('Postcode'),
  );
  $address_formats['BR'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'dependent_locality_label' => t('Neighborhood'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['BS'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['BY'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['CA'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['CC'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['CL'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['CN'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
    'dependent_locality_label' => t('District'),
  );
  $address_formats['CO'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Department', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['CV'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['CX'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['EG'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Governorate'),
  );
  $address_formats['EE'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'administrative_area_label' => t('County'),
  );
  $address_formats['ES'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['FM'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['GB'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'locality_label' => t('Town/City'),
    'administrative_area_label' => t('County'),
    'postal_code_label' => t('Postcode'),
  );
  $address_formats['GI'] = array(
    'used_fields' => array('postal_code'),
  );
  $address_formats['GU'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['HK'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'required_fields' => array('administrative_area'),
    'locality_label' => t('District'),
    'administrative_area_label' => t('Area', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['HN'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['ID'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'locality_label' => t('City/Regency'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['IE'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'locality_label' => t('Town/City'),
    'administrative_area_label' => t('County'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['IN'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('PIN code'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['IQ'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['IR'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'dependent_locality_label' => t('Neighborhood'),
  );
  $address_formats['IT'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['JM'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'required_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('Parish', array(), array('context' => 'Territory of a country')),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['JP'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Prefecture'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['KI'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['KN'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'required_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['KR'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'dependent_locality_label' => t('District'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['KY'] = array(
    'used_fields' => array('administrative_area', 'postal_code'),
    'required_fields' => array('administrative_area'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['KZ'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Region', array(), array('context' => 'Territory of a country')),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['LT'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'administrative_area_label' => t('County'),
  );
  $address_formats['LV'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'administrative_area_label' => t('Municipality'),
  );
  $address_formats['MH'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['MN'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['MP'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['MX'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'dependent_locality_label' => t('Neighborhood'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['MY'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'dependent_locality_label' => t('Village / Township'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['MZ'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['NF'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['NG'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['NI'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Department', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['NR'] = array(
    'used_fields' => array('administrative_area'),
    'required_fields' => array('administrative_area'),
    'administrative_area_label' => t('District'),
  );
  $address_formats['NZ'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'locality_label' => t('Town/City'),
    'postal_code_label' => t('Postcode'),
  );
  $address_formats['PA'] = array(
    'used_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['PE'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'locality_label' => t('District'),
    'administrative_area_label' => t('Department', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['PF'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['PG'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['PH'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['PR'] = array(
    'used_fields' => array('locality', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['PW'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['RU'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['SC'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['SG'] = array(
    'used_fields' => array('postal_code'),
    'required_fields' => array('postal_code'),
  );
  $address_formats['SM'] = array(
    'used_fields' => array('locality', 'postal_code'),
    'required_fields' => array('postal_code'),
  );
  $address_formats['SO'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['SR'] = array(
    'used_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['SV'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
  );
  $address_formats['TH'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['TR'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'locality_label' => t('District'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['TV'] = array(
    'used_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('Island'),
  );
  $address_formats['TW'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['UA'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
    'administrative_area_label' => t('Region', array(), array('context' => 'Territory of a country')),
  );
  $address_formats['UM'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['US'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['UY'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['UZ'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['VE'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area'),
    'render_administrative_area_value' => TRUE,
  );
  $address_formats['VI'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
    'required_fields' => array('locality', 'administrative_area', 'postal_code'),
    'administrative_area_label' => t('State', array(), array('context' => 'Territory of a country')),
    'postal_code_label' => t('ZIP code'),
  );
  $address_formats['VN'] = array(
    'used_fields' => array('locality', 'administrative_area', 'postal_code'),
  );
  $address_formats['ZA'] = array(
    'used_fields' => array('dependent_locality', 'locality', 'postal_code'),
    'required_fields' => array('locality', 'postal_code'),
  );

  // Allow other modules to alter the formats.
  drupal_alter('addressfield_address_formats', $address_formats);

  if (isset($address_formats[$country_code])) {
    $format = $address_formats[$country_code] + $default_values;
  }
  else {
    // There is no predefined address format for the requested country,
    // but the defaults should work fine.
    $format = $default_values;
  }

  return $format;
}
