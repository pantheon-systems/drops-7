<?php

/**
 * @file
 * ip_geoloc_map_multi_visitor.tpl.php
 *
 * This template is typically used to output a map of visitor locations in a
 * non-view context, e.g. through a block.
 *
 * Variables available:
 * - $locations: array of locations each with lat/long coordinates and balloon
 *   texts; the map will be centered on the first location, usually the current
 *   visitor location
 * - $div_id: id of the div in which the map will be injected; arbitrary but
 *   must be unique
 * - $map_options: passed to Google Maps API, for example:
 *   '{"mapTypeId":"roadmap", "zoom": 10}'
 * - $map_style: CSS style string, like 'height: 200px; width: 500px'
 */
?>
<div class="ip-geoloc-map visitor-map">
  <?php print ip_geoloc_output_map_multi_visitor($locations, $div_id, $map_options, $map_style); ?>
</div>
