<?php

/**
 * @file
 * Definition of commerce_backoffice_handler_filter_term_node_tid.
 *
 * This is a copy of views_handler_filter_term_node_tid that uses subqueries
 * instead of joins and allows terms from multiple vocabularies.
 */

/**
 * Filter by term id.
 */
class commerce_backoffice_handler_filter_term_node_tid extends views_handler_filter {
  // Stores the exposed input for this filter.
  var $validated_exposed_input = NULL;

  function option_definition() {
    $options = parent::option_definition();

    $options['per_vocabulary'] = array('default' => TRUE);
    $options['hierarchy'] = array('default' => 0);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['per_vocabulary'] = array(
      '#type' => 'checkbox',
      '#title' => t('Require all terms to be present'),
      '#description' => t('Shows only content that has all of the selected terms.'),
      '#default_value' => !empty($this->options['per_vocabulary']),
      '#dependency' => array('radio:options[type]' => array('select')),
    );

    $form['hierarchy'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show hierarchy in dropdown'),
      '#default_value' => !empty($this->options['hierarchy']),
      '#dependency' => array('radio:options[type]' => array('select')),
    );
  }

  function value_form(&$form, &$form_state) {
    $vocabularies = commerce_backoffice_get_vocabularies($this->definition['product display']);
    if (empty($vocabularies)) {
      $form['markup'] = array(
        '#markup' => '<div class="form-item">' . t('No terms found in the provided vocabularies.') . '</div>',
      );
      $form['value'] = array();
      return;
    }

    $options = array();
    foreach ($vocabularies as $vocabulary) {
      $options[$vocabulary->name] = array();
      $vocabularies_names[] = $vocabulary->name;
      if (!empty($this->options['hierarchy'])) {
        $tree = taxonomy_get_tree($vocabulary->vid);
        if ($tree) {
          foreach ($tree as $term) {
            $choice = new stdClass();
            if (!empty($this->options['per_vocabulary'])) {
              $choice->option = array($term->vid . '-' . $term->tid => str_repeat('-', $term->depth) . $term->name);
            }
            else {
              $choice->option = array($term->tid => str_repeat('-', $term->depth) . $term->name);
            }
            $options[$vocabulary->name][] = $choice;
          }
        }
      }
      else {
        $query = db_select('taxonomy_term_data', 'td');
        $query->innerJoin('taxonomy_vocabulary', 'tv', 'td.vid = tv.vid');
        $query->fields('td');
        $query->orderby('tv.weight');
        $query->orderby('tv.name');
        $query->orderby('td.weight');
        $query->orderby('td.name');
        $query->addTag('term_access');
        $query->condition('tv.machine_name', $vocabulary->machine_name);
        $result = $query->execute();
        foreach ($result as $term) {
          if (!empty($this->options['per_vocabulary'])) {
            $options[$vocabulary->name][$term->vid . '-' . $term->tid] = $term->name;
          }
          else {
            $options[$vocabulary->name][$term->tid] = $term->name;
          }
        }
      }
    }

    $default_value = (array) $this->value;

    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];

      if (!empty($this->options['expose']['reduce'])) {
        $options = $this->reduce_value_options($options);

        if (!empty($this->options['expose']['multiple']) && empty($this->options['expose']['required'])) {
          $default_value = array();
        }
      }

      if (empty($this->options['expose']['multiple'])) {
        if (empty($this->options['expose']['required']) && (empty($default_value) || !empty($this->options['expose']['reduce']))) {
          $default_value = 'All';
        }
        elseif (empty($default_value)) {
          $keys = array_keys($options);
          $default_value = array_shift($keys);
        }
        // Due to #1464174 there is a chance that array('') was saved in the admin ui.
        // Let's choose a safe default value.
        elseif ($default_value == array('')) {
          $default_value = 'All';
        }
        else {
          $copy = $default_value;
          $default_value = array_shift($copy);
        }
      }
    }
    $form['value'] = array(
      '#type' => 'select',
      '#title' => t('Select terms'),
      '#multiple' => TRUE,
      '#options' => $options,
      '#size' => min(9, count($options, COUNT_RECURSIVE)),
      '#default_value' => $default_value,
    );

    if (!empty($form_state['exposed']) && isset($identifier) && !isset($form_state['input'][$identifier])) {
      $form_state['input'][$identifier] = $default_value;
    }
  }

  function accept_exposed_input($input) {
    if (empty($this->options['exposed'])) {
      return TRUE;
    }

    // If view is an attachment and is inheriting exposed filters, then assume
    // exposed input has already been validated
    if (!empty($this->view->is_attachment) && $this->view->display_handler->uses_exposed()) {
      $this->validated_exposed_input = (array) $this->view->exposed_raw_input[$this->options['expose']['identifier']];
    }

    // If it's non-required and there's no value don't bother filtering.
    if (!$this->options['expose']['required'] && empty($this->validated_exposed_input)) {
      return FALSE;
    }

    $rc = parent::accept_exposed_input($input);
    if ($rc) {
      // If we have previously validated input, override.
      if (isset($this->validated_exposed_input)) {
        $this->value = $this->validated_exposed_input;
      }
    }

    return $rc;
  }

  function exposed_validate(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }

    $identifier = $this->options['expose']['identifier'];
    if (isset($form_state['values'][$identifier]) && $form_state['values'][$identifier] != 'All')  {
      $this->validated_exposed_input = (array) $form_state['values'][$identifier];
    }
    return;
  }

  function value_submit($form, &$form_state) {
    // prevent array_filter from messing up our arrays in parent submit.
  }

  function admin_summary() {
    return '';
  }

  function query() {
    // If no filter values are present, then do nothing.
    if (count($this->value) == 0) {
      return;
    }
    elseif (count($this->value) == 1) {
      // Somethis $this->value is an array with a single element so convert it.
      if (is_array($this->value)) {
        $this->value = current($this->value);
      }
      $operator = '=';
    }
    else {
      $operator = 'IN';
    }

    // The normal use of ensure_my_table() here breaks Views.
    // So instead we trick the filter into using the alias of the base table.
    // See http://drupal.org/node/271833
    // If a relationship is set, we must use the alias it provides.
    if (!empty($this->relationship)) {
      $this->table_alias = $this->relationship;
    }
    // If no relationship, then use the alias of the base table.
    elseif (isset($this->query->table_queue[$this->query->base_table]['alias'])) {
      $this->table_alias = $this->query->table_queue[$this->query->base_table]['alias'];
    }
    // This should never happen, but if it does, we fail quietly.
    else {
      return;
    }

    if (empty($this->options['per_vocabulary']) || $operator == '=') {
      if (!is_array($this->value) && $value_parts = explode('-', $this->value)) {
        $tid = $value_parts[1];
      }
      else {
        $tid = $this->value;
      }
      $subquery = db_select('taxonomy_index', 'tn');
      $subquery->addField('tn', 'nid');
      $last = 'tn';
      $subquery->condition('tn.tid', $tid, $operator);
      $this->query->add_where($this->options['group'], "$this->table_alias.nid", $subquery, 'IN');
    }
    else {
      $values = array();
      foreach ($this->value as $value) {
        $value_parts = explode('-', $value);
        $values[$value_parts[0]][] = $value_parts[1];
      }
      foreach ($values as $vid => $tids) {
        // Now build the subqueries.
        $subquery = db_select('taxonomy_index', 'tn');
        $subquery->addField('tn', 'nid');
        $last = 'tn';
        $subquery->innerJoin('taxonomy_term_data', 'td', 'td.tid = tn.tid');
        $last = 'td';
        $subquery->condition('tn.tid', array_values($tids), $operator);
        $subquery->condition('td.vid', $vid);
        $this->query->add_where($this->options['group'], "$this->table_alias.nid", $subquery, 'IN');
      }
    }
  }
}
