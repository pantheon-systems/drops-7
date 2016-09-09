<?php

/**
 * @file
 * theming functions for ldap_authentication module
 *
 */

/**
 * Returns HTML for user login block links.
 * @param $variables
 *   An associative array containing:
 *   - hide_reset_pwd (boolean) whether reset password link should be visible
 *   - auth_conf: object with ldap authentication configuration data
 *
 * @ingroup themeable
 */
function theme_ldap_authentication_user_login_block_links($variables) {
  extract($variables);

 // the code below modified from user.module user_login_block function
  $items = array();
  if (variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL)) {
    $items[] = l(t('Create new account'), 'user/register', array('attributes' => array('title' => t('Create a new user account.'))));
  }
  if ($show_reset_pwd) {
    $items[] = l(t('Request new password'), 'user/password', array('attributes' => array('title' => t('Request new password via e-mail.'))));
  }
  elseif ($auth_conf->ldapUserHelpLinkUrl) {
    $items[] = l(t($auth_conf->ldapUserHelpLinkText), $auth_conf->ldapUserHelpLinkUrl);
  }

  $output = theme('item_list', array('items' => $items));
  return $output;
}

/**
 * Returns HTML warning text for request new password/password reset form.
 * @param $variables
 *   An associative array containing:
 *   - auth_conf: object with ldap authentication configuration data
 *
 * @ingroup themeable
 */
function theme_ldap_authentication_user_pass_message($variables) {
  extract($variables);
  if ($auth_conf->authenticationMode == LDAP_AUTHENTICATION_EXCLUSIVE && $auth_conf->passwordOption != LDAP_AUTHENTICATION_PASSWORD_FIELD_ALLOW) {
    $msg = t('This page is only useful for the site administrator.  All other users
      need to reset their passwords');
    if ($auth_conf->ldapUserHelpLinkUrl) {
      $msg .=  ' ' . t('at') . ' ' . l(t($auth_conf->ldapUserHelpLinkText), $auth_conf->ldapUserHelpLinkUrl) . '.';
    }
    else {
      $msg .=  ' ' . t('with one of your organizations password management sites.');
    }
  }
  else { // mixed mode
    $msg = ""; // warning will come up on validation.  we do not know if the user is ldap authenticated or not until they submit form.
  }

  return $msg;
}

/**
 * Returns HTML warning text when an ldap authenticated user tries to reset their password.
 * @param $variables
 *   An associative array containing:
 *   - auth_conf: object with ldap authentication configuration data
 *   - account: user object
 *
 * @ingroup themeable
 */
function theme_ldap_authentication_user_pass_validate_ldap_authenticated($variables) {
  extract($variables);
  // already know user exists and is ldap authenticated

  if ($auth_conf->ldapUserHelpLinkUrl) {
    $msg = t('You may not reset your password here.  You must reset your password via the directions at')
    . ' ' . l(t($auth_conf->ldapUserHelpLinkText), $auth_conf->ldapUserHelpLinkUrl);
  }
  else {
    $msg = t('You may not reset your password here.  You must reset your password via one of your
      organization\'s password management sites.');
  }
  return $msg;
}


/**
 * The following three functions are theme callbacks for various messages
 * from NTLM/seamless login integration.
 *
 * Provides a theme callback for successful login messages. The reason for
 * using theme callbacks instead of a simple t() function is to provide the
 * ability to have more complex message handling performed; an example would
 * be to use the Real Name module to say "Welcome, User Name" upon successful
 * login.
 * @param $message
 *   A text string containing a translatable success message
 *
 * @ingroup themeable
 */
function theme_ldap_authentication_login_message($variables) {
  extract($variables);
  return $message;
}

/**
 * Provides a theme callback for user not found messages.
 * @param $message
 *   A text string containing a translatable "user not found" message
 *
 * @ingroup themeable
 */
function theme_ldap_authentication_message_not_found($variables) {
  extract($variables);
  return $message;
}

/**
 * Provides a theme callback for authentication failure messages.
 * @param $message
 *   A text string containing a translatable "authentication failure" message
 *
 * @ingroup themeable
 */
function theme_ldap_authentication_message_not_authenticated($variables) {
  extract($variables);
  return $message;
}
