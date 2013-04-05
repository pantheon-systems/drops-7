(function ($) {
  Drupal.behaviors.commerce_kickstartslideshow_custom = {
    attach: function(context, settings) {

      // Conflict betwwen this script bxslider and ctools for display the modal.
      // Hack: add a class on slider when the pager is displayed and test after if the class exist.
      var processed = $('.event-slider', context).hasClass('pager-processed');
      if (typeof $.fn.bxSlider != 'undefined' && processed == false) {
        // bx Slider.
        var slider = $('.event-slider', context).bxSlider({
          auto: true,
          autoHover: true,
          controls: true,
          pause: 5000,
          hideControlOnEnd: false,
          mode: 'fade',
          pager: false,
          prevText: '<span class="control">' + $('.event-slider .views-row:last .views-field-nothing').html() + '</span>',
          nextText: '<span class="control">' + $('.event-slider .views-row-2 .views-field-nothing').html() + '</span>',
          onSlideBefore: function($slideElement, oldIndex, newIndex) {
            Drupal.behaviors.commerce_kickstartslideshow_custom.processSlide(newIndex, slider.getSlideCount(), $slideElement);
          },
          onSliderLoad: function(currentIndex) {
            var $slideElement = $($('.event-slider li', context)[currentIndex]);
            Drupal.behaviors.commerce_kickstartslideshow_custom.processSlide(currentIndex, slider.getSlideCount(), $slideElement);
          },
          speed: 400
        });
        $('.event-slider', context).addClass('pager-processed')
      };
    },
    processSlide: function(currentSlideNumber, totalSlideQty, currentSlideHtmlObject){
      var leftSlideNumber = currentSlideNumber == 0 ? (totalSlideQty - 1) : (currentSlideNumber - 1);
      var rightSlideNumber = currentSlideNumber == (totalSlideQty - 1) ? 0 : (currentSlideNumber + 1);
      var leftSlideText = $(currentSlideHtmlObject).parents('.event-slider').find('.views-row-' + (leftSlideNumber + 1) + ':first .views-field-nothing').html();
      var rightSlideText = $(currentSlideHtmlObject).parents('.event-slider').find('.views-row-' + (rightSlideNumber + 1) + ':first .views-field-nothing').html();
      $(currentSlideHtmlObject).parents('.bx-wrapper').find('a.bx-prev span').html(leftSlideText);
      $(currentSlideHtmlObject).parents('.bx-wrapper').find('a.bx-next span').html(rightSlideText);
    }
  }

})(jQuery);
