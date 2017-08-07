<?php
/**
 * @file
 * cu_ldap.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_ldap_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'user-user-field_ldap_display_name'.
  $field_instances['user-user-field_ldap_display_name'] = array(
    'bundle' => 'user',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'user',
    'fences_wrapper' => 'div_div_div',
    'field_name' => 'field_ldap_display_name',
    'label' => 'Display Name',
    'required' => 0,
    'settings' => array(
      'linkit' => array(
        'button_text' => 'Search',
        'enable' => 0,
        'profile' => '',
      ),
      'text_processing' => 0,
      'user_register_form' => 0,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 38,
    ),
  );

  // Exported field_instance: 'user-user-field_ldap_primary_affiliation'.
  $field_instances['user-user-field_ldap_primary_affiliation'] = array(
    'bundle' => 'user',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 3,
      ),
    ),
    'entity_type' => 'user',
    'fences_wrapper' => 'div_div_div',
    'field_name' => 'field_ldap_primary_affiliation',
    'label' => 'Primary Affiliation',
    'required' => 0,
    'settings' => array(
      'linkit' => array(
        'button_text' => 'Search',
        'enable' => 0,
        'profile' => '',
      ),
      'text_processing' => 0,
      'user_register_form' => 0,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 39,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Display Name');
  t('Primary Affiliation');

  return $field_instances;
}
