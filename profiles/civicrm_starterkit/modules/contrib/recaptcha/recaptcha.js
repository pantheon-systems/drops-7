(function ($) {
  Drupal.behaviors.recaptcha = {
    attach: function (context) {
      $(document).ready(Recaptcha.create(Drupal.settings.recaptcha.public_key, Drupal.settings.recaptcha.container, {theme: Drupal.settings.recaptcha.theme}));
    },
    detach: function (context) {}
  };
}(jQuery));
