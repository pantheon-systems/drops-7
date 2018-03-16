/**
 * Visualization for arcgis and rest files.
 */

(function ($) {
  Drupal.behaviors.Recline = {
    attach: function (context) {
      if (typeof Drupal.settings.recline !== 'undefined' && typeof Drupal.settings.recline.url !== 'undefined') {
        var map = L.map('rest-map').setView([Drupal.settings.recline.lat, Drupal.settings.recline.lon], 4);
        var baseLayer = L.esri.basemapLayer('Gray').addTo(map);
        var fl = L.esri.dynamicMapLayer({
          url: Drupal.settings.recline.url,
          opacity: 0.5,
          useCors: false
        }).addTo(map);
        var bounds = L.latLngBounds([]);

        fl.metadata(function(error, metadata){
          let layersIds = metadata.layers.map(l => l.id);
          let counter = sl.length;
          layersIds.forEach(id => {
            L.esri.query({
              url: Drupal.settings.recline.url + '/' + id
            }).bounds(function(error, latLngBounds, response){
              counter--;
              bounds.extend(latLngBounds);
              if(!counter) {
                map.fitBounds(bounds);
              }
            });
          })
        });
      }
    }
  }
})(jQuery);