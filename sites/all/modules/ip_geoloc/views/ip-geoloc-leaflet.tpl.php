<?php
/**
 * @file
 * ip-geoloc-leaflet.tpl.php
 *
 * This template is used to output a placeholder for a map with location
 * markers taken from a View.
 *
 * Variables available:
 * - $map_id
 * - $height
 * - $view (to add title?)
 */

  $marker_set = variable_get('ip_geoloc_marker_directory', '/markers');
  $marker_set = drupal_substr($marker_set, strrpos($marker_set, '/') + 1);
?>
<div class="ip-geoloc-map leaflet-view <?php echo $marker_set; ?>">
  <div id="<?php echo $map_id; ?>" style="height:<?php echo $height; ?>px"></div>
</div>
