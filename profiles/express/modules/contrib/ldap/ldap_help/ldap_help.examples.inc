<?php

/**
 * @file
 * The ldap_help issues provides a filtered watchdog view for ldap issues.
 *
 */

/**
 * the goal of this function is to illustrate samples from various ldap
 * implementations (AD, openldap, etc) alongside default/common
 * ldap module configurations.  The data for the ldaps and the configuration
 * should be the same as is used in the simpletets.
 */
function ldap_help_examples() {
  module_load_include('php', 'ldap_test', 'LdapTestFunctions.class');
  $test_functions = new LdapTestFunctions();
  drupal_add_library('system', 'drupal.collapse');
  $sample_ldaps = array(
   'activedirectory' => 'activedirectory1',
   'openldap' =>  'openldap1',
  );


  $form = array();
  foreach ($sample_ldaps as  $ldap_type =>  $sample_ldap_id) {

    $sample_ldap_id = $sample_ldaps[$ldap_type];
    $test_functions->populateFakeLdapServerData(LDAP_TEST_LDAP_NAME, $sample_ldap_id);
    $data = $test_functions->data['ldap_servers'][$sample_ldap_id]['ldap'];
    $form[$sample_ldap_id] =  array(
        '#type' => 'fieldset',
        '#title' => $ldap_type,
        '#description' => '',
        '#attributes' => array('class' => array('collapsible', 'collapsed')),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    foreach (array('people', 'groups') as $ou) {
      $form[$sample_ldap_id][$ou] =  array(
        '#type' => 'fieldset',
        '#title' => "ou=$ou",
        '#description' => '',
        '#attributes' => array('class' => array('collapsible', 'collapsed')),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    }

    foreach ($data as $dn => $item) {
      $ou = ldap_servers_get_all_rdn_values_from_dn($dn, 'ou');
      $ou = $ou[0];
      unset($item['count']);

      $li = array();
      foreach ($item as $attr => $values) {
        unset($values['count']);
        if (count($values) == 1) {
          $li[] = "$attr: " . $values[0] . '<br/>';
        }
        else {
          $li[]  = theme('item_list', array('items' => $values , 'type' => 'ul', 'title' => $attr));
        }
      }
      $form[$sample_ldap_id][$ou][$dn] =  array(
        '#type' => 'fieldset',
        '#attributes' => array('class' => array('collapsible', 'collapsed')),
        '#title' => $dn,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form[$sample_ldap_id][$ou][$dn][] =  array(
        '#markup' => theme('item_list', array('items' => $li , 'type' => 'ul', 'title' => '')),
      );

    }
  }

  return drupal_render($form);
}
