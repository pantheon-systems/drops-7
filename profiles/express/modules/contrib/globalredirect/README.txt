CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * Configuration

INTRODUCTION
------------
 * Checks the current URL for an alias and does a 301 redirect to it if it is
   not being used.
 * Checks the current URL for a trailing slash, removes it if present and
   repeats check 1 with the new request.
 * Checks if the current URL is the same as the site_frontpage and redirects to
   the frontpage if there is a match.
 * Checks if the Clean URLs feature is enabled and then checks the current URL
   is being accessed using the clean method rather than the 'unclean' method.
 * Checks access to the URL. If the user does not have access to the path, then
   no redirects are done. This helps avoid exposing private aliased node's.
 * Make sure the case of the URL being accessed is the same as the one set by
   the author/administrator. For example, if you set the alias
   "articles/cake-making" to node/123, then the user can access the alias with
   any combination of case.
 * Most of the above options are configurable in the settings page.

INSTALLATION
------------
 * Install as you would normally install a contributed Drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.

CONFIGURATION
-------------
 * Customize the module settings in Administration » Configuration »
   System » Global Redirect (/admin/config/system/globalredirect)
 * The module can check whether the user has access to a path before
   redirecting. However, this has a performance impact, so the option is
   disabled by default. If this is a concern for your site, enable
   "Menu Access Checking".
