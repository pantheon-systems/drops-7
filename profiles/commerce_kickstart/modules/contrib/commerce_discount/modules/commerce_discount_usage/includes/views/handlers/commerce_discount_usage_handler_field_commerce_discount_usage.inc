<?php

/**
 * Field handler to show commerce discount operations in dropbutton.
 */
class commerce_discount_usage_handler_field_commerce_discount_usage extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields['discount_id'] = 'discount_id';

    // Set real_field in order to make it generate a field_alias.
    $this->real_field = 'discount_id';
  }

  function render($values) {
    $discount_id = $this->get_value($values, 'discount_id');
    $commerce_discount = entity_load_single('commerce_discount', $discount_id);
    $wrapper = entity_metadata_wrapper('commerce_discount', $commerce_discount);

    $max_uses = $wrapper->commerce_discount_max_uses->value() ? t('@max available', array('@max' => $wrapper->commerce_discount_max_uses->value())) : t('Unlimited');
    $uses = t('Used @used times', array('@used' => (int)$wrapper->commerce_discount_uses->value()));

    return theme('item_list', array('items' => array($max_uses, $uses)));
  }
}
