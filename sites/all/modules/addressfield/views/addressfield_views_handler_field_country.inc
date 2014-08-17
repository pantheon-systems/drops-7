<?php

/**
 * Defines a field handler that can display the country name instead of the two
 * character country code for an address field country value.
 */
class addressfield_views_handler_field_country extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['display_name'] = array('default' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['display_name'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display the localized country name instead of the two character country code'),
      '#default_value' => $this->options['display_name'],
    );
  }

  function get_value($values, $field = NULL) {
    $value = parent::get_value($values, $field);

    // If we have a value for the field, look for the country name in the
    // Address Field options list array if specified.
    if (!empty($value) && !empty($this->options['display_name'])) {
      $countries = _addressfield_country_options_list();

      if (!empty($countries[$value])) {
        $value = $countries[$value];
      }
    }

    return $value;
  }
}
