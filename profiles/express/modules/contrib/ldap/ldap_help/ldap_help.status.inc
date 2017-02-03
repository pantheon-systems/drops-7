<?php
// $Id: ldap_help.status.inc,v 1.1.2.1 2011/02/08 06:14:20 johnbarclay Exp $
/**
 * @file
 * status file for ldaphelp module
 *
 */

function ldap_help_status($return = 'html') {

  drupal_add_css(drupal_get_path('module', 'system') . '/system.css', 'module', 'all', FALSE);
  include_once(drupal_get_path('module', 'system') . '/system.admin.inc');

  // server and LDAP Module Data
  $heading = "Server and LDAP Module";
  $key = 'server';
  $phpinfo = ldap_help_parsePHPModules();
  $status['server'] = ldap_help_get_server($phpinfo, $info);
  $reporting_text = _ldap_help_parse_status_to_text($status['server'], $heading);
  $content =  "<h3>$heading</h3>" . theme('status_report', array('requirements' => $status['server'])); //  . ldap_help_phpinfo();

  $status['user_settings'] = ldap_help_get_user_settings();
  $content .=  "<h3>User Settings</h3>" . theme('status_report', array('requirements' => $status['user_settings']));

  if (module_exists('ldap_servers')) {
    list($status['ldap_servers'], $servers) = ldap_help_get_ldap_servers();
    $content .=  "<h3>General Settings</h3>" . theme('status_report', array('requirements' => $status['ldap_servers']));

    foreach ($servers as $sid => $server) {
      $content .=  "<h3>Server: " . $server[1]['value'] . "</h3>" . theme('status_report', array('requirements' => $server));
    }
  }

  if (module_exists('ldap_authentication')) {
    $status['ldap_authentication'] = ldap_help_get_ldap_authentication();
    $content .=  "<h3>LDAP Authentication Settings</h3>" . theme('status_report', array('requirements' => $status['ldap_authentication']));
  }
  if (module_exists('ldap_authorization')) {
    $status['ldap_authorizations'] = ldap_help_get_ldap_authorization();
    if (count($status['ldap_authorizations'])) {
      foreach ($status['ldap_authorizations'] as $sid => $mapping) {
        $content .=  "<h3>LDAP Authorization Mapping</h3>" . theme('status_report', array('requirements' => $mapping));
      }
    }
  }

  if ($return == 'array') {
    return $status;
  }
  else {
    return $content;
  }

}
function ldap_help_get_server($phpinfo, &$info) {

  $info['phpversion'] = phpversion();
  $info['ldaploaded'] = extension_loaded('ldap');

  $info['ldap'] = $phpinfo['ldap'];

  $text = "";

  foreach ($phpinfo['ldap'] as $key => $value) {
    if (is_scalar($value)) {
      $text .= "<br/>$key: " . $value;
    }
    else {
      $text .= "<br/>$key: " . print_r($value, TRUE);
    }
  }
  $info['ldap']['text'] = $text;

  $modules = system_rebuild_module_data();
  $ldapmodules = array('ldap_servers', 'ldap_authentication', 'ldap_authorization',
    'ldap_authorization_drupal_role', 'ldap_authorization_og', 'ldap_feeds',
    'ldap_query', 'ldap_sso', 'ldap_views');

  foreach ($ldapmodules as $ldapmodule) {
    $data['status'] = $modules[$ldapmodule]->status;
    $data['schema_version'] = $modules[$ldapmodule]->schema_version;
    $data['version'] = ($modules[$ldapmodule]->info['version']) ? $modules[$ldapmodule]->info['version'] : 'unknown';
    $data['text'] = "status: " . $data['status']
      . ", schema_version: " . $data['schema_version']
      . ", v: " . $data['version'];

    $info[$ldapmodule] = $data;
  }

  // set status array to be converted into html table.
  if (isset($phpinfo['Apache Environment'])) {
    $status[] = array(
      'title' => 'Apache',
      'value' => $phpinfo['Apache Environment']['SERVER_SOFTWARE']
    );
  }
  elseif (isset($_SERVER["SERVER_SOFTWARE"])) {
        $status[] = array(
      'title' => 'SERVER_SOFTWARE',
      'value' => $_SERVER["SERVER_SOFTWARE"]
    );

  }


  $status[] = array(
    'title' => 'PHP version',
    'value' => phpversion()
  );




  if (! $info['ldaploaded']) {
    $status[] = array( 'title' => 'PHP ldap extension not loaded',
    'value' => l(t('PHP LDAP extension'), 'http://us2.php.net/ldap') . ' must be loaded for LDAP Integration to work.
    It comes compiled with most versions of PHP.'  ,
    'severity' => REQUIREMENT_ERROR);
  }
  else {
    $status[] = array( 'title' => 'PHP ldap extension data',
    'value' => $info['ldap']['text']  ,
    'severity' => 0);
  }

  $status[] = array(
    'title' => 'mcrypt extension loaded',
    'value' => (extension_loaded('mcrypt')) ? "yes" : "no",
  );

  $status[] = array(
    'title' => 'open ssl extension loaded',
    'value' => extension_loaded('openssl') ? "yes" : "no",
  );

  $status[] = array('title' => 'Drupal', 'value' => VERSION , 'severity' => "0");

  foreach ( $ldapmodules as $ldapmodule) {
    $status[] =  array('title' => $ldapmodule, 'value' => $info[$ldapmodule]['text'], 'severity' => "0");
  }
  return $status;
}

function ldap_help_get_ldapauth(&$info) {

  $status[] =  array(
    'title' => 'Authentication mode',
    value =>   $info['ldapauth']['login_process_text'] ,
  'severity' => "0");

  $status[] =  array(
    'title' => 'Conflict Resolve Feature',
    value =>   $info['ldapauth']['login_conflict_text'],
  'severity' => "0");

  $info['ldapauth']['ldapauth_disable_pass_change_text'] = (LDAPAUTH_FORGET_PASSWORDS == TRUE) ? "Do not " : "Do";
  $info['ldapauth']['ldapauth_disable_pass_change_text'] .= 'store users\' passwords during sessions.';

  $status[] =  array('title' => 'Security Options', value => $info['ldapauth']['ldapauth_disable_pass_change_text'], 'severity' => "0");

  $info['ldapauth']['ldapauth_disable_pass_change_text'] = (LDAPAUTH_DISABLE_PASS_CHANGE == TRUE) ? "R" : "Do not R";
  $info['ldapauth']['ldapauth_disable_pass_change_text'] .= 'emove password change fields from user edit form';
  $info['ldapauth']['ldapauth_alter_email_field_text'] = 'ldapauth_alter_email_field: ' . LDAPAUTH_ALTER_EMAIL_FIELD ;

  $status[] =  array('title' => 'LDAP UI Options', value => $info['ldapauth']['ldapauth_disable_pass_change_text'] . "<br/>" . $info['ldapauth']['ldapauth_alter_email_field_text'], 'severity' => "0");

  return $status;
}

function _ldap_help_parse_status_to_text($status, $heading, $replacements = array()) {

  $var_del = "\r\n------------------------------------------------\r\n";
  $section_del = "\r\n\r\n================================================\r\n";
  $name_val_del = ":\r\n";
  $lr = "\r\n";

  $replacements = array_merge($replacements, array(
  '<br/>' => $lr,
  '<ul>' => $lr,
  '</ul>' => $lr,
  '<li>' => $lr,
  '</li>' => "",
  "<code>" => "",
  "</code>" => "",
  "<strong>" => "",
  "</strong>" => ""
  ));


  $content = drupal_strtoupper($heading) . $lr  ;
  foreach ($status as $item) {
  //  $item['value'] = str_replace(array('<br/>','<ul>','</ul>','<li>','</li>',"<code>","</code>"),
   // array("\r\n","\r\n","\r\n","\r\n","\r\n"),$item['value'] );
    $item['value'] = str_replace(array_keys($replacements), array_values($replacements), $item['value'] );
    $item['title'] = str_replace(array_keys($replacements), array_values($replacements), $item['title'] );
    $content .=   $item['title'] . $name_val_del . $item['value'] . $var_del;
  }
  return $content;
}

function ldap_help_phpinfo() {
  ob_start();
  phpinfo();
  $s = ob_get_contents();
  ob_end_clean();
  return $s;
}
/** parse php modules from phpinfo */
function ldap_help_parsePHPModules() {
  ob_start();
  phpinfo();
  $s = ob_get_contents();
  ob_end_clean();

  $s = strip_tags($s, '<h2><th><td>');
  $s = preg_replace('/<th[^>]*>([^<]+)<\/th>/', "<info>\\1</info>", $s);
  $s = preg_replace('/<td[^>]*>([^<]+)<\/td>/', "<info>\\1</info>", $s);
  $vtmp = preg_split('/(<h2>[^<]+<\/h2>)/', $s, -1, PREG_SPLIT_DELIM_CAPTURE);
  $vmodules = array();
  for ($i=1; $i<count($vtmp); $i++) {
    if (preg_match('/<h2>([^<]+)<\/h2>/', $vtmp[$i], $vmat)) {
      $vname = trim($vmat[1]);
      $vtmp2 = explode("\n", $vtmp[$i+1]);
      foreach ($vtmp2 AS $vone) {
        $vpat = '<info>([^<]+)<\/info>';
        $vpat3 = "/$vpat\s*$vpat\s*$vpat/";
        $vpat2 = "/$vpat\s*$vpat/";
        if (preg_match($vpat3, $vone, $vmat)) { // 3cols
          $vmodules[$vname][trim($vmat[1])] = array(trim($vmat[2]), trim($vmat[3]));
        }
        elseif (preg_match($vpat2, $vone, $vmat)) { // 2cols
          $vmodules[$vname][trim($vmat[1])] = trim($vmat[2]);
        }
      }
    }
  }
  return $vmodules;
}

function ldap_help_get_user_settings() {

  $user_register = variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL);
  $options = array(
    USER_REGISTER_ADMINISTRATORS_ONLY => t('Administrators only'),
    USER_REGISTER_VISITORS => t('Visitors'),
    USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL => t('Visitors, but administrator approval is required'),
  );


    $status[] = array(
      'title' => 'Who can register accounts?',
      'value' => $options[$user_register],
    );

    return $status;

}

function ldap_help_get_ldap_servers() {
  if (!module_exists('ldap_servers')) {
    return FALSE;
  }

  ldap_server_module_load_include('inc', 'ldap_servers', 'ldap_servers.settings');
  ldap_server_module_load_include('inc', 'ldap_servers', 'ldap_servers.admin');

  $conf_form = ldap_servers_settings();

  $selected = @$conf_form['encryption']['ldap_servers_encryption']['#default_value'];
  $status[] = array(
      'title' => 'encryption',
      'value' => @$conf_form['encryption']['ldap_servers_encryption']['#options'][$selected],
    );

  $servers_objects = ldap_servers_get_servers(NULL, 'all');
  $servers = array();
  $field_to_properties = LdapServer::field_to_properties_map();
  $props = array_values($field_to_properties); // array('sid', 'name', 'status', 'ldap_type', 'port', 'tls', 'user_attr', 'mail_attr', 'user_dn_expression', 'ldap_to_drupal_user');
  foreach ($servers_objects as $sid => $server_object) {
    $server_admin = new LdapServerAdmin($sid);
    $form = $server_admin->drupalForm('update');

    foreach ($props as $property) {
      $value = print_r($server_object->{$property}, TRUE);

      if ($property == 'bind_method') {
        $value = $form['bind_method']['bind_method']['#options'][$value];
      }

      $servers[$sid][] = array(
          'title' => $property,
          'value' => $value,
      );
    }

  $ldaps = (strpos(drupal_strtolower($server_object->address), 'ldaps') !== FALSE) ?  'y' : 'n';
  $servers[$sid][] = array(
    'title' => 'ldaps in address',
    'value' => $ldaps,
    );

  $binddn = trim($server_object->binddn) ? 'yes' : 'no';
  $servers[$sid][] = array(
    'title' => 'binddn present',
    'value' => $binddn,
    );

  $basedn = count($server_object->basedn);
  $servers[$sid][] = array(
    'title' => 'basedn count',
    'value' => $basedn,
    );

  }

  return array($status, $servers);
}

function ldap_help_get_ldap_authentication() {

  ldap_server_module_load_include('php', 'ldap_authentication', 'LdapAuthenticationConfAdmin.class');
  $auth_conf = new LdapAuthenticationConfAdmin();
  $conf_form = $auth_conf->drupalForm();

  $items['logon']['authenticationMode'] = 'string';
  $items['logon']['authenticationServers'] = 'checkbox';
  $items['restrictions']['allowOnlyIfTextInDn'] = 'string';
  $items['restrictions']['excludeIfTextInDn'] = 'string';
  $items['restrictions']['allowTestPhp'] = 'string';
  $items['restrictions']['excludeIfNoAuthorizations '] = 'checkbox';
  $items['drupal_accounts']['loginConflictResolve'] = 'radios';
  $items['drupal_accounts']['acctCreation'] = 'radios';
  $items['email']['emailOption'] = 'radios';
  $items['email']['emailUpdate'] = 'radios';
  $items['sso']['ssoRemoteUserStripDomainName'] = 'checkbox';
  $items['sso']['seamlessLogin'] = 'checkbox';
  $items['sso']['cookieExpire '] = 'select';
  $items['sso']['ldapImplementation'] = 'select';

  $status = ldap_help_parse_form($conf_form, $items);

  return $status;
}

function ldap_help_get_ldap_authorization() {
  $status = array();
  ldap_server_module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerConfAdmin.class');
  ldap_server_module_load_include('inc', 'ldap_servers', 'ldap_servers.settings');
  ldap_server_module_load_include('inc', 'ldap_servers', 'ldap_servers.admin');

  $servers = ldap_servers_get_servers(NULL, 'enabled');
  $consumers = ldap_authorization_get_consumers();

  if (count($servers) == 0) {
    return array();
  }

  foreach ($servers as $sid => $server) {
    $server_options[$sid] = $server->name;
  }


  $items['status']['mapping_id'] = 'string';
  $items['status']['consumer_type'] = 'string';
  $items['status']['status'] = 'string';
  $items['status']['only_ldap_authenticated'] = 'string';


  $items['derive_from_dn']['derive_from_dn'] = 'string';
  $items['derive_from_dn']['derive_from_dn_attr'] = 'string';

  $items['derive_from_attr']['derive_from_attr'] = 'string';
  $items['derive_from_attr ']['derive_from_attr_attr'] = 'string';
  $items['derive_from_attr']['derive_from_attr_use_first_attr'] = 'checkbox';

  $items['derive_from_entry']['derive_from_entry'] = 'string';
  $items['derive_from_entry']['derive_from_entry_entries'] = 'string';
  $items['derive_from_entry']['derive_from_entry_attr'] = 'string';
  $items['derive_from_entry']['derive_from_entry_user_ldap_attr'] = 'string';
  $items['derive_from_entry']['derive_from_entry_nested'] = 'checkbox';
  $items['derive_from_entry']['derive_from_entry_use_first_attr'] = 'checkbox';

  // $items['filter_and_mappings']['mappings'] = 'string';
  $items['use_filter']['mappings'] = 'string';


  $items['misc_settings']['allow_synch_both_directions'] = 'string';
  $items['misc_settings']['synchronization_modes'] = 'string';
  $items['misc_settings']['synchronization_actions'] = 'string';

  foreach ($consumers as $consumer_type => $consumer) {
    $consumer_obj = ldap_authorization_get_consumer_object($consumer_type);
    $mapping_admin = new LdapAuthorizationConsumerConfAdmin($consumer_obj, FALSE);
    $authorization_form = $mapping_admin->drupalForm($server_options, 'edit');
    $status[$consumer_type] = ldap_help_parse_form($authorization_form, $items);
    $maps = $authorization_form['filter_and_mappings']['mappings']['#default_value'];
    $map_content = (trim($maps)) ? "mappings present" : "empty";
    if ($map_content == 'data present') {
      $map_content .= ($maps == drupal_strtolower($maps)) ? ', lower case' : ', mixed case';
    }
    $status[$consumer_type][] = array(
      'title' => 'mappings',
      'value' => $map_content,
    );

  }
  return $status;

}


function ldap_help_parse_form($form, $items) {

  foreach ($form as $fieldset_name => $field) {
    foreach ($field as $field_name => $field_data) {
      if (isset($items[$fieldset_name][$field_name])) {
        $value = "";
        if (!@is_array($field_data['#default_value'])) {
          $default_value = @$field_data['#default_value'];
        }
        else {
          $default_value = join(', ', array_values($field_data['#default_value']));
        }

        switch ($field_data['#type']) {
          case 'radios':
            $value = $field_data['#options'][$default_value];
            break;
          case 'select':
          case 'checkboxes':
            $value = $default_value;
            break;
          case 'checkbox':
            $value = (boolean)($field_data['#default_value']) ? 'y' : 'n';
            break;
          case 'hidden':
            $value = $field_data['#value'];
            break;
          case 'textarea':
          case 'textfield':

            $value = $default_value;
            break;
        }
        if ($items[$fieldset_name][$field_name] == 'empty') {
          $value = ((boolean)trim($value)) ?  'populated': 'empty';
        }
        $status[] = array(
          'title' => $field_name,
          'value' => $value,
        );

      }

    }

  }

  return $status;


}
