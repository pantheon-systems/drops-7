<?php

/**
 * Implements hook_inline_conditions_info().
 */
function inline_conditions_inline_conditions_info() {
  $conditions = array();

  // Example?

  return $conditions;
}

/**
 * Implements hook_inline_conditions_build_alter().
 */
function inline_conditions_inline_conditions_build_alter(&$value) {
  switch ($value['condition_name']) {
    // Example?
  }
}

/**
 * Need to create configure examples, for now review
 * commerce_discount.inline_conditions.inc
 */
