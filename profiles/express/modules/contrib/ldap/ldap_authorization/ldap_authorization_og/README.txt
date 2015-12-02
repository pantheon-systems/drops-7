
LDAP Authorization Organic Groups:

----------------------
LDAP Authorization OG Storage:
----------------------
OG authorizations are stored in form gid-rid from the tables og (og.gid) and og_roles (og_roles.rid).  E.G. 1-2, 2-3, 3-4.  OG in Drupal 7 does not use machine names so numeric ids are the only way to store such identifiers.

such as:

$user->data = array(
  'ldap_authorizations' => array(
    'og_group' => array (
      '3-2' => array (
        'date_granted' => 1329105152,
      ),
      '2-3' => array (
        'date_granted' => 1329105152,
      ),
    ),
    'drupal_role' => array (
      '7' => array (
        'date_granted' => 1329105152,
      ),
      '5' => array (
        'date_granted' => 1329105152,
      ),
    ),
  );



----------------------
To Dos (Too small for issue queue)
----------------------
- add support for gid-rid normalized format such as 3-2 in ldap mapping interface
