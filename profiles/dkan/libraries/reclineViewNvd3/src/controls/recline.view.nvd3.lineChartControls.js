/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.lineChartControls = recline.View.nvd3.BaseControl.extend({
    template: '<fieldset>' +
                '<legend>Extra</legend>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-datapoints">' +
                  '<input type="checkbox" id="control-chart-datapoints" {{#options.datapoints}}checked{{/options.datapoints}}/> Show data points' +
                  '</label>' +
                '</div>' +
              '</fieldset>',
    events: {
      'change input[type="checkbox"]': 'update',
    },
    getUIState:function(){
      var self = this;
      var computedState = {options: {}};
      computedState.options.datapoints = self.$('#control-chart-datapoints').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);
