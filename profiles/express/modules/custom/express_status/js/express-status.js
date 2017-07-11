(function ($) {
  Drupal.behaviors.expressStatus = {
    attach: function (context, settings) {

      var statusID = Drupal.settings.express_status.statusio_id;
      var statusAPI = Drupal.settings.express_status.statusio_apikey;

      var sp = new StatusPage.page({page: statusID, apiKey: statusAPI});
      sp.summary({
        success: function (data) {
          statusioDisplay(data);
        }
      });
      function formattedDate(d) {
        let month = String(d.getMonth() + 1);
        let day = String(d.getDate());
        const year = String(d.getFullYear());
        var time = d.toLocaleString('en-US', {
          hour: 'numeric',
          minute: 'numeric',
          hour12: true
        });
        return `${month}/${day}/${year} ${time}`;
      }

      function statusioDisplay(data) {
        $('.express-status .status-message').text(data.status.description).addClass(data.status.indicator);
        var incidentDisplay = false;
        var statusDisplay = false;
        // Incidents.
        if (typeof data.incidents[0] !== 'undefined') {
          $.each(data.incidents[0].incident_updates, function (k, i) {
            incidentDisplay = true;
            var d = new Date(i.created_at);
            var incidentDate = formattedDate(d);
            $('.express-status .incidents .incidents-list').append('<div class="incident ' + i.impact + '">' + i.body + '<div class="incident-date">' + incidentDate + '</div></div>');

            if (incidentDisplay) {
              $('.express-status .incidents').show();
            }
          });
        }
        // Components.
        if (typeof data.components !== 'undefined') {
          $.each(data.components, function (k, c) {
            if (c.status != 'operational') {
              statusDisplay = true;
              $('.express-status .components .components-list').append('<div class="component ' + c.status + '">' + c.name + '</div>');
            }
            if (statusDisplay) {
              $('.express-status .components, .express-status .status-info').show();
            }
          });
        }
      }
    }
  };
})(jQuery);
