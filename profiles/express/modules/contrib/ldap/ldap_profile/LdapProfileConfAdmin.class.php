<?php

/**
 * @file
 * This classextends by LdapProfileConf for configuration and other admin functions
 */

require_once('LdapProfileConf.class.php');
class LdapProfileConfAdmin extends LdapProfileConf {

  // no need for LdapAuthenticationConf id as only one instance will exist per drupal install
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
    variable_set('ldap_profile_conf', $save);
  }

  static public function uninstall() {
    variable_del('ldap_profile_conf');
  }

  public function __construct() {
    parent::__construct();
  }


  public function drupalForm($accounts = array()) {

    if (count($this->servers) == 0) {
      $message = ldap_servers_no_enabled_servers_msg('configure LDAP Profiles');
      $form['intro'] = array(
        '#type' => 'item',
        '#markup' => t('<h1>LDAP Profile Settings</h1>') . $message,
      );
      return $form;
    }

    // grabs field information for a user account
    $fields = field_info_instances('user','user');
    $profileFields = array();
    foreach($fields as $key => $field) {
      $profileFields[$key] = $field['label'];
    }

    $form['intro'] = array(
        '#type' => 'item',
        '#markup' => t('<h1>LDAP Profile Settings</h1>'),
    );

    $form['defaultMaps'] = array(
      '#type' => 'fieldset',
      '#title' => 'Profile Fields Already Mapped to Ldap Fields',
      '#collapsible' => FALSE,
      '#collapsed' => false,
      '#tree' => true,
    );

    $user_attr = array();
    $mail_attr = array();
    $servers = ldap_servers_get_servers('','enabled');
    foreach($servers as $key => $server) {
      $user_attr[] = $server->user_attr;
      $mail_attr[] = $server->mail_attr;
    }
    $user_attr_display = (count($user_attr)) ? join(', ', $user_attr) : 'No Value Set';
    $mail_attr_display = (count($mail_attr)) ? join(', ', $mail_attr) : 'No Value Set';

    $form['defaultMaps']['username'] = array(
        '#type' => 'textfield',
        '#title' => 'UserName',
        '#default_value' => $user_attr_display,
        '#disabled' => true,
        '#description' => 'This must be altered in the ldap server configuration page',
    );
    $form['defaultMaps']['mail'] = array(
        '#type' => 'textfield',
        '#title' => 'Email',
        '#default_value' => $mail_attr_display,
        '#disabled' => true,
        '#description' => 'This must be altered in the ldap server configuration page',
    );

    if (count($this->servers)) {

      $form['tokens'] = array(
        '#type' => 'fieldset',
        '#title' => 'Sample User Values and Tokens',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => t('Below is a list of attributes for sample users for a given server.
          These may be used in the mappings below.  Singular attributes such as cn can be expressed
          as [cn] or cn.  This will be empty if the server does not have a sample user or
          uses a binding method other than service account or anonymous.'),
      );

      require_once(drupal_get_path('module','ldap_servers') . '/ldap_servers.functions.inc');
      foreach ($this->servers as $sid => $server) {
        if ($markup = ldap_servers_show_sample_user_tokens($sid)) {
           $form['tokens'][$sid] = array(
            '#type' => 'item',
            '#markup' => $markup,
          );
        }
      }
    }

    $form['mapping'] = array(
      '#type' => 'fieldset',
      '#title' => t('Profile Fields that need Mapped to Ldap Fields'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#tree' => true,
    );

    if (count($profileFields) == 0) {
         $form['mapping']['no_mappings'] = array(
           '#type' => 'item',
           '#title' => t('No custom User Fields Available'),
           '#markup' => t('Additional fields must be created on the user
              for mapping to work.  User fields are managed at: ') .
             l('admin/config/people/accounts/fields','admin/config/people/accounts/fields'),
        );
    }
    else {
      foreach($profileFields as $field => $label) {
        $mapping = $this->mapping;
        $derivedMapping = $this->derivedMapping;

        if(!empty($mapping) && array_key_exists($field,$mapping)) $default = $mapping[$field];
        else $default = '';
        $form['mapping'][$field] = array(
           '#type' => 'fieldset',
           '#title' => $label . t(' Profile Field to LDAP Field Mapping'),
           '#collapsible' => TRUE,
           '#collapsed' => FALSE,
        );
        $form['mapping'][$field]['ldap'] = array(
          '#type' => 'textfield',
          '#title' => $label,
          '#default_value' => $default,
        );
        if(!empty($derivedMapping) && array_key_exists($field,$derivedMapping) && array_key_exists('derive',$derivedMapping[$field])) $default = $derivedMapping[$field]['derive'];
        else $default = '';
        $form['mapping'][$field]['derive'] = array(
           '#type' => 'checkbox',
           '#title' => t('Derive from DN Search'),
           '#default_value' =>  $default,
        );
        if(!empty($derivedMapping) && array_key_exists($field,$derivedMapping) && array_key_exists('derive_value',$derivedMapping[$field])) $default = $derivedMapping[$field]['derive_value'];
        else $default = '';
        $form['mapping'][$field]['derive_value'] = array(
           '#type' => 'textfield',
           '#title' => t('LDAP Field to Derive from'),
           '#default_value' =>  $default,
         );
      }
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Update',
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

    return $errors;
  }

  protected function populateFromDrupalForm($values) {
    $this->ldap_fields = array();
    $this->mapping = array();
    foreach($values['defaultMaps'] as $field => $value) {
      if($value != '') {
        //store value in lower case to fix a ldap searching bug
        $l_value = strtolower($value);
        $this->mapping[$field] = $l_value;
        // don't add duplicates & ignore case
        if(!in_array($l_value, array_map('strtolower', $this->ldap_fields))) {
          $this->ldap_fields[] = $l_value;
        }
      }
    }
    if (isset($values['mapping']) && is_array($values['mapping'])) {
      foreach(array_keys($values['mapping']) as $field) {
        if($values['mapping'][$field]['ldap'] != '') {
          //store value in lower case to fix a ldap searching bug
          $l_value = strtolower($values['mapping'][$field]['ldap']);
          $this->mapping[$field] = $l_value;
          if((bool)($values['mapping'][$field]['derive']) && $values['mapping'][$field]['derive_value'] != '') {
              $l_value = strtolower($values['mapping'][$field]['derive_value']);
              $this->derivedMapping[$field]['derive'] = TRUE;
              $this->derivedMapping[$field]['derive_value'] = $l_value;
          } else {
              $this->derivedMapping[$field]['derive'] = FALSE;
              $this->derivedMapping[$field]['derive_value'] = '';
          }
        }
      }
    }
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

}
