/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
  'use strict';

  my.lineChart = recline.View.nvd3.Base.extend({
    initialize: function(options) {
      var self = this;
      self.graphType = 'lineChart';
      recline.View.nvd3.Base.prototype.initialize.call(self, options);
      self.state.set('computeXLabels', true);
    },
    render: function(){
      var self = this;
      recline.View.nvd3.Base.prototype.render.call(self, {});
      var options = self.state.get('options');
      if (options.datapoints) {
        $('.recline-graph').addClass('recline-show-datapoints');
      }
    },
    getDefaults: function(){
      var self = this;
      return {
        options: {
          useInteractiveGuideline: false,
          tooltips: true,
          xAxis:{
            tickFormat: function(id) {
              return (self.chartMap) ? self.chartMap.get(id) : id;
            }
          }
        }
      };
    }
  });

})(jQuery, recline.View.nvd3);
