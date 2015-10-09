<?php

/**
 * @file
 * Provides a field handler for the Views module.
 */

/**
 * Field handler to show commerce discount operations in dropbutton.
 */
class commerce_discount_handler_field_operations_dropbutton extends views_handler_field {

  /**
   * {@inheritdoc}
   */
  public function construct() {
    parent::construct();
    $this->additional_fields['discount_id'] = 'discount_id';

    // Set real_field in order to make it generate a field_alias.
    $this->real_field = 'discount_id';
  }

  /**
   * {@inheritdoc}
   */
  public function render($values) {
    // Load the line item and return its title.
    $discount_id = $this->get_value($values, 'discount_id');
    $commerce_discount = entity_load_single('commerce_discount', $discount_id);

    $entity_info = entity_get_info('commerce_discount');
    $path = $entity_info['admin ui']['path'] . '/manage';

    // Preapre the links.
    $links = array();
    $links[] = array(
      'href'  => $path . '/' . $commerce_discount->name,
      'title' => t('edit'),
    );

    if (!entity_has_status('commerce_discount', $commerce_discount, ENTITY_IN_CODE)) {
      $links[] = array(
        'href'  => $path . '/' . $commerce_discount->name . '/delete',
        'title' => t('delete'),
        'query' => drupal_get_destination(),
      );
    }
    elseif (entity_has_status('commerce_discount', $commerce_discount, ENTITY_OVERRIDDEN)) {
      $links[] = array(
        'href'  => $path . '/' . $commerce_discount->name . '/revert',
        'title' => t('revert'),
        'query' => drupal_get_destination(),
      );
    }

    $links[] = array(
      'href'  => $path . '/' . $commerce_discount->name . '/clone',
      'title' => t('clone'),
    );

    $links[] = array(
      'href'  => $path . '/' . $commerce_discount->name . '/export',
      'title' => t('export'),
    );

    return theme('links__ctools_dropbutton', array('links' => $links));
  }

}
