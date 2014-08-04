<?php
/**
 * IOPN callbacks for commerce_cba module.
 */

/**
 * Implements hook_menu().
 */
function commerce_cba_iopn_menu() {
  $items = array();

  $items['commerce-cba-iopn'] = array(
    'title' => 'IOPN callback',
    'page callback' => 'commerce_cba_iopn_callback',
    'access callback' => TRUE,
    'type' => MENU_CALLBACK,
  );

  return $items;
}

/**
 * IOPN Callback.
 *
 * This function should only be called by Amazon IOPN systems.
 * Returns a HTTP status code depending on how the request is build and the
 *   related order status.
 * - Status 200: OK
 * - Status 403: in case that the signature doesn't match.
 * - Status 500: Request is not consistent, this could happen if the Ready to
 *              ship notification arrives before the New.
 * - Status 503: Force retry in case of timestamp mismatch.
 */
function commerce_cba_iopn_callback() {
  $path = drupal_get_path('module', 'commerce_cba') . '/includes';

  $type = filter_input(INPUT_POST, 'NotificationType');
  $data = filter_input(INPUT_POST, 'NotificationData');
  $uuid = filter_input(INPUT_POST, 'UUID');
  $timestamp = filter_input(INPUT_POST, 'Timestamp');
  $signature = filter_input(INPUT_POST, 'Signature');
  $debug_log = variable_get('cba_debug_log', FALSE);
  $success = FALSE;

  if (empty($data) || empty($type)) {
    if ($debug_log) {
      watchdog('Checkout by Amazon IOPN', 'Bad request. Request details: %request', array('%type' => $type, '%request' => '<pre>' . print_r($_POST, TRUE) . '</pre>'), WATCHDOG_ERROR);
    }
    drupal_add_http_header('Status', '500 Internal Server Error');
    exit();

  }

  // Verify the signature. the signature is caculated using UUID, Timestamp and
  // the secret merchant key.
  $comparison_signature = base64_encode(hash_hmac("sha1", $uuid . $timestamp, variable_get('cba_secret_key', ''), TRUE));
  if ($comparison_signature <> $signature) {
    if ($debug_log) {
      watchdog('Checkout by Amazon IOPN', 'Signature mismatch on %type Amazon notification. Request details: %request', array('%type' => $type, '%request' => '<pre>' . print_r($_POST, TRUE) . '</pre>'), WATCHDOG_ERROR);
    }
    drupal_add_http_header('Status', '403 Forbidden');
    exit();
  }

  // Verify that the timestamp and the system clock is within 15 minutes.
  $response_date = new DateTime($timestamp);
  $date = new DateTime();
  $diff = abs(($date->getTimestamp() - $response_date->getTimestamp()) / 60);
  if ($diff > 15) {
    if ($debug_log) {
      watchdog('Checkout by Amazon IOPN', 'More than 15 minutes of timestamp difference on %type Amazon notification. Request details: %request', array('%type' => $type, '%request' => print_r($_POST, 1)), WATCHDOG_ERROR);
    }
    drupal_add_http_header('Status', '503 Service unavailable');
    exit();
  }

  // Check if the xml received is valid.
  $objDom = new DomDocument();
  $objDom->loadXML($data);
  if (!$objDom->schemaValidate($path . '/xsd/iopn.xsd')) {
    $allErrors = libxml_get_errors();
    if ($debug_log) {
      watchdog('Checkout by Amazon IOPN', 'XML is not valid for on %type Amazon notification. Request details: %request. Error details: %details', array('%type' => $type, '%request' => '<pre>' . print_r($_POST, TRUE) . '</pre>', '%details' => $allErrors), WATCHDOG_ERROR);
    }
    drupal_add_http_header('Status', '500 Internal Server Error');
    exit();
  }

  $data = new SimpleXMLElement($data);
  // Load the local order referenced to the Amazon one.
  $amazon_order_id = (string) $data->ProcessedOrder->AmazonOrderID;
  if ($order = commerce_cba_load_order_by_amazon_id($amazon_order_id)) {
    $shipping_address = (array) $data->ProcessedOrder->ShippingAddress;
    $buyer_info = (array) $data->ProcessedOrder->BuyerInfo;
    if (isset($data->ProcessedOrder->BillingAddress)) {
      $billing_address = (array) $data->ProcessedOrder->BillingAddress;
    }
  }
  else {
    if ($debug_log) {
      watchdog('Checkout by Amazon IOPN', 'Order referenced by Amazon call doesn\'t exist. Request details: %request', array('%request' => '<pre>' . print_r($_POST, TRUE) . '</pre>'), WATCHDOG_ERROR);
    }
    drupal_add_http_header('Status', '500 Internal Server Error');
    exit();
  }

  switch ($type) {
    case 'NewOrderNotification':
      rules_invoke_all('commerce_cba_iopn_new_order_notification', $order, $shipping_address, $buyer_info);
      watchdog('Checkout by Amazon IOPN', 'Order updated by Amazon to "New"', array(), WATCHDOG_INFO, l(t('View order'), 'admin/commerce/orders/' . $order->order_id));
      drupal_add_http_header('Status', '200 OK');
      $success = TRUE;
      break;
    case 'OrderCancelledNotification':
      rules_invoke_all('commerce_cba_iopn_cancel_order_notification', $order, $shipping_address, $buyer_info);
      watchdog('Checkout by Amazon IOPN', 'Order canceled by Amazon', array(), WATCHDOG_INFO, l(t('View order'), 'admin/commerce/orders/' . $order->order_id));
      drupal_add_http_header('Status', '200 OK');
      $success = TRUE;
      break;
    case 'OrderReadyToShipNotification':
      // If the Ready to ship notification is arriving before the New status
      // we need to return an error 500.
      if (commerce_cba_iopn_order_has_been_new($order->order_id)) {
        rules_invoke_all('commerce_cba_iopn_rts_order_notification', $order, $shipping_address, $billing_address, $buyer_info);
        if ($debug_log) {
          watchdog('Checkout by Amazon IOPN', 'Order updated by Amazon to "Ready to ship"', array(), WATCHDOG_INFO, l(t('View order'), 'admin/commerce/orders/' . $order->order_id));
        }
        drupal_add_http_header('Status', '200 OK');
        $success = TRUE;
      }
      else {
        if ($debug_log) {
          watchdog('Checkout by Amazon IOPN', 'Error: Order "Ready to Ship" notification has arrived before "New". Request details: %request', array('%request' => '<pre>' . print_r($_POST, TRUE) . '</pre>'), WATCHDOG_ERROR);
        }
        drupal_add_http_header('Status', '500 Internal Server Error');
      }
      break;
    default:
      // If we arrived here, it means something wrong happened that was not covered.
      if ($debug_log) {
        watchdog('Checkout by Amazon IOPN', 'Unexpected Amazon return call. Request details: %request', array('%request' => '<pre>' . print_r($_POST, TRUE) . '</pre>'), WATCHDOG_ERROR);
      }
      drupal_add_http_header('Status', '500 Internal Server Error');
      break;
  }
  // Debugging logs on success.
  if ($success && $debug_log) {
    // Build debugging message content.
    $debug_message = t('Amazon IOPN call.');
    $debug_message .= '<br /><strong>' . $type . '</strong>:<br />';
    $debug_message .= '<pre>' . print_r($_POST, TRUE) . '</pre>';
    // Logs a system message.
    watchdog('Checkout by Amazon IOPN', $debug_message, array(), WATCHDOG_INFO);
  }

  exit();
}

/**
 * When order is processed by Amazon, the Ready to ship notification may arrive
 * before the order New one, we need to check this for consistency.
 */
function commerce_cba_iopn_order_has_been_new($order_id) {
  $entity_type = 'commerce_order';
  $entity_info = entity_get_info($entity_type);

  $query = db_select($entity_info['revision table'], 'revision');
  $query->fields('revision', array($entity_info['entity keys']['id'], $entity_info['entity keys']['revision']))
    ->condition('revision.' . $entity_info['entity keys']['id'], $order_id);
  $revisions = $query
    ->execute()
    ->fetchAllAssoc($entity_info['entity keys']['revision']);

  foreach ($revisions as $key => $revision) {
    $order = entity_revision_load($entity_type, $key);
    if ($order->status == 'amazon_new') {
      return TRUE;
    }
  }

  return FALSE;
}
