<?php

/**
 * @file
 * Theme functions for First Data Global Gateway e4.
 */

/**
 * Theme commerce_firstdata_gge4_help_link
 */
function theme_commerce_firstdata_gge4_help_link($vars) {
  $text = isset($vars['text']) ? $vars['text'] : t('Help');
  $options = array(
    'attributes' => array(
      'class' => array('commerce-firstdata-gge4-help-link'),
      'target' => '_blank',
    ),
  );
  if (!empty($vars['options'])) {
    $options = array_merge_recursive($vars['options'], $options);
  }

  return l($text, $vars['path'], $options);
}

/**
 * Theme commerce_firstdata_gge4_ctr
 * - Reproduce GGe4 Customer Transaction Record
 */
function theme_commerce_firstdata_gge4_ctr($vars) {
  $transaction = $vars['transaction'];
  $payload_timestamp = $vars['payload_timestamp'];

  if (!_commerce_firstdata_gge4_check_payment_method($transaction->payment_method)) {
    return '';
  }
  if (!is_array($transaction->payload) || empty($transaction->payload[$payload_timestamp])) {
    return '';
  }

  $data = $transaction->payload[$payload_timestamp];
  $output = '';


  $merchant_address_lines = array();
  if (isset($data['merchant_name'])) {
    $merchant_address_lines[] = check_plain($data['merchant_name']);
  }
  if (isset($data['merchant_address'])) {
    $merchant_address_lines[] = check_plain($data['merchant_address']);
  }
  if (isset($data['merchant_city']) && isset($data['merchant_province']) && isset($data['merchant_province'])) {
    $merchant_address_lines[] = t('@city, @province @postal', array(
      '@city' => $data['merchant_city'],
      '@province' => $data['merchant_province'],
      '@postal' => $data['merchant_postal'],
    ));
  }
  if (isset($data['merchant_country'])) {
    $merchant_address_lines[] = check_plain($data['merchant_country']);
  }
  if (!empty($merchant_address_lines)) {
    $output .= '<div class="commerce-firstdata-gge4-ctr-merchant commerce-firstdata-gge4-ctr-section">'. implode('<br />', $merchant_address_lines) . '</div>';
  }

  $output .= '<div class="commerce-firstdata-gge4-ctr-account commerce-firstdata-gge4-ctr-section">';
  $output .= t('@card_type@amount', array(
    '@card_type' => !empty($data['credit_card_type']) ? $data['credit_card_type'] . ' ' : '',
    '@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code),
  ));
  $output .= '</div>';

  $detail_lines = array();
  if (!empty($data['cardholder_name'])) {
    $detail_lines[] = t('Cardholder name: @value', array('@value' => $data['cardholder_name']));
  }
  if (!empty($data['cc_number'])) {
    $detail_lines[] = t('Card number (last 4): @value', array('@value' => substr($data['cc_number'], -4)));
  }

  $detail_lines[] = t('Date: @date', array( '@date' => format_date($payload_timestamp, 'short')));

  $detail_lines[] = t('Reference number: @value', array('@value' => $transaction->remote_id));

  if (!empty($data['authorization_num'])) {
    $detail_lines[] = t('Authorization number: @value', array('@value' => $data['authorization_num']));
  }

  if (!empty($data['reference_no'])) {
    $detail_lines[] = t('Transaction reference: @value', array('@value' => $data['reference_no']));
  }

  $output .= '<div class="commerce-firstdata-gge4-ctr-details commerce-firstdata-gge4-ctr-section">';
  $output .= implode('<br />', $detail_lines);
  $output .= '</div>';

  if (!empty($data['transaction_approved'])) {
    $output .= '<div class="commerce-firstdata-gge4-ctr-approval commerce-firstdata-gge4-ctr-section">';
    $output .= t('Approved - Thank You');
    $output .= '</div>';
  }

  $output .= '<div class="commerce-firstdata-gge4-ctr-reminder commerce-firstdata-gge4-ctr-section">';
  $output .= t('Please retain this copy for your records.');
  $output .= '</div>';


  $output .= '<div class="commerce-firstdata-gge4-ctr-legal commerce-firstdata-gge4-ctr-section">';
  $output .= t('Cardholder will pay above amount to card issuer pursuant to cardholder agreement.');
  $output .= '</div>';

  return '<div class="commerce-firstdata-gge4-ctr">' . $output . '</div>';
}
