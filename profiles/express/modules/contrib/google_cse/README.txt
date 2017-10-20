
CONTENTS OF THIS FILE
---------------------

 * Overview
 * Quick setup
 * Requirements
 * Blocks
 * SiteSearch
 * Search module integration
 * Advanced settings
 * Installation
 * Maintainers

OVERVIEW
--------

Google Custom Search Engine (CSE) is an embedded search engine that can 
be used to search any set of one or more sites.  No Google API key is 
required.  Read more at http://www.google.com/cse/.

QUICK SETUP
-----------

After installing this module, activate Google CSE at 
admin/config/search/settings, optionally setting it as the default 
search module, and configure it by entering Google's unique ID for your 
CSE.  Once you have granted permission for one or more roles to search 
the Google CSE, the search page can be found at search/google, and a 
separate self-contained search block can also be enabled.

BLOCKS
------

The include Google CSE block can optionally be enabled at 
admin/structure/block.  The "Google CSE" block provides a search box and 
also displays the search results.  After entering search terms, the user 
will be returned to the same page (via GET request) and the results will 
be displayed.  Do not allow this Google CSE block to appear on the 
search/google page, as the search results will fail to display.

SITESEARCH
----------

In addition to the CSE functionality, SiteSearch on one or more domains 
or URL paths can optionally be configured.  Radio buttons allow users to 
search on either the SiteSearch option(s) or the CSE, and searches can 
default to either option.

ADVANCED SETTINGS
-----------------

The collapsed advanced settings on the settings page provide various 
customizations such as country and language preferences.  For example, 
with the Locale module enabled, the Google CSE user interface language 
can be selected dynamically based on the current user's language.

INSTALLATION
------------

Place the google_cse directory in your sites/all/modules directory.  
Enable the Google CSE module at admin/modules, configure it at 
admin/config/search/settings, and assign permissions for "search Google 
CSE" at admin/people/permissions.

To configure this module, you will need your CSE's unique ID.  Go to 
http://www.google.com/cse/manage/all, click on control panel and you 
will find the "Search engine unique ID" under "Basic information".

MAINTAINERS
-----------

Authored and maintained by mfb: http://drupal.org/user/12302

The current maintainer does not plan to add new features to this module, 
such as support for multiple CSEs; however, patches providing new 
features are welcome and will be reviewed.

For bugs, feature requests and support requests, please use the issue 
queue at http://drupal.org/project/issues/google_cse
