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
  const ADVANCED = 'advanced';

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

    // assume an standard active account
    $current_status = variable_get('lingotek_account_status', self::ACTIVE);
    $current_plan_type = variable_get('lingotek_account_plan_type', 'standard');

    if (isset($current_status) && isset($current_plan_type)) {
      $this->setStatus($current_status);
      $this->setPlanType($current_plan_type);
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
    return ( $this->status == 'active' ) ? '<span style="color: green;">'.t('Active').'</span>' : '<span style="color: red;">'.t('Inactive').'</span>';
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
    // isPlanType type values: 'advanced', 'standard'
    $account_type = $this->getPlanType();
    return (strcasecmp($type, $account_type) == 0);
  }

  public function getPlanTypeText() {
    $plan_pieces = explode('_', $this->getPlanType());
    $details = ucwords(end($plan_pieces)); // e.g., Enterprise, Monthly, Yearly
    return $details;
  }

  public function showAdvanced() {
    return $this->isPlanType(self::ADVANCED);
  }

}
