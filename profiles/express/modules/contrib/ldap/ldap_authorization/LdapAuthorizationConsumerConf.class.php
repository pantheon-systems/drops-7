<?php

/**
 * @file
 * class to encapsulate an ldap entry to authorization consumer ids mapping configuration
 *
 * this is the lightweight version of the class for use on logon etc.
 * the LdapAuthorizationConsumerConfAdmin extends this class and has save,
 * iterate, etc methods.
 *
 */

/**
 * LDAP Authorization Consumer Configuration
 */
class LdapAuthorizationConsumerConf {

  public $sid = NULL;
  public $server;
  public $consumerType = NULL;
  public $consumerModule = NULL;
  public $consumer = NULL;
  public $inDatabase = FALSE;
  public $numericConsumerConfId = NULL;

  public $description = NULL;
  public $status = NULL;
  public $onlyApplyToLdapAuthenticated = TRUE;

  public $useFirstAttrAsGroupId = FALSE;

  public $mappings = array();
  public $useMappingsAsFilter = TRUE;

  public $synchToLdap = FALSE;

  public $synchOnLogon = TRUE;

  public $revokeLdapProvisioned = TRUE;
  public $regrantLdapProvisioned = TRUE;
  public $createConsumers = TRUE;

  public $errorMsg = NULL;
  public $hasError = FALSE;
  public $errorName = NULL;


  public function clearError() {
    $this->hasError = FALSE;
    $this->errorMsg = NULL;
    $this->errorName = NULL;
  }
   /**
   * Constructor Method
   */
  function __construct(&$consumer, $_new = FALSE, $_sid = NULL) {
    $this->consumer = $consumer;
    $this->consumerType = $consumer->consumerType;
    if ($_new) {
      $this->inDatabase = FALSE;
    }
    else {
      $this->inDatabase = TRUE;
      $exists = $this->loadFromDb();
      if (!$exists) {
        watchdog('ldap_authorization', 'failed to load existing %consumer object', array('%consumer' => $consumer->consumerType), WATCHDOG_ERROR);
      }
    }
    // default value for deriveFromEntryAttrMatchingUserAttr set up this way for backward compatibility in 1.0 branch,
    // make deriveFromEntryAttrMatchingUserAttr default to dn in 2.0 branch.
  }

  protected function loadFromDb() {
    if (module_exists('ctools')) {
      ctools_include('export');
      $result = ctools_export_load_object('ldap_authorization', 'names', array($this->consumerType));

      // @todo, this is technically wrong, but I don't quite grok what we're doing in the non-ctools case - justintime
      $server_record = array_pop($result);
      // There's no ctools api call to get the reserved properties, so instead of hardcoding a list of them
      // here, we just grab everything.  Basically, we sacrifice a few bytes of RAM for forward-compatibility.
    }
    else {
      $select = db_select('ldap_authorization', 'ldap_authorization');
      $select->fields('ldap_authorization');
      $select->condition('ldap_authorization.consumer_type',  $this->consumerType);
      $server_record = $select->execute()->fetchObject();
    }

    if (!$server_record) {
      $this->inDatabase = FALSE;
      return FALSE;
    }

    foreach ($this->field_to_properties_map() as $db_field_name => $property_name ) {
      if (isset($server_record->$db_field_name)) {
        if (in_array($db_field_name, $this->field_to_properties_serialized())) {
          $this->{$property_name} = unserialize($server_record->$db_field_name);
        }
        else {
          $this->{$property_name} = $server_record->$db_field_name;
        }
      }
    }
    $this->numericConsumerConfId = isset($server_record->numeric_consumer_conf_id)? $server_record->numeric_consumer_conf_id : NULL;
    $this->server = ldap_servers_get_servers($this->sid, NULL, TRUE);
    return TRUE;

  }

  // direct mapping of db to object properties
  public static function field_to_properties_map() {
    return array(
      'sid' => 'sid',
      'consumer_type' => 'consumerType',
      'numeric_consumer_conf_id'  => 'numericConsumerConfId' ,
      'status'  => 'status',
      'only_ldap_authenticated'  => 'onlyApplyToLdapAuthenticated',
      'use_first_attr_as_groupid'  => 'useFirstAttrAsGroupId',
      'mappings'  => 'mappings',
      'use_filter'  => 'useMappingsAsFilter',
      'synch_to_ldap' => 'synchToLdap',
      'synch_on_logon'  => 'synchOnLogon',
      'regrant_ldap_provisioned'  => 'regrantLdapProvisioned',
      'revoke_ldap_provisioned' => 'revokeLdapProvisioned',
      'create_consumers'  => 'createConsumers',
    );
  }

  public static function field_to_properties_serialized() {
    return array('mappings');
  }

  /**
   * Destructor Method
   */
  function __destruct() {

  }

  protected $_sid;
  protected $_new;

  protected function linesToArray($lines) {
    $lines = trim($lines);

    if ($lines) {
      $array = preg_split('/[\n\r]+/', $lines);
      foreach ($array as $i => $value) {
        $array[$i] = trim($value);
      }
    }
    else {
      $array = array();
    }
    return $array;
  }


  protected function pipeListToArray($mapping_list_txt, $make_item0_lowercase = FALSE) {
    $result_array = array();
    $mappings = preg_split('/[\n\r]+/', $mapping_list_txt);
    foreach ($mappings as $line) {
      if (count($mapping = explode('|', trim($line))) == 2) {
        $item_0 = ($make_item0_lowercase) ? drupal_strtolower(trim($mapping[0])) : trim($mapping[0]);
        $result_array[] = array($item_0, trim($mapping[1]));
      }
    }
    return $result_array;
  }
}
