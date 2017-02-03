(function($) {
  Drupal.behaviors.visualization_choropleth = {
    attach: function(context) {
      var settings = Drupal.settings.visualization_choropleth;
      console.log(settings);
      var resources = settings.resources;
      var geojson = settings.geojson;
      var data_column = settings.data_column ? settings.data_column : [];
      var map_column = settings.map_column;
      var geojson_key = settings.geojson_key;
      var geojson_label = settings.geojson_label;
      var color_scale = settings.colors || ['#FFEDA0', '#FEB24C', '#E31A1C', '#800026'];
      var breakpoints = settings.breakpoints ? settings.breakpoints : [];
      var view = null;
      var container = $('#visualization .data-view-container');
      var sidebar = $('#visualization .data-view-sidebar');
      var pageSize = 100;

      if (resources.length === 1) {
        // Process resource and create recline model instance.
        resource = resources[0];
        if (resource.datastoreStatus === 2) {
          resource = resource.dataset.replace(/(\r\n|\n|\r)/gm,"\n");
          resource = new recline.Model.Dataset({
            records: recline.Backend.CSV.parse(resource, resource.delimiter),
          });
          initView(resource);
        }
        else {
          var drupal_base_path = Drupal.settings.basePath;
          var DKAN_API = drupal_base_path + 'api/action/datastore/search.json';
          var url = window.location.origin + DKAN_API + '?resource_id=' + resource.resource_id;

          var records = [];
          var numReq;
          var currReq = 0;

          var progressInfo = $('<div class="progress-info"></p>');
          progressInfo.hide();

          // Entry point.
          initData();
        }
      }
      else {
        for (var k = 0; k < resources.length; k++){
          var dataset = {};
          resources[k].baseColor = color_scale;
          if (breakpoints.length > 0) {
            resources[k].breakpoints = breakpoints;
          }
          resources[k].selectable_fields = data_column;
          resources[k].fieldToDisplay = data_column.length > 0 ? data_column[0] : '';
          resources[k].dataset = new recline.Model.Dataset({
            records: recline.Backend.CSV.parse(
              resources[k].dataset.replace(/(\r\n|\n|\r)/gm,"\n"),
              resources[k].delimiter
            ),
          });
        }
        view = new recline.View.MultiDatasetChoroplethMap({
          polygons: geojson,
          resources: resources,
          selectable_fields: data_column,
          label_to_map: map_column,
          base_color: color_scale,
        });
        container.append(view.el);
        sidebar.append(view.elSidebar);
        view.render();
      }
      // Transform results into records.
      function processData(data, first) {
        var fields = _.pluck(data.result.fields, 'id');
        var records = _.map(data.result.records, function(r){
          return _.values(r);
        });
        first && records.unshift(fields);
        return records;
      }
      // Request more data.
      function requestData(pageUrl){
        return jQuery.get(pageUrl).done(function(data){
          showProgressInfo();
          records = records.concat(processData(data));
          var dataset = new recline.Model.Dataset({records:records});
          view.model = dataset;
          view.redraw();
          return records;
        });
      }
      // Initialize ChoroplethMap.
      function initView(dataset){
        view = new recline.View.ChoroplethMap({
          polygons: geojson,
          model: dataset,
          map_column: map_column,
          selectable_fields: data_column,
          breakpoints: breakpoints,
          geojson_key: geojson_key,
          geojson_label: geojson_label,
          base_color: color_scale,
          avg: resources[0].avg,
          unitOfMeasure: resources[0].unitOfMeasure,
        });
        // Attach html and render the Recline view.
        container.append(view.el);
        sidebar.append(view.elSidebar);
        view.render();
        $(window).on('resize', function(){
          view.$el.find('.recline-map .map').height($(window).height() - 10);
        });
        view.$el.find('.recline-map .map').height($(window).height() - 10);
      }
      // Get page url from iteration number.
      function getPageURL(i, resource) {
        var drupal_base_path = Drupal.settings.basePath;
        var DKAN_API = drupal_base_path + 'api/action/datastore/search.json';
        var url = window.location.origin + DKAN_API + '?resource_id=' + resource.resource_id;
        var offset = i * pageSize;
        var limit = pageSize;
        return url + '&offset=' + offset + '&limit=' + limit;
      }

      // Get number of requests
      function getRequestNumber(total, pageSize){
        return Math.floor(total / pageSize);
      }
      // Renders progress request info.
      function showProgressInfo(){
        if (currReq === 0) {
          progressInfo.show();
        }
        currReq++;
        if (currReq === numReq) {
          progressInfo.hide();
        }
        progressInfo.html('Downloading ' + currReq + ' of ' + numReq );
      }
      // Make first request and populate initial data.
      function initData(){
        container.prepend(progressInfo);
        jQuery.get(getPageURL(0, resource)).done(function(data) {
          records = processData(data, true);
          initView(new recline.Model.Dataset({records:records}));
          numReq = getRequestNumber(data.result.total, pageSize);
          for (var i = 1; i <= numReq; i++) {
            requestData(getPageURL(i, resource));
          }
        });
      }
    },
  };
})(jQuery);
