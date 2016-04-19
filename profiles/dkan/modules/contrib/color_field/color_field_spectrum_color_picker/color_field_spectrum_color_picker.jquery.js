/**
 * @file
 * Javascript for Color Field.
 */
(function ($) {
  Drupal.behaviors.color_field_spectrum = {
    attach: function (context) {
      $.each(Drupal.settings.color_field_spectrum, function (selector) {
        $('#' + this.id).spectrum({
          preferredFormat: "hex",
          showInput: this.show_input,
          showAlpha: false,
          showInitial: true,
          showPalette: this.show_palette,
          showPaletteOnly: this.show_palette_only,
          palette:[this.palette],
          showButtons: this.show_buttons,
          allowEmpty: this.allow_empty,
        });
      });
    }
  };
})(jQuery);
