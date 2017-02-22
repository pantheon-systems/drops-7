<?php

/**
 * Adds column to recline field for storing non-view data.
 *
 *  @param array columns
 *  Array of existing schema definitions.
 *
 */
function hook_recline_field_columns(&$columns) {
  $columns['my_new_view_additional_data'] = array(
    'type' => 'varchar',
    'length' => '32',
    'description' => 'Column to determine the choropleth map data.',
  );
}


/**
 * Adds an additional view to Recline view options.
 *
 * @return
 * A key/value of the new view.
 */
function hook_recline_view_options() {
  // Adds "my_new_view" column to database and "My New View" to list of Recline
  // views.
  return array('my_new_view' => t('My New View'));
}

/**
 * Additional columns and views need to be added to existing fields.
 * Below is an example using the addtional column and view defined above.
 * my_new_view_additional_data() is schema defined in
 * hook_recline_field_columns.
 */
function hook_update_N(&$sandbox) {
  $ret = array();
  $fields = field_info_fields();
  foreach ($fields as $field_name => $field) {
    if ($field['type'] == 'recline_field' && $field['storage']['type'] == 'field_sql_storage') {
      foreach ($field['storage']['details']['sql'] as $type => $table_info) {
        foreach ($table_info as $table_name => $columns) {
          $column_name = _field_sql_storage_columnname($field_name, 'my_new_view_additional_data');
          // Adding my_new_view_additional_data.
          if (!(db_field_exists($table_name, $column_name))) {
            // Calling schema defined in hook_recline_field_column().
            $schema = my_new_view_additional_data();
            db_add_field($table_name, $column_name, $schema);
          }
          // Adding my_new_view.
          $column_name = _field_sql_storage_columnname($field_name, 'my_new_view');
          $schema = recline_field_schema();
          if (!(db_field_exists($table_name, $column_name))) {
            db_add_field($table_name, $column_name, $schema['columns']['my_new_view']);
          }
          field_cache_clear();
        }
      }
    }
  }
  return $ret;
}

/**
 * The following are examples of adding the additional column data to the
 * recline widget.
 */

/**
 * Implements hook_field_widget_form_alter().
 */
function hook_field_widget_form_alter(&$element, &$form_state, $context) {
  if ($context['field']['type'] == 'recline_field') {
    foreach ($element as $delta => $instance) {
      $my_new_view_additional_data = isset($element[$delta]['#default_value']['my_new_view_additional_data']) ? $element[$delta]['#default_value']['my_new_view_additional_data'] : FALSE;
      $element[$delta]['my_new_view_additional_data'] = array(
        '#title' => 'My Additional Data',
        '#description' => t('Enter additional data.'),
        '#type' => 'textfield',
        '#weight' => 1,
        '#default_value' => $my_new_view_additional_data,
      );
    }
  }
}

/**
 * The following is an example of responding to a new view or additional column.
 * The implementing module is responsible for adding additional view to page.
 */

/**
 * Implements hook_theme_registry_alter().
 */
function hook_theme_registry_alter(&$theme_registry) {
  $theme_registry['recline_default_formatter']['function'] = 'MY_MODULE_recline_default_formatter';
}

/**
 * Adds js settings from recline field.
 */
function MY_MODULE_recline_default_formatter($variables) {
  $output = recline_preview_multiview($variables);
  $settings = array();
  if (isset($variables['item']['my_new_view_additional_data']) || isset($variables['item']['my_new_view_additional_data'])) {
    $settings['choropleth'] = array(
      'my_new_view_additional_data' => $variables['item']['my_new_view_additional_data'],
      'my_new_view' => $variables['item']['my_new_view'],
    );
    drupal_add_js($settings, 'setting');
  }
  drupal_add_js('path/to/new_recline_view.js');
  return drupal_render($output);
}
