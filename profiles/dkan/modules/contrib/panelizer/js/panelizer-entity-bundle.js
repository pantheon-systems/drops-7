/**
 * @file
 * This JavaScript provides Vertical Tabs integration with Panelizer so that the
 * tab displays a summary of what is enabled.
 */
(function ($) {
  'use strict';

Drupal.behaviors.panelizerFieldsetSummary = {
  attach: function (context) {
    $('fieldset.panelizer-entity-bundle', context).drupalSetSummary(function (context) {
      // Identify whether Panelizer is enabled.
      if ($('input#panelizer-status:checked', context).length === 0) {
        return Drupal.t('Not panelized');
      }

      // Indicate which view modes are panelized.
      var vals = [];
      $('input[name*="view modes"][name*=status]:checked', context).each(function () {
        vals.push(Drupal.t("@name: enabled", {'@name' : $(this).attr('title')}));
      });

      // The view modes might not actually be enabled.
      if (vals.length === 0) {
        return Drupal.t('No view modes enabled');
      }
      else {
        return vals.join('<br/>');
      }
    });
  }
};

})(jQuery);
