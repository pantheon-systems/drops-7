<?php

/**
 * Entity class representing the commerce_discount_offer entity type.
 */
class CommerceDiscountOffer extends Entity {

  /**
   * The discount offer type (bundle).
   *
   * @var string
   */
  public $type;

  public function __construct($values = array()) {
    parent::__construct($values, 'commerce_discount_offer');
  }
}
