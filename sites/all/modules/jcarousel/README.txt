
Description
-----------
This module provides a central function for adding jCarousel jQuery plugin
elements. For more information about jCarousel, visit the official project:
http://sorgalla.com/jcarousel/


Installation
------------
1) Place this module directory in your modules folder (this will usually be
   "sites/all/modules/").

2) Enable the module within your Drupal site at Administer -> Site Building ->
   Modules (admin/build/modules).

Usage
-----
The jCarousel module is most commonly used with the Views module to turn
listings of images or other content into a carousel.

1) Install the Views module (http://drupal.org/project/views) on your Drupal
   site if you have not already.

2) Add a new view at Administration -> Structure -> Views (admin/structure/views).

3) Change the "Display format" of the view to "jCarousel". Disable the
   "Use pager" option, which cannot be used with the jCarousel style. Click the
   "Continue & Edit" button to configure the rest of the View.

4) Click on the "Settings" link next to the jCarousel Format to configure the
   options for the carousel such as the animation speed and skin.

5) Add the items you would like to include in the rotator under the "Fields"
   section, and build out the rest of the view as you would normally. Note that
   the preview of the carousel within Views probably will not appear correctly
   because the necessary JavaScript and CSS is not loaded in the Views
   interface. Save your view and visit a page URL containing the view to see
   how it appears.

API Usage
---------
The jcarousel_add function allows you to not only add the required jCarousel
JavaScript, and apply the jCarousel behavior to the elements on the page. The
arguments are as follows:

  jcarousel_add($class_name, $settings);

The $id is the CSS class of the element which will become the carousel. Multiple
carousels may have the same carousel ID and all their settings will be shared.

The $settings are the configuration options that are sent during the creation
of the jCarousel element (optional). The configuration options can be found at:
http://sorgalla.com/projects/jcarousel/#Configuration

A few special keys may also be provided in $settings, such as $settings['skin'],
which can be used to apply a specific skin to the carousel. jCarousel module
comes with a few skins by default, but other modules can provide their own skins
by implementing hook_jcarousel_skin_info().

An alternative to using jcarousel_add() is passing a list of items that will be
in your carousel into theme('jcarousel'). This can be useful to not only add
the necessary JavaScript and CSS to the page but also to print out the HTML
list.

  print theme('jcarousel', array('items' => $items, 'options' => $options, 'identifier' => $identifier));

See admin/help/jcarousel for demonstrations of how to utilize jCarousel in your
own code.

Example
-------
The following would add a vertical jCarousel to the page:

  <ul class="mycarousel jcarousel-skin-default">
    <li><img src="http://static.flickr.com/66/199481236_dc98b5abb3_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/75/199481072_b4a0d09597_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/57/199481087_33ae73a8de_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/77/199481108_4359e6b971_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/58/199481143_3c148d9dd3_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/72/199481203_ad4cdcf109_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/58/199481218_264ce20da0_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/69/199481255_fdfe885f87_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/60/199480111_87d4cb3e38_s.jpg" width="75" height="75" alt="" /></li>
    <li><img src="http://static.flickr.com/70/229228324_08223b70fa_s.jpg" width="75" height="75" alt="" /></li>
  </ul>
  <?php
    jcarousel_add('mycarousel', array('vertical' => TRUE));
  ?>
See jcarousel_help() for more examples.

Authors
-------
Nate Haug (http://quicksketch.org)
Matt Farina (http://www.mattfarina.com)
Wim Leers (work@wimleers.com | http://wimleers.com/work)
Rob Loach (http://www.robloach.net)
