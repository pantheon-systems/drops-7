<?php
// $Id: ldap_servers.theme.inc,v 1.4.2.1 2011/02/08 20:05:41 johnbarclay Exp $


/**
 * @file
 * theming functions for ldap_servers module
 *
 */

/**
 * Returns HTML for ldap servers list.
 *
 * @param $variables
 *   An associative array containing:
 *   - ldap_servers: an array of one or more ldap server configurations.
 *   - actions:  true or false indicating include update, delete, etc. links
 *   - type:  'table', 'list', etc for style to render
 *
 * @ingroup themeable
 */
function theme_ldap_servers_list($variables) {
  extract($variables);

  $table = array(
    'header' => array(t('Name'), t('Type'), t('Enabled'), t('Server Address')),
    'attributes' => array('id' => 'ldap_servers_servers', 'class' => 'data'),
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => '',
    'caption' => 'LDAP Server Configurations',
  );

  if ($actions) {
    $table['header'][] = "Operations";
  }
  if (count($ldap_servers)) {
    foreach ($ldap_servers as $sid => $ldap_server ) {
      $row =  array(
        $ldap_server->name,
        $ldap_server->ldap_type,
        ($ldap_server->status == 1) ? "Yes" : "No",
        $ldap_server->address
          );
      if ($actions) {
        $admin = new LdapServerAdmin($ldap_server->sid);
        $row[] = join(' | ', $admin->getLdapServerActions());
      }
      $table['rows'][] = $row;
    }
  }
  else {
    $table['rows'] = array();
  }
  $output = theme('table', $table);

  return $output;
}

/**
 * Returns HTML for ldap server.
 *
 * @param $variables
 *   An associative array containing:
 *   - ldap_servers_config: an array of one or more ldap server configurations.
 *   - actions:  true or false indicating include update, delete, etc. links
 *   - type:  'table', 'list', etc for style to render
 *
 * @ingroup themeable
 */

function theme_ldap_servers_server($variables) {
  extract($variables);  // $ldap_server, $actions, $type (see above)

  ldap_server_module_load_include('php', 'ldap_servers', 'LdapServer.class');
  $properties = array();
  foreach (LdapServer::field_to_properties_map() as $field_name => $property_name) {
    $properties[] = "$field_name = " . print_r($ldap_server->$property_name, TRUE);
  }

  if ($actions) {
    $admin = new LdapServerAdmin($ldap_server->sid);
    $properties = join(' | ', $admin->getLdapServerActions());
  }

  $output = theme_item_list(
    array(
      'items' => $properties,
      'type' => 'ul',
      'title' => 'Server Properties',
      'attributes' => array()
      )
    );

  return $output;
}

function theme_ldap_servers_https_required($vars) {

  if (!isset($vars['site_contact_link']) || empty($vars['site_contact_link'])) {
    $vars['site_contact_link'] = 'site admin';
  }
  return t("You are accessing site_name using an unencrypted connection. For your security,
     site_name only supports account logins using a secure protocol such as HTTPS. You can switch
     to HTTPS by trying to view this page again after changing the URL in your browser's
     location bar to begin with \"https\" instead of \"http\". Please contact
      site_contact_link for help if this error continues.", $vars);

}

function theme_ldap_server_token_table($variables) {
  $header = array(
    array('data' => 'Token', 'sort' => 'asc'),
    array('data' => 'Value'),
  );
  foreach ($variables['tokens'] as $token => $value) {
    $rows[] = array('data' => array($token, $value));
  }
  $token_table = '<div class="content"><h2>' . t('Available Tokens') . '</h2>' .
    theme_table(array('colgroups' => NULL, 'caption' => 'tokens', 'header' => $header, 'rows' => $rows, 'sticky' => TRUE, 'attributes' => array(), 'empty' => 'No data')) .
    '</div>';

  return $token_table;
}

function theme_ldap_server_ldap_entry_table($variables) {

  if (!isset($variables['entry']) || !is_array($variables['entry'])) {
    return '<p>' . t('No Entry Returned.') . t('</p>');
  }

  $header = array('Attribute Name', 'Instance', 'Value', 'token');
  $rows = array();
  foreach ($variables['entry'] as $key => $value) {
    if (is_numeric($key) || $key == 'count') {
    }
    elseif (count($value) > 1) {
      $count = (int)$value['count'];
      foreach ($value as $i => $value2) {

        if ((string)$i == 'count') {
          continue;
        }
        elseif ($i == 0 && $count == 1) {
          $token = LDAP_SERVERS_TOKEN_PRE . $key . LDAP_SERVERS_TOKEN_POST;
        }
        elseif ($i == 0 && $count > 1) {
          $token = LDAP_SERVERS_TOKEN_PRE . $key . LDAP_SERVERS_TOKEN_DEL . '0' . LDAP_SERVERS_TOKEN_POST;
        }
        elseif (($i == $count - 1) && $count > 1) {
          $token = LDAP_SERVERS_TOKEN_PRE . $key . LDAP_SERVERS_TOKEN_DEL . 'last' . LDAP_SERVERS_TOKEN_POST;
        }
        elseif ($count > 1) {
          $token = LDAP_SERVERS_TOKEN_PRE . $key . LDAP_SERVERS_TOKEN_DEL . $i . LDAP_SERVERS_TOKEN_POST;
        }
        else {
          $token = "";
        }
        $rows[] = array('data' => array($key, $i, $value2, $token));
      }
    }
  }
  $entry_table = '<div class="content"><h2>' . t('LDAP Entry for %username', array('%username' => $variables['username'])) . '</h2>' . theme('table', array('colgroups' => NULL, 'caption' => 'ldap entry array', 'header' => $header, 'rows' => $rows, 'sticky' => TRUE, 'attributes' => array(), 'empty' => 'No data')) . '</div>';
  return $entry_table;
}
