(function($) {
jQuery(document).ready(function($) {
  /**
   * Pull the remote URL from the element attribute, which is
   * set by the backend for test or production
   */
  var json_alerts_url = $("#cu-alerts").attr('data-remote-url');

  /**
   * Pull the remote data.
   * While cache: false can be used, it appends a timestamp to every request
   * making varnish not be able to handle it. We like varnish, so keeping true.
   */
  if (json_alerts_url) {
    $.ajax({
      url: json_alerts_url,
      dataType: 'jsonp',
      jsonp: 'callback',
      jsonpCallback: "cu_alerts",
      //cache: false,
      success: loopResults
    });
  }

  function loopResults(data) {
    displayResults(data);
    if (data.length > 0) {
      var autoRefresh = setInterval(function() {

      $.ajax({
        url: json_alerts_url,
        dataType: 'jsonp',
        jsonp: 'callback',
        jsonpCallback: "cu_alerts",
        //cache: false,
        success: displayResults
      });

      }, 30000);
    }
  }

  /**
   * Function to display the results on the page
   */
  function displayResults(data) {
    $("#cu-alerts").html('');
    /* Total number of records returned */
    if(data.length > 0) {
      /* Grab the first result */
      var d = data[0];
      /* See if we have a URL or not */
      if (d.alert_url.length > 0) {
        $("#cu-alerts").append('<div class="alert js">' + d.title+"&nbsp;<a href='" + d.alert_url + "'>Read&nbsp;More&nbsp;&raquo;</a></div>\n");
      }
      else {
        $("#cu-alerts").append('<div class="alert js">' + d.title+"</div>\n");
      }
      $("#cu-alerts").slideDown(300);
    }
  }
});
})(jQuery);
