<?php

/**
 * @file
 * Provides Inline Entity Form integration for the Commerce Discount module.
 */

/**
 * Defines the inline entity form controller for Commerce Discount Offers.
 */
class CommerceDiscountOfferInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * {@inheritdoc}
   */
  public function entityForm($entity_form, &$form_state) {
    // If we're cloning, we want to clone the offer entity too.
    if ($form_state['op'] === 'clone') {
      unset($entity_form['#entity']->discount_offer_id);
      $entity_form['#entity']->is_new = TRUE;
    }

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

    $offer_types = array();
    foreach (commerce_discount_offer_types() as $type => $info) {
      if (in_array($discount_type['entity type'], $info['entity types'])) {
        $offer_types[$type] = $info['label'];
      }
    }

    // Ensures the discount type includes the passed offer type. If not, set the
    // offer type to the first found valid offer.
    if (!in_array($offer->type, array_keys($offer_types))) {
      $offer->type = reset(array_keys($offer_types));
    }

    $ief_id = $entity_form['#ief_id'];
    field_attach_form('commerce_discount_offer', $offer, $entity_form, $form_state, LANGUAGE_NONE);

    $fields_wrapper = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('commerce-offer-fields-wrapper'),
      ),
      '#weight' => 2,
      '#parents' => array('commerce_discount_fields', 'commerce_discount_offer', LANGUAGE_NONE, 'form'),
    );

    // Transfer field elements onto fields container.
    foreach (element_children($entity_form) as $key) {
      $fields_wrapper[$key] = $entity_form[$key];
      unset($entity_form[$key]);
    }
    // Set the fields container back on the form.
    $entity_form['commerce_discount_offer_fields'] = $fields_wrapper;

    // Add offer type options.
    $entity_form['type'] = array(
      '#type' => 'radios',
      '#title' => t('Choose offer type'),
      '#title_display' => 'invisible',
      '#options' => $offer_types,
      '#required' => FALSE,
      '#default_value' => $offer->type,
      '#ajax' => array(
        'callback' => 'inline_entity_form_get_element',
        'wrapper' => 'inline-entity-form-' . $ief_id,
      ),
      '#weight' => 1,
    );

    // Add wrapper class for CSS to avoid form item collisions.
    $entity_form['#attributes']['class'][] = 'commerce-offer-type-wrapper';
    $entity_form['#type'] = 'fieldset';
    $entity_form['#title'] = t('Choose offer type');

    return $entity_form;
  }

}
