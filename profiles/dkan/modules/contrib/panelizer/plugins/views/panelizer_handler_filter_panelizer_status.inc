<?php
/**
 * Filter by node type
 */
class panelizer_handler_filter_panelizer_status extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Panelizer defaults');
      $entity_info = entity_get_info($this->definition['entity_type']);

      // The normal get_panelizer_default_objects() requires a bundle type
      // but in a filter all we know for sure is the entity type. This
      // is going to make a bit of an awkward list, unfortunately.
      $conditions = array(
        'panelizer_type' => $this->definition['entity_type'],
      );

      ctools_include('export');
      $defaults = ctools_export_load_object('panelizer_defaults', 'conditions', $conditions);

      foreach ($defaults as $name => $default) {
        if (empty($default->title)) {
          $default->title = t('Default');
        }
        $options[$name] = t('@bundle: @title', array('@bundle' => $entity_info['bundles'][$default->panelizer_key]['label'], '@title' => $default->title));
      }
      natcasesort($options);
      $options = array(
        'not' => t('Not panelized'),
        'custom' => t('Customized'),
      ) + $options;

      $this->value_options = $options;
    }
  }

  function op_simple() {
    if (empty($this->value)) {
      return;
    }
    $this->ensure_my_table();

    $or = db_or();
    $values = $this->value;

    // @todo -- see if we need to use in_array() when a select is used.
    if (in_array('not', $values)) {
      if ($this->operator == 'in') {
        $or->condition(
          db_and()
            ->condition("$this->table_alias.$this->real_field", NULL)
            ->condition("$this->table_alias.did", 0)
        );
      }
      else {
        $or->condition(
          db_or()
            ->isNotNull("$this->table_alias.$this->real_field")
            ->condition(db_and()
              ->isNull("$this->table_alias.$this->real_field")
              ->condition("$this->table_alias.did", 0, '!=')
            )
        );
      }
      unset($values['not']);
    }

    if (in_array('custom', $values)) {
      if ($this->operator == 'in') {
        $or->condition(
          db_and()
            ->condition("$this->table_alias.$this->real_field", NULL)
            ->condition("$this->table_alias.did", 0, '!=')
        );
      }
      else {
        $or->condition(
          db_or()
            ->isNotNull("$this->table_alias.$this->real_field")
            ->condition(db_and()
              ->isNull("$this->table_alias.$this->real_field")
              ->condition("$this->table_alias.did", 0, '=')
            )
        );
      }
      unset($values['custom']);
    }

    if ($values) {
      $or->condition("$this->table_alias.$this->real_field", array_values($values), $this->operator);
    }

    // We use array_values() because the checkboxes keep keys and that can cause
    // array addition problems.
    $this->query->add_where($this->options['group'], $or);
  }

}
