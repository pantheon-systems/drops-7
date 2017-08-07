<?php

/**
 * @file
 * The ldap_help watchdog is just a filtered watchdog summary.
 *
 */
function ldap_help_watchdog() {

  $path =  drupal_get_path("module", "ldap_help");
  $_content = "";
  if (module_exists('dblog')) {
    include_once(drupal_get_path('module', 'dblog') . '/dblog.admin.inc');
    $_SESSION['dblog_overview_filter']['type'] = Array('ldap' => 'ldap');
    $_content .= "<h3>" . t('LDAP Watchdog Errors and Notifications') . "</h3>";
    $overview = dblog_overview();
    $_content .= render($overview);

    $_content .= l(t('...more watchdog'), 'admin/reports/dblog');
  }
  else {
    $_content .= "<h3>" . t('LDAP Help Watchdog Errors and Notifications') . "</h3>";
    $_content .= 'This feature requires <code>Database logging</code> module to be turned on. ';
    $_content .= l(t('Module enable page'), 'admin/build/modules');
  }


  return $_content;
}
