/**
 * Visualization for arcgis and rest files.
 */

(function ($) {
  Drupal.behaviors.Recline = {
    attach: function (context) {
      if (typeof Drupal.settings.recline !== 'undefined' && typeof Drupal.settings.recline.url !== 'undefined') {
        var map = L.map('rest-map').setView([Drupal.settings.recline.lat, Drupal.settings.recline.lon], 4);
        L.esri.basemapLayer('Gray').addTo(map);
        L.esri.dynamicMapLayer({
          url: Drupal.settings.recline.url,
          opacity: 0.5,
          useCors: false
        }).addTo(map);
        var query = L.esri.Tasks.query({
          url: Drupal.settings.recline.url + '/0'
        });
        query.bounds(function(error, latLngBounds, response){
          map.fitBounds(latLngBounds);
        });
      }
    }
  }
})(jQuery);