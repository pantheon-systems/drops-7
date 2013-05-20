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
  // We start with a reasonable default: a simple block format suitable
  // for international shipping. We extend it with country-specific heuristics
  // below.

  // The street block.
  $format['street_block'] = array(
    '#type' => 'addressfield_container',
    '#attributes' => array('class' => array('street-block')),
    '#weight' => 0,
  );
  $format['street_block']['thoroughfare'] = array(
    '#title' => t('Address 1'),
    '#tag' => 'div',
    '#attributes' => array('class' => array('thoroughfare')),
    '#size' => 30,
    // The #required will be automatically set to FALSE when processing.
    '#required' => TRUE,
  );
  $format['street_block']['premise'] = array(
    '#title' => t('Address 2'),
    '#tag' => 'div',
    '#attributes' => array('class' => array('premise')),
    '#size' => 30,
  );
  $format['locality_block'] = array(
    '#type' => 'addressfield_container',
    '#attributes' => array('class' => array('addressfield-container-inline', 'locality-block', 'country-' . $address['country'])),
    '#weight' => 50,
  );
  $format['locality_block']['#attached']['css'][] = drupal_get_path('module', 'addressfield') . '/addressfield.css';
  $format['locality_block']['postal_code'] = array(
    '#title' => t('Postal code'),
    '#size' => 10,
    '#required' => TRUE,
    '#attributes' => array('class' => array('postal-code')),
  );
  $format['locality_block']['locality'] = array(
    '#title' => t('City'),
    '#size' => 30,
    '#required' => TRUE,
    '#prefix' => ' ',
    '#attributes' => array('class' => array('locality')),
  );
  $format['country'] = array(
    '#title' => t('Country'),
    '#options' => _addressfield_country_options_list(),
    '#render_option_value' => TRUE,
    '#required' => TRUE,
    '#attributes' => array('class' => array('country')),
    '#weight' => 100,
  );

  // Those countries do not seem to have a relevant postal code.
  static $countries_no_postal_code = array('AF', 'AG', 'AL', 'AO', 'BB', 'BI', 'BJ', 'BO', 'BS', 'BW', 'BZ', 'CF', 'CG', 'CM', 'CO', 'DJ', 'DM', 'EG', 'ER', 'FJ', 'GD', 'GH', 'GM', 'GQ', 'GY', 'HK', 'IE', 'KI', 'KM', 'KP', 'KY', 'LC', 'LY', 'ML', 'MR', 'NA', 'NR', 'RW', 'SB', 'SC', 'SL', 'SR', 'ST', 'TD', 'TG', 'TL', 'TO', 'TT', 'TV', 'TZ', 'UG', 'VC', 'VU', 'WS', 'ZW');
  if (in_array($address['country'], $countries_no_postal_code)) {
    unset($format['locality_block']['postal_code']);

    // Remove the prefix from the first widget of the block.
    $element_children = element_children($format['locality_block']);
    $first_child = reset($element_children);
    unset($format['locality_block'][$first_child]['#prefix']);
  }

  // Those countries generally use the administrative area in postal addresses.
  static $countries_administrative_area = array('AR', 'AU', 'BR', 'BS', 'BY', 'BZ', 'CA', 'CN', 'DO', 'EG', 'ES', 'FJ', 'FM', 'GB', 'HN', 'ID', 'IE', 'IN', 'IT', 'JO', 'JP', 'KI', 'KN', 'KR', 'KW', 'KY', 'KZ', 'MX', 'MY', 'MZ', 'NG', 'NI', 'NR', 'NZ', 'OM', 'PA', 'PF', 'PG', 'PH', 'PR', 'PW', 'RU', 'SM', 'SO', 'SR', 'SV', 'TH', 'TW', 'UA', 'US', 'UY', 'VE', 'VI', 'VN', 'YU', 'ZA');
  if (in_array($address['country'], $countries_administrative_area)) {
    $format['locality_block']['administrative_area'] = array(
      '#title' => t('State', array(), array('context' => 'addressfield')),
      '#size' => 10,
      '#required' => TRUE,
      '#prefix' => ' ',
      '#attributes' => array('class' => array('state')),
    );
  }

  // A few countries have a well-known list of administrative divisions.
  if ($address['country'] == 'US') {
    $format['locality_block']['administrative_area']['#options'] = array(
      ''   => t('--'),
      'AL' => t('Alabama'),
      'AK' => t('Alaska'),
      'AZ' => t('Arizona'),
      'AR' => t('Arkansas'),
      'CA' => t('California'),
      'CO' => t('Colorado'),
      'CT' => t('Connecticut'),
      'DE' => t('Delaware'),
      'DC' => t('District Of Columbia'),
      'FL' => t('Florida'),
      'GA' => t('Georgia'),
      'HI' => t('Hawaii'),
      'ID' => t('Idaho'),
      'IL' => t('Illinois'),
      'IN' => t('Indiana'),
      'IA' => t('Iowa'),
      'KS' => t('Kansas'),
      'KY' => t('Kentucky'),
      'LA' => t('Louisiana'),
      'ME' => t('Maine'),
      'MD' => t('Maryland'),
      'MA' => t('Massachusetts'),
      'MI' => t('Michigan'),
      'MN' => t('Minnesota'),
      'MS' => t('Mississippi'),
      'MO' => t('Missouri'),
      'MT' => t('Montana'),
      'NE' => t('Nebraska'),
      'NV' => t('Nevada'),
      'NH' => t('New Hampshire'),
      'NJ' => t('New Jersey'),
      'NM' => t('New Mexico'),
      'NY' => t('New York'),
      'NC' => t('North Carolina'),
      'ND' => t('North Dakota'),
      'OH' => t('Ohio'),
      'OK' => t('Oklahoma'),
      'OR' => t('Oregon'),
      'PA' => t('Pennsylvania'),
      'RI' => t('Rhode Island'),
      'SC' => t('South Carolina'),
      'SD' => t('South Dakota'),
      'TN' => t('Tennessee'),
      'TX' => t('Texas'),
      'UT' => t('Utah'),
      'VT' => t('Vermont'),
      'VA' => t('Virginia'),
      'WA' => t('Washington'),
      'WV' => t('West Virginia'),
      'WI' => t('Wisconsin'),
      'WY' => t('Wyoming'),
      ' ' => t('--'),
      'AA' => t('Armed Forces (Americas)'),
      'AE' => t('Armed Forces (Europe, Canada, Middle East, Africa)'),
      'AP' => t('Armed Forces (Pacific)'),
      'AS' => t('American Samoa'),
      'FM' => t('Federated States of Micronesia'),
      'GU' => t('Guam'),
      'MH' => t('Marshall Islands'),
      'MP' => t('Northern Mariana Islands'),
      'PW' => t('Palau'),
      'PR' => t('Puerto Rico'),
      'VI' => t('Virgin Islands'),
    );
    $format['locality_block']['postal_code']['#title'] = t('ZIP Code');

    if ($context['mode'] == 'render') {
      $format['locality_block']['locality']['#suffix'] = ',';
    }
  }
  else if ($address['country'] == 'IT') {
    $format['locality_block']['administrative_area']['#options'] = array(
      '' => t('--'),
      'AG' => t('Agrigento'),
      'AL' => t('Alessandria'),
      'AN' => t('Ancona'),
      'AO' => t("Valle d'Aosta/Vallée d'Aoste"),
      'AP' => t('Ascoli Piceno'),
      'AQ' => t("L'Aquila"),
      'AR' => t('Arezzo'),
      'AT' => t('Asti'),
      'AV' => t('Avellino'),
      'BA' => t('Bari'),
      'BG' => t('Bergamo'),
      'BI' => t('Biella'),
      'BL' => t('Belluno'),
      'BN' => t('Benevento'),
      'BO' => t('Bologna'),
      'BR' => t('Brindisi'),
      'BS' => t('Brescia'),
      'BT' => t('Barletta-Andria-Trani'),
      'BZ' => t('Bolzano/Bozen'),
      'CA' => t('Cagliari'),
      'CB' => t('Campobasso'),
      'CE' => t('Caserta'),
      'CH' => t('Chieti'),
      'CI' => t('Carbonia-Iglesias'),
      'CL' => t('Caltanissetta'),
      'CN' => t('Cuneo'),
      'CO' => t('Como'),
      'CR' => t('Cremona'),
      'CS' => t('Cosenza'),
      'CT' => t('Catania'),
      'CZ' => t('Catanzaro'),
      'EN' => t('Enna'),
      'FC' => t('Forlì-Cesena'),
      'FE' => t('Ferrara'),
      'FG' => t('Foggia'),
      'FI' => t('Firenze'),
      'FM' => t('Fermo'),
      'FR' => t('Frosinone'),
      'GE' => t('Genova'),
      'GO' => t('Gorizia'),
      'GR' => t('Grosseto'),
      'IM' => t('Imperia'),
      'IS' => t('Isernia'),
      'KR' => t('Crotone'),
      'LC' => t('Lecco'),
      'LE' => t('Lecce'),
      'LI' => t('Livorno'),
      'LO' => t('Lodi'),
      'LT' => t('Latina'),
      'LU' => t('Lucca'),
      'MB' => t('Monza e Brianza'),
      'MC' => t('Macerata'),
      'ME' => t('Messina'),
      'MI' => t('Milano'),
      'MN' => t('Mantova'),
      'MO' => t('Modena'),
      'MS' => t('Massa-Carrara'),
      'MT' => t('Matera'),
      'NA' => t('Napoli'),
      'NO' => t('Novara'),
      'NU' => t('Nuoro'),
      'OG' => t('Ogliastra'),
      'OR' => t('Oristano'),
      'OT' => t('Olbia-Tempio'),
      'PA' => t('Palermo'),
      'PC' => t('Piacenza'),
      'PD' => t('Padova'),
      'PE' => t('Pescara'),
      'PG' => t('Perugia'),
      'PI' => t('Pisa'),
      'PN' => t('Pordenone'),
      'PO' => t('Prato'),
      'PR' => t('Parma'),
      'PT' => t('Pistoia'),
      'PU' => t('Pesaro e Urbino'),
      'PV' => t('Pavia'),
      'PZ' => t('Potenza'),
      'RA' => t('Ravenna'),
      'RC' => t('Reggio di Calabria'),
      'RE' => t("Reggio nell'Emilia"),
      'RG' => t('Ragusa'),
      'RI' => t('Rieti'),
      'RM' => t('Roma'),
      'RN' => t('Rimini'),
      'RO' => t('Rovigo'),
      'SA' => t('Salerno'),
      'SI' => t('Siena'),
      'SO' => t('Sondrio'),
      'SP' => t('La Spezia'),
      'SR' => t('Siracusa'),
      'SS' => t('Sassari'),
      'SV' => t('Savona'),
      'TA' => t('Taranto'),
      'TE' => t('Teramo'),
      'TN' => t('Trento'),
      'TO' => t('Torino'),
      'TP' => t('Trapani'),
      'TR' => t('Terni'),
      'TS' => t('Trieste'),
      'TV' => t('Treviso'),
      'UD' => t('Udine'),
      'VA' => t('Varese'),
      'VB' => t('Verbano-Cusio-Ossola'),
      'VC' => t('Vercelli'),
      'VE' => t('Venezia'),
      'VI' => t('Vicenza'),
      'VR' => t('Verona'),
      'VS' => t('Medio Campidano'),
      'VT' => t('Viterbo'),
      'VV' => t('Vibo Valentia'),
    );
    $format['locality_block']['administrative_area']['#title'] = t('Province');
  }
  else if ($address['country'] == 'BR') {
    $format['locality_block']['administrative_area']['#render_option_value'] = TRUE;
    $format['locality_block']['administrative_area']['#options'] = array(
      '' => t('--'),
      'AC' => t('Acre'),
      'AL' => t('Alagoas'),
      'AM' => t('Amazonas'),
      'AP' => t('Amapá'),
      'BA' => t('Bahia'),
      'CE' => t('Ceará'),
      'DF' => t('Distrito Federal'),
      'ES' => t('Espírito Santo'),
      'GO' => t('Goiás'),
      'MA' => t('Maranhão'),
      'MG' => t('Minas Gerais'),
      'MS' => t('Mato Grosso do Sul'),
      'MT' => t('Mato Grosso'),
      'PA' => t('Pará'),
      'PB' => t('Paraíba'),
      'PE' => t('Pernambuco'),
      'PI' => t('Piauí'),
      'PR' => t('Paraná'),
      'RJ' => t('Rio de Janeiro'),
      'RN' => t('Rio Grande do Norte'),
      'RO' => t('Rondônia'),
      'RR' => t('Roraima'),
      'RS' => t('Rio Grande do Sul'),
      'SC' => t('Santa Catarina'),
      'SE' => t('Sergipe'),
      'SP' => t('São Paulo'),
      'TO' => t('Tocantins'),
    );
  }
  else if ($address['country'] == 'CA') {
    $format['locality_block']['administrative_area']['#options'] = array(
      '' => t('--'),
      'AB' => t('Alberta'),
      'BC' => t('British Columbia'),
      'MB' => t('Manitoba'),
      'NB' => t('New Brunswick'),
      'NL' => t('Newfoundland and Labrador'),
      'NT' => t('Northwest Territories'),
      'NS' => t('Nova Scotia'),
      'NU' => t('Nunavut'),
      'ON' => t('Ontario'),
      'PE' => t('Prince Edward Island'),
      'QC' => t('Quebec'),
      'SK' => t('Saskatchewan'),
      'YT' => t('Yukon Territory'),
    );
    $format['locality_block']['administrative_area']['#title'] = t('Province');

    if ($context['mode'] == 'render') {
      $format['locality_block']['locality']['#suffix'] = ',';
    }
  }
  else if ($address['country'] == 'AU') {
    $format['locality_block']['administrative_area']['#options'] = array(
      '' => t('--'),
      'ACT' => t('Australian Capital Territory'),
      'NSW' => t('New South Wales'),
      'NT' => t('Northern Territory'),
      'QLD' => t('Queensland'),
      'SA' => t('South Australia'),
      'TAS' => t('Tasmania'),
      'VIC' => t('Victoria'),
      'WA' => t('Western Australia'),
    );
  }
  else if ($address['country'] == 'NZ') {
    $format['locality_block']['locality']['#title'] = ('Town/City');
    $format['locality_block']['postal_code']['#title'] = t('Postcode');
    $format['locality_block']['administrative_area']['#render_option_value'] = TRUE;
    $format['locality_block']['administrative_area']['#title'] = t('Region');
    $format['locality_block']['administrative_area']['#required'] = FALSE;
    $format['locality_block']['administrative_area']['#options'] = array(
      ''   => t('--'),
      'AUK' => t('Auckland'),
      'BOP' => t('Bay of Plenty'),
      'CAN' => t('Canterbury'),
      'HKB' => t("Hawke's Bay"),
      'MWT' => t('Manawatu-Wanganui'),
      'NTL' => t('Northland'),
      'OTA' => t('Otago'),
      'STL' => t('Southland'),
      'TKI' => t('Taranaki'),
      'WKO' => t('Waikato'),
      'WGN' => t('Wellington'),
      'WTC' => t('West Coast'),
      'GIS' => t('Gisborne District'),
      'MBH' => t('Marlborough District'),
      'NSN' => t('Nelson City'),
      'TAS' => t('Tasman District'),
      'CIT' => t('Chatham Islands Territory'),
    );
  }

  // Those countries tend to put the postal code after the locality.
  static $countries_postal_code_after_locality = array('AU', 'BD', 'BF', 'BH', 'BM', 'BN', 'BT', 'CA', 'FM', 'GB', 'ID', 'IN', 'JM', 'JO', 'KH', 'LB', 'LS', 'LV', 'MM', 'MN', 'MV', 'MW', 'NG', 'NP', 'NZ', 'PE', 'PK', 'PR', 'PW', 'SA', 'SG', 'SO', 'TH', 'US', 'VI', 'VG', 'VN');
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

  // GB-specific tweaks
  if ($address['country'] == 'GB') {
    // Locality
    $format['locality_block']['locality'] = array_merge(
      $format['locality_block']['locality'],
      array(
        '#title' => t('Town/City'),
        '#weight' => 40,
        '#prefix' => '',
        '#tag' => 'div',
      )
    );

    // Administrative
    $format['locality_block']['administrative_area'] = array_merge(
      $format['locality_block']['administrative_area'],
      array(
        '#title' => t('County'),
        '#required' => FALSE,
        '#weight' => 50,
        '#size' => 30,
        '#prefix' => '',
        '#tag' => 'div',
      )
    );

    // Postal code
    $format['locality_block']['postal_code'] = array_merge(
      $format['locality_block']['postal_code'],
      array(
        '#title' => t('Postcode'),
        '#weight' => 60,
        '#prefix' => '',
        '#tag' => 'div',
      )
    );
  }

  if ($context['mode'] == 'form') {
    // Provide a wrapper ID for AJAX replacement based on country selection.
    if (!isset($format['#wrapper_id'])) {
      $format['#wrapper_id'] = drupal_html_id('addressfield-wrapper');
      $format['#prefix'] = '<div id="' . $format['#wrapper_id'] . '">';
      $format['#suffix'] = '</div>';
    }

    // Form mode, move the country selector to the top of the form.
    $format['country']['#weight'] = -10;

    // Limit it to the countries supported by the widget.
    if (isset($context['field'])) {
      $format['country']['#options'] = _addressfield_country_options_list($context['field'], $context['instance']);
    }

    // AJAX enable it.
    $format['country']['#ajax'] = array(
      'callback' => 'addressfield_standard_widget_refresh',
      'wrapper' => $format['#wrapper_id'],
    );
    $format['country']['#element_validate'] = array('addressfield_standard_country_validate');
    // Don't validate any element when the country is changed.
    $format['country']['#limit_validation_errors'] = array();

    if (isset($context['delta']) && $context['delta'] > 0) {
      // On subsequent elements of a field, we make the country field non
      // required and add a ' - None - ' option to it, so as to allow the
      // user to remove the address by clearing the country field.
      $format['country']['#required'] = FALSE;
      $format['country']['#empty_value'] = '';
    }
  }
}
