/**
 * Visualization for json.
 */

(function ($) {
  Drupal.behaviors.Recline = {
    attach: function (context) {
      if (typeof Drupal.settings.recline !== 'undefined' && typeof Drupal.settings.recline.data !== 'undefined') {
        var json = Drupal.settings.recline.data;
        $('#recline-data-json').JSONView(json);
        $('#recline-data-json').JSONView('collapse');
        $('#toggle-btn').on('click', function(){
          $('#recline-data-json').JSONView('toggle');
        });
      }
    }
  }
})(jQuery);