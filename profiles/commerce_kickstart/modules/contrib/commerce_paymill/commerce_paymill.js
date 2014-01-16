/**
 * @file
 * Generates Paymill tokens and handles client side errors.
 */

(function($) {

  Drupal.behaviors.commercePaymill = {
    attach: function(context, settings) {
      window.PAYMILL_PUBLIC_KEY = settings.commercePaymill.publicKey;
      if ($('input.paymill-token', context).size() > 0) {
        var form = $('input.paymill-token', context).parents('form');
        if (!$(form).hasClass('paymill-preprocessed')) {
          $(form).addClass('paymill-preprocessed');
          $(form).submit(function(event) {
            // If as only one submit button.
            if ($('input[type=submit], button[type=submit]').size() == 1) {
              $('input[type=submit], button[type=submit]').click();
            }
            if (!$(form).hasClass('paymill-processed')) {
              event.preventDefault();
              return false;
            }
          });
          $('input[type=submit]', form).click(function() {
            if (!$(form).hasClass('paymill-processed')) {
              paymillGetToken(form, this);
            }
          });
        }
      }
    }
  };

  function paymillGetToken(form, button) {
    var cardHolderName = $('.paymill-owner', form).val();
    var cardNumber = $('.paymill-number', form).val();
    var expMonth = $('.paymill-exp-month', form).val();
    var expYear = $('.paymill-exp-year', form).val();
    var securityCode = $('.paymill-code', form).val();
    var orderAmount = $('.paymill-amount', form).val();
    var orderCurrency = $('.paymill-currency', form).val();
    paymill.config('3ds_cancel_label', Drupal.t('Cancel'));
    paymill.createToken({
        number: cardNumber,
        exp_month: expMonth,
        exp_year: expYear,
        cvc: securityCode,
        cardholder: cardHolderName,
        amount: orderAmount,
        currency: orderCurrency
      },
      function(error, result) {
        if (error) {
          $('.paymill-error', form).val(error.apierror);
          if (typeof error.message != "undefined") {
            $('.paymill-error-message', form).val(error.message);
          }
          else {
            $('.paymill-error-message', form).val('');
          }
        }
        else {
          $('.paymill-error', form).val('');
          $('.paymill-error-message', form).val('');
        }
        if (result) {
          $('.paymill-token', form).val(result.token);
        }
        else {
          $('.paymill-token', form).val('');
        }
        $(form).removeClass('paymill-preprocessed');
        $(form).addClass('paymill-processed');
        $(button).click();
      });


  }

})(jQuery);

