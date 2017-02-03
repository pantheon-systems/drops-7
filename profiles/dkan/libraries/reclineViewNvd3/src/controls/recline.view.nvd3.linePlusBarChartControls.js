/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.linePlusBarChartControls = recline.View.nvd3.BaseControl.extend({
    template: '',
		getUIState:function(){
      var self = this;
      var computedState = {options: {}};
      computedState.options.showValues = self.$('#control-chart-show-values').is(':checked');
      computedState.options.staggerLabels = self.$('#control-chart-stagger-labels').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);
