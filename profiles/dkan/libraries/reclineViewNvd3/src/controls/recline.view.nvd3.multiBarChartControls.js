/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.multiBarChartControls = recline.View.nvd3.BaseControl.extend({
    template: '<div class="form-group checkbox">' +
                '<label for="control-chart-stagger-labels">' +
                '<input type="checkbox" id="control-chart-stagger-labels" {{#options.staggerLabels}}checked{{/options.staggerLabels}}/> Stagger Labels' +
                '</label>' +
              '</div>',
    events: {
      'change input[type="checkbox"]': 'update',
    },
    getUIState:function(){
      var self = this;
      var computedState = {options: {}};
      computedState.options.staggerLabels = self.$('#control-chart-stagger-labels').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);