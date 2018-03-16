/*jshint multistr:true */
 /*jshint -W030 */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};
this.recline.View.nvd3 = this.recline.View.nvd3 || {};
var globalchart;
var chartAxes = ['x','y','y1','y2'];
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
                        'style="height:{{height}}px;width: 100%;">' +
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
            group: false
          },
          self.getDefaults(),
          self.options.state.toJSON()
        );
        self.graphType = self.graphType || 'multiBarChart';
        self.uuid = makeId('nvd3chart_');
        self.state = self.options.state;
        self.model = self.options.model;
        self.state.set(stateData);
        self.chartMap = d3.map();
        self.listenTo(self.model.records, 'add remove reset change', self.lightUpdate.bind(self));
        globalchart = self;
      },
      getLayoutParams: function(){
        var self = this;
        var layout = {
          columnClass: 'col-md-12',
          width: self.state.get('width') || self.$el.innerWidth() || DEFAULT_CHART_WIDTH,
          height: self.state.get('chartHeight') || DEFAULT_CHART_HEIGHT
        };
        return layout;
      },
      render: function(){
        var self = this;
        var tmplData;
        var htmls;
        var layout;
        var xFormat;
        var yFormat, y1Format, y2Format;

        // Reattach listener
        self.listenToOnce(self.state, 'change', self.render.bind(self));

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

          // Format axes
          chartAxes.forEach(function (axis) {
            if (self.chart[axis+'Axis']) {
              var format = self.state.get(axis+'Format') || {type: 'String', format: ''};
              var formatter = self.getFormatter(format.type, format.format, axis);
              self.calcTickValues(axis, self.chart[axis+'Axis'], self.state.get(axis+'Values'), self.state.get(axis+'ValuesStep'));
              self.chart[axis+'Axis'].tickFormat(formatter);
            }
          })

          d3.select('#' + self.uuid + ' svg')
            .datum(self.series)
            .transition()
            .duration(self.state.get('transitionTime') || 500)
            .call(self.chart);
          self.renderGoals();
          // Hack to reduce ticks even if the chart has not that option.
          if(self.graphType === 'discreteBarChart' && self.state.get('options') && self.state.get('options').reduceXTicks){
            self.reduceXTicks();
          }

          // Ensure backward compatibility
          // 1.8.1 version
          if (typeof self.chart.tooltips === 'function') {
            self.chart.tooltips(self.state.get('options').tooltips);
          // 1.8.2 version
          } else {
            self.chart.tooltip.enabled(self.state.get('options').tooltips);
          }

          nv.utils.windowResize(self.updateChart.bind(self));
          return self.chart;
        });
        return self;
      },

      calcTickValues: function(axisName, axis, range, step){
        var self = this;
        var ordinalScaled = ['multiBarChart', 'discreteBarChart', 'linePlusBarChart'];
        var tickValues;

        step = step || 1;

        // check for old formatted range values
        if (this.isOldRangeType(range)) {
          range = this.convertRange(range);
        }

        // If this is linePlusBar use chart.bars.forceY & chart.lines.forceY
        if (range && self.rangesValid(range)) {
          range[0] = parseInt(range[0]);
          range[1] = parseInt(range[1]);
          tickValues = d3.range(range[0], range[1], step);

          if (self.graphType === 'linePlusBarChart' && axisName === 'y1') {
            self.chart.bars.yDomain([range[0], range[1]], step);
          } else if (self.graphType === 'linePlusBarChart' && axisName === 'y2') {
            self.chart.lines.yDomain([range[0], range[1]], step);
          } else if (!_.inArray(ordinalScaled, self.graphType) || axisName === 'y') {
            self.chart[axisName + 'Domain']([range[0], range[1]]);
          } else {
            self.chart[axisName + 'Domain'](d3.range(range[0], range[1], step));
          }
        }
        axis.tickValues(tickValues);
      },
      // check for old range format
      isOldRangeType: function (range) {
        return (range && range.indexOf('-') !== -1);
      },
      // convert old range format to new array type
      convertRange: function (range) {
       var temp = range.replace(' ', '').split('-');
       for (var i = 0; i < range.length; i++) {
         if (temp[i] === '' && i < temp.length) {
           temp[i + 1] = '-' + temp[i + 1];
         }
       }
       var newRange = [];
       for (i = 0; i < temp.length; i++) {
         if (temp[i] !== '') {
           newRange.push(parseFloat(temp[i]));
         }
       }
       newRange = newRange.sort(function(a, b){return a-b;});
       return newRange;
      },
      rangesValid: function (range){
        var valid = true;
        if (!range || range.length !== 2) valid = false;
        _.each(range, function (bound) {
          if (!bound || isNaN(parseInt(bound))) valid = false;
        });
        if (valid) {
          if (parseInt(range[0]) >= parseInt(range[1])) {
            valid = false;
          }
        }
        return valid;
      },
      lightUpdate: function(){
        var self = this;
        if(self.chart){
          self.series = self.createSeries(self.model.records);
          self.setOptions(self.chart, self.state.get('options'));
          setTimeout(function(){
            d3.select('#' + self.uuid + ' svg')
              .datum(self.series)
              .transition()
              .duration(500)
              .call(self.chart);
          }, 0);
        }
      },
      canRenderGoal: function(goal){
        return !d3.select('svg').empty() &&
          d3.select('svg .goal').empty() && goal &&
          goal.value && !isNaN(goal.value) && this.chart.yAxis;
      },
      renderGoals: function(){
        var self = this;
        var goal = self.state.get('goal');
        nv.dispatch.on('render_end', null);
        if(this.canRenderGoal(goal)){
          nv.dispatch.on('render_end', function(){
            var yScale = self.chart.yAxis.scale();
            var margin = self.chart.margin();
            var y = yScale(goal.value) + margin.top;
            var x = margin.left;
            var xWidth = (d3.select('svg').size())? parseInt(d3.select('svg').style('width')) - 10 : 0;
            var g = d3.select('svg').append('g');
            var labelX, labelY;

            if(goal.label) {
              if (goal.outside) {
                labelX =  x - 50;
                labelY = y + 3;
              } else {
                labelX =  x + 5;
                labelY = y - 6;
              }
              g.append('text')
                .text('TARGET')
                .attr('x', labelX)
                .attr('y', labelY)
                .attr('fill', goal.color || 'red' )
                .style('font-size','10px')
                .style('font-weight','bold')
                .style('font-style','italic');
            }

            g.append('line')
              .attr('class', 'goal')
              .attr('x1', x)
              .attr('y1', y)
              .attr('x2', xWidth)
              .attr('y2', y)
              .attr('stroke-width', 1)
              .attr('stroke', goal.color || 'red')
              .style('stroke-dasharray', ('3, 3'));
          });
        }
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

        series = _.map(self.getSeries(), function(serie){
          var data = {};
          data.key = serie;

          // Group by xfield and acum all the series fields.
          var rc = (self.state.get('group'))?
            _.reportBy(records, self.state.get('xfield'), self.state.get('seriesFields'))
            : records;

          // Sorting
          rc = _.sortBy(rc, self.getSort(self.state.get('sort')));

          rc = _.reduce(rc, function(memo, record){
            var y = self.cleanupY(self.y(record, serie));
            if(y || y === 0 || self.graphType === 'stackedAreaChart') {
              memo.push(record);
            } else if(self.state.get('options').stacked) {
              record[serie] = 0;
              memo.push(record);
            }
            return memo;
          }, []);

          data.values = _.map(rc, function(record, index){
            // Cleanup 'y' value removing special characters.
            var y = self.cleanupY(self.y(record, serie));

            // Get specified type for 'y' values.
            if(self.state.get('yDataType') && self.state.get('yDataType') === 'Number'){
              // If 'Number' then parse it.
              y = numeral(y).value();
            } else {
              // If any other type, then infer it and cast the value.
              y = _.cast(y, _.inferType(y));
            }

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
      getFormatter: function(type, format, axisName){
        var self = this;
        axisName = axisName || 'x';

        if(self.state.get('computeXLabels') && axisName === 'x')
          return self.chartMap.get.bind(self.chartMap);

        var formatter = {
          'String': _.identity,
          'Date': _.compose(d3.time.format(format || '%x'),_.instantiate(Date)),
          'Number': d3.format(format || '.02f'),
          'Percentage': function (n) { return d3.format(format || '.02f')(n*100) + '%'; },
          'PercentageA': function (n) { return d3.format(format || '.02f')(n) + '%'; },
        };
        return formatter[type];
      },
      setOptions: function(chart, options) {
        var self = this;
        for(var optionName in options){
          var optionValue = options[optionName];
          if(optionName === 'margin'){
            // Force zero since auto-legend adjustment fails with other values.
            // REF novus/nvd3#515
            optionValue.top = 0;
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
      },
      destroy: function(){
        this.stopListening();
      },
      alterChart: function () {
        // implement
      }
  });

})(jQuery, recline.View.nvd3);
