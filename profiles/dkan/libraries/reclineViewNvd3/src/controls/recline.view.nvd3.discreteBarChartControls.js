/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.discreteBarChartControls = recline.View.nvd3.BaseControl.extend({
    template: '<div class="form-group checkbox">' +
                '<label for="control-chart-stagger-labels">' +
                  '<input type="checkbox" id="control-chart-stagger-labels" {{#options.staggerLabels}}checked{{/options.staggerLabels}}/> Stagger Labels' +
                '</label>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-show-values">' +
                  '<input type="checkbox" id="control-chart-show-values" {{#options.showValues}}checked{{/options.showValues}}/> Show Values' +
                '</label>' +
              '</div>',
    events: {
      'change input[type="checkbox"]': 'update',
    },
    getUIState:function(){
      var self = this;
      var computedState = {options: {}};
      computedState.options.showValues = self.$('#control-chart-show-values').is(':checked');
      computedState.options.staggerLabels = self.$('#control-chart-stagger-labels').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);