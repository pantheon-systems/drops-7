/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';
my.BaseControl = Backbone.View.extend({
  template: '<div id="control-chart-container">' +
              '<div class="form-group">' +
                '<label for="control-chart-x-format">X-Format</label>' +
                '<input value="{{xFormat}}" type="text" id="control-chart-x-format" class="form-control" placeholder="e.g: %Y"/>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-label-x-rotation">Label X Rotation</label>' +
                '<input value="{{options.xAxis.rotateLabels}}" type="text" id="control-chart-label-x-rotation" class="form-control" placeholder="e.g: -45"/>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-transition-time">Transition Time (milliseconds)</label>' +
                '<input value="{{transitionTime}}" type="text" id="control-chart-transition-time" class="form-control" placeholder="e.g: 2000"/>' +
              '</div>' +
              '<div class="form-group">' +
                  '<label for="control-chart-color">Color</label>' +
                  '<input class="form-control" type="text" id="control-chart-color" value="{{options.color}}" placeholder="e.g: #FF0000,green,blue,#00FF00"/>' +
              '</div>' +
              '<div class="form-group">' +
                  '<label for="control-chart-x-axis-label">X Axis Label</label>' +
                  '<input class="form-control" type="text" id="control-chart-x-axis-label" value="{{options.xAxis.axisLabel}}"/>' +
              '</div>' +
              '<div class="form-group">' +
                  '<label for="control-chart-x-axis-label">Y Axis Label</label>' +
                  '<input class="form-control" type="text" id="control-chart-y-axis-label" value="{{options.yAxis.axisLabel}}"/>' +
              '</div>' +
              '<div class="form-group">' +
                  '<label for="control-chart-y-axis-label-distance">Y Axis Label Distance</label>' +
                  '<input class="form-control" type="text" id="control-chart-y-axis-label-distance" value="{{options.yAxis.axisLabelDistance}}"/>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-sort">Sort</label>' +
                '<select id="control-chart-sort" class="form-control chosen-select">' +
                  '{{#sortFields}}' +
                    '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                  '{{/sortFields}}' +
                '</select>' +
              '</div>' +
              '<div class="form-group">' +
                '<div class="row">' +
                  '<div class="col-md-12 col-sm-12">' +
                    '<label for="exampleInputPassword1">Margin</label>' +
                  '</div>' +
                '</div>' +
                '<div class="row">' +
                  '<div class="col-md-3 col-sm-3">' +
                    '<input id="control-chart-margin-top" type="text" class="form-control" aria-label="" placeholder="Top" value="{{options.margin.top}}">' +
                  '</div>' +
                  '<div class="col-md-3 col-sm-3">' +
                    '<input id="control-chart-margin-right" type="text" class="form-control" aria-label="" placeholder="Right" value="{{options.margin.right}}">' +
                  '</div>' +
                  '<div class="col-md-3 col-sm-3">' +
                    '<input id="control-chart-margin-bottom" type="text" class="form-control" aria-label="" placeholder="Bottom" value="{{options.margin.bottom}}">' +
                  '</div>' +
                  '<div class="col-md-3 col-sm-3">' +
                    '<input id="control-chart-margin-left" type="text" class="form-control" aria-label="" placeholder="Left" value="{{options.margin.left}}">' +
                  '</div>' +
                '</div>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-show-title">' +
                  '<input type="checkbox" id="control-chart-show-title" value="{{showTitle}}" {{#showTitle}}checked{{/showTitle}}/> Show title' +
                '</label>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-show-controls">' +
                  '<input type="checkbox" id="control-chart-show-controls" value="{{options.showControls}}" {{#options.showControls}}checked{{/options.showControls}}/> Show controls' +
                '</label>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-show-legend">' +
                  '<input type="checkbox" id="control-chart-show-legend" value="{{options.showLegend}}" {{#options.showLegend}}checked{{/options.showLegend}}/> Show legend' +
                '</label>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-group">' +
                  '<input type="checkbox" id="control-chart-group" value="{{group}}" {{#group}}checked{{/group}}/> Group by X-Field' +
                '</label>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-show-tooltips">' +
                  '<input type="checkbox" id="control-chart-show-tooltips" {{#options.tooltips}}checked{{/options.tooltips}}/> Show Tooltips' +
                '</label>' +
              '</div>' +
              '<div class="form-group checkbox">' +
                '<label for="control-chart-reduce-ticks">' +
                  '<input type="checkbox" id="control-chart-reduce-ticks" {{#options.reduceXTicks}}checked{{/options.reduceXTicks}}/> Reduce Ticks' +
                '</label>' +
              '</div>' +
            '</div>',

  initialize: function(options){
    var self = this;
    self.state = options.state;
    self.model = options.model;
    self.parent = options.parent;
  },
  events: {
    'change input[type="checkbox"]': 'update',
    'change select': 'update',
    'blur input[type="text"]': 'update',
    'keydown input[type="text"]': 'update',
    'submit #control-chart': 'update'
  },
  render: function(){
    var self = this;
    var sortFields = _.arrayToOptions(_.getFields(self.state.get('model')));
    sortFields.unshift({name:'default', label:'Default', selected: false});
    self.state.set('sortFields', _.applyOption(sortFields, [self.state.get('sort')]));

    var options = self.state.get('options');
    options.margin = options.margin || {top: 15, right: 10, bottom: 50, left: 60};
    self.state.set('options', options);

    self.$el.html(Mustache.render(self.template, self.state.toJSON()));
    self.$('.chosen-select').chosen({width: '95%'});
  },
  update: function(e){
    var self = this;
    if(self.$(e.target).closest('.chosen-container').length) return;
    var newState = {};
    if(e.type === 'keydown' && e.keyCode !== 13) return;
    newState = _.merge({}, self.state.toJSON(), self.getUIState(), function(a, b) {
      if (_.isArray(a)) {
        return b;
      }
    });
    self.state.set(newState);
  },
  getUIState: function(){
    var self = this;
    var color;

    var computedState = {
      group: self.$('#control-chart-group').is(':checked'),
      transitionTime: self.$('#control-chart-transition-time').val(),
      xFormat: self.$('#control-chart-x-format').val(),
      sort: self.$('#control-chart-sort').val(),
      showTitle: self.$('#control-chart-show-title').is(':checked')
    };
    computedState.options = computedState.options || {};
    computedState.options.xAxis = computedState.options.xAxis || {};
    computedState.options.yAxis = computedState.options.yAxis || {};
    computedState.options.tooltips = self.$('#control-chart-show-tooltips').is(':checked');
    computedState.options.showControls = self.$('#control-chart-show-controls').is(':checked');
    computedState.options.showLegend = self.$('#control-chart-show-legend').is(':checked');
    computedState.options.reduceXTicks = self.$('#control-chart-reduce-ticks').is(':checked');
    computedState.options.xAxis.rotateLabels = self.$('#control-chart-label-x-rotation').val();
    color = _.invoke(self.$('#control-chart-color').val().split(','), 'trim');
    computedState.options.xAxis.axisLabel = self.$('#control-chart-x-axis-label').val();
    computedState.options.yAxis.axisLabel = self.$('#control-chart-y-axis-label').val();
    computedState.options.yAxis.axisLabelDistance = parseInt(self.$('#control-chart-y-axis-label-distance').val()) || 0;
    if(self.$('#control-chart-color').val()){
      computedState.options.color = color;
    } else {
      if(computedState.options.color){
        delete computedState.options.color;
      }
    }
    var margin = {
      top: parseInt(self.$('#control-chart-margin-top').val()),
      right: parseInt(self.$('#control-chart-margin-right').val()),
      bottom: parseInt(self.$('#control-chart-margin-bottom').val()),
      left: parseInt(self.$('#control-chart-margin-left').val()),
    };
    computedState.options.margin = margin;
    return computedState;
  }
});

})(jQuery, recline.View.nvd3);
