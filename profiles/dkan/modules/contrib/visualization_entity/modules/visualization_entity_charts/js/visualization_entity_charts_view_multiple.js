/**
 * @file
 * Provides options for chart visualization.
 */

(function ($) {

  function cleanURL(url){
    var haveProtocol = new RegExp('^(?:[a-z]+:)?//', 'i');
    if(haveProtocol.test(url)){
      url = url.replace(haveProtocol, '//');
    }
    return url;
  }

  Drupal.behaviors.VisualizationEntityChartsViewMultiple = {
    attach: function (context) {
      var charts = $('.chart');

      // Iterate over all the charts in the current page.
      $.each(charts, function(key, chart){
        var state;
        var model;
        var dataset;
        var graph;
        var $chartContainer;
        $chartSource = $(chart).find('.chart-source');
        $chartContainer = $(chart).find('.chart-container');

        // Grab serialized code from the chart source.
        // This is a hidden tag that contains all the
        // chart configuration stuff.
        state = $chartSource.text();

        if (state) {
          state = new recline.Model.ObjectState(JSON.parse(state));

          // Use height and width provided by the ctools
          // custom content.
          state.set('height', $chartContainer.height());
          state.set('width', $chartContainer.width());
          model = state.get('source');

          // We don't know if the url will have an https protocol
          // If so, and page is served using http then a security
          // error could be raised. In order to avoid this we
          // remove the protocol from the url.
          model.url = cleanURL(model.url);
          dataset = new recline.Model.Dataset(model);
          dataset.fetch().done(function(dataset){
            dataset.queryState.set(state.get('queryState'));
            graph = new recline.View.nvd3[state.get('graphType')]({
              model: dataset,
              state: state,
              el: $chartContainer
            });
            graph.render();
          });
        }
      });
    }
  };
})(jQuery);