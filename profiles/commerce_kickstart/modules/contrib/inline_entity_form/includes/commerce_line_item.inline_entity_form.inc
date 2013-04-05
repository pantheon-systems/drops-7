<?php

/**
 * @file
 * Defines the inline entity form controller for Commerce Line Items.
 */

class CommerceLineItemInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * Overrides EntityInlineEntityFormController::defaultFields().
   */
  public function defaultFields($bundles) {
    $fields = array();
    $fields['line_item_label'] = array(
      'type' => 'property',
      'label' => t('Label'),
      'visible' => TRUE,
      'weight' => 1,
    );
    $fields['commerce_unit_price'] = array(
      'type' => 'field',
      'label' => t('Unit price'),
      'formatter' => 'commerce_price_formatted_amount',
      'settings' => array(),
      'visible' => TRUE,
      'weight' => 2,
    );
    $fields['quantity'] = array(
      'type' => 'property',
      'label' => t('Quantity'),
      'visible' => TRUE,
      'weight' => 3,
    );
    $fields['commerce_total'] = array(
      'type' => 'field',
      'label' => t('Total'),
      'formatter' => 'commerce_price_formatted_amount',
      'settings' => array(),
      'visible' => TRUE,
      'weight' => 4,
    );

    return $fields;
  }

  /**
   * Overrides EntityInlineEntityFormController::defaultSettings().
   */
  public function defaultSettings() {
    $defaults = parent::defaultSettings();

    // Line items should always be deleted when the order is deleted, they
    // are never managed alone.
    $defaults['delete_references'] = TRUE;

    return $defaults;
  }

  /**
   * Overrides EntityInlineEntityFormController::settingsForm().
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);

    // Adding existing entities is not supported for line items.
    unset($form['allow_existing']);
    unset($form['match_operator']);

    return $form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   */
  public function entityForm($entity_form, &$form_state) {
    $line_item = $entity_form['#entity'];
    $extra_fields = field_info_extra_fields('commerce_line_item', $line_item->type, 'form');

    $entity_form['line_item_details'] = array(
      '#type' => 'fieldset',
      '#title' => t('Line item details'),
      '#attributes' => array('class' => array('ief-line_item-details', 'ief-entity-fieldset')),
    );
    $entity_form['line_item_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Line item label'),
      '#description' => t('Supply the line item label to be used for this line item.'),
      '#default_value' => $line_item->line_item_label,
      '#maxlength' => 128,
      '#required' => TRUE,
      '#weight' => $extra_fields['label']['weight'],
      '#fieldset' => 'line_item_details',
    );
    $entity_form['quantity'] = array(
      '#type' => 'textfield',
      '#datatype' => 'integer',
      '#title' => t('Quantity'),
      '#description' => t('The quantity of line items.'),
      '#default_value' => (int) $line_item->quantity,
      '#size' => 4,
      '#maxlength' => max(4, strlen($line_item->quantity)),
      '#required' => TRUE,
      '#weight' => $extra_fields['label']['weight'],
      '#fieldset' => 'line_item_details',
    );
    field_attach_form('commerce_line_item', $line_item, $entity_form, $form_state);

    // If the order is still in a cart status, then the prices are still being
    // recalculated, meaning that no changes made to them would be permanent.
    if (!empty($form_state['commerce_order']) && commerce_cart_order_is_cart($form_state['commerce_order'])) {
      $language = $entity_form['commerce_unit_price']['#language'];
      $entity_form['commerce_unit_price'][$language][0]['amount']['#disabled'] = TRUE;
      $entity_form['commerce_unit_price'][$language][0]['currency_code']['#disabled'] = TRUE;
    }

    // Tweaks specific to product line items.
    if (in_array($line_item->type, commerce_product_line_item_types())) {
      $entity_form['line_item_label']['#access'] = FALSE;
      $entity_form['commerce_display_path']['#access'] = FALSE;
      $entity_form['commerce_product']['#weight'] = -100;
    }

    // Add all fields to the main fieldset.
    foreach (field_info_instances('commerce_line_item', $line_item->type) as $a => $instance) {
      $entity_form[$instance['field_name']]['#fieldset'] = 'line_item_details';
    }

    return $entity_form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormValidate().
   *
   * @todo Remove once Commerce gets a quantity #element_validate function.
   */
  public function entityFormValidate($entity_form, &$form_state) {
    $line_item = $entity_form['#entity'];

    $parents_path = implode('][', $entity_form['#parents']);
    $line_item_values = drupal_array_get_nested_value($form_state['values'], $entity_form['#parents']);
    $quantity = $line_item_values['quantity'];

    if (!is_numeric($quantity) || $quantity <= 0) {
      form_set_error($parents_path . '][quantity', t('You must specify a positive number for the quantity'));
    }
    elseif ($entity_form['quantity']['#datatype'] == 'integer' &&
      (int) $quantity != $quantity) {
      form_set_error($parents_path . '][quantity', t('You must specify a whole number for the quantity.'));
    }

    field_attach_form_validate('commerce_line_item', $line_item, $entity_form, $form_state);
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   */
  public function entityFormSubmit(&$entity_form, &$form_state) {
    $line_item = $entity_form['#entity'];
    $line_item_values = drupal_array_get_nested_value($form_state['values'], $entity_form['#parents']);
    $line_item->quantity = sprintf("%.2f", $line_item_values['quantity']);

    field_attach_submit('commerce_line_item', $line_item, $entity_form, $form_state);

    // Update the line item label based on the line item type.
    // Product line item types always get the product SKU as the label.
    if (in_array($line_item->type, commerce_product_line_item_types())) {
      $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
      $line_item->line_item_label = $line_item_wrapper->commerce_product->sku->value();
    }
    else {
      $line_item->line_item_label = trim($line_item_values['line_item_label']);
    }

    // Update the total price.
    commerce_line_item_rebase_unit_price($line_item);

    // Product line item types by default get the product SKU as the label.
    if (empty($line_item->line_item_id) && in_array($line_item->type, commerce_product_line_item_types())) {
      $product = commerce_product_load($line_item->commerce_product[LANGUAGE_NONE][0]['product_id']);
      $line_item->line_item_label = $product->sku;
    }
  }
}
