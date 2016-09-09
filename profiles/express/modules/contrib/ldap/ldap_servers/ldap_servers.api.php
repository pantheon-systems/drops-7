<?php

/**
 * @file
 * Hooks provided by ldap_servers module
 */


/**
 * Allows other modules to periodically affect an ldap associated user
 * or its corresponding ldap entry.
 *
 * when cron runs a batch of ldap associated drupal accounts
 * will be looked at and marked as tested.  over the course
 * of time all ldap related users will be looked at
 *
 * Each module implementing this hook is responsible for
 * altering ldap entries and drupal user objects; simply
 * altering the variables will have no affect on the actual
 * ldap entry or drupal user
 */

function hook_ldap_servers_user_cron(&$users) {



}

/**
 * helper hook to see if a batch of ldap users
 * needs to be queried
 *
 * if a module implements hook_ldap_servers_user_cron,
 * but currently does not need to process user cron batches,
 * it should return FALSE
 */

function hook_ldap_servers_user_cron_needed() {
  return TRUE;
}

/**
 * Allows other modules to alter ldap entry or perform other necessary
 *   LDAP operations before entires are provisioned.
 * This should be invoked before provisioning ldap entries
 *
 * @param array $ldap_entries as array keyed on lowercase dn of entry with
 *   value of array in format used in ldap_add or ldap_modify function
 *   e.g.
 *   $ldap_entries['cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'] = array(
 *    "attribute1" => array("value"),
 *    "attribute2" => array("value1", "value2"),
 *  );
 *
 * @param LdapServer $ldap_server as ldap server configuration object that is
 *   performing provisioning
 *
 * @param array $context with the following key/values:
 *   'action' => add|modify|delete
 *
 *   'corresponding_drupal_data' => if ldap entries have corresponding drupal objects, such
 *     as ldap user entries and drupal user objects; ldap groups and drupal roles; etc
 *     this will be array keyed on lowercase dn with values of objects
 *     e.g.
 *     $context['corresponding_drupal_data'] = array(
 *      'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu' => drupal user object1,
 *      'cn=jfun,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'  => drupal user object2,
 *     )
 *
 *    'corresponding_drupal_data_type' => 'user', 'role', etc.
 *
 *  );
 */

function hook_ldap_entry_pre_provision_alter(&$ldap_entries, $ldap_server, $context) {



}

/**
 * Allows modules to react to provisioning of ldap entries.
 *
 * This should be invoked after provisioning ldap entries
 *
 * -- same signature as hook_ldap_entry_pre_provision_alter with ldap entries not passed by reference
 * -- ldap entries are not queried after provisioning, so $ldap_entries are in form
 *    hook_ldap_entry_pre_provision; not actual queryied ldap entries
 * -- if actual ldap entries are available after provisioning, they will be in
 *    $context['provisioned_ldap_entries][<dn>] => ldap entry array in format of an ldap query
 *    returned from ldap_get_entries() with 'count' keys
 */

function hook_ldap_entry_post_provision(&$ldap_entries, $ldap_server, $context) {



}



/**
 * Perform alterations of ldap attributes before query is made.
 *
 * To avoid excessive attributes in an ldap query, modules should
 * alter attributes needed based on $op parameter
 *
 * @param array $attributes
 *   array of attributes to be returned from ldap queries where:
 *     - each key is ldap attribute name (e.g. mail, cn)
 *     - each value is associative array of form:
 *       - 'conversion' => NULL,
 *       - 'values' => array(0 => 'john', 1 => 'johnny'))
 *
 * @param array $params context array with some or all of the following key/values
 *   'sid' => drupal account object,
 *   'ldap_context' => ,
 *   'direction' =>
 *
 */
function hook_ldap_attributes_needed_alter(&$attributes, $params) {

  $attributes['dn'] = ldap_servers_set_attribute_map(@$attributes['dn'], 'ldap_dn') ;
  if ($params['sid']) { // puid attributes are server specific
    $ldap_server = (is_object($params['sid'])) ? $params['sid'] : ldap_servers_get_servers($params['sid'], 'enabled', TRUE);

    switch ($op) {
      case 'user_insert':
      case 'user_update':
        if (!isset($attributes[$ldap_server->user_attr])) {
          // don't provide attribute if it exists, unless you are adding data_type or value information
          //   in that case, don't overwrite the whole array (see $ldap_server->mail_attr example below)
          $attributes[$ldap_server->user_attr] = ldap_servers_set_attribute_map();
        }
        if (!isset($attributes[$ldap_server->mail_attr])) {
          $attributes[$ldap_server->mail_attr] = ldap_servers_set_attribute_map(); // set default values for an attribute, force data_type
        }

        ldap_servers_token_extract_attributes($attributes,  $ldap_server_obj->mail_template);
        $attributes[$ldap_server->unique_persistent_attr] = ldap_servers_set_attribute_map(@$attributes[$ldap_server->unique_persistent_attr]);

      break;
    }
  }
}


/**
 * Perform alterations of $ldap_user variable.
 *
 *
 * @param array $ldap_user see README.developers.txt for structure
 * @param array $params context array with some or all of the following key/values
 *   'account' => drupal account object,
 *   'ldap_context' => ,
 *   'module' =>  module calling alter, e.g. 'ldap_user',
 *   'function' => function calling alter, e.g. 'provisionLdapEntry'
 *
 */

function hook_ldap_user_alter(&$ldap_entry, $params) {


}

/**
 * Allow the results from the ldap search answer to be modified
 * The query parameters are provided as context infomation
 * (readonly)
 *
 */
function hook_ldap_server_search_results_alter(&$entries, $ldap_query_params) {
  // look for a specific part of the $results array
  // and maybe change it
}

/**
 * Allows other modules to transform the Drupal login username to an LDAP
 * UserName attribute.
 * Invoked in LdapServer::userUsernameToLdapNameTransform()
 *
 * @param $ldap_username
 *   The ldap username that will be used for the AuthName attribute
 * @param $drupal_username
 *   The Drupal user name
 * @param $context
 *   An array of additional contextual information
 *   - ldap_server: The LDAP server that is invoking the hook
 */
function hook_user_ldap_servers_username_to_ldapname_alter(&$ldap_username, $drupal_username, $context) {
  // Alter the name only if it has not been altered already, ie php eval code
  if ($ldap_username == $drupal_username) {
    $authname = ldap_user_get_authname($ldap_username);
    if (!empty($authname)) {
      $ldap_username = $authname;
    }
  }
}
