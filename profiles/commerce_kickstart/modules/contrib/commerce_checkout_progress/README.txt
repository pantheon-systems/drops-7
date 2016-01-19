Commerce Progress Checkout
==========================

commerce_progress_checkout is a module that provides visual clues
about the progress of the checkout procedure for Drupal Commerce.

Upgrade to 7.x-1.4: Links are now disabled by default
-----------------------------------------------------

If you're upgrading to 7.x-1.4, you'll notice that the links on the
checkout pages are disabled. This is done to guarantee data integrity.

The fact that the user can move around the checkout process phases
using the links causes problems coming from the fact that these are
GET requests and thus interact with the browser cache and possibly
invite issues like duplicated payments and incorrect address data into
the checkout.

You can turn them back on at any time using in the block configuration.
