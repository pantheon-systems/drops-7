(function ($) {
  Drupal.behaviors.commerceKickstartDfp = {
    attach: function (context, settings) {
      $('#progress', context).once('commerce-kickstart-dfp', function () {
        var dfpUnit = Drupal.settings.dfp.dfp_unit;
        if (dfpUnit) {
          var slot = $('<div id="dfp-wrapper"><div class="dfp-install-ad"><div id="div-gpt-ad" class="dfp-install-ad"></div></div></div>');
          $(this).parent().append(slot);
          if (typeof googletag != 'undefined') {
            googletag.cmd.push(function() {
              googletag.defineSlot("/17601239/" + dfpUnit, [300, 250], "div-gpt-ad").addService(googletag.pubads());
              googletag.pubads().enableSingleRequest();
              googletag.enableServices();
            });
            googletag.display('div-gpt-ad');
          }
        }
      });
    }
  };
})(jQuery);
