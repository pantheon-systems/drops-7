<?php

/**
 * @file
 * Contains the Page callbacks for the Kiala module.
 */

/**
 * Page callback for Kiala Point details map
 */
function commerce_kiala_point_details_map_page($point_id) {
  return theme('commerce_kiala_point_details_map', array(
    'point_id' => $point_id,
  ));
}
