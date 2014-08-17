<?php
/**
 * @file
 * ip-geoloc-openlayers.tpl.php
 *
 * This template is used to output a map of marker locations taken from a view.
 *
 * Variables available:
 * - $map: the map object, contains id, name, height and width
 * - $container_width, $container_height: dimensions of enclosing div
 * - $view: the view object, if required (maybe to display title above map?)
 */
?>
<div class="ip-geoloc-map openlayers-view">
  <div id="openlayers-container-<?php echo $map['id']; ?>"
       style="width:<?php echo $container_width; ?>; height:<?php echo $container_height; ?>"
       class="openlayers-container openlayers-container-map-<?php echo $map['id']; ?>">
    <div id="<?php echo $map['id']; ?>"
         style="width:<?php echo $map['width']; ?>; height:<?php echo $map['height']; ?>"
         class="openlayers-map openlayers-map-<?php echo $map['map_name']; ?>">
    </div>
  </div>
</div>
