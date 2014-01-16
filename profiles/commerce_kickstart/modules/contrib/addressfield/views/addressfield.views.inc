<?php

/**
 * Implements hook_field_views_data().
 */
function addressfield_field_views_data($field) {
  $data = field_views_field_default_views_data($field);

  // Add a filter handler for countries.
  $key = key($field['storage']['details']);
  foreach ($field['storage']['details'][$key] as $type => $tables) {
    foreach ($tables as $table_name => $columns) {
      if (!isset($columns['country'])) {
        continue;
      }

      $column_real_name = $columns['country'];
      if (!isset($data[$table_name][$column_real_name]['filter'])) {
        continue;
      }

      $data[$table_name][$column_real_name]['filter']['handler'] = 'addressfield_views_handler_filter_country';
    }
  }

  // Only expose these components as Views field handlers.
  $implemented = array(
    'country' => 'addressfield_views_handler_field_country',
    'administrative_area' => 'views_handler_field',
    'locality' => 'views_handler_field',
    'postal_code' => 'views_handler_field',
    'thoroughfare' => 'views_handler_field',
    'premise' => 'views_handler_field',
    'organisation_name' => 'views_handler_field',
    'name_line' => 'views_handler_field',
    'first_name' => 'views_handler_field',
    'last_name' => 'views_handler_field',
  );

  // Get the translated field information.
  $properties = addressfield_data_property_info();

  // Iterate over addressfield defined tables.
  foreach ($data as &$table) {
    // Make sure the parent Views field (addressfield) is defined.
    if (isset($table[$field['field_name']]['field'])) {
      // Use the parent field definition as a template for component columns.
      $field_def = $table[$field['field_name']]['field'];

      // Remove 'additional fields' from the field definition. We don't
      // necessarily want all our sibling columns.
      unset($field_def['additional fields']);

      // Define the valid columns.
      $valid_columns = array();
      foreach ($implemented as $implement => $handler) {
        $column_name = $field['field_name'] . '_' . $implement;
        $valid_columns[$column_name] = $handler;
      }

      // Iterate over the addressfield components.
      foreach ($table as $column_name => &$column) {
        if (empty($column['field']) && isset($valid_columns[$column_name])) {
          // Assign the default component definition.
          $column['field'] = $field_def;
          $column['field']['real field'] = $column_name;
          $column['field']['handler'] = $valid_columns[$column_name];

          // Assign human-friendly field labels for addressfield components.
          $field_labels = field_views_field_label($field['field_name']);
          $field_label = array_shift($field_labels);
          $property = str_replace($field_def['field_name'] . '_', '', $column_name);

          if (!empty($properties[$property])) {
            $property_label = $properties[$property]['label'];

            $title = t('@field-label - @property-label', array(
              '@field-label' => $field_label,
              '@property-label' => $property_label,
            ));

            $column['title'] = $title;
            $column['title short'] = $title;
          }
        }
      }
    }
  }

  return $data;
}
