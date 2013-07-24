(function ($) {
  Drupal.behaviors.ws_pb = {
    attach: function(context, settings) {
      var $buttons = $('a.service-links-pinterest-button', context).once('service-links-pinterest-button');
      $buttons.each(function(){
        $(this).attr('count-layout', Drupal.settings.ws_pb.countlayout);
      });
      $.getScript( '//assets.pinterest.com/js/pinit.js' );
    }
  }
})(jQuery);
