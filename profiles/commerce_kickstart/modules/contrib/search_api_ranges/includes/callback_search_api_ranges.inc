<?php

/**
 * Search API ranges data alteration callback that indexes the min and max
 * of selected numeric fields.
 */
class SearchApiRangesAlter extends SearchApiAbstractAlterCallback {

  protected $min_suffix = '_asc';
  protected $max_suffix = '_desc';

  public function configurationForm() {
    // Retrieve indexed fields.
    $fields = $this->index->getFields(TRUE);
    $field_options = array();
    $this->options += array('fields' => array());
    $eligible_types = array('integer', 'decimal');
    foreach ($fields as $name => $field) {
      if (search_api_is_list_type($field['type']) && in_array(search_api_extract_inner_type($field['type']), $eligible_types)) {
        $field_options[$name] = $field['name'];
      }
    }
    if (!empty($field_options)) {
      $form['fields'] = array(
        '#type' => 'fieldset',
        '#title' => t('Fields to run on'),
      );
      foreach ($field_options as $field_name => $label) {
        $default_value = FALSE;
        if (isset($this->options['fields'][$field_name])) {
          if (!is_array($this->options['fields'][$field_name])) {
            $default_value = TRUE;
          }
          else {
            if (!empty($this->options['fields'][$field_name]['name'])) {
              $default_value = TRUE;
            }
          }
        }
        $form['fields'][$field_name]['enabled'] = array(
          '#type' => 'checkbox',
          '#title' => $label,
          '#default_value' => $default_value,
        );
        $form['fields'][$field_name]['name'] = array(
          '#type' => 'value',
          '#value' => $label,
        );
        $form['fields'][$field_name]['description'] = array(
          '#type' => 'value',
          '#value' => isset($fields[$field_name]['description']) ? $fields[$field_name]['description'] : '',
        );
        $form['fields'][$field_name]['type'] = array(
          '#type' => 'value',
          '#value' => search_api_extract_inner_type($fields[$field_name]['type']),
        );
      }
      return $form;
    }
  }

  public function configurationFormValidate(array $form, array &$values, array &$form_state) {
    // Make the index believe that the status of the callback has changed to
    // force Search API to take in account our properties.
    // We can't do that in the submit because it's happening too late.
    if ($form_state['values']['callbacks']['search_api_ranges_alter']['status'] == 1) {
      if ($this->index->options['data_alter_callbacks']['search_api_ranges_alter']['status'] == 1) {
        $form_state['index']->options['data_alter_callbacks']['search_api_ranges_alter']['status'] = 0;
      }
    }
  }

  /**
   * Submit callback for configuration form.
   */
  public function configurationFormSubmit(array $form, array &$values, array &$form_state) {
    if (empty($values['fields'])) {
      return array();
    }
    foreach ($values['fields'] as $name => $field) {
      if (empty($field['enabled'])) {
        unset($values['fields'][$name]);
      }
      else {
        // Don't save the enabled flag, it's only used here.
        unset($values['fields'][$name]['enabled']);
      }
    }
    $this->options = $values;
    return $values;
  }

  /**
   * Index the min and max for the selected fields.
   */
  public function alterItems(array &$items) {
    if (!$items) {
      return;
    }
    if (!empty($this->options['fields'])) {
      foreach ($this->options['fields'] as $name => $field) {
        $required_fields[$name] = array(
          'type' => search_api_extract_inner_type($this->index->options['fields'][$name]['type']),
        );
      }
      foreach ($items as $item) {
        $wrapper = $this->index->entityWrapper($item);
        $fields = search_api_extract_fields($wrapper, $required_fields);
        foreach ($fields as $name => $f) {
          $name = str_replace(':', '_', $name);
          if (!empty($f['value'])) {
            if (!is_array($f['value'])) {
              $f['value'] = array($f['value']);
            }
            $item->{$name . $this->min_suffix} = min($f['value']);
            $item->{$name . $this->max_suffix} = max($f['value']);
          }
        }
      }
    }
  }

  /**
   * Add properties to the index (Min and max) of the selected fields.
   */
  public function propertyInfo() {
    $ret = array();
    if (!empty($this->options['fields'])) {
      foreach ($this->options['fields'] as $name => $field) {
        // Backward compatibility with the old format.
        $is_array = is_array($field);
        $prefix = str_replace(':', '_', $name);
        $ret[$prefix . $this->min_suffix] = array(
          'label' => ($is_array && !empty($field['name'])) ? $field['name'] . ' (Min)' : t('Search API ranges (Min)'),
          'description' => ($is_array && !empty($field['description'])) ? $field['description'] : '',
          'type' => ($is_array && !empty($field['type'])) ? $field['type'] : search_api_extract_inner_type($this->index->options['fields'][$name]['type']),
        );
        $ret[$prefix . $this->max_suffix] = array(
          'label' => ($is_array && !empty($field['name'])) ? $field['name'] . ' (Max)' : t('Search API ranges (Max)'),
          'description' => ($is_array && !empty($field['description'])) ? $field['description'] : '',
          'type' => ($is_array && !empty($field['type'])) ? $field['type'] : search_api_extract_inner_type($this->index->options['fields'][$name]['type']),
        );
      }
    }
    return $ret;
  }

}
