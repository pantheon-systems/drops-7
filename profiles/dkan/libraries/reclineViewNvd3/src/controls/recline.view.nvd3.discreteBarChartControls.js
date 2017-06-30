/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.discreteBarChartControls = recline.View.nvd3.BaseControl.extend({
    customOptions: '<fieldset>' +
                '<legend>Extra</legend>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-stagger-labels">' +
                    '<input type="checkbox" id="control-chart-stagger-labels" {{#options.staggerLabels}}checked{{/options.staggerLabels}}/> Stagger Labels' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" ' +
                  'title="Stagger Labels" data-content="Alternate the vertical position of labels if the spacing is tight along the x-axis."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-values">' +
                    '<input type="checkbox" id="control-chart-show-values" {{#options.showValues}}checked{{/options.showValues}}/> Show Values' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" ' +
                  'title="Show Values" data-content="Display the data values on the chart."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +
              '</fieldset>',
    events: function(){
      return _.extend({}, recline.View.nvd3.BaseControl.prototype.events, {
        'change input[type="checkbox"]': 'update'
      });
    },
    getUIState:function(){
      var self = this;
      var computedState = recline.View.nvd3.BaseControl.prototype.getUIState.call(this);
      computedState = _.merge({}, computedState, {options: {}});
      computedState.options.showValues = self.$('#control-chart-show-values').is(':checked');
      computedState.options.staggerLabels = self.$('#control-chart-stagger-labels').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);