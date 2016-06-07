Adds a zoomToGeometries method to L.map

### Demo

http://nucivic.github.io/leaflet.map.zoomToGeometries.js/

### Usage

```js
var div = $('#map');
var map = new L.map(div.get(0));
var geojson = {
  type: "FeatureCollection",
  features: [
  {
  type: "Feature",
  id: "01",
  properties: {
  name: "Alabama",
  density: 94.65
  },
  geometry: {
  type: "Polygon",
  coordinates: [
    [
      [
        -87.359296,
        35.00118
      ],
      [
        -85.606675,
        34.984749
      ],
      [
        -85.431413,
        34.124869
      ],
      [
        -87.359296,
        35.00118
      ]
    ]
  ]
};
var geojson = new L.GeoJSON(geojson);
geojson.addTo(map);
map.zoomToGeometries(geojson);
```
