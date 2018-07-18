
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Recommended modules
 * Installation
 * Configuration
 * Troubleshooting
 * Maintainers


INTRODUCTION
------------

This Google Tag Manager project allows non-technical stakeholders to manage the
analytics for their website by triggering the insertion of tags and tracking
systems onto their page(s) via Google's Tag Manager (GTM) hosted application.

 * For a full description, visit the project page:
   https://www.drupal.org/project/google_tag

 * To submit bug reports and feature suggestions, or to track changes:
   https://www.drupal.org/project/issues/google_tag


REQUIREMENTS
------------

Sign up for GTM and obtain a 'container ID' for your website. Enter the
'container ID' on the settings form for this module (see Configuration).

 * https://www.google.com/analytics/tag-manager/


INSTALLATION
------------

Place the project files in an appropriate modules directory and enable the
module as you would any other contributed module. For further information see:

 * https://www.drupal.org/node/895232


CONFIGURATION
-------------

Users in roles with the 'Administer Google Tag Manager' permission will be able
to manage the settings for this module. Configure permissions as usual at:

 * Administration » People » Permissions
 * admin/people/permissions

From the module settings page, configure the conditions on which the tags are
inserted on a page response. Conditions exist for: page paths, user roles, and
response statuses. See:

 * Administration » Configuration » System » Google Tag Manager
 * admin/config/system/google_tag

The module implements the Variable API, so that settings may be separately
configured by realm, thus enabling support for multiple languages and domains.
If these features are needed, then review the other projects at:

 * https://www.drupal.org/project/variable
 * https://www.drupal.org/project/18n
 * https://www.drupal.org/project/domain_variable

For development purposes, create a GTM environment for your website and enter
the 'environment ID' on the 'Advanced' tab of the settings form.

 * https://tagmanager.google.com/#/admin

For additional data layer management, consider the dataLayer module. It supports
the default name for the data layer. To use a non-default name, apply a patch to
the code from that module module.

 * https://www.drupal.org/project/dataLayer


TROUBLESHOOTING
---------------

If the JavaScript snippets are not present in the HTML output, try the following
steps to debug the situation:

 * Confirm the snippet files exist at public://google_tag/ (on most sites this
   equates to sites/default/files/google_tag/).

   If missing, then visit the module settings page and submit the form to
   recreate the snippet files. The need to do this may arise if the project is
   deployed from one environment to another (e.g. development to production) but
   the snippet files are not deployed.

   Due to a known bug during an update to releases 1.2-rc3 or 1.2, the snippet
   directory is not created. A simple workaround for this bug is to disable and
   enable the module (uninstall is not necessary).

 * Enable debug output on the 'Advanced' tab of the settings page to display the
   result of each snippet insertion condition in the message area. Modify the
   insertion conditions as needed.


MAINTAINERS
-----------

Current maintainer:

 * Jim Berry (https://www.drupal.org/u/solotandem)
