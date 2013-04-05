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
      if (search_api_is_list_type($field['type']) && in_array(search_api_extract_inner_type($field['type']), $eligible_types))  {
        $field_options[$name] = $field['name'];
      }
    }
    if (!empty($field_options)) {
      $form['#attached']['css'][] = drupal_get_path('module', 'search_api') . '/search_api.admin.css';
      $form['fields'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Fields to run on'),
        '#options' => $field_options,
        '#default_value' => $this->options['fields'],
        '#attributes' => array('class' => array('search-api-checkboxes-list')),
      );
      return $form;
    }
  }

  /**
   * Submit callback for configuration form.
   */
  public function configurationFormSubmit(array $form, array &$values, array &$form_state) {
    $save_index = FALSE;
    $this->options = $values;
    if (!empty($values['fields'])) {
      $values['fields'] = array_filter($values['fields']);
      if (!isset($this->options['fields']) || ($values['fields'] != $this->options['fields'])) {
        foreach ($values['fields'] as $field) {
          $prefix = str_replace(':', '_', $field);
          $type = search_api_extract_inner_type($this->index->options['fields'][$field]['type']);
          $this->options['fields'][$prefix . $this->min_suffix] = array(
            'type' => $type,
          );
          $this->options['fields'][$prefix . $this->max_suffix] = array(
            'type' => $type,
          );
        }
        $save_index = TRUE;
      }
    }
    // Remove non wanted anymore min and max alterations
    if (!empty($this->options['fields'])) {
      $alteration_to_remove = array();
      if (empty($values['fields'])) {
        $alteration_to_remove = $this->options['fields'];
        $save_index = TRUE;
      }
      else {
        $alteration_to_remove = array_diff_key($this->options['fields'], $values['fields']);
        $save_index = TRUE;
      }
      foreach ($alteration_to_remove as $key => $value) {
        $prefix = str_replace(':', '_', $key);
        if (isset($this->index->options['fields'][$prefix . $this->min_suffix])) {
          unset($this->index->options['fields'][$prefix . $this->min_suffix]);
          unset($this->index->options['fields'][$prefix . $this->max_suffix]);
        }
      }
    }
    if ($save_index) {
      $this->index->save();
    }
    return $values;
  }

  public function alterItems(array &$items) {
    if (!$items) {
      return;
    }
    if (!empty($this->options['fields'])) {
      foreach ($this->options['fields'] as $field) {
        $required_fields[$field] = array(
          'type' => search_api_extract_inner_type($this->index->options['fields'][$field]['type']),
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
            $item->{$name . '_asc'} = min($f['value']);
            $item->{$name . '_desc'} = max($f['value']);
          }
        }
      }
    }
  }

  public function propertyInfo() {
    $ret = array();
    if (!empty($this->options['fields'])) {
      $index_fields = $this->index->getFields(TRUE);;
      foreach ($this->options['fields'] as $name => $field) {
        if (isset($index_fields[$name])) {
          $prefix = str_replace(':', '_', $name);
          $ret[$prefix . $this->min_suffix] = array(
            'label' => $index_fields[$name]['name'] . ' (Min)',
            'description' => empty($index_fields[$name]['description']) ? '' : $index_fields[$name]['description'],
            'type' => search_api_extract_inner_type($index_fields[$name]['type']),
          );
          $ret[$prefix . $this->max_suffix] = array(
            'label' => $index_fields[$name]['name'] . ' (Max)',
            'description' => empty($index_fields[$name]['description']) ? '' : $index_fields[$name]['description'],
            'type' => search_api_extract_inner_type($index_fields[$name]['type']),
          );
        }
      }
    }
    return $ret;
  }

}
