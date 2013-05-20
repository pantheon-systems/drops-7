<?php
/**
 * @file
 *
 * Plugin for Commerce Checkout by Amazon wallet widget.
 */

class CommerceCbaOrderDetails extends BeanPlugin {
  /**
   * Declares default block settings.
   */
  public function values() {
    return parent::values();
  }

  /**
   * Builds extra settings for the block edit form.
   */
  public function form($bean, $form, &$form_state) {
    return array();
  }

  /**
   * Displays the bean.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL, $test=NULL) {
    // There's no point in displaying the button if amazon js was not included.
    if (($js = commerce_cba_javascript_file()) && isset($bean->order) && ($order = $bean->order)) {
      if (!empty($order->commerce_cba_amazon_order_ids)) {
        $content['bean'][$bean->delta]['#attached']['library'] = array(array('commerce_cba', 'amazon_widgets'));

        $order_ids = field_get_items('commerce_order', $order, 'commerce_cba_amazon_order_ids');
        foreach ($order_ids as $order_id) {
          $html_id = drupal_html_id('OrderDetailsWidget');
          // @TODO: Add height and width.
          $data = array(
            'commerce_cba' => array(
              $html_id => array(
                'html_id' => $html_id,
                'widget_type' => 'OrderDetailsWidget',
                'merchantId' => variable_get('cba_merchant_id', ''),
                'orderId' => $order_id['value'],
              ),
            ),
          );

          $content['bean'][$bean->delta][$html_id]['#attached']['js'][] = array(
            'data' => $data,
            'type' => 'setting',
          );

          $content['bean'][$bean->delta][$html_id]['#type'] = 'container';
          $content['bean'][$bean->delta][$html_id]['#attributes'] = array('id' => $html_id);
        }
      }
    }

    return $content;
  }


}
