/**
 * @file
 * A JavaScript file for the theme.
 *
 * In order for this JavaScript to be loaded on pages, see the instructions in
 * the README.txt next to this file.
 */

// JavaScript should be made compatible with libraries other than jQuery by
// wrapping it with an "anonymous closure". See:
// - https://drupal.org/node/1446420
// - http://www.adequatelygood.com/2010/3/JavaScript-Module-Pattern-In-Depth
(function ($, Drupal, window, document, undefined) {
  $(document).ready(function(){
    $("#toggle").click(function() {
      $("#mobile-navigation-wrapper").slideToggle('fast');
      $("#toggle i.fa").toggleClass('fa-reorder');
      $("#toggle i.fa").toggleClass('fa-times');
      return false;
    });
  });
})(jQuery, Drupal, this, this.document);
