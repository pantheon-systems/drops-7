<?php

error_reporting(E_ALL | E_STRICT);
set_time_limit (0);
ini_set("display_errors", 1);
ini_set("max_execution_time", 0);

define('LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT', 1);
define('LDAP_SERVERS_BIND_METHOD_ANON', 3);

define('LDAP_FAIL', -1);

define('LDAP_SUCCESS', 0x00);
define('LDAP_OPERATIONS_ERROR', 0x01);
define('LDAP_PROTOCOL_ERROR', 0x02);
define('LDAP_TIMELIMIT_EXCEEDED', 0x03);
define('LDAP_SIZELIMIT_EXCEEDED', 0x04);
define('LDAP_COMPARE_FALSE', 0x05);
define('LDAP_COMPARE_TRUE', 0x06);
define('LDAP_AUTH_METHOD_NOT_SUPPORTED', 0x07);
define('LDAP_STRONG_AUTH_REQUIRED', 0x08);
//NotusedinLDAPv3);
define('LDAP_PARTIAL_RESULTS', 0x09);

//Next5newinLDAPv3);
define('LDAP_REFERRAL', 0x0a);
define('LDAP_ADMINLIMIT_EXCEEDED', 0x0b);
define('LDAP_UNAVAILABLE_CRITICAL_EXTENSION', 0x0c);
define('LDAP_CONFIDENTIALITY_REQUIRED', 0x0d);
define('LDAP_SASL_BIND_INPROGRESS', 0x0e);

define('LDAP_NO_SUCH_ATTRIBUTE', 0x10);
define('LDAP_UNDEFINED_TYPE', 0x11);
define('LDAP_INAPPROPRIATE_MATCHING', 0x12);
define('LDAP_CONSTRAINT_VIOLATION', 0x13);
define('LDAP_TYPE_OR_VALUE_EXISTS', 0x14);
define('LDAP_INVALID_SYNTAX', 0x15);

define('LDAP_NO_SUCH_OBJECT', 0x20);
define('LDAP_ALIAS_PROBLEM', 0x21);
define('LDAP_INVALID_DN_SYNTAX', 0x22);

define('LDAP_IS_LEAF', 0x23);
define('LDAP_ALIAS_DEREF_PROBLEM', 0x24);

define('LDAP_INAPPROPRIATE_AUTH', 0x30);
define('LDAP_INVALID_CREDENTIALS', 0x31);
define('LDAP_INSUFFICIENT_ACCESS', 0x32);
define('LDAP_BUSY', 0x33);
define('LDAP_UNAVAILABLE', 0x34);
define('LDAP_UNWILLING_TO_PERFORM', 0x35);
define('LDAP_LOOP_DETECT', 0x36);

define('LDAP_SORT_CONTROL_MISSING', 0x3C);
define('LDAP_INDEX_RANGE_ERROR', 0x3D);

define('LDAP_NAMING_VIOLATION', 0x40);
define('LDAP_OBJECT_CLASS_VIOLATION', 0x41);
define('LDAP_NOT_ALLOWED_ON_NONLEAF', 0x42);
define('LDAP_NOT_ALLOWED_ON_RDN', 0x43);
define('LDAP_ALREADY_EXISTS', 0x44);
define('LDAP_NO_OBJECT_CLASS_MODS', 0x45);
define('LDAP_RESULTS_TOO_LARGE', 0x46);
//NexttwoforLDAPv3);
define('LDAP_AFFECTS_MULTIPLE_DSAS', 0x47);
define('LDAP_OTHER', 0x50);

//UsedbysomeAPIs);
define('LDAP_SERVER_DOWN', 0x51);
define('LDAP_LOCAL_ERROR', 0x52);
define('LDAP_ENCODING_ERROR', 0x53);
define('LDAP_DECODING_ERROR', 0x54);
define('LDAP_TIMEOUT', 0x55);
define('LDAP_AUTH_UNKNOWN', 0x56);
define('LDAP_FILTER_ERROR', 0x57);
define('LDAP_USER_CANCELLED', 0x58);
define('LDAP_PARAM_ERROR', 0x59);
define('LDAP_NO_MEMORY', 0x5a);

//PreliminaryLDAPv3codes);
define('LDAP_CONNECT_ERROR', 0x5b);
define('LDAP_NOT_SUPPORTED', 0x5c);
define('LDAP_CONTROL_NOT_FOUND', 0x5d);
define('LDAP_NO_RESULTS_RETURNED', 0x5e);
define('LDAP_MORE_RESULTS_TO_RETURN', 0x5f);
define('LDAP_CLIENT_LOOP', 0x60);
define('LDAP_REFERRAL_LIMIT_EXCEEDED', 0x61);


//error_reporting(E_ALL  &  ~E_NOTICE);

define('LDAP_SCRIPTS_COMMAND_LINE_WARNING', "Warning: PHP from the command line may have different PHP versions, php.ini files, and security context than running in a webserver context. This may produce false test results since Drupal LDAP Modules are run in the web server context.");

require_once('config.inc'); 

function ldap_help_connect($address, $port, $tls, $test = FALSE) {
    
  if ($test) {
    $false_con = ldap_connect("fakehostname.sdfserewerdfsdf.com", 389); // test for ldap extensions that don't actually connect until bind
    if (ldap_errno($false_con) == LDAP_SUCCESS) {
      $con = ldap_connect($address, $port);
      return array(LDAP_OTHER, "ldap_connect does not actually connect until bind with installed extension, so connect is not a valid test.", $con);
    }
  }
  
  $con = ldap_connect($address, $port);
  
  if (!$con || ldap_errno($con) != LDAP_SUCCESS) {
    $err = ldap_errno($con) . ":" . ldap_error($con) . ":" . ldap_err2str(ldap_errno($con)) .  "!";
    return array(LDAP_CONNECT_ERROR, "LDAP Connect failure to  $address : $port. $err");
  }

  // Use TLS if we are configured and able to.
  if ($tls) {
    ldap_get_option($con, LDAP_OPT_PROTOCOL_VERSION, $vers);
    if ($vers == -1) {
      return array(LDAP_PROTOCOL_ERROR, "Could not get LDAP protocol version.");
    }
    if ($vers != 3) {
      return array(LDAP_CONNECT_ERROR, 'Could not start TLS, only supported by LDAP v3.');
    }
    elseif (!function_exists('ldap_start_tls')) {
      return array(LDAP_CONNECT_ERROR, 'Could not start TLS. It does not seem to be supported by this PHP setup.');
    }
    elseif (!ldap_start_tls($con)) {
      return array(LDAP_CONNECT_ERROR, "Could not start TLS. (Error ". ldap_errno($con) . ":" . ldap_error($con) . ").");
    }
  }

  return array(LDAP_SUCCESS, "Successful Connection!", $con);
}

function ldap_help_show_error($con) {
   return "\nLDAP Error Number: " . ldap_errno($con) . "\nLDAP Error Description: " . ldap_error($con);
}

function ldap_help_display($title, $value = NULL) {
  if (is_array($value)) {
    echo "\n" . $title; 
    foreach ($title as $subtitle => $subvalue) {
      ldap_help_display($subtitle, $subvalue);
    }
  }
  if (!$title && $value) {
    echo "\n" . $value; 
  }
  elseif ((int)$title === $title) {
    echo "\n" . $value;
  }
  else {
    echo "\n" .  "$title: $value"; 
  }

}
/**
 * Disconnect (unbind) from an active LDAP server.
 */
function ldap_help_disconnect(&$con) {
  if (!$con) {
    // never bound or not currently bound, so no need to disconnect
  }
  else {
    ldap_help_disconnect($con);
    $con = NULL;
  }
}

/** parse php modules from phpinfo */
function ldap_help_parsePHPModules() {
  ob_start();
  phpinfo(INFO_MODULES);
  $s = ob_get_contents();
  ob_end_clean();
  $matches = array();
  preg_match_all("/(\nLDAP Support.*Vendor Version[^\n]*?).*$/iDsU", $s, $matches);
  return isset($matches[1][0]) ? "\nphpinfo() LDAP Info:" . $matches[1][0] : '';
}

function ldap_help_encodePassword($password) {
  $password="\"".$password."\"";
  $encoded="";
  for ($i=0; $i <strlen($password); $i++){ $encoded.="{$password{$i}}\000"; }
  return $encoded;
}

?>