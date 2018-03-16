/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
  'use strict';

  my.linePlusBarChart = recline.View.nvd3.Base.extend({
    initialize: function(options) {
      var self = this;
      self.graphType = 'linePlusBarChart';
      recline.View.nvd3.Base.prototype.initialize.call(self, options);
      self.state.set('computeXLabels', true);
    },
    render: function(){
      var self = this;
      recline.View.nvd3.Base.prototype.render.call(self, {});
    },
    alterChart: function(chart) {
			var self = this;
      chart
        .x(function(d,i) { 
					return i;//d.x; 
        })
        .y(function(d,i) {
          return d.y; 
        });
      chart.options({focusEnable: false});

			// Determine which field should be rendered as bar
      var barIndex = 0;
      var field = self.state.get('lpbBarChartField') || $('#control-lpb-barchart-field').val();
      self.series.forEach(function (row, i) {
       if (row.originalKey) {
         row.key = row.originalKey;
         delete row.originalKey;
       }
       delete row.bar; // reset bar value for initialization bug
       if (row.key === field || row.originalKey === field) {
         barIndex = i;
       }
      });
      self.series[barIndex].bar = true;
    },
		getDefaults: function () {
      return {
        options: {
          tooltips: true,
          showLegend: true,
        }
      }
    }

  });

})(jQuery, recline.View.nvd3);
