(function($) {

/**
 * Adds a shipping quote recalculation change callback to customer profile fields.
 */
Drupal.behaviors.commerceShipping = {
  attach: function (context, settings) {
    $('[id^="edit-customer-profile-"] .form-item', context).children('.form-select, .form-text, .form-radio, .form-checkbox:not([name*="[commerce_customer_profile_copy]"])').filter(':not(.shipping-recalculation-processed)').addClass('shipping-recalculation-processed').change(function() {
      return $.fn.commerceCheckShippingRecalculation();
    });

    $(window).load(function() {
      return $.fn.commerceCheckShippingRecalculation();
    });
  }
}

/**
 * Checks to see if we can recalculate shipping rates and dispatches the command.
 */
$.fn.commerceCheckShippingRecalculation = function() {
  var recalculate = true;

  // Define the callback used with setTimeout to click the recalculation button
  // if there is ongoing AJAX operation.
  var recalculateCallback = function() {
    if ($('[id^="edit-customer-profile-"]').find('.ajax-progress').length) {
      return setTimeout($.fn.commerceCheckShippingRecalculation, 100);
    }

    // Trigger the click event on the shipping recalculation button.
    $('[id^="edit-commerce-shipping-recalculate"]').trigger('click');
  };

  // If other ajax logic is still running we can ignore the empty field check,
  // as we can expect the forms to change and we want to see what the backend
  // think we should do.
  if (!$('[id^="edit-customer-profile-"]').find('.ajax-progress').length) {
    $('[id^="edit-customer-profile-shipping"] .form-item').children('.required').filter(':not(.chosen-container)').each(function() {
      if (!$(this).val()) {
        recalculate = false;
      }
    });
  }

  if (recalculate == true) {
    return setTimeout(recalculateCallback, 100);
  }
}

})(jQuery);
