/*jshint multistr:true */
this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

my.BaseControl = Backbone.View.extend({
  templateTop:
            '<div id="control-chart-container">' +
              '<div class="recline-nvd3-query-editor"></div>' +
              '<div class="recline-nvd3-filter-editor"></div>' ,
  templateXFormat:
              '<fieldset>' +
                '<legend>X Axis</legend>' +
              '<div class="form-group">' +
                '<label for="control-chart-x-format">X-Format</label>' +
                '<select class="form-control" id="control-chart-x-format">' +                    
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="Percentage" value="p,.2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value="d">97 -> 97%</option>' +
                      '<option data-type="PercentageA" value="">97 -> 97.00%</option>' +
                    '</optgroup>' +
                '</select>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-label-x-rotation">X Label Rotation</label>' +
                '<input value="{{options.xAxis.rotateLabels}}" type="text" id="control-chart-label-x-rotation" class="form-control" placeholder="e.g: -45"/>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-transition-time">Transition Time (milliseconds)</label>' +
                '<input value="{{transitionTime}}" type="text" id="control-chart-transition-time" class="form-control" placeholder="e.g: 2000"/>' +
              '</div>' +
              '<div class="form-group">' +
                  
                  '<label for="control-chart-color-picker">Color</label>' +
                  '<input type="text" class="form-control" id="control-chart-color-picker"/>' +
                  '<input class="form-control" type="text" id="control-chart-color" value="{{options.color}}" placeholder="e.g: #FF0000,green,blue,#00FF00"/>' +
              '</div>' +
              '<div class="form-group">' +
                  '<label for="control-chart-x-axis-label">X Axis Label</label>' +
                  '<input class="form-control" type="text" id="control-chart-x-axis-label" value="{{options.xAxis.axisLabel}}"/>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-sort">Sort</label>' +
                '<select id="control-chart-sort" class="form-control chosen-select">' +
                  '{{#sortFields}}' +
                    '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                  '{{/sortFields}}' +
                '</select>' +
              '</div>' +

                /// Axis ticks
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-x-values">Tick Values</label>' +
                      '<input class="form-control" type="text" placeholder="From.." id="control-chart-x-values-from" value="{{xValuesFrom}}"/>' +
                      '<input class="form-control" type="text" placeholder="To.." id="control-chart-x-values-to" value="{{xValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-x-values-step">Step</label>' +
                      '<input class="form-control" type="number" id="control-chart-x-values-step" value="{{xValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
              '</fieldset>',
  templateYFormat:
              //////// Y AXIS
              '<fieldset>' +
                '<legend>Y Axis</legend>' +

                /// Format
                '<div class="form-group">' +
                  '<label for="control-chart-y-format">Format</label>' +
                  '<select class="form-control" id="control-chart-y-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="PercentageB" value=".2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value=".0f">97 -> 97%</option>' +
                      '<option data-type="PercentageA" value=".2f">97 -> 97.00%</option>' +
                    '</optgroup>' +
                  '</select>' +
                '</div>' +

                /// Axis label
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-y-axis-label">Y Axis Label</label>' +
                      '<input class="form-control" type="text" id="control-chart-y-axis-label" value="{{options.yAxis.axisLabel}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-y-axis-label-distance">Distance</label>' +
                      '<input class="form-control" type="number" id="control-chart-y-axis-label-distance" value="{{options.yAxis.axisLabelDistance}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Axis ticks
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-y-values">Tick Values</label>' +
                      '<input class="form-control" placeholder="From.." type="text" id="control-chart-y-values-from" value="{{yValuesFrom}}"/>' +
                      '<input class="form-control" placeholder="To.." type="text" id="control-chart-y-values-to" value="{{yValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-y-values-step">Step</label>' +
                      '<input class="form-control" type="number" id="control-chart-y-values-step" value="{{yValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
              '</fieldset>',
  templateY1Format:
              //////// Y1 AXIS
              '<fieldset>' +
                '<legend>Y-1 Axis</legend>' +

                /// Format
                '<div class="form-group">' +
                  '<label for="control-chart-y1-format">Format</label>' +
                  '<select class="form-control" id="control-chart-y1-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="Percentage" value="p,.2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value="">97 -> 97.00%</option>' +
                    '</optgroup>' +
                  '</select>' +
                '</div>' +

                /// Axis label
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-y1-axis-label">Y Axis Label</label>' +
                      '<input class="form-control" type="text" id="control-chart-y1-axis-label" value="{{options.y1Axis.axisLabel}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-y1-axis-label-distance">Distance</label>' +
                      '<input class="form-control" type="number" id="control-chart-y1-axis-label-distance" value="{{options.y1Axis.axisLabelDistance}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Axis ticks
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-y1-values">Tick Values</label>' +
                      '<input class="form-control" placeholder="From.." type="text" id="control-chart-y1-values-from" value="{{y1ValuesFrom}}"/>' +
                      '<input class="form-control" placeholder="To.." type="text" id="control-chart-y1-values-to" value="{{y1ValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-y1-values-step">Step</label>' +
                      '<input class="form-control" type="number" id="control-chart-y1-values-step" value="{{y1ValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
              '</fieldset>',
  templateY2Format: 
              //////// Y2 AXIS
              '<fieldset>' +
                '<legend>Y-2 Axis</legend>' +

                /// Format
                '<div class="form-group">' +
                  '<label for="control-chart-y2-format">Format</label>' +
                  '<select class="form-control" id="control-chart-y2-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="Percentage" value="p,.2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value="d">97 -> 97%</option>' +
                      '<option data-type="PercentageA" value="">97 -> 97.00%</option>' +
                    '</optgroup>' +
                  '</select>' +
                '</div>' +

                /// Axis label
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-y2-axis-label">Y Axis Label</label>' +
                      '<input class="form-control" type="text" id="control-chart-y2-axis-label" value="{{options.y2Axis.axisLabel}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-y2-axis-label-distance">Distance</label>' +
                      '<input class="form-control" type="number" id="control-chart-y2-axis-label-distance" value="{{options.y2Axis.axisLabelDistance}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Axis ticks
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-9 col-sm-9">' +
                      '<label for="control-chart-y2-values">Tick Values</label>' +
                      '<input class="form-control" placeholder="From.." type="text" id="control-chart-y2-values-from" value="{{y2ValuesFrom}}"/>' +
                      '<input class="form-control" placeholder="To.." type="text" id="control-chart-y2-values-to" value="{{y2ValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<label for="control-chart-y2-values-step">Step</label>' +
                      '<input class="form-control" type="number" id="control-chart-y2-values-step" value="{{y2ValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
              '</fieldset>',
  templateGeneral: 
              //////// GENERAL
              '<fieldset>' +
                '<legend>General</legend>' +

                /// Color
                '<div class="form-group">' +
                    '<label for="control-chart-color">Color</label>' +
                    '<input class="form-control" type="text" id="control-chart-color" value="{{options.color}}" placeholder="e.g: #FF0000,green,blue,#00FF00"/>' +
                '</div>' +

                /// Transition time
                '<div class="form-group">' +
                  '<label for="control-chart-transition-time">Transition Time (milliseconds)</label>' +
                  '<input value="{{transitionTime}}" type="text" id="control-chart-transition-time" class="form-control" placeholder="e.g: 2000"/>' +
                '</div>' +

                /// Goal
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-12 col-sm-12">' +
                      '<label>Goal</label>' +
                    '</div>' +
                  '</div>' +
                  '<div class="row">' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input id="control-chart-goal-value" type="text" class="form-control" aria-label="" placeholder="e.g.: 50" value="{{goal.value}}">' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input id="control-chart-goal-color" type="text" class="form-control" aria-label="" placeholder="e.g.: red" value="{{goal.color}}">' +
                    '</div>' +
                    '<div class="col-md-6 col-sm-3">' +
                      '<div class="form-group checkbox checkbox-without-margin">' +
                        '<label for="control-chart-goal-outside">' +
                          '<input type="checkbox" id="control-chart-goal-outside" value="{{goal.outside}}" {{#goal.outside}}checked{{/goal.outside}}/> Label outside' +
                        '</label>' +
                      '</div>' +
                      '<div class="form-group checkbox checkbox-without-margin">' +
                        '<label for="control-chart-goal-label">' +
                          '<input type="checkbox" id="control-chart-goal-label" value="{{goal.label}}" {{#goal.label}}checked{{/goal.label}}/> Show label' +
                        '</label>' +
                      '</div>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Data sort
                '<div class="form-group">' +
                  '<label for="control-chart-sort">Sort</label>' +
                  '<select id="control-chart-sort" class="form-control chosen-select">' +
                    '{{#sortFields}}' +
                      '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                    '{{/sortFields}}' +
                  '</select>' +
                '</div>' +

                /// Margin
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-12 col-sm-12">' +
                      '<label>Margin</label>' +
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

                /// Show title
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-title">' +
                    '<input type="checkbox" id="control-chart-show-title" value="{{showTitle}}" {{#showTitle}}checked{{/showTitle}}/> Show title' +
                  '</label>' +
                '</div>' +

                /// Show controls
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-controls">' +
                    '<input type="checkbox" id="control-chart-show-controls" value="{{options.showControls}}" {{#options.showControls}}checked{{/options.showControls}}/> Show controls' +
                  '</label>' +
                '</div>' +

                /// Show legend
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-legend">' +
                    '<input type="checkbox" id="control-chart-show-legend" value="{{options.showLegend}}" {{#options.showLegend}}checked{{/options.showLegend}}/> Show legend' +
                  '</label>' +
                '</div>' +

                /// Group
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-group">' +
                    '<input type="checkbox" id="control-chart-group" value="{{group}}" {{#group}}checked{{/group}}/> Group by X-Field' +
                  '</label>' +
                '</div>' +

                /// Show tooltips
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-tooltips">' +
                    '<input type="checkbox" id="control-chart-show-tooltips" {{#options.tooltips}}checked{{/options.tooltips}}/> Show Tooltips' +
                  '</label>' +
                '</div>' +

                /// Reduce ticks
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-reduce-ticks">' +
                    '<input type="checkbox" id="control-chart-reduce-ticks" {{#options.reduceXTicks}}checked{{/options.reduceXTicks}}/> Reduce Ticks' +
                  '</label>' +
                '</div>' +
              '</div>' +
            '</fieldset>',
  
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
    template += this.templateXFormat;

    if (this.state.get('graphType') === 'linePlusBarChart') {
      template += this.templateY1Format
      template += this.templateY2Format;
      template += this.linePlusBarXSelect();
    } else {
      template += this.templateYFormat;
    }

    template += this.templateGeneral;
    
    return template;
  },
  initialize: function(options){
    _.extend(this, options);
  },
  events: {
    'change input[type="checkbox"]': 'update',
    'change select': 'update',
    'blur input[type="text"]': 'update',
    'keydown input[type="text"]': 'update',
    'keydown input[type="number"]': 'update',
    'change input[type="number"]': 'update',
    'submit #control-chart': 'update'
  },
  render: function(){
    var controlsRendered = false;
    var self = this;
    var sortFields = _.arrayToOptions(_.getFields(self.state.get('model')));
    sortFields.unshift({name:'default', label:'Default', selected: false});
    self.state.set('sortFields', _.applyOption(sortFields, [self.state.get('sort')]));

    var options = self.state.get('options') || {};
    options.margin = options.margin || {top: 15, right: 10, bottom: 50, left: 60};
    self.state.set('options', options, {silent : true});
    
    // subclasses define a template with extra controls
    // otherwise we're rendering the base controls using our compose method
    if (self.template) {
      $('#extended-controls').html(Mustache.render(self.template), self.state.toJSON());
    } 
    
    if (!self.controlsRendered){
      self.controlsRendered = true;
      $('#base-controls').html(Mustache.render(self.composeTemplate(), self.state.toJSON()));
    }

    self.$('.chosen-select').chosen({width: '95%'});

    if(self.state.get('xFormat') && self.state.get('xFormat').format) {
      self.$('#control-chart-x-format option[value="' + self.state.get('xFormat').format + '"]').attr('selected', 'selected');
    }
    if(self.state.get('yFormat') && self.state.get('yFormat').format) {
      self.$('#control-chart-y-format option[value="' + self.state.get('yFormat').format + '"]').attr('selected', 'selected');
    }
    $('#control-chart-color').on('blur', function (e) {
      self.update(e);
    });
    $('#control-chart-color-picker').spectrum({
      change : function (color) {
        $('#control-chart-color').val(function (i, val) {
          var newVal;
          if (val) { newVal = val + ', ' + color.toHexString(); }
          else { newVal = color.toHexString(); }
          return newVal;
        });
        $('input#control-chart-color').trigger('blur');
      }
    });
    if (self.renderQueryEditor) {
      this.queryEditor = new my.QueryEditor({
        el : '.recline-nvd3-query-editor',
        model: this.model.queryState,
        state: this.state
      });
      this.queryEditor.render();
    }
    if (self.renderFilterEditor) {
      this.filterEditor = new my.FilterEditor({
        el : '.recline-nvd3-filter-editor',
        model: this.model,
        state: this.state
      });
    }
  },
  update: function(e){
    var self = this;
    var newState = {};
    if (e) {
      if(self.$(e.target).closest('.chosen-container').length) return;
      if(e.type === 'keydown' && e.keyCode !== 13) return;
    }
    newState = _.merge({}, self.state.toJSON(), self.getUIState());
    self.state.set(newState);
  },
  getUIState: function(){
    var self = this;
    var color;
    var rotationVal = parseInt(self.$('#control-chart-label-x-rotation').val());
    var computedState = {
      group: self.$('#control-chart-group').is(':checked'),
      transitionTime: self.$('#control-chart-transition-time').val(),
      xFormat:{
        type: self.$('#control-chart-x-format option:selected').data('type'),
        format: self.$('#control-chart-x-format option:selected').val()
      },
      yFormat:{
        type: self.$('#control-chart-y-format option:selected').data('type'),
        format: self.$('#control-chart-y-format option:selected').val()
      },
      y1Format:{
        type: self.$('#control-chart-y1-format option:selected').data('type'),
        format: self.$('#control-chart-y1-format option:selected').val()
      },
      y2Format:{
        type: self.$('#control-chart-y2-format option:selected').data('type'),
        format: self.$('#control-chart-y2-format option:selected').val()
      },
      sort: self.$('#control-chart-sort').val(),
      showTitle: self.$('#control-chart-show-title').is(':checked'),
      xValues: [self.$('#control-chart-x-values-from').val(), self.$('#control-chart-x-values-to').val()],
      xValuesFrom: self.$('#control-chart-x-values-from').val(),
      xValuesTo: self.$('#control-chart-x-values-to').val(),
      xValuesStep: parseInt(self.$('#control-chart-x-values-step').val() || 1),
      yValues: [self.$('#control-chart-y-values-from').val(), self.$('#control-chart-y-values-to').val()],
      yValuesFrom: self.$('#control-chart-y-values-from').val(),
      yValuesTo: self.$('#control-chart-y-values-to').val(),
      yValuesStep: parseInt(self.$('#control-chart-y-values-step').val() || 1),
      y1Values: [self.$('#control-chart-y1-values-from').val(), self.$('#control-chart-y1-values-to').val()],
      y1ValuesFrom: self.$('#control-chart-y1-values-from').val(),
      y1ValuesTo: self.$('#control-chart-y1-values-to').val(),
      y1ValuesStep: parseInt(self.$('#control-chart-y1-values-step').val() || 1),
      y2Values: [self.$('#control-chart-y2-values-from').val(), self.$('#control-chart-y2-values-to').val()],
      y2ValuesFrom: self.$('#control-chart-y2-values-from').val(),
      y2ValuesTo: self.$('#control-chart-y2-values-to').val(),
      y2ValuesStep: parseInt(self.$('#control-chart-y2-values-step').val() || 1),
      lpbBarChartField: self.$('#control-lpb-barchart-field').val(),
    };

    computedState.options = computedState.options || {};
    computedState.options.xAxis = computedState.options.xAxis || {};
    computedState.options.yAxis = computedState.options.yAxis || {};
    computedState.options.y1Axis = computedState.options.y1Axis || {};
    computedState.options.y2Axis = computedState.options.y2Axis || {};
    computedState.options.tooltips = self.$('#control-chart-show-tooltips').is(':checked');
    computedState.options.showControls = self.$('#control-chart-show-controls').is(':checked');
    computedState.options.showLegend = self.$('#control-chart-show-legend').is(':checked');
    computedState.options.reduceXTicks = self.$('#control-chart-reduce-ticks').is(':checked');
    computedState.options.xAxis.rotateLabels = (isNaN(rotationVal)) ? 0 : rotationVal;
    color = _.invoke(self.$('#control-chart-color').val().split(','), 'trim');
    computedState.options.xAxis.axisLabel = self.$('#control-chart-x-axis-label').val();
    computedState.options.yAxis.axisLabel = self.$('#control-chart-y-axis-label').val();
    computedState.options.yAxis.axisLabelDistance = parseInt(self.$('#control-chart-y-axis-label-distance').val()) || 0;
    computedState.options.y1Axis.axisLabel = self.$('#control-chart-y1-axis-label').val();
    computedState.options.y1Axis.axisLabelDistance = parseInt(self.$('#control-chart-y1-axis-label-distance').val()) || 0;
    computedState.options.y2Axis.axisLabel = self.$('#control-chart-y2-axis-label').val();
    computedState.options.y2Axis.axisLabelDistance = parseInt(self.$('#control-chart-y2-axis-label-distance').val()) || 0;
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
    var goal = {
      value: parseFloat(self.$('#control-chart-goal-value').val()) || '',
      color: self.$('#control-chart-goal-color').val(),
      outside: self.$('#control-chart-goal-outside').is(':checked'),
      label: self.$('#control-chart-goal-label').is(':checked'),
    };
    
    // replace NaN Vals with 0
    _.each(_.keys(margin), function (key) {
      margin[key] = (isNaN(margin[key])) ? 0 : margin[key];
    });
    computedState.goal = goal;
    computedState.options.margin = margin;
    return computedState;
  }
});

my.QueryEditor = Backbone.View.extend({
    template: ' \
      <form action="" method="GET" class="form-inline" role="form"> \
        <div class="form-group"> \
          <div class="input-group text-query"> \
            <div class="input-group-btn"> \
              <button type="button" class="btn btn-default">Go &raquo;</button> \
            </div> \
            <input class="form-control search-query" type="text" name="q" value="{{q}}" placeholder="Search data ..."> \
          </div> \
        </div> \
      </form> \
    ',

    events: {
      'click button': 'onFormSubmit',
      'change input': 'onFormSubmit'
    },

    initialize: function() {
      _.bindAll(this, 'render');
      this.listenTo(this.model, 'change', this.render);
      this.render();
    },
    onFormSubmit: function(e) {
      e.preventDefault();
      var query = this.$el.find('.search-query').val();
      this.model.set({q: query});
    },
    render: function() {
      var tmplData = this.model.toJSON();
      var templated = Mustache.render(this.template, tmplData);
      this.$el.html(templated);
    }
  });

  my.FilterEditor = Backbone.View.extend({
    template: ' \
      <div class="filters"> \
        <div class="form-stacked js-add"> \
          <div class="form-group"> \
            <label>Field</label> \
            <select class="fields form-control"> \
              {{#fields}} \
              <option value="{{id}}">{{label}}</option> \
              {{/fields}} \
            </select> \
          </div> \
          <div class="form-group"> \
            <label>Filter type</label> \
            <select class="filterType form-control"> \
              <option value="term">Value</option> \
              <option value="range">Range</option> \
              <option value="geo_distance">Geo distance</option> \
            </select> \
          </div> \
          <button id="add-filter-btn" type="button" class="btn btn-default">Add</button> \
        </div> \
        <div class="form-stacked js-edit"> \
          {{#filters}} \
            {{{filterRender}}} \
          {{/filters}} \
          {{#filters.length}} \
          <button type="button" class="btn btn-default">Update</button> \
          {{/filters.length}} \
        </div> \
      </div> \
    ',
    filterTemplates: {
      term: ' \
        <div class="filter-{{type}} filter"> \
          <div class="form-group"> \
            <label> \
              {{field}} <small>{{type}}</small> \
              <a class="js-remove-filter" href="#" title="Remove this filter" data-filter-id="{{id}}">&times;</a> \
            </label> \
            <input class="form-control" type="text" value="{{term}}" name="term" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
          </div> \
        </div> \
      ',
      range: ' \
        <div class="filter-{{type}} filter"> \
          <fieldset> \
            <div class="form-group"> \
              <label> \
                {{field}} <small>{{type}}</small> \
                <a class="js-remove-filter" href="#" title="Remove this filter" data-filter-id="{{id}}">&times;</a> \
              </label> \
            </div> \
            <div class="form-group"> \
              <label for="">From</label> \
              <input class="form-control" type="text" value="{{from}}" name="from" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
            <div class="form-group"> \
              <label for="">To</label> \
              <input class="form-control" type="text" value="{{to}}" name="to" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
          </fieldset> \
        </div> \
      ',
      geo_distance: ' \
        <div class="filter-{{type}} filter"> \
          <fieldset> \
            <legend> \
              {{field}} <small>{{type}}</small> \
              <a class="js-remove-filter" href="#" title="Remove this filter" data-filter-id="{{id}}">&times;</a> \
            </legend> \
            <div class="form-group"> \
              <label class="control-label" for="">Longitude</label> \
              <input class="input-sm" type="text" value="{{point.lon}}" name="lon" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
            <div class="form-group"> \
              <label class="control-label" for="">Latitude</label> \
              <input class="input-sm" type="text" value="{{point.lat}}" name="lat" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
            <div class="form-group"> \
              <label class="control-label" for="">Distance (km)</label> \
              <input class="input-sm" type="text" value="{{distance}}" name="distance" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
          </fieldset> \
        </div> \
      '
    },
    events: {
      'click .js-remove-filter': 'onRemoveFilter',
      'click .js-add-filter': 'onAddFilterShow',
      'click .js-edit button': 'onTermFiltersUpdate',
      'click #add-filter-btn': 'onAddFilter'
    },
    initialize: function(opts) {
      _.bindAll(this, 'render');
      this.listenTo(this.model.fields, 'all', this.render);
      this.listenTo(this.model.queryState, 'change change:filters:new-blank', this.render);
      _.extend(this, opts);
      this.render();
    },
    render: function() {
      var self = this;
      var tmplData = $.extend(true, {}, this.model.queryState.toJSON());
      // we will use idx in list as there id ...
      tmplData.filters = _.map(tmplData.filters, function(filter, idx) {
        filter.id = idx;
        return filter;
      });
      tmplData.fields = this.model.fields.toJSON();
      tmplData.filterRender = function() {
        return Mustache.render(self.filterTemplates[this.type], this);
      };
      var out = Mustache.render(this.template, tmplData);
      this.$el.html(out);
    },
    onAddFilterShow: function(e) {
      e.preventDefault();
      var $target = $(e.target);
      $target.hide();
      this.$el.find('.js-add').show();
    },
    onAddFilter: function(e) {
      e.preventDefault();
      var $target = $(e.target).closest('.form-stacked');
      $target.hide();
      var filterType = $target.find('select.filterType').val();
      var field      = $target.find('select.fields').val();
      this.model.queryState.addFilter({type: filterType, field: field});
    },
    onRemoveFilter: function(e) {
      e.preventDefault();
      var $target = $(e.target);
      var filterId = $target.attr('data-filter-id');
      this.model.queryState.removeFilter(filterId);
    },
    onTermFiltersUpdate: function(e) {
     var self = this;
      e.preventDefault();
      var filters = self.model.queryState.get('filters');
      var $form = $(e.target).closest('.form-stacked');
      _.each($form.find('input'), function(input) {
        var $input = $(input);
        var filterType  = $input.attr('data-filter-type');
        var filterIndex = parseInt($input.attr('data-filter-id'), 10);
        var name        = $input.attr('name');
        var value       = $input.val();

        switch (filterType) {
          case 'term':
            filters[filterIndex].term = value;
            break;
          case 'range':
            filters[filterIndex][name] = value;
            break;
          case 'geo_distance':
            if(name === 'distance') {
              filters[filterIndex].distance = parseFloat(value);
            }
            else {
              filters[filterIndex].point[name] = parseFloat(value);
            }
            break;
        }
      });
      self.model.queryState.set({filters: filters, from: 0});
      self.model.queryState.trigger('change');
    }
  });

})(jQuery, recline.View.nvd3);
