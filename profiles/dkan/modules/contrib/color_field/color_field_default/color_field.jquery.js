/**
 * @file
 * Javascript for Color Field.
 */
(function ($) {
  Drupal.behaviors.color_field = {
    attach: function (context) {
      $.each(Drupal.settings.color_field, function (selector) {
        var id = selector.replace("#div","edit");
        var value = $('#' + id).val();
        if (value == '') value = this.value;
        $(selector).empty().addColorPicker({
          currentColor:value,
          colors:this.colors,
          clickCallback: function(c) {
            id = selector;
            id = id.replace("#div","edit");
            $('#' + id).val(c);
          }
        });
      });
    }
  };
})(jQuery);
