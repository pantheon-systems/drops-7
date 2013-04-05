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
        $offer->type = $offer_values['type'];
      }
    }

    $options = array();
    foreach (commerce_discount_offer_types() as $type => $info) {
      $options[$type] = $info['label'];
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
