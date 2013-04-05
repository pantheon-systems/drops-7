<?php

/**
 * @file
 * Hooks provided by the Commerce Physical Product module.
 */


/**
 * Allows modules to specify a different weight field name for the given entity
 * than the one determined by Commerce Physical Product.
 *
 * @param &$field_name
 *   The name of the field instance on the entity that is currently specified as
 *   the weight field to use in weight calculations.
 * @param $entity_type
 *   The type of entity the field is attached to.
 * @param $entity
 *   The actual entity whose weight value is being determined.
 *
 * @see commerce_physical_entity_weight_field_name()
 */
function hook_commerce_physical_entity_weight_field_name_alter(&$field_name, $entity_type, $entity) {
  // No example.
}

/**
 * Allows modules to alter the weight that has been determined for a product
 * line item.
 *
 * @param &$weight
 *   The weight field value array representing the weight of the product line
 *   item, which defaults to the weight of the product times the quantity.
 * @param $line_item
 *   The product line item object whose weight is being determined. The product
 *   data can be accessed through this line item's commerce_product field.
 *
 * @see commerce_physical_product_line_item_weight()
 */
function hook_commerce_physical_product_line_item_weight_alter(&$weight, $line_item) {
  // Add a one pound tare weight to the product for shipping calculation.
  $weight['weight'] += physical_weight_convert(array('weight' => 1, 'unit' => 'lb'), $weight['unit']);
}

/**
 * Allows modules to alter the weight that has been determined for an order.
 *
 * @param &$weight
 *   The weight field value array representing the weight of the order or NULL
 *   if no weight has yet been determined.
 * @param $order
 *   The order object whose weight is being determined.
 * @param $unit
 *   The unit of measurement to use for the returned weight of the order.
 *
 * @see commerce_physical_order_weight()
 */
function hook_commerce_physical_order_weight_alter(&$weight, $order, $unit) {
  // No example.
}

/**
 * Allows modules to specify a different dimensions field name for the given
 * entity than the one determined by Commerce Physical Product.
 *
 * @param &$field_name
 *   The name of the field instance on the entity that is currently specified as
 *   the dimensions field to use in dimensions and volume calculations.
 * @param $entity_type
 *   The type of entity the field is attached to.
 * @param $entity
 *   The actual entity whose dimensions value is being determined.
 *
 * @see commerce_physical_entity_dimensions_field_name()
 */
function hook_commerce_physical_entity_dimensions_field_name_alter(&$field_name, $entity_type, $entity) {
  // No example.
}

/**
 * Allows modules to alter the dimensions that have been determined for a single
 * product referenced by a product line item.
 *
 * @param &$dimensions
 *   The dimensions field value array representing the length, width, and height
 *   of a single product referenced by a product line item. These dimensions may
 *   subsequently be used in volume calculations for an order.
 * @param $line_item
 *   The product line item object whose product dimensions are being determined.
 *   The product data can be accessed through this line item's commerce_product
 *   field.
 *
 * @see commerce_physical_product_line_item_dimensions()
 */
function hook_commerce_physical_product_line_item_dimensions_alter(&$dimensions, $line_item) {
  // No example.
}

/**
 * Allows modules to alter the volume that has been determined for an order.
 *
 * @param &$volume
 *   A volume value array with keys representing the total 'volume' of the order
 *   in the 'unit' specified or NULL if no volume has yet been determined.
 * @param $order
 *   The order object whose volume is being calculated.
 * @param $unit
 *   The unit of measurement whose corresponding cubic unit will be used for the
 *   returned volume of the order.
 *
 * @see commerce_physical_order_volume()
 */
function hook_commerce_physical_order_volume_alter(&$volume, $order, $unit) {
  // No example.
}

/**
 * Allows modules to alter the shippability that has been determined for a line
 * item.
 *
 * @param &$shippable
 *   Boolean indicating whether or not the given line item has been determined
 *   to be shippable.
 * @param $line_item
 *   The line item object whose shippability is being determined.
 *
 * @see commerce_physical_line_item_shippable()
 */
function hook_commerce_physical_line_item_shippable_alter(&$shippable, $line_item) {
  // No example.
}

/**
 * Allows modules to alter the shippability that has been determined for an
 * order.
 *
 * @param &$shippable
 *   Boolean indicating whether or not the given order has been determined to be
 *   shippable.
 * @param $order
 *   The order object whose shippability is being determined.
 *
 * @see commerce_physical_order_shippable()
 */
function hook_commerce_physical_order_shippable_alter(&$shippable, $order) {
  // No example.
}

/**
 * Allows modules to specify a different shipping customer profile reference
 * field name for the given order than the one determined by Commerce Physical
 * Product.
 *
 * @param &$field_name
 *   The name of the field instance on the order that is currently specified as
 *   the customer profile reference field to use for shipping information.
 * @param $order
 *   The order whose shipping customer profile reference field name is being
 *   determined.
 *
 * @see commerce_physical_order_shipping_field_name()
 */
function hook_commerce_physical_order_shipping_field_name_alter(&$field_name, $order) {
  // No example.
}

/**
 * Allows modules to specify a different phone number field name for the given
 * customer profile than the one determined by Commerce Physical Product.
 *
 * @param &$field_name
 *   The name of the field instance on the customer profile that is currently
 *   specified as the field to use for the customer's phone number.
 * @param $profile
 *   The customer profile whose phone number field is being determined.
 *
 * @see commerce_physical_customer_profile_phone_number_field_name()
 */
function hook_commerce_physical_customer_profile_phone_number_field_name_alter(&$field_name, $profile) {
  // No example.
}

/**
 * Allows modules to specify a different residential status for the given
 * customer profile than the one determined by Commerce Physical Product.
 *
 * @param &$residential
 *   Boolean indicating whether or not the given customer profile has been
 *   determined to contain a residential address.
 * @param $profile
 *   The customer profile whose residential status is being determined.
 *
 * @see commerce_physical_customer_profile_residential()
 */
function hook_commerce_physical_customer_profile_residential_alter(&$residential, $profile) {
  // No example.
}
