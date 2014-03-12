<?php

/**
 * Entity class representing the commerce_discount entity type.
 */
class CommerceDiscount extends Entity {

  /**
   * The machine name of the discount offer.
   *
   * @var string
   */
  public $name;

  /**
   * The label of the discount.
   *
   * @var string
   */
  public $label;

  /**
   * Set the default discount type (bundle).
   *
   * @var string
   */
  public $type = 'order_discount';

  /**
   * Whether the discount is active.
   *
   * @var bool
   */
  public $status = TRUE;

  /**
   * Set the component price title shown for customers.
   *
   * @var string
   */
  public $component_title;

  public function __construct($values = array()) {
    parent::__construct($values, 'commerce_discount');
  }

  protected function defaultUri() {
    $entity_info = entity_get_info('commerce_discount');
    $path = $entity_info['admin ui']['path'] . '/manage';
    return array('path' => $path . '/' . $this->name);
  }
}
