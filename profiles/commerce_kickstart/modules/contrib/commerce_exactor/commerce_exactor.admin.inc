<?php
/**
 * @file
 *   commerce_exactor.admin.inc
 *   Administrtive forms for Commerce Exactor module
 */

/**
 * Settings form
 */
function commerce_exactor_settings_form($form_state) {

  $form['introduction'] = array(
    '#type' => 'markup',
    '#markup' => '<p><img align="right" src = "' . file_create_url(drupal_get_path('module', 'commerce_exactor') . '/theme/exactor_logo.png') . '">' .
    t('Exactor is a fully automated, end-to-end system to calculate sales and use taxes and file tax returns. It calculates taxes wherever necessary, in real time, and without impacting transactions.')
    . '</p>',
  );

  $form['links'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' .
    t('For more information visit the <a href=@marketplace>Commerce Marketplace</a>, or signup now with <a href=@exactor>Exactor</a>.', array(
      '@marketplace' => 'https://marketplace.commerceguys.com/add-on/exactor/overview',
      '@exactor' => 'https://commerceguys.exactor.com/account/signup/application',
      ))
    . '</p>',
  );

  $form['exactor_account_id'] = array(
    '#type'=> 'textfield',
    '#title' => t('Exactor Account ID'),
    '#default_value' => variable_get('exactor_account_id'),
    '#description' => t('Your 8 digit account ID.'),
    '#required' => TRUE,
  );

  $form['exactor_user_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Exactor User ID'),
    '#default_value' => variable_get('exactor_user_id'),
    '#description' => t('Your alphanumeric user ID.'),
    '#required' => TRUE,
  );

  $form['exactor_name_line'] = array(
    '#type' => 'textfield',
    '#title' => t('Business Name'),
    '#default_value' => variable_get('exactor_name_line'),
    '#description' => t('Your business name.'),
    '#required' => TRUE,
  );

  $form['exactor_thoroughfare'] = array(
    '#type' => 'textfield',
    '#title' => t('Street 1'),
    '#default_value' => variable_get('exactor_thoroughfare'),
    '#required' => TRUE,
  );

  $form['exactor_premise'] = array(
    '#type' => 'textfield',
    '#title' => t('Street 2'),
    '#default_value' => variable_get('exactor_premise'),
    '#required' => FALSE,
  );

  $form['exactor_locality'] = array(
    '#type' => 'textfield',
    '#title' => t('Town / City'),
    '#default_value' => variable_get('exactor_locality'),
    '#required' => TRUE,
  );

  $form['exactor_administrative_area'] = array(
    '#type' => 'textfield',
    '#title' => t('State or Province'),
    '#default_value' => variable_get('exactor_administrative_area'),
    '#required' => TRUE,
  );

  $form['exactor_postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
    '#default_value' => variable_get('exactor_postal_code'),
    '#required' => TRUE,
  );

  $form['exactor_country'] = array(
    '#type' => 'textfield',
    '#title' => t('Country'),
    '#default_value' => variable_get('exactor_country', 'USA'),
    '#description' => t('3 character country code.'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
