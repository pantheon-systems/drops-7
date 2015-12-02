<?php
// $Id: ldap_help.resources.inc,v 1.1 2011/02/07 23:12:23 johnbarclay Exp $
/**
 * @file
 * The ldap_help resources are just links.
 *
 */
function ldap_help_main() {

  $text = '<h3>' . t('LDAP Help Module') . '</h3><p>' .
  t('This module is meant to assist Drupal admins in configuring, debugging, sharing, and submitting
  support and bug request related to LDAP modules.') . '<strong><em> ' .
  t('LDAP Help Module should be disabled unless you are debugging or configuring ldap problems.') . ' </em></strong>' .
  t('It adds no functionality to the LDAP modules.') . '</p>';

  $path = drupal_get_path("module", "ldap_servers");

  $text .= <<<EOT

  <h3>LDAP Module Resources</h3>
  <ul>
    <li>The <a href="http://drupal.org/node/997082">Drupal.org Documentation</a> covers basics of module.</li>
    <li>Search <a href="http://drupal.org/project/issues/search/ldap">issue queue</a>  For best results,
    select version and category before searching.</li>
    <li><a href="http://drupal.org/project/issues/ldap">View all issues</a></li>
    <li><a href="http://docs.moodle.org/20/en/LDAP_authentication">Moodle LDAP module documentation</a> is
    well done and provides insight into LDAP in a PHP environment.</li>
  </ul>

  <h3>Your local LDAP Documentation and Administrators</h3>
  <p>You would be surprised how much is documented about your local ldap.  Find your organization's LDAP documentation and
     support staff before you stuggle blindly.  Know if ldap or ldaps is used.  Know if an odd port is used.
     Know how service accounts are expected to be used.
     Below are examples of ldap implementation documentation:</p>
  <ul>
  <li><a href="http://www.it.northwestern.edu/bin/docs/CentralAuthenticationServicesThroughLDAP.pdf">Northwestern University</li>
  <li><a href="http://www.netid.washington.edu/documentation/ldapConfig.aspx">University of Washington</a></li>
  <li><a href="https://wiki.uchicago.edu/display/idm/LDAP">University of Chicago</a></li>
  </ul>

  <h3>General Drupal Support Help</h3>
  <ul>
  <li><a href="http://drupal.org/node/314185">How to report a problem in Drupal</a></li>
  <li><a href="http://drupal.org/node/19279">How to report bugs in Drupal</a></li>
  <li><a href="http://drupal.org/Troubleshooting-FAQ">Drupal Troubleshooting FAQ</a></li>
  </ul>

  <h3>General LDAP Resources</h3>
  <ul>
  <li><a href="http://us.php.net/manual/en/book.ldap.php">PHP LDAP</a></li>
  <li><a href="http://directory.apache.org/studio/">Apache Directory Studio</a> LDAP Browser and Directory Client.</li>
  <li><a href="http://www.novell.com/documentation/edir873/index.html?page=/documentation/edir873/edir873/data/h0000007.html">Novell Edirectory</a></li>
  <li><a href="http://images.apple.com/server/macosx/docs/Open_Directory_Admin_v10.5_3rd_Ed.pdf">Apple Open Directory</a></li>
  <li><a href="http://msdn.microsoft.com/en-us/library/aa705886(VS.85).aspx">Microsoft Active Directory LDAP</a></li>
  </ul>

EOT;


  return $text;
}
