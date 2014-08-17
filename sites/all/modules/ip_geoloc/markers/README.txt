
Marker images for "IP Gelocation Views and Maps" module
=======================================================

All you need to do to add a new marker image to your set of markers is drop a
new .png image file in the /markers folder.

The markers in this directory are all 21 pixels wide. If you wish to use
markers of different dimensions you can do so by specifying an alternative
marker directory and marker dimensions on the module configuration page.
One alternative markers directory comes included with the module: /amarkers. Its
markers are wider (32 pixels), so are more suitable for imposing font icons.
See the ip_geoloc/README.txt for details.

The anchor point of all marker images is expected to be in the centre of the 
bottom border of the image.

Note: images in this folder are NOT used when you elect OpenLayers.
OpenLayers has its own set of markers. On the admin/structure/openlayers/maps
page click edit or clone to open one of your maps. Then click the vertical tab
"Layers & Styles" to view available marker styles in the "Overlay layers"
section of the page.

ACKNOWLEDGEMENT:
Most of the markers in this directory were kindly copied from the GMap module.
