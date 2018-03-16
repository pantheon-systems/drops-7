/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.cumulativeLineChart = recline.View.nvd3.Base.extend({
    initialize: function(options) {
      var self = this;
      self.graphType = 'cumulativeLineChart';
      recline.View.nvd3.Base.prototype.initialize.call(self, options);
    },
    render: function(){
      var self = this;
      recline.View.nvd3.Base.prototype.render.call(self, {});
    },
    getDefaults: function(){
      return {
        useInteractiveGuideline: true,
        tooltips: true
      };
    }
  });

})(jQuery, recline.View.nvd3);