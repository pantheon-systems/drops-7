/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.linePlusBarChartControls = recline.View.nvd3.BaseControl.extend({
    customOptions: '',
    linePlusBarXSelect: function () {
      var self = this;
      var markup = '<fieldset><legend>Bar Chart Series</legend>' +
                   '<p>Select which series should be represented as a bar chart</p>' +
                   '<select id="control-lpb-barchart-field">';
      this.state.get('seriesFields').forEach(function (field) {
        markup += '<option value="' + field + '">' + field + '</option>';
      });
      markup += '</select></fieldset>';
      return markup;
    },
    composeTemplate: function() {
      var template = '';
      template += this.templateTop;
      template += this.templateGeneral;
      template += this.templateXFormat;
      template += this.templateY1Format
      template += this.templateY2Format;
      template += this.linePlusBarXSelect();
      template += this.customOptions ? this.customOptions : '';
      return template;
    },
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
