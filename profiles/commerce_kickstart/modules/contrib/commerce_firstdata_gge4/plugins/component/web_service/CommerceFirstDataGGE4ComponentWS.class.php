<?php

/**
 * @file
 * First Data Global Gateway e4 API Web Services Component.
 */

/**
 * Web Service plugin class
 */
class CommerceFirstDataGGE4ComponentWS extends CommerceFirstDataGGE4ComponentBase {
  const LIVE_URL = 'https://api.globalgatewaye4.firstdata.com/transaction/v12';
  const TEST_URL = 'https://api.demo.globalgatewaye4.firstdata.com/transaction/v12';

  /**
   * Default settings
   */
  public function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings += array(
      'gateway_id' => '',
      'gateway_password' => '',
      'hmac_key_id' => '',
      'hmac_key' => '',
    );

    return $settings;
  }

  /**
   * Settings form
   */
  public function settingsForm() {
    $module_path = drupal_get_path('module', 'commerce_firstdata_gge4');
    $settings = $this->getSettings();
    $form = parent::settingsForm();
    
    $form['help'] = array(
      '#type' => 'item',
      '#markup' => t('Once you log in to your First Data account, navigate to the Administration &rarr; Terminals area, then click on the desired terminal.') . '<br />' .
        t('In your First Data terminal settings, navigate to the "Details" tab.') . '<br />' .
        theme('commerce_firstdata_gge4_help_link', array(
          'text' => t('View screenshot'),
          'path' => $module_path . '/images/firstdata-ws-settings-terminal-details.png'
        )),
    );

    $form['gateway_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Gateway ID'),
      '#default_value' => $settings['gateway_id'],
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );
    $form['gateway_password'] = array(
      '#type' => 'textfield', //'password', //
      '#title' => t('Gateway password'),
      '#description' => t('You can set up your password by clicking on "Generate" or change the existing password.'),
      '#default_value' => $settings['gateway_password'],
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );

    $form['api_access_help'] = array(
      '#type' => 'item',
      '#title' => t('Web Service: API Access'),
      '#markup' => t('In your First Data terminal settings, navigate to the "API Access" tab.') . '<br />' .
        theme('commerce_firstdata_gge4_help_link', array(
          'text' => t('View screenshot'),
          'path' => $module_path . '/images/firstdata-ws-settings-terminal-api-access.png'
        )),
    );
    $form['hmac_key_id'] = array(
      '#type' => 'textfield',
      '#title' => t('API Access: Key ID'),
      '#default_value' => $settings['hmac_key_id'],
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );
    $form['hmac_key'] = array(
      '#type' => 'textfield',
      '#title' => t('API Access: HMAC Key'),
      '#default_value' => $settings['hmac_key'],
      '#description' => t('Clicking "Generate New Key" will generate a new key, and an email notice will be sent to all Merchant Administrators of that account alerting them of this change. The key will be displayed in plain text when it is generated but will not be saved until the "Update" button is clicked. Note:  After this step has been taken, it will not be possible to view the key in plain text again so if this value is not stored it will be necessary to generate a new key.'),
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );

    return $form;
  }

  /**
   * Returns the URL to the First Data server determined by transaction mode.
   *
   * @param $txn_mode
   *   Optional. The transaction mode.
   *   If not provided, the txn mode per the settings is used
   *
   * @return
   *   The URL to use to submit Web Service requests.
   */
  public function getServerUrl($txn_mode = NULL) {
    $txn_mode = isset($txn_mode) ? $txn_mode : $this->controller->getSettings('txn_mode');
    
    switch ($txn_mode) {
      case FIRSTDATA_GGE4_TXN_MODE_LIVE:
      case FIRSTDATA_GGE4_TXN_MODE_LIVE_TEST:
        return self::LIVE_URL;
      case FIRSTDATA_GGE4_TXN_MODE_DEVELOPER:
        return self::TEST_URL;
    }
  }

  /**
   * Returns a payment form to be used during checkout or elsewhere
   */
  public function paymentForm($values = NULL) {
    return $this->controller->creditCardForm();
  }
  
  /**
   * Web Service: Validate request
   *
   * @param $request_state
   *   @see CommerceFirstDataGGE4Controller::resolvePaymentState()
   *   Additional properties:
   *   - 'validation_errors': An array of validation errors
   *
   * @return
   *   TRUE if the request can be performed, else FALSE with $request_state
   *   updated with an array of  'validation_errors'
   */
  public function requestValidate(&$request_state) {
    $request_state['validation_errors'] = array();
    $errors = &$request_state['validation_errors'];
    
    if (!$this->isValid()) {
      $errors[] = t('First Data web service is not configured.');
      return FALSE;
    }

    if (empty($request_state) || !is_array($request_state)) {
      $errors[] = t('Empty request.');
      return FALSE;
    }

    // Resolve state
    $this->controller->resolvePaymentState($request_state);

    // Set local vars
    $card = $request_state['card'];
    $charge = $request_state['charge'];

    // Check transaction type
    $txn_type_info = $this->controller->transactionType($request_state['txn_type']);
    if (empty($txn_type_info)) {
      $errors[] = t('Invalid transaction type requested.');
      return FALSE;
    }

    // Charge parameters are required for all txn types
    if (empty($request_state['skip_charge_validation'])) {
      // Check that charge is a valid price array
      if (empty($charge) || !isset($charge['amount']) || !isset($charge['currency_code'])) {
        $errors[] = t('No charge details provided.');
        return FALSE;
      }

      // Zero Dollar Pre-Authorizations
      if ($request_state['txn_type'] != FIRSTDATA_GGE4_CREDIT_PREAUTH_ONLY &&
          commerce_firstdata_gge4_is_zero($charge['amount'])) {

        if (empty($txn_type_info['zero_auth_allowed'])) {
          $requested_txn_type_info = $this->controller->transactionType($request_state['txn_type']);
          $errors[] = t('Zero Dollar Pre-Authorizations cannot be performed on @label transactions.', array(isset($requested_txn_type_info['label']) ? $requested_txn_type_info['label'] : $request_state['txn_type']));
          return FALSE;
        }

        // Switch type to preauth
        $request_state['txn_type'] = FIRSTDATA_GGE4_CREDIT_PREAUTH_ONLY;
        $txn_type_info = $this->controller->transactionType($request_state['txn_type']);
      }
    }
    
    // Set variable for easier checks
    $charge_exists = !empty($charge) && isset($charge['amount']) && isset($charge['currency_code']);

    // Set transaction type
    $txn_type_code = $request_state['txn_type'];

    // Validate per txn type
    if (!empty($txn_type_info['requires_card'])) {
      // Purchase, Pre-auth, Pre-auth only, Refund via card

      // Card data is needed for these transactions
      if (empty($card)) {
        $errors[] = t('Missing credit card data.');
        return FALSE;
      }

      // Only active cards can be processed
      if (isset($card->status) && $card->status != 1) {
        $errors[] = t('Credit card is not active.');
        return FALSE;
      }

      // Expiration date is needed for these transactions
      if (empty($card->card_exp_month) || empty($card->card_exp_year)) {
        $errors[] = t('Missing credit card expiration.');
        return FALSE;
      }

      // Load card functions
      module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');

      // Validate the expiration date.
      if (commerce_payment_validate_credit_card_exp_date($card->card_exp_month, $card->card_exp_year) !== TRUE) {
        $errors[] = t('Credit card has expired.');
        return FALSE;
      }

      // Check if empty card number or remote id
      // - cardonfile plugin validates for request basd on remote_id
      if (empty($card->card_number) && empty($card->remote_id)) {
        $errors[] = t('Missing credit card number.');
        return FALSE;
      }

      // Name on card
      if (empty($card->card_name)) {
        $has_card_name = FALSE;
        if (!empty($request_state['billing_address'])) {
          if (!empty($request_state['billing_address']['name_line'])) {
            $has_card_name = TRUE;
          }
        }
        if (!$has_card_name) {
          $errors[] = t('Missing name for the credit card.');
          return FALSE;
        }
      }
    }
    elseif (!empty($txn_type_info['transaction_operation'])) {
      // Pre-auth capture, Void, Refund
/** @todo: static transactionAccess($txn_type, $transaction, $amount = NULL) ****/
      $prev_transaction = $request_state['previous_transaction'];
      if (empty($prev_transaction)) {
        $errors[] = t('Missing previous transaction.');
        return FALSE;
      }
      if (empty($prev_transaction->data['authorization_num'])) {
        $errors[] = t('Missing previous transaction authorization number.');
        return FALSE;
      }
      if (empty($prev_transaction->data['transaction_tag'])) {
        $errors[] = t('Missing previous transaction tag.');
        return FALSE;
      }

      // Now used for all time calculations
      $now = time();

      // Check if requested transaction type is expired
      if (!empty($txn_type_info['expiration']) && !empty($prev_transaction->created)) {
        if (($now - $prev_transaction->created) > $txn_type_info['expiration']) {
          $errors[] = t('@label is not available after @expiration.', array(
            '@label' => $txn_type_info['label'],
            '@expiration' => format_interval($txn_type_info['expiration']),
          ));
          return FALSE;
        }
      }

      // Check expiration since the last update to the transaction.
      if (!empty($txn_type_info['changed_expiration']) && !empty($prev_transaction->changed)) {
        if (($now - $prev_transaction->changed) > $txn_type_info['changed_expiration']) {
          $errors[] = t('@label is not available after @expiration.', array(
            '@label' => $txn_type_info['label'],
            '@expiration' => format_interval($txn_type_info['changed_expiration']),
          ));
          return FALSE;
        }
      }

      // Check previous transaction with type info
      if (!empty($prev_transaction->remote_status) &&
          ($prev_txn_type_info = $this->controller->transactionType($prev_transaction->remote_status))) {

        // Check allowed_transactions
        if (empty($prev_txn_type_info['allowed_transactions']) || !in_array($txn_type_code, $prev_txn_type_info['allowed_transactions'])) {
          $errors[] = t('@label is not available for this transaction.', array('@label' => $txn_type_info['label']));
          return FALSE;
        }

        // Check the amount is less than or equal to the max transaction amount allowed.
        if (!empty($txn_type_info['max_amount_factor']) && $charge_exists && empty($request_state['skip_charge_validation'])) {
          $txn_max = $this->controller->transactionMaxAmount($txn_type_code, $prev_transaction);
          $txn_max = array(
            'amount' => commerce_currency_convert($txn_max['amount'], $txn_max['currency_code'], $charge['currency_code']),
            'currency_code' => $charge['currency_code'],
          );

          if ($charge['amount'] > $txn_max['amount']) {
            $action_word = !empty($txn_type_info['action_word']['present']) ? $txn_type_info['action_word']['present']: $txn_type_code;
            if ($txn_type_info['max_amount_factor'] > 1) {
              $percent = 100 * ($txn_type_info['max_amount_factor'] - 1);
              $errors[] = t('You cannot @action more than @max_amount, @percent% above the transaction amount.', array(
                '@action' => $action_word,
                '@percent' => number_format($percent, 2),
                '@max_amount' => commerce_currency_format($txn_max['amount'], $prev_transaction->currency_code),
              ));
            }
            elseif ($txn_type_info['max_amount_factor'] < 1) {
              $percent = 100 * (1 - $txn_type_info['max_amount_factor']);
              $errors[] = t('You cannot @action more than @max_amount, @percent% below the transaction amount.', array(
                '@action' => $action_word,
                '@percent' => number_format($percent, 2),
                '@max_amount' => commerce_currency_format($txn_max['amount'], $prev_transaction->currency_code),
              ));
            }
            else {
              $errors[] = t('You cannot @action more than the transaction amount.', array(
                '@action' => $action_word,
              ));
            }
            return FALSE;
          }
        }
      }

      // Requested transaction type specific checks
      switch ($txn_type_code) {
        case COMMERCE_CREDIT_CREDIT:
          // Doesn't have a success status or has an amount of 0 or less.
          if ($prev_transaction->status != COMMERCE_PAYMENT_STATUS_SUCCESS || $prev_transaction->amount <= 0) {
            $errors[] = t('Credit processing is not available for this transaction.');
            return FALSE;
          }
          break;
      }
    }

    // Let other plugins and modules validate
    $event_errors = $this->controller->trigger('ws_request_validate', $request_state);
    if (!empty($event_errors)) {
      $errors = array_merge($errors, $event_errors);
      return FALSE;
    }

    // ALLOW if made it here
    return TRUE;
  }

  /**
   * Web Service: Build JSON request parameters
   *
   * Assumes request has been validated.
   *
   * @param $request_state
   *   @see CommerceFirstDataGGE4Controller::resolvePaymentState()
   *
   * @return
   *   A name-value pair array for a JSON request
   */
  protected function requestBuild(&$request_state) {
    // Resolve state
    $this->controller->resolvePaymentState($request_state);

    // Set local vars for easy reference
    $charge = $request_state['charge'];
    $card = $request_state['card'];
    $order = $request_state['order'];
    $billing_address = $request_state['billing_address'];
    $prev_transaction = $request_state['previous_transaction'];

    // load transaction type info
    $txn_type_info = $this->controller->transactionType($request_state['txn_type']);

    // Add build info with request indicators
    $request_state['build_info'] = array(
      'zero_amount' => commerce_firstdata_gge4_is_zero($charge['amount']),
    );

    // Zero Dollar Pre-Authorizations
    if ($request_state['txn_type'] != FIRSTDATA_GGE4_CREDIT_PREAUTH_ONLY &&
        $request_state['build_info']['zero_amount'] &&
        !empty($txn_type_info['zero_auth_allowed'])) {

      $request_state['txn_type'] = FIRSTDATA_GGE4_CREDIT_PREAUTH_ONLY;
      $txn_type_info = $this->controller->transactionType($request_state['txn_type']);
    }

    // Convert Commerce txn type to gateway code
    $request_state['gateway_txn_type'] = $txn_type_info['gateway_code'];

    // Set transaction type
    $txn_type_code = $request_state['txn_type'];

    // Initialize request parameters
    $params = array(
      'transaction_type' => $request_state['gateway_txn_type'],
    );

    // Determine charge context
    $params += array(
      'amount' => !$request_state['build_info']['zero_amount'] ? commerce_currency_amount_to_decimal($charge['amount'], $charge['currency_code']) : 0,
      'currency_code' => $charge['currency_code'],
    );

    // Parameters required per txn type
    if (!empty($txn_type_info['requires_card'])) {
      // Purchase, Pre-auth, Pre-auth only, Refund via credit card

      // Billing address parameters
      if (!empty($billing_address)) {
        $params['zip_code'] = substr($billing_address['postal_code'], 0, 10);

        // cc_verification_str1: "Street Address|Zip/Postal|City|State/Prov|Country"
        $billing_address_verify_parts = array(
          $billing_address['street_line'],
          $billing_address['postal_code'],
          $billing_address['locality'],
          $billing_address['administrative_area'],
          $billing_address['country'],
        );
        $params['cc_verification_str1'] = implode('|', $billing_address_verify_parts);
        $params['cc_verification_str1'] = substr($params['cc_verification_str1'], 0, 41);
      }

      // Add expiration
      $params += array(
        'cc_expiry' => str_pad($card->card_exp_month, 2, '0', STR_PAD_LEFT) . substr($card->card_exp_year, -2),
      );

      // Add cardholder name
      $cardholder_name = '';
      if (!empty($card->card_name)) {
        // Set to name on card
        $cardholder_name = $card->card_name;
      }
      elseif (!empty($billing_address['name_line'])) {
        // Set to billing address name
        $cardholder_name = $billing_address['name_line'];
      }
      $card->card_name = $params['cardholder_name'] = substr($cardholder_name, 0, 30);

      // Add additional card data
      $params += array(
        'cc_number' => substr($card->card_number, 0, 16),
      );

      // CVV code should only be available during checkout or new cards
      if (!empty($card->card_code)) {
        $params['cc_verification_str2'] = substr($card->card_code, 0, 4);
        $params['cvd_presence_ind'] = "1";
      }
    }
    elseif (!empty($txn_type_info['transaction_operation'])) {
      // Pre-auth capture, Void, Refund
      $params['authorization_num'] = substr($prev_transaction->data['authorization_num'], 0, 8);
      $params['transaction_tag'] = (int) $prev_transaction->data['transaction_tag'];
    }

    
    // Add order information
    if (!empty($order->order_number)) {
      $params['reference_no'] = $order->order_number;
    }

    // @todo: Level 2 order info - tax, etc

    // @todo: Level 3 order info - line items, etc


    // Add customer params
    if (isset($request_state['customer']->uid)) {
      $params['customer_ref'] = substr($request_state['customer']->uid, 0, 20);
    }
    if (isset($request_state['customer']->mail)) {
      $params['client_email'] = substr($request_state['customer']->mail, 0, 255);
    }
    
    $params['client_ip'] = substr(ip_address(), 0, 15);


    // Common parameters
/** @todo use site or get from owner - order or card **/
    $params['language'] = $this->controller->convertLanguage(language_default('language'));

    // Allow other plugins and modules to alter
    $this->controller->alter('ws_request_build', $params, $request_state);
    
    return $params;
  }

  /**
   * Performs a Web Service JSON request
   *
   * @param $request_state
   *   @see CommerceFirstDataGGE4Controller::resolvePaymentState()
   *
   * @return
   *   An array of response parameters
   */
  public function request(&$request_state) {
    $empty_response = array(
      'transaction_approved' => 0,
      'bank_message' => '',
    );

    // Exit if request is not valid
    if (!$this->requestValidate($request_state)) {
      // merge errors into response
      return $empty_response + array('validation_errors' => $request_state['validation_errors']);
    }

    // Build the request
    $params = $this->requestBuild($request_state);

    // Exit if empty request
    if (empty($params)) {
      return $empty_response;
    }

    // Add some request params to empty response
    $empty_response['transaction_type'] = $params['transaction_type'];

    // Get settings
    $settings = $this->getSettings();
    $log_settings = $this->controller->getSettings('log');

    // Add API credentials
    $params += array(
      'gateway_id' => $settings['gateway_id'],
      'password' => $settings['gateway_password'],
    );

/** @todo: live test mode parameter for WS?**/
    // if ($this->controller->getSettings('txn_mode') == FIRSTDATA_GGE4_TXN_MODE_LIVE_TEST)


    // Get request url
    $request_url = $this->getServerUrl();

    // Log the request if specified.
    if ($log_settings['request'] == 'request') {
      $this->controller->log('First Data GGe4 web service request', $params);
    }

    // Prepare the JSON string.
    ksort($params);
    $request_content = drupal_json_encode($params);
    $request_content_type = 'application/json';

    // Add content headers
    $request_headers = array(
      'Content-Type' => $request_content_type . '; charset=UTF-8',
      'Accept' => $request_content_type,
      'Content-Length' => strlen($request_content),
    );

    // Add security headers
    $request_headers += $this->generateSecurityRequestHeaders($request_headers['Content-Type'], $request_content, 'POST');

    // Combine header keys and values
    foreach ($request_headers as $header_key => &$header_value) {
      $header_value = $header_key . ': ' . $header_value;
    }

    // Setup the cURL request.
    $curl_options = array(
      CURLOPT_URL => $request_url,
      CURLOPT_VERBOSE => 0,
      CURLOPT_POST => 1,
      CURLOPT_POSTFIELDS => $request_content,
      CURLOPT_RETURNTRANSFER => 1,
      CURLOPT_SSL_VERIFYPEER => 0,
      CURLOPT_NOPROGRESS => 1,
      CURLOPT_FOLLOWLOCATION => 0,
      CURLOPT_FRESH_CONNECT => 1,
      CURLOPT_FORBID_REUSE => 1,
      CURLOPT_TIMEOUT => 60,
      CURLOPT_HTTPHEADER => array_values($request_headers),
    );

    $ch = curl_init();
    curl_setopt_array($ch, $curl_options);
    $raw_response = curl_exec($ch);

    // Check for cURL errors.
    $errorno = NULL;
    if ($errorno = curl_errno($ch)) {
      watchdog('commerce_firstdata_gge4', 'Error with cURL request: (@error_no). Message: @error_message', array(
          '@error_no' => $errorno,
          '@error_message' => curl_error($ch),
        ),
        WATCHDOG_ERROR
      );
      curl_close($ch);
      return $empty_response;
    }

    curl_close($ch);

    // @todo: Response Hash check
    // First Data calculates security hash using the response body.
    // This would require CURLOPT_HEADER => 1 and parsing the response into a
    // header and body, then extracting the headers for date, etc to calcualte.

    // Process response
    $response = drupal_json_decode($raw_response);

    // Handle empty / non-json responses
    if (empty($response)) {
      if (is_string($raw_response) && strlen($raw_response) > 0) {
        $error_message = $raw_response;
      }
      else {
        $error_message = t('Empty response from the gateway.');
      }

      // Add custom error property
      $response = array(
        'gge4_fatal_error' => $error_message,
        'bank_message' => $error_message,
        'transaction_approved' => 0,
      ) + $empty_response;
      watchdog('commerce_firstdata_gge4', 'Request error: @error_message', array('@error_message' => $error_message), WATCHDOG_ERROR);
    }

    // Sort alphabetically
    ksort($response);

    // Create / Update Commerce payment transaction
    $transaction = $this->controller->saveTransaction($response, $request_state);
    if ($transaction) {
      $request_state['transaction'] = $transaction;
    }

    // Allow other plugins and modules to react to non-fatal responses
    if (empty($response['gge4_fatal_error'])) {
      $event_context = array(
        'plugin' => $this->plugin['name'],
        'response' => $response,
        'state' => $request_state,
      );
      $this->controller->trigger('response_process', $event_context);
    }

    // Log the response if specified.
    if ($log_settings['response'] == 'response') {
      $this->controller->log('First Data GGe4 web service response', $response);
    }

    return $this->controller->sanitizeParameters($response);
  }

  /**
   * Build an array of common error messages.
   *
   * @return
   *   An array of unsanitized error messages keyed by the error type.
   */
  function getErrorMessages($response = NULL, $request_state = array()) {
    $messages = array();
    if (isset($response)) {
      if (empty($response['transaction_approved'])) {
        if (!empty($response['bank_message'])) {
          $messages['bank_message'] = $response['bank_message'];
        }

        if (!empty($response['avs']) && $response['avs'] != 'M') {
          $messages['avs'] = t('AVS response:') . ' ' . $this->controller->avsMessage($response['avs']);
        }

        // Add the CVV response if enabled.
        if (!empty($response['cvv2'])) {
          $messages['cvv2'] = t('CVV match:') . ' ' . $this->controller->cvvMessage($response['cvv2']);
        }
      }
    }

    $validation_errors = array();
    if (!empty($response['validation_errors'])) {
      $validation_errors = $response['validation_errors'];
    }
    elseif (!empty($request_state['validation_errors'])) {
      $validation_errors = $request_state['validation_errors'];
    }

    if (!empty($validation_errors)) {
      $i = 0;
      foreach ($validation_errors as $validation_error) {
        $messages['validation_' .  $i] = $validation_error;
        $i++;
      }
    }

    return $messages;
  }

  /**
   * Generate security headers
   *
   * @see https://firstdata.zendesk.com/entries/22069302-api-security-hmac-hash
   */
  public function generateSecurityRequestHeaders($content_type, $content, $request_method = 'POST') {
    $settings = $this->getSettings();
    if (empty($request_method)) {
      $request_method = 'POST';
    }
    else {
      $request_method = strtoupper($request_method);
    }

    // Extract request path
    $request_url = $this->getServerUrl();
    $request_url_path = parse_url($request_url, PHP_URL_PATH);

    // Calculate digest
    $content_digest = sha1($content);

    // Calculate ISO 8601 date - custom since gmdate('c') adds +00:00 instead of Z
    $hash_date = gmdate('Y-m-d\TH:i:s') . 'Z';

    // Calculate Authentication HMAC
    $hash_data_parts = array($request_method, $content_type, $content_digest, $hash_date, $request_url_path);
    $hash_data = implode("\n", $hash_data_parts);
    $hmac = base64_encode(hash_hmac('sha1', $hash_data, $settings['hmac_key'], TRUE));

    return array(
      'X-GGe4-Content-SHA1' => $content_digest,
      'X-GGe4-Date' =>  $hash_date,
      'Authorization' => 'GGE4_API ' . $settings['hmac_key_id'] . ':' . $hmac,
    );
  }
}
