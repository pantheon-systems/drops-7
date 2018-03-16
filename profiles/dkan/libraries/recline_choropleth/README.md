Creates choropleth maps for Recline.

### Dependencies

https://github.com/NuCivic/leaflet.map.zoomToGeometries.js

### Usage

```
$.ajax({
  url: 'http://example.com/mycsvfile.csv',
  dataType: "text",
  timeout: 500,
  success: function(data) {
    data = data.replace(/(\r\n|\n|\r)/gm,"\n");
    var dataset = new recline.Model.Dataset({
      records: recline.Backend.CSV.parseCSV(data, options)
    });

    var geojson = 'http://example.com/mygeojsonfile.json';

    view = new recline.View.ChoroplethMap({
      // GeoJSON file.
      polygons: geojson,
      // Recline model.
      model: dataset,
      // Column in model that maps to GeoJSON key.
      map_column: "state_code", // As in US State (CA, PA, NY)
      // Optional column to use for choropleth data. If blank all numeric fields
      // are offered.
      selectable_fields: "",
      // Optional breakpoints for data.
      breakpoints: "",
      // GeoJSON property that maps to map_column.
      geojson_key: "stateCode", // Same data as map_column, may have a different name.
      // Human readable name in GeoJSON property list. Optional.
      geojson_label: "stateName,
      // Optional color scale.
      base_color: color_scale,
      // Optional. Can average instead of sum data in selectable fields.
      avg: dataset[0].avg,
      // Opptional. Units for displayed data.
      unitOfMeasure: dataset[0].unitOfMeasure,
    });
  }
});
```

### Output
![screen shot 2014-10-27 at 8 33 49 am](https://cloud.githubusercontent.com/assets/512243/4791528/f5086486-5ddd-11e4-9a7b-cf097f02a294.png)

### Field explanation
![viz_entity_explanation](https://cloud.githubusercontent.com/assets/512243/4800527/327bbd06-5e27-11e4-9332-20eead9c9b9a.png)
