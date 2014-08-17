(function ($) {

  Drupal.behaviors.addthis = {
    attach: function(context, settings) {

      // If addthis settings are provided a display is loaded.
      if (typeof Drupal.settings.addthis != 'undefined') {

        if (typeof Drupal.settings.addthis.load_type != 'undefined') {
          if (Drupal.settings.addthis.load_type == 'async') {
            addthis.init();
          }
          if (Drupal.settings.addthis.load_type == 'domready') {
            $.getScript(
              Drupal.settings.addthis.widget_url,
              function(data, textStatus) {});
          }
          // Trigger ready on ajax attach.
          if (context != window.document && window.addthis != null) {
            window.addthis.ost = 0;
            window.addthis.ready();
          }
        }
      }

    }
  }

}(jQuery));
