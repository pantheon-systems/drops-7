<?php
/**
 * @file
 *
 * Rules integration for commerce checkout by Amazon IOPN module.
 */

/**
 * Implements hook_rules_event_info().
 */
function commerce_cba_iopn_rules_event_info() {
  $info = array();

  $info['commerce_cba_iopn_new_order_notification'] = array(
    'group' => t('Commerce cba'),
    'label' => t('New Order notification received from Amazon'),
    'variables' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Commerce order'),
      ),
      'shipping_address' => array(
        'type' => 'struct',
        'label' => t('Shipping address returned by Amazon'),
      ),
      'buyer_info' => array(
        'type' => 'struct',
        'label' => t('Buyer info returned by Amazon'),
        'description' => t('Normally buyer name and email address'),
      ),
    ),
  );

  $info['commerce_cba_iopn_rts_order_notification'] = array(
    'group' => t('Commerce cba'),
    'label' => t('Ready to ship Order notification received from Amazon'),
    'variables' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Commerce order'),
      ),
      'shipping_address' => array(
        'type' => 'struct',
        'label' => t('Shipping address returned by Amazon'),
      ),
      'buyer_info' => array(
        'type' => 'struct',
        'label' => t('Buyer info returned by Amazon'),
        'description' => t('Normally buyer name and email address'),
      ),
    ),
  );

  $info['commerce_cba_iopn_cancel_order_notification'] = array(
    'group' => t('Commerce cba'),
    'label' => t('Cancel Order notification received from Amazon'),
    'variables' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Commerce order'),
      ),
      'shipping_address' => array(
        'type' => 'struct',
        'label' => t('Shipping address returned by Amazon'),
      ),
      'buyer_info' => array(
        'type' => 'struct',
        'label' => t('Buyer info returned by Amazon'),
        'description' => t('Normally buyer name and email address'),
      ),
    ),
  );

  return $info;
}

/**
 * Implements hook_rules_action_info().
 */
function commerce_cba_iopn_rules_action_info(){
  $info = array();

  $info['commerce_cba_iopn_save_customer_profile'] = array(
    'group' => t('Commerce cba'),
    'label' => t('Create/alter the customer profile from the address returned by Amazon'),
    'description' => t('This will override the customer profile associated to the order if any.'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Commerce order'),
        'save' => TRUE,
      ),
      'customer_profile_field' => array(
        'type' => 'commerce_cba_iopn_select_profile_field',
        'label' => t('Customer profile type'),
        'restriction' => 'input',
      ),
      'shipping_address' => array(
        'type' => 'struct',
        'label' => t('Shipping address returned by Amazon'),
      ),
    ),
    'provides' => array(
      'commerce_customer_profile' => array(
        'type' => 'commerce_customer_profile',
        'label' => t('Cusomer profile saved'),
      ),
    ),
  );

  return $info;
}

/**
 * Implements hook_rules_data_info().
 */
function commerce_cba_iopn_rules_data_info() {
  $data['commerce_cba_iopn_select_profile_field'] = array(
    'label' => t('Select customer profile'),
    'ui class' => 'RulesDataUICommerceCbaIopnProfile',
  );
  return $data;
}

function commerce_cba_iopn_save_customer_profile($order, $profile_name, $shipping_address) {
  $profile_field = 'commerce_customer_' . $profile_name;
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  if (!empty($order->{$profile_field})) {
    $customer_profile = $order_wrapper->{$profile_field}->value();
  }
  else {
    $customer_profile = commerce_customer_profile_new($profile_name, $order->uid);
  }

  $address = array(
    'country' => $shipping_address['CountryCode'],
    'name_line' => $shipping_address['Name'],
    'postal_code' => $shipping_address['PostalCode'],
    'locality' => $shipping_address['City'],
    'thoroughfare' => isset($shipping_address['AddressFieldTwo']) ? $shipping_address['AddressFieldTwo'] : $shipping_address['AddressFieldOne'],
    'administrative_area' => $shipping_address['State'],
    'data' => serialize($shipping_address),
  );
  $customer_profile->commerce_customer_address[LANGUAGE_NONE][0] = array_merge($customer_profile->commerce_customer_address[LANGUAGE_NONE][0], $address);
  commerce_customer_profile_save($customer_profile);

  $order_wrapper->{$profile_field} = $customer_profile->profile_id;

  return $customer_profile;
}

/**
 * Adds a customer field selection.
 */
class RulesDataUICommerceCbaIopnProfile extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $profile_types = commerce_customer_profile_types();
    $options = array();
    foreach ($profile_types as $id => $type) {
      if ($type['addressfield'] == TRUE) {
        $options[$id] = $type['name'];
      }
    }

    $form[$name] = array(
      '#type' => 'select',
      '#title' => t('Select the customer profile field to store the address'),
      '#options' => $options,
    );

    return $form;
  }

  public static function render($value) {
    return array();
  }

}
