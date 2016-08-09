/**
 * @file
 * This javascript provides Vertical Tabs integration with Panelizer so that the
 * tab displays the correct value of the field within the tab.
 */

(function ($) {
  'use strict';

Drupal.behaviors.panelizerFieldsetSummary = {
  attach: function (context) {
    $('fieldset.panelizer-entity-options', context).drupalSetSummary(function (context) {
      var val = $('select', context).val();
      if (val === 0) {
        return Drupal.t('Not panelized');
      }
      return Drupal.t('Use display @name',
        { '@name' : $('option[value="' + $('select', context).val() + '"]', context).html() });
    });
  }
};

})(jQuery);
