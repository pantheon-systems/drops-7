


=======================================
PHP to Test for Allowed LDAP Users
=======================================

Remember:
-- php module must be enabled (its one of the core drupal modules)
-- code should not be enclosed in <?php   ?>

Two variables are available:

(1) $_name - the username ldap server configuration has mapped user to such as "jdoe" etc.  How this is derived is configured in ldap_servers module.



(2) $_ldap_user_entry - their ldap entry as returned from php ldap extension.

$_ldap_user_entry is something like:

array(
    'dn' => 'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'jkool@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );


Result should print 1 for allowed or 0 for disallowed.  The function used to evaluate the code is php_eval() in php.module

---------------------------------
Example 1:


//exclude users with guests.myuniversity.edu email address
if (strpos($_ldap_user_entry['attr']['mail'][0], '@guests.myuniversity.edu') === FALSE) {
  print 1;
}
else {
  print 0;
}

---------------------------------
Example 2:

// test behaviour of nobody excluded
print 1;

---------------------------------
Example 3:

// test behaviour of nobody excluded
print 0;







