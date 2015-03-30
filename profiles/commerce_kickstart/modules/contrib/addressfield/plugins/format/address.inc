<?php

/**
 * @file
 * The default format for adresses.
 */

$plugin = array(
  'title' => t('Address form (country-specific)'),
  'format callback' => 'addressfield_format_address_generate',
  'type' => 'address',
  'weight' => -100,
);

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_generate(&$format, $address, $context = array()) {
  // Load the predefined address format for the selected country.
  module_load_include('inc', 'addressfield', 'addressfield.address_formats');
  $address_format = addressfield_get_address_format($address['country']);

  // Used to move certain fields to their own row.
  $clearfix = '<div class="clearfix"></div>';

  // The street block.
  $format['street_block'] = array(
    '#type' => 'addressfield_container',
    '#attributes' => array(
      'class' => array('street-block'),
    ),
    '#weight' => 0,
  );
  $format['street_block']['thoroughfare'] = array(
    '#title' => t('Address 1'),
    '#tag' => 'div',
    '#attributes' => array(
      'class' => array('thoroughfare'),
      'x-autocompletetype' => 'address-line1',
      'autocomplete' => 'address-line1',
    ),
    '#size' => 30,
    '#required' => TRUE,
  );
  $format['street_block']['premise'] = array(
    '#title' => t('Address 2'),
    '#tag' => 'div',
    '#attributes' => array(
      'class' => array('premise'),
      'x-autocompletetype' => 'address-line2',
      'autocomplete' => 'address-line2',
    ),
    '#size' => 30,
  );
  $format['locality_block'] = array(
    '#type' => 'addressfield_container',
    '#attributes' => array(
      'class' => array('addressfield-container-inline', 'locality-block', 'country-' . $address['country']),
    ),
    '#weight' => 50,
  );
  $format['locality_block']['#attached']['css'][] = drupal_get_path('module', 'addressfield') . '/addressfield.css';
  $format['locality_block']['postal_code'] = array(
    '#title' => $address_format['postal_code_label'],
    '#required' => in_array('postal_code', $address_format['required_fields']),
    '#access' => in_array('postal_code', $address_format['used_fields']),
    '#size' => 10,
    '#attributes' => array(
      'class' => array('postal-code'),
      'x-autocompletetype' => 'postal-code',
      'autocomplete' => 'postal-code',
    ),
  );
  $format['locality_block']['dependent_locality'] = array(
    '#title' => $address_format['dependent_locality_label'],
    '#required' => in_array('dependent_locality', $address_format['required_fields']),
    '#access' => in_array('dependent_locality', $address_format['used_fields']),
    '#size' => 25,
    '#tag' => 'div',
    '#attributes' => array(
      'class' => array('dependent-locality')
    ),
    // Most formats place this field in its own row.
    '#suffix' => $clearfix,
  );
  $format['locality_block']['locality'] = array(
    '#title' => $address_format['locality_label'],
    '#required' => in_array('locality', $address_format['required_fields']),
    '#access' => in_array('locality', $address_format['used_fields']),
    '#size' => 30,
    '#prefix' => ' ',
    '#attributes' => array(
      'class' => array('locality'),
      'x-autocompletetype' => 'locality',
      'autocomplete' => 'locality',
    ),
  );
  $format['locality_block']['administrative_area'] = array(
    '#title' => $address_format['administrative_area_label'],
    '#required' => in_array('administrative_area', $address_format['required_fields']),
    '#access' => in_array('administrative_area', $address_format['used_fields']),
    '#empty_value' => '',
    '#size' => 10,
    '#prefix' => ' ',
    '#render_option_value' => $address_format['render_administrative_area_value'],
    '#attributes' => array(
      'class' => array('state'),
      'x-autocompletetype' => 'region',
      'autocomplete' => 'region',
    ),
  );
  $format['country'] = array(
    '#title' => t('Country'),
    '#options' => _addressfield_country_options_list(),
    '#render_option_value' => TRUE,
    '#required' => TRUE,
    '#attributes' => array(
      'class' => array('country'),
      'x-autocompletetype' => 'country',
      'autocomplete' => 'country',
    ),
    '#weight' => 100,
  );

  if (empty($format['locality_block']['postal_code']['#access'])) {
    // Remove the prefix from the first widget of the block.
    $element_children = element_children($format['locality_block']);
    $first_child = reset($element_children);
    unset($format['locality_block'][$first_child]['#prefix']);
  }

  if (!empty($format['locality_block']['administrative_area']['#access'])) {
    // Set the predefined administrative areas, if found.
    module_load_include('inc', 'addressfield', 'addressfield.administrative_areas');
    $administrative_areas = addressfield_get_administrative_areas($address['country']);
    $format['locality_block']['administrative_area']['#options'] = $administrative_areas;
  }

  // Country-specific customizations.
  if (in_array($address['country'], array('AU', 'EE', 'LT', 'LV', 'NZ', 'RU'))) {
    // These countries don't use the "Address 2" line.
    // Leave it as a precaution, but hide the label to avoid confusing users.
    $format['street_block']['thoroughfare']['#title'] = t('Address');
    $format['street_block']['premise']['#title_display'] = 'invisible';
  }
  elseif ($address['country'] == 'US') {
    if ($context['mode'] == 'render') {
      $format['locality_block']['locality']['#suffix'] = ',';
    }
  }
  elseif ($address['country'] == 'BR') {
    $format['locality_block']['dependent_locality']['#suffix'] = $clearfix;
    $format['locality_block']['dependent_locality']['#tag'] = 'div';
    $format['locality_block']['administrative_area']['#suffix'] = $clearfix;
    $format['locality_block']['postal_code']['#tag'] = 'div';
    // Change some titles to make translation easier.
    $format['street_block']['#attributes'] = array(
      'class' => array('addressfield-container-inline'),
    );
    $format['street_block']['thoroughfare']['#title'] = t('Thoroughfare');
    $format['street_block']['thoroughfare']['#tag'] = NULL;
    $format['street_block']['premise'] = array(
      '#title' => t('Complement'),
      '#tag' => NULL,
      '#attributes' => array('class' => array('premise')),
      '#size' => 20,
      '#prefix' => ', ',
    );
    $format['locality_block']['locality']['#suffix'] = ' - ';
    // Hide suffixes and prefixes while in form.
    if ($context['mode'] == 'form') {
      $format['street_block']['premise']['#prefix'] = NULL;
      $format['street_block']['premise']['#suffix'] = NULL;
      $format['locality_block']['locality']['#suffix'] = NULL;
    }
  }
  elseif ($address['country'] == 'CN') {
    $format['locality_block']['locality']['#suffix'] = $clearfix;
  }
  elseif ($address['country'] == 'CA') {
    if ($context['mode'] == 'render') {
      $format['locality_block']['locality']['#suffix'] = ',';
    }
  }
  elseif ($address['country'] == 'GB') {
    $format['locality_block']['administrative_area']['#size'] = '30';
  }
  elseif ($address['country'] == 'ID') {
    $format['locality_block']['administrative_area']['#weight'] = 1;
  }
  elseif ($address['country'] == 'JP') {
    $format['locality_block']['#weight'] = 10;
    $format['locality_block']['postal_code']['#weight'] = 10;
    $format['locality_block']['postal_code']['#tag'] = 'div';
    $format['locality_block']['postal_code']['#size'] = 30;
    $format['locality_block']['administrative_area']['#weight'] = 20;
    $format['locality_block']['administrative_area']['#size'] = 30;
    $format['locality_block']['locality']['#weight'] = 30;
    $format['street_block']['#weight'] = 20;
  }
  elseif ($address['country'] == 'PE') {
    $format['locality_block']['administrative_area']['#weight'] = 1;
    $format['locality_block']['locality']['#weight'] = 2;
  }
  elseif ($address['country'] == 'PH' || $address['country'] == 'TH') {
    $format['locality_block']['dependent_locality']['#suffix'] = '';
    $format['locality_block']['locality']['#suffix'] = $clearfix;
  }

  // These countries show every field in its own row.
  $countries_field_per_row = array(
    'AM', 'EG', 'FK', 'GB', 'GG', 'GS', 'HK', 'HU', 'IE', 'IM', 'IO', 'JE', 'JM',
    'JP', 'KE', 'KR', 'KZ', 'LK', 'PA', 'PN', 'RU', 'SC', 'SH', 'SZ', 'TC', 'UA',
    'VG', 'ZA',
  );
  if (in_array($address['country'], $countries_field_per_row)) {
    $format['locality_block']['#attributes']['class'][0] = 'addressfield-container';
    $format['locality_block']['postal_code']['#prefix'] = '';
    $format['locality_block']['postal_code']['#tag'] = 'div';
    $format['locality_block']['locality']['#prefix'] = '';
    $format['locality_block']['locality']['#tag'] = 'div';
    $format['locality_block']['administrative_area']['#prefix'] = '';
    $format['locality_block']['administrative_area']['#tag'] = 'div';
  }

  // These countries tend to put the postal code after the locality.
  $countries_postal_code_after_locality = array(
    'AS', 'AU', 'BD', 'BF', 'BH', 'BM', 'BN', 'BR', 'BT', 'CA', 'CC', 'CN', 'CX',
    'EG', 'FK', 'FM', 'GB', 'GG', 'GS', 'GU', 'HN', 'HU', 'ID', 'IL', 'IM', 'IN',
    'IO', 'IQ', 'IR', 'JE', 'JO', 'JP', 'KE', 'KH', 'KR', 'LB', 'LK', 'LS', 'LV',
    'MH', 'MM', 'MN', 'MP', 'MT', 'MV', 'MX', 'MY', 'NF', 'NG', 'NP', 'NZ', 'PG',
    'PH', 'PK', 'PN', 'PR', 'PW', 'RU', 'SA', 'SH', 'SO', 'SZ', 'TC', 'TH', 'TW',
    'UA', 'UM', 'US', 'VE', 'VI', 'VG', 'VN', 'ZA',
  );
  if (in_array($address['country'], $countries_postal_code_after_locality)) {
    // Take the widget out of the array.
    $postal_code_widget = $format['locality_block']['postal_code'];
    $postal_code_widget['#prefix'] = ' ';
    unset($format['locality_block']['postal_code']);

    // Add it back.
    $format['locality_block']['postal_code'] = $postal_code_widget;

    // Remove the prefix from the first widget of the block.
    $element_children = element_children($format['locality_block']);
    $first_child = reset($element_children);
    unset($format['locality_block'][$first_child]['#prefix']);
  }

  if ($context['mode'] == 'form') {
    // Provide a wrapper ID for AJAX replacement based on country selection.
    if (!isset($format['#wrapper_id'])) {
      $format['#wrapper_id'] = drupal_html_id('addressfield-wrapper');
      $format['#prefix'] = '<div id="' . $format['#wrapper_id'] . '">';
      $format['#suffix'] = '</div>';
    }
    // AJAX enable it.
    $format['country']['#ajax'] = array(
      'callback' => 'addressfield_standard_widget_refresh',
      'wrapper' => $format['#wrapper_id'],
    );
    $format['country']['#element_validate'] = array('addressfield_standard_country_validate');
    // Don't validate any element when the country is changed.
    $format['country']['#limit_validation_errors'] = array();

    // Move the country selector to the top of the form.
    $format['country']['#weight'] = -500;
    // Limit it to the countries supported by the widget.
    if (isset($context['field'])) {
      $format['country']['#options'] = _addressfield_country_options_list($context['field'], $context['instance']);
    }

    // The whole field is considered empty if the country column is empty.
    // Therefore, if the field is optional, allow the country to be optional.
    // The same logic applies if the field allows multiple values, in which case
    // only the first delta needs to have a value.
    if (empty($context['instance']['required']) || (isset($context['delta']) && $context['delta'] > 0)) {
      $format['country']['#required'] = FALSE;
      $format['country']['#empty_value'] = '';
      // Hide all other fields until the country is selected.
      if (empty($address['country'])) {
        $format['street_block']['#access'] = FALSE;
        $format['locality_block']['#access'] = FALSE;
      }
    }
  }
}
