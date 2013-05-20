<?php
/**
 * @file
 *
 * Plugin for Commerce Checkout by Amazon address widget.
 */

class CommerceCbaAddress extends BeanPlugin {
  /**
   * Declares default block settings.
   */
  public function values() {
    return array(
      'settings' => array(
        'destination' => 'billing',
        'width' => 400,
        'height' => 180,
      ),
    ) + parent::values();
  }

  /**
   * Builds extra settings for the block edit form.
   */
  public function form($bean, $form, &$form_state) {
    $form = array();
    $form['settings'] = array(
      '#type' => 'fieldset',
      '#tree' => 1,
      '#title' => t('Options'),
    );

    $destination_options = commerce_customer_profile_type_get_name();
    $form['settings']['destination'] = array(
      '#type' => 'select',
      '#title' => t('Address destination'),
      '#description' => t('Select the customer profile that will be assigned as this address, select "None" if you are providing your own custom code for the integration.'),
      '#options' =>  $destination_options,
      '#default_value' => isset($bean->settings['destination']) ? $bean->settings['destination'] : '',
      '#empty_option' => t('None'),
    );

    // @TODO: Add a validation on the width and height depending on the display
    // mode.
    $form['settings']['width'] = array(
      '#type' => 'textfield',
      '#title' => t('Width of the widget'),
      '#description' => t('Width of the wiget in pixes, min: 150, max: 600 for read mode and min: 280, max: 600 for edit mode'),
      '#default_value' => isset($bean->settings['width']) ? $bean->settings['width'] : '',
    );

    $form['settings']['height'] = array(
      '#type' => 'textfield',
      '#title' => t('Height of the widget'),
      '#description' => t('Height of the wiget in pixes, min: 180, max: 400 for read mode and min: 230, max: 400 for edit mode'),
      '#default_value' => isset($bean->settings['height']) ? $bean->settings['height'] : '',
    );

    return $form;
  }

  /**
   * Displays the bean.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
    if ($purchase_contract_id = commerce_cba_get_purchase_contract_id()) {
      $html_id = drupal_html_id('AmazonAddressWidget');
      $content['bean'][$bean->delta]['#attached']['library'][] = array('commerce_cba', 'amazon_widgets');
      // @TODO: Add height and width.

      $callbacks = array('callbacks' => array('onAddressSelect' => 'commerce_cba_add_widget_info'));
      $display_mode = ($view_mode == 'commerce_cba_read_only') ? 'Read' : 'Edit';

      $data = array(
        'commerce_cba' => array(
          $html_id => array(
            'html_id' => $html_id,
            'purchaseContractId' => commerce_cba_get_purchase_contract_id(),
            'widget_type' => 'AddressWidget',
            'merchantId' => variable_get('cba_merchant_id', ''),
            'displayMode' => $display_mode,
            'destinationName' => isset($bean->settings['destination']) ? $bean->settings['destination'] : 'billing',
          ) + $callbacks,
        ),
      );

      $content['bean'][$bean->delta]['#attached']['js'][] = array(
        'data' => $data,
        'type' => 'setting',
      );

      $content['bean'][$bean->delta]['#type'] = 'container';
      $content['bean'][$bean->delta]['#attributes'] = array('id' => $html_id);
    }
    return $content;
  }


}
