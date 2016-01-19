<?php

/**
 * Defines a field handler that can display the administrative area name
 * instead of the code.
 */
class addressfield_views_handler_field_administrative_area extends views_handler_field {

  function query() {
    parent::query();

    $this->country_alias = $this->query->add_field($this->table_alias, $this->definition['field_name'] . '_country');
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['display_name'] = array('default' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['display_name'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display the name of administrative area instead of the code.'),
      '#default_value' => $this->options['display_name'],
    );
  }

  function get_value($values, $field = NULL) {
    $value = parent::get_value($values, $field);

    // If we have a value for the field, look for the administrative area name in the
    // Address Field options list array if specified.
    if (!empty($value) && !empty($this->options['display_name'])) {
      module_load_include('inc', 'addressfield', 'addressfield.administrative_areas');
      $country = $values->{$this->country_alias};
      $areas = addressfield_get_administrative_areas($country);

      if (!empty($areas[$value])) {
        $value = $areas[$value];
      }
    }

    return $value;
  }
}
