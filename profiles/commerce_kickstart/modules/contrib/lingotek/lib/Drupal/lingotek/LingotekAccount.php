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


  const LINGOTEK_ACCOUNT_STATUS_UNKNOWN = 'unknown';
  const LINGOTEK_ACCOUNT_STATUS_NOT_FOUND = 'not_found';
  const LINGOTEK_ACCOUNT_STATUS_ACTIVE = 'active';

  const LINGOTEK_ACCOUNT_PLAN_UNKNOWN = 'unknown';
  const LINGOTEK_ACCOUNT_PLAN_NONE = 'none';
  const LINGOTEK_ACCOUNT_ENTERPRISE = 'enterprise';


  /**
   * Holds the static instance of the singleton object.
   *
   * @var LingotekAccount
   */
  private static $instance;

  private $status;
  private $plan;
  private $enterprise;

  /**
   * Constructor.
   *
   * Sets default values
   */
  public function __construct() {

    // Set the Defaults
    $this->status = self::LINGOTEK_ACCOUNT_STATUS_UNKNOWN;
    $this->plan   = self::LINGOTEK_ACCOUNT_PLAN_UNKNOWN;
    $this->enterprise = FALSE;

    // Load the Current Account Status from Cached local Drupal information.
    $current_status = variable_get( 'lingotek_account_status', NULL );
    $current_plan = variable_get( 'lingotek_account_plan', NULL );
    $current_enterprise = variable_get( 'lingotek_account_enterprise', NULL ); // Stored as 0/1
    if ( isset( $current_status ) && isset( $current_plan ) && isset( $current_enterprise ) ) {
      $this->setStatus( $current_status );
      $this->setPlan( $current_plan );
      $this->setEnterpriseStatus( $current_enterprise );
    }
    else { // If the Account data isn't cached locally pull it down.
      $this->getAccountStatus();
    }

  } // END:  __construct()


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

  public function setStatus( $value = 'inactive' ) {
    $this->status = $value;
  }

  public function getStatus() {
    return $this->status;
  }

  public function getStatusText() {
    return ( $this->status == 'active' ) ? '<span style="color: green;">Active</span>' : '<span style="color: red;">Inactive</span>';
  }
  
  public function getManagedTargets($as_detailed_objects = FALSE, $return_lingotek_codes = TRUE) {
    lingotek_add_missing_locales();// fills in any missing lingotek_locale values to the languages table
    
    $targets_drupal = language_list();
    $default_language = language_default();
    
    $targets = array();
    foreach ($targets_drupal as $key => $target) {
      $is_source = $default_language->language == $target->language;
      $is_lingotek_managed = ($this->isEnterprise() === TRUE) || $target->lingotek_enabled;//in_array($target->language, $lingotek_managed_targets);
      if ($is_source) {
        continue; // skip, since the source language is not a target
      }
      else if (!$is_lingotek_managed) {
        continue; // skip, since lingotek is not managing the language
      }
      $target->active = $target->lingotek_enabled;
      $targets[$key] = $target;
    }
    return $as_detailed_objects ? $targets : (array_map(function ($obj) {
              return $obj->lingotek_locale;
            }, $targets));
  }

  public function getManagedTargetsAsJSON() {
    return drupal_json_encode(array_values($this->getManagedTargets(FALSE, TRUE)));
  }

  public function setPlan( $value ) {
    $this->plan = $value;
  }

  public function getPlan() {
    return $this->plan;
  }

  public function getPlanText() {
    $plan_pieces = explode('_', $this->plan); 
    $details = ucwords(end($plan_pieces));// e.g., Enterprise, Monthly, Yearly
    return $details;
  }



  public function isEnterprise() {
    return $this->getEnterpriseStatus();
  }

  public function getEnterpriseStatus() {
    return $this->enterprise;
  }

  public function setEnterpriseStatus( $value ) {
    $this->enterprise = (bool)$value;
  }

  public function getEnterpriseStatusText() {
    return ( $this->enterprise === TRUE ) ? '<span style="color: green;">Yes</span>' : '<span>No</span>';
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

    $fields = array(
      'community'    => variable_get( 'lingotek_community_identifier', '' ),
      'external_id'  => variable_get( 'lingotek_login_id', '' ),
      'oauth_key'    => variable_get( 'lingotek_oauth_consumer_id', '' ),
      'oauth_secret' => variable_get( 'lingotek_oauth_consumer_secret', '' ),
    );

    if( !empty($fields['community']) && !empty($fields['external_id']) && !empty($fields['oauth_key']) && !empty($fields['oauth_secret']) ) {

      $ch = curl_init( LINGOTEK_BILLING_SERVER . '?' . http_build_query( $fields ) );
      curl_setopt( $ch, CURLOPT_RETURNTRANSFER, TRUE );
      curl_setopt( $ch, CURLOPT_SSL_VERIFYPEER, FALSE );
      //curl_setopt( $ch, CURLINFO_HEADER_OUT, TRUE );

      $response = curl_exec( $ch );
      $info = curl_getinfo( $ch );
      curl_close( $ch );

      //debug( $response );
      //debug( $info );

      $json = json_decode( $response );
      if ( isset( $json ) && $info['http_code'] == 200  ) { // Did we get valid json data back?  If not, $json is NULL.
        //debug ( $json );
        $result = TRUE;

        // Not Found - {"state":"not_found"} - Account isn't setup yet.  The state after autoprovisioning a community, but before setting up your billing account.
        if ( $json->state == self::LINGOTEK_ACCOUNT_STATUS_NOT_FOUND ) {
          $this->setStatus( self::LINGOTEK_ACCOUNT_STATUS_NOT_FOUND );
          $this->setPlan( self::LINGOTEK_ACCOUNT_PLAN_NONE );
        } // END:  Not Found


        // Active Account
        // Additionally, Save the account settings locally.
        elseif ( $json->state == self::LINGOTEK_ACCOUNT_STATUS_ACTIVE ) {

          $this->setStatus( self::LINGOTEK_ACCOUNT_STATUS_ACTIVE );
          variable_set( 'lingotek_account_status', self::LINGOTEK_ACCOUNT_STATUS_ACTIVE );

          if ( is_object( $json->plan ) ) {

            $this->setPlan( $json->plan->type );
            variable_set( 'lingotek_account_plan', $json->plan->type );

            if ( $json->plan->type == self::LINGOTEK_ACCOUNT_ENTERPRISE ) {
              $this->setEnterpriseStatus( TRUE );
              variable_set( 'lingotek_account_enterprise', 1 ); // Store as 0/1
            }
            else {
              //$this->setEnterpriseStatus( FALSE );
              variable_set( 'lingotek_account_enterprise', 0 ); // Store as 0/1
            }

          } // END:  Plan

          menu_rebuild();

        } // END  Active

      } // END:  Got 200 Response

    } // END:  has credentials

    return $result;

  } // END:  lingotek_get_account_status()

}
