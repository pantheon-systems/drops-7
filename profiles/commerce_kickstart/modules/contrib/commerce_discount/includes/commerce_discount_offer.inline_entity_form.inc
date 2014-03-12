<?php

/**
 * @file
 * Defines the inline entity form controller for Commerce Discount Offers.
 */

class CommerceDiscountOfferInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   */
  public function entityForm($entity_form, &$form_state) {
    $offer = $entity_form['#entity'];
    if (!empty($form_state['values'])) {
      $offer_values = drupal_array_get_nested_value($form_state['values'], $entity_form['#parents']);
      // If the type was changed via AJAX, change it on the entity.
      if (!empty($offer_values['type'])) {
        // We have to use the formstate to hold this, because if other elements
        // set #limit_validation_errors, form state values will not contain what
        // we need.
        $form_state['commerce_dicount_offer_selection_' . $entity_form['#ief_id']] = $offer_values['type'];
      }
    }

    if (isset($form_state['commerce_dicount_offer_selection_' . $entity_form['#ief_id']])) {
      $offer->type = $form_state['commerce_dicount_offer_selection_' . $entity_form['#ief_id']];
    }

    // Get discount type.
    $discount_type = commerce_discount_type($form_state['commerce_discount']->type);

    $options = array();
    foreach (commerce_discount_offer_types() as $type => $info) {
      if (in_array($discount_type['entity type'], $info['entity types'])) {
        $options[$type] = $info['label'];
      }
    }

    // Ensures the discount type includes the passed offer type. If not, set the
    // offer type to the first found valid offer.
    if (!in_array($offer->type, array_keys($options))) {
      $offer->type = reset(array_keys($options));
    }

    $entity_form['type'] = array(
      '#title' => t('Choose offer type'),
      '#type' => 'radios',
      '#options' => $options,
      '#required' => FALSE,
      '#default_value' => $offer->type,
      '#ajax' => array(
         'callback' => 'inline_entity_form_get_element',
         'wrapper' => 'inline-entity-form-' . $entity_form['#ief_id'],
      ),
    );

    field_attach_form('commerce_discount_offer', $offer, $entity_form, $form_state, LANGUAGE_NONE);

    return $entity_form;
  }
}
