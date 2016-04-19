<?php

/**
 * @file
 *   Provides the callback wrapper functions.
 */

// Build functions for each callback types.
while ($spec = _entity_path_dynamic_callback_definitions()) {
  $func = _entity_path_dynamic_callback($spec[0], $spec[1]);
  // If the function is not allready defined, we dynamically do that.
  if (!function_exists($func)) {
    $entity_type = $spec[0];
    $bundle = $spec[1];
    // evaluate the function definition.
    $function_eval = "function $func(\$entity) {
      return entity_path_uri_callback_wrapper('$entity_type', '$bundle', \$entity);
    }";
    eval($function_eval);
  }
}