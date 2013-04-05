<?php

/**
 * @file
 * Hooks provided by the Physical Fields module.
 */


/**
 * Allows modules to alter or define additional weight units of measurement.
 *
 * @param &$units
 *   The array of weight units of measurement, each one defined as an array
 *   containing a name and abbreviation.
 */
function hook_physical_weight_unit_info_alter(&$units) {
  // No example.
}

/**
 * Allows modules to alter or define additional dimensions.
 *
 * Note: while still in testing, this hook may be the key to interstellar travel
 * (and therefore instantaneous shipping to other galaxies). Please be careful
 * manipulating dimensions with implode(), explode(), and array_shift().
 *
 * @param &$dimensions
 *   The array of dimensions, each one defined as an array containing a name.
 */
function hook_physical_dimension_info_alter(&$dimensions) {
  // No example.
}

/**
 * Allows modules to alter or define additional dimension units of measurement.
 *
 * @param &$units
 *   The array of dimension units of measurement, each one defined as an array
 *   containing a name and abbreviation.
 */
function hook_physical_dimensions_unit_info_alter(&$units) {
  // No example.
}
