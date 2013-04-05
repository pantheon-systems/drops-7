(function ($) {
  Drupal.behaviors.commerceKickstartDfpAdmin = {
    attach: function (context, settings) {
      // Setting the variables from dfp_settings array.
      var dfpSelector = Drupal.settings.dfp.dfp_selector;
      var dfpClass = Drupal.settings.dfp.dfp_class;
      var dfpPosition = Drupal.settings.dfp.dfp_position;
      var dfpHeight = parseInt(Drupal.settings.dfp.dfp_height);
      var dfpWidth = parseInt(Drupal.settings.dfp.dfp_width);
      var dfpId = Drupal.settings.dfp.dfp_id;
      var dfpUnit = Drupal.settings.dfp.dfp_unit;

      // Wrapping titles for header placement consistency between standard and overlay.
      $('.page-admin-commerce-orders h1:not(#overlay-content h1, #page h1, #title-wrapper h1)').wrap('<div id="title-wrapper" />');
      $('.page-admin-commerce-products h1:not(#overlay-content h1, #page h1, #title-wrapper h1)').wrap('<div id="title-wrapper" />');
      $('.page-admin-commerce-customer-profiles h1:not(#overlay-content h1, #page h1, #title-wrapper h1)').wrap('<div id="title-wrapper" />');

      // DFP placement script.
      $(dfpSelector, context).once('commerce-kickstart-dfp', function () {
        if (typeof googletag != 'undefined') {
          var slot = $('<div id="dfp-wrapper"><div id="' + dfpId + '" class="dfp-unit ' + dfpClass.join(' ') + '"></div></div>');
          if (dfpPosition == 'before') {
            $(this).prepend(slot);
          } else {
            $(this).append(slot);
          }
          googletag.cmd.push(function() {
            googletag.defineSlot('/17601239/' + dfpUnit, [dfpWidth, dfpHeight], dfpId).addService(googletag.pubads());
            googletag.pubads().enableSingleRequest();
            googletag.enableServices();
          });
          googletag.display(dfpId);
        }
      });
    }
  };
})(jQuery);
