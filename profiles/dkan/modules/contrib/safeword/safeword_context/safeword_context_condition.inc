<?php

class safeword_context_condition extends context_condition {

  function condition_values() {
    return array();
  }

  /**
   * Condition form.
   */
  function condition_form($context) {
    $form = parent::condition_form($context);
    unset($form['#options']);

    $values = $this->fetch_from_context($context, 'values');

    $form['#type']          = 'textfield';
    $form['#default_value'] = reset($values);

    return $form;
  }

  /**
   * Condition form submit handler.
   */
  function condition_form_submit($values) {
    return (array)$values;
  }

  function options_form($context) {
    $entity_types = array_map(function ($type) {
      return $type['label'];
    }, entity_get_info());

    $options = $this->fetch_from_context($context, 'options');

    return array(
      'entity_types' => array(
        '#title'            => t('Only entity types'),
        '#type'             => 'checkboxes',
        '#options'          => $entity_types,
        '#description'      => t('Leave empty to match all entity types.'),
        '#default_value'    => isset($options['entity_types']) ? array_filter($options['entity_types']) : array(),

        // This gets rid of all the unchecked options that Drupal would normally keep: node => 0, file => 'file' etc.
        '#element_validate' => array(function ($element, &$form_state) {
          form_set_value($element, array_filter($element['#value']), $form_state);
        }),
      ),
    );
  }

  function execute($entity_type, $entity, $op = 'view') {
    // Only 'view' for now.
    if ($op != 'view') {
      return;
    }

    list(, , $bundle) = entity_extract_ids($entity_type, $entity);
    $fields_map = field_info_field_map();

    foreach ($fields_map as $field_name => $field_usage) {
      if ($field_usage['type'] == 'safeword' || $field_usage['type'] == 'safeword_title') {
        if (isset($field_usage['bundles'][$entity_type]) && in_array($bundle, $field_usage['bundles'][$entity_type])) {
          $items = field_get_items($entity_type, $entity, $field_name);
          if ($items) {
            $machine_name = $items[0]['machine'];

            $contexts = $this->get_contexts($machine_name);
            foreach ($contexts as $name => $context) {
              $options = $this->fetch_from_context($context, 'options');
              if (empty($options['entity_types']) || !empty($options['entity_types'][$entity_type])) {
                $this->condition_met($context, $machine_name);
              }
            }
          }

          break;
        }
      }
    }
  }

}
