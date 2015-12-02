// $Id: README.txt,v 1.2 2010/12/29 01:37:46 johnbarclay Exp $

Vocubulary of LDAP Authorization and its Code

----------------------
"Consumer"
----------------------
The "consumer" or entity that authorization is being granted.

Examples:  Drupal role, Organic Group group

----------------------
"Consumer Type"
----------------------
Machine ID of a consumer.  This is used in naming conventionss.

Examples:  drupal_role, og_group

----------------------
"Consumer Module"
----------------------
The module that bridges ldap_authorization and the consumer.
It needs to (1) provide a class: LdapAuthorizationConsumer<consumer_type>
and (2) implement hook_ldap_authorization_consumer.

Examples:  ldap_authorization_drupal_role


----------------------
"Authorization ID" aka "Consumer ID"
----------------------
The id of an individual authorization such as a drupal role or organic group.

Examples:  "authenticated user", "admin" (for drupal roles)
Examples:  "knitters on skates", "vacationing programmers" (og group names for organic groups)


----------------------
"Consumer Configuration"
----------------------
Configuration of how a users ldap attributes will
determine a set of Consumer ids the user should be granted.
Represented by LdapAuthorizationConsumerConf and LdapAuthorizationConsumerConfAdmin classes
and managed at /admin/config/people/ldap/authorization.  Stored in ldap_authorization database table.

---------------------
LDAP Server Configuration
---------------------
Each Consumer Configuration will use a single ldap server configuration to bind
and query ldap.  The ldap server configuration is also used to map the drupal
username to an ldap user entry.


----------------------
LDAP Authorization data storage:
---------------------

Authorization data is stored in user->data array.  Ultimately these should be stored in $user entity fields to make integration with other modules better.

$user->data['ldap_authorizations'][<consumerType>][<authorization_id>] => attributes

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
