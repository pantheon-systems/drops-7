/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.multiBarChartControls = recline.View.nvd3.BaseControl.extend({
    customOptions: '<fieldset>' +
                '<legend>Extra</legend>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-stagger-labels">' +
                    '<input type="checkbox" id="control-chart-stagger-labels" {{#options.staggerLabels}}checked{{/options.staggerLabels}}/> Stagger Labels' +
                  '</label>' +
                '</div>' +
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-stacked">' +
                    '<input type="checkbox" id="control-chart-stacked" {{#options.stacked}}checked{{/options.stacked}}/> Stacked' +
                  '</label>' +
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
      computedState.options.staggerLabels = self.$('#control-chart-stagger-labels').is(':checked');
      computedState.options.stacked = self.$('#control-chart-stacked').is(':checked');
      return computedState;
    }
  });

})(jQuery, recline.View.nvd3);