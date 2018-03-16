/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
  'use strict';

  my.multiBarHorizontalChart = recline.View.nvd3.Base.extend({
    initialize: function(options) {
      var self = this;
      self.graphType = 'multiBarHorizontalChart';
      recline.View.nvd3.Base.prototype.initialize.call(self, options);
      self.state.set('computeXLabels', false);
    },
    render: function(){
      var self = this;
      recline.View.nvd3.Base.prototype.render.call(self, {});
    },
    getDefaults: function(){
      return {
        options: {
          tooltips: true,
          reduceXTicks: false,
        }
      };
    },
      renderGoals: function(){
        var self = this;
        var goal = self.state.get('goal');
        nv.dispatch.on('render_end', null);
        if(this.canRenderGoal(goal)){
          nv.dispatch.on('render_end', function(){
            var scale = self.chart.yAxis.scale();
            var margin = self.chart.margin();
            var x = scale(goal.value) + margin.left;
            var y = margin.top;
            var height = jQuery('g').get(0).getBBox().height;
            var g = d3.select('svg').append('g');
            var labelX, labelY;

            if(goal.label) {
              labelX =  x + 5;
              labelY = y + 5;
              g.append('text')
              .text(goal.title || 'TARGET')
              .attr('x', labelX)
              .attr('y', labelY)
              .attr('fill', goal.color || 'red' )
              .style('font-size','10px')
              .style('font-weight','bold')
              .style('font-style','italic');
            }

            g.append('line')
            .attr('class', 'goal')
            .attr('y1', y)
            .attr('x1', x)
            .attr('y2', height - 5)
            .attr('x2', x)
            .attr('stroke-width', 1)
            .attr('stroke', goal.color || 'red')
            .style('stroke-dasharray', ('3, 3'));
          });
        }
      }
  });

})(jQuery, recline.View.nvd3);
