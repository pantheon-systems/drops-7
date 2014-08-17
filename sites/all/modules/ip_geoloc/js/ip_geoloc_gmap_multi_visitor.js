(function ($) {

  Drupal.behaviors.addGMapMultiVisitor = {
    attach: function (context, settings) {

      var mapOptions = settings.ip_geoloc_multi_location_map_options;
      if (!mapOptions) {
        alert(Drupal.t('Syntax error in visitor map options.'));
        mapOptions = { mapTypeId: google.maps.MapTypeId.ROADMAP, zoom: 2 };
      }
      if (typeof(google) != 'object') {
        // When not connected to Internet.
        return;
      }
      var map = new google.maps.Map(document.getElementById(settings.ip_geoloc_multi_location_map_div), mapOptions);

      var locations = settings.ip_geoloc_locations;
      if (locations.length == 0) {
        // Don't pop up annoying alert. Just show blank map of the world.
        map.setZoom(0);
        map.setCenter(new google.maps.LatLng(0, 0));
      }

      var balloonTexts = [];
      var i = 0;
      for (key in locations) {
        var position = new google.maps.LatLng(locations[key].latitude, locations[key].longitude);
        if (++i == 1) {
          // Use the first, i.e. most recent, visitor to center the map.
          map.setCenter(position);
          mouseOverText = Drupal.t('Latest Visitor');
        }
        else {
          mouseOverText = Drupal.t('Visitor #@i', { '@i': i });
        }
        marker = new google.maps.Marker({ map: map, position: position, title: mouseOverText });

        balloonTexts['LL' + position] = Drupal.t('Lat/long @lat/@lng', {
          // @todo: fix to 4 decimals?
          '@lat': locations[key].latitude,
          '@lng': locations[key].longitude });

        if (locations[key].balloon_text) {
          balloonTexts['LL' + position] += '<br/>' + locations[key].balloon_text;
        }

        google.maps.event.addListener(marker, 'click',  function(event) {
          new google.maps.InfoWindow({
            content: balloonTexts['LL' + event.latLng],
            position: event.latLng,
            // See [#1777664].
            maxWidth: 200
          }).open(map);
        });
      }
    }
  }
})(jQuery);
