<?php
/**
 * @file
 *
 * Admin forms for Commerce bean module.
 */

function commerce_bean_admin_form($form, &$form_state) {
  $type_options = commerce_bean_get_options();
  $form['commerce_bean_checkout_types'] = array(
    '#type' => 'select',
    '#title' => t('Bean types to be exposed as checkout panes'),
    '#options' => $type_options,
    '#default_value' => variable_get('commerce_bean_checkout_types', drupal_map_assoc(array_keys($type_options))),
    '#multiple' => TRUE,
  );

  return system_settings_form($form);
}
