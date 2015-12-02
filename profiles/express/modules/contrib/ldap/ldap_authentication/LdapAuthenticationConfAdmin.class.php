<?php
// $Id: LdapAuthenticationConfAdmin.class.php,v 1.4.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * This classextends by LdapAuthenticationConf for configuration and other admin functions
 */
module_load_include('php', 'ldap_authentication', 'LdapAuthenticationConf.class');

class LdapAuthenticationConfAdmin extends LdapAuthenticationConf {

  protected function setTranslatableProperties() {

    /**
     * 0.  Logon Options
     */

    $values['authenticationModeOptions']  = array(
      LDAP_AUTHENTICATION_MIXED => t('Mixed mode. Drupal authentication is tried first.  On failure, LDAP authentication is performed.'),
      LDAP_AUTHENTICATION_EXCLUSIVE => t('Only LDAP Authentication is allowed except for user 1.
        If selected, (1) reset password links will be replaced with links to ldap end user documentation below.
        (2) The reset password form will be left available at user/password for user 1; but no links to it
        will be provided to anonymous users.
        (3) Password fields in user profile form will be removed except for user 1.'),
      );

    $values['authenticationServersDescription'] = t('Check all LDAP server configurations to use in authentication.
     Each will be tested for authentication until successful or
     until each is exhausted.  In most cases only one server configuration is selected.');

    /**
     * 1.  User Login Interface
     */
    $values['loginUIUsernameTxtDescription'] = t('Text to be displayed to user below the username field of
     the user login screen.');

    $values['loginUIPasswordTxtDescription'] = t('Text to be displayed to user below the password field of
     the user login screen.');

    $values['ldapUserHelpLinkUrlDescription'] = t('URL to LDAP user help/documentation for users resetting
     passwords etc. Should be of form http://domain.com/. Could be the institutions ldap password support page
     or a page within this drupal site that is available to anonymous users.');

    $values['ldapUserHelpLinkTextDescription']  = t('Text for above link e.g. Account Help or Campus Password Help Page');


    /**
     * 2.  LDAP User Restrictions
     */

    $values['allowOnlyIfTextInDnDescription'] = t('A list of text such as ou=education
      or cn=barclay that at least one of be found in user\'s dn string.  Enter one per line
      such as <pre>ou=education') . "\n" . t('ou=engineering</pre>   This test will be case insensitive.');

    $values['excludeIfTextInDnDescription'] = t('A list of text such as ou=evil
      or cn=bad that if found in a user\'s dn, exclude them from ldap authentication.
      Enter one per line such as <pre>ou=evil') . "\n" . t('cn=bad</pre> This test will be case insensitive.');

    $values['allowTestPhpDescription'] = t('PHP code which should print 1
        for allowing ldap authentication or 0 for not allowed.  Available variables are:
        $_name and $_ldap_user_entry  See readme.txt for more info.');

    $values['excludeIfNoAuthorizationsDescription'] = t('If the user is not granted any drupal roles,
      organic groups, etc. by LDAP Authorization, login will be denied.  LDAP Authorization must be
      enabled for this to work.');



    /**
    * 3. Drupal Account Provisioning and Syncing
    */
    $values['loginConflictResolveDescription'] = t('What should be done if a local Drupal or other external
      authentication account already exists with the same login name.');
    $values['loginConflictOptions'] = array(
      LDAP_AUTHENTICATION_CONFLICT_LOG => t('Disallow login and log the conflict'),
      LDAP_AUTHENTICATION_CONFLICT_RESOLVE => t('Associate local account with the LDAP entry.  This option
      is useful for creating accounts and assigning roles before an ldap user authenticates.'),
      );


    $values['acctCreationOptions'] = array(
      LDAP_AUTHENTICATION_ACCT_CREATION_LDAP_BEHAVIOR => t('Create accounts automatically for ldap authenticated users.
        Account creation settings at /admin/config/people/accounts/settings will only affect non-ldap authenticated accounts.'),
      LDAP_AUTHENTICATION_ACCT_CREATION_USER_SETTINGS_FOR_LDAP => t('Use account creation policy
         at /admin/config/people/accounts/settings under for both Drupal and LDAP Authenticated users.
         "Visitors" option automatically creates and account when they successfully LDAP authenticate.
         "Admin" and "Admin with approval" do not allow user to authenticate until the account is approved.'),
      );


    /**
    * 4. Email
    */

    $values['emailOptionOptions'] = array(
      LDAP_AUTHENTICATION_EMAIL_FIELD_REMOVE => t('Don\'t show an email field on user forms.  LDAP derived email will be used for user and connot be changed by user'),
      LDAP_AUTHENTICATION_EMAIL_FIELD_DISABLE => t('Show disabled email field on user forms with LDAP derived email.  LDAP derived email will be used for user and connot be changed by user'),
      );

    $values['emailUpdateOptions'] = array(
      LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_ENABLE_NOTIFY => t('Update stored email if LDAP email differs at login and notify user.'),
      LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_ENABLE => t('Update stored email if LDAP email differs at login but don\'t notify user.'),
      LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_DISABLE => t('Don\'t update stored email if LDAP email differs at login.'),
      );


    /**
     * 5. Single Sign-On / Seamless Sign-On
     */

      $values['ldapImplementationOptions'] = array(
        'mod_auth_sspi' => t('mod_auth_sspi'),
        'mod_auth_kerb' => t('mod_auth_kerb'),
        );

      $values['cookieExpirePeriod'] = array(0 => t('Immediately')) +
        drupal_map_assoc(array(3600, 86400, 604800, 2592000, 31536000, 315360000), 'format_interval')
        + array(-1 => t('Never'));

      $values['ssoEnabledDescription'] = '<strong>' . t('Single Sign on is enabled.') .
        '</strong> ' . t('To disable it, disable the LDAP SSO Module on the ') .  l('Modules Form', 'admin/modules') . '.<p>' .
        t('Single Sign-On enables ' .
        'users of this site to be authenticated by visiting the URL ' .
        '"user/login/sso, or automatically if selecting "automated ' .
        'single sign-on" below. Set up of LDAP authentication must be ' .
        'performed on the web server. Please review the !readme file ' .
        'for more information.', array('!readme' =>
        l(t('README.txt'), drupal_get_path('module', 'ldap_sso') . '/README.txt')))
        . '</p>';

      $values['ssoRemoteUserStripDomainNameDescription'] = t('Useful when the ' .
        'WWW server provides authentication in the form of user@realm and you ' .
        'want to have both SSO and regular forms based authentication ' .
        'available. Otherwise duplicate accounts with conflicting e-mail ' .
        'addresses may be created.');
      $values['seamlessLogInDescription'] = t('This requires that you ' .
        'have operational NTLM or Kerberos authentication turned on for at least ' .
        'the path user/login/sso, or for the whole domain.');
      $values['cookieExpireDescription'] = t('If using the seamless login, a ' .
        'cookie is necessary to prevent automatic login after a user ' .
        'manually logs out. Select the lifetime of the cookie.');
      $values['ldapImplementationDescription'] = t('Select the type of ' .
        'authentication mechanism you are using.');

      foreach ($values as $property => $default_value) {
        $this->$property = $default_value;
      }
    }

  /**
   * 0.  Logon Options
   */
  public $authenticationModeDefault = LDAP_AUTHENTICATION_MIXED;
  public $authenticationModeOptions;

  protected $authenticationServersDescription;
  protected $authenticationServersOptions = array();

  /**
   * 1.  User Login Interface
   */
  protected $loginUIUsernameTxtDescription;
  protected $loginUIPasswordTxtDescription;
  protected $ldapUserHelpLinkUrlDescription;
  protected $ldapUserHelpLinkTextDescription;


  /**
   * 2.  LDAP User Restrictions
   */

  protected $allowOnlyIfTextInDnDescription;
  protected $excludeIfTextInDnDescription;
  protected $allowTestPhpDescription;

   /**
   * 3. Drupal Account Provisioning and Syncing
   */
  public $loginConflictResolveDescription;
  public $loginConflictResolveDefault = LDAP_AUTHENTICATION_CONFLICT_LOG; // LDAP_CONFLICT_RESOLVE;
  public $loginConflictOptions;

  public $acctCreationDescription = '';
  public $acctCreationDefault = LDAP_AUTHENTICATION_ACCT_CREATION_DEFAULT;
  public $acctCreationOptions;


   /**
   * 4. Email
   */

  public $emailOptionDefault = LDAP_AUTHENTICATION_EMAIL_FIELD_REMOVE;
  public $emailOptionOptions;

  public $emailUpdateDefault = LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_ENABLE_NOTIFY;
  public $emailUpdateOptions;


   /**
   * 5. Single Sign-On / Seamless Sign-On
   */

  public $ssoEnabledDescription;
  public $ssoRemoteUserStripDomainNameDescription;
  public $ldapImplementationOptions;
  public $cookieExpirePeriod;
  public $seamlessLogInDescription;
  public $cookieExpireDescription;
  public $ldapImplementationDescription;


  public $errorMsg = NULL;
  public $hasError = FALSE;
  public $errorName = NULL;

  public function clearError() {
    $this->hasError = FALSE;
    $this->errorMsg = NULL;
    $this->errorName = NULL;
  }

  public function save() {
    foreach ($this->saveable as $property) {
      $save[$property] = $this->{$property};
    }
    variable_set('ldap_authentication_conf', $save);
  }

  static public function getSaveableProperty($property) {
    $ldap_authentication_conf = variable_get('ldap_authentication_conf', array());
    return isset($ldap_authentication_conf[$property]) ? $ldap_authentication_conf[$property] : FALSE;

  }

  static public function uninstall() {
    variable_del('ldap_authentication_conf');
  }

  public function __construct() {
    parent::__construct();
    $this->setTranslatableProperties();
    if ($servers = ldap_servers_get_servers(NULL, 'enabled')) {
      foreach ($servers as $sid => $ldap_server) {
        $enabled = ($ldap_server->status) ? 'Enabled' : 'Disabled';
        $this->authenticationServersOptions[$sid] = $ldap_server->name . ' (' . $ldap_server->address . ') Status: ' . $enabled;
      }
    }
  }


  public function drupalForm() {

    if (count($this->authenticationServersOptions) == 0) {
      $message = ldap_servers_no_enabled_servers_msg('configure LDAP Authentication');
      $form['intro'] = array(
        '#type' => 'item',
        '#markup' => t('<h1>LDAP Authentication Settings</h1>') . $message,
      );
      return $form;
    }

    $tokens = array();  // not sure what the tokens would be for this form?

    $form['intro'] = array(
        '#type' => 'item',
        '#markup' => t('<h1>LDAP Authentication Settings</h1>'),
    );

    $form['logon'] = array(
      '#type' => 'fieldset',
      '#title' => t('Logon Options'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['logon']['authenticationMode'] = array(
      '#type' => 'radios',
      '#title' => t('Allowable Authentications'),
      '#required' => 1,
      '#default_value' => $this->authenticationMode,
      '#options' => $this->authenticationModeOptions,
    );


    $form['logon']['authenticationServers'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Authentication LDAP Server Configurations'),
      '#required' => FALSE,
      '#default_value' => $this->sids,
      '#options' => $this->authenticationServersOptions,
      '#description' => $this->authenticationServersDescription
    );

    $form['login_UI'] = array(
      '#type' => 'fieldset',
      '#title' => t('User Login Interface'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['login_UI']['loginUIUsernameTxt'] = array(
      '#type' => 'textfield',
      '#title' => t('Username Description Text'),
      '#required' => 0,
      '#default_value' => $this->loginUIUsernameTxt,
      '#description' => $this->loginUIUsernameTxtDescription,
    );

    $form['login_UI']['loginUIPasswordTxt'] = array(
      '#type' => 'textfield',
      '#title' => t('Password Description Text'),
      '#required' => 0,
      '#default_value' => $this->loginUIPasswordTxt,
      '#description' => $this->loginUIPasswordTxtDescription,
    );

    $form['login_UI']['ldapUserHelpLinkUrl'] = array(
      '#type' => 'textfield',
      '#title' => t('LDAP Account User Help URL'),
      '#required' => 0,
      '#default_value' => $this->ldapUserHelpLinkUrl,
      '#description' => $this->ldapUserHelpLinkUrlDescription,
    );


    $form['login_UI']['ldapUserHelpLinkText'] = array(
      '#type' => 'textfield',
      '#title' => t('LDAP Account User Help Link Text'),
      '#required' => 0,
      '#default_value' => $this->ldapUserHelpLinkText,
      '#description' => $this->ldapUserHelpLinkTextDescription,
    );

    $form['restrictions'] = array(
      '#type' => 'fieldset',
      '#title' => t('LDAP User "Whitelists" and Restrictions'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );


    $form['restrictions']['allowOnlyIfTextInDn'] = array(
      '#type' => 'textarea',
      '#title' => t('Allow Only Text Test'),
      '#default_value' => $this->arrayToLines($this->allowOnlyIfTextInDn),
      '#cols' => 50,
      '#rows' => 3,
      '#description' => t($this->allowOnlyIfTextInDnDescription, $tokens),
    );

    $form['restrictions']['excludeIfTextInDn'] = array(
      '#type' => 'textarea',
      '#title' => t('Excluded Text Test'),
      '#default_value' => $this->arrayToLines($this->excludeIfTextInDn),
      '#cols' => 50,
      '#rows' => 3,
      '#description' => t($this->excludeIfTextInDnDescription, $tokens),
    );

    $form['restrictions']['allowTestPhp'] = array(
      '#type' => 'textarea',
      '#title' => t('PHP to Test for Allowed LDAP Users'),
      '#default_value' => $this->allowTestPhp,
      '#cols' => 50,
      '#rows' => 3,
      '#description' => t($this->allowTestPhpDescription, $tokens),
      '#disabled' => (boolean)(!module_exists('php')),
    );
    if (!module_exists('php')) {
      $form['restrictions']['allowTestPhp']['#title'] .= ' <em>' . t('php module currently disabled') . '</em>';
    }


    $form['restrictions']['excludeIfNoAuthorizations'] = array(
      '#type' => 'checkbox',
      '#title' => t('New and lightly tested feature. Use with caution!  Requires LDAP Authorization to be enabled and configured.  Deny access to users without Ldap Authorization Module authorization mappings such as Drupal roles.'),
      '#default_value' =>  $this->excludeIfNoAuthorizations,
      '#description' => t($this->excludeIfNoAuthorizationsDescription, $tokens),
      '#disabled' => (boolean)(!module_exists('ldap_authorization')),
    );


    $form['drupal_accounts'] = array(
      '#type' => 'fieldset',
      '#title' => t('Drupal User Account Creation'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['drupal_accounts']['loginConflictResolve'] = array(
      '#type' => 'radios',
      '#title' => t('Existing Drupal User Account Conflict'),
      '#required' => 1,
      '#default_value' => $this->loginConflictResolve,
      '#options' => $this->loginConflictOptions,
      '#description' => t( $this->loginConflictResolveDescription),
    );


    $form['drupal_accounts']['acctCreation'] = array(
      '#type' => 'radios',
      '#title' => t('Account Creation for LDAP Authenticated Users'),
      '#required' => 1,
      '#default_value' => $this->acctCreation,
      '#options' => $this->acctCreationOptions,
      '#description' => t($this->acctCreationDescription),
    );

    $form['email'] = array(
      '#type' => 'fieldset',
      '#title' => t('Email'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['email']['emailOption'] = array(
      '#type' => 'radios',
      '#title' => t('Email Behavior'),
      '#required' => 1,
      '#default_value' => $this->emailOption,
      '#options' => $this->emailOptionOptions,
    );

    $form['email']['emailUpdate'] = array(
      '#type' => 'radios',
      '#title' => t('Email Update'),
      '#required' => 1,
      '#default_value' => $this->emailUpdate,
      '#options' => $this->emailUpdateOptions,
      );


    /**
     * Begin single sign-on settings
     */
    $form['sso'] = array(
      '#type' => 'fieldset',
      '#title' => t('Single Sign-On'),
      '#collapsible' => TRUE,
      '#collapsed' => (boolean)(!$this->ssoEnabled),
    );

/**
    $form['sso']['ssoEnabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Single Sign-On'),
      '#description' => t($this->ssoEnabledDescription),
      '#default_value' => $this->ssoEnabled,
      '#disabled' => (boolean)(!module_exists('ldap_sso')),
      );
**/
    if ($this->ssoEnabled) {

      $form['sso']['enabled'] = array(
        '#type' => 'markup',
        '#markup' => $this->ssoEnabledDescription,
      );

    }
    else {
      $form['sso']['disabled'] = array(
       '#type' => 'markup',
       '#markup' => '<p><em>' . t('LDAP Single Sign-On module must be enabled for options below to work.')
       . ' ' . t('It is currently disabled.')
        . ' ' .  l('Modules Form', 'admin/modules') . '</p></em>',
     );



    }
    $form['sso']['ssoRemoteUserStripDomainName'] = array(
      '#type' => 'checkbox',
      '#title' => t('Strip REMOTE_USER domain name'),
      '#description' => t($this->ssoRemoteUserStripDomainNameDescription),
      '#default_value' => $this->ssoRemoteUserStripDomainName,
      '#disabled' => (boolean)(!$this->ssoEnabled),
    );

    $form['sso']['seamlessLogin'] = array(
      '#type' => 'checkbox',
      '#title' => t('Turn on automated single sign-on'),
      '#description' => t($this->seamlessLogInDescription),
      '#default_value' => $this->seamlessLogin,
      '#disabled' => (boolean)(!$this->ssoEnabled),
      );

    $form['sso']['cookieExpire'] = array(
      '#type' => 'select',
      '#title' => t('Cookie Lifetime'),
      '#description' => t($this->cookieExpireDescription),
      '#default_value' => $this->cookieExpire,
      '#options' => $this->cookieExpirePeriod,
      '#disabled' => (boolean)(!$this->ssoEnabled),
    );

    $form['sso']['ldapImplementation'] = array(
      '#type' => 'select',
      '#title' => t('Authentication Mechanism'),
      '#description' => t($this->ldapImplementationDescription),
      '#default_value' => $this->ldapImplementation,
      '#options' => $this->ldapImplementationOptions,
      '#disabled' => (boolean)(!$this->ssoEnabled),
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
    );

  return $form;
}

/**
 * validate form, not object
 */
  public function drupalFormValidate($values)  {

    $this->populateFromDrupalForm($values);

    $errors = $this->validate();

    return $errors;
  }

/**
 * validate object, not form
 */
  public function validate() {
    $errors = array();

    $enabled_servers = ldap_servers_get_servers(NULL, 'enabled');
    if ($this->ssoEnabled) {
      foreach ($this->sids as $sid) {
        if ($enabled_servers[$sid]->bind_method == LDAP_SERVERS_BIND_METHOD_USER || $enabled_servers[$sid]->bind_method == LDAP_SERVERS_BIND_METHOD_ANON_USER) {
          $methods = array(
            LDAP_SERVERS_BIND_METHOD_USER => 'Bind with Users Credentials',
            LDAP_SERVERS_BIND_METHOD_ANON_USER => 'Anonymous Bind for search, then Bind with Users Credentials',
          );
          $tokens = array(
            '!edit' => l($enabled_servers[$sid]->name, LDAP_SERVERS_INDEX_BASE_PATH . '/edit/' . $sid),
            '%sid' => $sid,
            '%bind_method' => $methods[$enabled_servers[$sid]->bind_method],
          );

          $errors['ssoEnabled'] = t('Single Sign On is not valid with the server !edit (id=%sid) because that server configuration uses %bind_method.  Since the user\'s credentials are never available to this module with single sign on enabled, there is no way for the ldap module to bind to the ldap server with credentials.', $tokens);
        }
      }
    }
    return $errors;
  }

  protected function populateFromDrupalForm($values) {
    $this->authenticationMode = ($values['authenticationMode']) ? (int)$values['authenticationMode'] : NULL;
    $this->sids = $values['authenticationServers'];
    $this->allowOnlyIfTextInDn = $this->linesToArray($values['allowOnlyIfTextInDn']);
    $this->excludeIfTextInDn = $this->linesToArray($values['excludeIfTextInDn']);
    $this->allowTestPhp = $values['allowTestPhp'];
    $this->loginConflictResolve  = ($values['loginConflictResolve']) ? (int)$values['loginConflictResolve'] : NULL;
    $this->acctCreation  = ($values['acctCreation']) ? (int)$values['acctCreation'] : NULL;
    $this->loginUIUsernameTxt = ($values['loginUIUsernameTxt']) ? (string)$values['loginUIUsernameTxt'] : NULL;
    $this->loginUIPasswordTxt = ($values['loginUIPasswordTxt']) ? (string)$values['loginUIPasswordTxt'] : NULL;
    $this->ldapUserHelpLinkUrl = ($values['ldapUserHelpLinkUrl']) ? (string)$values['ldapUserHelpLinkUrl'] : NULL;
    $this->ldapUserHelpLinkText = ($values['ldapUserHelpLinkText']) ? (string)$values['ldapUserHelpLinkText'] : NULL;
    $this->excludeIfNoAuthorizations = ($values['excludeIfNoAuthorizations']) ? (int)$values['excludeIfNoAuthorizations'] : NULL;
    $this->emailOption  = ($values['emailOption']) ? (int)$values['emailOption'] : NULL;
    $this->emailUpdate  = ($values['emailUpdate']) ? (int)$values['emailUpdate'] : NULL;
   // $this->ssoEnabled = ($values['ssoEnabled']) ? (int)$values['ssoEnabled'] : NULL;
    $this->ssoRemoteUserStripDomainName = ($values['ssoRemoteUserStripDomainName']) ? (int)$values['ssoRemoteUserStripDomainName'] : NULL;
    $this->seamlessLogin = ($values['seamlessLogin']) ? (int)$values['seamlessLogin'] : NULL;
    $this->cookieExpire = ($values['cookieExpire']) ? (int)$values['cookieExpire'] : NULL;
    $this->ldapImplementation = ($values['ldapImplementation']) ? (string)$values['ldapImplementation'] : NULL;
  }

  public function drupalFormSubmit($values) {

    $this->populateFromDrupalForm($values);
    try {
        $save_result = $this->save();
    }
    catch (Exception $e) {
      $this->errorName = 'Save Error';
      $this->errorMsg = t('Failed to save object.  Your form data was not saved.');
      $this->hasError = TRUE;
    }

  }

  protected function arrayToLines($array) {
        $lines = "";
        if (is_array($array)) {
          $lines = join("\n", $array);
        }
        elseif (is_array(@unserialize($array))) {
          $lines = join("\n", unserialize($array));
        }
        return $lines;
      }

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

}
