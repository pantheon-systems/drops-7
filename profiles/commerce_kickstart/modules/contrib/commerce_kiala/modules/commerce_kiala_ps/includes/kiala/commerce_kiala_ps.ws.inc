<?php

/**
 * @file
 * Provides a base classes for all Kiala APIs.
 */

class CommerceKialaPSWS {
  const SETTINGS_GROUP = 'packship_ws';
  const DEFAULT_FAULT_ERROR_CODE = -999999;
  const DEFAULT_DATA_ERROR_CODE = -999999;
  const DEFAULT_DATA_ERROR_MESSAGE = '';

  public $settings = array();
  public $order = NULL;
  protected $request = array();
  protected $requestErrors = array();

  public function __construct($order) {
    $this->settings_group = self::SETTINGS_GROUP;
    $this->settings = commerce_kiala_settings(array(
      'include_passwords' => TRUE,
      'decrypt' => TRUE,
    ));

    if (!empty($order)) {
      $this->order = $order;
    }
  }

  public function getServiceURL() {
    if (!empty($this->settings[$this->settings_group . '_url'])) {
      return $this->settings[$this->settings_group . '_url'];
    }
  }

  public function getOrderNumber() {
    $number = '';
    if (!empty($this->order->order_id)) {
      // convert order id to a string
      $number = strval($this->order->order_id);

      // add Commerce plugin's order number prefix
      if (!empty($this->settings['packship_plugin_number_prefix'])) {
        $number = $this->settings['packship_plugin_number_prefix'] . $number;
      }

      // ensure max width
      $number = substr($number, 0, 32);
    }

    return $number;
  }

  public function getReferenceNumber() {
    return $this->getOrderNumber();
  }

  public function getIdentification() {
    $reference_number = $this->getReferenceNumber();
    if (empty($reference_number)) {
      return array();
    }

    // Exit if no web service credentials
    if (empty($this->settings[$this->settings_group . '_sender_id']) ||
        empty($this->settings[$this->settings_group . '_password'])) {
      return array();
    }

    // get the raw sender id
    $sender_id = $this->settings[$this->settings_group . '_sender_id'];

    // compute secret hash:
    // SHA-512 hash code of the order number, Kiala sender id, and Kiala password
    $hash = hash('sha512', $reference_number . $sender_id . $this->settings[$this->settings_group . '_password']);

    // set originator
    $originator = '';
    if (!empty($this->settings['packship_platform_originator'])) {
      $originator = $this->settings['packship_platform_originator'];
      $originator = substr($originator, 0, 32);
    }

    return array(
      'identification' => array(
        'sender' => $sender_id,
        'hash' => $hash,
        'originator' => $originator,
      ),
    );
  }

  public function createRequest() {
    $this->request = array();
    $this->requestErrors = array();

    if (empty($this->order)) {
      $this->requestErrors[] = t('Missing order');
      return $this;
    }

    $order = $this->order;
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

    // Exit if no kiala point
    $point_id = $order_wrapper->kiala_point_id->value();
    $point_details = $order_wrapper->kiala_point->point_details->value();
    $short_id = $point_details['shortId'];
    if (empty($point_id)) {
      return $this;
    }

    // Exit if no order id
    if (empty($order->order_id)) {
      $this->requestErrors[] = t('Missing order id');
      return $this;
    }

    if (!commerce_kiala_ps_ws_order_access($order)) {
      $this->requestErrors[] = t('Web service constraints are not met for the order.');
      return $this;
    }

    // Exit if no recipient address or not enough info
    $recipient_address = commerce_kiala_order_recipient_addressfield($order);
    if (empty($recipient_address) || empty($recipient_address['country']) ||
        empty($recipient_address['postal_code']) || empty($recipient_address['last_name'])) {
      return $this;
    }

    // Get decrypted settings
    $settings = $this->settings;

    // set reference number
    $reference_number = $this->getReferenceNumber();
    if (empty($reference_number)) {
      return $this;
    }

    // calculate order total weight
    $weight = commerce_physical_order_weight($order, 'kg');

    // get order completed date
    $order_completed_date = commerce_kiala_order_completed_date($order);

    // build request
    $request = array(
      'reference' => $reference_number,
      'delivery' => array(
        'from' => array(
          'country' => $this->processValue($settings['country'], 2),
          'node' => '',
        ),
        'to' => array(
          'country' => $this->processValue($recipient_address['country'], 2),
          'node' => substr($short_id, 0, 5),
        ),
      ),
      'parcel' => array(
        'description' => '',
        'weight' => sprintf("%01.3f", round($weight['weight'], 3)),
        'orderNumber' => $this->getOrderNumber(),
        'orderDate' => format_date($order_completed_date, 'custom', 'Y-m-d'),
      ),
      'receiver' => array(
        'surname' => $this->processValue($recipient_address['last_name'], 100),
        'address' => array(
          'postalCode' => $this->processValue($recipient_address['postal_code'], 20),
          'country' => $this->processValue($recipient_address['country'], 2),
        ),
        'email' => $this->processValue($order->mail, 320),
        'language' => language_default()->language,
      )
    );

    // Add optional parameters
    if (!empty($recipient_address['first_name'])) {
      $request['receiver']['firstName'] = $this->processValue($recipient_address['first_name'], 100);
    }
    else {
      $request['receiver']['firstName'] = '';
    }

    if (!empty($recipient_address['thoroughfare'])) {
      $request['receiver']['address']['line1'] = $this->processValue($recipient_address['thoroughfare'], 200);
    }
    else {
      $request['receiver']['address']['line1'] = '';
    }

    if (!empty($recipient_address['premise'])) {
      $request['receiver']['address']['line2'] = $this->processValue($recipient_address['premise'], 200);
    }
    else {
      $request['receiver']['address']['line2'] = '';
    }

    if (!empty($recipient_address['locality'])) {
      $request['receiver']['address']['city'] = $this->processValue($recipient_address['locality'], 100);
    }
    else {
      $request['receiver']['address']['city'] = '';
    }

    $this->request = $request;
    return $this;
  }

  public function sendRequest($method = 'createOrder') {
    if (empty($method)) {
      $method = 'createOrder';
    }

    $service_url = $this->getServiceURL();
    if (empty($service_url)) {
      watchdog('commerce_kiala_ps', 'Kiala WS: Missing API url', array(), WATCHDOG_ERROR);
      return $this->createErrorResponse(t('Missing API url'));
    }

    // Create request if not set
    if (empty($this->request)) {
      $this->createRequest();
    }

    // exit if no request
    if (empty($this->request)) {
      if (!empty($this->requestErrors)) {
        $message = implode('<br />', $this->requestErrors);
        watchdog('commerce_kiala_ps', 'Kiala WS: error while building the request.<br />' . $message, array(), WATCHDOG_ERROR);
        return $this->createErrorResponse($message);
      }

      return NULL;
    }

    // exit if no reference set
    if (empty($this->request['reference'])) {
      return $this->createErrorResponse(t('No order reference number provided.'));
    }

    // set identification
    $identification = $this->getIdentification();
    if (empty($identification)) {
      return $this->createErrorResponse(t('Missing or invalid credentials.'));
    }

    // merge identification
    $request = $identification + $this->request;

    // Allow other modles the ability to alter the request before sending.
    drupal_alter('commerce_kiala_ps_ws_send_soap_request', $request, $method);

    // Perform service call
/*
// Kiala PS web service testing
$response = new stdClass;
$response->status = 'success';
$response->trackingNumber = 'CC000000' . substr(sprintf("%1$03d", $this->order->order_id), 0, 3);
return $response;
*/

    try {
      // Create the SOAP client.
      $client = new SoapClient($service_url, array(
        'trace' => 1,
        'cache_wsdl' => WSDL_CACHE_NONE,
      ));

      // Attempt the SOAP request.
      $response = $client->$method($request);

      // Log errors
      if (isset($response->error)) {
        $response->status = 'error';
        watchdog('commerce_kiala_ps', 'Kiala PS: Request failed<br /><b>Code:</b> @code <br /><b>Message:</b> @message', array(
                '@code' => $response->error->code,
                '@message' => $response->error->message,
        ), WATCHDOG_ERROR);
      }
      else {
        $response->status = 'success';
      }

      return $response;
    }
    catch (SoapFault $exception) {
      $fault_string = '';
      if (isset($exception->faultstring)) {
        $fault_string = $exception->faultstring;
      }

      $orderfault_code = 'unknown order fault code';
      if (isset($exception->detail->orderFault->faultCode)) {
        $orderfault_code = $exception->detail->orderFault->faultCode;
      }

      $data_error_code = 'unknown data error code';
      if(isset($exception->detail->orderFault->dataError->errorCode)) {
          $data_error_code = $exception->detail->orderFault->dataError->errorCode;
      }

      $data_error_message = 'No data error message defined';
      if(isset($exception->detail->orderFault->dataError->message)) {
          $data_error_message = $exception->detail->orderFault->dataError->message;
      }

      $orderfault_message = 'No order fault message defined';
      if (isset($exception->detail->orderFault->message)) {
        $orderfault_message = $exception->detail->orderFault->message;
      }

      watchdog('commerce_kiala_ps', 'Kiala PS: SOAP Fault @faultstring<br /><b>Fault Code:</b> @detailcode <br /><b>Fault Message:</b> @detailmessage <br/> <b>Data Error Code:</b> @dataErrorCode <br/><b>Data Error Message:</b> @dataErrorMessage', array(
              '@faultstring' => $fault_string,
              '@detailcode' => $orderfault_code,
              '@detailmessage' => $orderfault_message,
              '@dataErrorMessage' => $data_error_message,
              '@dataErrorCode' => $data_error_code,
      ), WATCHDOG_ERROR);

      return $this->createErrorResponse('Fault string:  ' . $fault_string . ',  Fault Message:  ' . $orderfault_message, $data_error_message, $orderfault_code, $data_error_code);
    }
    return $this->createErrorResponse(t('An unknown error has occurred.'));
  }

  /**
   * Returns a pseudo error response consistent with WS response errors
   */
  protected function createErrorResponse($fault_message, $data_error_message = NULL, $fault_code = NULL, $data_error_code = NULL) {
    $response = new stdClass;
    $response->status = 'error';
    $response->error->fault_code = isset($fault_code) ? $fault_code : self::DEFAULT_FAULT_ERROR_CODE;
    $response->error->fault_message = $fault_message;
    $response->error->data_error_code = isset($data_error_code) ? $data_error_code : self::DEFAULT_DATA_ERROR_CODE;
    $response->error->data_error_message = isset($data_error_message) ? $data_error_message : self::DEFAULT_DATA_ERROR_MESSAGE;

    return $response;
  }

  /**
   * Process data for Web Service request
   */
  protected function processValue($x, $max_length = NULL) {
    $y = trim($x);
    $y = preg_replace('/(?:\n|\r|\r\n)+/', '; ', $y);
    $y = check_plain($y);

    if (!empty($max_length)) {
      $y = substr($y, 0, $max_length);
    }

    return $y;
  }
}
