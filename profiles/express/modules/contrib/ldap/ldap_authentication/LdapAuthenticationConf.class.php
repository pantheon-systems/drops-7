<?php
// $Id: LdapAuthenticationConf.class.php,v 1.4.2.2 2011/02/08 20:05:41 johnbarclay Exp $

/**
 * @file
 * This class represents an ldap_authentication module's configuration
 * It is extended by LdapAuthenticationConfAdmin for configuration and other admin functions
 */

class LdapAuthenticationConf {

  // no need for LdapAuthenticationConf id as only one instance will exist per drupal install

  public $sids = array();  // server configuration ids being used for authentication
  public $enabledAuthenticationServers = array(); // ldap server object
  public $inDatabase = FALSE;
  public $authenticationMode = LDAP_AUTHENTICATION_MODE_DEFAULT;
  public $loginUIUsernameTxt;
  public $loginUIPasswordTxt;
  public $ldapUserHelpLinkUrl;
  public $ldapUserHelpLinkText = LDAP_AUTHENTICATION_HELP_LINK_TEXT_DEFAULT;
  public $loginConflictResolve = LDAP_AUTHENTICATION_CONFLICT_RESOLVE_DEFAULT;
  public $acctCreation = LDAP_AUTHENTICATION_ACCT_CREATION_DEFAULT;
  public $emailOption = LDAP_AUTHENTICATION_EMAIL_FIELD_DEFAULT;
  public $emailUpdate = LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_DEFAULT;
  public $ssoEnabled = FALSE;
  public $ssoRemoteUserStripDomainName = FALSE;
  public $seamlessLogin = FALSE;
  public $ldapImplementation = FALSE;
  public $cookieExpire = LDAP_AUTHENTICATION_COOKIE_EXPIRE;

  public $apiPrefs = array();
  public $createLDAPAccounts; // should an drupal account be created when an ldap user authenticates
  public $createLDAPAccountsAdminApproval; // create them, but as blocked accounts

  /**
   * Advanced options.   whitelist / blacklist options
   *
   * these are on the fuzzy line between authentication and authorization
   * and determine if a user is allowed to authenticate with ldap
   *
   */

  public $allowOnlyIfTextInDn = array(); // eg ou=education that must be met to allow ldap authentication
  public $excludeIfTextInDn = array();
  public $allowTestPhp = NULL; // code that returns boolean TRUE || FALSE for allowing ldap authentication
  public $excludeIfNoAuthorizations = LDAP_AUTHENTICATION_EXCL_IF_NO_AUTHZ_DEFAULT;

  public $saveable = array(
    'sids',
    'authenticationMode',
    'loginConflictResolve',
    'acctCreation',
    'loginUIUsernameTxt',
    'loginUIPasswordTxt',
    'ldapUserHelpLinkUrl',
    'ldapUserHelpLinkText',
    'emailOption',
    'emailUpdate',
    'allowOnlyIfTextInDn',
    'excludeIfTextInDn',
    'allowTestPhp',
    'excludeIfNoAuthorizations',
    'ssoRemoteUserStripDomainName',
    'seamlessLogin',
    'ldapImplementation',
    'cookieExpire',
  );

  /** are any ldap servers that are enabled associated with ldap authentication **/
  public function hasEnabledAuthenticationServers() {
    return !(count($this->enabledAuthenticationServers) == 0);
  }
  public function enabled_servers() {
    return $this->hasEnabledAuthenticationServers();
  }
  
  function __construct() {
    $this->load();
  }


  function load() {

    if ($saved = variable_get("ldap_authentication_conf", FALSE)) {
      $this->inDatabase = TRUE;
      foreach ($this->saveable as $property) {
        if (isset($saved[$property])) {
          $this->{$property} = $saved[$property];
        }
      }
      
      $enabled_ldap_servers = ldap_servers_get_servers(NULL, 'enabled');
      foreach ($this->sids as $sid => $enabled) {
        if ($enabled && isset($enabled_ldap_servers[$sid])) {
          $this->enabledAuthenticationServers[$sid] = $enabled_ldap_servers[$sid];
        }
      }
    }
    else {
      $this->inDatabase = FALSE;
    }

    $this->ssoEnabled = module_exists('ldap_sso');
    $this->apiPrefs['requireHttps'] = variable_get('ldap_servers_require_ssl_for_credentails', 1);
    $this->apiPrefs['encryption'] = variable_get('ldap_servers_encryption', LDAP_SERVERS_ENC_TYPE_CLEARTEXT);

    // determine account creation configuration
    $user_register = variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL);
    if ($this->acctCreation == LDAP_AUTHENTICATION_ACCT_CREATION_DEFAULT || $user_register == USER_REGISTER_VISITORS) {
      $this->createLDAPAccounts = TRUE;
      $this->createLDAPAccountsAdminApproval = FALSE;
    }
    elseif ($user_register == USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) {
      $this->createLDAPAccounts = FALSE;
      $this->createLDAPAccountsAdminApproval = TRUE;
    }
    else {
      $this->createLDAPAccounts = FALSE;
      $this->createLDAPAccountsAdminApproval = FALSE;
    }

  }

  /**
   * Destructor Method
   */
  function __destruct() {


  }


 /**
   * decide if a username is excluded or not
   *
   * return boolean
   */
  public function allowUser($name, $ldap_user_entry, $account_exists = NULL) {

    /**
     * do one of the exclude attribute pairs match
     */
    $exclude = FALSE;
    
    // if user does not already exists and deferring to user settings AND user settings only allow 
    $user_register = variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL);
    if (!$account_exists && $this->acctCreation == LDAP_AUTHENTICATION_ACCT_CREATION_USER_SETTINGS_FOR_LDAP && $user_register == USER_REGISTER_ADMINISTRATORS_ONLY) {
      return FALSE;
    }
    
    foreach ($this->excludeIfTextInDn as $test) {
      if (stripos($ldap_user_entry['dn'], $test) !== FALSE) {
        return FALSE;//  if a match, return FALSE;
      }
    }


    /**
     * evaluate php if it exists
     */
    if ($this->allowTestPhp) {
      if (module_exists('php')) {
        global $_name, $_ldap_user_entry;
        $_name = $name;
        $_ldap_user_entry = $ldap_user_entry;
        $code = '<?php ' . "global \$_name; \n  global \$_ldap_user_entry; \n" . $this->allowTestPhp . ' ?>';
        $code_result = php_eval($code);
        $_name = NULL;
        $_ldap_user_entry = NULL;
        if ((boolean)($code_result) == FALSE) {
          return FALSE;
        }
      }
      else {
        drupal_set_message(t(LDAP_AUTHENTICATION_DISABLED_FOR_BAD_CONF_MSG), 'warning');
        $tokens = array('!ldap_authentication_config' => l(t('LDAP Authentication Configuration'), 'admin/config/people/ldap/authentication'));
        watchdog('ldap_authentication', 'LDAP Authentication is configured to deny users based on php execution with php_eval function, but php module is not enabled. Please enable php module or remove php code at !ldap_authentication_config .', $tokens);
        return FALSE;
      }
    }

    /**
     * do one of the allow attribute pairs match
     */
    if (count($this->allowOnlyIfTextInDn)) {
      $fail = TRUE;
      foreach ($this->allowOnlyIfTextInDn as $test) {
        if (stripos($ldap_user_entry['dn'], $test) !== FALSE) {
          $fail = FALSE;
        }
      }
      if ($fail) {
        return FALSE;
      }

    }
    /**
     * is excludeIfNoAuthorizations option enabled and user not granted any groups
     */

    if ($this->excludeIfNoAuthorizations) {
      if (!module_exists('ldap_authorization')) {
        drupal_set_message(t(LDAP_AUTHENTICATION_DISABLED_FOR_BAD_CONF_MSG), 'warning');
        $tokens = array('!ldap_authentication_config' => l(t('LDAP Authentication Configuration'), 'admin/config/people/ldap/authentication'));
        watchdog('warning', 'LDAP Authentication is configured to deny users without LDAP Authorization mappings, but LDAP Authorization module is not enabled.  Please enable and configure LDAP Authorization or disable this option at !ldap_authentication_config .', $tokens);
        return FALSE;
      }
      $user = new stdClass();
      $user->name = $name;
      $user->ldap_authenticated = TRUE; // fake user property added for query
      $consumers = ldap_authorization_get_consumers();
      $has_enabled_consumers = FALSE;

      foreach ($consumers as $consumer_type => $consumer_config) {
        $consumer_obj = ldap_authorization_get_consumer_object($consumer_type);
        if ($consumer_obj->consumerConf->status) {
          $has_enabled_consumers = TRUE;
          list($authorizations, $notifications) = ldap_authorizations_user_authorizations($user, 'query', $consumer_type, 'test_if_authorizations_granted');
          if (count(array_filter(array_values($authorizations))) > 0) {
            return TRUE;
          }
        }
      }

      if (!$has_enabled_consumers) {
        drupal_set_message(t(LDAP_AUTHENTICATION_DISABLED_FOR_BAD_CONF_MSG), 'warning');
        $tokens = array('!ldap_consumer_config' => l(t('LDAP Authorization Configuration'), 'admin/config/people/ldap/authorization'));
        watchdog('ldap_authentication', 'LDAP Authentication is configured to deny users without LDAP Authorization mappings, but 0 LDAP Authorization consumers are configured:  !ldap_consumer_config .', $tokens);
        return FALSE;
      }

      return FALSE;
    }

    // allow other modules to hook in and refuse if they like
    $hook_result = TRUE;
    drupal_alter('ldap_authentication_allowuser_results', $ldap_user_entry, $name, $hook_result);
    if (!$hook_result) {
      watchdog('ldap_authentication', "Authentication Allow User Result=refused for %name", array('%name' => $name), WATCHDOG_NOTICE);
      return FALSE;
    }

    /**
     * default to allowed
     */
    return TRUE;
  }


}
