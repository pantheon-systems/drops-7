<?php

/**
 * @file
 * Admin include file for admin settings form.
 */

/**
 * Implements settings for the module.
 */
function simplesamlphp_auth_settings() {
  global $_simplesamlphp_auth_saml_version, $base_url;

  if (!empty($_simplesamlphp_auth_saml_version)) {
    $ver = explode('.', $_simplesamlphp_auth_saml_version);
    if (!($ver[0] >= 1 && $ver[1] >= 5)) {
      drupal_set_message(t("Please upgrade SimpleSAMLphp. You are using %ssp_version", array('%ssp_version' => $_simplesamlphp_auth_saml_version)), 'warning');
    }
  }

  $roles = user_roles(TRUE);

  $form['simplesamlphp_auth_grp_setup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic Setup'),
    '#collapsible' => FALSE,
  );
  $form['simplesamlphp_auth_grp_setup']['simplesamlphp_auth_activate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Activate authentication via SimpleSAMLphp'),
    '#default_value' => variable_get('simplesamlphp_auth_activate', FALSE),
    '#description' => t('Checking this box before configuring the module could lock you out of Drupal.'),
  );
  $form['simplesamlphp_auth_grp_setup']['simplesamlphp_auth_installdir'] = array(
    '#type' => 'textfield',
    '#title' => t('Installation directory (default: /var/simplesamlphp)'),
    '#default_value' => variable_get('simplesamlphp_auth_installdir', '/var/simplesamlphp'),
    '#description' => t('The base directory of simpleSAMLphp. Absolute path with no trailing slash.'),
  );
  $form['simplesamlphp_auth_grp_setup']['simplesamlphp_auth_authsource'] = array(
    '#type' => 'textfield',
    '#title' => t('Authenticaton source for this SP (default: default-sp)'),
    '#default_value' => variable_get('simplesamlphp_auth_authsource', 'default-sp'),
    '#description' => t('The name of the source to use from /var/simplesamlphp/config/authsources.php'),
  );
  $form['simplesamlphp_auth_grp_setup']['simplesamlphp_auth_forcehttps'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force https for login links'),
    '#default_value' => variable_get('simplesamlphp_auth_forcehttps', TRUE),
    '#description' => t('Should be enabled on production sites.'),
  );

  $form['simplesamlphp_auth_grp_user'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Info and Syncing'),
    '#collapsible' => FALSE,
  );
  $form['simplesamlphp_auth_grp_user']['simplesamlphp_auth_user_name'] = array(
    '#type' => 'textfield',
    '#title' => t("Which attribute from simpleSAMLphp should be used as user's name"),
    '#default_value' => variable_get('simplesamlphp_auth_user_name', 'eduPersonPrincipalName'),
    '#description' => t('Example: <i>eduPersonPrincipalName</i> or <i>displayName</i><br />If the attribute is multivalued, the first value will be used.'),
    '#required' => TRUE,
  );
  $form['simplesamlphp_auth_grp_user']['simplesamlphp_auth_unique_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Which attribute from simpleSAMLphp should be used as unique identifier for the user'),
    '#default_value' => variable_get('simplesamlphp_auth_unique_id', 'eduPersonPrincipalName'),
    '#description' => t('Example: <i>eduPersonPrincipalName</i> or <i>eduPersonTargetedID</i><br />If the attribute is multivalued, the first value will be used.'),
    '#required' => TRUE,
  );
  $form['simplesamlphp_auth_grp_user']['simplesamlphp_auth_mailattr'] = array(
    '#type' => 'textfield',
    '#title' => t('Which attribute from simpleSAMLphp should be used as user mail address'),
    '#default_value' => variable_get('simplesamlphp_auth_mailattr', 'mail'),
    '#description' => t('Example: <i>mail</i><br />If the user attribute is multivalued, the first value will be used.'),
  );
  $form['simplesamlphp_auth_grp_user']['simplesamlphp_auth_rolepopulation'] = array(
    '#type' => 'textarea',
    '#title' => t('Automatic role population from simpleSAMLphp attributes'),
    '#default_value' => variable_get('simplesamlphp_auth_rolepopulation', ''),
    '#description' => t('A pipe separated list of rules.<br />Example: <i>roleid1:condition1|roleid2:contition2...</i> <br />For instance: <i>1:eduPersonPrincipalName,@=,uninett.no;affiliation,=,employee|2:mail,=,andreas@uninett.no</i>,3:mail,~=,andre'),
  );
  $form['simplesamlphp_auth_grp_user']['simplesamlphp_auth_roleevaleverytime'] = array(
    '#type' => 'checkbox',
    '#title' => t('Reevaluate roles every time the user logs in.'),
    '#default_value' => variable_get('simplesamlphp_auth_roleevaleverytime', 0),
    '#description' => t('NOTE: This means users could loose any roles that have been assigned manually in Drupal.'),
  );

  $form['simplesamlphp_auth_grp_reg'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Provisioning'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['simplesamlphp_auth_grp_reg']['simplesamlphp_auth_registerusers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Register users (i.e., auto-provisioning)'),
    '#default_value' => variable_get('simplesamlphp_auth_registerusers', TRUE),
    '#description' => t("Determines wether or not the module should automatically create/register new Drupal accounts for users that authenticate using SimpleSAMLphp. Unless you've done some custom work to provision Drupal accounts with the necessary authmap entries you will want this checked. NOTE: If unchecked each user must already have been provisioned a Drupal account with an appropriate entry in the authmap table before logging in. Otherwise they will receive a notice and be denied access. Be aware that simply creating a Drupal account will not create the necessary entry in the authmap table."),
  );

  $form['simplesamlphp_auth_grp_auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Drupal Authentication'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['simplesamlphp_auth_grp_auth']['simplesamlphp_auth_allowsetdrupalpwd'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow SAML users to set Drupal passwords.'),
    '#default_value' => variable_get('simplesamlphp_auth_allowsetdrupalpwd', FALSE),
    '#description' => t('Check this box if you want to let people set passwords for their local Drupal accounts. This will allow users to log in using either SAML or a local Drupal account. Disabling this removes the password change fields from the user profile form.<br/>NOTE: In order for them to login using their local Drupal password you must allow local logins with the settings below.'),
  );
  $form['simplesamlphp_auth_grp_auth']['simplesamlphp_auth_allowdefaultlogin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow authentication with local Drupal accounts'),
    '#default_value' => variable_get('simplesamlphp_auth_allowdefaultlogin', TRUE),
    '#description' => t('Check this box if you want to let people log in with local Drupal accounts (without using simpleSAMLphp). If you want to restrict this privilege to certain users you can enter the Drupal user IDs in the field below.'),
  );
  $form['simplesamlphp_auth_grp_auth']['simplesamlphp_auth_allowdefaultloginroles'] = array(
    '#type' => 'select',
    '#size' => 3,
    '#options' => $roles,
    '#multiple' => TRUE,
    '#title' => t('Which ROLES should be allowed to login with local accounts?'),
    '#default_value' => variable_get('simplesamlphp_auth_allowdefaultloginroles', array()),
    '#description' => t('Roles that should be allowed to login without simpleSAMLphp. Examples are dev/admin roles or guest roles.'),
  );
  $form['simplesamlphp_auth_grp_auth']['simplesamlphp_auth_allowdefaultloginusers'] = array(
    '#type' => 'textfield',
    '#title' => t('Which users should be allowed to login with local accounts?'),
    '#default_value' => variable_get('simplesamlphp_auth_allowdefaultloginusers', ''),
    '#description' => t('Example: <i>1,2,3</i><br />A comma-separated list of user IDs that should be allowed to login without simpleSAMLphp. If left blank, all local accounts can login.'),
  );
  $form['simplesamlphp_auth_grp_auth']['simplesamlphp_auth_logoutgotourl'] = array(
    '#type' => 'textfield',
    '#title' => t('Optionally, specify a URL for users to go to after logging out'),
    '#default_value' => variable_get('simplesamlphp_auth_logoutgotourl', ''),
    '#description' => t('Example: @base_url', array('@base_url' => $base_url)),
  );

  return system_settings_form($form);
}
