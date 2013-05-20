<?php
/**
 * @file
 *   commerce_exactor_avs.rules.inc
 * Rules integrations for ExactoAVS
 */

/**
 * Impliments hook_rules_action_info().
 */
function commerce_exactor_avs_rules_action_info() {
  $actions = array();

  $actions['commerce_exactor_avs_request'] = array(
    'label' => t('Submit an address for validation'),
    'parameter' => array(
      'input_address' => array(
        'type' => 'addressfield',
        'label' => t('Address'),
      ),
    ),
    'provides' => array(
      'output_address' => array(
        'type' => 'addressfield',
        'label' => t('Output Address'),
      ),
      'status' => array(
        'type' => 'integer',
        'label' => t('ExactoAVS Status'),
      ),
    ),
    'group' => t('Commerce Exactor'),
    'callbacks' => array(
      'execute' => 'commerce_exactor_avs_request',
    ),
  );

  return $actions;
}

/**
 * Rules action callback
 */
function commerce_exactor_avs_request($input_address) {
  $data = array();

  if ($input_address['country'] != 'US') {
    return;
  }

  $input_address['country'] = countries_country_lookup($input_address['country'], 'iso2');

  $url = 'https://avs.exactor.com/avs/v1/xml';

  $xml = '<?xml version="1.0" encoding="UTF-8" ?>';
  $xml .= '<avs:AddressRequest xmlns:avs="http://www.exactor.com/ns_avs/v1">';
  $xml .= '<MerchantId>' . variable_get('exactor_account_id') . '</MerchantId>';
  $xml .= '<UserId>' . variable_get('exactor_user_id') . '</UserId>';
  $xml .= '<Address>';
  $xml .= '<FullName>' . $input_address['name_line'] . '</FullName>';
  $xml .= '<Street1>' . $input_address['thoroughfare'] . '</Street1>';
  $xml .= '<Street2>' . $input_address['premise'] . '</Street2>';
  $xml .= '<City>' . $input_address['locality'] . '</City>';
  if (isset($input_address['sub_administrative_area'])) {
    $xml .= '<County>' . $input_address['sub_administrative_area'] . '</City>';
  }
  $xml .= '<StateOrProvince>' . $input_address['administrative_area'] . '</StateOrProvince>';
  $xml .= '<PostalCode>' . $input_address['postal_code'] . '</PostalCode>';
  $xml .= '<Country>' . $input_address['country']->iso3 . '</Country>';
  $xml .= '</Address>';
  $xml .= '</avs:AddressRequest>';

  $result = commerce_exactor_xml_post($url, $xml);

  watchdog('commerce_exactor_avs', '<pre>' . print_r($input_address, TRUE) . '</pre>');

  $data = new SimpleXMLElement($result->data);

  $output_address = $input_address;
  $output_address['name_line'] = (string) $data->Address->FullName;
  $output_address['thoroughfare'] = (string) $data->Address->Street1;
  $output_address['premise'] = (string) $data->Address->Street2;
  $output_address['locality'] = (string) $data->Address->City;
  $output_address['sub_administrative_area'] = (string) $data->Address->County;
  $output_address['administrative_area'] = (string) $data->Address->StateOrProvince;
  $output_address['postal_code'] = (string) $data->Address->PostalCode;
  $output_address['country'] = countries_country_lookup((string) $data->Address->Country, 'iso3');
  $output_address['country'] = $output_address['country']->iso2;

  watchdog('commerce_exactor_avs', '<pre>' . print_r($output_address, TRUE) . '</pre>');

  return array(
    'output_address' => $output_address,
    'status' => (int) $data->Address->attributes()->status,
  );
}
