/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
  'use strict';

  my.stackedAreaChart = recline.View.nvd3.Base.extend({
    initialize: function(options) {
      var self = this;
      self.graphType = 'stackedAreaChart';
      recline.View.nvd3.Base.prototype.initialize.call(self, options);
      self.state.set('computeXLabels', true);
    },
    render: function(){
      var self = this;
      recline.View.nvd3.Base.prototype.render.call(self, {});
    },
    getDefaults: function(){
      return {
        options:{
          useInteractiveGuideline: true,
          tooltips: true,
          xAxis:{
            tickFormat: function(d) {
              return d3.time.format('%Y')(new Date(d));
            }
          }
        }
      };
    }
  });

})(jQuery, recline.View.nvd3);