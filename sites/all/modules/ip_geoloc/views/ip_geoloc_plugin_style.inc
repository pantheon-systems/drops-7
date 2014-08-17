<?php

/**
 * @file
 * ip_geoloc_plugin_style.inc
 *
 * Helper functions for Views style plugins, in particular to extract location
 * data (lat/long columns) from the associated view.
 *
 * @todo turn this into a common base clase for ip_geoloc_style_*.inc.
 */

// Leaflet only.
define('IP_GEOLOC_MAP_AUTOBOX_IGNORE_VISITOR', -1);

define('IP_GEOLOC_MAP_CENTER_FIXED', 0);
define('IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION', 1);
define('IP_GEOLOC_MAP_CENTER_ON_VISITOR', 2);
define('IP_GEOLOC_MAP_CENTER_OF_LOCATIONS', 3);
define('IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED', 4);
define('IP_GEOLOC_MAP_CENTER_ON_LOCATION_FROM_ARGUMENT', 5);

/**
 * The bulk of the plugin style form.
 */
function ip_geoloc_plugin_style_bulk_of_form($views_plugin_style, &$form, &$form_state) {

  // We only ever want to offer at most 1 grouping field.
  if (!empty($form['grouping']) && count($form['grouping']) > 1) {
    $form['grouping'] = array_slice($form['grouping'], 0, 1);
  }

  $fields = ip_geoloc_get_display_fields($views_plugin_style->display->handler, TRUE);

  $field_options = $fields;
  $field_options['---'] = '[' . t('Type a field name') . ']';
  $selected_option = $views_plugin_style->options['ip_geoloc_views_plugin_latitude'];
  if (!isset($field_options[$selected_option])) {
    $selected_option = '---';
  }
  $form['ip_geoloc_views_plugin_latitude'] = array(
    '#title' => t('Name of latitude field in Views query'),
    '#type' => 'select',
    '#options' => $field_options,
    '#default_value' => $selected_option,
    '#weight' => 10,
  );
  $form['ip_geoloc_views_plugin_latitude_text'] = array(
    '#type' => 'textfield',
    '#default_value' => $views_plugin_style->options['ip_geoloc_views_plugin_latitude'],
    '#states' => array(
      'visible' => array(
        ':input[name="style_options[ip_geoloc_views_plugin_latitude]"]' => array('value' => '---'),
      ),
    ),
    '#weight' => 11,
  );
  $form['ip_geoloc_views_plugin_latitude_desc'] = array(
    '#type' => 'item',
    '#markup' => t('Select the field holding the latitude from the dropdown above. Make sure you have added it to the Fields list of this View or it won\'t show up above.<br/><strong>Special cases</strong> If the View has relationships and lat/lon are accessed via the relationships, select <em>[Type field name]</em> and enter the field name in the additional text box presented. If you use a view based on the <strong>Location</strong> module, enter <strong>location_latitude</strong>. If the latitude is stored in a <strong>field</strong>, then enter the field\'s machine name. For instance, when using the <strong>Geofield</strong>, <strong>Geolocation field</strong> or <strong>Get Locations</strong> modules enter <strong>field_<em>yourfieldname</em></strong>. If necessary, check the <a target="_field_names" href="!url">list of all field names</a>. <br/>For IP Geolocation Views & Maps <em>visitor</em> data, enter <strong>ip_geoloc_latitude</strong>.', array(
       '!url' => url('admin/reports/fields'),
    )),
    '#weight' => 12,
  );
  $selected_option = $views_plugin_style->options['ip_geoloc_views_plugin_longitude'];
  if (!isset($field_options[$selected_option])) {
    $selected_option = '---';
  }
  $form['ip_geoloc_views_plugin_longitude'] = array(
    '#title' => t('Name of longitude field in Views query'),
    '#type' => 'select',
    '#options' => $field_options,
    '#default_value' => $selected_option,
    '#weight' => 15,
  );
  $form['ip_geoloc_views_plugin_longitude_text'] = array(
    '#type' => 'textfield',
    '#default_value' => $views_plugin_style->options['ip_geoloc_views_plugin_longitude'],
    '#states' => array(
      'visible' => array(
        ':input[name="style_options[ip_geoloc_views_plugin_longitude]"]' => array('value' => '---'),
      ),
    ),
    '#weight' => 16,
  );
  $form['ip_geoloc_views_plugin_longitude_desc'] = array(
    '#type' => 'item',
    '#markup' => t('See comments above. When using the <strong>Geofield</strong>, <strong>Geolocation field</strong> or <strong>Get Locations</strong> modules select <em>&lt;none&gt;</em>.'),
    '#weight' => 17,
  );

  $differentiator = isset($form_state['differentiator']) ? $form_state['differentiator'] : $views_plugin_style->options['differentiator']['differentiator_field'];

  if (isset($form_state['triggering_element'])) {
    // Get here when any form element with #ajax was changed/clicked causing
    // an auto-rebuild of the form. Can't put this in an ajax handler, as these
    // are called AFTER the form rebuild, so too late.
    if (strpos($form_state['triggering_element']['#id'], 'differentiator-differentiator-field') > 0) {
      // Get here when it was the differentiator drop-down that was changed.
      $differentiator = $form_state['triggering_element']['#value'];
      unset($form_state['num_associations']);
    }
  }
  $form_state['differentiator'] = $differentiator;
  // Or AJAX won't work!
  $form_state['no_cache'] = FALSE;

  $is_openlayers = $form_state['renderer'] == 'openlayers';

  $form['default_marker_color'] = array(
    '#title' => t('Default location marker'),
    '#type' => 'select',
    '#default_value' => $views_plugin_style->options['default_marker_color'],
    '#options' => $is_openlayers ? ip_geoloc_openlayers_marker_layers() : ip_geoloc_marker_colors(),
    '#description' => t('Select an image to use for all location markers whose images are not overridden by the <strong>location differentiator</strong> below.'),
    '#attributes' => array(
      'class' => $is_openlayers ? array('marker-color-ol') : array('marker-color')),
    '#weight' => 20,
  );
  if (!$is_openlayers) {
    $path = drupal_get_path('module', 'ip_geoloc');
    $css_file = strpos(variable_get('ip_geoloc_marker_directory'), 'amarkers') ? 'ip_geoloc_admin_a.css' : 'ip_geoloc_admin.css';
    $form['default_marker_color']['#attached']['css'] = array("$path/css/$css_file");
  }

  $descr = t('You may designate one field from your view as a <strong>location differentiator</strong>. Locations that match the same corresponding differentiator value will have the same marker image on the map. Examples of location differentiators are content type and taxonomy term. You can enter numeric and alphabetic <em>ranges</em> too, e.g. price ranges, like 100--125.<br/><strong>Leaflet</strong> only: you may input a special character, for example from <a target="_font_awesome" href="!url2">Font Awesome</a> (requires installation, see README) or <a target="_fsymbols" href="!url1">fsymbols</a> (no installation required), to be superimposed as a font-icon on the marker image. For <em>fsymbols</em> copy and paste the desired font-icon into the Special Char field. For <em>Font Awesome</em> icons leave the Special Char field empty. Instead type the <em>Font Awesome</em> icon name in the Special Char CSS Class field. Example: <strong>fa fa-beer</strong>. <br/>All this works best with the markers from the <em>/amarkers</em> directory, configurable <a target="_ip_geoloc_config" href="!url3">here</a>.', array(
    '!url1' => url('http://fsymbols.com'),
    '!url2' => url('http://fortawesome.github.io/Font-Awesome/cheatsheet'),
    '!url3' => url('admin/config/system/ip_geoloc'),
  ));
  // Add wrapper for differentiator drop-down, association table and buttons.
  // The id in the prefix must match the AJAX submit handlers below.
  $form['differentiator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Location differentiator and associated markers'),
    '#description' => $descr,
    '#prefix' => '<div id="differentiator-wrapper">',
    '#suffix' => '</div>',
    '#weight' => 30,
  );

  $instances = ip_geoloc_get_field_instances($differentiator);
  $instance = reset($instances);

  if (empty($differentiator)) {
    $description = t('Optionally select a location differentiator.');
  }
  else {
    $description = t('Below associate %differentiator values with marker images.', array(
      '%differentiator' => empty($instance) ? $fields[$differentiator] : $instance['label'],
    )) . '<br/>';
    $field = field_info_field($differentiator);
    if (!$field || $field['module'] == 'text') {
      $description .= t('You may enter a range of values by separating "from" and "to" by a <strong>double hyphen</strong>.<br/>Example: <strong>A--ZZ</strong><br/>You may omit "from" or "to" to create open-ended ranges.');
    }
    elseif ($field['module'] == 'number') {
      $description .= t('You may enter a numeric range of by separating "from" and "to" by a <strong>double hyphen</strong>.<br/>Example: <strong>2.5--7.95</strong><br/>You may omit "from" or "to" to create open-ended ranges.');
    }
    $description .= '<br/>' . t('You can refer to View URL arguments via !1, !2 etc.');
  }
  $form['differentiator']['differentiator_field'] = array(
    '#title' => t('Location differentiator'),
    '#type' => 'select',
    '#default_value' => $differentiator,
    '#options' => $fields,
    '#description' => $description,
    '#ajax' => array(
      'callback' => '_ip_geoloc_plugin_style_refresh_color_table_js',
      'wrapper' => 'differentiator-wrapper',
    ),
  );
  if (!empty($differentiator)) {
    // Container for the differentiator color associations table.
    $form['differentiator']['color_table'] = array(
      '#theme' => 'ip_geoloc_plugin_style_differentiator_color_table',
    // '#prefix' => '<div id="differentiator-color-table-wrapper">',
    // '#suffix' => '</div>'
    );

    _ip_geoloc_plugin_style_differentiator_color_table_form($form, $form_state);

    $form['differentiator']['add-another'] = array(
      '#type' => 'submit',
      '#value' => empty($form_state['num_associations']) ? t('Add an association') : t('Add another association'),
      '#weight' => 1,
      '#submit' => array('_ip_geoloc_plugin_style_add_association_submit'),
      '#ajax' => array(
        'callback' => '_ip_geoloc_plugin_style_refresh_color_table_js',
        'wrapper' => 'differentiator-wrapper',
        // Or 'none' or 'slide'.
        'effect' => 'fade',
        // Or 'slow' or number of millisec.
        'speed' => 'fast',
      ),
    );
    if (!empty($form_state['num_associations'])) {
      $form['differentiator']['remove'] = array(
        '#type' => 'submit',
        '#value' => t('Remove bottom association'),
        '#weight' => 2,
        '#submit' => array('_ip_geoloc_plugin_style_remove_association_submit'),
        '#ajax' => array(
          'callback' => '_ip_geoloc_plugin_style_refresh_color_table_js',
          'wrapper' => 'differentiator-wrapper',
          // Or 'fade' or 'slide'.
          'effect' => 'none',
          // Or 'slow' or number of millisec.
          'speed' => 'fast',
        ),
      );
    }
  }
  if ($is_openlayers) {
    $desc = t('*) If you want a fixed center, visit the "Center & Bounds" vertical tab on the <a target="_ol_map_edit" href="@url">map edit page</a>. This is also where you set the initial zoom level.', array(
      '@url' => url('admin/structure/openlayers/maps/' . $views_plugin_style->options['map'] . '/edit'),
    ));
  }
  else {
    $desc = t('*) If you choose the first option you may center the map via the special <strong>Map options</strong> <em>"centerLat"</em> and <em>"centerLng"</em> for example:<br/>%center_example', array(
      '%center_example' => '{ "mapTypeId":"terrain", "centerLat":-37.8, "centerLng":145 }',
    ));
  }
  $form['center_option'] = array(
    '#title' => t('Map centering options'),
    '#type' => 'radios',
    '#default_value' => $views_plugin_style->options['center_option'],
    '#options' => array(
      IP_GEOLOC_MAP_CENTER_FIXED => t('Fixed center; see note below *)'),
      IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION => t('Use the first location returned by the view as the center of the map.'),
      IP_GEOLOC_MAP_CENTER_ON_VISITOR => t("Center the map on the visitor's current location."),
      IP_GEOLOC_MAP_CENTER_OF_LOCATIONS => t('Use the center of the rectangle whose sides are defined by the left-most, right-most, top and bottom locations (this option is insensitive to location clusters).'),
      IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED => t('Use the center of gravity based on all locations (this option is sensitive to location clusters)'),
    ),
    '#description' => $desc,
    '#weight' => 40,
  );
  $form['empty_map_center'] = array(
    '#title' => t('No locations behaviour'),
    '#type' => 'textfield',
    '#size' => 30,
    '#default_value' => $views_plugin_style->options['empty_map_center'],
    '#description' => t('a) Show empty map: enter <em>latitude, longitude</em> of the map center.<br/>b) Show empty map centered on visitor location: enter the word %visitor.<br/>c) Show nothing: if this field is left blank, the map canvas will be suppressed when there are no locations to show. If you wish, you may use the <strong>No results behavior</strong> panel in the Advanced section of the Views UI to show an alternative text or content.', array(
      '%visitor' => t('visitor'))),
    '#weight' => 110,
  );
  return $form;
}

/**
 * Validation for ip_geoloc_plugin_style_bulk_of_form().
 */
function ip_geoloc_plugin_style_bulk_of_form_validate(&$form, &$form_state) {
  if ($form_state['values']['style_options']['ip_geoloc_views_plugin_latitude'] == '---') {
    $form_state['values']['style_options']['ip_geoloc_views_plugin_latitude'] = $form_state['values']['style_options']['ip_geoloc_views_plugin_latitude_text'];
  }
  if ($form_state['values']['style_options']['ip_geoloc_views_plugin_longitude'] == '---') {
    $form_state['values']['style_options']['ip_geoloc_views_plugin_longitude'] = $form_state['values']['style_options']['ip_geoloc_views_plugin_longitude_text'];
  }
}

/**
 * Submit handler for the "Add another association" button.
 *
 * Increments the counter and forces a form rebuild.
 */
function _ip_geoloc_plugin_style_add_association_submit($form, &$form_state) {
  $form_state['num_associations']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for the "Remove" button.
 *
 * Decrements the counter and forces a form rebuild.
 */
function _ip_geoloc_plugin_style_remove_association_submit($form, &$form_state) {
  $form_state['num_associations']--;
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax callback in response to new rows or the diff. drop-down being changed.
 *
 * At this point the $form has already been rebuilt. All we have to do here is
 * tell AJAX what part of the browser form needs to be updated.
 */
function _ip_geoloc_plugin_style_refresh_color_table_js($form, &$form_state) {
  // Return the updated table, so that ajax.inc can issue commands to the
  // browser to update only the targeted sections of the page.
  return $form['options']['style_options']['differentiator'];
}

/**
 * Submit handler as declared in ip_geoloc_form_alter().
 */
function ip_geoloc_plugin_style_differentiator_color_associations_submit($form, &$form_state) {
  if (empty($form_state['view']) || empty($form_state['display_id']) || empty($form_state['differentiator'])) {
    return;
  }
  $view_id = $form_state['view']->name;
  $display_id = $form_state['display_id'];
  $differentiator = $form_state['differentiator'];

  // Erase differentiator values for this display, then rebuild based on form.
  $differentiator_color_associations = variable_get('ip_geoloc_' . $view_id . '_color_mappings', array());
  unset($differentiator_color_associations[$display_id][$differentiator]);

  if (!empty($form_state['values']['color_table'])) {
    $i = 0;
    foreach ($form_state['values']['color_table'] as $association) {

      if (is_array($association[$differentiator])) {
        // E.g association['field_address']['und'][0]['postal_code']
        foreach ($association[$differentiator][LANGUAGE_NONE] as $values) {
          $differentiator_value = array();
          if (is_array($values)) {
            foreach ($values as $name => $value) {
              if (isset($value) && strpos($value, '|') === FALSE) {
                $differentiator_value[$name] = $value;
              }
            }
          }
          else {
            $differentiator_value[] = $values;
          }
          if (!empty($differentiator_value)) {
            $differentiator_color_associations[$display_id][$differentiator][$i]['differentiator_value'] = $differentiator_value;
            $differentiator_color_associations[$display_id][$differentiator][$i]['color'] = $association['color'];
            $differentiator_color_associations[$display_id][$differentiator][$i]['special char'] = $association['special char'];
            $differentiator_color_associations[$display_id][$differentiator][$i]['special char class'] = $association['special char class'];
            $i++;
          }
        }
      }
      else {
        // Plain text field or plain taxonomy select.
        $differentiator_value = trim($association[$differentiator]);
        if (!empty($differentiator_value) || $differentiator_value == 0) {
          $differentiator_color_associations[$display_id][$differentiator][$i]['differentiator_value'] = array($differentiator_value);
          $differentiator_color_associations[$display_id][$differentiator][$i]['color'] = $association['color'];
          $differentiator_color_associations[$display_id][$differentiator][$i]['special char'] = $association['special char'];
          $differentiator_color_associations[$display_id][$differentiator][$i]['special char class'] = $association['special char class'];
          $i++;
        }
      }
    }
  }
  variable_set('ip_geoloc_' . $view_id . '_color_mappings', $differentiator_color_associations);
}

/**
 * Plugin style color differ.
 */
function _ip_geoloc_plugin_style_differentiator_color_table_form(&$form, &$form_state) {

  $is_openlayers = $form_state['renderer'] == 'openlayers';

  // First the saved rows...
  // @todo: if $field['cardinality'] > 1, compress multiple diff. values
  // for the same color together in a single row.
  $view_id = $form_state['view']->name;
  $display_id = $form_state['display_id'];
  $differentiator = $form_state['differentiator'];

  $differentiator_color_associations = variable_get('ip_geoloc_' . $view_id . '_color_mappings', array());
  if (empty($differentiator_color_associations[$display_id][$differentiator])) {
    $differentiator_color_associations[$display_id] = array($differentiator => array());
  }
  $row = 0;
  foreach ($differentiator_color_associations[$display_id][$differentiator] as $association) {
    if (!is_array($association)) {
      // Data corrupt.
      break;
    }
    if (isset($form_state['num_associations']) && $row >= $form_state['num_associations']) {
      break;
    }
    $form['differentiator']['color_table'][$row] = _ip_geoloc_plugin_style_diff_color_table_row_form($is_openlayers, $row, $differentiator, $association);
    $row++;
  }
  // ... then the empty rows.
  if (!isset($form_state['num_associations'])) {
    $form_state['num_associations'] = count($differentiator_color_associations[$display_id][$differentiator]);
  }
  while ($row < $form_state['num_associations']) {
    $form['differentiator']['color_table'][$row] = _ip_geoloc_plugin_style_diff_color_table_row_form($is_openlayers, $row, $differentiator);
    $row++;
  }
}

/**
 * Diffs the color table plugin style row.
 */
function _ip_geoloc_plugin_style_diff_color_table_row_form($is_openlayers, $row, $differentiator, $association = NULL) {

  $differentiator_value = empty($association) ? NULL : $association['differentiator_value'];
  $attr_class = $is_openlayers ? array('differentiator ol') : array('differentiator');

  if (strpos($differentiator, 'field_') === 0) {
    $field = field_info_field($differentiator);
    $instances = ip_geoloc_get_field_instances($differentiator);
    $instance = reset($instances);
    $widget = $instance['widget']['type'];

    if (strpos($widget, 'taxonomy') !== FALSE) {
      // Covers core and Autocomplete Deluxe.
      $vid = 0;
      $vocabulary_name = $field['settings']['allowed_values'][0]['vocabulary'];
      foreach (taxonomy_get_vocabularies() as $vid => $vocabulary) {
        if ($vocabulary->machine_name == $vocabulary_name) {
          break;
        }
      }
      $options = array();
      foreach (taxonomy_get_tree($vid) as $term) {
        $options[$term->tid] = str_repeat('-', $term->depth) . $term->name;
      }
      $form[$differentiator] = array(
        '#type' => empty($options) ? 'textfield' : 'select',
        '#options' => $options,
        '#default_value' => $differentiator_value,
        '#attributes' => array('class' => $attr_class),
      );
    }
    else {
      // No label, unless other modules override this.
      $instance['label'] = '';
      // Don't want asterisk to appear.
      $instance['required'] = FALSE;
      // Make sure the text field is wide enough, especially for the case of a
      // range, which needs to receive two values and a separator.
      if (isset($instance['widget']['settings']['size']) && $instance['widget']['settings']['size'] < 15) {
        $instance['widget']['settings']['size'] = 15;
      }
      $items[0] = empty($differentiator_value) ? array() : is_array($differentiator_value) ? $differentiator_value : array($differentiator_value);
      if (isset($instance['default_value'])) {
        $items[0] += is_array($instance['default_value']) && !empty($instance['default_value']) ? reset($instance['default_value']) : $instance['default_value'];
      }
      $form['#parents'] = array();
      $form_state = array();

      $form = field_default_form($instance['entity_type'], NULL, $field, $instance, LANGUAGE_NONE, $items, $form, $form_state);

      if ($field['module'] == 'number' || $field['module'] == 'text') {
        $form[$differentiator][LANGUAGE_NONE][0][key($field['columns'])]['#element_validate'] = array('ip_geoloc_range_widget_validate');
      }
      $form[$differentiator]['#attributes']['class'][] = $attr_class;
    }
  }
  else {
    // Not a field, maybe an expression.
    $form[$differentiator] = array(
      '#type' => 'textfield',
      '#size' => 40,
      '#default_value' => $differentiator_value,
      '#element_validate' => array('ip_geoloc_range_widget_validate'),
      '#attributes' => array('class' => $attr_class),
    );
  }
  $form['color'] = array(
    '#type' => 'select',
    '#default_value' => empty($association) ? NULL : $association['color'],
    '#options' => $is_openlayers ? ip_geoloc_openlayers_marker_layers() : ip_geoloc_marker_colors(),
    '#attributes' => array(
      'class' => $is_openlayers ? array('marker-color-ol') : array('marker-color'),
    ),
  );
  $form['special char'] = array(
    '#type' => 'textfield',
    '#size' => 8,
    '#default_value' => empty($association) ? NULL : $association['special char'],
  );
  $form['special char class'] = array(
    '#type' => 'textfield',
    '#size' => 25,
    '#default_value' => empty($association) ? NULL : $association['special char class'],
  );
  // We'll manually set the #parents property of these fields so that their
  // values appear in the $form_state['values']['color_table'] array.
  $form[$differentiator]['#parents'] = array(
    'color_table', $row, $differentiator);
  $form['color']['#parents'] = array(
    'color_table', $row, 'color');
  $form['special char']['#parents'] = array(
    'color_table', $row, 'special char');
  $form['special char class']['#parents'] = array(
    'color_table', $row, 'special char class');

  return $form;
}

/**
 * Return HTML for differentiator to color associations table.
 *
 * @param array $variables
 *   An associative array containing $variables['form']: a render element
 *   representing the form.
 *
 * @ingroup themeable
 */
function theme_ip_geoloc_plugin_style_differentiator_color_table($variables) {
  // Use the first form child to find out the name of the differentiator.
  $form = $variables['form'];
  $form_children = element_children($form);
  if (empty($form_children)) {
    return '';
  }
  $key = reset($form_children);
  foreach ($form[$key] as $attribute_name => $element) {
    if (drupal_substr($attribute_name, 0, 1) != '#' && $attribute_name != 'color') {
      $differentiator = $attribute_name;
      break;
    }
  }
  if (empty($differentiator)) {
    return '';
  }
  $instances = ip_geoloc_get_field_instances($differentiator);
  $instance = reset($instances);
  $differentiator_label = isset($instance) ? $instance['label'] : $differentiator;

  $headers = array(
    t('%differentiator value', array('%differentiator' => $differentiator_label)),
    t('Associated marker image'),
    t('Special char'),
    t('Special char CSS class'),
  );
  $rows = array();
  foreach ($form_children as $key) {
    $row = array('data' => array(), 'class' => array());
    $row['data'][] = drupal_render($form[$key][$differentiator]);
    $row['data'][] = drupal_render($form[$key]['color']);
    $row['data'][] = drupal_render($form[$key]['special char']);
    $row['data'][] = drupal_render($form[$key]['special char class']);
    $rows[] = $row;
  }

  $output = theme('table', array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array('id' => 'differentiator-color-table'),
  ));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Extract an array of locations from the supplied views_plugin_style.
 *
 * @param string $views_plugin_style
 *   the style of views plugin
 *
 * @return array
 *   array of location objects, each containing lat/long and balloon_text
 */
function ip_geoloc_plugin_style_extract_locations($views_plugin_style) {

  $latitude  = trim($views_plugin_style->options['ip_geoloc_views_plugin_latitude']);
  $longitude = trim($views_plugin_style->options['ip_geoloc_views_plugin_longitude']);

  $view = &$views_plugin_style->view;
  $view_id = $view->name;
  $display_id = $views_plugin_style->display->id;

  if ($views_plugin_style->display->handler->is_defaulted('style_options')) {
    //$display_id = 'default';
  }

  $differentiator = FALSE;
  if (!empty($views_plugin_style->options['differentiator']['differentiator_field'])) {
    $differentiator = $views_plugin_style->options['differentiator']['differentiator_field'];
    $differentiator_color_associations = variable_get('ip_geoloc_' . $view_id . '_color_mappings', array());
    if (empty($differentiator_color_associations[$display_id])) {
      $differentiator_color_associations[$display_id] = array($differentiator => array());
    }
  }
  $tooltip_field = FALSE;
  if (isset($views_plugin_style->options['tooltips']['marker_tooltip'])) {
    $marker_tooltip = $views_plugin_style->options['tooltips']['marker_tooltip'];
    if (isset($view->field[$marker_tooltip])) {
      $tooltip_field = $view->field[$marker_tooltip];
    }
  }
  $tag_field = FALSE;
  if (isset($views_plugin_style->options['tags']['marker_tag'])) {
    $marker_tag = $views_plugin_style->options['tags']['marker_tag'];
    if (isset($view->field[$marker_tag])) {
      $tag_field = $view->field[$marker_tag];
    }
  }
  $locations = array();
  $error_count = $first_error = 0;
  $loc_field_name = $loc_field_alias = NULL;
  if (isset($view->field[$latitude])) {
    $loc_field_name = $view->field[$latitude]->definition['field_name'];
    $loc_field_alias = $view->field[$latitude]->field_alias;
    // Example: $loc_field_name == 'field_geo'; $loc_field_alias == 'nid';
  }

  // Group the rows according to the grouping instructions, if specified.
  $sets = $views_plugin_style->render_grouping(
    $views_plugin_style->view->result,
    $views_plugin_style->options['grouping'],
    TRUE
  );
  if (!empty($views_plugin_style->options['grouping'])) {
    // @todo - Add support for multiple grouping fields?
    $group_field = $views_plugin_style->options['grouping'][0]['field'];
  }

  foreach ($sets as $set) {
    // Render as a grouping set.
    if (!empty($set['group'])) {
      // This is required for some Views functions.
      $view->row_index = $i = key($set['rows']);

      $row = reset($set['rows']);
      $base = _ip_geoloc_plugin_style_get_base1($row, $loc_field_name, $loc_field_alias);

      if ($location = _ip_geoloc_plugin_style_extract_lat_lng($row, $latitude, $longitude, $base)) {
        // Remaining row values go into the balloon.
        if (!empty($views_plugin_style->rendered_fields[$i])) {
          $location->balloon_text = $views_plugin_style->rendered_fields[$i][$group_field];
          foreach ($set['rows'] as $i => $row) {
            $rendered_fields = $views_plugin_style->rendered_fields[$i];
            unset($rendered_fields[$group_field]);
            $location->balloon_text .= '<br />' . implode('<br/>', $rendered_fields);
          }
        }
        if (!empty($differentiator_color_associations[$display_id][$differentiator])) {
          _ip_geoloc_plugin_style_set_marker_color($location, $row, $differentiator, $view->args, $differentiator_color_associations[$display_id][$differentiator]);
        }
         _ip_geoloc_plugin_style_decorate($location, $row, $tooltip_field, $tag_field);
        $locations[] = $location;
      }
      elseif ($error_count++ == 0) {
        $first_error = $i;
      }
    }
    // Render as a record set.
    else {
      foreach ($view->result as $i => $row) {
        // This is required for some Views functions.
        $view->row_index = $i;

        $base = _ip_geoloc_plugin_style_get_base1($row, $loc_field_name, $loc_field_alias);

        if ($location = _ip_geoloc_plugin_style_extract_lat_lng($row, $latitude, $longitude, $base)) {
          // Remaining row values go into the balloon.
          if (!empty($views_plugin_style->rendered_fields[$i])) {
            $location->balloon_text = implode('<br/>', $views_plugin_style->rendered_fields[$i]);
          }
          if (!empty($differentiator_color_associations[$display_id][$differentiator])) {
            _ip_geoloc_plugin_style_set_marker_color($location, $row, $differentiator, $view->args, $differentiator_color_associations[$display_id][$differentiator]);
          }
          _ip_geoloc_plugin_style_decorate($location, $row, $tooltip_field, $tag_field);
          $locations[] = $location;
        }
        elseif ($error_count++ == 0) {
          $first_error = $i;
        }
      }
    }
  }
  global $user;
  if ($error_count > 0 && $user->uid == 1) {
    $row_count = count($view->result);
    $title = empty($views_plugin_style->display->display_options['title']) ? $view->get_human_name() . ' (' . $views_plugin_style->display->display_title . ')' : $views_plugin_style->display->display_options['title'];
    $t_args = array(
      '%view_title' => $title,
      '@total' => $row_count,
      '@error_count' => $error_count,
      '@first' => $first_error + 1,
      '%latitude' => $latitude,
    );
    $msg = ($error_count >= $row_count) ? t('None of the @total result rows in view %view_title had their %latitude lat/lng set. Therefore those rows could not be displayed as locations on the map. Is %latitude the correct field name?', $t_args) : t('Out of a total of @total result rows in view %view_title, @error_count rows did not have their %latitude lat/lng set and therefore could not be shown on the map. The first row that could not be located was row #@first. You can improve execution efficiency by using the Views UI to apply an "is not empty" filter criterion to %latitude.', $t_args);

    $row_contents = t('First error row:') . '<br/>';
    foreach ((array) $view->result[$first_error] as $field_name => $value) {
      $row_contents .= "<strong>$field_name</strong> = " . print_r($value, TRUE) . '<br/>';
    }
    drupal_set_message($msg, 'warning');
    drupal_set_message($row_contents, 'warning');
    drupal_set_message(t('The above messages are shown only to user 1.'), 'warning');
  }
  // Allow other modules to implement
  // hook_ip_geoloc_marker_locations_alter(&$locations, &$view)
  drupal_alter('ip_geoloc_marker_locations', $locations, $view);

  return $locations;
}

function _ip_geoloc_plugin_style_get_base1($row, $location_field_name, $location_field_alias) {
  $base = NULL;
  if (isset($location_field_alias)) {
    // The lines below are stripped back fast versions of this call:
    // $loc_field_value = $loc_field->get_value($row);
    $entity = $row->_field_data[$location_field_alias]['entity'];
    // The field value may not be there, i.e. no coordinates entered.
    if (isset($entity->{$location_field_name})) {
      $location_field_value = reset($entity->{$location_field_name});
      $base = is_array($location_field_value) ? reset($location_field_value) : $location_field_value;
    }
  }
  return $base;
}

/**
 * Plugin style base.
 */
function _ip_geoloc_plugin_style_get_base2($row, $field_name, $delta) {
  if (!isset($row) || !isset($field_name) || !isset($delta)) {
    return NULL;
  }
  if (isset($row->{$field_name}) && is_array($row->{$field_name})) {
    if (!empty($row->{$field_name}[$delta]['raw'])) {
      return $row->{$field_name}[$delta]['raw'];
      // Geofield     : field_<field_name>[0]['raw']['geom']
      // Geolocation  : field_<field_name>[0]['raw']['lat'] and ...]['lng']
      // Get Locations: field_<field_name>[0]['raw']['latitude']
    }
    $lang = LANGUAGE_NONE;
    if (!empty($row->{$field_name}[$lang][$delta])) {
      // EntityFieldQuery Views Backend.
      // For Geofield module lat/long are as follows (note: no extra
      // 'field_' prefix):
      // <field_name>['und]['0']['wkt'], for example:
      // POINT (144.976 -37.813) or MULTIPOINT ( (),(),() )
      return $row->{$field_name}[$lang][$delta];
    }
    // Last resort, retrieve from rendered, as opposed to raw, result.
    if (!empty($row->{$field_name}[$delta]['rendered'])) {
      $base = $row->{$field_name}[$delta]['rendered'];
      return empty($base['#markup']) ? $base : $base['#markup'];
    }
  }
  return NULL;
}


/**
 * Extract coordinates from the View result row.
 *
 * @todo This function is in flux and a bit messy at the moment....
 *
 * @param array $row
 *   View result row
 * @param string $latitude
 *   name of the row field that holds latitude
 * @param string $longitude
 *   name of the row field that holds longitude
 *
 * @return object|NULL
 *   location -- @todo return array for consistency with Leaflet?
 */
function _ip_geoloc_plugin_style_extract_lat_lng($row, $latitude, $longitude, $loc_field_value = NULL) {

  $delta = &drupal_static(__FUNCTION__);
  if (isset($delta)) {
    $delta++;
  }
  // Hack for http://drupal.org/node/1824538
  // In the View, AddressField must have "Display all values in the same row"
  // UNTICKED, while Geofield (and all other fields) must have it TICKED.
  foreach ((array) $row as $key => $value) {
    if (drupal_substr($key, -6) == '_delta' && strpos($key, 'field_data_field_') === 0) {
      $delta = $value;
      break;
    }
  }
  $field_name = 'field_' . $latitude;
  if (!isset($delta) || !isset($row->{$field_name}) || $delta >= count($row->{$field_name})) {
    $delta = 0;
  }
  $location = new stdClass();

  $base = _ip_geoloc_plugin_style_get_base2($row, $field_name, $delta);
  if (empty($base)) {
    // Drop the "field_" prefix and try again.
    $base = _ip_geoloc_plugin_style_get_base2($row, $latitude, $delta);
  }
  if (empty($base)) {
    $base = $loc_field_value;
  }
  if (empty($base)) {
    // Empty $base, e.g. Location module.
    if (isset($row->{$latitude}) && isset($row->{$longitude})) {
      // If not node fields then db table fields...
      $location->latitude  = $row->{$latitude};
      $location->longitude = $row->{$longitude};
    }
  }
  else {
    if (is_string($base)) {
      // Expect a rendered result like "Latitude: -37.8<br/>Longitude: 144.96"
      $parts = explode(':', $base);
      if (isset($parts[2])) {
        $location->latitude = (float) $parts[1];
        $location->longitude = (float) $parts[2];
      }
    }
    elseif (isset($base['geo_type'])) {
      // Geofield.
      // Quick fix: take advantage of Leaflet module, if enabled.
      // @todo generalise Leaflet code and incorporate here.
      if (function_exists('leaflet_process_geofield')) {
        // Support WKT lines and polygons in Leaflet. Call requires:
        // $base['geo_type'] eg 'point' or 'linestring'
        // $base['geom'] eg. 'POINT(145 -37)' or 'LINESTRING(145 -37, 146, -38)'
        if (!isset($base['wkt'])) {
          // Until fixed in leaflet_process_geofield().
          $base['wkt'] = $base['geom'];
        }
        $location = leaflet_process_geofield(array($base));
        $location = (object) reset($location);
        // @todo get rid of this duality, use lat/lon throughout
        if (isset($location->lat)) {
          $location->latitude = $location->lat;
          unset($location->lat);
        }
        if (isset($location->lon)) {
          $location->longitude = $location->lon;
          unset($location->lon);
        }
      }
      else {
        $is_point = ($base['geo_type'] == 'point');
        if ($is_point) {
          // Wkt more accurate than lat,lon in Geofield 7.x-1.x.
          $point = empty($base['geom']) ? $base['wkt'] : $base['geom'];
          // @todo Consider using the following:
          // $geometry = geoPHP::load($base['geom']);
          // $location = json_decode($geometry->out('json'));
          $is_point = (drupal_substr(trim($point), 0, 7) == 'POINT (');
          if ($is_point) {
            $parts = explode(' ', drupal_substr($point, 7));
            $is_point = count($parts) > 1;
          }
        }
        // $is_point==FALSE may indicate a MULTIPOINT cluster, which has its
        // averaged center on 'lat' and 'lon' indices.
        $location->longitude = $is_point ? (float) $parts[0] : $base['lon'];
        $location->latitude  = $is_point ? (float) $parts[1] : $base['lat'];
      }
    }
    elseif (isset($base['lng'])) {
      // GeoLocation.
      $location->latitude  = $base['lat'];
      $location->longitude = $base['lng'];
    }
    elseif (isset($base['longitude'])) {
      // Get Locations.
      $location->latitude  = $base['latitude'];
      $location->longitude = $base['longitude'];
    }
    elseif (isset($row->{'field_' . $latitude}[$delta]['raw']['value'])) {
      // Other module.
      // Field values tend to be inside ...[0]['raw']['value']:
      $location->latitude  = $row->{'field_' . $latitude}[$delta]['raw']['value'];
      $location->longitude = $row->{'field_' . $longitude}[$delta]['raw']['value'];
    }
  }
  return isset($location->type) || isset($location->latitude) ? $location : NULL;
}

/**
 * Set the marker color.
 */
function _ip_geoloc_plugin_style_set_marker_color(&$location, $row, $differentiator, $view_args, $differentiator_color_associations) {

  if (!empty($differentiator)) {

    if (preg_match('/^!([0-9])/', $differentiator, $matches)) {
      // Check if an argument was appended to the URL. Internal count is from 0.
      $position = $matches[1] - 1;
      if (isset($view_args[$position])) {
        $differentiator_value = $view_args[$position];
      }
    }
    elseif (isset($row->{$differentiator})) {
      $differentiator_value = $row->{$differentiator};
    }
    elseif (!empty($row->{'field_' . $differentiator})) {
      $differentiator_values = $row->{'field_' . $differentiator};
      if (is_array($differentiator_values)) {
        // Count > 1 for taxonomy term with lineage.
        $num_values = count($differentiator_values);
        if (isset($differentiator_values[0]['raw'])) {
          $differentiator_value = $differentiator_values[$num_values - 1]['raw'];
        }
        elseif (isset($differentiator_values[0]['rendered']['#markup'])) {
          $differentiator_value = $differentiator_values[$num_values - 1]['rendered']['#markup'];
        }
      }
    }
    elseif (isset($row->_entity_properties[$differentiator])) {
      // Search API
      $differentiator_value = $row->_entity_properties[$differentiator];
    }
    else {
      $differentiator_value = '';
      if (!isset($row->{'field_' . $differentiator})) {
        drupal_set_message(t('IPGV&M: no differentiator values found for %diff. Cannot set marker images.', array(
          '%diff' => $differentiator)), 'warning', FALSE);
      }
    }
    if (!empty($differentiator_value) || $differentiator_value == 0) {

      foreach ($differentiator_color_associations as $association) {
        $match = TRUE;
        if (is_array($differentiator_value)) {
          if (empty($differentiator_value['tid'])) {
            // Eg AddressField, has multiple subvalues. All must match the
            // corresponding differentiator subvalues.
            foreach ($association['differentiator_value'] as $name => $subvalue) {
              if (!isset($differentiator_value[$name]) || !ip_geoloc_is_in_range($differentiator_value[$name], $subvalue, $view_args)) {
                $match = FALSE;
                break;
              }
            }
          }
          else {
            // 'tid' set: taxonomy widget (plain or autocomplete)
            // $association['differentiator_value'] can have index 0 or 'tid'
            $range = reset($association['differentiator_value']);
            $match = ip_geoloc_is_in_range($differentiator_value['tid'], $range, $view_args);
          }
        }
        else {
          // Single value, either as simple value or array of length 1.
          $range = $association['differentiator_value'];
          $match = ip_geoloc_is_in_range($differentiator_value, $range, $view_args);
        }
        if ($match) {
          $location->marker_color = $association['color'];
          if (!empty($association['special char'])) {
            $location->marker_special_char = $association['special char'];
          }
          if (!empty($association['special char class'])) {
            $location->marker_special_char_class = $association['special char class'];
          }
        }
      }
    }
  }
  if (isset($location->marker_color) && is_array($location->marker_color)) {
    $location->marker_color = reset($location->marker_color);
  }
}

function _ip_geoloc_plugin_style_decorate(&$location, $row, $tooltip_field, $tag_field) {
  if ($tooltip_field) {
    $tooltip = $tooltip_field->theme($row);
    $location->marker_tooltip = strip_tags(_ip_geoloc_plugin_style_add_label($tooltip_field, $tooltip));
  }
  if ($tag_field) {
    $tag = $tag_field->theme($row);
    $location->marker_tag = _ip_geoloc_plugin_style_add_label_and_styling($tag_field, $tag);
  }
  if (isset($row->nid)) {
    $location->id = $row->nid;
  }
  elseif (isset($row->tid)) {
    $location->id = $row->tid;
  }
}

/**
 * Perform token replacement, convert timestamps to date strings etc.
 *
 * Store the rendered rows on the object passed in, which will typically be an
 * instance of class views_plugin_style or subclass.
 * Note that fields that have their Exclude box ticked, won't be rendered,
 * Typical candidates for exclusion are the latitude and longitude fields.
 *
 * @param string $views_plugin_style
 *   The views plugin style.
 */
function ip_geoloc_plugin_style_render_fields($views_plugin_style) {
  if (!$views_plugin_style->uses_fields() || isset($views_plugin_style->rendered_fields)) {
    return;
  }
  $views_plugin_style->rendered_fields = array();
  $differentiator = $views_plugin_style->options['differentiator']['differentiator_field'];
  $latitude = trim($views_plugin_style->options['ip_geoloc_views_plugin_latitude']);
  $longitude = trim($views_plugin_style->options['ip_geoloc_views_plugin_longitude']);

  foreach ($views_plugin_style->view->result as $i => &$row) {
    // God knows why we need this...
    $views_plugin_style->view->row_index = $i;
    foreach ($views_plugin_style->view->field as $field_id => $field) {
      // theme() also renders the tokens that may be used in the balloon fields.
      $field_value = $field->theme($row);

      // Add the special 'views_' fields that normally aren't in the results
      // set to the row, if required for the $differentiator or lat/lon that
      // have math expressions applied through the Views UI.
      // Examples:
      //  "User: Roles" used as differentiator becomes 'views_rid'
      //  "Global: View result counter" as differentiator becomes 'views_counter'
      $special_field_id = 'views_' . $field_id;
      if (($special_field_id == $differentiator || $special_field_id == $latitude || $special_field_id == $longitude) && empty($row->{$special_field_id})) {
        $row->{$special_field_id}[] = $field_value;
      }
      // If Excluded is ticked, we don't style and add the field to the
      // rendered_fields.
      if (!$field->options['exclude']) {
        $styled_field_value = _ip_geoloc_plugin_style_add_label_and_styling($field, $field_value);
        $views_plugin_style->rendered_fields[$i][$field_id] = $styled_field_value;
      }
    }
  }
  unset($views_plugin_style->view->row_index);
  return $views_plugin_style->rendered_fields;
}

/**
 * Prefix the field value with its label, where requested. No styling.
 *
 * @param string $field
 *   The field.
 * @param string $field_content
 *   The content of the field.
 *
 * @return string
 *   $field_content prefixed with the label defined for the field
 */
function _ip_geoloc_plugin_style_add_label($field, $field_content) {
  $label = $field->label();
  if ($label && $field->options['element_label_colon']) {
    $label .= ': ';
  }
  return $label . $field_content;
}

/**
 * Style field value, label and wrapper around both, where requested.
 *
 * Note: the "Add default classes" tickbox, options['element_default_classes'],
 * is NOT supported to avoid the balloons having excessive bulky markup.
 *
 * @param string $field
 *   The field.
 * @param string $field_content
 *   The content of the field.
 *
 * @return string
 *   $field_content wrapped in HTML with CSS classes
 */
function _ip_geoloc_plugin_style_add_label_and_styling($field, $field_content) {

  if ($field_content_type = $field->element_type(TRUE, TRUE)) {
    $element = '<' . $field_content_type;
    if ($field_content_class = $field->element_classes(NULL)) {
      $element .= ' class="' . $field_content_class . '"';
    }
    $element .= '>';
    $close_element = '</' . $field_content_type . '>';
    $field_content = $element . $field_content . $close_element;
  }

  $label = $field->label();
  if ($label && $field->options['element_label_colon']) {
    $label .= ': ';
  }
  $label_and_field_content = $label . $field_content;

  if ($label_type = $field->element_label_type(TRUE, TRUE)) {
    $element = '<' . $label_type;
    if ($label_class = $field->element_label_classes(NULL)) {
      $element .= ' class="' . $label_class . '"';
    }
    $element .= '>';
    $close_element = '</' . $label_type . '>';
    $label_and_field_content = $element . $label . $close_element . $field_content;
  }

  if ($wrapper_type = $field->element_wrapper_type(TRUE, TRUE)) {
    $element = '<' . $wrapper_type;
    if ($wrapper_class = $field->element_wrapper_classes(NULL)) {
      $element .= ' class="' . $wrapper_class . '"';
    }
    $element .= '>';
    $close_element = '</' . $wrapper_type . '>';
    return $element . $label_and_field_content . $close_element;
  }
  return $label_and_field_content;
}

/**
 * Implements hook_get_display_fields().
 */
function ip_geoloc_get_display_fields($view_display, $with_table_prefixes = TRUE) {
  $fields = array('' => '<' . t('none') . '>');
  $field_handlers = $view_display->get_handlers('field');
  foreach ($field_handlers as $field_id => $field_handler) {
    if ($with_table_prefixes) {
      if ($field_id == 'rid') {
        // See ip_geoloc_plugin_style_render_fields().
        $field_id = 'views_rid';
      }
      elseif (($field_id != 'nid') && ($field_id != 'uid') && strpos($field_id, 'field_') === FALSE) {
        // Example: 'title' becomes 'node_title'.
        $field_id = $field_handler->table . "_$field_id";
      }
    }
    $fields[$field_id] = $field_handler->ui_name();
  }
  // Add special differentiators: up to 4 View arguments appended to the URL
  for ($i = 1; $i <= 4; $i++) {
    $fields["!$i"] = t('View URL argument #@num', array('@num' => $i));
  }
  return $fields;
}

/**
 * Implements hook_get_field_instances().
 */
function ip_geoloc_get_field_instances($field_name) {
  $instances = array();
  foreach (field_info_instances() as $type_bundles) {
    foreach ($type_bundles as $bundle_instances) {
      foreach ($bundle_instances as $fld_name => $instance) {
        if ($fld_name == $field_name) {
          $instances[] = $instance;
        }
      }
    }
  }
  return $instances;
}
