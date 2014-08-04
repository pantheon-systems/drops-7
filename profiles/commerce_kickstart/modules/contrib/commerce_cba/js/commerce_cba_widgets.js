(function ($) {

  // Declaring a namespace so other modules could potentially extend it.
  Drupal.CommerceCba = {callbacks: {} };

  // Checkout button.
  Drupal.CommerceCba.callbacks.commerce_cba_redirect_checkout = function(widget) {
    if (widget.type == 'InlineCheckoutWidget' || widget.type == 'ExpressCheckoutWidget') {
      // Checkout flag for the order.
      Drupal.CommerceCba.callbacks.commerce_cba_add_widget_info(widget);
    }
  };

  // Checkout addresswidget.
  Drupal.CommerceCba.callbacks.commerce_cba_address_redirect_checkout = function(widget) {
    if (widget.type == 'InlineCheckoutWidget') {
      var params = $.deparam.querystring(location.href);
      var obj = {'addresswidget' : [widget.checkout_pane]};
      if ('addresswidget' in params) {
        for (var element in params['addresswidget']) {
          obj['addresswidget'].push(params['addresswidget'][element]);
        }
      }
      window.location = $.param.querystring(location.href, obj);
    }
  };

  // Flag the Order for the Amazon widgets used.
  Drupal.CommerceCba.callbacks.commerce_cba_add_widget_info = function(widget) {
    if (widget.type == 'AddressWidget') {
      var type = widget.getDestinationName();
      // WalletWidget on the same page, make it visible.
      if ($('#amazonwalletwidget').length > 0) {
        $('#amazonwalletwidget').show();
        $('#amazonwalletwidget-message').remove();
      }
    }
    else if (widget.type == 'WalletWidget') {
      var type = 'wallet';
    }
    else if (widget.type == 'InlineCheckoutWidget') {
      var type = 'inline-checkout';
    }
    else if (widget.type == 'ExpressCheckoutWidget') {
      var type = 'express-checkout';
    }
    $.ajax({
      type: 'POST',
      dataType: 'json',
      data: {purchaseContractId: widget.purchaseContractId},
      url: Drupal.settings.basePath + 'commerce_cba/setorder/' + type,
      success: function (data, textStatus, jqXHR) {
        // Redirection if needed.
        if (widget.redirect) {
          window.location = widget.redirect;
        }
      },
      error: function(jqXHR, textStatus, errorThrown) {}
    });
  };

  Drupal.behaviors.commerceCbaWidget = {
    attach: function (context, settings) {
      for (var key in settings.commerce_cba) {
        var cba_settings = settings.commerce_cba[key];
        var widget = new CBA.Widgets[cba_settings.widget_type]({
          merchantId: cba_settings.merchantId,
          purchaseContractId: cba_settings.purchaseContractId,
          orderID: cba_settings.orderId,
          checkout_pane: cba_settings.checkout_pane,
          displayMode: cba_settings.displayMode,
          destinationName: cba_settings.destinationName,
          design: cba_settings.design
        });

        if (cba_settings.hasOwnProperty('callbacks')) {
          for (var callback in cba_settings.callbacks) {
            widget[callback] = function(widget) {
              if (cba_settings.callbacks.hasOwnProperty(callback)) {
                Drupal.CommerceCba.callbacks[cba_settings.callbacks[callback]](widget);
              }
            };
          }
        }

        // Add extra settings.
        for (var index in cba_settings.settings) {
          widget[index] = cba_settings.settings[index];
        }
        widget.render(key);

        // AddressWidget and WalletWidget on the same page.
        // Hide the WalletWidget.
        if ($('#amazonwalletwidget').length > 0 && $('.commerce_cba_addresswidget #amazonaddresswidget').length > 0) {
          $('#amazonwalletwidget').hide();
        }
      }
    }
 };

})(jQuery);
