/**
 * @file
 * Provides confirm forms for additional IPE buttons that are Panelizer
 * specific.
 */

(function ($) {
  'use strict';

Drupal.behaviors.PanelizerIPE = {
  attach: function (context) {
    // Disable the Leave Page dialog warning.
    if ($(context).is('form#panels-ipe-edit-control-form.panels-ipe-edit-control-form')) {
      window.onbeforeunload = null;
      window.onunload = null;
    }

    // Update the default with this display.
    $('input#panelizer-save-default', context).once('save-default-alert', function() {
      $(this).bind('mouseup', function (e) {
        if (!confirm(Drupal.t("This will save this configuration as the new default for all entities of this type. This action cannot be undone. Are you sure?"))) {
          this.ipeCancelThis = true;
          return false;
        }
      });
    });

    // Redirect to confirmation page.
    $('input#panelizer-ipe-revert', context).once('revert-alert', function() {
      $(this).bind('mouseup', function (e) {
        window.location.href = Drupal.settings.panelizer.revert_default_url;
        this.ipeCancelThis = true;
        return false;
      });
    });
  }
};

})(jQuery);
