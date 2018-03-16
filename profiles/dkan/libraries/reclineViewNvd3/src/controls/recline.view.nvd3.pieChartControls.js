/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.pieChartControls = recline.View.nvd3.BaseControl.extend({
    customOptions: '<fieldset>' +
                '<legend>Extra</legend>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-donut">' +
                  '<input type="checkbox" id="control-chart-donut" {{#options.donut}}checked{{/options.donut}}/> Donut' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" ' +
                  'title="Donut" data-content="Turn your pie chart into a donut chart."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
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
      computedState.options.donut = self.$('#control-chart-donut').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);

