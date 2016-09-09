<?php

/**
 * @file
 * theming functions for ldap_query module
 *
 */

/**
 * Returns HTML for ldap servers list.
 *
 * @param $variables
 *   An associative array containing:
 *   - ldap_query: an array of one or more ldap server configurations.
 *   - actions:  true or false indicating include update, delete, etc. links
 *   - type:  'table', 'list', etc for style to render
 *
 * @ingroup themeable
 */
function theme_ldap_query_list($variables) {
  extract($variables);

  $table = array(
    'header' => array(t('Name'), t('Base DN'), t('Filter'), t('Attributes'), t('Enabled')),
    'attributes' => array('id' => 'ldap_queries', 'class' => 'data'),
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => '',
    'caption' => 'LDAP Queries',
  );

  if ($actions) {
    $table['header'][] = "Operations";
  }

  if (count($ldap_queries)) {
    foreach ($ldap_queries as $qid => $ldap_query ) {
      $row =  array(
        $ldap_query->name,
        $ldap_query->base_dn_str,
        $ldap_query->filter,
        $ldap_query->attributes_str,
        ($ldap_query->status == 1) ? "Yes" : "No",
      );
      if ($actions) {
        $admin = new LdapQueryAdmin($ldap_query->qid);
        $row[] = join(' | ', $admin->getActions());
      }
      $table['rows'][] = $row;
    }
  }
  else {
    $table['rows'] = array();
  }
  $output = theme('table', $table);

  return $output;
}

/**
 * Returns HTML for ldap server.
 *
 * @param $variables
 *   An associative array containing:
 *   - ldap_query_config: an array of one or more ldap server configurations.
 *   - actions:  true or false indicating include update, delete, etc. links
 *   - type:  'table', 'list', etc for style to render
 *
 * @ingroup themeable
 */

function theme_ldap_query($variables) {

  extract($variables);  // $ldap_query, $actions, $type (see above)

  ldap_servers_module_load_include('php', 'ldap_query', 'LdapQuery.class');
  $properties = array();
  foreach (LdapQuery::fields() as $field_id => $field) {
    $value = $ldap_query->{$field['property_name']};
    if (is_scalar($value)) {
      $properties[] = $field_id . " = " . $value; // $value";
    }
  }

  if ($actions) {
    $admin = new LdapQueryAdmin($ldap_query->sid);
    $properties = join(' | ', $admin->getActions());
  }

  $output = theme_item_list(
    array(
      'items' => $properties,
      'type' => 'ul',
      'title' => 'Query Properties',
      'attributes' => array()
      )
    );

  return $output;
}

function theme_ldap_query_results($variables) {
  extract($variables);  // $ldap_query, $results, $show_query

  $query = theme('ldap_query', array('ldap_query' => $ldap_query));
  $results_text = t('LDAP Query Results: count=%count', array('%count' => $results['count']));

  if (!$results['count']) {
    return $query . '<br/>' . $results_text;
  }

  $table = array(
    'header' =>  array('DN'),
    'attributes' => array('id' => 'ldap_query_resultset', 'class' => 'data'),
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => '',
    'caption' => $results_text,
  );
  unset($results['count']);

  $attributes_display = array();
  if (isset($results[0]) && $results[0]) { // searching on all attributes
    foreach ($results[0] as $k => $v) {
      if (is_numeric($k)) {
        $attributes_display[] = $v;
      }
    }
  }


  foreach ($attributes_display as $attr) {
    $table['header'][] = $attr;
  }

  foreach ($results as $i => $entry) {
    $row = array($entry['dn']);
    foreach ($attributes_display as $i => $attr_name) {
      $attr_name = ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_QUERY_ARRAY); // drupal_strtolower($attr_name);
      if (!isset($entry[$attr_name])) {
        $row[] = 'no data';
      }
      elseif (is_array($entry[$attr_name])) {
        unset($entry[$attr_name]['count']);
        $row[] = join("<br/>", $entry[$attr_name]);
      }
      else {
        $row[] = $entry[$attr_name];
      }
    //  unset($attr_data['count']);

    }
    unset($entry['count']);
    $table['rows'][] = $row;
  }
  return $query . '<br/>' . theme_table($table);

}
