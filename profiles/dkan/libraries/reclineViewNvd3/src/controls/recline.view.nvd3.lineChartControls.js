/*jshint multistr:true */
this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.lineChartControls = recline.View.nvd3.BaseControl.extend({
    customOptions: '<fieldset>' +
                '<legend>Extra</legend>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-datapoints">' +
                  '<input type="checkbox" id="control-chart-datapoints" {{#options.datapoints}}checked{{/options.datapoints}}/> Show data points' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" ' +
                  'title="Show data points" data-content="This will display the data points along each line."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
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
      computedState.options.datapoints = self.$('#control-chart-datapoints').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);
