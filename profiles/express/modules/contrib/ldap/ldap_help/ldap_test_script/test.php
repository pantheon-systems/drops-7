<?php


/**
 * @file
 * test script functions for testing ldap functionality outside of Drupal
 * see README.txt for instructions
 */


require_once('functions.inc');

$config = ldap_help_config();

ldap_help_display(NULL, "------------------------------------------\n");
ldap_help_display(NULL, LDAP_SCRIPTS_COMMAND_LINE_WARNING . "\n");

ldap_help_display(NULL, "------------------------------------------\nldap extension test\n------------------------------------------");
ldap_help_display("PHP Version", phpversion());
ldap_help_display('LDAP Extension Loaded', (extension_loaded('ldap')) ? 'yes' : 'no');
ldap_help_display(NULL, ldap_help_parsePHPModules());

if (!extension_loaded('ldap')) {
  die('PHP LDAP extension not loaded.  Can not run tests.  Check your php.ini and make sure ldap extension is avaialable.');
}

foreach ($config['servers'] as $sid => $server) {

  /**
   * Test LDAP Connect
   */
  $results = ldap_help_connect($server['server_address'], $server['server_port'], $server['server_tls'], TRUE);
  $test_name = "\"" . $sid . "\"";

  ldap_help_display(NULL, "------------------------------------------\n$test_name connect\n------------------------------------------");
  $tls = ($server['server_tls']) ? 'yes' : 'no';
  ldap_help_display('tls', $tls);

  $anon_bind = ($server['server_bind_method'] == LDAP_SERVERS_BIND_METHOD_ANON);
  $anon_bind_text = ($anon_bind) ? 'yes' : 'no';
  ldap_help_display('anonymous bind', $anon_bind_text);

  ldap_help_display('connect result', $results[1]);
  ldap_help_display('connect context', join("", array("server: ", $server['server_address'], ", port: ", $server['server_port'], ", tls= $tls")));
  $con = FALSE;
  if ($results[0] == LDAP_SUCCESS) {
    $con = $results[2];
  }
  elseif ($results[0] == LDAP_OTHER) {
    $con = $results[2];
  }
  else {
    ldap_help_disconnect($con);
    continue;
  }

  /**
   * Test LDAP Bind
   */

  ldap_help_display(NULL, "------------------------------------------\n$test_name bind\n------------------------------------------");

  if ($anon_bind) {
    if (@!ldap_bind($con)) {
      $results = array(ldap_errno($con), "LDAP anonymous bind error." . ldap_help_show_error($con));
    }
  }
  else {
    $bind_result = @ldap_bind($con, $server['server_bind_dn'], $server['server_bind_pw']);
    if (!$bind_result) {
      $results = array(ldap_errno($con), "LDAP bind failure for user " . $server['server_bind_dn'] . "." . ldap_help_show_error($con));
    }
    else {
      $results = array(LDAP_SUCCESS, "LDAP bind success.");
    }
  }

  ldap_help_display('bind result', $results[1]);
  ldap_help_display('bind dn', $server['server_bind_dn']);
  if ($results[0] != LDAP_SUCCESS) {
    continue;
  }

  /**
   * Test LDAP Queries
   */
  foreach ($server['test_queries'] as $query_id => $query) {
    ldap_help_display(NULL, "------------------------------------------\n$test_name query \"$query_id\" \n------------------------------------------");

    $filter = $query['filter'];
    ldap_help_display('search base_dn', $server['server_base_dn']);
    ldap_help_display('search filter', $filter);
    ldap_help_display('server_address', $server['server_address']);
    ldap_help_display('server_port', $server['server_port']);
    ldap_help_display('tls', $tls);

    $query_result = @ldap_search($con, $server['server_base_dn'], $filter);
    if (!$query_result) {
      ldap_help_display(ldap_errno($con), "LDAP search failure for user $filter." . ldap_help_show_error($con));
    }
    else {// display results
      $entries = ldap_get_entries($con, $query_result);
     // print_r($entries);
      ldap_help_display('search result');
      if (is_array($entries)) {
        $entry_count = $entries['count'];
        if ($entry_count == 0) {
          ldap_help_display('no entries found');
        }
        else {
          for ($j=0; $j<$entry_count; $j++) {
            $entry = $entries[$j];
            $attr_count = $entry['count'];
            ldap_help_display(NULL, "\nsearch results, entry[$j]:");
            ldap_help_display('  dn[' . $j . ']', $entry['dn']);
            for ($i=0; $i<$attr_count; $i++) {
              $attr_name = $entry[$i];
              if (in_array($attr_name, $query['show_attr'])) {
                $values_count = $entry[$attr_name]['count'];
                for ($k=0; $k<$values_count; $k++) {
                  ldap_help_display('  ' . $attr_name . '[' . $k . ']', $entry[$attr_name][$k]);
                }
              }
            }
          }
        }
      }
    }
  }

   /**
   * Test LDAP Provisioning
   */
  foreach ($server['test_provisions'] as $provision_id => $provision) {
    ldap_help_display(NULL, "------------------------------------------\n$test_name provision \"$provision_id\"\n------------------------------------------");

    $dn = $provision['dn'];

    ldap_help_display(NULL, "\nprovision, entry[$dn]:");
    if ($provision['delete_if_exists']) {
      $query_result = @ldap_search($con, $server['server_base_dn'], $provision['find_filter']);
      if ($query_result) {
        $entries = ldap_get_entries($con, $query_result);
        if ($entries['count'] == 1) {
          $result = @ldap_delete($con, $dn);
          if ($result) {
            ldap_help_display('deleted existing entry', $dn);
          }
          else {
            ldap_help_display('failed to delete existing entry in provision', $dn);
            continue;
          }
        }
        else {
          // no entry exists
        }
      }
    }


    $result = @ldap_add($con, $dn, $provision['attr']);
    $show_result = $result ? 'success' : 'fail';
    ldap_help_display('provision result', $show_result);
    if (!$result) {
      ldap_help_display('provision error', ldap_help_show_error($con));
    }
  }

}
