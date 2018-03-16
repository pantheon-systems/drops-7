/**
 * @file
 * Provides options for chart visualization.
 */

(function ($) {
  Drupal.behaviors.VisualizationEntityChartsView = {
    attach: function (context) {
      var isIframe = !$('.content').is(':visible');
      var state = $('.field-name-field-ve-settings .field-item:eq(0)').text();
      var $el;
      var title;
      var height;
      var $body;

      if(state){
        state = new recline.Model.ObjectState(JSON.parse(state));
        $body = $(document.body);
        $window = $(window);
        $body.removeClass('admin-menu');

        if($('#iframe-shell').length){
          $el = $('#iframe-shell');
          if(state.get('showTitle')){
            title = $el.find('h2 a').html();
            $body.prepend('<h2 class="chartTitle">' + title + '</h2>');
            height = getChartHeight(true);
            resize();
          } else {
            height = getChartHeight(false);
          }
          state.set('height', height);
          state.set('width', $window.width() - 10);
          $window.on('resize', resize);
        } else {
          $el = $('#graph');
          state.set('width', $('.field-name-field-ve-settings').width());
        }

        var source = state.get('source');
        var graph = null;
        source.url = cleanURL(source.url);
        var model = new recline.Model.Dataset(source);
        state.set('model', model);
        state.get('model').queryState.attributes = state.get('queryState');

        graph = new recline.View.nvd3[state.get('graphType')]({
          model: model,
          state: state,
          el: $el
        });
        model.fetch().done(graph.render.bind(graph)).fail(function (err) {
          console.log(err);
          alert('Failed to fetch the resource');
        });
      }

      function cleanURL(url){
        var haveProtocol = new RegExp('^(?:[a-z]+:)?//', 'i');
        if(haveProtocol.test(url)){
          url = url.replace(haveProtocol, '//');
        }
        return url;
      }

      function resize(){
        var $title = $body.find('h2.chartTitle');
        var hasTitle = !!$title.length;
        $title.css({marginTop:'0px', padding:'20px', marginBottom:'0px'});
        var height = getChartHeight(hasTitle);
        $('.recline-nvd3').height(height);
        $('#iframe-shell').height(height);

      }
      function getChartHeight(hasTitle) {
        var height = (!hasTitle)
          ? $(window).height()
          : $(window).height() - $body.find('h2.chartTitle').outerHeight(true);

        return height;
      }

    }
  };
})(jQuery);
