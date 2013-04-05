<?php

/**
 * @file
 * Provides support for the Views module.
 */

/**
 * Commerce discount entity Views definitions.
 */
class CommerceDiscountViewsController extends EntityDefaultViewsController {

  /**
   * Override EntityDefaultViewsController::views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // Show the operations using CTools dropbutton menu.
    $data['commerce_discount']['operations_dropbutton'] = array(
      'title' => t('Operations'),
      'help' => t('Show the operation links using dropbutton menu.'),
      'field' => array(
        'handler' => 'commerce_discount_handler_field_operations_dropbutton',
      ),
      // This is a dummy field, so attach it to the ID column.
      'real field' => 'discount_id',
    );

    return $data;
  }

}
