<?php
// $Id: ldap_query.inc,v 1.4.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 *
 */



/**
 * see ldap_query_get_servers()
 */
function _ldap_query_get_queries($qid, $type, $flatten, $reset) {
  ldap_server_module_load_include('php', 'ldap_query', 'LdapQueryAdmin.class');
  static $queries;

  if ($reset) {
    $queries = array();
  }
  if (!isset($queries['all'])) {
    $queries['all'] = LdapQueryAdmin::getLdapQueryObjects('all', 'all');
  }
  if (!isset($queries['enabled'])) {
    $queries['enabled'] = array();
    foreach ($queries['all'] as $_qid => $ldap_query) {
      if ($ldap_query->status == 1) {
        $queries['enabled'][$_qid] = $ldap_query;
      }
    }
  }


  if ($qid) {
    if (!isset($queries[$type][$qid])) {
      return FALSE;
    }
    return ($flatten) ? $queries[$type][$qid] : $queries[$type];
  }

  if (isset($queries[$type])) {
    return $queries[$type];
  }
}

/**
 * see ldap_query_get_servers()
 */
function _ldap_query_get_simpletest_queries($qid, $type, $flatten, $reset = TRUE) {
  require_once('tests/LdapQueryTest.class.php');

  $queries['all'] = LdapQueryTest::getLdapQueryObjects('all', 'all');

  foreach ($queries['all'] as $_sid => $ldap_query) {
    if ($ldap_query->status == 1) {
      $queries['enabled'][$_qid] = $ldap_query;
    }
  }


  if ($sid) {
    if (!isset($queries[$type][$qid])) {
      return FALSE;
    }
    return ($flatten) ? $queries[$type][$qid] : $queries[$type];
  }

  if (isset($queries[$type])) {
    return $queries[$type];
  }
}
