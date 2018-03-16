(function ($) {

  Drupal.leaflet_widget = Drupal.leaflet_widget || {};

  Drupal.behaviors.geofield_widget = {
    attach: attach
  };

  function attach(context, settings) {
    $('div.leaflet-widget').once('leaflet-widget').each(function(i, item) {
      var id = $(item).attr('id'),
      options = settings.leaflet_widget_widget[id];

      var map = L.map(id, options.map);

      L.tileLayer(options.map.base_url).addTo(map);

      // Get initial geojson value
      var current = $('#' + id + '-input').val();
      current = JSON.parse(current);
      var layers = Array();
      if (current.features.length) {
        var geojson = L.geoJson(current)
        for (var key in geojson._layers) {
          layers.push(geojson._layers[key]);
        }
      }

      // Save ORIG layers to use with RESET button.
      Drupal.settings.leaflet_widget_widget[id]['orig_layers'] = layers;

      var Items = new L.FeatureGroup(layers).addTo(map);

      // Autocenter if that's cool.
      if (options.map.auto_center) {
        if (current.features.length) {
          map.fitBounds(Items.getBounds());
        }
      }

      // Add controls to the map
      var drawControl = new L.Control.Draw({
        autocenter: true,
        draw: {
          position: 'topleft',
          polygon: options.draw.tools.polygon,
          circle: options.draw.tools.circle,
          marker: options.draw.tools.marker,
          rectangle: options.draw.tools.rectangle,
          polyline: options.draw.tools.polyline
        },
        edit: {
          featureGroup: Items
        }
      });


      map.addControl(drawControl);

      map.on('draw:created', function (e) {
        Items.addLayer(e.layer);

        // Update the field input.
        leafletWidgetFormWrite(Items._layers, id)
      });

      // Remove layer from the input field that will be saved.
      map.on('draw:deleted', function(e) {
        var layers = e.layers;
        layers.eachLayer(function(layer) {
          leafletWidgetFormDelete(Items._layers, id, layers);
        });
      });

      map.on('draw:edited', function(e) {
        leafletWidgetFormWrite(Items._layers, id);
      });

      Drupal.leaflet_widget[id] = map;

      if (options.toggle) {
        $('#' + id).before('<ul class="ui-tabs-nav leaflet-widget">' +
          '<li><a href="#' + id + '">Map</a></li>' +
          '<li><a href="#' + id + '-geojson">GeoJSON</a></li>' +
          '<li><a href="#' + id + '-points">Points</a></li>' +
        '</ul>');

      $('#' + id).after('<div id="' + id + '-geojson">' +
        '<label for="' + id + '-geojson-textarea">' + Drupal.t('Enter GeoJSON:') + '</label>' +
        '<textarea class="text-full form-control form-textarea" id="' + id + '-geojson-textarea" title="GeoJSON textarea field" cols="60" rows="10"></textarea>' +
      '</div>');

    // Set placeholder
    $('#' + id + '-geojson-textarea').attr('placeholder', JSON.stringify({"type":"FeatureCollection","features":[]}));

    // Update field's input when geojson input is updated.
    // @TODO validate before sync
    $('#' + id + '-geojson-textarea').on('input', function(e) {
      if(!$('#' + id + '-geojson-textarea').val()) {
        $('#' + id + '-input').val(JSON.stringify({"type":"FeatureCollection","features":[]}));
      } else {
        $('#' + id + '-input').val($('#' + id + '-geojson-textarea').val());
      }
    });

    $('#' + id).after('<div id="' + id + '-points" class="">' +
      '<label for="' + id + '-points">' + Drupal.t('Points') + '</label>' +
      '<input class="text-full form-control form-text" type="text" id="' + id + '-points-input" title="Points text field" placeholder="latitude, longitude;latitude, longitude; ..." "size="60" maxlength="255"> <a href="#add-point" class="map btn btn-default btn-add-point btn-primary" id="' + id +'-points-add">Add Points</a>' +
    '</div>');

  // Update field's input when geojson input is updated.
  $('#' + id + '-points-add').on('click', function(e) {
    var points = $('#' + id + '-points-input').val().split(';');
    points.forEach(function(point) {
      if (!point) {
        return;
      }
      try {
        var latlng = L.latLng(point.split(','));
        var coordinates = LatLngToCoords(latlng, true);
        var geojsonFeature = {"type":"FeatureCollection","features":[{"type":"Feature","geometry":{"type":"Point","coordinates":coordinates},"properties":[]}]}
        var write = JSON.stringify(geojsonFeature);
        var l  = L.geoJson(geojsonFeature).addTo(map);
        leafletWidgetLayerAdd(l._layers, Items);
        leafletWidgetFormWrite(map._layers, id);
        map.fitBounds(Items.getBounds());
        $('#' + id + '-points-input').val("");
        map.fitBounds(Items.getBounds());
      }
      catch(e) {
        console.log(e);
      }
    });
  });

  // Add tabs.
  $('#' + id).parent().tabs();

  // Map tab is selected
  // Clear previous layers
  $('a[href="#' + id + '"]').click(function() {
    if (!$('div#' + id).is(':visible')) {
      $('div#' + id).show();
    }
    leafletWidgetLayerRemove(map._layers, Items);
    var current = $('#' + id + '-input').val();
    current = JSON.parse(current);
    if (current.features.length) {
      var geojson = L.geoJson(current)
      for (var key in geojson._layers) {
        // Add new layer.
        Items.addLayer(geojson._layers[key]);
      }
      map.fitBounds(Items.getBounds());
    }
  });

  // Reset button is selected.
  // Update field's input when geojson input is updated.
  $('#' + id + '-reset').click(function () {
    if ($('div#' + id).is(':visible')) {
      map.invalidateSize().setView(options.map['center'], options.map['zoom']);
      leafletWidgetLayerRemove(map._layers, Items);
      map._layers = Drupal.settings.leaflet_widget_widget[id]['orig_layers'];
      leafletWidgetFormWrite(layers, id);
      leafletWidgetLayerAdd(map._layers, Items);
      if (current.features.length && options.map.auto_center) {
        map.fitBounds(Items.getBounds());
      }
    }
  });

  // GeoJSON tab is selected
  // Sync from field's input
  $('a[href="#' + id + '-geojson"]').click(function() {
    if ($('div#' + id).is(':visible')) {
      $('div#' + id).hide();
    }
    $('#' + id + '-geojson-textarea').val($('#' + id + '-input').val());
  });

  // Points tab is selected
  $('a[href="#' + id + '-points"]').click(function() {
    if (!$('div#' + id).is(':visible')) {
      $('div#' + id).show();
    }
  });
      }


      if (options.geographic_areas) {
        var json_data = {};
        var selectList = "<div class='geographic_areas_desc'><p></br>Select a state to add into the map:</p><select id='geographic_areas' name='area'>";
        selectList += "<option value='0'>" + Drupal.t('-none-') + "</option>";

        for (i = 0; i < options.areas.length; i++) {
          json_data = jQuery.parseJSON(options.areas[i]);
          $.each(json_data.features, function (index, item) {
            selectList += "<option value='" + item.id + "'>" + item.properties.name + "</option>";
          });
        }

        selectList += "</select></div></br>";
        $('#' + id + '-input').before(selectList);

        $('#geographic_areas').change(function() {
          var area = $(this).val();

          for (i = 0; i < options.areas.length; i++) {
            json_data = jQuery.parseJSON(options.areas[i]);
            $.each(json_data.features, function (index, item) {
              if (item.id == area) {
                L.geoJson(item).addTo(map);
                leafletWidgetFormWrite(map._layers, id);
              }
            });
          }
        });
      }
    });
  }

  /**
  * Writes layer to input field if there is a layer to write.
  */
  function leafletWidgetFormWrite(layers, id) {
    var write  = Array();
    for (var key in layers) {
      if (layers[key]._latlngs || layers[key]._latlng) {
        var feature = '{ "type": "Feature","geometry":' + layerToGeometry(layers[key]) + '}';
        write.push(feature);
      }
    }
    // If no value then provide empty collection.
    if (!write.length) {
      write = JSON.stringify({"type":"FeatureCollection","features":[]});
    }
    $('#' + id + '-input').val('{"type":"FeatureCollection", "features":[' + write + ']}');
  }

  /**
  * Filters out layer from input if layer exists.
  */
  function leafletWidgetFormDelete(layers, id, layer) {
    var write  = Array();
    for (var key in layers) {
      if (layers[key]._latlngs || layers[key]._latlng) {
        var feature = '{ "type": "Feature","geometry":' + layerToGeometry(layers[key]) + '}';
        if (layer.feature != layers[key].feature) {
          write.push(feature);
        }
      }
    }

    // If no value then provide empty collection.
    if (!write.length) {
      write = JSON.stringify({"type":"FeatureCollection","features":[]});
    }
    $('#' + id + '-input').val('{"type":"FeatureCollection", "features":[' + write + ']}');
  }

  /**
  * Add layers that are already on the map.
  */
  function leafletWidgetLayerAdd(layers, Items) {
    for (var key in layers) {
      if (layers[key]._latlngs || layers[key]._latlng) {
        Items.addLayer(layers[key]);
      }
    }
  }

  /**
  * Removes layers that are already on the map.
  */
  function leafletWidgetLayerRemove(layers, Items) {
    for (var key in layers) {
      if (layers[key]._latlngs || layers[key]._latlng) {
        Items.removeLayer(layers[key]);
      }
    }
  }

  // This will all go away once this gets into leaflet main branch:
  // https://github.com/jfirebaugh/Leaflet/commit/4bc36d4c1926d7c68c966264f3cbf179089bd998
  var layerToGeometry = function(layer) {
    var json, type, latlng, latlngs = [], i;

    if (L.Marker && (layer instanceof L.Marker)) {
      type = 'Point';
      latlng = LatLngToCoords(layer._latlng);
      return JSON.stringify({"type": type, "coordinates": latlng});

    } else if (L.Polygon && (layer instanceof L.Polygon)) {
      type = 'Polygon';
      latlngs = LatLngsToCoords(layer._latlngs[0], 1);
      return JSON.stringify({"type": type, "coordinates": [latlngs]});

    } else if (L.Polyline && (layer instanceof L.Polyline)) {
      type = 'LineString';
      latlngs = LatLngsToCoords(layer._latlngs);
      return JSON.stringify({"type": type, "coordinates": latlngs});

    }
  }

  var LatLngToCoords = function (LatLng, reverse) { // (LatLng, Boolean) -> Array
    var lat = parseFloat(reverse ? LatLng.lng : LatLng.lat),
    lng = parseFloat(reverse ? LatLng.lat : LatLng.lng);

    return [lng,lat];
  }

  var LatLngsToCoords = function (LatLngs, levelsDeep, reverse) { // (LatLngs, Number, Boolean) -> Array
    var coord,
    coords = [],
    i, len;

    for (i = 0, len = LatLngs.length; i < len; i++) {
      coord = levelsDeep ?
      LatLngToCoords(LatLngs[i], levelsDeep - 1, reverse) :
      LatLngToCoords(LatLngs[i], reverse);
      coords.push(coord);
    }

    return coords;
  }

}(jQuery));
