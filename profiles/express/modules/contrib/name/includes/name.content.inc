<?php

/**
 * @file
 * Provides additional Field functionality for the name module.
 *
 * Most of these functions are related to setting configuration for the field,
 * instance and formatter.
 */

/* ----------------------------- Field Code --------------------------------- */

/**
 * Implements hook_field_settings_form().
 */
function _name_field_settings_form($field, $instance, $has_data) {
  $settings = $field['settings'];
  $form = array('#tree' => TRUE);
  $components = _name_translations();
  $form['components'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Components'),
    '#default_value' => $settings['components'],
    '#required' => TRUE,
    '#description' => t('Only selected components will be activated on this field. All non-selected components / component settings will be ignored.'),
    '#options' => $components,
    '#element_validate' => array('_name_field_minimal_component_requirements'),
  );

  $form['minimum_components'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Minimum components'),
    '#default_value' => $settings['minimum_components'],
    '#required' => TRUE,
    '#element_validate' => array('_name_field_minimal_component_requirements'),
    '#description' => t('The minimal set of components required before the field is considered completed enough to save.'),
    '#options' => $components,
  );
  $form['labels'] = array();
  $form['max_length'] = array();
  $form['autocomplete_sources'] = array();
  $autocomplete_sources_options = array();
  if (module_exists('namedb')) {
    $autocomplete_sources_options['namedb'] = t('Names DB');
  }
  $autocomplete_sources_options['title'] = t('Title options');
  $autocomplete_sources_options['generational'] = t('Generational options');
  // TODO: Placing in the to hard basket for the time being!
  //$autocomplete_sources_options['data'] = t('Data');

  $has_data = field_has_data($field);
  foreach ($components as $key => $title) {
    $min_length = 1;
    if ($has_data) {
      $min_length = $settings['max_length'][$key];
      if ($field['storage']['type'] == 'field_sql_storage') {
        try {
          $table = 'field_data_' . $field['field_name'];
          $column = $field['storage']['details']['sql'][FIELD_LOAD_CURRENT]
            [$table][$key];
          $min_length = db_query("SELECT MAX(CHAR_LENGTH({$column})) AS len FROM {$table}")->fetchField();
          if ($min_length < 1) {
            $min_length = 1;
          }
        }
        catch (Exception $e) {
        }
      }
    }
    $form['max_length'][$key] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum length for !title', array('!title' => $title)),
      '#default_value' => $settings['max_length'][$key],
      '#required' => TRUE,
      '#size' => 10,
      '#min_size' => $min_length,
      '#description' => t('The maximum length of the field in characters. This must be between !min and 255.', array('!min' => $min_length)),
      '#element_validate' => array('_name_validate_varchar_range'),
    );
    $form['labels'][$key] = array(
      '#type' => 'textfield',
      '#title' => t('Label for !title', array('!title' => $title)),
      '#default_value' => $settings['labels'][$key],
      '#required' => TRUE,
    );
    $form['autocomplete_source'][$key] = array(
      '#type' => 'checkboxes',
      '#title' => t('Autocomplete options'),
      '#default_value' => $settings['autocomplete_source'][$key],
      '#description' => t("This defines what autocomplete sources are available to the field."),
      '#options' => $autocomplete_sources_options,
    );
    if ($key != 'title') {
      unset($form['autocomplete_source'][$key]['#options']['title']);
    }
    if ($key != 'generational') {
      unset($form['autocomplete_source'][$key]['#options']['generational']);
    }
    $form['autocomplete_separator'][$key] = array(
      '#type' => 'textfield',
      '#title' => t('Autocomplete separator for !title', array('!title' => $title)),
      '#default_value' => $settings['autocomplete_separator'][$key],
      '#size' => 10,
    );
  }

  // TODO - Grouping & grouping sort
  // TODO - Allow reverse free tagging back into the vocabulary.
  $title_options = implode("\n", array_filter(explode("\n", $settings['title_options'])));
  $form['title_options'] = array(
    '#type' => 'textarea',
    '#title' => t('!title options', array('!title' => $components['title'])),
    '#default_value' => $title_options,
    '#required' => TRUE,
    '#description' => t("Enter one !title per line. Prefix a line using '--' to specify a blank value text. For example: '--Please select a !title'.", array('!title' => $components['title'])),
  );
  $generational_options = implode("\n", array_filter(explode("\n", $settings['generational_options'])));
  $form['generational_options'] = array(
    '#type' => 'textarea',
    '#title' => t('!generational options', array('!generational' => $components['generational'])),
    '#default_value' => $generational_options,
    '#required' => TRUE,
    '#description' => t("Enter one !generational suffix option per line. Prefix a line using '--' to specify a blank value text. For example: '----'.", array('!generational' => $components['generational'])),
  );
  if (module_exists('taxonomy')) {
    // TODO - Make the labels more generic.
    // Generational suffixes may be also imported from one or more vocabularies
    // using the tag '[vocabulary:xxx]', where xxx is the vocabulary id. Terms
    // that exceed the maximum length of the generational suffix are not added
    // to the options list.
    $form['title_options']['#description'] .= ' ' . t("%label_plural may be also imported from one or more vocabularies using the tag '[vocabulary:xxx]', where xxx is the vocabulary machine-name or id. Terms that exceed the maximum length of the %label are not added to the options list.",
        array('%label_plural' => t('Titles'), '%label' => t('Title')));
    $form['generational_options']['#description'] .= ' ' . t("%label_plural may be also imported from one or more vocabularies using the tag '[vocabulary:xxx]', where xxx is the vocabulary machine-name or id. Terms that exceed the maximum length of the %label are not added to the options list.",
        array('%label_plural' => t('Generational suffixes'), '%label' => t('Generational suffix')));
  }
  $sort_options = is_array($settings['sort_options']) ? $settings['sort_options'] : array(
    'title' => 'title',
    'generational' => '',
  );
  $form['sort_options'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select field sort options'),
    '#default_value' => $sort_options,
    '#description' => t("This enables sorting on the options after the vocabulary terms are added and duplicate values are removed."),
    '#options' => _name_translations(array('title' => '', 'generational' => '')),
  );

  $form['#element_validate'] = array('_name_field_settings_form_validate');
  return $form;
}

/**
 * Implements the validation callback for the name_field_settings_form() form.
 *
 * This is an #element_validate callback.
 *
 * TODO - Ensure that the #parent path is correct in all instances. This avoids
 *        the need to pull out individual values from the $elements[xx][#value].
 */
function _name_field_settings_form_validate($elements, &$form_state, $form) {
  $values = $form_state['values']['field']['settings'];

  // Validates options against the title / generational sizes.
  _name_options_validate($values['title_options'], $values['max_length']['title'],
      t('Title options'), 'field][settings][title_options');
  _name_options_validate($values['generational_options'], $values['max_length']['generational'],
      t('Generational options'), 'field][settings][generational_options');

  // Validates that a minimum component is checked when that component is not used.
  _name_field_components_validate($values['components'],
      $values['minimum_components'], 'field][settings][minimum_components');
}

// TODO - hook_field_views_data() if required

/**
 * Checks that a component is not selected as a minimal component when the
 * component is not selected.
 */
function _name_field_minimal_component_requirements($element, &$form_state) {
  $required_field_set = array_flip(array('given', 'family'));
  $value = array_intersect_key($required_field_set, array_filter((array) $element['#value']));
  if (empty($value)) {
    $required_fields = array_intersect_key(_name_translations(), $required_field_set);
    form_set_error(implode('][', $element['#parents']) . '][given]',
        t('%label must have one of the following components: %components',
        array('%label' => $element['#title'], '%components' => implode(', ', $required_fields))));
  }
}

/**
 * Checks that individual values in the defined options list do not exceed the
 * limits placed on the component.
 */
function _name_options_validate($options, $max, $label, $error_element) {
  $values = array_filter(explode("\n", $options));
  $long_options = array();
  $valid_options = array();
  $default_options = array();
  foreach ($values as $value) {
    $value = trim($value);
    // Blank option - anything goes!
    if (strpos($value, '--') === 0) {
      $default_options[] = $value;
    }
    // Simple checks on the taxonomy includes.
    elseif (preg_match('/^\[vocabulary:([0-9a-z\_]{1,})\]/', $value, $matches)) {
      if (!module_exists('taxonomy')) {
        form_set_error($error_element, t("The taxonomy module must be enabled before using the '%tag' tag in %label.",
            array('%tag' => $matches[0], '%label' => $label)));
      }
      elseif ($value !== $matches[0]) {
        form_set_error($error_element, t("The '%tag' tag in %label should be on a line by itself.",
            array('%tag' => $matches[0], '%label' => $label)));
      }
      else{
        if (!_name_get_vocabulary_id_by_code_or_number($matches[1])) {
          form_set_error($error_element, t("The vocabulary '%tag' in %label could not be found.",
            array('%tag' => $matches[1], '%label' => $label)));
        }
      }
    }
    elseif (drupal_strlen($value) > $max) {
      $long_options[] = $value;
    }
    elseif (!empty($value)) {
      $valid_options[] = $value;
    }
  }
  if (count($long_options)) {
    form_set_error($error_element, t('The following options exceed the maximun allowed %label length: %options',
        array('%options' => implode(', ', $long_options), '%label' => $label)));
  }
  elseif (empty($valid_options)) {
    form_set_error($error_element, t('%label are required.',
        array('%label' => $label)));
  }
  elseif (count($default_options) > 1) {
    form_set_error($error_element, t('%label can only have one blank value assigned to it.',
        array('%label' => $label)));
  }
}

function _name_field_components_validate($components, $minimum, $error_element) {
  $diff = array_diff_key(array_filter($minimum), array_filter($components));
  if (count($diff)) {
    $components = array_intersect_key(_name_translations(), $diff);
    form_set_error($error_element . '][' . key($diff),
        t('%components can not be selected for %label when they are not selected for %label2.',
        array('%label' => t('Minimum components'), '%label2' => t('Components'),
        '%components' => implode(', ', $components))));
  }
}

/* ----------------------------- Widget Code -------------------------------- */
/**
 * Implements hook_field_instance_settings_form().
 */
function _name_field_instance_settings_form($field, $instance = NULL) {
  $settings = $instance['settings'];
  _name_defaults($settings, 'instance_settings');
  $components = _name_translations();
  $form = array(
    'size' => array(),
    'title_display' => array(),
  );
  $field_options = array(
    'select' => t('Drop-down'),
    'text' => t('Text field'),
    'autocomplete' => t('Autocomplete'));

  foreach ($components as $key => $title) {
    $default_field_type = ($key == 'title' || $key == 'generational')
      ? 'select' : 'text';
    $form['field_type'][$key] = array(
      '#type' => 'radios',
      '#title' => t('!title field type', array('!title' => $components['title'])),
      '#default_value' => isset($settings['field_type'][$key])
          ? $settings['field_type'][$key]
          // Provides backwards compatibility with Drupal 6 modules.
          : (isset($settings[$key . '_field']) ? $settings[$key . '_field'] : $default_field_type),
      '#required' => TRUE,
      '#options' => $field_options,
    );
    if (!($key == 'title' || $key == 'generational')) {
      unset($form['field_type'][$key]['#options']['select']);
    }

    $form['size'][$key] = array(
      '#type' => 'textfield',
      '#title' => t('HTML size property for !title', array('!title' => $title)),
      '#default_value' => isset($settings['size']) ? $settings['size'][$key] : '',
      '#required' => FALSE,
      '#size' => 10,
      '#description' => t('The maximum length of the field in characters. This must be between 1 and 255.'),
      '#element_validate' => array('_element_validate_integer_positive'),
    );
    $form['title_display'][$key] = array(
      '#type' => 'radios',
      '#title' => t('Label display for !title', array('!title' => $title)),
      '#default_value' => isset($settings['title_display'])
          ? $settings['title_display'][$key] : 'description',
      '#options' => array(
        'title' => t('above'),
        'description' => t('below'),
        'none' => t('hidden'),
      ),
      '#description' => t('This controls how the label of the component is displayed in the form.'),
    );
    $form['inline_css'][$key] = array(
      '#type' => 'textfield',
      '#title' => t('Additional inline styles for !title input element.', array('!title' => $title)),
      '#default_value' => isset($settings['inline_css']) ? $settings['inline_css'][$key] : '',
      '#size' => 8,
    );
  }

  $form['component_css'] = array(
    '#type' => 'textfield',
    '#title' => t('Component separator CSS'),
    '#default_value' => empty($settings['component_css']) ? '' : $settings['component_css'],
    '#description' => t('Use this to override the default CSS used when rendering each component. Use "&lt;none&gt;" to prevent the use of inline CSS.'),
  );

  $form['credentials_inline'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show the credentials inline'),
    '#default_value' => empty($settings['credentials_inline']) ? 0 : 1,
    '#description' => t('The default position is to show the credentials on a line by themselves. This option overrides this to render the component inline.'),
  );

  // Add the overwrite user name option.
  if ($instance['entity_type'] == 'user' && $instance['bundle'] == 'user') {
    $preferred_field = variable_get('name_user_preferred', '');
    $form['name_user_preferred'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use this field to override the users login name?'),
      '#default_value' => $preferred_field == $instance['field_name'] ? 1 : 0,
    );
    $form['override_format'] = array(
      '#type' => 'select',
      '#title' => t('User name override format to use'),
      '#default_value' => isset($settings['override_format']) ? $settings['override_format'] : 'default',
      '#options' => array('default' => t('Default')) + name_get_custom_format_options(),
    );
  }
  else {
    // We may extend this feature to Profile2 latter.
    $form['override_format'] = array(
      '#type' => 'value',
      '#value' => isset($settings['override_format']) ? $settings['override_format'] : 'default',
    );
  }
  
  return $form;

}

/**
 * Implements hook_field_widget_form().
 */
function _name_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $widget = $instance['widget'];
  _name_defaults($instance['settings'], 'instance_settings');
  _name_defaults($field['settings'], 'settings');
  $fs = $field['settings'];
  $ws = $instance['settings'];

  $element += array(
    '#type' => 'name_element',
    '#title' => $instance['label'],
    '#label' => $instance['label'],
    '#components' => array(),
    '#minimum_components' => array_filter($fs['minimum_components']),
    '#default_value' => isset($items[$delta]) ? $items[$delta] : NULL,
    '#field' => $field,
    '#credentials_inline' => empty($ws['credentials_inline']) ? 0 : 1,
    '#component_css' => empty($ws['component_css']) ? '' : $ws['component_css'],
  );

  $components = array_filter($fs['components']);
  foreach (_name_translations() as $key => $title) {
    if (in_array($key, $components)) {
      $element['#components'][$key]['type'] = 'textfield';

      $size = !empty($ws['size'][$key]) ? $ws['size'][$key] : 60;
      $title_display = isset($ws['title_display'][$key]) ? $ws['title_display'][$key] : 'description';

      $element['#components'][$key]['title'] = check_plain($fs['labels'][$key]);
      $element['#components'][$key]['title_display'] = $title_display;

      $element['#components'][$key]['size'] = $size;
      $element['#components'][$key]['maxlength'] = !empty($fs['max_length'][$key]) ? $fs['max_length'][$key] : 255;


      // Provides backwards compatibility with Drupal 6 modules.
      $field_type = ($key == 'title' || $key == 'generational') ? 'select' : 'text';
      $field_type = isset($ws['field_type'][$key])
          ? $ws['field_type'][$key]
          // Provides .
          : (isset($ws[$key . '_field']) ? $ws[$key . '_field'] : $field_type);


      if ($field_type == 'select') {
        $element['#components'][$key]['type'] = 'select';
        $element['#components'][$key]['size'] = 1;
        $element['#components'][$key]['options'] = _name_field_get_options($field, $key);
      }
      elseif ($field_type == 'autocomplete') {
        if ($sources = $field['settings']['autocomplete_source'][$key]) {
          $sources = array_filter($sources);
          if (!empty($sources)) {
            $element['#components'][$key]['autocomplete'] = 'name/autocomplete/'
                . str_replace('_', '-', $field['field_name']) . '/' . $key;
          }
        }
      }

      if (isset($ws['inline_css'][$key]) && drupal_strlen($ws['inline_css'][$key])) {
        $element['#components'][$key]['attributes'] = array(
          'style' => $ws['inline_css'][$key],
        );
      }
    }
    else {
      $element['#components'][$key]['exclude'] = TRUE;
    }

  }

  return $element;
}

function _name_field_get_options($field, $key) {
  _name_defaults($field['settings'], 'settings');
  $fs = $field['settings'];

  $options = array_filter(explode("\n", $fs[$key . '_options']));
  foreach ($options as $index => $opt) {
    if (preg_match('/^\[vocabulary:([0-9a_z\_]{1,})\]/', trim($opt), $matches)) {
      unset($options[$index]);
      if (module_exists('taxonomy')) {
        if ($vid = _name_get_vocabulary_id_by_code_or_number($matches[1])) {
          $max_length = isset($fs['max_length'][$key]) ? $fs['max_length'][$key] : 255;
          foreach (taxonomy_get_tree($vid) as $term) {
            if (drupal_strlen($term->name) <= $max_length) {
              $options[] = $term->name;
            }
          }
        }
      }
    }
  }
  // Options could come from multiple sources, filter duplicates.
  $options = array_unique($options);

  if ($fs && isset($fs['sort_options']) && !empty($fs['sort_options'][$key])) {
    natcasesort($options);
  }
  $default = FALSE;
  foreach ($options as $index => $opt) {
    if (strpos($opt, '--') === 0) {
      unset($options[$index]);
      $default = drupal_substr($opt, 2);
    }
  }
  $options = drupal_map_assoc(array_map('trim', $options));
  if ($default !== FALSE) {
    $options = array('' => $default) + $options;
  }
  return $options;
}
/* ---------------------------- Formatter Code ------------------------------ */

/* --------------------------- Helper Functions ----------------------------- */

/**
 * Helper function to set the defaults for a name field / widget.
 */
function _name_defaults(&$settings, $key) {
  $name_info = name_field_info();
  $settings = isset($settings) ? (array) $settings : array();
  foreach ($name_info['name'][$key] as $index => $defaults) {
    if (!isset($settings[$index])) {
      if (is_array($defaults)) {
        if (!array_key_exists($index, $settings)) {
          $settings[$index] = array();
        }
        $settings[$index] += $defaults;
      }
      else {
        $settings[$index] = $defaults;
      }
    }
  }
}

/**
 * Helper form element validator: integer > 0 && <= 255.
 */
function _name_validate_varchar_range($element, &$form_state) {
  $value = $element['#value'];
  $min = isset($element['#min_size']) ? $element['#min_size'] : 1;
  if ($value !== '' && (!is_numeric($value) || intval($value) != $value || $value < $min || $value > 255)) {
    form_error($element, t('%name must be a positive integer between !min and 255.', array('%name' => $element['#title'], '!min' => $min)));
  }
}

/**
 * Helper function to get the vid from a machine name or number.
 */
function _name_get_vocabulary_id_by_code_or_number($value) {
  $vid = FALSE;
  if (empty($value)) {
    return $vid;
  }
  if (intval($value) == $value && $vocab = taxonomy_vocabulary_load($value)) {
    $vid = $value;
  }
  elseif ($vocab = taxonomy_vocabulary_machine_name_load($value)) {
    $vid = $vocab->vid;
  }
  return $vid;
}