this.recline = this.recline || {};
this.recline.View = this.recline.View || {};
this.recline.View.nvd3 = this.recline.View.nvd3 || {};

;(function ($, my, global) {
  'use strict';

  /**
   * Chart options step
   */
  global.ChartOptionsView = Backbone.View.extend({
    template: '<div class="col-md-12" id="chart-with-controls">' +
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
                        '<div>{{source.url}}</div>'+
                      '</div>' +
                      '<div class="form-group">' +
                        '<label>X Field</label>' +
                        '<div>{{xfield}}</div>'+
                      '</div>' +
                      '<div class="form-group">' +
                        '<label>Series fields</label>' +
                        '<div>{{seriesFields}}</div>'+
                      '</div>' +
                      '<div class="form-group">' +
                        '<label>Graph Type</label>' +
                        '<div>{{graphType}}</div>'+
                      '</div>' +
                    '</div>' +
                    '<div role="tabpanel" class="tab-pane" id="dataset-tab">' +
                      '<div id="pager"></div>' +
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
                '<div id="prev" class="btn btn-default pull-left">Back</div>' +
                '<button type="submit" class="form-submit btn btn-success pull-right">Finish</button>' +
              '</div>',
    events: {
      '#query-editor button': 'onEditorUpdate'
    },
    initialize: function(options){
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
      console.log('copyQueryState');
      var self = this;
      self.state.set('queryState', self.state.get('model').queryState.toJSON());
    },
    render: function(){
      var self = this;
      var graphType = self.state.get('graphType');

      self.listenTo(self.state.get('model').queryState, 'change', self.copyQueryState);
      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
      self.$('a[data-toggle="tab"]').on('shown.bs.tab', function (e) {
        self.graph.render();
      });

      self.$('.expander').on('click', function(){
        var visible = self.$(this).next().is(':visible');
        var sp = self.$(this).find('span');
        var sign = (!visible) ? '-' : '+' ;
        sp.html(sign);
        self.$(this).next().slideToggle('fast');
      });

      // Common controls for all the charts.
      self.baseControls = new recline.View.nvd3.BaseControl({
        model: self.state.get('model'),
        state: self.state,
        parent: self
      });

      // Controls available only for this graphType.
      self.extendedControls = new recline.View.nvd3[graphType + 'Controls']({
        model: self.state.get('model'),
        state: self.state
      });

      // Chart itself.
      self.graph = new recline.View.nvd3[graphType]({
        model: self.state.get('model'),
        state: self.state
      });

      // Pager widget
      self.pager = new recline.View.Pager({
        model: self.state.get('model'),
        state: self.state
      });

      // Search wiget
      self.queryEditor = new recline.View.nvd3.QueryEditor({
        model: self.state.get('model').queryState,
        state: self.state
      });

      // Filter widget
      self.filterEditor = new recline.View.nvd3.FilterEditor({
        model: self.state.get('model'),
        state: self.state
      });

      // Grid
      self.grid = new recline.View.SlickGrid({
        model: self.state.get('model'),
        el: self.$('#grid'),
        options:{}
      });
      self.grid.visible = true;

      self.assign(self.graph, '#chart-viewport');
      self.assign(self.baseControls, '#base-controls');
      self.assign(self.extendedControls, '#extended-controls');
      self.assign(self.grid, '#grid');
      self.assign(self.pager, '#pager');
      self.assign(self.queryEditor, '#query-editor');
      self.assign(self.filterEditor, '#filter-editor');

      // Slickgrid needs to update after tab content is displayed
      $('#grid')
      .closest('.tab-content')
      .prev()
      .find('a[data-toggle="tab"]')
      .on('shown.bs.tab', function () {
        self.grid.grid.resizeCanvas();
      });

      self.$('.chosen-select').chosen({width: '95%'});
    },
    onEditorUpdate: function(){
      return false;
    },
    updateState: function(state, cb){
      cb(state);
    },
    assign: function(view, selector){
      var self = this;
      view.setElement(self.$(selector)).render();
    },
  });

  /**
   * Choose chart view
   */
  global.ChooseChartView = Backbone.View.extend({
    template: '<div class="form-group">' +
                '<div class="form-group">' +
                  '<label>Source</label>' +
                  '<div>{{source.url}}</div>'+
                '</div>' +
                '<div class="form-group">' +
                  '<label>X Field</label>' +
                  '<div>{{xfield}}</div>'+
                '</div>' +
                '<div class="form-group">' +
                  '<label>Series fields</label>' +
                  '<div>{{seriesFields}}</div>'+
                '</div>' +
                '<ul id="chart-selector">' +
                  '{{#graphTypes}}' +
                    '<li class="{{value}} {{#selected}}selected{{/selected}}" data-selected="{{value}}"></li>' +
                  '{{/graphTypes}}' +
                '</ul>' +
              '</div>' +
              '<div id="controls">' +
                '<div id="prev" class="btn btn-default pull-left">Back</div>' +
                '<div id="next" class="btn btn-primary pull-right">Next</div>' +
              '</div>',
    initialize: function(options){
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.stepInfo = {
        title: 'Choose Chart',
        name: 'chooseChart'
      };
    },
    events: {
      'click #chart-selector li': 'selectChart'
    },
    selectChart: function(e){
      var self = this;
      self.$('li').removeClass('selected');
      self.$(e.target).addClass('selected');
    },
    getSelected: function(){
      var self = this;
      return self.$('li.selected').data('selected');
    },
    render: function(){
      var self = this;
      var graphTypes = ['discreteBarChart', 'multiBarChart', 'multiBarHorizontalChart', 'stackedAreaChart', 'pieChart',
        'lineChart', 'lineWithFocusChart', 'scatterChart', 'linePlusBarChart'
      ];

      self.state.set('graphTypes', _.applyOption(
        _.arrayToOptions(graphTypes), [self.state.get('graphType') || 'discreteBarChart']
      ));
      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
      self.$('.chosen-select').chosen({width: '95%'});
    },
    updateState: function(state, cb){
      var self = this;
      var type = self.getSelected();
      state.set('graphType', type);
      cb(state);
    }
  });

  /**
   * Data options view
   */
  global.DataOptionsView = Backbone.View.extend({
    template: '<div class="form-group">' +
                  '<div class="form-group">' +
                    '<label>Source</label>' +
                    '<div>{{source.url}}</div>'+
                  '</div>' +
                  '<label for="control-chart-series">Series</label>' +
                  '<select id="control-chart-series" multiple class="form-control chosen-select">' +
                    '{{#fields}}' +
                      '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                    '{{/fields}}' +
                  '</select>' +
                '</div>' +
                '<div class="form-group">' +
                  '<label for="control-chart-xfield">X-Field</label>' +
                  '<select id="control-chart-xfield" class="form-control chosen-select">' +
                    '{{#xfields}}' +
                      '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                    '{{/xfields}}' +
                  '</select>' +
                '</div>' +
                '<div class="form-group relative">' +
                  '{{#xDataTypes}}' +
                    '<label class="radio-inline">' +
                      '<input type="radio" name="control-chart-x-data-type" id="control-chart-x-data-type-{{value}}" value="{{value}}" {{#selected}}checked {{/selected}}> {{name}}' +
                    '</label>' +
                  '{{/xDataTypes}}' +
                '</div>' +
                '<div id="controls">' +
                  '<div id="prev" class="btn btn-default pull-left">Back</div>' +
                  '<div id="next" class="btn btn-primary pull-right">Next</div>' +
                '</div>' +
              '</div>',
    initialize: function(options){
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.stepInfo = {
        title: 'Define Variables',
        name: 'dataOptions'
      };
    },
    render: function(){
      var self = this;
      var dataTypes = ['Number', 'String', 'Date', 'Auto'];

      self.state.set('fields', _.applyOption(
        _.arrayToOptions(_.getFields(self.state.get('model'))), self.state.get('seriesFields')
      ));
      self.state.set('xfields', _.applyOption(
        _.arrayToOptions(_.getFields(self.state.get('model'))), [self.state.get('xfield')]
      ));
      self.state.set('xDataTypes', _.applyOption(
        _.arrayToOptions(dataTypes), [self.state.get('xDataType') || 'Auto']
      ));

      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
      self.$('.chosen-select').chosen({width: '95%'});
    },
    updateState: function(state, cb){
      var self = this;
      state.set('seriesFields', self.$('#control-chart-series').val());
      state.set('xfield', self.$('#control-chart-xfield').val());
      state.set('xDataType', self.$('input[name=control-chart-x-data-type]:checked').val());
      cb(state);
    }
  });

  /**
   * Load data view
   */
  global.LoadDataView = Backbone.View.extend({
    template: '<div class="form-group">' +
                '<label for="control-chart-source">Source</label>' +
                '<input value="{{source.url}}" type="text" id="control-chart-source" class="form-control" />' +
              '</div>' +
              '<div class="form-group">' +
                '<select id="control-chart-backend" class="form-control">' +
                  '<option value="csv">CSV</option>' +
                  '<option value="gdocs">Google Spreadsheet</option>' +
                  '<option value="dataproxy">DataProxy</option>' +
                '</select>' +
              '</div>' +
              '<div id="controls">' +
                '<div id="next" class="btn btn-primary pull-right">Next</div>' +
              '</div>',
    initialize: function(options){
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.model = self.options.model;
      self.stepInfo = {
        title: 'Load Data',
        name: 'loadData'
      };
    },
    render: function(){
      var self = this;
      self.$el.html(Mustache.render(self.template, self.state.toJSON()));
    },
    updateState: function(state, cb){
      var self = this;
      var url = self.$('#control-chart-source').val();
      var backend = self.$('#control-chart-backend').val();
      var source = {
        backend: backend,
        url: url
      };

      state.set('source', source);
      var model = new recline.Model.Dataset(source);
      state.set('model', model)
      model.fetch().done(cb.bind(this, state)).fail(function (err) {
        console.log(err);
        alert('Failed to fetch the resource');
      });
    }
  });

  /**
   * Multi stage view
   */
  global.MultiStageView = Backbone.View.extend({
    template: '<h3>{{title}}</h3>' +
              '<input type="hidden" value="{{state}}"/>' +
              '<div id="step"></div>',
    events:{
      'click #next': 'nextStep',
      'click #prev': 'prevStep'
    },
    initialize: function(options){
      var self = this;
      self.options = _.defaults(options || {}, self.options);
      self.state = self.options.state;
      self.currentView = null;
      self.currentStep = self.state.get('step') || 0;
      self.steps = [];

      self.state.set('step', self.currentStep);
    },
    render: function(){
      var self = this;
      self.currentView = self.getStep(self.currentStep);
      _.extend(self.currentView.stepInfo, {state:JSON.stringify(self.state.toJSON())});
      self.$el.html(Mustache.render(self.template, self.currentView.stepInfo));

      self.assign(self.currentView, '#step');
      return self;
    },
    assign: function(view, selector){
      var self = this;
      view.setElement(self.$(selector)).render();
    },
    addStep: function(view){
      var self = this;
      self.steps.push(view);
    },
    getStep: function(index){
      var self = this;
      return self.steps[index];
    },
    nextStep: function(){
      var self = this;
      var toNext = self.updateStep(self.getNext(self.steps, self.currentStep));
      self.currentView.updateState(self.state, toNext);
    },
    prevStep: function(){
      var self = this;
      var toPrev = self.updateStep(self.getPrev(self.steps, self.currentStep));
      self.currentView.updateState(self.state, toPrev);
    },
    getNext: function(steps, current){
      var limit = steps.length - 1;
      if(limit === current){
        return current;
      }
      return ++current;
    },
    getPrev: function(steps, current){
      if(current){
        return --current;
      }
      return current;
    },
    updateStep: function(n){
      var self = this;
      return function(state){
        self.state = state;
        self.gotoStep(n);
        self.trigger('multistep:change', {step:n});
      };
    },
    gotoStep: function(n){
      var self = this;
      self.currentStep = n;
      self.state.set('step', self.currentStep);
      self.render();
    }
  });

})(jQuery, recline.View.nvd3, window);
