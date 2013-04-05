(function ($) {

Drupal.behaviors.commerceDiscount = {};
Drupal.behaviors.commerceDiscount.attach = function(context) {
  $('input:radio').change(function() {
    $('input:radio:not(:checked)').closest('div').removeClass('selected');
    $(this).closest('div').addClass('selected');
  })
  .filter(':checked').closest('div').addClass('selected');
};

})(jQuery);
