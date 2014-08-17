(function ($) {

  Drupal.behaviors.addGMapCurrentLocation = {
    attach: function (context, settings) {

      if (typeof(google) !== 'object') {
        // When not connected to Internet.
        return;
      }
      // Start with a map canvas, then add marker and balloon with address info
      // when the geo-position comes in, if not supplied already.
      var mapOptions = settings.ip_geoloc_current_location_map_options;
      if (!mapOptions) {
        mapOptions = { mapTypeId: google.maps.MapTypeId.ROADMAP, zoom: 15 };
      }
      var map = new google.maps.Map(document.getElementById(settings.ip_geoloc_current_location_map_div), mapOptions);

      var latLng = settings.ip_geoloc_current_location_map_latlng;
      if (latLng[0] === null || latLng[1] === null) {
        if (navigator.geolocation) {
          // Note that we use the same function for normal and error behaviours.
          navigator.geolocation.getCurrentPosition(displayMap, displayMap, {enableHighAccuracy: true});
        }
        else {
          // Don't pop up annoying alert. Just show blank map of the world.
          map.setZoom(0);
          map.setCenter(new google.maps.LatLng(0, 0));
        }
      }
      else {
        var center = new google.maps.LatLng(latLng[0], latLng[1]);
        map.setCenter(center);
        var marker = new google.maps.Marker({ map: map, position: center });
        var infoText = settings.ip_geoloc_current_location_map_info_text;
        var lat = latLng[0].toFixed(4);
        var lon = latLng[1].toFixed(4);
        var latLongText = Drupal.t('lat. !lat, lon. !lon', { '!lat': lat, '!lon': lon });
        var text = infoText ? infoText + '<br/>' + latLongText : latLongText;
        var infoPopUp = new google.maps.InfoWindow({ content: text });
        google.maps.event.addListener(marker, 'click', function() { infoPopUp.open(map, marker) });
        // google.maps.event.addListener(map, 'center_changed', function() {
        //   alert('New coords: ' + map.getCenter().lat() + ', ' + map.getCenter().lng());
        // });
      }

      function displayMap(position) {
        if (!position.coords) {
          // If the user declined to share their location or if there was some
          // other error, stop here.
          map.setZoom(0);
          map.setCenter(new google.maps.LatLng(0, 0));
          return;
        }
        var coords = position.coords;
        var center = new google.maps.LatLng(coords.latitude, coords.longitude);
        map.setCenter(center);
        var marker = new google.maps.Marker({ map: map, position: center });
        new google.maps.Geocoder().geocode({'latLng': center}, function(response, status) {
          var infoText = '?';
          if (status === google.maps.GeocoderStatus.OK) {
            infoText = response[0]['formatted_address'];
          }
          else {
            alert(Drupal.t('IPGV&M: Google address lookup for HTML5 position failed with status code !code.', { '!code': status }));
          }
          var lat = coords.latitude.toFixed(4);
          var lon = coords.longitude.toFixed(4);
          var latLongText = Drupal.t('lat. !lat, lon. !lon', { '!lat': lat, '!lon': lon }) + '<br/>' +
            Drupal.t('accuracy !accuracy m', { '!accuracy': coords.accuracy });
          var infoPopUp = new google.maps.InfoWindow({ content: infoText + '<br/>' + latLongText });
          google.maps.event.addListener(marker, 'click', function() { infoPopUp.open(map, marker) })
        });
      }
    }
  };
})(jQuery);
