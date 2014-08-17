<?php

/**
 * @file
 *   Contains the field handler for the node relist form.
 */

/**
 * Views field handler for the node relist form.
 *
 * @ingroup views
 */
class commerce_node_checkout_expire_field_node_relist_form extends views_handler_field_entity {
  /**
   * Implements parent::query().
   */
  function query() {
    $this->add_additional_fields(array('node.type', 'node.uid', 'node.status'));
    parent::query();
  }

  /**
   * Implements parent::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['cart_link_text'] = array('default' => t('In shopping cart'));
    return $options;
  }

  /**
   * Implements parent::option_form().
   */
  function options_form(&$form, &$form_state) {
    $form['cart_link_text'] = array(
      '#type' => 'textfield',
      '#title' => t('Shopping cart link text'),
      '#default_value' => $this->options['cart_link_text'],
      '#description' => t('If this node is already in the shopping cart, instead of the form appearing, a link to the shopping cart will. Omitting this text will make nothing appear.'), 
    );
    parent::options_form($form, $form_state);
  }

  /**
   * Implements parent::render().
   */
  function render($values) {
    // Build a mock node object so we can avoid loading
    $node = new stdClass;
    $node->nid = $values->nid;
    $node->title = $values->node_title;
    $node->type = $values->nodetype;
    $node->uid = $values->nodeuid;
    $node->status = $values->nodestatus;

    // See if this node is already in the user's cart
    if (commerce_node_checkout_expire_node_in_cart($node)) {
      // See if we should provide a shopping cart link
      if ($link_text = $this->options['cart_link_text']) {
        return l(t($link_text), 'cart');
      }
      // Stop here
      return;
    }

    // See if the user has access to relist this node
    if (commerce_node_checkout_expire_relist_access($node)) {
      // Get the form by appending the node ID so each form is technically
      // a completely different form. If we didn't do this, the forms
      // would collide since there can be multiple on the same page.
      $form = drupal_get_form('commerce_node_checkout_expire_relist_form_' . $node->nid, $node, FALSE);

      // Render the form
      return drupal_render($form);
    }
    
    return NULL;
  }
}
