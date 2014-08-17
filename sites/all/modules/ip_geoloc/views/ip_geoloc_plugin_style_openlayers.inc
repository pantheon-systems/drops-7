<?php

/**
 * @file
 * ip_geoloc_plugin_style_openlayers.inc
 *
 * Views Style plugin extension for OpenLayers (if enabled).
 */

require_once 'ip_geoloc_plugin_style.inc';

class ip_geoloc_plugin_style_openlayers extends views_plugin_style {

  /**
   * Set default OpenLayer options.
   */
  public function option_definition() {
    $options = parent::option_definition();

    $latitude  = module_exists('location') ? 'location_latitude' : 'ip_geoloc_latitude';
    // For field-based modules.
    $longitude = module_exists('location') ? 'location_longitude' : ($latitude == 'ip_geoloc_latitude' ? 'ip_geoloc_longitude' : $latitude);
    $options['ip_geoloc_views_plugin_latitude'] = array('default' => $latitude);
    $options['ip_geoloc_views_plugin_longitude'] = array('default' => $longitude);

    $options['differentiator'] = array(
      'contains' => array(
        'differentiator_field' => array('default' => ''),
      ),
    );
    $options['default_marker_color'] = array('default' => '');
    $options['center_option'] = array('default' => IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION);
    $options['argument'] = array('default' => '');

    return $options;
  }

  /**
   * Create the options form.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['map'] = array(
      '#type' => 'select',
      '#title' => t('Map'),
      '#description' => t('The OpenLayers map used to place the view locations on. You can configure map and markers <a href="!url">here</a>.', array(
        '!url' => url('admin/structure/openlayers/maps'),
      )),
      '#options' => openlayers_map_options(),
      '#default_value' => isset($this->options['map']) ? $this->options['map'] : variable_get('openlayers_default_map', 'default'),
    );

    $form_state['renderer'] = 'openlayers';
    ip_geoloc_plugin_style_bulk_of_form($this, $form, $form_state);
    // OpenLayers has "Hide empty map for Views" for this.
    unset($form['empty_map_center']);

    $form['center_option']['#options'][IP_GEOLOC_MAP_CENTER_ON_LOCATION_FROM_ARGUMENT] = t('Use lat/lon coordinate arguments from a contextual filter');
    $argument_handlers = $this->view->display_handler->get_handlers('argument');
    if (!empty($argument_handlers)) {
      $argument_options = array('' => '<' . t('none') . '>');
      foreach ($argument_handlers as $key => $handler) {
        $argument_options[$key] = $handler->definition['group'] . ': ' . $handler->definition['title'];
      }
      $form['argument'] = array(
        '#type' => 'select',
        '#title' => t('Choose a contextual filter to pull data from'),
        '#options' => $argument_options,
        '#default_value' => $this->options['argument'],
        '#weight' => 100,
      );
    }
  }

  /**
   * Validate the options form.
   */
  public function options_validate(&$form, &$form_state) {
    ip_geoloc_plugin_style_bulk_of_form_validate($form, $form_state);
  }

  /**
   * Transform the View result in a list of markers and render these on a map.
   */
  public function render() {

    // This somewhat mimics function openlayers_render_map_data() and class
    // openlayers_views_style_map, function render()
    $map = openlayers_map_load(isset($this->options['map']) ? $this->options['map'] : variable_get('openlayers_default_map', 'default'));

    // Function openlayers_build_map() invokes:
    // - hook_openlayers_map_preprocess_alter(&$map), which introduces the names
    //   of the layers we'll use: IP_GEOLOC_MARKER_LAYER_1..3 and
    //   IP_GEOLOC_VISITOR_MARKER_LAYER
    // - hook_openlayers_layers(), which creates barebone layers for the above
    // - hook_openlayers_map_alter(&$map), for modules to make final changes to
    //   the map and its layers.
    $map = openlayers_build_map($map->data);

    // Return themed map if no errors found.
    if (empty($map['errors'])) {

      ip_geoloc_plugin_style_render_fields($this);
      $locations = ip_geoloc_plugin_style_extract_locations($this);

      $center_option = !isset($this->options['center_option']) ? IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION : $this->options['center_option'];
      $visitor_location = ip_geoloc_get_visitor_location();
      if (!isset($visitor_location['longitude'])) {
        $visitor_location = db_query('SELECT * FROM {ip_geoloc} WHERE ip_address = :ip_address', array(':ip_address' => ip_address()))->fetchAssoc();
      }
      if ($center_option == IP_GEOLOC_MAP_CENTER_ON_VISITOR && $center_set = isset($visitor_location['longitude'])) {
        $longitude = $visitor_location['longitude'];
        $latitude  = $visitor_location['latitude'];
        $map['center']['initial']['centerpoint'] = "$longitude,$latitude";
      }
      elseif (($center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS || $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED) && !empty($locations)) {
        list($latitude, $longitude) = ip_geoloc_center_of_locations($locations, $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED);
        $map['center']['initial']['centerpoint'] = "$longitude,$latitude";
        $center_set = TRUE;
      }
      elseif ($center_option == IP_GEOLOC_MAP_CENTER_ON_LOCATION_FROM_ARGUMENT) {
        if ($handler = $this->view->display_handler->get_handler('argument', $this->options['argument'])) {
          $latitude = $handler->value['lat'];
          $longitude = $handler->value['lon'];
          $map['center']['initial']['centerpoint'] = "$longitude,$latitude";
          $center_set = TRUE;
        }
      }
      foreach ($locations as $location) {
        if (isset($location->lon)) {
          $lon = $location->lon;
          $lat = $location->lat;
        }
        elseif (isset($location->longitude)) {
          $lon = $location->longitude;
          $lat = $location->latitude;
        }
        else {
          continue;
        }
        if (empty($center_set) && $center_option == IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION) {
          $map['center']['initial']['centerpoint'] = "$lon,$lat";
          $center_set = TRUE;
        }
        $feature = array(
          // Only 'name' and 'description' are valid attribute names.
          // @see openlayers_behavior_popup.js
          'attributes' => array('description' => $location->balloon_text),
          'projection' => 'EPSG:4326',
          'wkt' => "POINT($lon $lat)",
        );
        $layer = isset($location->marker_color) && is_numeric($location->marker_color) ? $location->marker_color : 1;
        $map['layers'][IP_GEOLOC_MARKER_LAYER . $layer]['features'][] = $feature;
      }

      // See admin/structure/openlayers/maps/<map-name>/edit...
      // If "Hide empty map for Views" is ticked, then if the map has no
      // features, it should not be displayed.
      if (!empty($map['hide_empty_map'])) {
        $num_location_marker_layers = variable_get('ip_geoloc_num_location_marker_layers', IP_GEOLOC_DEF_NUM_MARKER_LAYERS);
        for ($layer = 1; $layer <= $num_location_marker_layers; $layer++) {
          if (!empty($map['layers'][IP_GEOLOC_MARKER_LAYER . $layer]['features'])) {
            $some_location_layers = TRUE;
            break;
          }
        }
        if (empty($some_location_layers) && empty($map['layers'][IP_GEOLOC_VISITOR_MARKER_LAYER]['features'])) {
          // Nothing to show.
          return '';
        }
      }

      $js = array('openlayers' => array('maps' => array($map['id'] => $map)));
      drupal_add_js($js, 'setting');

      // Unlike openlayers.theme.inc, theme_openlayers_map(), we use a template.
      //
      // To ensure that any controls are on the map correctly, we need to
      // wrap the map in a container, and take into account %-dimensions
      $container_width = $map['width'];
      $container_height = $map['height'];
      $map['width']  = (strpos($map['width'], '%') > 0) ? '100%' : $map['width'];
      $map['height'] = (strpos($map['height'], '%') > 0) ? '100%' : $map['height'];

      // Uses ip-geoloc-openlayers.tpl.php.
      $output = theme(array('ip_geoloc_openlayers'), array(
        'view' => $this->view,
        // 'options' => $this->options,
        'map' => $map,
        'container_width' => $container_width,
        'container_height' => $container_height,
      ));
    }
    else {
      $output = implode('<br/>', $map['errors']);
    }
    return $output;
  }
}
