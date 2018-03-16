/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.multiBarHorizontalChartControls = recline.View.nvd3.BaseControl.extend({
    templateGoal:
    '<div class="form-group">' +
      '<div class="row">' +
        '<div class="col-md-12 col-sm-12">' +
          '<label>Goal</label>' +
          '</div>' +
        '</div>' +
      '<div class="row">' +
        '<div class="col-md-3 col-sm-3">' +
          '<input id="control-chart-goal-value" type="text" class="form-control" aria-label="Goal value" placeholder="e.g.: 50" value="{{goal.value}}">' +
          '</div>' +
        '<div class="col-md-3 col-sm-3">' +
          '<input id="control-chart-goal-color" type="text" class="form-control" aria-label="Goal color" placeholder="e.g.: red" value="{{goal.color}}">' +
          '</div>' +
        '<div class="col-md-6 col-sm-3">' +
          '<div class="form-group checkbox checkbox-without-margin">' +
            '<label for="control-chart-goal-label">' +
              '<input type="checkbox" id="control-chart-goal-label" value="{{goal.label}}" {{#goal.label}}checked{{/goal.label}}/> Show label' +
              '</label>' +
            '</div>' +
          '</div>' +
        '</div>' +
      '</div>',
    customOptions: '',
    events: function(){
      return _.extend({}, recline.View.nvd3.BaseControl.prototype.events, {});
    }
  });

})(jQuery, recline.View.nvd3);
