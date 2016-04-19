/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
  'use strict';

  my.multiBarChart = recline.View.nvd3.Base.extend({
    initialize: function(options) {
      var self = this;
      self.graphType = 'multiBarChart';
      recline.View.nvd3.Base.prototype.initialize.call(self, options);
    },
    render: function(){
      var self = this;
      recline.View.nvd3.Base.prototype.render.call(self, {});
      nv.dispatch.on('render_end', function(){
        d3.selectAll('.nv-controlsWrap .nv-legend .nv-series').on('click', function(e){
          var stacked = (e.key === 'Stacked') ? true : false;
          self.state.get('options').stacked = stacked;
          self.lightUpdate();
          self.chart.stacked(stacked);
        });
      });
    },
    getDefaults: function(){
      return {
        computeXLabels: false,
        options:{
          reduceXTicks: true,
          tooltips: true
        }
      };
    }
  });

})(jQuery, recline.View.nvd3);