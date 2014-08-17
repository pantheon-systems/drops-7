/* Use this when you want several small maps on the same page, e.g. via Views */

function displayGMap(latitude, longitude, elementId, balloonText) {

  if (typeof(google) != 'object') {
    // When not connected to Internet.
    return;
  }
  var mapOptions = {
    mapTypeId: google.maps.MapTypeId.ROADMAP,
    disableDefaultUI: true,
    zoom: 15,
    zoomControl: true
  };
  var map = new google.maps.Map(document.getElementById(elementId), mapOptions);
  var position = new google.maps.LatLng(latitude, longitude);
  map.setCenter(position);
  var marker = new google.maps.Marker({ map: map, position: position });
  if (balloonText) {
    var infoPopUp = new google.maps.InfoWindow({ content: balloonText });
    google.maps.event.addListener(marker, 'click', function() { infoPopUp.open(map, marker) });
  }
}
