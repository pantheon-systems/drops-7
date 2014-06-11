<?php

/**
 * @file
 * A specific handler for US Address Verification.
 */

$plugin = array(
  'title' => t('Exactor Address Verification Service'),
  'format callback' => 'commerce_exactor_avs_generate',
  'type' => 'address',
  'weight' => -80,
);

/**
 * Form generate callback.
 */
function commerce_exactor_avs_generate(&$format, &$address, $context = array()) {
  if ($address['country'] == 'US' && $context['mode'] == 'form') {
    // Defaults.
    if (!isset($address['verified'])) {
      $address['verified'] = FALSE;
    }

    // Add verify button.
    $format['exactor_avs_verify'] = array(
      '#type' => 'submit',
      '#value' => t('Verify'),
      '#weight' => 100,
      '#action_name' => 'verify',
      '#name' => $context['field']['field_name'] . '-' . $context['instance']['id'] . '-exactor-avs-verify',
      '#element_validate' => array('commerce_exactor_avs_address_form_validate'),
      '#executes_submit_callback' => FALSE,
      '#limit_validation_errors' => array(array('exactor_avs_verify')),
      '#ajax' => array(
        'callback' => 'commerce_exactor_avs_ajax_callback',
        'wrapper' => $format['#wrapper_id'],
      ),
    );
  }
}

/**
 * Form validation callback.
 */
function commerce_exactor_avs_address_form_validate($element, &$form_state, &$form) {
  if (isset($form_state['triggering_element']['#action_name']) && $form_state['triggering_element']['#action_name'] === 'verify') {
    // Get array parents.
    array_pop($element['#array_parents']);

    // Get the submitted values from the form state.
    $values = drupal_array_get_nested_value($form_state['values'], $element['#array_parents']);

    // Lookup for Exactor values.
    $response = commerce_exactor_avs_lookup($values);

    if (isset($response['status'])) {
      // Set the new values in the form state.
      drupal_array_set_nested_value($form_state['values'], $element['#array_parents'], $response['output_address'], TRUE);
      // Do the same for entered values.
      drupal_array_set_nested_value($form_state['input'], $element['#array_parents'], $response['output_address'], TRUE);
    }
    else {
      form_set_error('exactor_avs_verify', t("The Exactor services can't verify your address, please complete it."));
    }
  }
}
