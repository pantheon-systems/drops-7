<?php

/**
 * @file
 * Page callbacks.
 */

/**
 * Form callback wrapper: create or edit a card.
 *
 * @param $card
 *   The card object to edit through the form.
 * @param $account
 *   For new cards, the customer's user account.
 *
 * @see commerce_firstdata_gge4_cardonfile_form()
 */
function commerce_firstdata_gge4_cardonfile_form_wrapper($card, $account = NULL) {
  global $user;

  // Set the page title and a default owner if necessary.
  if (empty($card->card_id)) {
    drupal_set_title(t('Add new credit card'));

    // Set properties cached in menu's stub card
    $card->created = REQUEST_TIME;
    $card->uid = !empty($account) ? $account->uid : $user->uid;
  }

  // Load API controller
  $gateway = commerce_firstdata_gge4_get_controller($card->instance_id);

  // Preference given to hosted payment if enabled
  // Redirect to custom form - currently only hpp
  if ($gateway->isActive('hpp')) {
    module_load_include('inc', 'commerce_firstdata_gge4', 'includes/commerce_firstdata_gge4.forms');
    return drupal_get_form('commerce_firstdata_gge4_cardonfile_form', $card);
  }

  // Fallback to commerce_cardonfile
  $op = empty($card->card_id) ? 'create' : 'update';
  if (commerce_cardonfile_access($op, $card, $account)) {
    module_load_include('inc', 'commerce_cardonfile', 'includes/commerce_cardonfile.pages');
    return drupal_get_form('commerce_cardonfile_card_form', $op, $card);
  }


  return MENU_ACCESS_DENIED;
}
