(function ($) {
  Drupal.behaviors.ws_fisheye = {
    attach: function (context, settings) {
      var $fisheyes = $('.fisheye', context).once('fisheye');
      $fisheyes.Fisheye({
        maxWidth: 32,
        items: 'a',
        itemsText: 'span',
        container: '.fisheyeContainer',
        itemWidth: 16,
        proximity: 60,
        halign : 'center'
      });
    }
  }
})(jQuery);
