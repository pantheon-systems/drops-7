<?php

/**
 * @file
 * Definition of ldap_views module
 */

/**
 * Implements hook_views_data().
 */

function ldap_views_views_data() {
  $data['ldap']['table']['group'] = t('LDAP Query');

  $data['ldap']['table']['base'] = array(
    'field' => 'dn',
    'title' => t('LDAP Query'),
    'help' => t('Data from LDAP Server.'),
    'query class' => 'ldap_views',
  );
  // basedn
  $data['ldap']['basedn'] = array(
    'title' => t('LDAP Base DN'),
    'help' => t('The LDAP search base entry'),
    'argument' => array(
      'handler' => 'ldap_views_handler_argument',
    ),
  );
  // filter
  // Generic column
  $data['ldap']['attribute'] = array(
    'title' => t('LDAP Attribute'),
    'help' => t('An attribute in LDAP Server'),
    'argument' => array(
      'handler' => 'ldap_views_handler_argument_attribute',
    ),
    'field' => array(
      'handler' => 'ldap_views_handler_field_attribute',
    ),
    'sort' => array(
      'handler' => 'ldap_views_handler_sort_attribute',
    ),
    'filter' => array(
      'handler' => 'ldap_views_handler_filter_attribute',
    ),
  );
  $data['ldap']['dn'] = array(
    'title' => t('DN'),
    'help' => t('Distinguished name'),
    'field' => array(
      'handler' => 'ldap_views_handler_field',
    ),
    'sort' => array(
      'handler' => 'ldap_views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['ldap']['cn'] = array(
    'title' => t('CN'),
    'help' => t('Common name'),
    'argument' => array(
      'handler' => 'ldap_views_handler_argument',
    ),
    'field' => array(
      'handler' => 'ldap_views_handler_field',
    ),
    'sort' => array(
      'handler' => 'ldap_views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'ldap_views_handler_filter',
    ),
  );
  $data['ldap']['objectClass'] = array(
    'title' => t('Object Class'),
    'help' => t('The LDAP objectClass'),
    'argument' => array(
      'handler' => 'ldap_views_handler_argument',
    ),
    'field' => array(
      'handler' => 'ldap_views_handler_field',
    ),
    'sort' => array(
      'handler' => 'ldap_views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'ldap_views_handler_filter',
    ),
  );


  return $data;
}

/*
Alternative to 'attribute' field: add fields here, but we need the qid. Alternative: bundles + entity fields
function ldap_views_views_data_alter(&$data) {
  $data['ldap']['example_field'] = array(
    'title' => t('Example field'),
    'help' => t('Some example content that references a user'),
    'field' => array(
      'handler' => 'ldap_views_handler_field',
    ),
  );

}
 */

/**
 * Implements hook_views_plugins().
 */
function ldap_views_views_plugins() {
  return array(
    'query' => array(
      'ldap_views' => array(
        'title' => t('LDAP'),
        'help' => t('Reads from an ldap server.'),
        'handler' => 'ldap_views_plugin_query_ldap',
      ),
    ),
  );
}
