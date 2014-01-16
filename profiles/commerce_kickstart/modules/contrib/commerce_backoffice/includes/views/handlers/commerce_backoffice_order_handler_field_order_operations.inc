<?php

/**
 * Displays the order operations in a ctools dropbutton.
 *
 * @ingroup views_field_handlers
 */
class commerce_backoffice_order_handler_field_order_operations extends views_handler_field_entity {

  function option_definition() {
    $options = parent::option_definition();
    $options['add_destination'] = TRUE;

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['add_destination'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add a destination parameter to operations links so users return to this View on form submission.'),
      '#default_value' => $this->options['add_destination'],
    );
  }

  function render($values) {
    $order_id = $this->get_value($values, 'order_id');
    $links = menu_contextual_links('commerce-order', 'admin/commerce/orders', array($order_id));
    // Remove the View link, that functionality opens in a megarow.
    array_shift($links);

    // Add the quick-edit link which opens the megarow.
    $quick_edit['quick-edit'] = menu_get_item('commerce_backoffice/order/' . $order_id . '/' . $this->view->current_display);
    $quick_edit['quick-edit']['title'] = t('Quick edit');
    $quick_edit['quick-edit']['attributes']['class'] = 'views-megarow-open';
    $links = array_merge($quick_edit, $links);

    // Add the destination to the links if specified.
    if ($this->options['add_destination']) {
      foreach ($links as $id => &$link) {
        $link['query'] = drupal_get_destination();
      }
    }

    $element = array(
      '#markup' => theme('links__ctools_dropbutton', array('links' => $links, 'attributes' => array('class' => array('links', 'inline')))),
    );

    return $element;
  }
}
