<?php

/**
 * @file
 * First Data Global Gateway e4 API implementation.
 */

/**
 * Provide a separate Exception so it can be caught separately.
 */
class CommerceFirstDataGGE4ControllerException extends Exception { }

/**
 * Gateway API Controller class
 */
class CommerceFirstDataGGE4Controller {
  // -----------------------------------------------------------------------
  // Class methods - API definitions

  /**
   * Convert a Drupal language code to an acceptable API language
   */
  public static function convertLanguage($langcode) {
    $langcode = strtoupper($langcode);
    return in_array($langcode, array('EN', 'FR', 'ES')) ? $langcode : 'EN';
  }

  /**
   * Transaction type information
   *
   * @ref: https://firstdata.zendesk.com/entries/407574-transaction-types-available
   */
  public static function transactionTypes() {
    $types = &drupal_static(__FUNCTION__);

    if (!isset($types)) {
      $types = array(
        COMMERCE_CREDIT_AUTH_CAPTURE => array(
          'commerce_code' => COMMERCE_CREDIT_AUTH_CAPTURE,
          'gateway_code' => '00',
          'label' => t('Authorization and capture'),
          'short_label' => t('Auth and capture'),
          'action_word' => array(
            'present' => t('authorize and capture'),
            'past' => t('authorized and captured'),
          ),
          'requires_card' => TRUE,
          'zero_auth_allowed' => TRUE,
          'allowed_transactions' => array(
            COMMERCE_CREDIT_VOID,
            COMMERCE_CREDIT_CREDIT,
          ),
        ),
        COMMERCE_CREDIT_AUTH_ONLY => array(
          'commerce_code' => COMMERCE_CREDIT_AUTH_ONLY,
          'gateway_code' => '01',
          'label' => t('Authorization only'),
          'short_label' => t('Auth only'),
          'action_word' => array(
            'present' => t('authorize'),
            'past' => t('authorized'),
          ),
          'commerce_status_success' => COMMERCE_PAYMENT_STATUS_PENDING,
          'requires_card' => TRUE,
          'zero_auth_allowed' => TRUE,
          'allowed_transactions' => array(
            COMMERCE_CREDIT_PRIOR_AUTH_CAPTURE,
            COMMERCE_CREDIT_VOID,
          ),
        ),
        FIRSTDATA_GGE4_CREDIT_PREAUTH_ONLY => array(
          'commerce_code' => FIRSTDATA_GGE4_CREDIT_PREAUTH_ONLY,
          'gateway_code' => '05',
          'label' => t('Pre-Authorization Only'),
          'short_label' => t('Pre-Auth only'),
          'action_word' => array(
            'present' => t('authorize'),
            'past' => t('authorized'),
          ),
          'commerce_status_success' => COMMERCE_PAYMENT_STATUS_GGE4_PREAUTH_ONLY,
          'requires_card' => TRUE,
        ),
        COMMERCE_CREDIT_PRIOR_AUTH_CAPTURE => array(
          'commerce_code' => COMMERCE_CREDIT_PRIOR_AUTH_CAPTURE,
          'gateway_code' => '32',
          'label' => t('Prior authorization capture'),
          'short_label' => t('Prior auth capture'),
          'action_word' => array(
            'present' => t('capture'),
            'past' => t('captured'),
          ),
          'transaction_operation' => TRUE,
          'variable_amount' => TRUE,
          'updates_existing' => TRUE,
          'expiration' => 5 * 86400, // First Data states 2- 5 days
          'max_amount_factor' => 1.15,
          'allowed_transactions' => array(
            COMMERCE_CREDIT_VOID,
            COMMERCE_CREDIT_CREDIT,
          ),
        ),
        COMMERCE_CREDIT_VOID => array(
          'commerce_code' => COMMERCE_CREDIT_VOID,
          'gateway_code' => '33',
          'label' => t('Void'),
          'short_label' => t('Void'),
          'action_word' => array(
            'present' => t('void'),
            'past' => t('voided'),
          ),
          'transaction_operation' => TRUE,
          'updates_existing' => TRUE,
          'changed_expiration' => 86400, // 1 day since last change
          'commerce_status_success' => COMMERCE_PAYMENT_STATUS_GGE4_VOIDED,
        ),
        COMMERCE_CREDIT_CREDIT => array(
          'commerce_code' => COMMERCE_CREDIT_CREDIT,
          'gateway_code' => '34',
          'label' => t('Credit'),
          'short_label' => t('Credit'),
          'action_word' => array(
            'present' => t('credit'),
            'past' => t('credited'),
          ),
          'ledger_type' => 'credit',
          'transaction_operation' => TRUE,
          'variable_amount' => TRUE,
          'max_amount_factor' => 1,
          'expiration' => 86400 * 120, // 120 days, @todo: verify 120 day refund
          'allowed_transactions' => array(
            COMMERCE_CREDIT_VOID,
          ),
        ),
        FIRSTDATA_GGE4_CREDIT_REFUND => array(
          'commerce_code' => FIRSTDATA_GGE4_CREDIT_REFUND,
          'gateway_code' => '04',
          'label' => t('Refund'),
          'description' => t('Refunds an amount for a credit card without the need of a prior transaction.'),
          'short_label' => t('Refund'),
          'action_word' => array(
            'present' => t('refund'),
            'past' => t('refunded'),
          ),
          'ledger_type' => 'credit',
          'requires_card' => TRUE,
          'allowed_transactions' => array(
            COMMERCE_CREDIT_VOID,
          ),
        ),
      );

      // Merge defaults
      foreach ($types as &$type) {
        $type += array(
          'commerce_status_success' => COMMERCE_PAYMENT_STATUS_SUCCESS,
        );
      }
    }

    return $types;
  }

  /**
   * Returns the supported credit card types
   *
   * @return
   *   An array keyed by Drupal Commerce credit card type key and values of
   *   First Data credit card type string
   */
  public static function supportedCardTypes() {
    return array(
      'visa' => 'Visa',
      'mastercard' => 'Mastercard',
      'amex' => 'American Express',
      'discover' => 'Discover',
      'dc' => 'Diners Club',
      'jcb' => 'JCB',
    );
  }

  /**
   * Returns the message text for an AVS response code.
   */
  public static function avsMessage($code) {
    switch ($code) {
      case 'X':
        return t('Address (Street) and nine digit ZIP match');
      case 'Y':
        return t('Address (Street) and five digit ZIP match');
      case 'A':
        return t('Address (Street) matches, ZIP does not');
      case 'W':
        return t('Nine digit ZIP matches, Address (Street) does not');
      case 'Z':
        return t('Five digit ZIP matches, Address (Street) does not');
      case 'N':
        return t('No Match on Address (Street) or ZIP');
      case 'U':
        return t('Address information is unavailable');
      case 'G':
        return t('Non-U.S. Card Issuing Bank');
      case 'R':
        return t('Retry Ð System unavailable or timed out');
      case 'E':
        return t('AVS error'); // Not a mail or phone order
      case 'S':
        return t('Service not supported by issuer');
      case 'Q':
        return t('Bill to address did not pass edit checks');
      case 'D':
        return t('International street address and postal code match');
      case 'B':
        return t('International street address match, postal code not verified due to incompatible formats');
      case 'C':
        return t('International street address and postal code not verified due to incompatible formats');
      case 'P':
        return t('International postal code match, street address not verified due to incompatible format');
      case '1':
        return t('Cardholder name matches');
      case '2':
        return t('Cardholder name, billing address, and postal code match');
      case '3':
        return t('Cardholder name and billing postal code match');
      case '4':
        return t('Cardholder name and billing address match');
      case '5':
        return t('Cardholder name incorrect, billing address and postal code match');
      case '6':
        return t('Cardholder name incorrect, billing postal code matches');
      case '7':
        return t('Cardholder name incorrect, billing address matches');
      case '8':
        return t('Cardholder name, billing address, and postal code are all incorrect');
    }

    return '-';
  }

  /**
   * Returns the message text for a CVV match.
   */
  public static function cvvMessage($code) {
    switch ($code) {
      case 'M':
        return t('Match');
      case 'N':
        return t('No Match');
      case 'P':
        return t('Not Processed');
      case 'S':
        return t('Should have been present');
      case 'U':
        return t('Issuer unable to process request');
    }

    return '-';
  }


  // -----------------------------------------------------------------------
  // Class methods

  /**
   * Returns transaction type information for a given Commerce type code
   *
   * @param $commerce_code
   *   The transaction type constant used by Drupal Commerce
   */
  public static function transactionType($commerce_code) {
    $types = self::transactionTypes();
    return isset($types[$commerce_code]) ? $types[$commerce_code] : array();
  }

  /**
   * Returns transaction type information for a given Gateway type code
   *
   * @param $gateway_code
   *   The transaction type code used by First Data
   */
  public static function transactionTypeByGateway($gateway_code) {
    $types = self::transactionTypes();
    foreach ($types as $type) {
      if ($type['gateway_code'] == $gateway_code) {
        return $type;
      }
    }
    return array();
  }

  /**
   * Returns transaction type information for a given Commerce type code
   *
   * @param $commerce_code
   *   The transaction type constant used by Drupal Commerce
   */
  public static function transactionMaxAmount($txn_commerce_code, $transaction) {
    if (!isset($transaction->amount) || !isset($transaction->currency_code)) {
      return array();
    }

    $max = array(
      'amount' => $transaction->amount,
      'currency_code' => $transaction->currency_code,
    );
    
    if ($type_info = self::transactionType($txn_commerce_code)) {
      if (!empty($type_info['max_amount_factor'])) {
        $max['amount'] *= $type_info['max_amount_factor'];

        // Always round down to avoid exceeding the max
        $max['amount'] = floor($max['amount']);
      }
    }

    return $max;
  }

  /**
   * Converts the card type into First Data accepted values
   *
   * @param $card_type
   *   The Drupal Commerce credit card type key
   *
   * @return
   *   The corresponding First Data credit card type string
   */
  public static function convertCardType($card_type) {
    $types = self::supportedCardTypes();
    return isset($types[$card_type]) ? $types[$card_type] : '';
  }

  /**
   * Reverses First Data card type to a Drupal Commerce card type
   *
   * @param $gateway_card_type
   *   The First Data card type string
   *
   * @return
   *   The corresponding Drupal Commerce card type
   */
  public static function reverseCardType($gateway_card_type) {
    $types = self::supportedCardTypes();
    $key = array_search($gateway_card_type, $types);
    return $key !== FALSE ? $key : '';
  }
  

  // -----------------------------------------------------------------------
  // Class Helpers

  /**
   * Parse a billing profile into an address array
   */
  public static function parseBillingProfile($billing_profile) {
    $field_items = field_get_items('commerce_customer_profile', $billing_profile, 'commerce_customer_address');
    if (empty($field_items)) {
      return array();
    }

    $address = reset($field_items);

    // Ensure names
    $address += array(
      'name_line' => '',
      'first_name' => '',
      'last_name' => '',
    );
    self::addressEnsureNames($address);

    // Street single line
    $streets = array();
    if (!empty($address['thoroughfare'])) {
      $streets[] = $address['thoroughfare'];
    }
    if (!empty($address['premise'])) {
      $streets[] = $address['premise'];
    }
    if (!empty($address['sub_premise'])) {
      $streets[] = $address['sub_premise'];
    }

    $address['street_line'] = implode(' ', $streets);

    return $address;
  }

  /**
   * Ensures all address name values are populated
   * - Alters address in place
   */
  public static function addressEnsureNames(&$address) {
    if (!empty($address['name_line'])) {
      // Split to first and last
      if (empty($address['first_name']) && empty($address['last_name'])) {
        $names = preg_split('@\s+@', $address['name_line'], 2);
        if (!empty($names)) {
          $address['first_name'] = array_shift($names);
          if (!empty($names)) {
            $address['last_name'] = implode(' ', $names);
          }
        }
      }
    }
    else {
      // Combine first and last into name_line
      $names = array();
      if (!empty($address['first_name'])) {
        $names[] = $address['first_name'];
      }
      if (!empty($address['last_name'])) {
        $names[] = $address['last_name'];
      }

      $address['name_line'] = implode(' ', $names);
    }
  }

  /**
   * Returns TRUE if the addressfield arrays are considered equal
   */
  public static function addressIsEqual($a, $b) {
    $excluded = array('street_line' => 1, 'data' => 1, 'organisation_name' => 1);

    self::addressEnsureNames($a);
    self::addressEnsureNames($b);

    $a_compare = array_filter(array_diff_key($a, $excluded));
    foreach ($a_compare as $k => $v) {
      if (!isset($b[$k]) || $b[$k] != $v) {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Returns a country name given a country code
   */
  public static function getCountryName($country_code) {
    if ($country_code) {
      include_once DRUPAL_ROOT . '/includes/locale.inc';
      $countries = country_get_list();
      $country_code = drupal_strtoupper($country_code);
      return isset($countries[$country_code]) ? $countries[$country_code] : '';
    }
  }

  /**
   * Returns a country code given a country name
   */
  public static function getCountryCode($country_name) {
    if ($country_name) {
      include_once DRUPAL_ROOT . '/includes/locale.inc';
      $countries = country_get_list();
      $countries = array_map('drupal_strtoupper', $countries);

      $country_name = drupal_strtoupper($country_name);
      $country_code = array_search($country_name, $countries);
      return $country_code !== FALSE ? drupal_strtoupper($country_code) : '';
    }
  }

  /**
   * Returns a state name given a state code
   */
  public static function getStateName($state_code, $country_code = NULL) {
    if ($state_code) {
      $state_code = drupal_strtoupper($state_code);
      $country_code = $country_code ? drupal_strtoupper($country_code) : 'US';

      $address['country'] = $country_code;
      $format_handlers = array('address');
      $context = array('mode' => 'form');
      $address_element = addressfield_generate($address, $format_handlers, $context);
      if (!empty($address_element['locality_block']['administrative_area']['#options'])) {
        $state_options = $address_element['locality_block']['administrative_area']['#options'];
        $state_options = array_change_key_case($state_options, CASE_UPPER);
        return isset($state_options[$state_code]) ? $state_options[$state_code] : '';
      }
    }
  }

  /**
   * Returns a state code given a state name
   */
  public static function getStateCode($state_name, $country_code = NULL) {
    if ($state_name) {
      $state_name = drupal_strtoupper($state_name);
      $country_code = $country_code ? drupal_strtoupper($country_code) : 'US';

      $address['country'] = $country_code;
      $format_handlers = array('address');
      $context = array('mode' => 'form');
      $address_element = addressfield_generate($address, $format_handlers, $context);
      if (!empty($address_element['locality_block']['administrative_area']['#options'])) {
        $state_options = $address_element['locality_block']['administrative_area']['#options'];
        $state_options = array_map('drupal_strtoupper', $state_options);

        $state_code = array_search($state_name, $state_options);
        return $state_code !== FALSE ? drupal_strtoupper($state_code) : '';
      }
    }
  }

  
  // -----------------------------------------------------------------------
  // Instance methods and properties

  /**
   * The loaded Commerce payment method instance.
   */
  public $payment_instance;

  /**
   * The payment method instance settings merged with defaults.
   */
  protected $settings;

  /**
   * The component plugin definitions
   */
  protected $plugins;

  /**
   * The loaded component plugin objects
   */
  protected $components = array();


  /**
   * Constructor.
   *
   * @param $instance_id
   *   The payment method instance ID
   */
  public function __construct($payment_instance_id = NULL, $settings = array()) {
    $this->payment_instance = NULL;
    $this->settings = array();

    // Set given settings before any instance settings
    if (!empty($settings) && is_array($settings)) {
      $this->settings += $settings;
    }

    // Set payment method instance
    if (!empty($payment_instance_id) && ($payment_instance = commerce_payment_method_instance_load($payment_instance_id))) {
      $this->payment_instance = $payment_instance;
      
      // Merge instance settings
      if (!empty($this->payment_instance['settings'])) {
        $this->settings += $this->payment_instance['settings'];
      }
    }

    // Merge defaults
    $this->settings += $this->defaultSettings();
  }

  /**
   * Default settings
   */
  public function defaultSettings() {
    $settings = array(
      'txn_mode' => FIRSTDATA_GGE4_TXN_MODE_DEVELOPER,
      'txn_type' => COMMERCE_CREDIT_AUTH_CAPTURE,
      'cardonfile' => FALSE,
      'log' => array('request' => 0, 'response' => 0),
      'card_types' => array(),
    );

    // Load all enabled components
    foreach ($this->getEnabledComponents() as $plugin_name => $component) {
      if ($component_defaults = $component->defaultSettings()) {
        if (!empty($this->settings[$plugin_name])) {
          $this->settings[$plugin_name] += $component_defaults;
        }
        else {
          $this->settings[$plugin_name] = $component_defaults;
        }
      }
    }
    
    return $settings;
  }
  
  /**
   * Returns the instance settings merged with defaults
   *
   * @param $name
   *   Optional. A property name
   */
  public function getSettings($name = NULL) {
    if (isset($name)) {
      return isset($this->settings[$name]) ? $this->settings[$name] : NULL;
    }

    return $this->settings;
  }

  /**
   * Settings form
   */
  public function settingsForm() {
    module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
    $module_path = drupal_get_path('module', 'commerce_firstdata_gge4');

    $settings = $this->settings;

    // Merge defaults of ALL components, enabled and disabled
    foreach ($this->getComponents() as $plugin_name => $component) {
      if ($component_defaults = $component->defaultSettings()) {
        if (!empty($settings[$plugin_name])) {
          $settings[$plugin_name] += $component_defaults;
        }
        else {
          $settings[$plugin_name] = $component_defaults;
        }
      }
    }

    $form = array();
    $form['#attached']['css'][] = array(
      'data' => $module_path . '/css/commerce_firstdata_gge4.admin.css',
    );
    $form['header'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="commerce-firstdata-gge4-setting-header clearfix">',
      '#suffix' => '</div>',
      '#weight' => -100,
    );
    $form['header']['logo'] = array(
      '#theme' => 'image',
      '#path' => $module_path . '/images/firstdata-logo.png',
      '#width' => '20%',
      '#attributes' => array('class' => 'commerce-firstdata-gge4-logo'),
    );

    // Plugin forms
    $plugins = $this->getPlugins();
    foreach ($this->getComponents() as $plugin_name => $component) {
      $component_form = $component->settingsForm();
      if (empty($component_form)) {
        continue;
      }

      $form[$plugin_name] = array(
        '#type' => 'container',
        '#title' => t('@title', array('@title' => $plugins[$plugin_name]['title'])),
        '#attributes' => array('class' => array(
          'commerce-firstdata-gge4-settings-plugin-container',
          'commerce-firstdata-gge4-settings-plugin-container-' . drupal_html_class($plugin_name),
        )),
      );

      // Merge component form
      $form[$plugin_name] += $component_form;

      // Add enable effects and conditional required
      $this->processSettingsFormPluginEnable($plugin_name, $form[$plugin_name], $form);
    }

    // Common settings
    $form['txn_mode'] = array(
      '#type' => 'radios',
      '#title' => t('Transaction mode'),
      '#description' => t('Adjust to live transactions when you are ready to start processing real payments.') . '<br />' . t('Only specify a developer test account if you login to your account through https://demo.globalgatewaye4.firstdata.com.'),
      '#options' => array(
        FIRSTDATA_GGE4_TXN_MODE_LIVE => t('Live transactions in a live account'),
        FIRSTDATA_GGE4_TXN_MODE_LIVE_TEST => t('Test transactions in a live account'),
        FIRSTDATA_GGE4_TXN_MODE_DEVELOPER => t('Developer test account transactions'),
      ),
      '#default_value' => $settings['txn_mode'],
    );
    $form['txn_type'] = array(
      '#type' => 'radios',
      '#title' => t('Default credit card transaction type'),
      '#description' => t('The default will be used to process transactions during checkout.'),
      '#options' => array(
        COMMERCE_CREDIT_AUTH_CAPTURE => t('Authorization and capture'),
        COMMERCE_CREDIT_AUTH_ONLY => t('Authorization only (requires manual or automated capture after checkout)'),
      ),
      '#default_value' => $settings['txn_type'],
    );

    $card_type_options = commerce_payment_credit_card_types();
    $card_type_options = array_intersect_key($card_type_options, $this->supportedCardTypes());

    $form['card_types'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Limit accepted credit cards to the following types'),
      '#description' => t('If you want to limit acceptable card types, you should only select those supported by your merchant account.') . '<br />' . t('If none are checked, any credit card type will be accepted.'),
      '#options' => $card_type_options,
      '#default_value' => $settings['card_types'],
    );

    $form['log'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Log the following messages for debugging'),
      '#options' => array(
        'request' => t('API request messages'),
        'response' => t('API response messages'),
      ),
      '#default_value' => $settings['log'],
    );

    return $form;
  }
  
  /**
   * Add enable effects to a component's settings form
   * - enable effects for show / hide
   * - Conditional required processing to make plugin settings required only
   *   if the plugin is enabled
   */
  public function processSettingsFormPluginEnable($plugin_name, &$plugin_form, &$form) {
    if (!isset($plugin_form['enable'])) {
      return;
    }
    
    $plugin_form['enable']['#prefix'] = '<div class="commerce-firstdata-gge4-settings-plugin-enable">';
    $plugin_form['enable']['#suffix'] = '</div>';

    // requires states for 'enable' element
    if (($plugin = $this->getPlugins($plugin_name)) && !empty($plugin['requires'])) {
      $required_by = array();
      foreach ($plugin['requires'] as $required_name) {
        if ($required_name != $plugin_name) {
          $plugin_form['enable']['#states']['enabled'][':input[name*="[' . $required_name . '][enable]"]'] = array('checked' => TRUE);
          //$form[$required_name]['enable']['#states']['disabled'][':input[name*="[' . $plugin_name . '][enable]"]'] = array('checked' => TRUE);

          $required_by[$required_name][] = $plugin['title'];
          $required_plugin = $this->getPlugins($required_name);
          $requires[] = $required_plugin['title'];
        }
      }

      $form[$plugin_name]['enable']['#title'] .= ' &nbsp;' . t('[requires: @titles]', array('@title' => implode(', ', $requires)));
      foreach ($required_by as $required_name => $plugin_titles) {
        if (isset($form[$required_name]['enable']['#title'])) {
          $form[$required_name]['enable']['#title'] .= ' &nbsp;';
        }
        else {
          $form[$required_name]['enable']['#title'] = '';
        }
        $form[$required_name]['enable']['#title'] .= t('[required by: @titles]', array('@title' => implode(', ', $plugin_titles)));
      }
    }

    $enabled_selector = array(
      ':input[name*="[' . $plugin_name . '][enable]"]' => array('checked' => TRUE),
    );

    foreach (element_get_visible_children($plugin_form) as $element_key) {
      if ($element_key == 'enable') {
        continue;
      }
      $element = &$plugin_form[$element_key];
      $element['#states']['visible'] = $enabled_selector;

      if ($element['#type'] != 'item') {
        if (!empty($element['#required'])) {
          unset($element['#required']);
          $element['#states']['required'] = $enabled_selector;
          $element['#element_validate'] = array('commerce_firstdata_gge4_element_validate_plugin_required');
        }
      }
    }
  }

  /**
   * Return a credit card form
   *
   * @param $values
   *   Optional. An array of form state information
   *   - values: Current form values
   */
  public function creditCardForm($values = array()) {
    module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
    $settings = $this->settings;

    // Prepare the fields to include on the credit card form.
    $fields = array(
      'code' => '',
    );

    // Add the credit card types array if necessary.
    if (!empty($settings['card_types'])) {
      $card_types = array_filter($settings['card_types']);
      if (!empty($card_types)) {
        $fields['type'] = $card_types;
      }
    }

    return commerce_payment_credit_card_form($fields);
  }


  // -----------------------------------------------------------------------
  // Plugin handling

  /**
   * Retrieve component plugin object
   *
   * @return
   *  A valid enabled plugin object, else NULL
   */
  public function get($name) {
    if (!isset($this->components[$name])) {
      if (empty($this->plugins[$name])) {
        return;
      }

      $plugin = $this->plugins[$name];
      $this->components[$name] = new $plugin['class']($plugin, $this);
    }

    return $this->components[$name];
  }

  /**
   * Gets all plugin defintions
   */
  public function getPlugins($name = NULL) {
    if (!isset($this->plugins)) {
      $this->plugins = commerce_firstdata_gge4_get_component_plugins();
    }
    if (isset($name)) {
      return isset($this->plugins[$name]) ? $this->plugins[$name] : NULL;
    }
    return $this->plugins;
  }

  /**
   * Load all plugins
   */
  public function getComponents() {
    foreach ($this->getPlugins() as $name => $plugin) {
      if (!isset($this->components[$name])) {
        $this->components[$name] = $this->get($name);
      }
    }

    return $this->components;
  }

  /**
   * Load only enabled plugins
   */
  public function getEnabledComponents() {
    $filtered_components = array();
    foreach ($this->getPlugins() as $name => $plugin) {
      if ($this->isEnabled($name)) {
        $filtered_components[$name] = $this->get($name);
      }
    }

    return $filtered_components;
  }

  /**
   * Load only valid plugins
   */
  public function getValidComponents() {
    $filtered_components = array();
    foreach ($this->getPlugins() as $name => $plugin) {
      if ($this->isValid($name)) {
        $filtered_components[$name] = $this->get($name);
      }
    }

    return $filtered_components;
  }

  /**
   * Load only valid plugins
   */
  public function getActiveComponents() {
    $filtered_components = array();
    foreach ($this->getPlugins() as $name => $plugin) {
      if ($this->isActive($name)) {
        $filtered_components[$name] = $this->get($name);
      }
    }

    return $filtered_components;
  }

  /**
   * Returns TRUE if the component plugin is enabled via settings
   *
   * This is a settings based check so that the plugin class does not get loaded.
   *
   * @param $name
   *   The plugin name
   */
  public function isEnabled($name) {
    return !empty($this->settings[$name]['enable']);
  }

  /**
   * Returns TRUE if the component plugin is enabled and valid
   *
   * @param $name
   *   The plugin name
   */
  public function isValid($name) {
    if ($component = $this->get($name)) {
      return $component->isValid();
    }

    return FALSE;
  }

  /**
   * Returns TRUE if the component plugin is enabled and valid
   *
   * @param $name
   *   The plugin name
   */
  public function isActive($name) {
    if ($this->isEnabled($name) && $this->isValid($name)) {
      if ($plugin = $this->getPlugins($name)) {
        if (!empty($plugin['requires'])) {
          foreach ($plugin['requires'] as $required) {
            if ($required != $name && !$this->isActive($required)) {
              return FALSE;
            }
          }
        }
      }
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Trigger an event for active plugins and Drupal
   */
  public function trigger($event_name, &$context) {
    $return = array();
    foreach ($this->getActiveComponents() as $plugin_name => $component) {
      if (method_exists($component, 'on')) {
        $result = $component->on($event_name, $context);
        if (isset($result) && is_array($result)) {
          $return = array_merge_recursive($return, $result);
        }
        elseif (isset($result)) {
          $return[] = $result;
        }
      }
    }

    $modules_return = module_invoke_all('commerce_firstdata_gge4_' . $event_name, $context);
    if (!empty($modules_return)) {
      $return = array_merge($return, $modules_return);
    }

    return $return;
  }
  
  /**
   * Trigger an "alter" event for active plugins and Drupal
   */
  public function alter($event_name, &$data, &$context) {
    foreach ($this->getActiveComponents() as $plugin_name => $component) {
      if (method_exists($component, 'onAlter')) {
        $component->onAlter($event_name, $data, $context);
      }
    }

    drupal_alter('commerce_firstdata_gge4_' . $event_name, $data, $context);
  }


  // -----------------------------------------------------------------------
  // Payment State handling

  /**
   * Resolve a payment state given a variable amount of information
   *
   * @param $data
   *   An array of payment information
   *   - 'txn_type': A Drupal Commerce transaction type constant.
   *   - 'charge: A Drupal Commerce price array with 'amount', 'currency_code'
   *   - 'card': An equivalent Commerce card on file object
   *   - 'order': A Drupal Commerce order object
   *   - 'previous_transaction': A Drupal Commerce payment transaction object
   */
  public function resolvePaymentState(&$data) {
    if (!empty($data['#resolved'])) {
      return;
    }

    $data['#resolved'] = TRUE;
    $data += array(
      'txn_type' => $this->getSettings('txn_type'),
      'charge' => NULL,
      'card' => NULL,
      'order' => NULL,
      'customer' => NULL,
      'previous_transaction' => NULL,
      'billing_profile_id' => NULL,
      'billing_address' => NULL,
      'build_info' => array(
        'zero_amount' => FALSE,
      ),
    );


    // Order
    if (empty($data['order']) && !empty($data['previous_transaction']->order_id)) {
      $data['order'] = commerce_order_load($data['previous_transaction']->order_id);
    }

    // Charge
    if (empty($data['charge'])) {
      if (!empty($data['previous_transaction']->transaction_id) && !empty($data['previous_transaction']->order_id)) {
        $data['charge'] = array(
          'amount' => $data['previous_transaction']->amount,
          'currency_code' => $data['previous_transaction']->currency_code,
        );
      }
      elseif (!empty($data['order']) && ($order_balance = commerce_payment_order_balance($data['order']))) {
        $data['charge'] = $order_balance;
      }
      else {
        $charge = array(
          'amount' => 0,
          'currency_code' => commerce_default_currency(),
        );
      }
    }

    if (!isset($data['charge']['amount'])) {
      $data['charge']['amount_original'] = $data['charge']['amount'];
      $data['charge']['amount'] = abs($data['charge']['amount']);
    }

    // Customer
    if (empty($data['customer'])) {
      $customer = array();
      if (!empty($data['order']->uid)) {
        $customer['uid'] = $data['order']->uid;
      }
      elseif (!empty($data['card']->uid)) {
        $customer['uid'] = $data['card']->uid;
      }

      if (!empty($data['order']->mail)) {
        $customer['mail'] = $data['order']->mail;
      }
      elseif (!empty($data['customer']->uid) && ($customer_account = user_load($data['customer']->uid))) {
        $customer['mail'] = $customer_account->mail;
      }

      if (!empty($customer)) {
        $data['customer'] = (object) $customer;
      }
    }

    // Billing profile
    if (empty($data['billing_address'])) {
      if (empty($data['billing_profile_id'])) {
        if (!empty($data['order']) && isset($data['order']->commerce_customer_billing[LANGUAGE_NONE][0]['profile_id'])) {
          $data['billing_profile_id'] = $data['order']->commerce_customer_billing[LANGUAGE_NONE][0]['profile_id'];
        }
        elseif (!empty($data['card']) && isset($data['card']->commerce_cardonfile_profile[LANGUAGE_NONE][0]['profile_id'])) {
          $data['billing_profile_id'] = $data['card']->commerce_cardonfile_profile[LANGUAGE_NONE][0]['profile_id'];
        }
      }

      if (!empty($data['billing_profile_id'])) {
        $billing_profile = commerce_customer_profile_load($data['billing_profile_id']);
        $data['billing_address'] = $this->parseBillingProfile($billing_profile);
      }
    }
  }

  
  // -----------------------------------------------------------------------
  // Gateway response handling

  /**
   * Create Commerce Payment transaction for the response
   *
   * - Request has been validated if there is a response
   *
   * @param $response
   *   A First Data GGe4 response array
   * @param $request_state
   *   A payment state, @see CommerceFirstDataGGE4Controller::resolvePaymentState()
   */
  public function saveTransaction($response, $request_state) {
    $this->resolvePaymentState($request_state);

    // Exit if no order id
    if (empty($request_state['order']->order_id)) {
      return;
    }
    
    // Reference local vars
    $order = $request_state['order'];
    $prev_transaction = $request_state['previous_transaction'];

    // Fatal error, no other properties are provided
    if (!empty($response['gge4_fatal_error'])) {
      if (!empty($prev_transaction->transaction_id)) {
        $prev_transaction->payload[REQUEST_TIME] = $response;
        commerce_payment_transaction_save($prev_transaction);
      }
      return $prev_transaction;
    }

    // Get type info
    $txn_type_info = $this->transactionTypeByGateway($response['transaction_type']);

    // Determine transaction to update
    if (!empty($prev_transaction->transaction_id) &&
        (!empty($txn_type_info['updates_existing']) || empty($prev_transaction->remote_id))) {
      // Update existing transaction
      $transaction = $prev_transaction;
    }
    else {
      // Create a new transaction
      $transaction = commerce_payment_transaction_new();
      $transaction->order_id = $order->order_id;

      if (!empty($this->payment_instance['instance_id'])) {
        $transaction->instance_id = $this->payment_instance['instance_id'];
        if (!empty($this->payment_instance['method_id'])) {
          $transaction->payment_method = $this->payment_instance['method_id'];
        }
      }
      elseif (!empty($prev_transaction->instance_id)) {
        $transaction->instance_id = $prev_transaction->instance_id;
        if (!empty($prev_transaction->payment_method)) {
          $transaction->payment_method = $prev_transaction->payment_method;
        }
      }
      elseif (!empty($order->data['payment_method']) && ($payment_method = commerce_payment_method_instance_load($order->data['payment_method']))) {
        $transaction->instance_id = $payment_method['instance_id'];
        $transaction->payment_method = $payment_method['method_id'];
      }
      else {
        // Not enough info to create a transaction
        return;
      }

      // Resolve method id from instance id
      if (empty($transaction->payment_method)) {
        list($method_id, $rule_name) = explode('|', $transaction->instance_id);
        $transaction->payment_method = $method_id;
      }
    }

    // Determine if this is a new transaction
    $is_new = empty($transaction->transaction_id);

    // Determine if request was approved
    $is_approved = !empty($response['transaction_approved']);

    // Add payload
    $transaction->payload[REQUEST_TIME] = $this->processTransactionPayload($response);

    // if new or existing approved, then update these properties ...
    if ($is_new || $is_approved) {
      $transaction->remote_id = '';
      if (isset($response['sequence_no'])) {
        $transaction->remote_id = $response['sequence_no'];
      }

      // Store the latest transaction_tag.
      $transaction->data['transaction_tag'] = '';
      if (isset($response['transaction_tag'])) {
        $transaction->data['transaction_tag'] = $response['transaction_tag'];
      }

      // Store the latest authorization_num.
      $transaction->data['authorization_num'] = '';
      if (isset($response['authorization_num'])) {
        $transaction->data['authorization_num'] = $response['authorization_num'];
      }

      // Add a reference to the original transaction
      if ($is_new && !empty($prev_transaction->remote_id)) {
        $transaction->data['remote_id_reference'] = $prev_transaction->remote_id;
      }

      // Store the commerce type of transaction in the remote status.
      $transaction->remote_status = $txn_type_info['commerce_code'];

      // Update amount with charge details
      $transaction->amount = $request_state['charge']['amount'];
      $transaction->currency_code = $request_state['charge']['currency_code'];
      if (isset($txn_type_info['ledger_type']) && $txn_type_info['ledger_type'] == 'credit') {
        $transaction->amount *= (-1);
      }

      // If we didn't get an approval response code...
      if (!$is_approved) {
        // Create a failed transaction with the error message.
        $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
      }
      elseif (!empty($txn_type_info['commerce_status_success'])) {
        $transaction->status = $txn_type_info['commerce_status_success'];
      }
      else {
        $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      }

      // Build a meaningful response message.
      $message = array();
      $tokens = array(
        '@label' => $txn_type_info['short_label'],
        '@date' => format_date(REQUEST_TIME, 'short'),
        '@approval' => $is_approved? t('Accepted') : t('Rejected'),
      );

      if ($is_new) {
        // New transaction
        /** @todo: could be more meaningful? **/
        /** @see https://firstdata.zendesk.com/entries/471297-First-Data-Global-Gateway-e4-Bank-Response-Codes  ***/
        $message[] = '<b>' . t('@label', $tokens) . '</b>';
        $message[] = '<b>' . t('@approval', $tokens) . '</b> ';
        if (!empty($prev_transaction->transaction_id)) {
          $message[] = t('@label to @prev_remote_id', array(
            '@prev_remote_id' => $prev_transaction->remote_id
          ) + $tokens);
        }

        // Failure message
        if (!$is_approved && !empty($response['bank_message'])) {
          $message[] = check_plain($response['bank_message']);
        }

        // AVS message
        if (!empty($response['avs'])) {
          $message[] = t('AVS response: @avs', array('@avs' => $this->avsMessage($response['avs'])));
        }

        // CVV message
        if (!empty($response['cvv2'])) {
          $cvv_message = $this->cvvMessage($response['cvv2']);
          if ($cvv_message != '-') {
            $message[] = t('CVV match: @cvv', array('@cvv' => $this->cvvMessage($response['cvv2'])));
          }
        }
      }
      else {
        // Existing transaction
        if (!empty($transaction->message)) {
          $message[] = $transaction->message;
        }

        $message[] = t('@label - @approval: @date', $tokens);
      }

      // Combine messages
      $transaction->message = implode('<br />', $message);
    }


    // Save the transaction.
    commerce_payment_transaction_save($transaction);
    return $transaction;
  }

  /**
   * Process a payload for transaction storage
   */
  public function processTransactionPayload($data) {
    $data = $this->sanitizeParameters($data);

    // Remove any form data keys
    unset($data['form_build_id'], $data['form_id'], $data['form_token']);

    // Remove big data that is not needed to save storage space
    // CTR can be reproduced with theme_commerce_firstdata_gge4_ctr()
    unset($data['ctr']);
    
    return $data;
  }
  
  /**
   * Sanitize parameters for the request or response
   */
  public function sanitizeParameters($params) {
    // Param key => length as define in substr(), if 0 entire value is masked
    $sensitive_params = array(
      'gateway_id' => 0,
      'password' => 0,
      'cc_number' => -4,
      'cc_verification_str2' => 0,
      'x_login' => 0,
    );

    $safe_params = $params;
    foreach ($sensitive_params as $name => $plaintext_length) {
      if (isset($params[$name]) && ($strlen = strlen($params[$name]))) {
        if (!$plaintext_length) {
          $safe_params[$name] = str_repeat('X', $strlen);
        }
        elseif ($plaintext_length < 0) {
          $safe_params[$name] = str_repeat('X', ($strlen + $plaintext_length)) . substr($params[$name], $plaintext_length);
        }
        else {
          $safe_params[$name] = substr($params[$name], $plaintext_length) .  str_repeat('X', ($strlen - $plaintext_length));
        }
      }
    }

    return $safe_params;
  }

  /**
   * Safely log the request or response parameters
   */
  public function log($message, $data = array()) {
    $tokens = array();
    if (isset($this->payment_instance['instance_id'])) {
      $tokens['@instance_id'] = $this->payment_instance['instance_id'];
    };

    if (!empty($data)) {
      $safe_data = $this->sanitizeParameters($data);
      $tokens['@params'] = print_r($safe_data, TRUE);
      watchdog('commerce_firstdata_gge4', $message . ' (@instance_id)<pre>@params</pre>', $tokens, WATCHDOG_DEBUG);
    }
    else {
      watchdog('commerce_firstdata_gge4', $message . ' (@instance_id)', $tokens, WATCHDOG_DEBUG);
    }
  }
}
