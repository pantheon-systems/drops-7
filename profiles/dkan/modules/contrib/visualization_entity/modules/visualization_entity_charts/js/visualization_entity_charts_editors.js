this.recline = this.recline || {};
this.recline.View = this.recline.View || {};
this.recline.View.nvd3 = this.recline.View.nvd3 || {};

;(function ($, my, global) {
  'use strict';

  my.QueryEditor = Backbone.View.extend({
    className: 'recline-nvd3-query-editor',
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
    className: 'recline-filter-editor well',
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
    initialize: function() {
      _.bindAll(this, 'render');
      this.listenTo(this.model.fields, 'all', this.render);
      this.listenTo(this.model.queryState, 'change change:filters:new-blank', this.render);
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
        var fieldId     = $input.attr('data-filter-field');
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

  my.Pager = Backbone.View.extend({
    className: 'recline-pager',
    template: ' \
      <div class="pagination"> \
        <ul class="pagination"> \
          <li class="prev action-pagination-update"><a href="" class="btn btn-default">&laquo;</a></li> \
          <li class="page-range"><a><label for="from">From</label><input name="from" type="text" value="{{from}}" /> &ndash; <label for="to">To</label><input name="to" type="text" value="{{to}}" /> </a></li> \
          <li class="next action-pagination-update"><a href="" class="btn btn-default">&raquo;</a></li> \
        </ul> \
      </div> \
    ',

    events: {
      'click .action-pagination-update': 'onPaginationUpdate',
      'change input': 'onFormSubmit'
    },

    initialize: function() {
      _.bindAll(this, 'render');
      this.listenTo(this.model.queryState, 'change', this.render);
      this.render();
    },
    onFormSubmit: function(e) {
      e.preventDefault();
      // filter is 0-based; form is 1-based
      var formFrom = parseInt(this.$el.find('input[name="from"]').val())-1;
      var formTo = parseInt(this.$el.find('input[name="to"]').val())-1;
      var maxRecord = this.model.recordCount-1;
      if (this.model.queryState.get('from') != formFrom) { // changed from; update from
        this.model.queryState.set({from: Math.min(maxRecord, Math.max(formFrom, 0))});
      } else if (this.model.queryState.get('to') != formTo) { // change to; update size
        var to = Math.min(maxRecord, Math.max(formTo, 0));
        this.model.queryState.set({size: Math.min(maxRecord+1, Math.max(to-formFrom+1, 1))});
      }
    },
    onPaginationUpdate: function(e) {
      e.preventDefault();
      var $el = $(e.target);
      var newFrom = 0;
      var currFrom = this.model.queryState.get('from');
      var size = this.model.queryState.get('size');
      var updateQuery = false;
      if ($el.parent().hasClass('prev')) {
        newFrom = Math.max(currFrom - Math.max(0, size), 0);
        updateQuery = newFrom != currFrom;
      } else {
        newFrom = Math.max(currFrom + size, 0);
        updateQuery = (newFrom < this.model.recordCount);
      }
      if (updateQuery) {
        this.model.queryState.set({from: newFrom});
      }
    },
    render: function() {
      var tmplData = this.model.toJSON();
      var from = parseInt(this.model.queryState.get('from'));
      tmplData.from = from+1;
      tmplData.to = Math.min(from+this.model.queryState.get('size'), this.model.recordCount);
      var templated = Mustache.render(this.template, tmplData);
      this.$el.html(templated);
      return this;
    }
  });
})(jQuery, recline.View.nvd3, window);