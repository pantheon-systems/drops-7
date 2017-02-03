<?php
/**
 * Documentations of the module hooks
 */


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
