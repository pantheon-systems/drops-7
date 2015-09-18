/*jshint multistr:true */
 /*jshint -W030 */
this.recline = this.recline || {};
this.recline.View = this.recline.View || {};
this.recline.View.nvd3 = this.recline.View.nvd3 || {};

;(function ($, my) {
  'use strict';

  var DEFAULT_CHART_WIDTH = 640;
  var DEFAULT_CHART_HEIGHT = 480;
  var MAX_ROW_NUM = 1000;

  function makeId(prefix) {
      prefix = prefix || '';
      return prefix + (Math.random() * 1e16).toFixed(0);
  }

  my.Base = Backbone.View.extend({
      template:'<div class="recline-graph recline-nvd3 row">' +
                  '{{data}}' +
                  '<div class="{{columnClass}} {{viewId}} recline-nvd3"style="display: block;">' +
                    '<div id="{{viewId}}" class="recline-nvd3">' +
                        '<svg version="1.1" xmlns="http://www.w3.org/2000/svg" ' +
                        ' height="{{height}}" width="{{width}}">' +
                        '</svg>' +
                    '</div>' +
                  '</div>' +
                '</div> ',

      CLEANUP_CHARS: '%$¥€',
      initialize: function(options) {
        var self = this;

        self.$el = $(self.el);
        self.options = _.defaults(options || {}, self.options);

        var stateData = _.merge({
            width: 640,
            height: 480,
            group: false,
          },
          self.getDefaults(),
          self.options.state.toJSON()
        );
        console.log(options);
        self.graphType = self.graphType || 'multiBarChart';
        self.uuid = makeId('nvd3chart_');
        self.state = self.options.state;
        self.model = self.options.model;
        self.state.set(stateData);
        self.chartMap = d3.map();
        self.render();
        self.listenTo(self.state, 'change', self.render.bind(self));
        self.listenTo(self.model.records, 'add remove reset change', self.lightUpdate.bind(self));
      },
      getLayoutParams: function(){
        var self = this;
        var layout = {
          columnClass: 'col-md-12',
          width: self.state.get('width') || self.$el.innerWidth() || DEFAULT_CHART_WIDTH,
          height: self.state.get('height') || DEFAULT_CHART_HEIGHT
        };
        return layout;
      },
      render: function(){
        var self = this;
        var tmplData;
        var htmls;
        var layout;

        layout = self.getLayoutParams();
        tmplData = self.model.toTemplateJSON();
        tmplData.viewId = self.uuid;

        _.extend(tmplData, layout);

        htmls = Mustache.render(self.template, tmplData);
        self.$el.html(htmls);
        self.$graph = self.$el.find('.panel.' + tmplData.viewId);
        self.trigger('chart:endDrawing');

        // Infering x value type
        var computeXLabels = self.needForceX(self.model.records, self.graphType);
        self.state.set('computeXLabels', computeXLabels, {silent:true});

        // If number of rows is too big then try to group by x.
        self.state.set('group', self.model.records.length > MAX_ROW_NUM || self.state.get('group', {silent:true}));

        self.series = self.createSeries(self.model.records);
        nv.addGraph(function() {
          self.chart = self.createGraph(self.graphType);
          // Give a chance to alter the chart before it is rendered.
          self.alterChart && self.alterChart(self.chart);

          if(self.chart.xAxis && self.chart.xAxis.tickFormat)
            self.chart.xAxis.tickFormat(self.xFormatter);
          if(self.chart.x2Axis)
            self.chart.x2Axis.tickFormat(self.xFormatter);

          d3.select('#' + self.uuid + ' svg')
            .datum(self.series)
            .transition()
            .duration(self.state.get('transitionTime') || 500)
            .call(self.chart);

          // Hack to reduce ticks even if the chart has not that option.
          if(self.graphType === 'discreteBarChart' && self.state.get('options') && self.state.get('options').reduceXTicks){
            self.reduceXTicks();
          }

          nv.utils.windowResize(self.updateChart.bind(self));
          return self.chart;
        });
        return self;
      },
      lightUpdate: function(){
        var self = this;
        self.series = self.createSeries(self.model.records);
        self.setOptions(self.chart, self.state.get('options'));
        setTimeout(function(){
          d3.select('#' + self.uuid + ' svg')
            .datum(self.series)
            .transition()
            .duration(500)
            .call(self.chart);
        }, 0);

      },
      updateChart: function(){
        var self = this;
        d3.select('#' + self.uuid + ' svg')
          .transition()
          .duration(self.state.get('transitionTime') || 500)
          .call(self.chart);
      },
      reduceXTicks: function(){
        var self = this;
        var layout = self.getLayoutParams(self.state.get('mode'));
        d3.select('.nv-x.nv-axis > g').selectAll('g')
          .filter(function(d, i) {
              return i % Math.ceil(self.model.records.length / (layout.width / 100)) !== 0;
          })
          .selectAll('text, line')
          .style('opacity', 0);
      },
      createSeries: function(records){
        var self = this;
        var series;
        var fieldType;
        var xDataType;

        // Return no data when x and y are no set.
        if(!self.state.get('xfield') || !self.getSeries()) return [];

        records = records.toJSON();

        fieldType = _.compose(_.inferType,_.iteratee(self.state.get('xfield')));

        if(!self.state.get('xDataType') || self.state.get('xDataType') === 'Auto'){
          xDataType =  fieldType(_.last(records) || []);
        } else {
          xDataType = self.state.get('xDataType');
        }

        self.xFormatter = self.getFormatter(xDataType, self.state.get('xFormat'));

        series = _.map(self.getSeries(), function(serie){
          var data = {};
          data.key = serie;

          // Group by xfield and acum all the series fields.
          records = (self.state.get('group'))?
            _.reportBy(records, self.state.get('xfield'), self.state.get('seriesFields'))
            : records;

          // Sorting
          records = _.sortBy(records, self.getSort(self.state.get('sort')));
          data.values = _.map(records, function(record, index){
            var y = self.cleanupY(self.y(record, serie));
            y = _.cast(y, _.inferType(y));

            if(self.state.get('computeXLabels')){
              self.chartMap.set(index, self.x(record, self.state.get('xfield')));
              return {y: y, x: index, label: self.x(record, self.state.get('xfield'))};
            } else {
              return {
                y: y,
                x: _.cast(self.x(record, self.state.get('xfield')), xDataType)
              };
            }
          });
          return data;
        });
        return series;
      },
      cleanupY: function(y){
        var self = this;
        if (typeof y === 'string') {
          return y.replace(new RegExp('[' + self.CLEANUP_CHARS + ']'), '');
        }
        return y;
      },
      getSort: function(sort){
        if(!sort || sort === 'default') return _.identity;
        return sort;
      },
      needForceX: function(records, graphType){
       var self = this;
       var xfield = self.state.get('xfield');
       records = records.toJSON();
       return _.some(records, function(record){
         return _.inferType(record[xfield]) === 'String';
       }) && graphType !== 'discreteBarChart' && graphType !== 'multiBarChart';
      },
      getFormatter: function(type, format){
        var self = this;
        if(self.state.get('computeXLabels')) return self.chartMap.get.bind(self.chartMap);

        var formatter = {
          'String': _.identity,
          'Date': _.compose(d3.time.format(format || '%x'),_.instantiate(Date)),
          'Number': d3.format(format || '.02f')
        };

        return formatter[type];
      },
      setOptions: function (chart, options) {
        var self = this;
        for(var optionName in options){
          var optionValue = options[optionName];
          if(optionName === 'margin'){
            chart.margin(optionValue);
          }
          if(chart && _.isObject(optionValue) && !_.isArray(optionValue)){
            self.setOptions(chart[optionName], optionValue);
          // if value is a valid function in the chart then we call it.
          } else if(chart && _.isFunction(chart[optionName])){
            chart[optionName](optionValue);
          }
        }
      },
      createGraph: function(graphType){
        var self = this;
        var chart = nv.models[graphType]();
        // Set each graph option recursively.
        self.setOptions(chart, self.state.get('options'));
        return chart;
      },
      getDefaults: function(){
        return {};
      },
      getState: function(){
        var self = this;
        return self.state.attributes;
      },
      getSeries: function(){
        var self = this;
        return self.state.get('seriesFields');
      },
      x: function(record, xfield){
        return record[xfield];
      },
      y: function(record, serie){
        return record[serie];
      }
  });

})(jQuery, recline.View.nvd3);
