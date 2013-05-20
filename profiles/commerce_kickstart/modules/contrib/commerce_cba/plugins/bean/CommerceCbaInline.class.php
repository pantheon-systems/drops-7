<?php
/**
 * @file
 *
 * Plugin for Commerce Checkout by Amazon inline widget.
 */

class CommerceCbaInline extends BeanPlugin {
  /**
   * Declares default block settings.
   */
  public function values() {
    return array(
      'settings' => array(
        'button_type' => FALSE,
        'addresswidget' => 'none',
        'button_settings' => array(
          'size' => 'large',
          'color' => 'orange',
          'background' => 'white',
        ),
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

    $form['settings']['button_type'] = array(
      '#type' => 'select',
      '#title' => t('Button type'),
      '#description' => t('The value of this parameter determines what type of button to display. Possible values are "checkout" or "addressBook".'),
      '#options' => drupal_map_assoc(array('checkout', 'addressBook')),
      '#default_value' => isset($bean->settings['button_type']) ? $bean->settings['button_type'] : '',
    );

    // Load all the beans that could be included in the options.
    $options = array();
    $beans = commerce_bean_get_beans('commerce_cba_address');
    foreach ($beans as $address_bean) {
      $options[$address_bean->delta] = $address_bean->adminTitle();
    }

    $form['settings']['addresswidget'] = array(
      '#type' => 'select',
      '#title' => t('Select the address widget associated with this one'),
      '#description' => t('Only Address Widget beans are eligible.'),
      '#options' => $options,
      '#empty_option' => t('None'),
      '#default_value' => isset($bean->settings['addresswidget']) ? $bean->settings['addresswidget'] : '',
      '#states' => array(
        'visible' => array(
          ':input[name="settings[button_type]"]' => array('value' => 'addressBook'),
        ),
      ),
    );

    $form['settings']['button_settings'] = array(
      '#type' => 'fieldset',
      '#tree' => 2,
      '#title' => t('Settings of the button'),
    );

    $form['settings']['button_settings']['size'] = array(
      '#type' => 'select',
      '#title' => t('Size of the button'),
      '#options' => array('medium' => t('Medium (126x24)'), 'large' => t('Large (151x27)'), 'x-large' => t('Extra large (173x27)')),
      '#default_value' => isset($bean->settings['button_settings']['size']) ? $bean->settings['button_settings']['size'] : '',
    );
    $form['settings']['button_settings']['color'] = array(
      '#type' => 'select',
      '#title' => t('Color of the button'),
      '#options' => array('orange' => t('Orange'), 'tan' => t('Tan')),
      '#default_value' => isset($bean->settings['button_settings']['color']) ? $bean->settings['button_settings']['color'] : '',
    );
    $form['settings']['button_settings']['background'] = array(
      '#type' => 'select',
      '#title' => t('Background of the button'),
      '#options' => array('white' => t('White'), 'light' => t('Light'), 'dark' => t('Dark')),
      '#default_value' => isset($bean->settings['button_settings']['background']) ? $bean->settings['button_settings']['background'] : '',
    );

    return $form;
  }

  /**
   * Displays the bean.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
    // There's no point in displaying the button if amazon js was not included.
    if ($js = commerce_cba_javascript_file()) {
      $html_id = drupal_html_id('AmazonInlineWidget');
      $content['bean'][$bean->delta]['#attached']['library'] = array(array('commerce_cba', 'amazon_widgets'));

      $callbacks = array();
      if ($bean->settings['button_type'] == 'checkout') {
        $callbacks = array('callbacks' => array('onAuthorize' => 'commerce_cba_redirect_checkout'));
      }
      elseif ($bean->settings['button_type'] == 'addressBook') {
        $callbacks = array('callbacks' => array('onAuthorize' => 'commerce_cba_address_redirect_checkout'));
      }
      $content['bean'][$bean->delta]['#attached']['js'][] = array(
        'data' => array(
          $html_id => $html_id,
          'commerce_cba' => array(
            $html_id => array(
              'merchantId' => variable_get('cba_merchant_id', ''),
              'purchaseContractId' => commerce_cba_get_purchase_contract_id(),
              'widget_type' => 'InlineCheckoutWidget',
              'checkout_pane' => isset($bean->checkout_pane) ? $bean->checkout_pane : NULL,
              'settings' => array(
                'buttonType' => isset($bean->settings['button_type']) ? $bean->settings['button_type'] : 'checkout',
                'buttonSettings' => array(
                  'size' => isset($bean->settings['button_settings']['size']) ? $bean->settings['button_settings']['size'] : 'large',
                  'color' => isset($bean->settings['button_settings']['color']) ? $bean->settings['button_settings']['color'] : 'orange',
                  'background' => isset($bean->settings['button_settings']['background']) ? $bean->settings['button_settings']['background'] : 'white',
                ),
               ),
            ) + $callbacks,
          )),
        'type' => 'setting',
      );
      $content['bean'][$bean->delta]['#attached']['css'] = array(drupal_get_path('module', 'commerce_cba') . '/css/commerce_cba.css');

      $content['bean'][$bean->delta]['#type'] = 'container';
      $content['bean'][$bean->delta]['#attributes'] = array('id' => $html_id);
      // Place the button aligned to the right if it's checkout.
      if ($bean->settings['button_type'] == 'checkout') {
        $content['bean'][$bean->delta]['#attributes']['class'] = 'checkout-by-amazon-pay';
      }
    }
    return $content;
  }
}
