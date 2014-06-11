<?php
/**
 * @file
 * commerce_exactor.admin.inc
 *   Administrative forms for Commerce Exactor module.
 */

/**
 * Settings callback form.
 */
function commerce_exactor_settings_form($form_state) {

  $form['introduction'] = array(
    '#type' => 'markup',
    '#markup' => '<p><img align="right" src = "' . file_create_url(drupal_get_path('module', 'commerce_exactor') . '/theme/exactor_logo.png') . '">' .
    t('Exactor is a fully automated, end-to-end system to calculate sales and use taxes and file tax returns. It calculates taxes wherever necessary, in real time, and without impacting transactions.')
    . '</p>',
  );

  $form['links'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' .
    t('For more information visit the <a href=@marketplace>Commerce Marketplace</a>, or signup now with <a href=@exactor>Exactor</a>.', array(
      '@marketplace' => 'https://marketplace.commerceguys.com/add-on/exactor/overview',
      '@exactor' => 'https://commerceguys.exactor.com/account/signup/application',
      ))
    . '</p>',
  );

  $form['exactor_account_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Exactor Account ID'),
    '#default_value' => variable_get('exactor_account_id'),
    '#description' => t('Your 8 digit account ID.'),
    '#required' => TRUE,
  );

  $form['exactor_user_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Exactor User ID'),
    '#default_value' => variable_get('exactor_user_id'),
    '#description' => t('Your alphanumeric user ID.'),
    '#required' => TRUE,
  );

  $form['exactor_name_line'] = array(
    '#type' => 'textfield',
    '#title' => t('Business Name'),
    '#default_value' => variable_get('exactor_name_line'),
    '#description' => t('Your business name.'),
    '#required' => TRUE,
  );

  $form['exactor_thoroughfare'] = array(
    '#type' => 'textfield',
    '#title' => t('Street 1'),
    '#default_value' => variable_get('exactor_thoroughfare'),
    '#required' => TRUE,
  );

  $form['exactor_premise'] = array(
    '#type' => 'textfield',
    '#title' => t('Street 2'),
    '#default_value' => variable_get('exactor_premise'),
    '#required' => FALSE,
  );

  $form['exactor_locality'] = array(
    '#type' => 'textfield',
    '#title' => t('Town / City'),
    '#default_value' => variable_get('exactor_locality'),
    '#required' => TRUE,
  );

  $form['exactor_administrative_area'] = array(
    '#type' => 'textfield',
    '#title' => t('State or Province'),
    '#default_value' => variable_get('exactor_administrative_area'),
    '#required' => TRUE,
  );

  $form['exactor_postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
    '#default_value' => variable_get('exactor_postal_code'),
    '#required' => TRUE,
  );

  $form['exactor_country'] = array(
    '#type' => 'textfield',
    '#title' => t('Country'),
    '#default_value' => variable_get('exactor_country', 'USA'),
    '#description' => t('3 character country code.'),
    '#required' => TRUE,
  );

  $form['#validate'][] = 'commerce_exactor_settings_form_request';

  return system_settings_form($form);
}

/**
 * Form validate to check login details.
 */
function commerce_exactor_settings_form_request($form, &$form_state) {
  $url = 'https://taxrequest.exactor.com/xml/invoice';

  $xml = new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8" standalone="yes"?><TaxRequest xmlns="http://www.exactor.com/ns"></TaxRequest>');
  $xml->MerchantId = $form_state['values']['exactor_account_id'];
  $xml->UserId = $form_state['values']['exactor_user_id'];

  $from_address = array(
    'name_line' => $form_state['values']['exactor_name_line'],
    'thoroughfare' => $form_state['values']['exactor_thoroughfare'],
    'premise' => $form_state['values']['exactor_premise'],
    'locality' => $form_state['values']['exactor_locality'],
    'administrative_area' => $form_state['values']['exactor_administrative_area'],
    'postal_code' => $form_state['values']['exactor_postal_code'],
    'country' => $form_state['values']['exactor_country'],
  );

  $invoice_request = $xml->addChild('InvoiceRequest');

  $invoice_request->SaleDate = date("Y-m-d", time());
  $invoice_request->ExemptionId = '1';

  $invoice_request->BillTo->FullName = check_plain($from_address['name_line']);
  $invoice_request->BillTo->Street1 = check_plain($from_address['thoroughfare']);
  $invoice_request->BillTo->Street2 = check_plain($from_address['premise']);
  $invoice_request->BillTo->City = check_plain($from_address['locality']);
  $invoice_request->BillTo->StateOrProvince = check_plain($from_address['administrative_area']);
  $invoice_request->BillTo->PostalCode = check_plain($from_address['postal_code']);
  $invoice_request->BillTo->Country = check_plain($from_address['country']);

  $invoice_request->ShipTo->FullName = check_plain($from_address['name_line']);
  $invoice_request->ShipTo->Street1 = check_plain($from_address['thoroughfare']);
  $invoice_request->ShipTo->Street2 = check_plain($from_address['premise']);
  $invoice_request->ShipTo->City = check_plain($from_address['locality']);
  $invoice_request->ShipTo->StateOrProvince = check_plain($from_address['administrative_area']);
  $invoice_request->ShipTo->PostalCode = check_plain($from_address['postal_code']);
  $invoice_request->ShipTo->Country = check_plain($from_address['country']);

  $invoice_request->ShipFrom->FullName = check_plain($from_address['name_line']);
  $invoice_request->ShipFrom->Street1 = check_plain($from_address['thoroughfare']);
  $invoice_request->ShipFrom->Street2 = check_plain($from_address['premise']);
  $invoice_request->ShipFrom->City = check_plain($from_address['locality']);
  $invoice_request->ShipFrom->StateOrProvince = check_plain($from_address['administrative_area']);
  $invoice_request->ShipFrom->PostalCode = check_plain($from_address['postal_code']);
  $invoice_request->ShipFrom->Country = check_plain($from_address['country']);

  $line = $invoice_request->addChild('LineItem');
  $line->addAttribute('id', '1');
  $line->addChild('Quantity', '1');
  $line->addChild('GrossAmount', '1.00');
  $line->addChild('SKU', 'SKU');
  $line->addChild('Description', 'Test Item');

  $request = commerce_exactor_xml_post($url, $xml->asXML());

  if (isset($request->data)) {
    $data = new SimpleXMLElement($request->data);
  }
  elseif (isset($request->error)) {
    form_set_error('exactor', t('Connection to Exactor unsucessful (Error: @error)', array('@error' => $request->error)));
    watchdog('commerce_exactor_calc', $request->error);
    return FALSE;
  }
  else {
    form_set_error('exactor', t('Unknown Error'));
    watchdog('commerce_exactor_calc', 'Unknown Error Contacting Exactor');
    return FALSE;
  }

  if (isset($data->ErrorResponse->ErrorDescription)) {
    form_set_error('exactor', (string) $data->ErrorResponse->ErrorDescription);
    watchdog('commerce_exactor_calc', (string) $data->ErrorResponse->ErrorDescription);
    return FALSE;
  }

}
