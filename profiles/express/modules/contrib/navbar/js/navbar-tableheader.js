/**
 * @file
 * Provide displacement information sticky tableheaders.
 */

(function ($, Drupal, displace) {

"use strict";

Drupal.navbar = Drupal.navbar || {};
  $.extend(Drupal.navbar, {
    calculatedHeight: NaN,
    calculateHeight: function () {
      var self = this;
      // Set the initial value of the height.
      if (isNaN(this.calculatedHeight)) {
        this.calculatedHeight = this.models.navbarModel.get('offsets').top;
      }
      // Add a listener to the model to update the offset only when changed.
      this.models.navbarModel.on('change:offsets', function(updated) {
        self.calculatedHeight = updated.changed.offsets.top;
      });
      return this.calculatedHeight;
    },
    // Height function required for sticky table headers.
    height: function () {
      // Returns the cached height value.
      return this.calculateHeight();
    }
  });

}(jQuery, Drupal, Drupal.displace));
