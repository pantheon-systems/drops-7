<?php

/**
 * Field handler to show commerce discount operations in dropbutton.
 */
class commerce_discount_usage_handler_field_commerce_discount_analytics extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields['discount_id'] = 'discount_id';

    // Set real_field in order to make it generate a field_alias.
    $this->real_field = 'discount_id';
  }

  function render($values) {
    // Wrap the discount entity found.
    $wrapper = entity_metadata_wrapper('commerce_discount', entity_load_single('commerce_discount', $this->get_value($values, 'discount_id')));

    return theme(
      'item_list',
      array(
        'items' => array(
          $wrapper->discount_usage_limit->value() ? t('@max available', array('@max' => $wrapper->discount_usage_limit->value())) : t('Unlimited'),
          $wrapper->discount_usage_per_person->value() ? t('Limit @max per customer', array('@max' => $wrapper->discount_usage_per_person->value())) : t('Unlimited usage per customer'),
          t('Used @used times', array('@used' => commerce_discount_usage_get_usage($wrapper->name->value())))
        )
      )
    );
  }
}
