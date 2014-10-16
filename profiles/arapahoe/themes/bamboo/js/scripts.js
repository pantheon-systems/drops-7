/**
 * @file
 * Misc JQuery scripts in this file
 */

(function ($) {

// Add drupal 7 code.
  Drupal.behaviors.miscBamboo = {
    attach:function (context, settings) {
// End drupal calls.

      $('html').addClass('js');

      // Mobile menu.
      // http://webdesignerwall.com/demo/mobile-nav/
      // http://www.learningjquery.com/2008/10/1-way-
      // to-avoid-the-flash-of-unstyled-content
      // Add a menu depth for better theming.
      // Lets not render the menu until it's fully ready.
      // It's hidden with CSS initally

        $('#main-menu ul').each(function() {
          var depth = $(this).parents('ul').length;
          $(this).addClass('ul-depth-' + depth);
        });

      /* toggle nav */
      $("#menu-icon").click(function() {
        $("#main-menu ul.menu.ul-depth-0").toggle();
        $(this).toggleClass("active");
      });

      // End mobile menu.

// prepend the post date before the h1.
  $(".date-in-parts").prependTo(".not-front.page-node #post-content");

// Global zebra stripes and first / last.
  $(".front article:visible:even").addClass("even");
  $(".front article:visible:odd").addClass("odd");
  $(".front #post-content article:last").addClass("last");
  $(".front #post-content article:first").addClass("first");

// Set image captions for image field.
 if ( $("body").hasClass("imagecaption") ) {
  $(".field-type-image img").each(function (i, ele) {
    var alt = this.alt;
      if ($("img-caption").length == 0) {
        $(this).closest(".field-type-image .field-item").append("<span " +
          "class='img-caption'>" + this.alt + "</span>");
        }
        else {
          $(this).closest(".field-type-image .field-item").append("");
        }
  });
}

// Add an "arrow" span to primary menus that are expanded.
  $('#main-menu ul li.expanded').each(function() {
    //if ($(this).hasClass('expanded')) {
      $(this).append("<span class='drop-arrow'></span>").html();
    //  }
  });

  }}})
(jQuery);
