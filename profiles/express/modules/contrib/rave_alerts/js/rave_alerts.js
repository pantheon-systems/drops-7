(function($) {

  $(document).ready(
    function alertWorker() {
      // @TODO: Add loop count or option to exit so this doesn't keep running after an 
      // active event has ended if someone leaves a browser open
      if ( Drupal.settings.rave_alerts_active_event == 1 ) {
				// Create our own rounded timestamp to limit paths cached by varnish
				var stamp = Math.ceil( $.now() / 10000 );
				//console.log(stamp);

				$.ajax({
					type: "GET",
					url: Drupal.settings.rave_alerts_rss_url + "?stamp=" + stamp,
					dataType: "xml",
					//cache: false, // Causes Varnish to miss every request
					success: rssParser,
					complete: function() {
						// Schedule the next request when the current one's complete
						setTimeout(alertWorker, 10000);
					}
				});
			};
    });

    function rssParser(xml) {
      $(xml).find("item").each(function () {
        if ($(this).find("link").text()) {
          feedLink = $(this).find("link").text();
        } else {
          // @TODO: get url from jQuery.extend(Drupal.settings
          feedLink = Drupal.settings.rave_alerts_deafult_read_more_url;
        }
        var stamp = Math.ceil( $.now() / 10000 );
        var alertTitle = $(this).find("description").text();
        var alertPubtime = $(this).find("pubDate").text();
        $("#rave-alerts .alert").html(alertTitle + ' <a href="' + feedLink + '" >' + 'Read More</a>');
        // data-alert-publish-time="' + alertPubtime + '" data-alert-timestamp="' + stamp + '"
        $("#rave-alerts .alert").attr('data-alert-publish-time', alertPubtime);
        $("#rave-alerts .alert").attr('data-alert-timestamp', stamp);
      });
    };

})(jQuery);
