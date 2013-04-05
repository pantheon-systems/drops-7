(function($) {
    Drupal.behaviors.commercehostedpci = {
        attach: function (context, settings) {
            $('#edit-commerce-payment-payment-details-credit-card-response-code', context).val('');
            $('#edit-commerce-payment-payment-details-credit-card-number', context).val('');

            $('#edit-commerce-payment', context).once('#edit-commerce-payment', function() {
                // Get the submit form element.
                var editBtn = $('#edit-commerce-payment').closest("form").find('#edit-continue');
                if (editBtn.length) {
                    // Attach the click element to this submit button.
                    $(editBtn, context).once(editBtn.attr('id'), function () {
                        $(this).bind('click', function (e) {
                            // Send to Hosted Pci if the payment method is
                            // selected.
                            if (($('#edit-commerce-payment-payment-method').length)
                            && $('#edit-commerce-payment #edit-commerce-payment-payment-method input[type=radio]:checked').attr('id') == 'edit-commerce-payment-payment-method-hosted-pcicommerce-payment-hosted-pci') {
                                // Request Hosted Pci if the cardonfile module is not enabled
                                if ($('#edit-commerce-payment .form-item-commerce-payment-payment-details-cardonfile').length == 0
                                    // Or if enabled, check if the payment method is a new one.
                                    || ($('#edit-commerce-payment .form-item-commerce-payment-payment-details-cardonfile')
                                        && Drupal.strPos($('#edit-commerce-payment .form-item-commerce-payment-payment-details-cardonfile input[type=radio]:checked').attr('id'), 'new'))) {
                                        var ret = sendHPCIMsg();
                                        return ret;
                                }
                            }
                        });
                    });
                }
            });

            $('#commerce-payment-order-transaction-add-form--2 #edit-submit', context).live('click', function() {
                // Send to Hosted Pci if the iframe is present.
                if ($('#commerce-payment-order-transaction-add-form--2').find('#ccframe').length) {
                    $('#edit-commerce-payment-payment-details-credit-card-response-code', context).val('');
                    var ret = sendHPCIMsg();
                    return ret;
                }
            });

            $('#commerce-cardonfile-update-form', context).once('#commerce-cardonfile-update-form', function() {
                // Get the submit form element.
                var editBtn = $('#commerce-cardonfile-update-form').closest("form").find('#edit-submit');
                if (editBtn.length) {
                    // Attach the click element to this submit button.
                    $(editBtn, context).once(editBtn.attr('id'), function () {
                        $(this).bind('click', function (e) {
                            if ($('#commerce-cardonfile-update-form').find('#ccframe')) {
                                // Remove any card number previously entered.
                                var ret = sendHPCIMsg();
                                return ret;
                            }
                        });
                    });
                }
            });
        }
    }

    // Because commerce_checkout.js prevent multiple accidental click on the
    // submit button, the js disable it. But in our case we have to let available
    // the button in the form.
    Drupal.enableContinueBtn = function () {
        var editBtn = $('#edit-commerce-payment').closest("form").find('#edit-continue').filter(':first');
        if (editBtn) {
           editBtn.show();
           editBtn.next().remove();
           editBtn.next('span').addClass('element-invisible');
        }
    }

    // Helper function to easily find if an Id exist or not
    // Because Drupal's form id change on every ajax loading I can't set it.
    Drupal.strPos = function (haystack, needle, offset) {
        var i = (haystack + '').indexOf(needle, (offset || 0));
        return i === -1 ? false : i;
    }

})(jQuery);
