(function($) {
  Drupal.behaviors.search_api_ranges = {
    attach: function(context, settings) {

      var submitTimeout = '';

      $('div.search-api-ranges-widget').each(function() {

        var widget = $(this);
        var slider = widget.find('div.range-slider');
        var rangeMin = widget.find('input[name=range-min]');
        var rangeMax = widget.find('input[name=range-max]');
        var rangeFrom = widget.find('input[name=range-from]');
        var rangeTo = widget.find('input[name=range-to]');

        slider.slider({
          range: true,
          animate: true,
          step: 1,
          min: parseInt(rangeMin.val()),
          max: parseInt(rangeMax.val()),
          values: [parseInt(rangeFrom.val()), parseInt(rangeTo.val())],

          // on change: when clicking somewhere in the bar
          change: function(event, ui) {
            widget.find('input[name=range-from]').val(ui.values[0]);
            widget.find('input[name=range-to]').val(ui.values[1]);
          },

          // on slide: when sliding with the controls
          slide: function(event, ui) {
            widget.find('input[name=range-from]').val(ui.values[0]);
            widget.find('input[name=range-to]').val(ui.values[1]);
          }
        });

        // submit once user stops changing values
        slider.bind('slidestop', function(event, ui) {
          clearTimeout(submitTimeout);
          delaySubmit(widget);
        });

        rangeFrom.numeric();
        rangeFrom.bind('keyup', function() {
          clearTimeout(submitTimeout);
          if (!isNaN(rangeFrom.val()) && rangeFrom.val() !== '') {
            var value = parseInt(rangeFrom.val());
            if (value > parseInt(rangeTo.val())) {
              value = parseInt(rangeTo.val());
            }
            slider.slider("option", "values", [value, parseInt(rangeTo.val())]);
            delaySubmit(widget);
          }
        });

        rangeTo.numeric();
        rangeTo.bind('keyup', function() {
          clearTimeout(submitTimeout);
          if (!isNaN(rangeTo.val()) && rangeTo.val() !== '') {
            var value = parseInt(rangeTo.val());
            if (value < parseInt(rangeFrom.val())) {
              value = parseInt(rangeFrom.val());
            }
            slider.slider("option", "values", [parseInt(rangeFrom.val()), value]);
            delaySubmit(widget);
          }
        });
      });

      function delaySubmit(widget) {
        var autoSubmitDelay = widget.find('input[name=delay]').val();
        if (autoSubmitDelay != undefined && autoSubmitDelay != 0) {
          submitTimeout = setTimeout(function() {
            widget.find('form').submit();
          }, autoSubmitDelay);
        }
      };
    }
  };
})(jQuery);
