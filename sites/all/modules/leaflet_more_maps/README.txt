
INSTALLATION
============
Before you enable the Leaflet More Maps module, you need to download and enable
the Leaflet module and the Libraries module.

Then download the Leaflet javascript library from
http://leafletjs.com/download.html.

Drop the unzipped folder in sites/all/libraries and rename it to leaflet, so
that the path to the essential javascript file becomes:
sites/all/libraries/leaflet/leaflet.js

If all's ok, you won't see any errors in the Status Report admin/reports/status.
After this all you have to do is enable Leaflet More Maps to enhance your
mapping experience with lots of attractive map options.

You select your favorite map when you format a single field (eg Geofield) as a
map or when you format a View (of multiple nodes or users) as a map. The module
"IP Geolocation Views and Maps" module is particularly good for this.

You can assemble your own map from the available layers at the Leaflet More Maps
configuration page: admin/config/system/leaflet_more_maps. A layer switcher will
automatically appear in the upper right-hand corner.

The included submodule Leaflet Demo introduces a block that you can enable on a
page to showcase all maps available, centered on your current location, or any
other location for which you specify lat/long coordinates.
Not all maps are available at all coordinates and zoom levels.
All maps show at lat=31, long=-89, zoom=4


FOR PROGRAMMERS
===============
You can add your own map by implementing hook_leaflet_map_info(). See
leaflet_leaflet_map_info() in leaflet.module for an example.
You can alter the default settings of any Leaflet map on the system by
implementing hook_leaflet_map_info_alter().
Example:

  function MYMODULE_leaflet_map_info_alter(&$map_info) {
    foreach ($map_info as $map_id => $info) {
      $map_info[$map_id]['settings']['zoom'] = 2;
      $map_info[$map_id]['label'] += ' ' . t('default zoom=2');
    }
  }


References and licensing terms:

o http://leaflet.cloudmade.com
o http://www.openstreetmap.org/copyright
o http://mapbox.com/tos
o http://maps.stamen.com/#watercolor/12/37.7706/-122.3782
o http://thunderforest.com
o http://www.esri.com
o http://mapquest.com
o http://www.google.com/intl/en_au/help/terms_maps.html
o http://legal.yandex.ru/maps_termsofuse
o http://www.microsoft.com/maps/product/terms.html
