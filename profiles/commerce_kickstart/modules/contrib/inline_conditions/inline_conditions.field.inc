<?php

/**
 * @file
 * Field module functionality for the Inline Conditions module.
 */

/**
 * Implements hook_field_info().
 */
function inline_conditions_field_info() {
  return array(
    'inline_conditions' => array(
      'label' => t('Inline conditions'),
      'description' => t('This field stores conditions that are later added to a matching rule.'),
      // entity_type is the type of the entity operated on by the matching rule.
      'instance_settings' => array('entity_type' => NULL),
      'default_widget' => 'inline_conditions',
      'default_formatter' => 'hidden',
      'no_ui' => TRUE,
      'property_type' => 'inline_conditions',
      'property_callbacks' => array('inline_conditions_field_property_callback'),
    ),
  );
}

/**
 * Implements hook_field_widget_info().
 */
function inline_conditions_field_widget_info() {
  return array(
    'inline_conditions' => array(
      'label' => t('Inline conditions'),
      'field types' => array('inline_conditions'),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_load().
 *
 * Prepare items array in order to be usable with inline_condition field widget.
 */
function inline_conditions_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  // Loop on every entities given.
  foreach ($entities as $id => $entity) {
    // Ensures that field is inline_conditions type.
    if ($field['type'] == 'inline_conditions') {

      foreach ($items[$id] as $delta => $item) {
        // Ensure condition_settings is unserialised.
        if (is_string($item['condition_settings'])) {
          // Unserialize the field settings.
          $item['condition_settings'] = unserialize($item['condition_settings']);

          // Look up for the value of the logic operators.
          if (isset($item['condition_settings']['condition_negate'])) {
            $item['condition_negate'] = $item['condition_settings']['condition_negate'];
            unset($item['condition_settings']['condition_negate']);
          }
          if (isset($item['condition_settings']['condition_logic_operator'])) {
            $item['condition_logic_operator'] = $item['condition_settings']['condition_logic_operator'];
            unset($item['condition_settings']['condition_logic_operator']);
          }

          // Replace item value.
          $items[$id][$delta] = $item;
        }
      }
    }
  }
}

/**
 * Implements hook_field_insert().
 *
 * Call inline_conditions_prepare_field_items() in order to rebuild items.
 *
 * @see inline_conditions_field_prepare_items()
 */
function inline_conditions_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // Ensures that field is inline_conditions type.
  if ($field['type'] == 'inline_conditions') {
    inline_conditions_field_prepare_items($items);
  }
}

/**
 * Implements hook_field_update().
 *
 * Call inline_conditions_prepare_field_items() in order to rebuild items.
 *
 * @see inline_conditions_field_prepare_items()
 */
function inline_conditions_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // Ensures that field is inline_conditions type.
  if ($field['type'] == 'inline_conditions') {
    inline_conditions_field_prepare_items($items);
  }
}

/**
 * Implements hook_field_is_empty().
 */
function inline_conditions_field_is_empty($item, $field) {
  return empty($item['condition_name']);
}

/**
 * Alter $items array and prepare it to be saved.
 *
 * Serialize the condition_settings column.
 *
 * @param array $items
 *   A referenced array of field items.
 *
 * @see inline_conditions_field_load()
 * @see inline_conditions_field_insert()
 */
function inline_conditions_field_prepare_items(&$items) {
  // A simple way to check if array is a multi-dimensional array.
  if (is_array($items)) {
    foreach ($items as $delta => $item) {
      // Ensures that $item has a condition name.
      if (!empty($item['condition_name'])) {
        // Ensure that condition_settings is not serialized.
        if (is_array($item['condition_settings'])) {
          $condition_settings = array_merge($item['condition_settings'], array(
            // Store the rule condition logic operators.
            'condition_negate' => isset($item['condition_negate']) ? $item['condition_negate'] : NULL,
            'condition_logic_operator' => isset($item['condition_logic_operator']) ? $item['condition_logic_operator'] : NULL,
          ));
          $item['condition_settings'] = serialize($condition_settings);
        }
        // Replace item value.
        $items[$delta] = $item;
      }
    }
  }
}

/**
 * Implements hook_field_widget_form().
 */
function inline_conditions_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Ensure this .inline_conditions files are loaded when the form is rebuilt
  // from the cache.
  foreach (inline_conditions_get_info_by_module() as $module => $condition) {
    $form_state['build_info']['files']["form_ic_$module"] = drupal_get_path('module', $module) . "/$module.inline_conditions.inc";
  }

  // Prepare a list of all possible conditions.
  $condition_info = inline_conditions_get_info_by_type(empty($instance['settings']['entity_type']) ? 'commerce_order' : $instance['settings']['entity_type'], $instance['entity_type']);

  // Build main wrapper id.
  $main_wrapper_id = sprintf('inline-conditions-%s', $field['field_name']);

  $element = array(
    '#type' => 'container',
    '#id' => $main_wrapper_id,
    '#caption' => $element['#title'],
    '#theme' => 'inline_conditions_table',
    '#element_validate' => array('inline_conditions_field_widget_form_validate'),
  ) + $element;

  // Attach module style sheet to widget form.
  $form['#attached']['css'][] = drupal_get_path('module', 'inline_conditions') . '/css/inline_conditions.css';

  // If "Add a new condition" button has been pressed.
  if (isset($form_state['triggering_element']['#condition_action'])) {
    switch ($form_state['triggering_element']['#condition_action']) {
      case 'and':
        $items[] = array(
          'condition_name' => '',
          'condition_settings' => array(),
          'condition_logic_operator' => INLINE_CONDITIONS_AND,
        );
        break;

      case 'or':
        $items[] = array(
          'condition_name' => '',
          'condition_settings' => array(),
          'condition_logic_operator' => INLINE_CONDITIONS_OR,
        );
        break;

      case 'remove':
        unset($items[$form_state['triggering_element']['#element_delta']]);
        // Rebuild array keys.
        $items = array_values($items);
        break;
    }
  }

  // If no items found, create an empty item for display purposes.
  if (empty($items)) {
    $items[] = array('condition_name' => '', 'condition_settings' => array());
  }

  foreach ($items as $delta => $item) {
    $settings_wrapper_id = sprintf('inline-conditions-settings-wrapper-%s-%s', $instance['id'], $delta);

    // Condition selector.
    $element[$delta]['condition_name'] = array(
      '#type' => 'select',
      '#options' => inline_conditions_options_list($condition_info),
      '#default_value' => $item['condition_name'],
      '#ajax' => array(
        'callback' => 'inline_conditions_form_ajax_callback',
        'wrapper' => $settings_wrapper_id,
      ),
      '#condition_action' => 'select',
      // Identifies the condition operated upon.
      '#element_delta' => $delta,
      '#limit_validation_errors' => array(),
    );

    // Condition settings.
    $element[$delta]['condition_settings'] = array(
      '#type' => 'value',
      '#value' => array(),
    );

    // A condition has been selected, and a "configure" callback exists. Fills
    // up the condition_settings key with form elements returned by condition
    // configure callback.
    if (!empty($item['condition_name']) && !empty($condition_info[$item['condition_name']]['callbacks']['configure'])) {
      $callback = $condition_info[$item['condition_name']]['callbacks']['configure'];
      $element[$delta]['condition_settings'] = $callback($item['condition_settings'], $instance, $delta);
      $element[$delta]['#attributes'] = array('class' => array('container-inline'));
    }

    // Merge additional properties for the ajax wrapping.
    $element[$delta]['condition_settings'] += array(
      '#prefix' => sprintf('<div id="%s" class="%s">', $settings_wrapper_id, 'inline-conditions-settings-wrapper'),
      '#suffix' => '</div>',
    );

    $element[$delta]['condition_negate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Negate'),
      '#default_value' => !empty($item['condition_negate']) ? $item['condition_negate'] : FALSE,
    );

    // Remove condition button.
    $element[$delta]['remove_condition'] = array(
      '#type' => 'button',
      '#name' => $field['field_name'] . '-' . $instance['id'] . '-' . $delta . '-remove_condition',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'inline_conditions_form_ajax_callback',
        'wrapper' => 'inline-conditions-' . $field['field_name'],
        'effect' => 'fade',
      ),
      '#element_delta' => $delta,
      '#condition_action' => 'remove',
      '#executes_submit_callback' => FALSE,
      '#limit_validation_errors' => array(),
    );

    // Add an option list to select logical disjunctions for each inline
    // condition.
    if ($delta > 0) {
      // Add an option list to select the logic operator for the current
      // condition.
      $element[$delta]['condition_logic_operator'] = array(
        '#type' => 'select',
        '#options' => array(
          INLINE_CONDITIONS_OR => t('OR'),
          INLINE_CONDITIONS_AND => t('AND'),
        ),
        '#default_value' => isset($item['condition_logic_operator']) ? $item['condition_logic_operator'] : INLINE_CONDITIONS_AND,
        '#attributes' => array('class' => array('inline-conditions-logic-operator')),
        '#weight' => -50,
      );
    }
  }

  // Prepare the action buttons.
  $element['and_condition'] = array(
    '#type' => 'button',
    '#name' => sprintf('%s-%s-and-condition', $field['field_name'], $instance['id']),
    '#value' => t('Attach AND condition'),
    '#ajax' => array(
      'callback' => 'inline_conditions_form_ajax_callback',
      'wrapper' => $main_wrapper_id,
      'effect' => 'fade',
    ),
    '#attributes' => array('class' => array('and-condition')),
    '#element_delta' => $element['#delta'],
    '#condition_action' => 'and',
    '#limit_validation_errors' => array(),
  );
  $element['or_condition'] = array(
    '#type' => 'button',
    '#name' => sprintf('%s-%s-or-condition', $field['field_name'], $instance['id']),
    '#value' => t('Attach OR condition'),
    '#ajax' => array(
      'callback' => 'inline_conditions_form_ajax_callback',
      'wrapper' => $main_wrapper_id,
      'effect' => 'fade',
    ),
    '#attributes' => array('class' => array('or-condition')),
    '#element_delta' => $element['#delta'],
    '#condition_action' => 'or',
    '#limit_validation_errors' => array(),
  );

  return $element;
}

/**
 * Callback : Validate inline_conditions_field_widget_form.
 *
 * @param array $element
 *   A form element array containing basic properties for the widget:
 *   - #entity_type: The name of the entity the field is attached to.
 *   - #bundle: The name of the field bundle the field is contained in.
 *   - #field_name: The name of the field.
 *   - #language: The language the field is being edited in.
 *   - #field_parents: The 'parents' space for the field in the form. Most
 *       widgets can simply overlook this property. This identifies the
 *       location where the field values are placed within
 *       $form_state['values'], and is used to access processing information
 *       for the field through the field_form_get_state() and
 *       field_form_set_state() functions.
 *   - #columns: A list of field storage columns of the field.
 *   - #title: The sanitized element label for the field instance, ready for
 *     output.
 *   - #description: The sanitized element description for the field instance,
 *     ready for output.
 *   - #required: A Boolean indicating whether the element value is required;
 *     for required multiple value fields, only the first widget's values are
 *     required.
 *   - #delta: The order of this item in the array of subelements; see $delta
 *     above.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param array $form
 *   The form structure where widgets are being attached to. This might be a
 *   full form structure, or a sub-element of a larger form.
 */
function inline_conditions_field_widget_form_validate($element, &$form_state, $form) {
  // Remove action buttons values from the form state values.
  $value = drupal_array_get_nested_value($form_state['values'], $element['#array_parents']);
  $value = array_intersect_key($value, array_flip(array_filter(array_keys($value), 'is_numeric')));
  drupal_array_set_nested_value($form_state['values'], $element['#array_parents'], $value, TRUE);
}

/**
 * Ajax callback for the Inline Conditions form elements.
 *
 * @param array $form
 *   The form array.
 * @param array &$form_state
 *   The reference of form_state array.
 *
 * @return array
 *   Return form element to display.
 */
function inline_conditions_form_ajax_callback($form, &$form_state) {
  $element_parents = array_slice($form_state['triggering_element']['#array_parents'], 0, -2);
  $element = drupal_array_get_nested_value($form, $element_parents);

  // Triggered when user selects a condition.
  if ($form_state['triggering_element']['#condition_action'] == 'select') {
    $delta = $form_state['triggering_element']['#element_delta'];
    // We are returning the condition settings because it's a dynamic element,
    // other elements don't need to be refreshed.
    return $element[$delta]['condition_settings'];
  }
  else {
    return $element;
  }
}
