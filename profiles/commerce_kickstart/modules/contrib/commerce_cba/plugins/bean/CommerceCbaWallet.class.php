<?php
/**
 * @file
 *
 * Plugin for Commerce Checkout by Amazon wallet widget.
 */

class CommerceCbaWallet extends BeanPlugin {
  /**
   * Declares default block settings.
   */
  public function values() {
    return array(
      'settings' => array(
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
    // There's no point in displaying the button if amazon js was not included.
    if (($js = commerce_cba_javascript_file()) && ($purchase_contract_id = commerce_cba_get_purchase_contract_id())) {
      $html_id = drupal_html_id('AmazonWalletWidget');
      $content['bean'][$bean->delta]['#attached']['library'] = array(array('commerce_cba', 'amazon_widgets'));
      // @TODO: Add height and width.
      $callbacks = array('callbacks' => array('onPaymentSelect' => 'commerce_cba_add_widget_info'));
      $display_mode = ($view_mode == 'commerce_cba_read_only') ? 'Read' : 'Edit';
      $data = array(
        'commerce_cba' => array(
          $html_id => array(
            'html_id' => $html_id,
            'widget_type' => 'WalletWidget',
            'merchantId' => variable_get('cba_merchant_id', ''),
            'purchaseContractId' => commerce_cba_get_purchase_contract_id(),
            'displayMode' => $display_mode,
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
