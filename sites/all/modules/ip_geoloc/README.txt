
IP GEOLOCATION VIEWS AND MAPS
=============================
This documentation concentrates on the installation and configuration of the
IP Geolocation Views & Maps (IPGV&M) module. A full description of the module
and its features can be found at http://drupal.org/project/ip_geoloc.

INSTALLATION & CONFIGURATION
============================
First, if you are using IPGV&M primarily for its Views mapping interface to
other modules and the features it adds, then here's a configuration shortcut.
Download and enable IPGV&M like any other module. Visit its
configuration page, .../admin/config/system/ip_geoloc.
If you intend to use IPGV&M's built-in interface to Google Maps, untick all
"Data collection option" boxes.
If you intend to use IPGV&M with the OpenLayers or Leaflet modules and also wish
to show and center on the visitor's HTML-5 retrieved location, then you DO need
to tick the first "Data collection option" and select applicable roles below it.

You are now ready to map your View of Location, Geofield, Geolocation Field or
GetLocations data and optionally center that map on the visitor's location.
Visit the IPGV&M configuration page to specify an alternative marker set. When
using Leaflet you can superimpose Font Awesome font characters on top of your
markers. See below for instructions on how to download the Font Awesome library.

The "Views PHP" module is required for the included /visitor-log View. The core
module Statistics must be enabled also when you wish to collect visitor data.
It is not required for maps in general. The "Session Cache API" and
"High-performance Javascript callback handler" modules are optional, but
recommended.

If you want to center the map on the visitor's location, but don't want to use
the HTML5 style of location retrieval involving a browser prompt, you may want
to configure an alternative lat/long lookup based on IP address. For this follow
installation instruction B1a or B1b below, depending on whether you'd like to
employ Smart IP or GeoIP for this.

If you DO want to auto-record visitor address details then complete the steps
under A and B below.

A. Present and future: reporting and mapping of location information about
guests visiting after you enabled IPGV&M

1. Install and enable like any other module, use Drush if you wish. Remain
connected to the internet.

2. Make sure the core Statistics module is enabled. Then at Configuration >>
Statistics, section System, verify that the access log is enabled. Select the
"Discard access logs older than" option as you please. "Never" is good.

3. Visit the IPGV&M configuration page at Configuration >> IP Geolocation V&M
If you don't see any errors or warnings (usually yellow) you're
good to proceed. Don't worry about any of the configuration options for now,
the defaults are fine.

4. At Structure >> Blocks put the block "Map of 20 most recent visitors" in the
content region of all or a selected page. View the page. That marker represents
you (or your service provider). Clicking the marker reveals more details.

5. Enable the Views and Views PHP (http://drupal.org/project/views_php) modules.
Then have a look at Structure >> Views for a couple of handy Views, e.g. the
"Visitor log", which shows for each IP address that visited, its street address,
as well as a local map. Or "Visitor log (lite)", which combines nicely when put
on the same page with the block "Map of 20 most recent visitors".
Modify these views as you please.


B. Historic data: location info about visits to your site from way back when

Note, that this step relies on you having had the Statistics module enabled
before you installed IPGV&M, as the access log is used as the source of IP
addresses that have visited your site previously.
There are a couple of options here. Use either
http://drupal.org/project/smart_ip and the IPinfoDB web service it uses, or
http://drupal.org/project/geoip, which takes its data from a file you download
for free.

1a. If you decide to employ Smart IP....
Install and enable Smart IP. There is no need to enable the Device Geolocation
submodule as IPGV&M already has that functionality, plus more. At
Configuration >> Smart IP you'll find a number of source to upload historic
lat/long data. Pick any of these. A low-cost option is to download
http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz and
uncompress it in /sites/default/private/smart_ip.
You may untick all the check boxes under the heading "Smart IP settings" on
the Configuration >> Smart IP page.

1b. If you decide to employ GeoIP instead of Smart IP...
Download and enable the module. Then download
http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz and 
uncompress it in sites/all/libraries/geoip. Go to the GeoIP configuration page
and type the name of the file you've just downloaded, GeoLiteCity.dat. Save.

2. With either Smart IP or GeoIP configured, visit Configuration >> IPGV&M.
Tick the check boxes as appropriate.

3. On the same page, start a small batch import of, say, size 10. Data for the
most recent visitors will be loaded first, so you don't have to complete the
import to check it's all working. For instance, the block "Map showing locations
of 10 most recent visitors" should now show more markers.

4. Go back the Configuration >> IPGV&M and complete the import process
with a larger batch size until the IP geolocation database is up to date with
the access log. It will automatically remain in sync from now on.

CONFIGURING YOUR VIEW TO DISPLAY AS A MAP
=========================================
If you have one of the Location, Geofield, Geolocation Field or Get Locations
modules enabled, then first create a normal tabular content View of nodes that
hold location coordinates via one of these modules. The coordinates will show up
in the Field list of your View.
Unless you use the Location module (with User Locations or Node Locations), make
sure you have included in your view's field selection a field named "Content:
your_Geofield/Geolocation_field". Only one copy is required, you do NOT need
both a latitude version plus a longitude version. The "Formatter", if it pops
up, is relevant only if you want the location field to appear in the marker
balloons. Make sure "Use field template" is UNTICKED. Commerce Kickstart tends
to have the box ticked, so UNTICK it. Untick it for the differentiator too, if
used.
Then, after selecting the View Format "Map (Google, via IPGV&M)", "Map (Leaflet,
via IPGV&M)" or "Map (OpenLayers, via IPGV&M)" select or type field_name in the
"Name of latitude field in Views query".
Fill out the remaining options to your liking. Save. Done.

LEAFLET TIPS
============
You need to download and install the Leaflet package on your system, but you
only have to enable the main module, no need for the Leaflet Views submodule.
Don't forget to download the Leaflet javascript library from
http://leafletjs.com/download.html dropping it in sites/all/libraries and
changing the folder name to leaflet. Remember to install and enable the
Libraries API module too.
When all's ok, you won't see any errors in the Status Report, i.e.
.../admin/reports/status.

OPENLAYERS TIPS
===============
Of the modules in the OpenLayers package you only need to enable OpenLayers and
OpenLayers UI. In fact, you could even disable OpenLayers UI when you're done
configuring your maps.
Initially the location markers are likely to show up as black circles. To
change the marker shapes and colours you need to first associate the "Location
markers" layer with your map at Structure >> OpenLayers >> Maps >> List. If
there is no custom map on this page yet, you have to first clone and save under
a different name one of the existing maps. Once you've done that, you'll find
that on Structure >> OpenLayers >> Maps >> List the map appears with an Edit
link. Click that, followed by the "Layers & Styles" vertical tab. In the bottom
section of the page you'll find the "Current visitor marker" layer and a number
of "Location markers" layers. You'll only need to activate Location markers #1,
unless you have defined a "differentiator" (e.g. taxonomy term) on your view.
Pick marker styles from the drop-downs as you please and press Save. Note that
the location markers won't show up in the map preview, but the visitor marker
should. This is because the map doesn't know which view it will be paired with.
In fact there may be several views using the same map. Return to edit your view.
Under Format, Settings select the map you've just created and edited. Save the
format and the view. Visit the page containing your view and all markers should
appear in the colours you chose.
In order to make the text balloons pop up at a marker when hovered or clicked,
visit Structure >> OpenLayers >> Maps >> Edit and click the "Behaviors"
vertical tab. Scroll down the page until you reach the "Pop Up for Features"
check box. Tick it and select the layers you're interested with, e.g.
ip_geoloc_visitor_marker_layer1 and/or ip_geoloc_marker_layer. Same for
"Tooltip for Features". Other nice behaviors that you may wish to flick on on
the same page are "Full Screen", "Layer Switcher", "Pan and Zoom Bar Controls"
and "Scale Line".

ALTERNATIVE MARKER ICONS (LEAFLET, GOOGLE MAPS)
===============================================
Find on the web a marker icon set you like, eg http://mapicons.nicolasmollet.com
Download and extract the icon image files, which must have extension .png,
into a directory anywhere in your Drupal instal,
e.g. sites/default/files/map_markers.
Now visit the the IP Geolocation Views & Maps configuration page at
admin/config/system/ip_geoloc. Expand the "Alternative markers" fieldset.
Enter the path to your map_markers directory and the dimensions of your markers.
The marker set will now be available in your map settings, in particular in the
differentiator settings.

"FONT AWESOME" ICONS SUPERIMPOSED ON YOUR MARKER IMAGES (LEAFLET)
=================================================================
To install Font Awesome visit http://fortawesome.github.io/Font-Awesome and
press the "Download" button. Unzip the downloaded file into the Drupal
libraries directory, typically sites/all/libraries. Remove the version number
from the directory name, so that the path to the essential style sheet becomes
sites/all/libraries/font-awesome/css/font-awesome.min.css. Double-check via the
Status Report page, .../admin/reports/status.

UTILITY FUNCTIONS
=================
Check out file ip_geoloc_api.inc for a number of useful utility functions for
creating maps and markers, calculating distances between locations etc. All
functions are documented and should be straightforward to use.

HOOKS
=====
See ip_geoloc.api.php

HIGH PERFORMANCE AJAX
=====================
IPGV&M will take advantage of the "High-performance Javascript callback
handler", if installed.
Installation instructions for Nginx: http://drupal.org/node/1876418
Installation instructions for Apache: 
o download and enable https://drupal.org/project/js
o find the .htacess in your document root, where your index.php lives
o visit admin/config/system/js which displays a number of lines tailored for
  your server
o copy those lines and paste them into .htaccess below the line
  "RewriteEngine on".

IPGV&M will now perform its AJAX calls more efficiently. To switch this feature
off, comment out the newly added lines from the .htaccess file (put # in front).

RESTRICTIONS IMPOSED BY GOOGLE
==============================
Taken from http://code.google.com/apis/maps/documentation/geocoding :

"Use of the Google Geocoding API is subject to a query limit of 2500 geolocation
requests per day. (Users of Google Maps API Premier may perform up to 100,000
requests per day.) This limit is enforced to prevent abuse and/or repurposing of
the Geocoding API, and this limit may be changed in the future without notice.
Additionally, we enforce a request rate limit to prevent abuse of the service.
If you exceed the 24-hour limit or otherwise abuse the service, the Geocoding
API may stop working for you temporarily. If you continue to exceed this limit,
your access to the Geocoding API may be blocked.
Note: the Geocoding API may only be used in conjunction with a Google map;
geocoding results without displaying them on a map is prohibited. For complete
details on allowed usage, consult the Maps API Terms of Service License
Restrictions."

AUTHOR
======
Rik de Boer of flink dot com dot au, Melbourne, Australia.
