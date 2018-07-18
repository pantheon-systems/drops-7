(function ($) {

  Drupal.honeypot = {};
  Drupal.honeypot.timestampJS = new Date();

  Drupal.behaviors.honeypotJS = {
    attach: function (context, settings) {
      $('form.honeypot-timestamp-js').once('honeypot-timestamp').bind('submit', function() {
        var $honeypotTime = $(this).find('input[name="honeypot_time"]');
        $honeypotTime.attr('value', Drupal.behaviors.honeypotJS.getIntervalTimestamp());
      });
    },
    getIntervalTimestamp: function() {
      var now = new Date();
      var interval = Math.floor((now - Drupal.honeypot.timestampJS) / 1000);
      return Drupal.settings.honeypot.jsToken + '|' + interval;
    }
  };

  if (Drupal.ajax && Drupal.ajax.prototype && Drupal.ajax.prototype.beforeSubmit) {
    Drupal.ajax.prototype.honeypotOriginalBeforeSubmit = Drupal.ajax.prototype.beforeSubmit;
    Drupal.ajax.prototype.beforeSubmit = function (form_values, element, options) {
      if (this.form && $(this.form).hasClass('honeypot-timestamp-js')) {
        for (key in form_values) {
          // Inject the right interval timestamp.
          if (form_values[key].name == 'honeypot_time' && form_values[key].value == 'no_js_available') {
            form_values[key].value = Drupal.behaviors.honeypotJS.getIntervalTimestamp();
          }
        }
      }

      // Call the original function in case someone else has overridden it.
      return Drupal.ajax.prototype.honeypotOriginalBeforeSubmit(form_values, element, options);
    }
  }

}(jQuery));
