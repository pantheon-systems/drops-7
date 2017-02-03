/**
 * @file
 * Javascript for Color Field.
 */
(function ($) {
  Drupal.behaviors.color_field_jquery_simple_color = {
    attach: function (context) {
      $.each(Drupal.settings.color_field_jquery_simple_color, function (selector) {
        if (!$(selector).hasClass('jquery-simple-color-init')) {
          $(selector).simpleColor({
            cellWidth: this.cell_width,
            cellHeight: this.cell_height,
            cellMargin: this.cell_margin,
            boxWidth: this.box_width,
            boxHeight: this.box_height
          });
          $(selector).addClass('jquery-simple-color-init');
        }
      });
    }
  };
})(jQuery);
