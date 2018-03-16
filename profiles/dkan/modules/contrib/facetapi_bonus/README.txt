

INSTALLATION


Simply save this module's subdir in your contrib module directory,
and enable the module.


USAGE


Within the Facet API UI of a specific facet, go to "Dependency" or "Filters" to
add one of this module's plugins. The usage of the specific plugins are well
explained within their settings forms.


MODULE DESCRIPTION


Facet API Bonus for Drupal 7 is a collection of additional Facet API plugins and
functionality, foremost filter and dependency plugins – And a place to collect
more additional Facet API extensions.

Currently Facet API Bonus includes:

* Facet Dependency

  Dependency plugin to make one facet (say "product category")
  to show up depending on other facets or specific facet items being active
  (say "content type" is "product" or "service"). Very flexible,
   supports multiple facets to be dependencies, as well as regexp for specifying
   facet item dependencies, as well as option how to behave if a dependency is
   being lost.

* Filter "Exclude Items"

  Filter plugin to exclude certain facet items by their
  markup/title or internal value (say excluding "page" from "content types").
  Regexp are also possible.

* Filter "Rewrite Items"

  Filter plugin to rewrite labels or other data of the facet items by
  implementing a new dedicated hook_facet_items_alter (in a structured array,
  before rendering). Very handy to rewrite list field values or totally custom
  encoded facet values for user friendly output.

  By enabling this filter, items of this facet can be
  rewritten prior to rendering by implementing the hook:

    function HOOK_facet_items_alter(&$build, &$settings) {
      if ($settings->facet == "YOUR_FACET_NAME") {
        foreach($build as $key => $item) {
          $build[$key]["#markup"] = drupal_strtoupper($item["#markup"]);
        }
      }
    }

  (This example simply rewrites all facet items output to be uppercase.)

  Replace "HOOK" with the name of a custom module containing
  your hook implementation, and "YOUR_FACET_NAME" with the
  machine name of the specific facet whose items you want to
  rewrite.

  $build is an array of facet items you
  can rewrite, $settings contains the facet filter settings
  as context to determine the facet and search context.

* Filter "Do not display items that do not narrow results"

  This filter checks the number of items that will be displayed after activating
  facet link and removes the link if the number is the same as currently
  displayed. If link has children in hierarchical structure, it won't be removed.

* Filter "Do not show facet with only X items"

  This filter checks total number of links and if number is less than X, we
  remove all items and hide block completely. Block will not be hidden if there
  any active items in it.

* Filter "Show only deepest level items"

  Removes all items that have children.


* Integration with Page Title module

  Now you can set search (views) page titles using Page Title module. Module
  provides possibility to set tokens 'facetapi_results' and 'facetapi_active'
  groups. So in title we can display number of results on the page or values of
  active facets. As there can be multiple active facet values please use following
  pattern to use facetapi_active tokens:

  list<[facetapi_active:facet-label]: [facetapi_active:active-value]>

  This will make coma separated list of active facet labels and their values.


* Current search block Reset Filters link

  Gives possibility to add link to current block that resets all applied facets.
  Text is customizable.

===> Further additions are very welcome! <===

Facet API Bonus is written for Drupal 7, and is stable, tested,
and ready to be used in production environments.

Requirements:

* Facet API is obviously required, as well as
  a compatible search module (e.g. apachesolr, search_api).

Similar projects:

* http://drupal.org/project/facetapi_extra Module has been deprecated in favour
  of facetapi_bonus module. All features have been merged.


MODULE URL


More information and issues, see the module page, currently at:

  http://drupal.org/project/facetapi_bonus
