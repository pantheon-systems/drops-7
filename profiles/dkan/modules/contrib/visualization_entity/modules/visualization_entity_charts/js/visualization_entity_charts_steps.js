/**
 * @file
 * Provides multi-page form for chart visualization.
 */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};
this.recline.View.nvd3 = this.recline.View.nvd3 || {};

;(function ($, my, global) {
  'use strict';

  /**
   * Chart options step.
   */
  global.ChartOptionsView = Backbone.View.extend({
    template: '<div class="data-explorer-help"><i class="fa fa-info-circle" aria-hidden="true"></i> ' +
              '<strong>Chart Preview:</strong> Note that by default the preview only displays up to 100 records. ' +
              'Click on the Dataset tab below to review the data in use. Adjust the start and end fields of the ' +
              'pager to set the number of records you wish to use.</div>' +
              '<div class="col-md-12" id="chart-with-controls">' +
                '<div class="col-md-7">' +
                  '<ul class="nav nav-tabs" role="tablist" id="myTab">' +
                    '<li role="presentation" class="active"><a href="#chart-tab" aria-controls="home" role="tab" data-toggle="tab">Chart</a></li>' +
                    '<li role="presentation"><a href="#dataset-tab" aria-controls="settings" role="tab" data-toggle="tab">Dataset</a></li>' +
                  '</ul>' +
                  '<div class="tab-content">' +
                    '<div role="tabpanel" class="tab-pane active" id="chart-tab">' +
                      '<div  id="chart-viewport"></div>' +
                      '<div class="form-group">' +
                        '<label>Source</label>' +
                        '<div>{{source.url}}</div>' +
                      '</div>' +
                      '<div class="form-group">' +
                        '<label>X Field</label>' +
                        '<div>{{xfield}}</div>' +
                      '</div>' +
                      '<div class="form-group">' +
                        '<label>Series fields</label>' +
                        '<div>{{seriesFields}}</div>' +
                      '</div>' +
                      '<div class="form-group">' +
                        '<label>Graph Type</label>' +
                        '<div>{{graphType}}</div>' +
                      '</div>' +
                    '</div>' +
                    '<div role="tabpanel" class="tab-pane" id="dataset-tab">' +
                      '<div class="data-details">' +
                        '<span id="pager"></span>' +
                        '<span class="data-results"><span class="doc-count">{{recordCount}}</span> records</span>' +
                      '</div>' +
                      '<div id="grid"></div>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
                '<div class="col-md-5">' +
                  '<div class="form-panel">' +
                    '<h4 class="expander">Query Editor <span>+</span></h4>' +
                    '<div class="expansible" style="display:none" id="query-editor"></div>' +
                  '</div>' +
                  '<div class="form-panel">' +
                    '<h4 class="expander">Filter Editor<span>+</span></h4>' +
                    '<div class="expansible" style="display:none" id="filter-editor"></div>' +
                  '</div>' +
                  '<div class="form-panel">' +
                    '<h4 class="expander">Chart configuration<span>-</span></h4>' +
                    '<div class="expansible">' +
                      '<div id="base-controls"></div>' +
                      '<div id="extended-controls"></div>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
              '</div>' +
              '<div class="col-md-12" id="controls">' +
                '<button type="button" id="prev" class="btn btn-default pull-left">Back</button>' +
                '<button type="submit" id="finish" class="form-submit btn btn-success pull-right">Finish</button>' +
              '</div>',
    events: {
      '#query-editor button': 'onEditorUpdate',
      'click #finish': 'finish'
    },
    initialize: function (options) {
      console.log('initialize');
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.model = self.state.get('model');

      self.stepInfo = {
        title: 'Preview and Adjust',
        name: 'chartOptions'
      };
    },
    copyQueryState: function(){
      var self = this;
      self.state.set('queryState', self.state.get('model').queryState.toJSON());
    },
    render: function () {
      var self = this;
      var graphType = self.state.get('graphType');
      self.listenTo(self.state.get('model').queryState, 'change', self.copyQueryState);
      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
      self.$el.find('.doc-count').text(self.state.get('model').recordCount || 'Unknown');
      self.$('a[data-toggle="tab"]').on('shown.bs.tab', function (e) {
        self.graph.render();
      });

      self.$('.expander').on('click', function () {
        var visible = self.$(this).next().is(':visible');
        var sp = self.$(this).find('span');
        var sign = (!visible) ? '-' : '+';
        sp.html(sign);
        self.$(this).next().slideToggle('fast');
      });

      // Controls available only for this graphType.
      self.controls = new recline.View.nvd3[graphType + 'Controls']({
        model: self.state.get('model'),
        state: self.state
      });

      // Chart itself.
      self.graph = new recline.View.nvd3[graphType]({
        model: self.state.get('model'),
        state: self.state
      });

      // Pager widget.
      self.pager = new recline.View.Pager({
        model: self.state.get('model'),
        state: self.state
      });

      // Search wiget.
      self.queryEditor = new recline.View.nvd3.QueryEditor({
        model: self.state.get('model').queryState,
        state: self.state
      });

      // Filter widget.
      self.filterEditor = new recline.View.nvd3.FilterEditor({
        model: self.state.get('model'),
        state: self.state
      });

      // Grid.
      self.grid = new recline.View.SlickGrid({
        model: self.state.get('model'),
        el: self.$('#grid'),
        options:{}
      });
      self.grid.visible = true;

      self.assign(self.graph, '#chart-viewport');
      self.assign(self.controls, '#base-controls');
      self.assign(self.grid, '#grid');
      self.assign(self.pager, '#pager');
      self.assign(self.queryEditor, '#query-editor');
      self.assign(self.filterEditor, '#filter-editor');

      // Slickgrid needs to update after tab content is displayed.
      $('#grid')
      .closest('.tab-content')
      .prev()
      .find('a[data-toggle="tab"]')
      .on('shown.bs.tab', function () {
        self.grid.grid.resizeCanvas();
      });

      self.$('.chosen-select').chosen({width: '95%'});
    },
    onEditorUpdate: function () {
      return false;
    },
    updateState: function(state, cb){
      // TO CHECK: This never gets executed.
      cb(state);
    },
    assign: function (view, selector) {
      var self = this;
      view.setElement(self.$(selector)).render();
    },
    finish: function(event) {
      var self = this;
      if (!self.validate()) {
        event.preventDefault();
      }
    },
    validate: function() {
      var self = this;
      return self.controls.validate();
    },
  });

  /**
   * Choose chart view.
   */
  global.ChooseChartView = Backbone.View.extend({
    template: '<div class="form-group">' +
                '<div class="form-group">' +
                  '<label>Source</label>' +
                  '<div>{{source.url}}</div>' +
                '</div>' +
                '<div class="form-group">' +
                  '<label>X Field</label>' +
                  '<div>{{xfield}}</div>' +
                '</div>' +
                '<div class="form-group">' +
                  '<label>Series fields</label>' +
                  '<div>{{seriesFields}}</div>' +
                '</div>' +
                '<div id="chart-selector">' +
                  '{{#graphTypes}}' +
                    '<button type="button" class="{{value}} {{#selected}}selected{{/selected}}" data-selected="{{value}}"  data-toggle="popover" data-placement="top" data-trigger="hover" data-content="{{value}}"><span class="sr-only">{{value}}</span></button>' +
                  '{{/graphTypes}}' +
                '</ul>' +
              '</div>' +
              '<div id="controls">' +
                '<button type="button" id="prev" class="btn btn-default pull-left">Back</button>' +
                '<button type="button" id="next" class="btn btn-primary pull-right">Next</button>' +
              '</div>',
    initialize: function (options) {
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.stepInfo = {
        title: 'Choose Chart',
        name: 'chooseChart'
      };
    },
    events: {
      'click #chart-selector button': 'selectChart'
    },
    selectChart: function (e) {
      var self = this;
      self.$('button').removeClass('selected');
      self.$(e.target).addClass('selected');
    },
    getSelected: function () {
      var self = this;
      return self.$('button.selected').data('selected');
    },
    render: function () {
      var self = this;
      var graphTypes = [
        'discreteBarChart', 'multiBarChart', 'multiBarHorizontalChart', 'stackedAreaChart',
        'pieChart', 'lineChart', 'lineWithFocusChart', 'scatterChart', 'linePlusBarChart'
      ];
      self.state.set('graphTypes', graphTypes.map(function (type, index) {
        var selected = type === (self.state.get('graphType') || 'discreteBarChart');
        return {value: type, selected: selected};
      }));
      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
      self.$('.chosen-select').chosen({width: '95%'});

      $('[data-toggle="popover"]').popover({ trigger: "hover" });
    },
    updateState: function (state, cb) {
      var self = this;
      var type = self.getSelected();
      state.set('graphType', type);
      var result = self.validate();
      cb(state, result);
    },
    validate: function() {
      return true;
    },
  });

  /**
   * Data options view.
   */
  global.DataOptionsView = Backbone.View.extend({
    template:   '<div class="form-group">' +
                  '<div class="form-group">' +
                    '<label>Source</label>' +
                    '<div>{{source.url}}</div>' +
                  '</div>' +
                '</div>' +
                '<div class="form-panel pad">' +
                  '<label for="control-chart-series">Series</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" ' +
                    'title="Series Help" data-content="Add all of the columns from your table from which you would like to plot the values. These will become the data series in your chart."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                  '<select title="Select a column whose values will be used as series" id="control-chart-series" multiple class="form-control chosen-select">' +
                    '{{#fields}}' +
                      '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                    '{{/fields}}' +
                  '</select>' +
                  '<div class="form-group relative">' +
                    '<label>Y-Field Data Type</label>' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" title="Y-Field Data Type" data-content="You can specify the type of data used for the Y-Field(s) here if the auto-detect feature is not picking up the correct type."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a><br>' +
                    '{{#yDataTypes}}' +
                      '<label class="radio-inline">' +
                        '<input type="radio" name="control-chart-y-data-type" id="control-chart-y-data-type-{{value}}" value="{{value}}" {{#selected}}checked {{/selected}}> {{name}}' +
                      '</label>' +
                    '{{/yDataTypes}}' +
                  '</div>' +
                '</div>' +
                '<div class="form-panel pad">' +
                  '<div class="form-group">' +
                    '<label for="control-chart-xfield">X-Field</label>' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" title="X-Field Help" data-content="Enter the column title to use for the horizontal (X) axis"><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                    '<div class="form-group"><select id="control-chart-xfield" class="form-control chosen-select">' +
                      '{{#xfields}}' +
                        '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                      '{{/xfields}}' +
                    '</select></div>' +
                  '</div>' +
                  '<div class="form-group relative">' +
                    '<label>X-Field Data Type</label>' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus" title="X-Field Data Type" data-content="You can specify the type of data used for the X-Field here if the auto-detect feature is not picking up the correct type."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a><br>' +
                    '{{#xDataTypes}}' +
                      '<label class="radio-inline">' +
                        '<input type="radio" name="control-chart-x-data-type" id="control-chart-x-data-type-{{value}}" value="{{value}}" {{#selected}}checked {{/selected}}> {{name}}' +
                      '</label>' +
                    '{{/xDataTypes}}' +
                  '</div>' +
                '</div>' +
                '<div id="controls">' +
                  '<button type="button" id="prev" class="btn btn-default pull-left">Back</button>' +
                  '<button type="button" id="next" class="btn btn-primary pull-right">Next</button>' +
                '</div>' +
              '</div>',
    initialize: function (options) {
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.stepInfo = {
        title: 'Define Variables',
        name: 'dataOptions'
      };
    },
    render: function () {
      var self = this;
      var dataTypes = ['Number', 'String', 'Date', 'Auto'];

      self.state.set('fields', _.applyOption(
        _.arrayToOptions(_.getFields(self.state.get('model'))), self.state.get('seriesFields')
      ));
      self.state.set('yDataTypes', _.applyOption(
        _.arrayToOptions(dataTypes), [self.state.get('yDataType') || 'Auto']
      ));
      self.state.set('xfields', _.applyOption(
        _.arrayToOptions(_.getFields(self.state.get('model'))), [self.state.get('xfield')]
      ));
      self.state.set('xDataTypes', _.applyOption(
        _.arrayToOptions(dataTypes), [self.state.get('xDataType') || 'Auto']
      ));

      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
      self.$('.chosen-select').chosen({width: '95%'});

      $('[data-toggle="popover"]').popover();
    },
    updateState: function (state, cb) {
      var self = this;
      state.set('seriesFields', self.$('#control-chart-series').val());
      state.set('yDataType', self.$('input[name=control-chart-y-data-type]:checked').val());
      state.set('xfield', self.$('#control-chart-xfield').val());
      state.set('xDataType', self.$('input[name=control-chart-x-data-type]:checked').val());
      var result = self.validate();
      cb(state, result);
    },
    validate: function() {
      return true;
    },
  });

  /**
   * Load data view.
   *
   * Hiding Source Url field until further work can be done on it, it is useless at this point.
   */
  global.LoadDataView = Backbone.View.extend({
    template: '<div class="form-group" style="display:none;">' +
                '<label for="control-chart-source">Source Url</label>' +
                ' <em>(Auto-populated if using DKAN.)</em>' +
                '<input value="{{source.url}}" type="text" id="control-chart-source" class="form-control" />' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-backend">Source Type</label>' +
                ' <em>(Experimental : Backends other than DKAN are still a work in progress.)</em>' +
                '<select title="Select backend source type" id="control-chart-backend" class="form-control">' +
                  '<option value="csv">CSV</option>' +
                  '<option value="gdocs">Google Spreadsheet</option>' +
                '</select>' +
              '</div>' +
              '<div id="controls">' +
                '<button type="button" id="next" class="btn btn-primary pull-right">Next</button>' +
              '</div>',
    initialize: function (options) {
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.model = self.options.model;
      self.stepInfo = {
        title: 'Load Data',
        name: 'loadData'
      };
    },
    render: function () {
      var self = this;
      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
    },
    updateState: function (state, cb) {
      var self = this;
      var url = self.$('#control-chart-source').val();
      var backend = self.$('#control-chart-backend').val();
      var source = {
        backend: backend,
        url: url
      };

      state.set('source', source);
      var model = new recline.Model.Dataset(source);
      model.fetch().done(function () {
        state.set('model', model);
        cb(state);
      }).fail(function (err) {
        console.log(err);
        alert('Failed to fetch the resource');
      });
    }
  });

  /**
   * Multi stage view.
   */
  global.MultiStageView = Backbone.View.extend({
    template: '<h3>{{title}}</h3>' +
              '<input type="hidden" value="{{state}}"/>' +
              '<div id="step"></div>',
    events:{
      'click #next': 'nextStep',
      'click #prev': 'prevStep'
    },
    initialize: function (options) {
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.currentView = null;
      self.currentStep = self.state.get('step') || 0;
      self.steps = [];

      self.state.set('step', self.currentStep);
    },
    render: function () {
      var self = this;
      self.currentView = self.getStep(self.currentStep);
      _.extend(self.currentView.stepInfo, {state:JSON.stringify(self.state.toJSON())});
      self.$el.html(Mustache.render(self.template, self.currentView.stepInfo));

      self.assign(self.currentView, '#step');
      return self;
    },
    assign: function (view, selector) {
      var self = this;
      view.setElement(self.$(selector)).render();
    },
    addStep: function (view) {
      var self = this;
      self.steps.push(view);
    },
    getStep: function (index) {
      var self = this;
      return self.steps[index];
    },
    nextStep: function () {
      var self = this;
      var toNext = self.updateStep(self.getNext(self.steps, self.currentStep));
      self.currentView.updateState(self.state, toNext);
    },
    prevStep: function () {
      var self = this;
      var toPrev = self.updateStep(self.getPrev(self.steps, self.currentStep));
      self.currentView.updateState(self.state, toPrev);
    },
    getNext: function (steps, current) {
      var limit = steps.length - 1;
      if (limit === current) {
        return current;
      }
      return ++current;
    },
    getPrev: function (steps, current) {
      if (current) {
        return --current;
      }
      return current;
    },
    updateStep: function (n) {
      var self = this;
      return function(state, success){
        success = typeof success !== 'undefined' ? success : true;
        if (success) {
          self.state = state;
          self.gotoStep(n);
          self.trigger('multistep:change', {step:n});
          self.$('.chosen-choices .search-field input, .chosen-search input').attr('aria-label', 'Choose some options');
        } 
      };
    },
    gotoStep: function (n) {
      var self = this;
      self.currentStep = n;
      self.state.set('step', self.currentStep);
      self.render();
    }
  });

})(jQuery, recline.View.nvd3, window);
