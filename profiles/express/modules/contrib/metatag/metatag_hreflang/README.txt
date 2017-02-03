Metatag: hreflang
-----------------
This module automatically adds hreflang meta tags for each locale currently
enabled on the site. It also provides support for the hreflang=x-default meta
tag.

This is similar to the Alternative hreflang [1] module. That module
automatically adds the hreflang tag to every page for every enabled locale,
which may not be what a site needs. This module allows the site builder control
over what tags are shown.

The module also provides new tokens to output the URLs of each of the current
node's translations, and assigns these as the defaults for the meta tags. As
such, this module may not need additional configuration once it is enabled, but
it's always worth confirming the output is as expected.

This module works best when the Translation or Entity Translation modules are
enabled and configured.


Configuration
--------------------------------------------------------------------------------
 1. By default if the hreflang="x-default" meta tag matches one of the
    hreflang="LANGCODE" meta tags that hreflang="LANGCODE" meta tag will be
    removed. It is possible to change this so that the meta tag is not removed
    by enabling the "Allow hreflang tag that matches the x-default tag" option
    on the main Metatag settings page:
      admin/config/search/metatags/settings


Credits / Contact
--------------------------------------------------------------------------------
Originally developed by Damien McKenna [2].


References
--------------------------------------------------------------------------------
1: https://www.drupal.org/project/hreflang
2: https://www.drupal.org/u/damienmckenna.
