/**
 * @file
 * Provides confirm forms for additional IPE buttons that are Panelizer specific.
 */
(function ($) {

Drupal.behaviors.PanelizerIPE = {
  attach: function (context) {
    $('input#panelizer-save-default', context).bind('mouseup', function (e) {
      if (!confirm(Drupal.t("This will save this configuration as the new default for all entities of this type. This action cannot be undone. Are you sure?"))) {
        this.ipeCancelThis = true;
        return false;
      }
    });
    $('input#panelizer-ipe-revert', context).bind('mouseup', function (e) {
      if (!confirm(Drupal.t("This will remove all panelizer configuration for this entity and revert it to default settings. This action cannot be undone. Are you sure?"))) {
        this.ipeCancelThis = true;
        return false;
      }
    });
  }
};

})(jQuery);


