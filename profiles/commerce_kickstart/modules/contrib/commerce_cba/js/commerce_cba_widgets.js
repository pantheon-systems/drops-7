(function ($) {
  Drupal.behaviors.commerceCbaWidget = {
    attach: function (context, settings) {
      if (context == document) {
        var callbacks = {
          commerce_cba_redirect_checkout: function(widget) {
            window.location = $.param.querystring('//' + location.host + settings.basePath + 'checkout', 'purchaseContractId=' + widget.getPurchaseContractId());
          },
          commerce_cba_address_redirect_checkout: function(widget) {
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
          },
          commerce_cba_add_widget_info: function(widget) {
            if (widget.type == 'AddressWidget') {
              var type = widget.getDestinationName();
            }
            else if (widget.type == 'WalletWidget') {
              var type = 'wallet';
            }
            $.ajax({
              type: 'POST',
              dataType: 'json',
              data: {purchaseContractId: widget.purchaseContractId},
              url: settings.basePath + 'commerce_cba/setorder/' + type,
              success: function (data, textStatus, jqXHR) {},
              error: function(jqXHR, textStatus, errorThrown) {}
            });
          }
        };

        for (var key in settings.commerce_cba) {
          var cba_settings = settings.commerce_cba[key];
          var widget = new CBA.Widgets[cba_settings.widget_type]({
            merchantId: cba_settings.merchantId,
            purchaseContractId: cba_settings.purchaseContractId,
            orderID: cba_settings.orderId,
            checkout_pane: cba_settings.checkout_pane,
            displayMode: cba_settings.displayMode,
            destinationName: cba_settings.destinationName
          });

          if (cba_settings.hasOwnProperty('callbacks')) {
            for (var callback in cba_settings.callbacks) {
              widget[callback] = function(widget) {
                if (cba_settings.callbacks.hasOwnProperty(callback)) {
                  callbacks[cba_settings.callbacks[callback]](widget);
                }
              };
            }
          }

          // Add extra settings.
          for (var index in cba_settings.settings) {
            widget[index] = cba_settings.settings[index];
          }

          widget.render(key);
        }
      }
    }
 };

})(jQuery);
