/**
 * @file
 * ChoroplethMap recline view implementation.
 */
String.prototype.capitalize = function() {
    return this.charAt(0).toUpperCase() + this.slice(1);
};

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

(function($, my) {
  "use strict";
  my.ChoroplethMap = Backbone.View.extend({
    template: ' \
            <div class="recline-map"> \
                <div class="panel map"></div> \
            </div> \
    ',
    /**
     * Constructor.
     * @param {object} options
     *   overrides for defaults.
     */
    initialize: function(options) {
      var self = this;

      this.rendered = false;
      this.visible = true;
      this.mapReady = false;
      this.options = options || {};
      // This will be the Leaflet L.Map object (setup below).
      this.map = null;

      // Property to hold Geo Data.
      this.polygons = options.polygons;
      this.polygons_layer = null;

      // Property to hold Units of Measure
      this.unit_of_measure = options.unit_of_measure ? options.unit_of_measure : '';

      // Property to set how to handle multiple row instances.
      this.avg = typeof options.avg !== undefined ? options.avg : true;

      // Breakpoints and color scale.
      this.breakpoints = options.breakpoints ? options.breakpoints : [];
      this.breakpoints = this.breakpoints.length > 0 ? this._validateBreakpoints(this.breakpoints) : [];
      this.dynamic_breakpoints = this.breakpoints.length > 0 ? false : true;

      this.base_color = options.base_color ? options.base_color : ['#FFEDA0', '#FEB24C', '#E31A1C', '#800026'];

      // Top Right Info Window.
      this.info = null;

      // Pointer for active Poly.
      this.active_poly = null;

      // Column Label that holds the value to map geojson features.
      this.map_column = options.map_column ? options.map_column.toLowerCase() : '';

      this.geojson_key = options.geojson_key ? options.geojson_key : 'CODE';
      this.geojson_label = options.geojson_label ? options.geojson_label : options.geojson_key;

      // Mapping fields headers with row indexes.
      this.fields_map = [];
      this._refreshFieldsMap();

      // Setting up default field (column) to display.
      var selectable_fields = options.selectable_fields ? options.selectable_fields : [];
      selectable_fields = this._validateSelectableColumns(selectable_fields);

      // If no ocurrances for selectable fields exists
      // build an array from fields that hold numerical data.
      if (!selectable_fields.length) {
        selectable_fields = this._refreshSelectableColumns();
      }

      // Try to grab the category from the options object or set the first one as a default
      var category = options.category ? options.category : '';
      var categories = my.ChoroplethMap._grabOptionsForColumn(this.model.toTemplateJSON(), 'category', 'categories');
      if (categories.length > 0) {
        category = (category !== '') && ($.inArray(category, categories) > -1) ? category : categories[0];
      }

      // Try to grab the year from the options object or set the first one as a default
      var year = options.year ? numeral().unformat(options.year) : '';

      var years = my.ChoroplethMap._grabOptionsForColumn(this.model.toTemplateJSON(), 'year', 'years');
      if (years.length > 0) {
        year = (year !== '') && ($.inArray(year, years) > -1) ? year : years[0];
      }

      // Build recline state.
      var stateData = _.extend(
        {
        'selectableColumns': selectable_fields,
        'columnToDisplay': selectable_fields[0],
        'category': category,
        'year': year,
        },
        options.state ? options.state : {}
      );
      this.state = new recline.Model.ObjectState(stateData);

      // Setup Menu and update state.
      var MenuClass = this._menuClass();
      this.menu = new MenuClass({
        model: this.model,
        state: this.state.toJSON(),
        label_to_map: this.map_column,
      });

      // Listen to changes in the fields.
      this.listenTo(this.model.fields, 'change', function() {
        self.render();
      });

      // Listen to changes in the records.
      this.listenTo(this.model.records, 'add', function(doc) {
        self.redraw();
      });
      this.listenTo(this.model.records, 'change', function(doc) {
        self.redraw();
      });
      this.listenTo(this.model.records, 'remove', function(doc) {
        self.redraw();
      });
      this.listenTo(this.model.records, 'reset', function() {
        self.redraw();
      });

      // Listen to changes in the Menu and update View State.
      this.listenTo(this.menu.state, 'change', function() {
        self.state.set(self.menu.state.toJSON());
      });
      this.elSidebar = this.menu.$el;
      // Listen to changes in View State.
      this.listenTo(this.state, 'change', function() {
        if (self.dynamic_breakpoints) {
          self.breakpoints = [];
        }
        self.redraw();
      });
    },
    /**
     * Validates pickable columns as sources to color the polygons.
     * @param {array} columns
     *   array of column names to validates.
     * @return {array}
     *   validated array of column names.
     */
    _validateSelectableColumns: function(columns) {
      var self = this;
      // Validating selectable fields.
      var selectable_fields = [];
      $.each(columns, function(i, v) {
        v = v.toLowerCase();
        if ($.inArray(v, self.fields_map) > -1) {
          selectable_fields.push(v);
        }
      });
      return selectable_fields;
    },
    /**
     * Validates if an array of breakpoints hold numeric values
     * @param  {array} breakpoints
     *   The array to validate
     * @return {array}
     *   The validated array
     */
    _validateBreakpoints: function(breakpoints) {
      var self = this;
      $.each(breakpoints, function(i, v){
        var n = v;
        if (self._isPercentage(n)) {
          n = self._preparePercentage(n);
        }
        if (!isNaN(parseFloat(n)) && isFinite(n)) {
          breakpoints[i] = n;
        }
        else {
          breakpoints = [];
          return false;
        }
      });
      return breakpoints;
    },
    /**
     * Check if a given string is a percentage
     * @param  {int|string}  n
     *   The string|int to check
     * @return {boolean}
     *   Boolean representing if the string is a percentage
     */
    _isPercentage: function(n) {
      if (n) {
        n = n.toString().trim();
        if (n.substr(n.length - 1) === '%') {
          return true;
        }
      }
      return false;
    },
    /**
     * Remove the % from a percentage number
     * @param  {string|int} n
     *
     * @return {[type]}   [description]
     */
    _preparePercentage: function(n) {
      if (n) {
        n = n.toString().trim();
        if (this._isPercentage(n)) {
          n = n.substr(0, n.length - 1);
        }
        return numeral().unformat(n);
      }
      if (this._isPercentage(n)) {
        n = n.substr(0, n.length - 1);
      }
      return numeral().unformat(n);
    },
    /**
     * Inspect the model and return an array of selectable columns.
     * @return {array}
     *   a selection of column names that contain numerical data.
     */
    _refreshSelectableColumns: function() {
      var self = this;
      var selectable_fields = [];
      var records = this.model.toJSON().records;
      var non_selectables = ['category', 'categories', 'year', 'years', 'latitude', 'longitude'];
      $.each(self.fields_map, function(index, value) {
        value = value.toString().toLowerCase();
        if($.inArray(value, non_selectables) < 0) {
          var n = records[1][index];
          if (self._isPercentage(n)) {
            n = self._preparePercentage(n);
          }
          if (!isNaN(parseFloat(n)) && isFinite(n)) {
            selectable_fields.push(value);
          }
        }

      });
      return selectable_fields;
    },
    /**
     * Refresh the array that hold the column names for the model.
     */
    _refreshFieldsMap: function() {
      var self = this;
      var fields = this.model.toTemplateJSON().fields;
      this.fields_map = [];
      $.each(fields, function(index, value) {
        self.fields_map.push(value.id.toLowerCase());
      });
    },
    /*
     * Helper function to get the proper menu class.
     */
    _menuClass: function() {
      return my.ChoroplethMapMenu;
    },
    /**
     * Renders the whole view.
     */
    render: function() {
      var self = this;
      var htmls = Mustache.render(this.template, this.model.toTemplateJSON());
      this.$el.html(htmls);
      this.$map = this.$el.find('.panel.map');
      this.redraw();
      return this;
    },
    /**
     * Behaviour for redrawing the display (map).
     */
    redraw: function() {
      // try to set things up if not already.
      if (!this.mapReady) {
        this._setupMap();
      }
      if (this.mapReady) {
        if (this.polygons_layer) {
          this.map.removeLayer(this.polygons_layer);
        }
        this._redrawPolygons();
        this._zoomToPolygons();
      }
    },
    /**
     * Show behaviour (multiview integration).
     */
    show: function() {
      // If the div was hidden, Leaflet needs to recalculate
      // some sizes to display properly.
      if (this.map) {
        this.map.invalidateSize();
        if (this._zoomPending && this.state.get('autoZoom')) {
          this._zoomToPolygons();
          this._zoomPending = false;
        }
      }
      this.visible = true;
      // Hide the Multiview Controls we don't need when this map is visible.
      var explorer = window.dataExplorer;
      $('.' + window.dataExplorer.pager.$el.attr('class')).hide();
      $('.' + explorer.queryEditor.$el.attr('class')).hide();
      $('.menu-right').hide();
    },
    /**
     * Hide behaviour (multiview integration).
     */
    hide: function() {
      this.visible = false;
      // Show the Multiview Controls we hide when this map was visible.
      var explorer = window.dataExplorer;
      $('.' + window.dataExplorer.pager.$el.attr('class')).show();
      $('.' + explorer.queryEditor.$el.attr('class')).show();
      $('.menu-right').show();
    },
    /**
     * Zoom map to preset bounds.
     */
    _zoomToPolygons: function() {
      if (this.options.location_default) {
        this.map.setView(
          [
            this.options.location_default.lat,
            this.options.location_default.lon
          ],
          this.options.location_default.zoom
        );
      }
      else {
        var layers = new L.GeoJSON(this.polygons);
        this.map.zoomToGeometries(layers);
      }
    },
    /**
     * Redraw the colored polygons.
     */
    _redrawPolygons: function() {
      var self = this;
      var state = self.state.toJSON();
      var field_index = self.fields_map.indexOf(state['columnToDisplay']);
      var fields = self.model.toTemplateJSON().fields;
      var records = self.model.toJSON().records;

      // Fallback when there's no predefined breakpoints.
      if (!this.breakpoints.length) {
        // Building default breakpoints distribution.
        var default_bps = [];
        for (var i = 0; i < 8; i++) {
          default_bps.push(1 * Math.pow(10, i));
          default_bps.push(2 * Math.pow(10, i));
          default_bps.push(5 * Math.pow(10, i));
        }

        var max = 0;
        var min = 0;
        var levels = 7;

        for (i = 1; i < records.length; i++) {
          var value = records[i];
          // convert number to make sure string with comas don't break functionality.
          var value_at_index = self._preparePercentage(value[field_index]);

          if (value_at_index !== null) {
            if (value_at_index > max || max === 0) {
              max = value_at_index;
            }
            if (value_at_index < min || min === 0) {
              min = value_at_index;
            }
          }
        }

        // Adding actual breakpoints given the min-max values of
        // the recline dataset records.
        $.each(default_bps, function(i, v) {
          if (v >= min && v <= max) {
            self.breakpoints.push(v);
          }
        });
      }

      // Updating color references.
      this.menu.updateColorScale(self.breakpoints, self.base_color, self.unit_of_measure);

      // Overlay geometry from statesData and add shading.
      this.polygons_layer = new L.GeoJSON(
        this.polygons,
        {
          style: function(feature) {
            var d = 0;
            var sum = 0;
            var n = 0;
            // Find all rows that match the criteria first.
            var filtered_records = self._filteredRecordsForPoly(feature.properties[self.geojson_key]);
            $.each(filtered_records, function(key, value) {
              // make sure string with comas don't break functionality.
              if (value[field_index] !== null) {
                sum += self._preparePercentage(value[field_index]);
                n++;
              }
            });

            if (n) {
              n = self.avg ? n : 1;
              d = sum/n;
            }

            var color_scale = chroma.scale(self.base_color);
            var scale = 0;

            for (var j = 0; j < self.breakpoints.length; j++) {
              if (d < self.breakpoints[j]) {
                break;
              }

            }
            scale = (j).toFixed(2) / (self.breakpoints.length).toFixed(2);

            return {
              fillColor: color_scale(scale).hex(),
              weight: 2,
              opacity: 1,
              color: 'white',
              dashArray: '3',
              fillOpacity: 0.9
            };
          },
          onEachFeature: function(feature, layer) {
            layer.on({
              mouseover: function(e) {
                var layer = e.target;
                layer.setStyle({
                  weight: 5,
                  color: '#666',
                  dashArray: '',
                  fillOpacity: 0.7
                });
                if (!L.Browser.ie && !L.Browser.opera) {
                  layer.bringToFront();
                }
                self.info.update(self._infoContent(feature));
              },
              mouseout: function(e) {
                self.polygons_layer.resetStyle(e.target);
                self.info.update();
              },
              click: function(e) {
                var poly = e.target;
                if (poly != self.active_poly) {
                  var bounds = poly.getBounds();
                  var popup_content = self._popupContent(feature);
                  if (popup_content) {
                    var popup = L.popup();
                    popup.options.closeButton = false;
                    popup.setLatLng(bounds.getCenter())
                         .setContent(popup_content)
                         .openOn(self.map);
                  }
                  self.active_poly = poly;

                }
                else {
                  self.map.closePopup();
                  self._zoomToPolygons();
                  self.active_poly = null;
                }
              },
            });
          },
        });
      this.polygons_layer.addTo(this.map);
    },
    /**
     * Map and Controls setup.
     */
    _setupMap: function() {
      var self = this;
      this.map = new L.Map(this.$map.get(0));

      var mapUrl = "//otile{s}-s.mqcdn.com/tiles/1.0.0/osm/{z}/{x}/{y}.png";
      var osmAttribution = 'Map data &copy; 2011 OpenStreetMap contributors, Tiles Courtesy of <a href="http://www.mapquest.com/" target="_blank">MapQuest</a> <img src="//developer.mapquest.com/content/osm/mq_logo.png">';

      var bg = new L.TileLayer(mapUrl, {maxZoom: 18, attribution: osmAttribution ,subdomains: '1234'});
      this.map.addLayer(bg);

      // Create control to hold poly data.
      var InfoControl = L.Control.extend({
          options: {
              position: 'topright'
          },
          onAdd: function(map) {
              this._div = L.DomUtil.create('div', 'info');
              this.update();
              return this._div;
          },
          update: function(value) {
              this._div.innerHTML = value ? value : 'Hover over a ' + self.map_column.capitalize();
          },
      });
      this.info = new InfoControl();
      this.map.addControl(this.info);
      // Set flag.
      this.mapReady = true;
    },
    /**
     * Provides the content for the top-right control.
     * @params {object} feature
     *   A geojson feature currently to be displayed on the map.
     */
    _infoContent: function(feature) {
      var self = this;
      var state = self.state.toJSON();
      var field_index = self.fields_map.indexOf(state['columnToDisplay']);
      var filtered_records = self._filteredRecordsForPoly(feature.properties[self.geojson_key]);
      var units = state['columnToDisplay'];
      var v = 0;
      var n = filtered_records.length;
      var template = ' \
                    <b>{{name}}</b> \
                    <br/> \
                    {{value}} {{units}} \
      ';
      $.each(filtered_records, function(key, value) {
        // We'll asume every row has the same value.
        if (!units && units_index > -1) {
          //units = value[units_index];
        }
        else {
          //units = self.unit_of_measure;
        }
        v += self._preparePercentage(value[field_index]);
      });
      if (n > 0) {
        n = self.avg ? n : 1;
        v = v / n;
        v = v.toFixed(2);
        v = parseFloat(v, 10);
      }

      return Mustache.render(
        template,
        {
          name: feature.properties[self.geojson_label],
          value: v,
          units: units,
        }
      );
    },
    /**
     * Provides the content for each popup.
     * @params {object} feature
     *   A geojson feature currently to be displayed on the map.
     */
    _popupContent: function(feature) {
      var self = this;
      var filtered_records = self._filteredRecordsForPoly(feature.properties.CODE);
      var popup_text = '';
      var popup_url = '';
      var popup_link_text = '';
      //
      var template = ' \
                  <div class="popup_text"> \
                    {{popup_text}} \
                  </div> \
                  <div> \
                    <a href="{{popup_url}}" target="_blank" title="Link to additional information opens in a new window."> \
                      {{popup_link_text}} \
                    </a> \
                  </div> \
      ';
      if (filtered_records.length === 1) {
        $.each(filtered_records, function(key, value) {
          // Check for field popup_text.
          popup_text = value[self._indexForColumn('popup text')];
          // Process line breaks in to html break if there is popup text.
          if (!!popup_text) {
            popup_text = popup_text.replace(/(\r\n|\n|\r)/g,"<br />");
          }

          // Check for field popup_link.
          popup_url = value[self._indexForColumn('popup link')];
          if (popup_url) {
            popup_url = self._processUrl(popup_url);
            // Check for available Link Text.
            popup_link_text = value[self._indexForColumn('popup link text')];
          }
        });
      }
      if (popup_text || popup_url) {
        return Mustache.render(
          template,
          {
            popup_text: popup_text ? popup_text : '',
            popup_url: popup_url ? popup_url : '',
            popup_link_text: popup_link_text ? popup_link_text : popup_url,
          }
        );
      }
      return '';
    },
    /**
     * Helper function to run a filter operation.
     * @param {string} poly_name
     *   A string to map a polygon through its name.
     */
    _filteredRecordsForPoly: function(poly_name) {
      var self = this;
      var state = this.state.toJSON();
      var records = self.model.toJSON().records;
      // Helper variables to keep things tidy.
      var label_index = self.fields_map.indexOf(self.map_column);
      var field_index = self.fields_map.indexOf(state['columnToDisplay']);
      var category_index = self._indexForColumn('category');
      var year_index = self._indexForColumn('year');
      var category = state['category'];
      var year = parseInt(state['year'], 10);
      var filtered_records = [];
      $.each(records, function(i, record) {
        if (i > 0) {
          var match_state = record[label_index] === poly_name;
          var match_category = category_index > -1 && category ? record[category_index] === category : true;
          var match_year = year_index > -1 && year ? record[year_index] === year : true;
          if (match_state && match_category && match_year) {
            filtered_records.push(record);
          }
        }
      });
      return filtered_records;
    },
    /**
     * Helper function to sanitise a url.
     * @param {string} url
     *   The incoming url in need of sanitising.
     * @returns {string}
     *   The sanitized url.
     */
    _processUrl: function(uri) {
      // Sanitize the url.
      var url = uri.toLowerCase();
      url = url.trim();
      // Since this data might feed other sites, all links provided must be absolute.
      // Verify that url has some of the necessary parts. (http:// or https:// or ftp://)
      if (url.indexOf("http://") === 0 || url.indexOf("https://") === 0 || url.indexOf("ftp://") === 0) {
        // This has a proper header, but leave this open for additional tests.

      }
      else {
        // Attempt to fix the url by appending http:// and hope for the best.
        url = 'http://' + url;
      }

      return url;
    },
    /**
     * Helper function to retrieve an index for a column in recline's dataset.
     * @param {string} column
     *   The key representing the column.
     */
    _indexForColumn: function(column) {
      var headers = this.model.toJSON().records[0];
      $.each(headers, function(i, v) {
        headers[i] = v.toString().toLowerCase();
      });
      return $.inArray(column.toLowerCase(), headers);
    }
  },
  {
    /**
     * Return option values for a given set of column names (in singular and plural)
     * @param  {object} model
     *   json representation of a recline model fields and records
     * @param  {string} column_singular
     *   singular for a column name
     * @param  {string} column_plural
     *   plural for a column name
     * @return {array}
     *   an array of unique values from the model records column
     */
    _grabOptionsForColumn: function(model, column_singular, column_plural) {
      var records = model.records;
      var column_matches = [column_singular.toLowerCase(), column_plural.toLowerCase()];
      var options = [];
      // Try to match the column.
      $.each(model.fields, function(key, field) {
        var label = model.fields[key].label.toLowerCase();
        if ($.inArray(label, column_matches) > -1) {
          // Populate options for select box.
          for (var i = 1; i < records.length; i++) {
            var value = records[i][key];
            if (value !== null && $.inArray(value, options) < 0) {
              options.push(value);
            }
          }
          // If the column contains years, reverse sort it (present -> past).
          if ($.inArray('year', column_matches) > -1) {
            options.reverse();
          }
          return false;
        }
      });
      return options;
    },
  }
  );

  /**
   * Menu class.
   */
  my.ChoroplethMapMenu = Backbone.View.extend({
    className: 'editor',
    template: ' \
            </div> \
            <form class="form-stacked"> \
                <div class="clearfix"> \
                    {{{field_to_display}}} \
                    {{{filter_by_category}}} \
                    {{{filter_by_year}}} \
                </div> \
                <input type="hidden" class="editor-id" value="chroropleth-map-1" /> \
            </form> \
            <div id="color-scale" class="reference"></div> \
            ',
        // Radio input template for active column.
        field_to_display_template: '\
                          <div class="editor-column-to-display"> \
                              <label>{{mapDataByLabel}}</label> \
                              {{#fields}} \
                              <label class="radio"> \
                                  <input type="radio" name="radio-column-to-display" id="radio-{{id}}" value="{{value}}"> \
                                  {{label}} \
                              </label> \
                              {{/fields}} \
                          </div> \
    ',
    // Select input template.
    filter_by_select_template: '\
                      <label>{{field_label}}</label> \
                      <div class="input editor-{{css_class}}"> \
                        <select> \
                        {{#options}} \
                        <option value="{{.}}">{{.}}</option> \
                        {{/options}} \
                        </select> \
                      </div> \
    ',
    // Checkbox input template.
    filter_by_checkbox_template: '\
                      <label>{{field_label}}</label> \
                      <div class="input editor-{{css_class}}"> \
                        <ul> \
                        {{#options}} \
                          <li><input type="checkbox" name="{{name}}" value="{{name}}" id="recline-point-input-{{index}}"> {{name}}</li> \
                        {{/options}} \
                        </li> \
                      </div> \
    ',
    // Define here events for UI elements.
    events: {
      'change .editor-column-to-display': 'onRadioColumnChange',
      'change .editor-select-year': 'onSelectChange',
      'change .editor-select-category': 'onSelectChange',
    },
    /**
     * Constructor.
     * @param {object} options
     *   overrides for defaults.
     */
    initialize: function(options) {
      var self = this;
      _.bindAll(this, 'render');
      this.listenTo(this.model.fields, 'change', this.render);
      this.state = new recline.Model.ObjectState(options.state);
      this.render();
    },
    /**
     * Renders the menu.
     */
    render: function() {
      var self = this;
      // Render the complete form.
      var htmls = Mustache.render(
        this.template,
        {
          // Render Radio Column.
          field_to_display: this._renderRadioColumn(),
          // Render the year select input.
          filter_by_year: this._renderSelect('year', 'Years'),
          // Render the category select input.
          filter_by_category: this._renderSelect('category', 'Categories'),
        }
      );
      // Attach html.
      this.$el.html(htmls);
      // Setting form state based on menu.state
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

      return this;
    },
    /**
     * Renders UI to select which fields sets the polygon coloring
     * only if there's more than one specified column.
     */
    _renderRadioColumn: function() {
      var render = '';
      var selectable_columns = this.state.toJSON()['selectableColumns'];
      var mapDataByLabel = Drupal.t('Map data by');

      // Only render the fields radio buttons
      // if there's more than one selectable column.
      if (selectable_columns.length > 1) {
        $.each(selectable_columns, function(i, v){
          selectable_columns[i] = {
            'id': i,
            'value': v,
            'label': v,
          };
        });
        render = Mustache.render(
          this.field_to_display_template,
          {
            'mapDataByLabel': mapDataByLabel,
            'fields': selectable_columns
          }
        );
      }
      this._setSelectedOption('year');
      this._setSelectedOption('category');
      return render;
    },
    /**
     * Renders select box for both category and year filters.
     * @param {string} field_key
     *   Key corresponding to one element of the dataset's header.
     * @param {string} field_label
     *   Label to display to the user.
     */
    _renderSelect: function(field_key, field_label) {
      var render = '';
      var options = my.ChoroplethMap._grabOptionsForColumn(this.model.toTemplateJSON(), field_key, field_label);
      // Render only if we have options for the select box.
      if (options.length > 0) {
        render = Mustache.render(
          this.filter_by_select_template,
          {
            css_class: 'select-' + field_key,
            field_label: field_label,
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
    _setSelectedOption: function(key) {
      var state_value = this.state.get(key);
      if (state_value !== '') {
        this.$el.find('.editor-select-' + key + ' option').each(function(i) {
          if ($(this).val() == state_value) {
            $(this).attr('selected', 'selected');
          }
        });
      }
    },
    /**
     * Gets called to make sure everything is in place and set
     * for backbone's event triggering and capturing.
     */
    _geomReady: function() {
      return Boolean(this.state.get('columnToDisplay'));
    },
    /**
     * Trigger Function that changes menu.state when the
     * editor-column-to-display radios change state.
     * @param  {object} e
     *   an event triggered by a DOM element.
     * @return {boolean}
     *   a boolean to stop event propagation.
     */
    onRadioColumnChange: function(e) {
      e.preventDefault();
      var state = this.state.toJSON();
      var viewing = this.state.get('columnToDisplay');
      var column_to_display = this.$el.find('.editor-column-to-display input:checked').attr('value');
      state['columnToDisplay'] = column_to_display;
      this.state.set(state);
      return false;
    },
    /**
     * Trigger Function that changes menu.state when the
     * year or category select boxes changes.
     * @param  {object} e
     *   an event triggered by a DOM element.
     * @return {boolean}
     *   a boolean to stop event propagation.
     */
    onSelectChange: function(e) {
      var state = this.state.toJSON();
      var select = $(e.target);
      var which = select.parent().attr('class').replace('input editor-select-', '');
      var value = select.find('option:selected').val();
      state[which] = value;
      this.state.set(state);
      $(document).trigger("choropleth-changed", [which, value]);
      return false;
    },
    /**
     * Helper function to select an option from a select list.
     * @param  {string} id
     *   a string that acts as a selector for a select element.
     * @param  {string} value
     *   the option value to mark as selected.
     */
    _selectOption: function(id, value) {
      var options = this.$el.find('.' + id + ' > select > option');
      if (options) {
        options.each(function(opt) {
          if (this.value == value) {
            $(this).attr('selected', 'selected');
            return false;
          }
        });
      }
    },
    /**
     * Updates color scale legend on menu
     * @param  {array} breakpoints
     *   an array of breakpoints for the color scale
     * @param  {array} colors
     *   an array of color to build the color scale
     * @param  {string} unit_of_measure
     *   a string containing units for the color scale
     */
    updateColorScale: function(breakpoints, colors, unit_of_measure) {
      var color_scale = chroma.scale(colors);
      var html = '';
      var temp = '';

      $.each(breakpoints, function(i, v) {
        var scale = (i).toFixed(2) / (breakpoints.length).toFixed(2);
        temp = '<i style="background:' + color_scale(scale).hex() + '"></i>';
        temp += (i === 0 ? '0' : breakpoints[i - 1]) + '&ndash;' + breakpoints[i] + '<br />';
        html = temp + html;
      });
      temp = '<i style="background:' + color_scale(1).hex() + '"></i>';
      temp += breakpoints[breakpoints.length - 1] + '+<br />';
      temp =  unit_of_measure + '</br>' + temp;
      this.$el.find('#color-scale').html(temp + html);
    }
  });
})(jQuery, recline.View);
