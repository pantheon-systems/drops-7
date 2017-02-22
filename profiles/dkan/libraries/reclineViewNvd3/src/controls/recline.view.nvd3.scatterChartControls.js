/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

  my.scatterChartControls = recline.View.nvd3.BaseControl.extend({
    customOptions: '',
    events: function(){
      return _.extend({}, recline.View.nvd3.BaseControl.prototype.events);
    }
  });

})(jQuery, recline.View.nvd3);

