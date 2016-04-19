/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.pieChartControls = recline.View.nvd3.BaseControl.extend({
    template: '<fieldset>' +
                '<legend>Extra</legend>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-donut">' +
                  '<input type="checkbox" id="control-chart-donut" {{#options.donut}}checked{{/options.donut}}/> Donut' +
                  '</label>' +
                '</div>' +
              '</fieldset>',
    events: {
      'change input[type="checkbox"]': 'update',
    },
    getUIState:function(){
      var self = this;
      var computedState = {options: {}};
      computedState.options.donut = self.$('#control-chart-donut').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);

