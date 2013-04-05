<?php

/**
 * @file
 * Extra functions for the price formatter module
 *
 */

/**
 *  Converts a price to a formatted currency but removes everything
 *  after the decimal.
 */
function commerce_extra_price_no_decimal_currency_format($amount, $currency_code, $object = NULL, $convert = TRUE, $options = array()) {
  // First load the currency array.
  $currency = commerce_currency_load($currency_code);

  // Then convert the price amount to the currency's major unit decimal value.
  if ($convert = TRUE) {
    $amount = commerce_currency_amount_to_decimal($amount, $currency_code);
  }

  // Invoke the custom format callback if specified.
  if (!empty($currency['format_callback'])) {
    return $currency['format_callback']($amount, $currency, $object);
  }

  $price = commerce_currency_round(abs($amount), $currency);

  if (isset($options['raw']) && $options['raw']) {
    // Format the price as a number.
    return trim($price);
  }

  // Format the price as a number.
  $price = number_format($price, 0, $currency['decimal_separator'], $currency['thousands_separator']);

  // Establish the replacement values to format this price for its currency.
  $replacements = array(
    '@code_before' => $currency['code_placement'] == 'before' ? $currency['code'] : '',
    '@symbol_before' => $currency['symbol_placement'] == 'before' ? $currency['symbol'] : '',
    '@price' => $price,
    '@symbol_after' => $currency['symbol_placement'] == 'after' ? $currency['symbol'] : '',
    '@code_after' => $currency['code_placement'] == 'after' ? $currency['code'] : '',
    '@negative' => $amount < 0 ? '-' : '',
  );

  $formatted_price = trim(t('@code_before @negative@symbol_before@price @symbol_after @code_after', $replacements));

  return trim($formatted_price);
}