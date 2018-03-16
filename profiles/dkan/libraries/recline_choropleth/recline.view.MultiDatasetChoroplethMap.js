/**
 * @file
 * MultiDatasetChoroplethMap recline view implementation.
 */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

(function($, my) {
  "use strict";
  my.MultiDatasetChoroplethMap = my.ChoroplethMap.extend({
    latitudeFieldNames: ['lat', 'latitude', 'ycoord'],
    longitudeFieldNames: ['lon', 'longitude', 'xcoord'],

    /**
     * Constructor.
     * @param {object} options
     *   overrides for defaults.
     */
    initialize: function(options) {
      var self = this;
      this.resources = options.resources ? options.resources : [];
      if (this.resources.length > 0) {
        // Populate options.
        var k = options.activeDataset ? options.activeDataset : self._determineinitialdataset();
        options = _.extend(
          {
            'selectable_fields': [options.resources[k].fieldToDisplay],
            'base_color': options.resources[k].baseColor,
            'breakpoints': options.resources[k].breakpoints ? options.resources[k].breakpoints : [],
            'unit_of_measure': options.resources[k].unitOfMeasure ?  options.resources[k].unitOfMeasure : "",
          },
          options
        );
        options.state = {
          'activeDataset': k,
          'pointLabel' : options.pointLabel,
          'resources': options.resources,
          'activePoints': options.activePoints ? options.activePoints : [],
          'latField': this._checkField(this.latitudeFieldNames, options.resources[k].dataset),
          'lonField': 'Longitude',
        };
        this.model = options.resources[k].dataset;
        this.last_points = '';

        // Call parent constructor.
        my.MultiDatasetChoroplethMap.__super__.initialize.call(this, options);

        // Remove Parent events.
        this.stopListening(this.state, 'change');
        // Listen to changes in View State.
        this.listenTo(this.state, 'change', function() {
          var state = this.state.toJSON();
          if (state['resources'][state['activeDataset']].dataset != this.model) {
            self.breakpoints = state['resources'][state['activeDataset']].breakpoints ? state['resources'][state['activeDataset']].breakpoints : [];

            self.unit_of_measure =  state['resources'][state['activeDataset']].unitOfMeasure ?  state['resources'][state['activeDataset']].unitOfMeasure :'';
            self.model = state['resources'][state['activeDataset']].dataset;
            self.base_color = state['resources'][state['activeDataset']].baseColor;
            self._refreshFieldsMap();
            self.menu.model = self.model;
            self.menu.partialRender();
          }
          self.redraw();
        });
      }
    },
    /**
     * The initial Dataset can't have a map view.
     * @return {int}
     *   The initial Dataset index.
     */
    _determineinitialdataset: function() {
      var i = 0;
      $.each(this.resources, function(k, v) {
        if (!v.map) {
          i = k;
          return false;
        }
      });
      return i;
    },
    /**
     * Creates a L.Marker instance with binded popup.
     * @param {object} feature
     *   the feature object
     * @param {object} latlng
     *   the L.Marker instance
     */
    addNewMarker: function (feature, latlng) {
      var marker = new L.Marker(latlng);
      marker.bindPopup(feature.properties.popupContent);
      return marker;
    },
    /**
     * Check if a field the current model exists in the provided list of names.
     * @param  {array} fieldNames
     *   The list of fieldnames.
     * @param  {object} model
     *   The model to crossreference field names with
     * @return {string}
     *   The matched fieldname
     */
    _checkField: function(fieldNames, model) {
      var modelFieldNames = model.fields.pluck('id');
      for (var i = 0; i < fieldNames.length; i++){
        for (var j = 0; j < modelFieldNames.length; j++){
          if (modelFieldNames[j].toLowerCase() == fieldNames[i].toLowerCase())
            return modelFieldNames[j];
        }
      }
      return null;
    },
    /**
     * Return a GeoJSON Point instance from a recline dataset row.
     * @param  {object} doc
     *   The recline dataset row
     * @return {object}
     *   The GeoJSON Point instance
     */
    _getGeometryFromRecord: function(doc, latField, lonField) {
      var lon = doc.get(lonField);
      var lat = doc.get(latField);
      if (!isNaN(parseFloat(lon)) && !isNaN(parseFloat(lat))) {
        return {
          type: 'Point',
          coordinates: [lat, lon]
        };
      }
      return null;
    },
    /**
     * Builds the infobox for a given row of data.
     * @param  {object} record
     *   A row of data associated with a marker
     * @return {string}
     *   The rendered infobox content
     */
    infobox: function(record) {
      var html = '';
      for (var key in record.attributes){
        if (!(this.state.get('geomField') && key == this.state.get('geomField'))){
          html += '<div><strong>' + key + '</strong>: '+ record.attributes[key] + '</div>';
        }
      }
      return html;
    },
    /**
     * Build the marker cluster for a given dataset
     * @param {object} docs
     *   The recline dataset rows
     * @param {int} resourceNumber
     *   The index for the resource
     */
    _add: function(docs, resourceNumber) {
      var self = this;

      if (!(docs instanceof Array)) docs = [docs];
      var latField = this._checkField(this.latitudeFieldNames, self.options.resources[resourceNumber].dataset);
      var lonField = this._checkField(this.longitudeFieldNames, self.options.resources[resourceNumber].dataset);

      var count = 0;
      var wrongSoFar = 0;
      var layer = [];
      _.every(docs, function(doc){
        count += 1;
        var feature = self._getGeometryFromRecord(doc, latField, lonField);
        if (typeof feature === 'undefined' || feature === null){
          // Empty field
          return true;
        }
        else if (feature instanceof Object) {
          feature.properties = {
            popupContent: self.infobox(doc),
            // Add a reference to the model id, which will allow us to
            // link this Leaflet layer to a Recline doc
            cid: doc.cid
          };

          var lat = numeral().unformat(feature.coordinates[0]);
          var lng = numeral().unformat(feature.coordinates[1]);
          var latLng = new L.LatLng(lat, lng);
          var marker = new L.Marker(latLng);
          marker.bindPopup(feature.properties.popupContent);
          self.points[resourceNumber].addLayer(marker);
        } else {
          wrongSoFar += 1;
          if(wrongSoFar <= 10) {
            self.trigger('recline:flash', {message: 'Wrong geometry value', category:'error'});
          }
        }
        return true;
      });
    },
    /*
     * Helper function to get the proper menu class.
     */
    _menuClass: function() {
      return my.MultiDatasetChoroplethMapMenu;
    },
    /**
     * Redraw active points layer if necessary.
     */
    pointRedraw: function() {
      var self = this;
      self.points = self.points ? self.points : [];
      var previousPoints = self.last_points !== '' ? self.last_points.split(',') : [];
      var activePoints = self.state.get('activePoints');
      if (activePoints) {
        $.each(activePoints, function(k, v) {
          // Make sure this resource wasn't previously displayed.
          if (previousPoints.indexOf(v) == -1 ) {
            // Double check that resource uses a map.
            if (self.resources[v].map) {
              self.points[v] = new L.MarkerClusterGroup({showCoverageOnHover: true});
              self._add(self.resources[v].dataset.records.models, v);
              self.map.addLayer(self.points[v]);
            }
          }
        });
      }

      if (previousPoints.length) {
        $.each(previousPoints, function(k, v) {
          // Make sure this resource isn't actively displayed.
          if (activePoints.indexOf(v) == -1 ) {
            self.map.removeLayer(self.points[v]);
          }
        });
      }
    },
    /**
     * Override ChoroplethMap redraw function to consider point layers.
     */
    redraw: function() {
      var map = this.resources[this.state.get('activeDataset')].map;
      if (!map) {
        my.MultiDatasetChoroplethMap.__super__.redraw.call(this);
      }
      var current_points = this.state.get('activePoints').join(',');

      if (current_points != this.last_points) {
        this.pointRedraw();
      }
      this.last_points = current_points;
    }

  });
  /**
   * Menu class
   */
  my.MultiDatasetChoroplethMapMenu = my.ChoroplethMapMenu.extend({
    template: ' \
            <form class="form-stacked"> \
              <div id="resource-form" class="clearfix" \
                {{{select_resources}}} \
              </div> \
              <div id="base-form" class="clearfix"> \
                {{{field_to_display}}} \
                {{{filter_by_category}}} \
                {{{filter_by_year}}} \
              </div> \
              <div id="points-form" class="clearfix"> \
                {{{filter_by_points}}} \
              </div> \
              <input type="hidden" class="editor-id" value="chroropleth-map-1" /> \
            </form> \
            <label id="unit-of-measure"></label> \
            <div id="color-scale" class="reference"></div> \
    ',
    // Define here events for UI elements.
    events: _.extend(
      my.ChoroplethMapMenu.prototype.events,
      {
        'change .editor-select-resources': 'onSelectResourceChange',
        'change .editor-select-point-data': 'onSelectPointChange',
      }
    ),
    /**
     * Renders check box for resource selection.
     * @return {string}
     *   Rendered resource selection UI.
     */
    _renderResourceCheckbox: function() {
      var self = this;
      var options = [];
      var render = '';
      var resources = this.state.toJSON()['resources'];
      $.each(resources, function(k, v) {
        if (v.map) {
          options.push({name: resources[k].title, index: k});
        }
      });
      // Render only if we have options for the select box.
      if (options.length > 0) {
        render = Mustache.render(
          this.filter_by_checkbox_template,
          {
            css_class: 'select-point-data',
            field_label: self.state.attributes.pointLabel,
            options: options
          }
        );
      }
      return render;
    },
    /**
     * Renders select box for resource selection.
     * @return {string}
     *   Rendered resource selection UI.
     */
    _renderResourcesSelect: function() {
      var self = this;
      var options = [];
      var render = '';
      var resources = this.state.toJSON()['resources'];
      $.each(resources, function(k, v) {
        if (!v.map) {
          options.push(resources[k].title);
        }
      });
      // Render only if we have options for the select box.
      if (options && options.length > 0) {
        render = Mustache.render(
          this.filter_by_select_template,
          {
            css_class: 'select-resources',
            field_label: 'Resources',
            options: options
          }
        );
      }
      return render;
    },
    /**
     * set an select input options as selected
     * @param {string} key
     *   the key to the state item
     */
    _setSelectedResource: function() {
      var k = this.state.get('activeDataset');
      var dataset = this.state.get('resources')[k].title;
      this.$el.find('.editor-select-resources option').each(function(i) {
        if ($(this).val() == dataset) {
          $(this).attr('selected', 'selected');
        }
      });
    },
    /**
     * Event handler for select resource UI.
     * @param  {object} e
     *   an event triggered by a DOM element.
     * @return {boolean}
     *   a boolean to stop event propagation.
     */
    onSelectPointChange: function(e) {
      e.preventDefault();
      var self = this;
      var state = this.state.toJSON();
      // var checkboxes = $(e.target);
      var checkboxes = $('.editor-select-point-data input');

      state.activePoints = [];
      $.each(checkboxes, function(k, v) {
        var id = $(v).attr('id');
        var index = id.split('recline-point-input-');
        if ($(v).is(":checked")) {
          state.activePoints.push(index[1]);
        }
      });
      self.state.set(state);
      $(document).trigger("choropleth-changed");
      return false;
    },
    /**
     * Event handler for select resource UI.
     * @param  {object} e
     *   an event triggered by a DOM element.
     * @return {boolean}
     *   a boolean to stop event propagation.
     */
    onSelectResourceChange: function(e) {
      e.preventDefault();
      var self = this;
      var state = this.state.toJSON();
      var select = $(e.target);
      var which = select.parent().attr('class').replace('input editor-select-', '');
      var value = select.find('option:selected').val();
      $.each(state['resources'], function(k, v) {
        if (state['resources'][k].title === value) {
          var dataset = state['resources'][k].dataset;
          var fieldToDisplay = state['resources'][k].fieldToDisplay.toLowerCase();
          var selectable_columns = self._refreshSelectableColumns(k);
          state['activeDataset'] = k;
          state['selectableColumns'] = fieldToDisplay ? [fieldToDisplay] : selectable_columns;
          state['columnToDisplay'] = fieldToDisplay ? fieldToDisplay : selectable_columns[0];
          state['category'] = my.ChoroplethMap._grabOptionsForColumn(dataset.toTemplateJSON(), 'category', 'categories')[0];
          state['year'] = my.ChoroplethMap._grabOptionsForColumn(dataset.toTemplateJSON(), 'year', 'years')[0];

          self.state.set(state);
          $(document).trigger("choropleth-changed", ['resource', value]);
          return false;
        }
      });
      return false;
    },
    /**
     * Return a list of selectable fields for a given resource.
     * @param {int} i
     *   the index for the resource.
     * @return {array}
     *   an array of selectable fields for that resource.
     */
    _refreshSelectableColumns: function(i) {
      var self = this;
      var selectable_fields = [];
      var state = this.state.toJSON();
      var model = state['resources'][i].dataset.toTemplateJSON();

      var records = model['records'];
      var non_selectables = ['category', 'categories', 'year', 'years', 'latitude', 'longitude'];

      $.each(records[0], function(index, value) {
        value = value.toString().toLowerCase();
        if($.inArray(value, non_selectables) < 0) {
          var n = records[1][index];
          if (!isNaN(parseFloat(n)) && isFinite(n)) {
            selectable_fields.push(value.toLowerCase());
          }
        }
      });
      return selectable_fields;
    },
    /**
     * Renders Full menu.
     */
    render: function() {
      var self = this;
      // Render the complete form.
      var htmls = Mustache.render(
        this.template,
        {
          select_resources: this._renderResourcesSelect(),
          // Render Radio Column.
          field_to_display: this._renderRadioColumn(),
          // Render the year select input.
          filter_by_year: this._renderSelect('year', 'Years'),
          filter_by_points: this._renderResourceCheckbox(),
          // Render the category select input.
          filter_by_category: this._renderSelect('category', 'Categories'),
        }
      );
      // Attach html.
      this.$el.html(htmls);
      // Setting form state based on state object.
      if (this._geomReady && this.model.fields.length) {
        var selectable_columns = this.state.get('selectableColumns');
        if (selectable_columns.length > 1) {
          var column_to_display = this.state.get('columnToDisplay');
          this.$el.find('.editor-column-to-display input:radio').each(function(i) {
            var value = $(this).attr('value');
            if (value === column_to_display) {
              $(this).attr('checked', 'checked');
              return false;
            }
          });
        }
      }
      this._setSelectedOption('year');
      this._setSelectedOption('category');
      this._setSelectedResource();

      // Set Selected Point resources.
      this._setSelectedResourceCheckBox();

      return this;
    },
    /**
     * Re renders base menu when onSelectResourceChange.
     */
    partialRender: function() {
      var self = this;
      // Render the complete form.
      var radio = this._renderRadioColumn();
      var year = this._renderSelect('year', 'Years');
      var category = this._renderSelect('category', 'Categories');
      // Attach html.
      this.$el.find('#base-form').html(radio + year + category);
    },

    /**
     * Sets the resource point based checkboxes to match state.
     */
    _setSelectedResourceCheckBox: function() {
      var self = this;
      var state = this.state.toJSON();
      // Set all the checkboxes for pointbased data to match the current state
      var active_points = state['activePoints'];
      //  Setting controls directly  - CLUNKY - Needs a more solid internal method.
      //BUG  on first load of page with hash... the checkboxes are not available yet .
      this.$el.find('#points-form input[type="checkbox"]').each(function(i){
        i = $(this).attr('id').replace('recline-point-input-', '');
        if ($.inArray(i, state['activePoints']) > -1) {
          $(this).attr('checked', true);
        }
        else {
          $(this).attr('checked', false);
        }

      });
    },
  });
})(jQuery, recline.View);
