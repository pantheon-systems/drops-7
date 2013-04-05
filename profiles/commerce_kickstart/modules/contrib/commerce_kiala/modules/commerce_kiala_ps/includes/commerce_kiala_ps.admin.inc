<?php

/**
 * @file
 * Administrative page callbacks for the Kiala Pack & Ship module.
 */


/**
 * Web service order page
 */
function commerce_kiala_ps_ws_order_page($order) {
  $settings = commerce_kiala_settings();

  // Check warnings to display and disable forms
  $warnings = commerce_kiala_ps_order_warnings($order) + commerce_kiala_ps_ws_order_warnings($order);

  if (!empty($warnings)) {
    $form['warnings'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('commerce-kiala-ps-ws-warnings'),
      ),
    );
    foreach ($warnings as $warning_key => $warning) {
      $form['warnings']['warning_' . $warning_key] = array(
        '#markup' => '<div class="messages warning commerce-kiala-ps-ws-warning ' . drupal_html_class('commerce-kiala-ps-ws-warning-' . $warning_key) . '">' . $warning . '</div>',
      );
    }
  }

  $form['create_order'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit order to Kiala'),
    '#description' => t('Submits the order to the web service to generate a tracking number.'),
    'content' => drupal_get_form('commerce_kiala_ps_ws_create_order_form', $order, $warnings),
  );

  return $form;
}

/**
 * Builds the form for P&S web service "create" order.
 *
 * @param $order
 *  An order object
 */
function commerce_kiala_ps_ws_create_order_form($form, &$form_state, $order, $warnings = NULL) {
  if (!isset($warnings)) {
    $warnings = commerce_kiala_ps_order_warnings($order) + commerce_kiala_ps_ws_order_warnings($order);
  }
  elseif (!is_array($warnings)) {
    $warnings = array();
  }

  $log_access = commerce_kiala_ps_ws_log_order_access('create', $order);

  $form['#disabled'] = !empty($warnings);

  $form['order_id'] = array(
    '#type' => 'value',
    '#value' => $order->order_id,
  );

  $logs = entity_load('commerce_kiala_ps_ws_log', FALSE, array(
    'order_id' => $order->order_id,
    'method' => 'createOrder',
  ));

  $have_logs = FALSE;
  $success_log = NULL;
  if (!empty($logs)) {
    $have_logs = TRUE;
    rsort($logs);
    foreach ($logs as $log) {
      if ($log->status == 'success') {
        $success_log = $log;
        break;
      }
    }
  }

  if (!empty($success_log)) {
    $form['create_order_force_submit'] = array(
      '#title' => t('Force re-submit'),
      '#type' => 'checkbox',
      '#default_value' => FALSE,
      '#access' => $log_access,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => !$have_logs ? t('Submit order') : t('Re-submit order'),
    '#access' => $log_access,
    '#states' => array(
      'invisible' => array(
        ':input[name*="create_order_force_submit"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['create_order_last_run'] = array(
    '#type' => 'fieldset',
    '#title' => t('Last run'),
    '#collapsible' => FALSE,
    '#attributes' => array('class' => array('kiala-ps-ws-last-run')),
  );

  if ($have_logs) {
    $last_log = reset($logs);
    $statuses = commerce_kiala_ps_ws_log_status_options_list();
    if (isset($statuses[$last_log->status])) {
      $form['create_order_last_run']['#title'] = t('Last run: @status', array('@status' => $statuses[$last_log->status]));
    }

    $form['create_order_last_run']['#attributes']['class'][] = drupal_html_class('kiala-ps-ws-last-run-' . $last_log->status);
    $form['create_order_last_run'][0] = array(
      '#theme' => 'commerce_kiala_ps_ws_log',
      '#log' => $last_log,
      '#access' => commerce_kiala_ps_ws_log_access('view', $last_log),
    );
  }
  else {
    $form['create_order_last_run'][0] = array(
      '#markup' => t('Service has never been run.'),
    );
  }

  return $form;
}

/**
 * Submit callback for commerce_kiala_ps_ws_create_order_form
 */
function commerce_kiala_ps_ws_create_order_form_submit($form, &$form_state) {
  $values = &$form_state['values'];

  $order_id = $values['order_id'];
  $order = commerce_order_load($order_id);

  $successful_logs = entity_load('commerce_kiala_ps_ws_log', FALSE, array(
    'order_id' => $order->order_id,
    'method' => 'createOrder',
    'status' => 'success'
  ));

  // Perform service call
  if (empty($successful_logs) || !empty($values['create_order_force_submit'])) {
    $response = commerce_kiala_ps_ws_ping_create_order($order);

    if ($response->status == 'success') {
      drupal_set_message('The order has been submitted sucessfully to Kiala.');
    }
    elseif (!empty($response->error)) {
      drupal_set_message(t('An error has occurred while processing the request. <br />@code@message', array(
        '@code' => !empty($response->error->fault_code) ? $response->error->fault_code . ': ' : '',
        '@message' => $response->error->fault_message,
      )), 'error');
    }
    else {
      drupal_set_message('An error has occurred processing the request.');
    }
  }

}

/**
 * Form callback wrapper: confirmation form for deleting a WS log.
 *
 * @param $order
 *   The order object containing the transaction being deleted by the form.
 * @param $log
 *   The actual Kiala PS WS log that will be deleted.
 */
function commerce_kiala_ps_ws_log_delete_form_wrapper($order, $log) {
  return drupal_get_form('commerce_kiala_ps_ws_log_delete_form', $order, $log);
}


/**
 * Form callback: confirmation form for deleting a web service log.
 *
 * @param $order
 *   The order object containing the transaction being deleted by the form.
 * @param $log
 *   The actual Kiala PS WS log that will be deleted.
 *
 * @see confirm_form()
 */
function commerce_kiala_ps_ws_log_delete_form($form, &$form_state, $order, $log) {
  $form_state['order'] = $order;
  $form_state['kiala_ps_ws_log'] = $log;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_payment') . '/includes/commerce_kiala_ps.admin.inc';

  $form['#submit'][] = 'commerce_kiala_ps_ws_log_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete this web service log entry?'),
    '',
    theme('commerce_kiala_ps_ws_log', array('log' => $log)),
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for commerce_kiala_ps_ws_log_delete_form().
 */
function commerce_kiala_ps_ws_log_delete_form_submit($form, &$form_state) {
  $log = $form_state['kiala_ps_ws_log'];
  $order = $form_state['order'];

  $return = entity_delete('commerce_kiala_ps_ws_log', $log->log_id);
  if ($return !== FALSE) {
    drupal_set_message(t('Web service log entry deleted.'));
    watchdog('commerce_kiala_ps', 'Deleted Kiala web service log entry @log.', array('@log' => $log->log_id), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('The web service log entry could not be deleted.'), 'error');
  }

  $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/kiala-packship-ws/logs';
}
