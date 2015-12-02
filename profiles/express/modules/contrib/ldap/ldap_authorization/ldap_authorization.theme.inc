<?php
// $Id: ldap_authorization.theme.inc,v 1.4.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * theming functions for the LDAP authorization module.
 */

function theme_ldap_authorization_admin_index(&$variables) {
  $consumers = $variables['consumers'];

  $table = array(
    'header' =>  array(t('LDAP Server ID'), t('Description'), t('Module'), t('Consumer Type'), t('Enabled'), t('Operations')),
    'attributes' => array('id' => 'ldap_consumer_confs', 'class' => 'data'),
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => '',
    'caption' => t('LDAP Authorization Configurations'),
    'rows' => array(),
  );

  foreach ($consumers as $consumer_type => $consumer) {

    if ($consumer->consumerConf->inDatabase) {
      $admin = new LdapAuthorizationConsumerConfAdmin($consumer);
      $actions =  join(' | ', $admin->getLdapAuthorizationConsumerActions());
    }
    else {
      $actions =  l(t('add'), LDAP_SERVERS_MENU_BASE_PATH . '/authorization/add/' . $consumer->consumerType);
    }

    $table['rows'][] = array(
      $consumer->consumerConf->sid,
      $consumer->name,
      $consumer->consumerModule,
      $consumer_type,
      ($consumer->consumerConf->status) ? t('Yes') : t('No'),
      $actions
    );
  }
  return theme('table', $table);

}


function theme_ldap_authorization_test_results($variables) {

  $results = $variables['results'];
  $consumer = $variables['consumer'];
  $notifications = $variables['notifications'];
  $consumer_conf_link = l($consumer->consumerType, LDAP_SERVERS_MENU_BASE_PATH . '/authorization/edit/' . $consumer->consumerType);
  $server_link = l($consumer->consumerConf->sid, LDAP_SERVERS_MENU_BASE_PATH . '/servers/edit/' . $consumer->consumerConf->sid);

  $table = array(
    'header' => array(t('Drupal Username'), t('Authorization Type'), t('Authorization IDs'), t('Configuration'), t('LDAP Server Configuration')),
    'attributes' => array('id' => 'ldap_authorization_authorizations', 'class' => 'data'),
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => '',
    'caption' => t('LDAP Authorizations Test Results for consumer %consumer', array('%consumer' => $consumer->name)),
    'rows' => array(),
  );

  if (count($results)) {
    foreach ($results as $username => $user_results) {
      $row = array();
      if ($user = user_load_by_name($username)) {
        $username_link = l($username, 'user/' . $user->uid . '/edit');
      }
      foreach ($user_results as $consumer_type => $authorizations) {
        if (is_array($authorizations) && count($authorizations) > 0) {
          $authorizations = $consumer->convertToFriendlyAuthorizationIds($authorizations);
          $authorizations_text =  theme('item_list', array('items' => $authorizations, 'title' => NULL, 'type' => 'ul', 'attributes' => array()));
        }
        else {
          $authorizations_text = "";
        }
        $row = array($username, $consumer->name, $authorizations_text, $consumer_conf_link, $server_link);
        $table['rows'][] = $row;
      }

      foreach ($notifications[$username] as $consumer_type => $user_notifications) {
        $authorizations_text = "";
        if ($consumer_type == 'all') {
          $authorizations_text = ldap_authorization_map_errors($user_notifications, $consumer_conf_link);
        }
        elseif (is_array($user_notifications) && count($user_notifications) > 0) {
          foreach ($user_notifications as $i => $notification) {
            $authorizations_text .=  ldap_authorization_map_errors($notification, $consumer_conf_link);
          }
        }
        $row = array($username_link, $consumer->name, $authorizations_text, $consumer_conf_link, $server_link);
        $table['rows'][] = $row;
      }
    }
  }

  $output = theme('table', $table);

  return $output;

  }

function ldap_authorization_map_errors($err_id, $consumer_conf_link) {

  $tokens = array('%consumer_conf_link' => $consumer_conf_link);
  switch ($err_id) {

    case  LDAP_AUTHORIZATION_USER_LDAP_NOT_FOUND:
      $authorizations_text = t('LDAP entry for drupal user not found.', $tokens);
    break;

    case LDAP_AUTHORIZATION_USER_NOT_LDAP_AUTHENTICATED:
      $authorizations_text = t('LDAP Authorizations not applied because user is not
      authenticated via LDAP and configuration requires is (%consumer_conf_link).', $tokens);
    break;

    case LDAP_AUTHORIZATION_MAP_NOT_CONF_FOR_LOGON:
      $authorizations_text = t('LDAP Authorizations not configured to be executed on logon in  (%consumer_conf_link).', $tokens);
    break;

    case LDAP_AUTHORIZATION_NOT_APPLY_USER_1:
      $authorizations_text = t('LDAP Authorizations not applicable to user 1.', $tokens);
    break;

    case LDAP_AUTHORIZATION_SERVER_CONFIG_NOT_FOUND:
      $authorizations_text = t('Enabled LDAP server configuration not found for given ldap consumer type in (%consumer_conf_link).', $tokens);
    break;

    default:
      $authorizations_text = "Failed.";
  }

  return $authorizations_text;
}
