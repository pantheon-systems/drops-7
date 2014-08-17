<?php

/**
 * @file
 * ip_geoloc_map_current_visitor.tpl.php
 * 
 * This template is used mainly inside a block rather than a view.
 * If $latitude or $longitude are empty, an HTML5 lookup will be initiated.
 */
?>

<div class="ip-geoloc-map-current-visitor">
  <?php print ip_geoloc_output_map_current_location($div_id, $map_options, $map_style, $latitude, $longitude, $info_text); ?>
</div>
