<?php

/**
 * Display a form for editing the price and status of a variation.
 *
 * @ingroup views_field_handlers
 */
class commerce_backoffice_product_handler_field_product_quick_edit_form extends views_handler_field {

  function render($values) {
    // Get the entity matching this row.
    $results = array($values);
    $entities = $this->query->get_result_entities($results);
    $entity = reset($entities[1]);
    $form = drupal_get_form('commerce_backoffice_product_quick_edit_form', $entity);

    return drupal_render($form);
  }
}
