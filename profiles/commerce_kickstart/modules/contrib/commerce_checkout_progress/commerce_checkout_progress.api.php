<?php

/**
 * @file
 * Hooks provided by the Commerce Checkout Progress module.
 */

/**
 * Allows modules to alter the checkout progress steps.
 *
 * @param array $items
 *   The array of checkout progress items.
 */
function hook_commerce_checkout_progress_items_alter(&$items) {
  $items['cart']['weight'] = 15;
}
