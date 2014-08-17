
INSTALLATION
============
Naturally you need to have Leaflet (the module and the javascript library)
installed before you can use Leaflet MarkerCluster.

Then download the MarkerCluster library from:
https://github.com/danzel/Leaflet.markercluster
Rename the downloaded directory to leaflet_markercluster (lowercase), so that
the path to the essential javascript file becomes
sites/all/libraries/leaflet_markercluster/dist/leaflet.markercluster.js

Visit the Status Report page, admin/reports/status, to check all's ok.

There are no permissions to configure.
This module does not itself have a UI to set MarkerCluster configuration
parameters. However parameters may be set through Drupal code as part of the
creation of the map and will thus be passed to the underlying javascript
library. See the section below.


FOR PROGRAMMERS
===============
You can set Leaflet MarkerCluster parameters in the same way that you set
Leaflet map parameters.
Example:

  $map_id = 'OSM Mapnik'; // default map that comes with Leaflet
  $map = leaflet_map_get_info($map_id);

  $map['settings']['zoom']                    = 10; // Leaflet parameter
  $map['settings']['maxClusterRadius']        = 50; // Leaflet MarkerCluster parameter
  $map['settings']['disableClusteringAtZoom'] = 2;  // Leaflet MarkerCluster parameter

  $features = ... // see the README.txt of the Leaflet module

  $output = '<div>' . leaflet_render_map($map, $features, '300px') . '</div>';

The following MarkerCluster parameters may be configured this way:

  animateAddingMarkers (default: FALSE)
  disableClusteringAtZoom (NULL)
  maxClusterRadius (80)
  showCoverageOnHover (TRUE)
  singleMarkerMode (FALSE)
  skipDuplicateAddTesting (FALSE)
  spiderfyOnMaxZoom (TRUE)
  zoomToBoundsOnClick (TRUE)
  addRegionToolTips (FALSE)

See the bottom reference for an explanation of these parameters.

References:

o http://leaflet.cloudmade.com/2012/08/20/guest-post-markerclusterer-0-1-released.html
o https://github.com/Leaflet/Leaflet.markercluster/blob/master/README.md