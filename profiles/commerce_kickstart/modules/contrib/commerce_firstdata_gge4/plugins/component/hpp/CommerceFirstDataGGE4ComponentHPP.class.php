<?php

/**
 * @file
 * First Data Global Gateway e4 API Hosted Payment Pages Component.
 */

/**
 * Hosted Payment Pages plugin class
 */
class CommerceFirstDataGGE4ComponentHPP extends CommerceFirstDataGGE4ComponentBase {
  const LIVE_URL = 'https://checkout.globalgatewaye4.firstdata.com/payment';
  const TEST_URL = 'https://demo.globalgatewaye4.firstdata.com/payment';
  const LOCAL_RELAY_PATH = 'commerce_firstdata_gge4/hosted-relay';
  const LOCAL_AUTO_POST_PATH = 'commerce_firstdata_gge4/hosted-auto-post';

  // -----------------------------------------------------------------------
  // Class methods

  /**
   * Map to translate HPP response to a JSON web service reponse
   */
  public static function webServiceKeyMap() {
    return array(
      'AVS' => 'avs',
      'Authorization_Num' => 'authorization_num',
      'Bank_Message' => 'bank_message',
      'Bank_Resp_Code' => 'bank_resp_code',
      'Bank_Resp_Code_2' => 'bank_resp_code_2',
      'CAVV_Algorithm' => 'cavv_algorithm',
      'CAVV_Response' => 'cavv_response',
      'CVD_Presence_Ind' => 'cvd_presence_ind',
      'CVV2' => 'cvv2',
      'CardHoldersName' => 'cardholder_name',
      'Card_Number' => 'cc_number',
      'Client_Email' => 'client_email',
      'Client_IP' => 'client_ip',
      'Customer_Ref' => 'customer_ref',
      'DollarAmount' => 'amount',
      'EXact_Message' => 'exact_message',
      'EXact_Resp_Code' => 'exact_resp_code',
      'Ecommerce_Flag' => 'ecommerce_flag',
      'Expiry_Date' => 'cc_expiry',
      'Language' => 'language',
      'MerchantAddress' => 'merchant_address',
      'MerchantCity' => 'merchant_city',
      'MerchantCountry' => 'merchant_country',
      'MerchantName' => 'merchant_name',
      'MerchantPostal' => 'merchant_postal',
      'MerchantProvince' => 'merchant_province',
      'Reference_3' => 'reference_3',
      'Reference_No' => 'reference_no',
      'Retrieval_Ref_No' => 'retrieval_ref_no',
      'Secure_AuthRequired' => 'secure_auth_required',
      'Secure_AuthResult' => 'secure_auth_result',
      'SequenceNo' => 'sequence_no',
      'SurchargeAmount' => 'surcharge_amount',
      'Tax1Amount' => 'tax1_amount',
      'Tax1Number' => 'tax1_number',
      'Tax2Amount' => 'tax2_amount',
      'Tax2Number' => 'tax2_number',
      'TransactionCardType' => 'credit_card_type',
      'Transaction_Approved' => 'transaction_approved',
      'Transaction_Error' => 'transaction_error',
      'Transaction_Tag' => 'transaction_tag',
      'Transaction_Type' => 'transaction_type',
      'XID' => 'xid',
      'ZipCode' => 'zip_code',
      'exact_ctr' => 'ctr',
    );
  }

  /**
   * Map to translate HPP response to a JSON web service reponse
   */
  public static function webServiceXKeyMap() {
    return array(
      'x_currency_code' => 'currency_code',
    );
  }

  /**
   * Convert an HPP card type to a web service card type
   */
  public static function webServiceCardType($hosted_type) {
    $map = array(
      'VISA' => 'Visa',
      'MASTERCARD' => 'Mastercard',
      'AMERICAN EXPRESS' => 'American Express',
      'DINERS CLUB' => 'Diners Club',
      'JCB' => 'JCB',
      'DISCOVER' => 'Discover',
    );

    return isset($map[$hosted_type]) ? $map[$hosted_type] : $hosted_type;
  }
  
  /**
   * Convert response to web service compatible JSON keys and expected values
   */
  public static function convertResponseToWebService($response) {
    $map = self::webServiceKeyMap();
    $converted = $response;

    // Convert keys
    foreach ($map as $key => $new_key) {
      if (isset($response[$key])) {
        unset($converted[$key]);
        $converted[$new_key] = $response[$key];
      }
    }

    // Process fields to common values

    // Transaction approved is returned as 'Yes', 'No', x_response_code is more reliable
    if (isset($converted['x_response_code'])) {
      $converted['transaction_approved'] = $converted['x_response_code'] ==  '1' ? 1 : 0;
    }
    elseif (isset($converted['transaction_approved'])) {
      $converted['transaction_approved'] = strtolower($converted['transaction_approved']) ==  'yes' ? 1 : 0;
    }

    // Card type
    if (isset($converted['credit_card_type'])) {
      $converted['credit_card_type'] = self::webServiceCardType($converted['credit_card_type']);
    }

    // Card number:
    // if transarmor is enabled, it is set to the transarmor token,
    // else it is the mask number.
    // However, there's no way to determine if transarmor is enabled or not
    // on the gateway, so always mapping for backwards compatibility if
    // card on file plugin is enabled later.
    $converted['transarmor_token'] = !empty($converted['cc_number']) ? $converted['cc_number'] : '';

    // Copy x params to translated key to preserve x_* params
    $x_map = self::webServiceXKeyMap();
    foreach ($x_map as $x_key => $new_key) {
      $converted[$new_key] = $converted[$x_key];
    }

    // Sort alphabetically
    ksort($response);

    return $converted;
  }

  /**
   * Group x_* parameters
   * @todo: the thinking was to group and then compress to save storage space
   */
  public static function groupResponseXParams($data) {
    // If data does NOT have x params
    if (!isset($data['x_response_code'])) {
      return $data;
    }

    $processed = $data;
    $x_data = array();
    foreach ($data as $k => $value) {
      if (strpos($k, 'x_') === 0) {
        $x_data[$k] = $value;
        unset($processed[$k]);
      }
    }

    $processed['x'] = $x_data;

    return $processed;
  }


  // -----------------------------------------------------------------------
  // Instance

  /**
   * Returns TRUE if the plugin is enabled
   */
  public function isValid() {
    $settings = $this->getSettings();
    return !empty($settings['page_id']) &&
           !empty($settings['hmac_encryption_type']) &&
           !empty($settings['transaction_key']) &&
           !empty($settings['response_key']);
  }
  
  /**
   * Default settings
   */
  public function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings += array(
      'page_id' => '',
      'hmac_encryption_type' => 'md5',
      'transaction_key' => '',
      'response_key' => '',
      'show_payment_instructions' => FALSE,
      'checkout_offsite_autoredirect' => FALSE,
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
      '#markup' => t('Once you log in to your First Data account, navigate to the "Payment Pages" tab in GGe4 Real-time Payment Manager.'),
    );

    $form['page_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Page ID'),
      '#default_value' => $settings['page_id'],
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );
    $form['help_security'] = array(
      '#type' => 'item',
      '#title' => t('Hosted Payment Page Security'),
      '#markup' => t('In your Hosted payment page settings, navigate to the "Security" page.') . '<br />' .
        theme('commerce_firstdata_gge4_help_link', array(
          'text' => t('View screenshot'),
          'path' => $module_path . '/images/firstdata-hpp-settings-security.png'
        )),
    );
    $form['hmac_encryption_type'] = array(
      '#type' => 'select',
      '#title' => t('HMAC Encryption Type'),
      '#default_value' => $settings['hmac_encryption_type'],
      '#options' => array(
        'md5' => t('MD5 (most common)'),
        'sha1' => t('SHA-1'),
      ),
      '#description' => t('The default encryption algorithm is MD5. SHA-1 is only advised for custom requirements.'),
      '#required' => TRUE,
    );
    $form['transaction_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Transaction Key'),
      '#default_value' => $settings['transaction_key'],
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );
    $form['response_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Response Key'),
      '#default_value' => $settings['response_key'],
      '#required' => TRUE,
      '#attributes' => array('autocomplete' => 'off'),
    );
    $form['show_payment_instructions'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show a message on the checkout form when this payment method is selected telling the customer to "Continue with checkout to complete payment via First Data."'),
      '#default_value' => $settings['show_payment_instructions'],
    );
    $form['checkout_offsite_autoredirect'] = array(
      '#type' => 'checkbox',
      '#title' => t('Automatically redirect the customer to the offsite payment site on the payment step of checkout.'),
      '#description' => t('If card on file is enabled then the automatic redirect will be disabled if the customer is presented the option to opt-in or opt-out of card storage.'),
      '#default_value' => $settings['checkout_offsite_autoredirect'],
    );
    
    $relay_url = str_replace(array('http://', 'https://'), '', $this->getRelayURL());

    $configs = array(
      array(
        'data' => t('Receipt Page &rarr; Authorize.Net Protocol - Relay Response Settings') . ' (' .
            theme('commerce_firstdata_gge4_help_link', array(
              'text' => t('View screenshot'),
              'path' => $module_path . '/images/firstdata-hpp-settings-relay-response-enabled.png'
            )) . ')',
        'children' => array(
          t('Allow Relay Response') . ': <span class="commerce-firstdata-gge4-config-value">' . t('Enabled') . '</span>',
          t('Relay Response URL') . ': ' . t('Enter the url below that matches the protocol that is used during checkout.') .
          '<ul>' .
            '<li>' . t('If http: ') . '<span class="commerce-firstdata-gge4-config-value">' . "http://{$relay_url}" . '</span></li>' .
            '<li>' . t('If https: ') . '<span class="commerce-firstdata-gge4-config-value">' . "https://{$relay_url}" . '</span></li>' .
          '</ul>',
        ),
      ),
    );

    $form['config_help'] = array(
      '#type' => 'item',
      '#title' => t('Configuration needed for your Payment Page Settings in GGe4 Real-time Payment Manager :'),
      '#prefix' => '<div class="commerce-firstdata-gge4-config-help">',
      '#markup' => theme('item_list', array('items' => $configs)),
      '#suffix' => '</div>',
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
   * Returns TRUE if test mode is enabled
   */
  public function isTestMode($txn_mode = NULL) {
    $txn_mode = isset($txn_mode) ? $txn_mode : $this->controller->getSettings('txn_mode');

    switch ($txn_mode) {
      case FIRSTDATA_GGE4_TXN_MODE_LIVE:
        return FALSE;
    }

    // Default to test mode
    return TRUE;
  }

  /**
   * Returns an absolute url for the relay response
   */
  public function getRelayURL() {
    return url(self::LOCAL_RELAY_PATH, array('absolute' => TRUE));
  }

  /**
   * Returns an absolute url for the return POST response
   */
  public function getAutoPostURL() {
    return url(self::LOCAL_AUTO_POST_PATH, array('absolute' => TRUE));
  }

  /**
   * Event handler
   * - Only called if this plugin is enabled and valid
   */
  public function on($event_name, &$context) {
    switch ($event_name) {
      case 'response_process':
        if ($context['plugin'] == $this->plugin['name']) {
          $this->relayResponseProcessOrder($context);
        }
        break;
    }
  }

  /**
   * Convert an HPP card type to a web service card type
   *
   * @param $response
   *   The HPP web service equivalent response
   */
  public function createResponseBillingAddress($response) {
    // Response param => Address field property
    $map = array(
      'x_first_name' => 'first_name',
      'x_last_name' => 'last_name',
      'x_company' => 'organisation_name',
      'x_address' => 'street_line',
      'x_city' => 'locality',
      'x_state' => 'administrative_area',
      'x_zip' => 'postal_code',
      'x_country' => 'country',
    );

    $address = array();
    foreach ($map as $x => $prop) {
      if (isset($response[$x])) {
        $address[$prop] = $response[$x];
      }
    }

    // Combine first and last into name_line
    $names = array();
    if (!empty($address['first_name'])) {
      $names[] = $address['first_name'];
    }
    if (!empty($address['last_name'])) {
      $names[] = $address['last_name'];
    }
    if (!empty($names)) {
      $address['name_line'] = implode(' ', $names);
    }

    // Convert country to a code
    if (!empty($address['country'])) {
      $country_code = $this->controller->getCountryCode($address['country']);
      $address['country'] = $country_code ? $country_code : '';
    }
    else {
      $address['country'] = '';
    }

    // Convert state to a code
    if (!empty($address['administrative_area'])) {
      $state_code = $this->controller->getStateCode($address['administrative_area'], $address['country']);
      $address['administrative_area'] = $state_code ? $state_code : '';
    }

    return $address + addressfield_default_values();
  }
  
  /**
   * Returns a payment form to be used during checkout or elsewhere
   */
  public function paymentForm($form, &$form_state, &$request_state = array()) {
    if (!$this->isValid()) {
      return $form;
    }

    // Resolve state
    $this->controller->resolvePaymentState($request_state);

    // Get plugin settings
    $settings = $this->getSettings();

    // Set transaction type based on settings.
    $txn_type = $this->controller->getSettings('txn_type');
    $x_type = 'AUTH_CAPTURE';
    if ($txn_type == COMMERCE_CREDIT_AUTH_ONLY) {
      $x_type = 'AUTH_ONLY';
    }

    // Initialize variables
    $order = $request_state['order'];
    $description = array();
    $card = $request_state['card'];
    $charge = $request_state['charge'];
    $cancel_path = '';

    // Order data
    if (!empty($order)) {
      $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
      $cancel_path = 'checkout/' . $order->order_id . '/payment/back/' . $order->data['payment_redirect_key'];

      // Build a description for the order.
      /** @todo: create details for x_line_item instead of x_description which is not used ***/
      foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
        if (in_array($line_item_wrapper->type->value(), commerce_product_line_item_types())) {
          $description[] = round($line_item_wrapper->quantity->value(), 2) . 'x ' . $line_item_wrapper->line_item_label->value();
        }
      }
    }

    // Card data
    if (!empty($card)) {
      if (empty($cancel_path) && !empty($card->uid)) {
        $cancel_path = 'user/' . $card->uid . '/cards';
      }
    }

    // Resolve charge - convert to decimal, fallback to 0
    if (!empty($charge['amount'])) {
      $charge['amount_decimal'] = commerce_currency_amount_to_decimal($charge['amount'], $charge['currency_code']);
    }
    else {
      // Fallback to Zero dollar authorization
      $x_type = 'AUTH_ONLY';
      $charge = array(
        'amount' => 0,
        'amount_decimal' => 0,
        'currency_code' => isset($charge['currency_code']) ? $charge['currency_code'] : commerce_default_currency(),
      );
    }

    // Build submit data
    $data = array(
      // essentials
      'x_login' => $settings['page_id'],
      'x_type' => $x_type,
      'x_amount' => !empty($charge['amount_decimal']) ? number_format($charge['amount_decimal'], 2, '.', '') : '0',
      'x_currency_code' => $charge['currency_code'],
      'x_show_form' => 'PAYMENT_FORM',
      'x_customer_ip' => ip_address(),

      // fallback method if relay response fails
      'x_receipt_link_method' => 'AUTO-POST',
      'x_receipt_link_url' => $this->getAutoPostURL(),

      // relay response settings
      'x_relay_response' => 'TRUE',
      // if url is not set, then the url in the First Data page settings is used
      'x_relay_url' => $this->getRelayURL(),

      /** @todo: not sure if there are used or need for first data **/
      //'x_delim_data' => 'FALSE',
      //'x_cancel_url' => url($cancel_path, array('absolute' => TRUE)),
      //'x_version' => '3.1',
      //'x_method' => 'CC',

      // Payment instance for easier loading on relay
      'commerce_payment_method' => $this->controller->payment_instance['instance_id'],

      // Default is used as defined in the First Data payment page settings
      //'x_email_customer' => empty($settings['customer_notification']) ? 'FALSE' : $settings['customer_notification'],
    );

    // Conditional fields

    // Order info
    if (!empty($order->order_id)) {
      $data += array(
        'commerce_order_id' => $order->order_id,
        'x_invoice_num' => $order->order_number,
        'x_description' => substr(implode(', ', $description), 0, 255),
      );
    }

    // Customer
    if (!empty($request_state['customer']->uid)) {
      $data['x_cust_id'] = substr($request_state['customer']->uid, 0, 20);

      // Set customer_ref similar to web service
      // - x_po_num is passed to customer_ref in response
      $data['x_po_num'] = $data['x_cust_id'];
    }

    if (!empty($request_state['customer']->mail)) {
      $data['x_email'] = substr($request_state['customer']->mail, 0, 255);
    }

    // Billing address
    if (!empty($request_state['billing_address'])) {
      $billing_address = $request_state['billing_address'];
      $data += array(
        // HPP form full name is limited to 20. Allowing both to be more than 20
        // to ensure card holder name is not trunctated. Long names will cause
        // a form error on the HPP. Authnet SIM uses 50.
        'x_first_name' => substr($billing_address['first_name'], 0, 50),
        'x_last_name' => substr($billing_address['last_name'], 0, 50),
        'x_company' => substr($billing_address['organisation_name'], 0, 20),
        'x_address' => substr($billing_address['street_line'], 0, 28),
        'x_city' => substr($billing_address['locality'], 0, 20),
        // State must be submitted as full name ie: Georgia, Maryland, Quebec
        'x_state' => $this->controller->getStateName($billing_address['administrative_area'], $billing_address['country']),
        'x_zip' => substr($billing_address['postal_code'], 0, 9),
        // Country must be submitted as full name ie: United States, China, Canada
        'x_country' => $this->controller->getCountryName($billing_address['country']),
      );
    }


    // Allow other plugins and modules to alter
    $this->controller->alter('hpp_post_data', $data, $request_state);

    // Create the hash fingerprint
    $hmac_encryption_type = !empty($settings['hmac_encryption_type']) ? $settings['hmac_encryption_type'] : 'md5';
    $data['x_fp_timestamp'] = REQUEST_TIME;
    $data['x_fp_sequence'] = mt_rand(1, 1000);

    $hash_seeds = array(
      $data['x_login'],
      $data['x_fp_sequence'],
      $data['x_fp_timestamp'],
      $data['x_amount'],
      $data['x_currency_code'],
    );
    $data['x_fp_hash'] = hash_hmac($hmac_encryption_type, implode('^', $hash_seeds), $settings['transaction_key']);

    // Log "request"
    $log_settings = $this->controller->getSettings('log');
    if ($log_settings['request'] == 'request') {
      $this->controller->log('First Data GGe4 HPP submit data', $data);
    }

    // Set post url and transaction mode
    $submit_url = $this->getServerUrl();
    $data['x_test_request'] = $this->isTestMode() ? 'TRUE' : 'FALSE';

    // Build form elements
    $form['#action'] = $submit_url;
    $form['#method'] = "post";
    foreach ($data as $name => $value) {
      $form[$name] = array(
        '#type' => 'hidden',
        '#value' => $value,
      );
    }

    $form['actions'] = array(
      '#type' => 'actions',
      '#weight' => 50,
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
    );
    $form['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => $cancel_path,
      '#options' => array(
        'absolute' => TRUE,
        'html' => FALSE,
      ),
    );

    // Allow other plugins and modules to alter
    $this->controller->alter('hpp_payment_form', $form, $request_state);
    return $form;
  }

  /**
   * Build a state array for relay response processing
   *
   * @param $response
   *  A web service converted response
   */
  protected function buildRelayResponseState($response) {
    $state = array();

    if (isset($response['amount']) && isset($response['currency_code'])) {
      $state['charge'] = array(
        'amount' => commerce_currency_decimal_to_amount($response['amount'], $response['currency_code']),
        'currency_code' => $response['currency_code'],
      );
    }

    if (!empty($response['commerce_order_id'])) {
      $state['order'] = commerce_order_load($response['commerce_order_id']);
    }

    $customer = array();
    if (!empty($response['customer_ref'])) {
      $customer['uid'] = $response['customer_ref'];
    }
    elseif (!empty($response['x_cust_id'])) {
      $customer['uid'] = $response['x_cust_id'];
    }

    if (!empty($response['client_email'])) {
      $customer['mail'] = $response['client_email'];
    }

    if (!empty($customer)) {
      $state['customer'] = (object) $customer;
    }

    // Add billing address per response parameters
    $state['billing_address'] = $this->createResponseBillingAddress($response);

    // Allow others to alter
    $alter_context = array('response' => $response);
    $this->controller->alter('hpp_relay_response_state', $state, $alter_context);

    // Resolve state
    $this->controller->resolvePaymentState($state);

    return $state;
  }

  /**
   * Process hosted payment page POST response
   */
  public function processResponse($raw_response = NULL) {
    // Set variable for altering
    if (!isset($raw_response)) {
      $raw_response = $_POST;
    }

    // Log "request"
    $log_settings = $this->controller->getSettings('log');
    if ($log_settings['response'] == 'response') {
      $this->controller->log('First Data GGe4 HPP response (raw post)', $raw_response);
    }

    // Exit if there is no processing entity
    if (empty($raw_response['commerce_order_id']) && empty($raw_response['commerce_card_id'])) {
      watchdog('commerce_firstdata_gge4', 'Process response accessed with no order or card data submitted.', array(), WATCHDOG_WARNING);
      return array();
    }

    // Decode the raw response
    foreach ($raw_response as $k => $v) {
      $raw_response[rawurldecode($k)] = rawurldecode($v);
    }
    
    // Check relay response hash
    if (($hash_calc = $this->generateResponseHash($raw_response)) != drupal_strtolower($raw_response['x_MD5_Hash'])) {
      watchdog('commerce_firstdata_gge4', 'An unauthenticated response from First Data made it to checkout for order @order_id, card @card_id: md5 mismatch - calcluated @hash_calc, response @hash_response.', array(
        '@order_id' => !empty($raw_response['commerce_order_id']) ? $raw_response['commerce_order_id'] : '--',
        '@card_id' => !empty($raw_response['commerce_card_id']) ? $raw_response['commerce_card_id'] : '--',
        '@hash_calc' => $hash_calc,
        '@hash_response' => $raw_response['x_MD5_Hash'], 
      ), WATCHDOG_ERROR);
      return array();
    }

    // Get settings
    $settings = $this->getSettings();

    // Convert to common web service parameters
    $response = $this->convertResponseToWebService($raw_response);

    // Build a request state
    $state = $this->buildRelayResponseState($response);
    
    // Update processed response
    $state['processed_response'] = $response;

    // Event context
    $context = array(
      'plugin' => $this->plugin['name'],
      'response' => $response,
      'state' => $state,
    );

    // Trigger relay response handlers
    $this->controller->trigger('response_process', $context);

    // Set to context to allow others to update the state
    $state = $context['state'];
    $state['processed'] = TRUE;
    
    /** @todo: more descriptive with card, etd ***/
    watchdog('commerce_firstdata_gge4', 'Response processed for Order @order_number, Card @card_id.', array(
      '@order_number' => isset($state['order']->order_number) ? $state['order']->order_number : ' -- ',
      '@card_id' => isset($state['card']->card_id) ? $state['card']->card_id : ' -- ',
    ), WATCHDOG_INFO);

    // Return redirect markup
    return $state;
  }

  /**
   * Returns TRUE if the relay response hash is valid
   */
  public function generateResponseHash($response) {
    if (!isset($response['x_trans_id']) || !isset($response['x_amount'])) {
      return '';
    }

    $settings = $this->getSettings();
    $trans_id = $response['x_trans_id'];
    $amount = $response['x_amount'];
    
    $hash = md5($settings['response_key'] . $settings['page_id'] . $trans_id . number_format($amount, 2, '.', ''));
    return drupal_strtolower($hash);
  }

  /**
   * Returns an md5 hash for the redirect url using the response key
   */
  public function generateRedirectHash($args) {
    if (isset($args['commerce'])) {
      $args = $args['commerce'];
    }
    unset($args['hash']);
    
    $seed = implode('^', $args);
    return drupal_strtolower(md5($this->getSettings('response_key') . $seed));
  }

  /**
   * Relay response: order processing
   */
  protected function relayResponseProcessOrder(&$context) {
    $response = $context['response'];
    $state = &$context['state'];

    // Exit if no order
    if (empty($response['commerce_order_id'])) {
      return;
    }

    // Reference order in resolved state
    $order = &$state['order'];
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

    // Save transaction
    $transaction = $this->controller->saveTransaction($response, $state);
    if ($transaction) {
      $state['transaction'] = $transaction;
    }

    // If this order does not have a billing profile yet ...
    if ($order_wrapper->commerce_customer_billing->value() === NULL) {
      // Create new and set address to resolved billing address from response
      $billing_profile = commerce_customer_profile_new('billing', $order->uid);
      $billing_profile_wrapper = entity_metadata_wrapper('commerce_customer_profile', $billing_profile);
      $billing_profile_wrapper->commerce_customer_address = $state['billing_address'] + addressfield_default_values();
      $billing_profile_wrapper->save();
      $order_wrapper->commerce_customer_billing = $billing_profile_wrapper;
      $order_wrapper->save();
      watchdog('commerce_firstdata_gge4', 'Billing profile created for Order @order_number containing the address data from the offsite Hosted Payment Page.', array('@order_number' => $order->order_number));
    }

/** @todo need this or checkout pane handle it? paypal does this ... **/
    if (!empty($response['transaction_approved'])) {
      // Send the customer on to the next checkout page.
      commerce_payment_redirect_pane_next_page($order, t('Customer successfully submitted payment at the payment gateway.'));
    }
    else {
      // Otherwise send the customer back.
      commerce_payment_redirect_pane_previous_page($order, t('Customer payment submission failed at the payment gateway.'));
    }
  }
}
