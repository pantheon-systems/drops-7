<?php

/**
 * @file
 * Defines LingotekAccount.
 */

/**
 * A class representing a Lingotek Account
 *
 * The Account class works off a locally cached copy of the account information, so it doesnt have to query the billing server very often.
 */
class LingotekAccount {

  const NOT_FOUND = 'not_found';
  const ACTIVE = 'active';
  const UNKNOWN = 'unknown';
  const NONE = 'none';
  const ENTERPRISE = 'enterprise';

  /**
   * Holds the static instance of the singleton object.
   *
   * @var LingotekAccount
   */
  private static $instance;
  private $status;
  private $plan;
  private $planType;

  /**
   * Constructor.
   *
   * Sets default values
   */
  public function __construct() {
    $this->status = self::UNKNOWN;
    $this->planType = self::UNKNOWN;

    // Load the Current Account Status from Cached local Drupal information.
    $current_status = variable_get('lingotek_account_status', NULL);
    $current_plan_type = variable_get('lingotek_account_plan_type', NULL);
    if (isset($current_status) && isset($current_plan_type)) {
      $this->setStatus($current_status);
      $this->setPlanType($current_plan_type);
    }
    else { // If the Account data isn't cached locally pull it down.
      $this->getAccountStatus();
    }
  }

  /**
   * Gets the singleton instance of the Account class.
   *
   * @return LingotekAccount
   *   An instantiated LingotekAccount object.
   */
  public static function instance() {
    if (!isset(self::$instance)) {
      $class_name = __CLASS__;
      self::$instance = new $class_name();
    }
    return self::$instance;
  }

  public function setStatus($value = 'inactive') {
    $this->status = $value;
  }

  public function getStatus() {
    return $this->status;
  }

  public function getStatusText() {
    return ( $this->status == 'active' ) ? '<span style="color: green;">Active</span>' : '<span style="color: red;">Inactive</span>';
  }

  public function getManagedTargets($as_detailed_objects = FALSE) {
    lingotek_add_missing_locales(); // fills in any missing lingotek_locale values to the languages table

    $targets_drupal = language_list();
    $default_language = language_default();

    $targets = array();
    foreach ($targets_drupal as $key => $target) {
      $is_source = $default_language->language == $target->language;
      $is_lingotek_managed = $target->lingotek_enabled;
      if ($is_source) {
        continue; // skip, since the source language is not a target
      }
      else if (!$is_lingotek_managed) {
        continue; // skip, since lingotek is not managing the language
      }
      $target->active = $target->lingotek_enabled;
      $targets[$key] = $target;
    }
    $result = $as_detailed_objects ? $targets : array_map(create_function('$obj', 'return $obj->lingotek_locale;'), $targets);
    return $result;
  }

  public function getManagedTargetsAsJSON() {
    return drupal_json_encode(array_values($this->getManagedTargets(FALSE, TRUE)));
  }

  public function setPlan($plan) {
    $this->plan = $plan;
    if (is_object($plan) && isset($plan->type)) {
      $this->setPlanType($plan->type);
    }
  }

  public function getPlan() {
    return $this->plan;
  }

  public function setPlanType($type = 'unknown') {
    variable_set('lingotek_account_plan_type', $type);
    $standard_types = array('cosmopolitan_monthly', 'cosmopolitan_yearly'); // if in this list, then set to 'standard'
    $type = in_array($type, $standard_types) ? 'standard' : $type;
    $this->planType = $type;
  }

  public function getPlanType() {
    return $this->planType;
  }

  public function isPlanType($type) {
    // isPlanType type values: 'enterprise', 'standard'
    $account_type = $this->getPlanType();
    return (strcasecmp($type, $account_type) == 0);
  }

  public function getPlanTypeText() {
    $plan_pieces = explode('_', $this->getPlanType());
    $details = ucwords(end($plan_pieces)); // e.g., Enterprise, Monthly, Yearly
    return $details;
  }

  public function isEnterprise() {
    return $this->isPlanType(self::ENTERPRISE);
  }

  public function getEnterpriseStatusText() {
    return ( $this->isPlanType(self::ENTERPRISE) ) ? '<span style="color: green;">Yes</span>' : '<span>No</span>';
  }

  /**
   * Get Account Status
   * NOTE:  You shouldnt need to call this directly.  Its called in the constructor.
   * Request:  https://LINGOTEK_BILLING_SERVER/billing/account.json?community=B2MMD3X5&external_id=community_admin&oauth_key=28c279fa-28dc-452e-93af-68d194a2c366&oauth_secret=0e999486-3b4d-47e4-ba9a-d0f3f0bbda73
   * Response:  {"state":"active","plan":{"trial_ends_at":0,"state":"active","activated_at":1355267936,"type":"cosmopolitan_monthly","languages_allowed":2,"language_cost_per_period_in_cents":14900}}
   * Will return FALSE or a json decoded object.
   */
  function getAccountStatus() {

    $result = FALSE;

    $parameters = array(
      'community' => variable_get('lingotek_community_identifier', ''),
      'external_id' => variable_get('lingotek_login_id', ''),
      'oauth_key' => variable_get('lingotek_oauth_consumer_id', ''),
      'oauth_secret' => variable_get('lingotek_oauth_consumer_secret', ''),
    );

    if (!empty($parameters['community']) && !empty($parameters['external_id']) && !empty($parameters['oauth_key']) && !empty($parameters['oauth_secret'])) {

      $timer_name = 'GET -' . microtime(TRUE);
      timer_start($timer_name);

      $api_url = LINGOTEK_BILLING_SERVER;

      $ch = curl_init($api_url . '?' . http_build_query($parameters));
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
      curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
      //curl_setopt( $ch, CURLINFO_HEADER_OUT, TRUE );

      $response = curl_exec($ch);
      $info = curl_getinfo($ch);
      curl_close($ch);

      $response_json = json_decode($response);
      //debug( $response ); //debug( $info );

      $timer_results = timer_stop($timer_name);

      $message_params = array(
        '@url' => $api_url,
        '@method' => 'GET account (billing API)',
        '!params' => $parameters,
        //'!request' => $request,
        '!response' => $response_json,
        '@response_time' => number_format($timer_results['time']) . ' ms',
      );
      
      if (isset($response_json) && $info['http_code'] == 200) { // Did we get valid json data back?  If not, $json is NULL.
        //debug ( $json );
        LingotekLog::info('<h1>@method</h1>
        <strong>API URL:</strong> @url
        <br /><strong>Response Time:</strong> @response_time<br /><strong>Request Params</strong>: !params<br /><strong>Response:</strong> !response', $message_params, 'api');

        $response_data = $response;


        $result = TRUE;

        // Not Found - {"state":"not_found"} - Account isn't setup yet.  The state after autoprovisioning a community, but before setting up your billing account.
        if ($response_json->state == self::NOT_FOUND) {
          $this->setStatus(self::NOT_FOUND);
          $this->setPlan(self::NONE);
        } // END:  Not Found
        // Active Account
        // Additionally, Save the account settings locally.
        elseif ($response_json->state == self::ACTIVE) {

          $this->setStatus(self::ACTIVE);
          variable_set('lingotek_account_status', self::ACTIVE);

          if (is_object($response_json->plan)) {

            $this->setPlan($response_json->plan);
          } // END:  Plan

          menu_rebuild();
        } // END  Active
      } // END:  Got 200 Response
      else {
        LingotekLog::error('<h1>@method (Failed)</h1>
        <strong>API URL:</strong> @url
        <br /><strong>Response Time:</strong> @response_time<br /><strong>Request Params</strong>: !params<br /><strong>Response:</strong> !response<br/><strong>Full Request:</strong> !request', $message_params, 'api');
      }
    } // END:  has credentials

    return $result;
  }

}
