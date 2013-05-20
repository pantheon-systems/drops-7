<?php
/**
 * @file
 *
 * Commerce checkout by Amazon class for API requests.
 */

class amazonAPI {

  private $purchaseContractId = '';
  private $country = 'gb';
  private $mode = 'sandbox';
  private $method = 'POST';
  private $merchant_id = '';
  private $publicKey = '';
  private $secretKey = '';
  private $endPoint = array();

  public function __construct() {
    $this->purchaseContractId = commerce_cba_get_purchase_contract_id();
    $this->country = variable_get('cba_country', 'gb');
    $this->mode = variable_get('cba_mode', 'sandbox');
    $this->method = variable_get('cba_method', 'POST');
    $this->publicKey = variable_get('cba_public_key', '');
    $this->secretKey = variable_get('cba_secret_key', '');
    $this->merchant_id = variable_get('cba_merchant_id', '');
    $this->endPoint = $this->getEndPoint();
    $this->integratorId = $this->getIntegratorId();
    $this->integratorName = $this->getIntegratorName();
  }

  /**
   * Returns the url for doing requests.
   * @return array|bool
   */
  private function getEndpoint() {
    $return = array(
      'uri' => '/cba/api/purchasecontract/',
      'schema' => 'https://',
    );
    switch ($this->country) {
      case 'gb':
        $return['host'] = ($this->mode == 'sandbox') ? 'payments-sandbox.amazon.co.uk' : 'payments.amazon.co.uk';
        return $return;
      case 'de':
        $return['host'] = ($this->mode == 'sandbox') ? 'payments-sandbox.amazon.de' : 'payments.amazon.de';
        return $return;
      case 'us':
        $return['host'] = ($this->mode == 'sandbox') ? 'payments-sandbox.amazon.com' : 'payments.amazon.com';
        return $return;
    }
    return FALSE;
  }

  /**
   * Returns the integrator Identifier.
   */
  private function getIntegratorId() {
    switch ($this->country) {
      case 'gb':
        return 'A1ODS54A1SKVGY';
      case 'de':
        return 'A3S5FUFFGRZCKS';
      case 'us':
        return 'A21MYPIWD7BX8A';
    }
  }

  /**
   * Returns the integrator Identifier.
   */
  private function getIntegratorName() {
    switch ($this->country) {
      case 'gb':
        return 'CommerceGuysUK';
      case 'de':
        return 'CommerceGuysDE';
      case 'us':
        return 'CommerceGuys';
    }
  }

  /**
   * Transform the params array into a canonicalized query.
   *
   * @param $params
   * @return string
   */
  private function canonizeQuery($params) {
    ksort($params);
    $canonicalized_query = array();
    foreach ($params as $param => $value) {
      $param = str_replace('%7E', '~', rawurlencode($param));
      $value = str_replace('%7E', '~', rawurlencode($value));
      $canonicalized_query[] = $param . '=' . $value;
    }
    return implode("&", $canonicalized_query);
  }

  /**
   * Provide a SHA signature for the query.
   *
   * @param string $canonicalized_query
   * @return mixed
   */
  private function signQuery($canonicalized_query) {
    $string_to_sign = $this->method . "\n" . $this->endPoint['host'] . "\n" . $this->endPoint['uri'] . "\n" . $canonicalized_query;
    $signature = base64_encode(hash_hmac("sha256", $string_to_sign, $this->secretKey, TRUE));
    return str_replace("%7E", "~", rawurlencode($signature));
  }

  /**
   * Prepare query string.
   *
   * @param $params
   * @return string
   */
  private function prepareQuery($params) {
    $canonicalized_query = $this->canonizeQuery($params);
    $signature = $this->signQuery($canonicalized_query);
    return $this->endPoint['schema'] . $this->endPoint['host'] . $this->endPoint['uri'] . "?". $canonicalized_query. "&Signature=". $signature;
  }

  /**
   * Performs a call to Amazon API.
   *
   * @param string $query
   * @return bool|object
   */
  private function query($query) {
    $options = array(
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
      'method' => $this->method,
    );
    $response = drupal_http_request($query, $options);
    if ($response->code <> '200') {
      drupal_set_message(t('An error happened while processing the request'), 'error');
      watchdog('commerce_cba', 'Error while processing Amazon response: !response', array('!response' => print_r($response, TRUE)), WATCHDOG_ERROR);
    }
    return $response;
  }

  /**
   * Constructs the params array with the common query elements.
   *
   * @param $action
   * @param $additional_params
   * @return array
   */
  private function constructParams($action, $additional_params = array()) {
    return array(
      'Action' => $action,
      'AWSAccessKeyId' => $this->publicKey,
      'Timestamp' => $this->getFormattedTimestamp(),
      'Version' => "2010-08-31",
      'SignatureVersion' => '2',
      'SignatureMethod' => 'HmacSHA256',
      'PurchaseContractId' => $this->purchaseContractId,
    ) + $additional_params;
  }

  /**
   * Perform a contract query call to Amazon API.
   *
   * @param $action
   * @param array $additional_params
   * @return bool|object
   */
  public function contractQuery($action, $additional_params = array()) {
    $params = $this->constructParams($action, $additional_params);
    $query = $this->prepareQuery($params);
    return $this->query($query);
  }

  /**
   * Perform a Purchase Items query call to Amazon API.
   *
   * @param commerce_order $order
   * @internal param $action
   * @return bool|object
   */
  public function setPurchaseItems($order) {
    $params = $this->constructParams('SetPurchaseItems');
    $i = 0;

    // Set the expected delivery method for the order.
    $delivery_method = '#default';
    if (isset($order->data['commerce_cba'])) {
      if (!empty($order->data['commerce_cba']['shipping'])) {
        $delivery_method = 'shipping';
      }
      elseif (!empty($order->data['commerce_cba']['billing'])) {
        $delivery_method = 'billing';
      }
    }

    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    foreach ($order_wrapper->commerce_line_items as $line_item_wrapper) {
      if (in_array($line_item_wrapper->type->value(), commerce_product_line_item_types())) {
        $product_wrapper = $line_item_wrapper->commerce_product;
        $i++;
        $line_item = $line_item_wrapper->value();
        if (isset($line_item->data['context']) && isset($line_item->data['context']['entity'])) {
          $entity = entity_load_single($line_item->data['context']['entity']['entity_type'], $line_item->data['context']['entity']['entity_id']);
          $uri = entity_uri($line_item->data['context']['entity']['entity_type'], $entity);
          $url = isset($uri['path']) ? url($uri['path'], array('absolute' => 'TRUE')) : '';
          $wrapper = entity_metadata_wrapper($line_item->data['context']['entity']['entity_type'], $entity);
        }

        $price = commerce_price_wrapper_value($line_item_wrapper, 'commerce_unit_price');
        $unit_amount = commerce_currency_amount_to_decimal($price['amount'], $price['currency_code']);
        $unit_currency_code = $price['currency_code'];

        $base = 'PurchaseItems.PurchaseItem.' . $i . '.';
        $params[$base . 'MerchantItemId'] = $product_wrapper->sku->value();
        $params[$base . 'SKU'] = $product_wrapper->sku->value();
        $params[$base . 'MerchantId'] = $this->merchant_id;
        $params[$base . 'Title'] = $product_wrapper->title->value();
        $params[$base . 'Description'] = !empty($entity->body) ? $wrapper->body->value->value() : '';
        $params[$base . 'UnitPrice.Amount'] = $unit_amount;
        $params[$base . 'UnitPrice.CurrencyCode'] = $unit_currency_code;
        $params[$base . 'Quantity'] = (int) $line_item_wrapper->quantity->value();
        $params[$base . 'URL'] = isset($url) ? $url : '';
        $params[$base . 'FulfillmentNetwork'] = 'MERCHANT';
        $params[$base . 'ProductType'] = 'PHYSICAL';

        // Add the right delivery method address.
        $params[$base . 'PhysicalProductAttributes.DeliveryMethod.DestinationName'] = $delivery_method;
      }
    }

    // Allow other modules to alter the params on demand.
    drupal_alter('commerce_cba_purchase_items', $params, $order);

    $query = $this->prepareQuery($params);
    return $this->query($query);
  }

  /**
   * Perform a Contract Charges query call to Amazon API.
   *
   * @param commerce_order $order
   * @internal param $action
   * @return bool|object
   */
  public function setContractCharges($order) {
    $params = $this->constructParams('SetContractCharges');
    $discount_amount = $shipping_amount = $tax_amount = 0;
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    $total_price = commerce_price_wrapper_value($order_wrapper, 'commerce_order_total');
    $tax_amount = commerce_tax_total_amount($total_price['data']['components'], FALSE, $total_price['currency_code']);


    foreach ($order_wrapper->commerce_line_items as $line_item_wrapper) {
      if (!in_array($line_item_wrapper->type->value(), commerce_product_line_item_types())) {
        // Shipping line items.
        if ($line_item_wrapper->type->value() == 'shipping') {
          $shipping_amount += $line_item_wrapper->commerce_total->amount->value();
          $shipping_currency_code = $line_item_wrapper->commerce_total->currency_code->value();
        }

        // We assume that a negative amount is a discount or similar.
        if ($line_item_wrapper->commerce_total->amount->value() < 0) {
          $discount_amount += $line_item_wrapper->commerce_total->amount->value();
          $discount_currency_code = $line_item_wrapper->commerce_total->currency_code->value();
        }
      }

    }

    // Providing taxes and shipping information, even if it's 0.
    $params['Charges.Tax.Amount'] = commerce_currency_amount_to_decimal($tax_amount, $total_price['currency_code']);
    $params['Charges.Tax.CurrencyCode'] = $total_price['currency_code'];
    $shipping_currency_code = !empty($shipping_currency_code) ? $shipping_currency_code : $total_price['currency_code'];
    $params['Charges.Shipping.Amount'] = commerce_currency_amount_to_decimal($shipping_amount, $shipping_currency_code);
    $params['Charges.Shipping.CurrencyCode'] = $shipping_currency_code;

    if ($discount_amount <> 0) {
      $params['Charges.Promotions.Promotion.1.PromotionId'] = t('Discount');
      $params['Charges.Promotions.Promotion.1.Description'] = t('Discount');
      $params['Charges.Promotions.Promotion.1.Discount.Amount'] = abs(commerce_currency_amount_to_decimal($discount_amount, $discount_currency_code));
      $params['Charges.Promotions.Promotion.1.Discount.CurrencyCode'] = $discount_currency_code;
    }

    // Allow other modules to alter the params on demand.
    drupal_alter('commerce_cba_contract_charges', $params, $order);

    if (!empty($params)) {
      $query = $this->prepareQuery($params);
      return $this->query($query);
    }
  }

  /**
   * Complete call to Amazon API.
   *
   * @return bool|object
   */
  public function completePurchaseContract() {
    $additional_params = array(
      'IntegratorID' => $this->integratorId,
      'IntegratorName' => $this->integratorName,
    );
    $params = $this->constructParams('CompletePurchaseContract', $additional_params);
    $query = $this->prepareQuery($params);
    return $this->query($query);
  }

  /**
   * Return an array of order ids from an Order response.
   *
   * @param $response
   * @return array
   */
  public function getOrderIds($response) {
    if ($response->code === '200') {
      $data = new SimpleXMLElement($response->data);
      return (array) $data->CompletePurchaseContractResult->OrderIds;
    }
    return array();
  }

  public function getAddress($response, $destinationName = '#default') {
    if (!empty($response->data)) {
      $data = new SimpleXMLElement($response->data);
      foreach ($data->GetPurchaseContractResult->PurchaseContract->Destinations as $destination) {
        if ((string) $destination->Destination->DestinationName == $destinationName) {
          return (array) $destination->Destination->PhysicalDestinationAttributes->ShippingAddress;
        }
      }
    }

    return array();
  }

  /**
   * Formats date as ISO 8601 timestamp
   */
  protected function getFormattedTimestamp() {
    return gmdate("Y-m-d\TH:i:s.\\0\\0\\0\\Z", time());
  }

}
