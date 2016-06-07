L.Map = L.Map.extend({
  _extendBounds: function(bounds, coordinates) {
    var latlng = new L.LatLng(coordinates[1], coordinates[0]);
    bounds.extend(latlng);
    return bounds;
  },
  zoomToGeometries: function(geojson) {
    var i, j, k = 0;
    var bounds = new L.LatLngBounds();
    for (var layer in geojson._layers) {
      if(geojson._layers.hasOwnProperty(layer)) {
        var feature = geojson._layers[layer].feature;
        switch (feature.geometry.type) {
          case 'Point':
            bounds = this._extendBounds(bounds, feature.geometry.coordinates);
            break;
          case 'LineString':
          case 'MultiPoint':
          case 'MultiLineString':
          case 'Polygon':
            for (i = 0; i < feature.geometry.coordinates.length; i++) {
              for (j = 0; j < feature.geometry.coordinates[i].length; j++ ) {
                bounds = this._extendBounds(bounds, feature.geometry.coordinates[i][j]);
              }
            }
            break;
          case 'MultiPolygon':
            for (i = 0; i < feature.geometry.coordinates.length; i++) {
              for (j = 0; j < feature.geometry.coordinates[i].length; j++ ) {
                for (k = 0; k < feature.geometry.coordinates[i][j].length; k++) {
                  bounds = this._extendBounds(bounds, feature.geometry.coordinates[i][j][k]);
                }
              }  
            }
            break;
        }
      }
    }
    this.fitBounds(bounds);
  },
});
