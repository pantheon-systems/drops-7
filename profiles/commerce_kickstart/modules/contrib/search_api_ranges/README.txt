
Search ranges
-------------

This module allows you to enable a block with custom ranges options.
Requires Facet API and Search API facetapi integration.

- go to your Search API index > Facets tab
- for any field of type: integer, decimal, float, Unix timestamp
- go to Display Options and choose Display Widget: Min/Max UI Slider
- (optionally) configure name, prefix, suffix
- (optionally) override theme for search-api-ranges-slider.tpl.php
- enable your new Facet block

Examples of advanced ranges
---------------------------

Most simply, you can provide a manual set of ranges such as this:

0-100
100-500
500-2000

If you omit a value, it will be set to the lowest/highest in the set. For
example, if our lowest facet value was 33, and highest was 66, and you provided
the following range settings:

-50
50-

You would get the follow ranges queried for:

33-50
50-66

If instead of omitting a value you use the * symbol, your facet query will
include that, which means 'no-limit'.

Oftentimes, you might wish to provide custom display text for a given range.
This can be achieved by putting your desired text after a pipe eg:

0-8|Less than 10 days
8-14|8 to 14 days
15-*|15+ days
