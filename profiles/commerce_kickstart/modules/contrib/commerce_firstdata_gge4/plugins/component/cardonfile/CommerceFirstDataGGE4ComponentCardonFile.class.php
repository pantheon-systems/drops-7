<?php

/**
 * @file
 * First Data Global Gateway e4 API Hosted Payment Pages Component.
 */

/**
 * Hosted Payment Pages plugin class
 */
class CommerceFirstDataGGE4ComponentCardonFile extends CommerceFirstDataGGE4ComponentBase {

  /**
   * Returns TRUE if the plugin is enabled
   */
  public function isValid() {
    return $this->getSettings('enable');
  }

  /**
   * Default settings
   */
  public function defaultSettings() {
    $settings = parent::defaultSettings();

    return $settings;
  }

  /**
   * Settings form
   */
  public function settingsForm() {
    $form = parent::settingsForm();

    return $form;
  }

  /**
   * Returns an array of fields that cannot be changed.
   * - Used to determine if a new card object should be saved
   */
  public function cardImmutableProperties() {
    return array(
      'card_id',
      'uid',
      'payment_method',
      'instance_id',
      'remote_id',
      'card_name',
      'card_number',
      'card_type',
      'card_exp_month',
      'card_exp_year',
    );
  }

  /**
   * Event handler
   * - Only called if this plugin is enabled and valid
   */
  public function on($event_name, &$context) {
    switch ($event_name) {
      case 'ws_request_validate':
        return $this->wsRequestValidate($context);
      case 'response_process':
        return $this->processCardResponse($context);
    }
  }

  /**
   * Alter Event handler
   * - Only called if this plugin is enabled and valid
   */
  public function onAlter($event_name, &$data, &$context) {
    switch ($event_name) {
      case 'ws_request_build':
        $this->wsRequestBuildAlter($data, $context);
        break;

      case 'hpp_post_data':
        $this->hppPostDataAlter($data, $context);
        break;

      case 'hpp_payment_form':
        $this->hppPaymentFormAlter($data, $context);
        break;

      case 'hpp_relay_response_state':
        $this->hppRelayResponseStateAlter($data, $context);
        break;
    }
  }
  
  /**
   * Validate a web service request for card on file
   */
  protected function wsRequestValidate($request_state) {
    $card = $request_state['card'];
    
    // Validate card and payment method match if explicitly passed a payment method
    if (!empty($card->instance_id) && !empty($request_state['payment_method']['instance_id'])) {
      if ($card->instance_id != $request_state['payment_method']['instance_id']) {
        return t('Credit card does not match the payment method provided.');
      }
    }

    // Transarmor requests
    $is_transarmor = !empty($card->remote_id) && !empty($card->card_type);
    if ($is_transarmor) {
      // Card type is required for TransArmor transactions
      if (!$this->controller->convertCardType($card->card_type)) {
        return t('Missing credit card type for the TransArmor request.');
      }
    }
  }

  /**
   * Validate a web service request for card on file
   */
  protected function wsRequestBuildAlter(&$params, &$request_state) {
    $card = $request_state['card'];
    
    // Add build info with request indicators
    $request_state['build_info']['transarmor'] = !empty($card->remote_id) && !empty($card->card_type);

    // TransArmor request
    if ($request_state['build_info']['transarmor']) {
      $params['transarmor_token'] = (string) $card->remote_id;
      $params['cc_number'] = '';
      $params['credit_card_type'] = $this->controller->convertCardType($card->card_type);

      unset($params['cc_verification_str1'], $params['cc_verification_str2']);
      $params['cvd_presence_ind'] = "0";
    }
  }

  /**
   * Implements hpp_post_data alter
   * - Add card id to post data
   */
  protected function hppPostDataAlter(&$data, &$request_state) {
    if (!empty($request_state['card']->card_id)) {
      $data['commerce_card_id'] = $request_state['card']->card_id;
    }
    else {
      $data['commerce_card_id'] = 'new';
    }
  }

  /**
   * Implements hpp_payment_form alter
   * - Add options for storage and default card
   */
  protected function hppPaymentFormAlter(&$form, &$request_state) {
    $storage = variable_get('commerce_cardonfile_storage', 'opt-in');
    if (in_array($storage, array('opt-in', 'opt-out'))) {
      $form['commerce_cardonfile_store'] = array(
        '#type' => 'checkbox',
        '#title' => t('Store this credit card on file for future use.'),
        '#default_value' => $storage == 'opt-out',
      );
    }
    else {
      $form['commerce_cardonfile_store'] = array(
        '#type' => 'hidden',
        '#value' => TRUE,
      );
    }

    $default_cards = array();
    if (!empty($request_state['customer']->uid) && !empty($this->controller->payment_instance['instance_id'])) {
      $default_cards = commerce_cardonfile_load_user_default_cards($request_state['customer']->uid, $this->controller->payment_instance['instance_id']);
    }

    $form['commerce_cardonfile_instance_default'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set as your default card'),
      '#default_value' => empty($default_cards),
      '#states' => array(
        'visible' => array(
          ':input[name*="commerce_cardonfile_store"]' => array('checked' => TRUE),
        ),
        'invisible' => array(
          ':input[name*="commerce_cardonfile_store"]' => array('value' => 0),
        ),
      ),
    );

    // Stop auto redirect to allow customer input before redirect
    unset($form['checkout_offsite_autoredirect']);
  }

  /**
   * Build HPP relay response state
   */
  protected function hppRelayResponseStateAlter(&$state, &$context) {
    $response = $context['response'];

    // Load existing card
    if (!empty($response['commerce_card_id']) && $response['commerce_card_id'] != 'new') {
      $state['card'] = commerce_cardonfile_load($response['commerce_card_id']);
    }
  }

  /**
   * Extract card values form the response
   */
  protected function createResponseCardValues($response, $state = NULL) {
    $values = array(
      'card_id' => NULL,
      'uid' => 0,
      'status' => 1,
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'payment_method' => $this->controller->payment_instance['method_id'],
      'instance_id' => $this->controller->payment_instance['instance_id'],
      'remote_id' => !empty($response['transarmor_token']) ? $response['transarmor_token'] : '',
      'card_name' => !empty($response['cardholder_name']) ? $response['cardholder_name'] : '',
      'card_number' => !empty($response['cc_number']) ? substr($response['cc_number'], -4) : '',
      'card_type' => '',
      'card_exp_month' => '',
      'card_exp_year' => '',
      'no_store' => TRUE,
    );

    if (!empty($response['credit_card_type'])) {
      $processed_card_type = $this->controller->reverseCardType($response['credit_card_type']);
      if (!empty($processed_card_type)) {
        $values['card_type'] = $processed_card_type;
      }
      else {
        $values['card_type'] = drupal_strtolower($response['credit_card_type']);
      }
    }
    
    if (!empty($response['cc_expiry'])) {
      $values['card_exp_month'] = (int) substr($response['cc_expiry'], 0, 2);
      // Note: in 2100 this will not work
      $values['card_exp_year'] = intval('20' . substr($response['cc_expiry'], -2));
    }

    if (!empty($state['card']->uid)) {
      $values['uid'] = $state['card']->uid;
    }
    elseif (!empty($state['customer']->uid)) {
      $values['uid'] = $state['customer']->uid;
    }
    elseif (!empty($state['order']->uid)) {
      $values['uid'] = $state['order']->uid;
    }

    if (isset($response['commerce_cardonfile_instance_default'])) {
      $values['instance_default'] = !empty($response['commerce_cardonfile_instance_default']);
    }
    elseif (isset($state['card']->instance_default)) {
      $values['instance_default'] = $state['card']->instance_default;
    }

    if (isset($response['commerce_cardonfile_store'])) {
      $values['no_store'] = empty($response['commerce_cardonfile_store']);
    }
    elseif (isset($state['card']->no_store)) {
      $values['no_store'] = $state['card']->no_store;
    }

    return $values;
  }

  /**
   * Create or update a card with a web service response
   */
  protected function processCardResponse(&$context) {
    // Exit if not enough info
    if (empty($context['response']) || empty($context['state'])) {
      return;
    }

    // Exit if request failed
    if (empty($context['response']['transaction_approved'])) {
      return;
    }

    $response = $context['response'];
    $state = &$context['state'];

    // Exit if no transarmor response
    if (empty($response['transarmor_token'])) {
      return;
    }

    // Extract card values from the response
    $new_values = $this->createResponseCardValues($response, $state);
    
    // Reference card and create new if needed
    $card = &$state['card'];
    if (empty($card)) {
      // New cards
      $card = commerce_cardonfile_new($new_values);
      $new_values = array();
    }
    elseif (!empty($card->card_id)) {
      // Existing card - only care if remote id changed or type was determined
      $new_values = array_intersect_key($new_values, array('remote_id' => 1, 'card_type' => 1));
    }

    // Exit if set not to store
    if (!empty($card->no_store)) {
      return;
    }

    // Determine new billing profile
    $new_billing_profile = NULL;
    $existing_billing_profile = NULL;
    if (!empty($card->commerce_cardonfile_profile[LANGUAGE_NONE][0]['profile_id'])) {
      $existing_billing_profile = commerce_customer_profile_load($card->commerce_cardonfile_profile[LANGUAGE_NONE][0]['profile_id']);
    }

    if (!empty($state['billing_address'])) {
      $create_billing = TRUE;
      if (!empty($existing_billing_profile)) {
        $existing_billing_profile_wrapper = entity_metadata_wrapper('commerce_customer_profile', $existing_billing_profile);

        if ($this->controller->addressIsEqual($state['billing_address'], $existing_billing_profile_wrapper->commerce_customer_address->value())) {
          $create_billing = FALSE;
        }
      }
      if ($create_billing) {
        if (!empty($state['billing_profile_id'])) {
          $resolved_billing_profile = commerce_customer_profile_load($state['billing_profile_id']);
          $resolved_billing_profile_wrapper = entity_metadata_wrapper('commerce_customer_profile', $resolved_billing_profile);

          if ($this->controller->addressIsEqual($state['billing_address'], $resolved_billing_profile_wrapper->commerce_customer_address->value())) {
            $new_billing_profile = $resolved_billing_profile;
          }
        }

        if (!$new_billing_profile) {
          $new_billing_profile = commerce_customer_profile_new('billing', $card->uid);
          $new_billing_profile_wrapper = entity_metadata_wrapper('commerce_customer_profile', $new_billing_profile);
          $new_billing_profile_wrapper->commerce_customer_address = $state['billing_address'] + addressfield_default_values();
        }
      }
    }

 
    // If new card ...
    if (empty($card->card_id)) {
      // Check required properties
      if (empty($card->uid) || empty($card->payment_method) || empty($card->instance_id)) {
        /** @todo: ??? **/
        return;
      }
    }

    // Create / Update the card
    $this->saveCard($card, $new_billing_profile, $new_values);
  }

  /**
   * Save a card on file object
   */
  public function saveCard($card, $billing_profile = NULL, $new_values = array()) {
    // Remove metadata values
    if (!empty($new_values)) {
      unset($new_values['no_store']);
    }

    // If new card ...
    if (empty($card->card_id)) {
      if (!empty($new_values)) {
        foreach ($new_values as $prop => $new_value) {
          $card->{$prop} = $new_value;
        }
      }

      // Ensure number is always sanitized
      $card->card_number = !empty($card->card_number) ? substr($card->card_number, -4) : 'XXXX';

      // Save the new card
      return commerce_cardonfile_save($card, $billing_profile);
    }

    // Update to an existing card
    if (empty($new_values)) {
      return 3;
    }

    // Load original card
    $card_original = entity_load_unchanged('commerce_cardonfile', $card->card_id);

    // Detect property changes
    $changes = array();
    foreach ($new_values as $prop => $new_value) {
      if ($card_original->{$prop} != $new_value) {
        $card->{$prop} = $new_value;
        $changes[] = $prop;
      }
    }

    // Detect if billing address changed
    $billing_updated = FALSE;
    if (!empty($billing_profile) && !empty($card->commerce_cardonfile_profile[LANGUAGE_NONE][0]['profile_id'])) {
      if (empty($billing_profile->profile_id) || $billing_profile->profile_id != $card->commerce_cardonfile_profile[LANGUAGE_NONE][0]['profile_id']) {
        $billing_updated = TRUE;
      }
    }

    // No changes
    if (empty($changes) && !$billing_updated) {
      return 3;
    }

    // Create new card if immutable property changed or billing updated
    if (array_intersect($changes, $this->cardImmutableProperties()) || $billing_updated) {
      // Disable the original card
      $card_original->status = 0;
      $card_original->changed = REQUEST_TIME;
      commerce_cardonfile_save($card_original);

      // Save the new card
      unset($card->card_id);
      $card->created = REQUEST_TIME;
      $card->changed = REQUEST_TIME;
    }

    // Ensure number is always sanitized
    $card->card_number = !empty($card->card_number) ? substr($card->card_number, -4) : 'XXXX';

    // Save the updated card
    if ($billing_updated) {
      return commerce_cardonfile_save($card, $billing_profile);
    }

    return commerce_cardonfile_save($card);
  }
}
