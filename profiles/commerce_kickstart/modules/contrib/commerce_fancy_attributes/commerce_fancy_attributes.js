(function ($) {
  Drupal.behaviors.commerce_fancy_attributes = {
    attach: function (context, settings) {
      $('.form-type-commerce-fancy-attributes').addClass('form-type-commerce-fancy-attributes-ajax');
      $('.form-type-commerce-fancy-attributes input[type=radio]').hide();
      $('.form-type-commerce-fancy-attributes label.option').hide();

      $('.form-type-commerce-fancy-attributes-ajax .description').click(function() {
        var parent = $(this).parent();
        $('input[type=radio]', parent).click();
        $('input[type=radio]', parent).change();
      });
    }
  };
}) (jQuery);
