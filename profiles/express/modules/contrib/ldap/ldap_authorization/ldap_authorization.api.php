<?php

/**
 * @file
 * summary of hooks and other developer related functions
 */

/**
 * Allow a custom module to alter ldap_authorization mappings
 *
 *  @param object $user as drupal acct object
 *  @param array $ldap_user
 *    See ldap_authentication/README.developers.txt for structure
 *  @param LdapServer $ldap_server
 *    The ldap server associated with this consumer type
 *  @param LdapAuthorizationConsumerConf $consumer_conf
 *    The ldap consumer configuraion associated with this consumer type
 *  @param array $proposed_ldap_authorizations with keys of consumer ids
 *    and values of consumers (drupal roles, og entity, etc.)

 *
 *  alters $proposed_ldap_authorizations by reference
 */



function hook_ldap_authorization_maps_alter($user, $ldap_user, $ldap_server, $consumer_conf, $proposed_ldap_authorizations, $op) {


}


/**
 * Allow a custom module to alter ldap_authorizations after they have been determined by ldap_authorizations,
 *  but before they are granted/removed from user.
 *
 *  @param array $authorizations as proposed authorizations for user.
 *    will be in format returned by LdapAuthorizationConsumerX::populateConsumersFromConsumerIds
 *
 *  @param array $params with the following key/value pairs
 *    'ldap_user' => See ldap_authentication/README.developers.txt for structure
 *    'ldap_server' => the LdapServer object for this consumer type
 *    'consumer' => LdapAuthorizationConsumerX object
 *    'user' => Drupal user account object
 *
 *  alters $proposed_ldap_authorizations by reference
 */

function hook_ldap_authorization_authorizations_alter($authorizations, $params) {

  // alter authorizations

}
