
Module: Google Analytics
Author: Alexander Hass <http://drupal.org/user/85918>


Description
===========
Adds the Google Analytics tracking system to your website.

Requirements
============

* Google Analytics user account

Installation
============
Copy the 'googleanalytics' module directory in to your Drupal
sites/all/modules directory as usual.

Upgrading from 6.x-3.x and 7.x-1.x
==================================
If you upgrade from 6.x-3.x and 7.x-1.x (ga.js) to 7.x-2.x (analytics.js) you
should verify if you used custom variables. Write down your settings or make a 
screenshot. You need to re-configure the settings to use custom dimensions or
metrics. There is no automatic upgrade path for custom variables feature. All
other module settings are upgraded automatically.

See https://support.google.com/analytics/answer/2795983?hl=en for more details.

Usage
=====
In the settings page enter your Google Analytics account number.

All pages will now have the required JavaScript added to the
HTML footer can confirm this by viewing the page source from
your browser.

Page specific tracking
======================
The default is set to "Add to every page except the listed pages". By
default the following pages are listed for exclusion:

admin
admin/*
batch
node/add*
node/*/*
user/*/*

These defaults are changeable by the website administrator or any other
user with 'Administer Google Analytics' permission.

Like the blocks visibility settings in Drupal core, there is a choice for
"Add if the following PHP code returns TRUE." Sample PHP snippets that can be
used in this textarea can be found on the handbook page "Overview-approach to
block visibility" at http://drupal.org/node/64135.

Custom dimensions and metrics
=============================
One example for custom dimensions tracking is the "User roles" tracking.

1. In the Google Analytics Management Interface (http://www.google.com/analytics/)
   you need to setup Dimension #1 with name e.g. "User roles". This step is
   required. Do not miss it, please.

2. Enter the below configuration data into the Drupal custom dimensions settings
   form under admin/config/system/googleanalytics. You can also choose another
   index, but keep it always in sync with the index used in step #1.

   Index: 1
   Value: [current-user:role-names]

More details about custom dimensions and metrics can be found in the Google API
documentation at https://developers.google.com/analytics/devguides/collection/analyticsjs/custom-dims-mets

Advanced Settings
=================
You can include additional JavaScript snippets in the custom javascript
code textarea. These can be found on the official Google Analytics pages
and a few examples at http://drupal.org/node/248699. Support is not
provided for any customisations you include.

To speed up page loading you may also cache the Google Analytics "analytics.js"
file locally.

Manual JS debugging
===================
For manual debugging of the JS code you are able to create a test node. This
is the example HTML code for this test node. You need to enable debugging mode
in your Drupal configuration of Google Analytics settings to see verbose
messages in your browsers JS console.

Title: Google Analytics test page

Body:
<ul>
  <li><a href="mailto:foo@example.com">Mailto</a></li>
  <li><a href="/files/test.txt">Download file</a></li>
  <li><a class="colorbox" href="#">Open colorbox</a></li>
  <li><a href="http://example.com/">External link</a></li>
  <li><a href="/go/test">Go link</a></li>
</ul>

Text format: Full HTML
