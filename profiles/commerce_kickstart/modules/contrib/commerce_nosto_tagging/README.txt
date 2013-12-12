Nosto Tagging for Drupal Commerce
---------------------------------

The module integrates the Nosto marketing automation service, that can produce
personalized product recommendations on the site.

The module adds the needed data for the service through Drupal blocks. There are
two kinds of blocks added; tagging blocks and element blocks.

Tagging blocks are used to hold meta-data about products, categories, orders,
shopping cart and customers on your site. These types of blocks do not hold any
visual elements, only meta-data. The meta-data is sent to the Nosto marketing
automation service when customers are browsing the site. The service then
produces product recommendations based on the information that is sent and
displays the recommendations in the element blocks.

Element blocks are placeholders for the product recommendations coming from the
Nosto marketing automation service. The blocks contain only an empty div element
that is populated with content from the Nosto marketing automation service.

By default the module creates the following nosto elements:

* 3 elements for the product page
    * "Other Customers Were Interested In"
    * "You Might Also Like"
    * "Most Popular Products In This Category"

* 3 elements for the shopping cart page
    * "Customers Who Bought These Also Bought"
    * "Products You Recently Viewed"
    * "Most Popular Right Now"

* 2 elements for the product category page, top and bottom
    * "Most Popular Products In This Category"
    * "Your Recent History"

* 2 elements for the search results page, top and bottom
    * "Customers who searched '{search term}' viewed"
    * "Your Recent History"

* 2 elements for the sidebars, 1 left and 1 right
    * "Popular Products"
    * "Products You Recently Viewed"

* 2 elements for all pages, top and bottom
    * "Products containing '{keywords}'"
    * "Products You Recently Viewed"

Note that you need not use all the default blocks and that you can change what
recommendations are shown in which block.

You can also add your own element blocks by creating new Drupal blocks, or
modifying template files, and adding the div-element:
"<div class="nosto_element" id="{id of your choice}"></div>"

The module also creates a new page called "Top Sellers" as well as a menu item
for the page. This menu item is not enabled by default and needs to be enabled
and placed in the appropriate menu by the site administrator.
