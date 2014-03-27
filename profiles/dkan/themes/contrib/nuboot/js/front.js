/**
 * @file
 * JS for front page.
 */
(function ($) {
  Drupal.behaviors.numegaFront = {
    attach: function (context) {
      // Fade in 'add dataset' block.
      $('#block-numega-sitewide-demo-front-numega-add-front').delay(1500).fadeIn();
      // Remove 'add dataset' block.
      $('#block-numega-sitewide-demo-front-numega-add-front a.close').click(function() {
        $('#block-numega-sitewide-demo-front-numega-add-front').fadeOut();
      });
    }
  }
})(jQuery);

