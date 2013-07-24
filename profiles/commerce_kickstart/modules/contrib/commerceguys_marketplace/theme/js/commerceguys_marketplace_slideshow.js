(function ($) {
  Drupal.behaviors.commerceguys_marketplace_slideshow = {
    attach: function(context, settings) {
      if ($('body').hasClass('page-commerceguys-marketplace')) {
        var processed = $('#commerceguys-marketplace-slideshow', context).hasClass('pager-processed');
        if ($('#commerceguys-marketplace-slideshow li').length >= 2 && typeof $.fn.bxSlider != 'undefined' && processed == false) {
          $('#commerceguys-marketplace-slideshow', context).bxSlider({
            auto: true,
            pager: false,
            slideWidth: 580,
            moveSlides: 1,
            slideMargin: 0,
            randomStart: true
          });
        };
        $('#commerceguys-marketplace-slideshow', context).addClass('pager-processed');
      }
    }
  }

  // Append overlays on carousel.
  Drupal.behaviors.commerceguys_marketplace_slideshow_overlay = {
    attach: function(context, settings) {

      $('<div class="slide-overlay-left"></div>').appendTo('.bx-controls-direction');
      $('<div class="slide-overlay-right"></div>').appendTo('.bx-controls-direction');

      // Calculating overlay sizes.
      var slideWidth = $('.bx-viewport').width();
      var panelWidth = ($(window).width() - slideWidth) / 2;
      $('.slide-overlay-left').attr('style', 'width:' + panelWidth + 'px');
      $('.slide-overlay-right').attr('style', 'width:' + panelWidth + 'px');

      // Recalculating on window resize.
      $(window).resize(function() {
        var slideWidth = $('.bx-viewport').width();
        var slideHeight = $('.bx-viewport').height();
        var panelWidth = ($(window).width() - slideWidth) / 2;
        $('.slide-overlay-left').attr('style', 'width:' + panelWidth + 'px; height:' + slideHeight + 'px');
        $('.slide-overlay-right').attr('style', 'width:' + panelWidth + 'px; height:' + slideHeight + 'px');
      });
    }
  }

})(jQuery);
