;(function($) {
  /**
   * Automatically submit the hidden form that points to the iframe.
   */
  Drupal.behaviors.commerceMoneybookersQuickCheckout = {
    attach: function (context, settings) {
      $('div.payment-redirect-form form', context).submit();
      $('div.payment-redirect-form #edit-submit', context).hide();
      $('div.payment-redirect-form .checkout-help', context).hide();
    }
  }
})(jQuery);
