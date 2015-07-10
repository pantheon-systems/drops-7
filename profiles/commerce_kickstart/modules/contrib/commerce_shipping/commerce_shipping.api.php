<?php


/**
 * Defines a shipping method.
 *
 * @return
 *   An associative array of shipping method info arrays keyed by machine-name.
 *   Each info array must contain values for the following keys:
 *   - title: the human readable title of the shipping method
 *   Values for the following keys are optional:
 *   - display_title: the title used for the method on the front end; defaults to
 *     the title
 *   - description: a basic description of the service
 *   - active: boolean indicating whether or not the default Rule for collecting
 *     rates for this shipping method should be defined enabled; defaults to TRUE
 *   When loaded, a shipping service info array will also contain a module key
 *   whose value is the name of the module that defined the service.
 */
function hook_commerce_shipping_method_info() {
  // No example.
}

/**
 * Allows modules to alter the shipping methods defined by other modules.
 *
 * @param $shipping_methods
 *   The array of shipping methods defined by other modules.
 *
 * @see hook_commerce_shipping_method_info()
 */
function hook_commerce_shipping_method_info_alter(&$shipping_methods) {
  // No example.
}

/**
 * Defines a shipping service.
 *
 * @return
 * An associative array of shipping service info arrays keyed by machine-name.
 * Each info array must contain values for the following keys:
 * - title: the human readable title of the shipping service
 * - shipping_method: the machine-name of the shipping method the service is for
 * - callbacks: an array of callback function names with the following keys:
 *   - rate: the function used to generate the price array for the base rate of
 *     the shipping service; rate callbacks are passed two parameters,
 *     ($shipping_service, $order)
 * Values for the following keys are optional:
 * - display_title: the title used for the service on the front end; defaults to
 *   the title
 * - description: a basic description of the service
 * - rules_component: boolean indicating whether or not a default Rules
 *   component should be created to be used for enabling this service on a given
 *   order; defaults to TRUE
 * - price_component: the name of the price component used in the unit price of
 *   shipping line items for the service; defaults to the name of the shipping
 *   service, which is important for data selection in Rules that may need to
 *   use a variable to specify a price component... accordingly, beware of name
 *   conflicts in your service names with other price components
 * - callbacks: additional callback function names may be specified for the
 *   service with the following keys:
 *   - details_form: the function used to generate a details form array for the
 *     shipping service to collect additional information related to the service
 *     on the checkout / admin forms; details form callbacks are passed five
 *     parameters, ($pane_form, $pane_values, $checkout_pane, $order, $shipping_service)
 *   - details_form_validate: the function used to validate input on the service
 *     details form; details form validate callbacks are passed five parameters,
 *     ($details_form, $details_values, $shipping_service, $order, $form_parents)
 *   - details_form_submit: the function used to perform any additional
 *     processing required on a shipping line item in light of the details form;
 *     details form submit callbacks are passed three parameters,
 *     ($details_form, $details_values, $line_item)
 * When loaded, a shipping service info array will also contain a module key
 * whose value is the name of the module that defined the service.
 */
function hook_commerce_shipping_service_info() {
  // No example.
}

/**
 * Allows modules to alter the shipping services defined by other modules.
 *
 * @param $shipping_services
 *   The array of shipping services defined by other modules.
 *
 * @see hook_commerce_shipping_service_info()
 */
function hook_commerce_shipping_service_info_alter(&$shipping_services) {
  // No example.
}

/**
 * Allows modules or Rules to add shipping rates to an order object for use in
 * building the shipping service checkout pane.
 *
 * Commerce Shipping includes a default Rules configuration that creates a rule
 * per shipping method to go and find services defined by that method and use
 * a rules component to determine whether or not the service should be rated
 * for a particular order.
 *
 * Modules implementing this hook directly should add rates to the order object
 * by adding them to the $order->shipping_rates associative array. The key
 * should be the machine-name of the shipping service and the value should be a
 * shipping line item for the service whose unit price is set to the rate
 * customers will be charged if they select that service.
 *
 * @param $order
 *   The order whose shipping rates should be collected.
 *
 * @see commerce_shipping_collect_rates()
 */
function hook_commerce_shipping_collect_rates($order) {
  // No example.
}

/**
 * Allows modules to add shipping rates to an order object for the specified
 * shipping method for use in building the shipping service checkout pane.
 *
 * This function is typically called via the Rules action "Collect rates for a
 * shipping method" attached to a default Rule. The hook functions just like
 * hook_commerce_shipping_collect_rates() but requests shipping rates added to
 * the order only be for the specified shipping method.
 *
 * @param $method
 *   The machine-name of the shipping method shipping rate collection should
 *   be limited to.
 * @param $order
 *   The order whose shipping rates should be collected.
 *
 * @see hook_commerce_shipping_collect_rates()
 * @see commerce_shipping_method_collect_rates()
 */
function hook_commerce_shipping_method_collect_rates($method, $order) {
  // No example.
}

/**
 * Allows modules or Rules to perform additional calculation of a shipping rate
 * on top of the base rate determined by the shipping service.
 *
 * Rate calculation rules work in much the same way as product pricing rules.
 * The event and hook take a shipping line item parameter whose unit price has
 * been initialized to the base rate for the shipping service. The unit price
 * may then be updated by the unit price manipulation Rules actions or directly
 * in code.
 *
 * Manipulations made via custom code should update the unit price and
 * create a price component in the unit price's data array representing the
 * change in pricing. Without this price component, the change to the unit price
 * amount will not be reflected in order total calculation.
 *
 * While it is not part of the rate calculation process, the function
 * commerce_shipping_example_service_details_form_submit() demonstrates what it
 * means both to increment the shipping line item's unit price amount and to add
 * a price component to its data array representing the change.
 *
 * @param $line_item
 *   The shipping line item that represents the shipping rate the customer must
 *   pay if the related shipping service is selected.
 *
 * @see commerce_shipping_service_rate_calculate()
 * @see commerce_shipping_example_service_details_form_submit()
 */
function hook_commerce_shipping_calculate_rate($line_item) {
  // No example.
}

/**
 * Allows modules to alter the options array generated to select a shipping
 * service on the checkout form.
 *
 * The options values for the shipping service radio buttons defaults to show
 * the display title of the shipping service and price. If you need to add
 * additional information for your shipping services or alter the apperance of
 * other modules' shipping services, you can use this hook to do so with the
 * order object available.
 *
 * The options array is keyed by the shipping service name and matches the keys
 * in the $order->shipping_rates array containing shipping line items for rates
 * calculated for the order.
 *
 * @param $options
 *   The options array used to select a calculated shipping rate.
 * @param $order
 *   The order the options array was generated for.
 *
 * @see commerce_shipping_service_rate_options()
 */
function hook_commerce_shipping_service_rate_options_alter(&$options, $order, &$form_state) {
  // No example.
}

/**
 * Allows modules to alter newly created shipping line items after their service
 * and unit price values have been set.
 *
 * @param $line_item
 *   The newly created shipping line item.
 *
 * @see commerce_shipping_line_item_new()
 */
function hook_commerce_shipping_line_item_new_alter($line_item) {
  // No example.
}
