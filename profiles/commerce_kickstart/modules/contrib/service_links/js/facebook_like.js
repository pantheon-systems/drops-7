(function ($) {
  Drupal.behaviors.ws_fl = {
    attach: function (context, settings) {
      $('a.service-links-facebook-like', context).each(function(){
        var iframe = document.createElement('iframe');

        iframe.src = $(this).attr('href').replace('http://', '//');
        iframe.setAttribute('scrolling', 'no');
        iframe.setAttribute('frameborder', 0);
        iframe.setAttribute('allowTransparency', 'true');

        $(iframe).css({
          'border': 'none',
          'overflow': 'hidden',
          'width': Drupal.settings.ws_fl.width + 'px',
          'height': Drupal.settings.ws_fl.height + 'px',
        });
       
        $(iframe).addClass($(this).attr('class'));

        $(this).replaceWith(iframe);
      });
    }
  }
})(jQuery);
