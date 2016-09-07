/**
 * Drupal-specific JS helper functions and utils. Not to be confused with the
 * Recline library, which should live in your libraries directory.
 */
;(function ($) {

    // Constants.
    var MAX_LABEL_WIDTH = 77;
    var LABEL_MARGIN = 5;

    // Undefined variables.
    var dataset, views, datasetOptions, fileSize, fileType, router;
    var dataExplorerSettings, state, $explorer, dataExplorer, maxSizePreview;
    var datastoreStatus;

    // Create drupal behavior
    Drupal.behaviors.Recline = {
      attach: function (context) {
        $explorer = $('.data-explorer');
        // Local scoped variables.
        fileSize = Drupal.settings.recline.fileSize;
        fileType = Drupal.settings.recline.fileType;
        maxSizePreview = Drupal.settings.recline.maxSizePreview;
        datastoreStatus = Drupal.settings.recline.datastoreStatus;

        dataExplorerSettings = {
          grid: Drupal.settings.recline.grid,
          graph: Drupal.settings.recline.graph,
          map: Drupal.settings.recline.map
        };

        // This is the very basic state collection.
        state = recline.View.parseQueryString(decodeURIComponent(window.location.hash));
        if ('#map' in state) {
          state.currentView = 'map';
        } else if ('#graph' in state) {
          state.currentView = 'graph';
        }

        // Init the explorer.
        init();

        // Attach toogle event.
        $('.recline-embed a.embed-link').on('click', function(){
          $(this).parents('.recline-embed').find('.embed-code-wrapper').toggle();
          return false;
        });
      }
    }

    // make Explorer creation / initialization in a function so we can call it
    // again and again
    function createExplorer (dataset, state, settings) {

      // Remove existing data explorer view.
      dataExplorer && dataExplorer.remove();

      var $el = $('<div />');
      $el.appendTo($explorer);

      var views = [];

      if (settings.grid) {
        views.push({
          id: 'grid',
          label: 'Grid',
          view: new recline.View.SlickGrid({
            model: dataset
          })
        });
      }
      if (settings.graph) {
        state.graphOptions = {
          xaxis: {
            tickFormatter: tickFormatter(dataset),
          },
          hooks:{
            processOffset: [processOffset(dataset)],
            bindEvents: [bindEvents],
          }
        };
        views.push({
          id: 'graph',
          label: 'Graph',
          view: new recline.View.Graph({
            model: dataset,
            state: state
          })
        });
      }
      if (settings.map) {
        views.push({
          id: 'map',
          label: 'Map',
          view: new recline.View.Map({
            model: dataset
          })
        });
      }

      // Multiview settings
      var multiviewOptions = {
        model: dataset,
        el: $el,
        state: state,
        views: views
      };

      // Getting base embed url.
      var urlBaseEmbed = $('.embed-code').text();
      var iframeOptions = {src: urlBaseEmbed, width:850, height:400};

      // Attaching router to dataexplorer state.
      dataExplorer = new recline.View.MultiView(multiviewOptions);
      router = new recline.DeepLink.Router(dataExplorer);

      // Adding router listeners.
      var changeEmbedCode = getEmbedCode(iframeOptions);
      router.on('init', changeEmbedCode);
      router.on('stateChange', changeEmbedCode);

      // Add map dependency just for map views.
      _.each(dataExplorer.pageViews, function(item, index){
        if(item.id && item.id === 'map'){
          var map = dataExplorer.pageViews[index].view.map;
          router.addDependency(new recline.DeepLink.Deps.Map(map, router));
        }
      });

      // Start to track state chages.
      router.start();

      $.event.trigger('createDataExplorer');
      return views;
    }

    // Returns the dataset configuration.
    function getDatasetOptions () {
      var datasetOptions = {};
      var delimiter = Drupal.settings.recline.delimiter;
      var file = Drupal.settings.recline.file;
      var uuid = Drupal.settings.recline.uuid;

     // Get correct file location, make sure not local
     file = (getOrigin(window.location) !== getOrigin(file)) ? '/node/' + Drupal.settings.recline.uuid + '/data' : file;

      // Select the backend to use
      switch(getBackend(datastoreStatus, fileType)) {
        case 'csv':
          datasetOptions = {
            backend: 'csv',
            url: file
          };
          break;
        case 'ckan':
          datasetOptions = {
            endpoint: '/api',
            id: uuid,
            backend: 'ckan'
          };
          break;
        case 'dataproxy':
          datasetOptions = {
            url: file,
            backend: 'dataproxy'
          };
          break;
        default:
          showError('File type ' + fileType + ' not supported for preview.');
          break;
      }
      return datasetOptions;
    }

     // Correct for fact that IE does not provide .origin
    function getOrigin(u) {
      var url = parseURL(u);
        return url.protocol + '//' + url.hostname + (url.port ? (':' + url.port) : '');
    }

    // Parse a simple URL string to get its properties
    function parseURL(url) {
      var parser = document.createElement('a');
      parser.href = url;
      return {
        protocol: parser.protocol,
        hostname: parser.hostname,
        port: parser.port,
        pathname: parser.pathname,
        search: parser.search,
        hash: parser.hash,
        host: parser.host
      }
    }

    // Retrieve a backend given a file type and and a datastore status.
    function getBackend (datastoreStatus, fileType) {
      if (datastoreStatus) return 'ckan';
      var formats = {
        'csv': ['text/csv', 'csv'],
        'dataproxy': ['application/vnd.ms-excel', 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'],
      };
      return _.findKey(formats, function(format) { return _.include(format, fileType) });
    }

    // Displays an error retrieved from the response object.
    function showRequestError (response) {

      // Actually dkan doesn't provide standarization over
      // error handling responses. For example: if you request
      // unexistent resources it will retrive an array with a
      // message inside.
      // Recline backends will return an object with an error.
      try {
        var ro = (typeof response === 'string') ? JSON.parse(response) : response;

        if(ro.error)  {
          showError(ro.error.message)
        } else if(ro instanceof Array) {
          showError(ro[0]);
        }
      } catch (error) {
        showError(response);
      }
    }

    // Displays an error.
    function showError (message) {
      $explorer.html('<div class="messages error">' + message + '</div>');
    }

    // Creates the embed code.
    function getEmbedCode (options){
      return function(state){
        var iframeOptions = _.clone(options);
        var iframeTmpl = _.template('<iframe width="<%= width %>" height="<%= height %>" src="<%= src %>" frameborder="0"></iframe>');
        var previewTmpl = _.template('<%= src %>');
        _.extend(iframeOptions, {src: iframeOptions.src + '#' + (state.serializedState || '')});
        var html = iframeTmpl(iframeOptions);
        $('.embed-code').text(html);
        var preview = previewTmpl(iframeOptions);
        $('.preview-code').text(preview);
      };
    }

    // Creates the preview url code.
    function getPreviewCode (options){
      return function(state){
        var previewOptions = _.clone(options);
        var previewTmpl = _.template('<%= src %>');
        _.extend(previewOptions, {src: previewOptions.src + '#' + (state.serializedState || '')});
        var html = previewTmpl(previewOptions);
        $('.preview-url').text(html);
      };
    }

    // Check if a chart has their axis inverted.
    function isInverted (){
      return dataExplorer.pageViews[1].view.state.attributes.graphType === 'bars';
    }

    // Computes the width of a chart.
    function computeWidth (plot, labels) {
      var biggerLabel = '';
      for( var i = 0; i < labels.length; i++){
        if(labels[i].length > biggerLabel.length && !_.isUndefined(labels[i])){
          biggerLabel = labels[i];
        }
      }
      var canvas = plot.getCanvas();
      var ctx = canvas.getContext('2d');
      ctx.font = 'sans-serif smaller';
      return ctx.measureText(biggerLabel).width;
    }

    // Resize a chart.
    function resize (plot) {
      var itemWidth = computeWidth(plot, _.pluck(plot.getXAxes()[0].ticks, 'label'));
      var graph = dataExplorer.pageViews[1];
      if(!isInverted() && $('#prevent-label-overlapping').is(':checked')){
        var canvasWidth = Math.min(itemWidth + LABEL_MARGIN, MAX_LABEL_WIDTH) * plot.getXAxes()[0].ticks.length;
        var canvasContainerWith = $('.panel.graph').parent().width();
        if(canvasWidth < canvasContainerWith){
            canvasWidth = canvasContainerWith;
        }
        $('.panel.graph').width(canvasWidth);
        $('.recline-flot').css({overflow:'auto'});
      }else{
        $('.recline-flot').css({overflow:'hidden'});
        $('.panel.graph').css({width: '100%'});
      }
      plot.resize();
      plot.setupGrid();
      plot.draw();
    }

    // Bind events after chart resizes.
    function bindEvents (plot, eventHolder) {
      var p = plot || dataExplorer.pageViews[1].view.plot;
      resize(p);
      setTimeout(addCheckbox, 0);
    }

    // Compute the chart offset to display ticks properly.
    function processOffset (dataset) {
      return function(plot, offset) {
        if(dataExplorer.pageViews[1].view.xvaluesAreIndex){
          var series = plot.getData();
          for (var i = 0; i < series.length; i++) {
            var numTicks = Math.min(dataset.records.length, 200);
            var ticks = [];
            for (var j = 0; j < dataset.records.length; j++) {
              ticks.push(parseInt(j, 10));
            }
            if(isInverted()){
              series[i].yaxis.options.ticks = ticks;
            }else{
              series[i].xaxis.options.ticks = ticks;
            }
          }
        }
      };
    }

    // Format ticks base on previews computations.
    function tickFormatter (dataset){
      return function (x) {
        x = parseInt(x, 10);
        try {
          if(isInverted()) return x;
          var field = dataExplorer.pageViews[1].view.state.get('group');
          var label = dataset.records.models[x].get(field) || '';
          if(!moment(String(label)).isValid() && !isNaN(parseInt(label, 10))){
            label = parseInt(label, 10) - 1;
          }
          return label;
        } catch(e) {
          return x;
        }
      };
    }

    // Add checkbox to control resize behavior.
    function addCheckbox () {
      $control = $('.form-stacked:visible').find('#prevent-label-overlapping');
      if(!$control.length){
        $form = $('.form-stacked');
        $checkboxDiv = $('<div class="checkbox"></div>').appendTo($form);
        $label = $('<label />', { 'for': 'prevent-label-overlapping', text: 'Resize graph to prevent label overlapping' }).appendTo($checkboxDiv);
        $label.prepend($('<input />', { type: 'checkbox', id: 'prevent-label-overlapping', value: '' }));
        $control = $('#prevent-label-overlapping');
        $control.on('change', function(){
          resize(dataExplorer.pageViews[1].view.plot);
        });
      }
    }

    // Init the multiview.
    function init () {
      if(fileSize < maxSizePreview || datastoreStatus) {
        dataset = new recline.Model.Dataset(getDatasetOptions());
        dataset.fetch().fail(showRequestError);
        views = createExplorer(dataset, state, dataExplorerSettings);
        views.forEach(function(view) { view.id === 'map' && view.view.redraw('refresh') });
      } else {
        showError('File was too large or unavailable for preview.');
      }
    }

})(jQuery);
