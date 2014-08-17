/**
 * @package commerce_paypal_ec
 * @description Small script that displays a popin for PayPal bill me later.
 */

(function ($) {
  Drupal.behaviors.commercePaypalEc = {
    attach: function (context, settings) {
      $('.paypal-bml-popin-text').hide();
      $('.paypal-bml-popin').click(function (event) {
        event.preventDefault();
        $('.paypal-bml-popin-text').dialog({modal:true});
      });
    }
  };
})(jQuery);
