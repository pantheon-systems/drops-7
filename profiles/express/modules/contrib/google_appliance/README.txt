$Id$

-- SUMMARY --

The Google Appliance module provides an interface to obtain search results from a dedicated Google Search Appliance (GSA) hardware device. The module can act as a replacemet for core Search, or it may operate in tandem with it. 

If you will use core search along with this module, core search results will show up normally at search/{search terms}, while Google Appliance searches will show up at gsearch/{search terms}. 

Since search index management for the GSA searches is offloaded to the GSA device, utilizing the module is a two-tier solution: 

  (1) search management with the GSA and its assocated (Drupal-external) tasks, and 
  (2) defining the commiunication interface for Drupal. 

This module simply defines the communication interface for Drupal.


-- REQUIREMENTS --

Before using this module, you (obviously) must have a dedicated Goodle Search Appliance (GSA) device, and you must also set up a collection and frontend on the GSA that can be accessed publicly. To produce results that include Drupal content, the GSA crawl must have visited your drupal installation and indexed the desired content. This will allow off-platform content to be integrated into your drupal search solution.


-- INSTALLATION --

Install in Drupal the normal way...

  * http://drupal.org/documentation/install/modules-themes/modules-7

-- CONFIGURATION --

The module needs to be configured to connect to your GSA device, which can be found here...

  * admin/config/search/google_appliance/settings
  
Or in the "Search & Metadata" fieldset on the configuration page, click on "Search Google Appliance". 

-- BLOCKS --

The module provides two blocks:

  (1) Search Form
  (2) Related Searches

Both blocks will need to be assigned to a region, but the Related Searches block is preconfigured to only appear on results pages. Neither block is required to be used, as you can access the search form on the search page

  * gsearch/

To setup your blocks, administer them in the normal way at
  
  * admin/structure/block

-- ONEBOX INTEGRATION --

This module provides a basic framework for adding onebox modules to the search interface. For the purposes of this documentation, we assume you already have oneboxes configured and running on your search appliance.

Onebox modules are represented as blocks--one block per onebox. To add onebox blocks to Drupal, add each name (exactly as it appears in the search appliance) to the "onebox modules" text area on the module configuration page, each onebox on its own line.

After saving your configuration changes, blocks will be created for each onebox. You'll want to place these blocks on the search result page. You can place them as described above (in the blocks section), or via your preferred Drupal layout module (like Context or Panels).

-- TESTING --

Automated tests have been written that fall into two categories:

  (1) Basic testing that doesn't require a connection to your GSA
  (2) Connectivity and results listings tests

To execute the second class of tests, you will need to provide an input file for the SimpleTest browser to configure your module. See the example file:

  * google_appliance/testing/test-settings.example.inc
