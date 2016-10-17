<?php

/**
 * @file
 * Webform module grid component.
 */

// Grid depends on functions provided by select.
webform_component_include('select');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_grid() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'required' => 0,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'options' => '',
      'questions' => '',
      'optrand' => 0,
      'qrand' => 0,
      'unique' => 0,
      'title_display' => 0,
      'custom_option_keys' => 0,
      'custom_question_keys' => 0,
      'sticky' => TRUE,
      'description' => '',
      'description_above' => FALSE,
      'private' => FALSE,
      'analysis' => TRUE,
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_grid() {
  return array(
    'webform_grid' => array(
      'render element' => 'element',
      'file' => 'components/grid.inc',
    ),
    'webform_display_grid' => array(
      'render element' => 'element',
      'file' => 'components/grid.inc',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_grid($component) {
  $form = array();

  $form['help'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => !empty($component['cid']),
    '#title' => t('About options and questions&hellip;'),
    '#description' => t('Options and questions may be configured here, in additional nested Select Options components, or even both.'),
    '#weight' => -4,
    'pros_and_cons' => array(
      '#theme' => 'table',
      '#header' => array('', t('Options and questions configured <strong>here</strong>'), t('Configured in additional <strong>nested</strong> components'), t('Both')),
      '#rows' => array(
        array(t('Questions'), t('Enter the questions below.'), t('Configure and save this grid, then add additional Select Options components nested (indented) below this grid.'), t('Additional questions from nested components will be displayed below any questions configured here.')),
        array(t('Options'), t('Enter options below.'), t('May be different for each question. Initially the same as defined below.'), t('Options from additional nested components will be merged with any options configured here.')),
        array(t('Checkboxes'), t('No. Radio buttons only.'), t('Yes. Some or all questions may be multiple choice with check boxes.'), ''),
        array(t('Default'), t('Yes. Must be same for all questions.'), t('Yes. May all be the same or different.'), ''),
        array(t('Pre-built option lists'), t('No.'), t('Yes.'), ''),
        array(t('Required'), t('Yes. Must be same for all questions.'), t('Yes. May all be the same or different.'), ''),
        array(t('Question conditionals'), t('No.'), t('Yes. Individual questions may be used in conditional rules and/or actions.'), t('The whole grid may be conditionally shown or required.')),
        array(t('Other types of nested components'), t('No.'), t('Yes. Other component types may also be included in the grid. They will be displayed where the options would normally be.'), ''),
      ),
    ),
  );

  if (module_exists('options_element')) {
    $form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Options'),
      '#collapsible' => TRUE,
      '#attributes' => array('class' => array('webform-options-element')),
      '#element_validate' => array('_webform_edit_validate_options'),
      '#weight' => -3,
    );
    $form['options']['options'] = array(
      '#type' => 'options',
      '#options' => _webform_select_options_from_text($component['extra']['options'], TRUE),
      '#default_value' => $component['value'],
      '#default_value_allowed' => TRUE,
      '#optgroups' => FALSE,
      '#key_type' => 'mixed',
      '#key_type_toggle' => t('Customize option keys (Advanced)'),
      '#key_type_toggled' => $component['extra']['custom_option_keys'],
      '#default_value_pattern' => '^%.+\[.+\]$',
      '#description' => t('<strong>Options to select across the top</strong>, such as "Poor" through "Excellent". Indicate the default to the left of the desired item. Use of only alphanumeric characters and underscores is recommended in keys.') . ' ' . theme('webform_token_help'),
    );

    $form['questions'] = array(
      '#type' => 'fieldset',
      '#title' => t('Questions'),
      '#collapsible' => TRUE,
      '#attributes' => array('class' => array('webform-options-element')),
      '#element_validate' => array('_webform_edit_validate_options'),
      '#weight' => -2,
    );
    $form['questions']['options'] = array(
      '#type' => 'options',
      '#options' => _webform_select_options_from_text($component['extra']['questions'], TRUE),
      '#optgroups' => FALSE,
      '#default_value' => FALSE,
      '#default_value_allowed' => FALSE,
      '#key_type' => 'mixed',
      '#key_type_toggle' => t('Customize question keys (Advanced)'),
      '#key_type_toggled' => $component['extra']['custom_question_keys'],
      '#description' => t('<strong>Questions list down the side of the grid.</strong> For a heading column on the right, append "|" and the right-side heading. Use of only alphanumeric characters and underscores is recommended in keys.') . ' ' . theme('webform_token_help'),
    );
  }
  else {
    $form['extra']['options'] = array(
      '#type' => 'textarea',
      '#title' => t('Options'),
      '#default_value' => $component['extra']['options'],
      '#description' => t('Options to select across the top, such as "Poor" through "Excellent" or "Stronly Disagree" through "Strongly Agree".') .
                          '<p>' . t('One key-value option per line. <strong>Key-value pairs MUST be specified as "safe_key|Some readable option"</strong>. Use of only alphanumeric characters and underscores is recommended in keys.') . '</p>' . theme('webform_token_help'),
      '#cols' => 60,
      '#rows' => 5,
      '#weight' => -3,
      '#wysiwyg' => FALSE,
      '#element_validate' => array('_webform_edit_validate_select'),
    );
    $form['extra']['questions'] = array(
      '#type' => 'textarea',
      '#title' => t('Questions'),
      '#default_value' => $component['extra']['questions'],
      '#description' => t('Questions list down the side of the grid. One question per line. <strong>Key-value pairs MUST be specified as "safe_key|Some readable question"</strong>. For a heading column on the right, append "|" and the right-side heading. Use of only alphanumeric characters and underscores is recommended in keys.') . ' ' . theme('webform_token_help') . ' ' .
                        '<p>' . t('<strong>Or for more control</strong> over the appearance and configuration, create additional additional Select Options or other type components nested under this grid. They will operate as separate components, but be displayed within this grid.') . '</p>',
      '#cols' => 60,
      '#rows' => 5,
      '#weight' => -2,
      '#wysiwyg' => FALSE,
      '#element_validate' => array('_webform_edit_validate_select'),
    );
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Default value'),
      '#default_value' => $component['value'],
      '#description' => t('The default option of the grid identified by its key.') . ' ' . theme('webform_token_help'),
      '#size' => 60,
      '#maxlength' => 1024,
      '#weight' => 1,
    );
  }

  $form['display']['optrand'] = array(
    '#type' => 'checkbox',
    '#title' => t('Randomize Options'),
    '#default_value' => $component['extra']['optrand'],
    '#description' => t('Randomizes the order of options on the top when they are displayed in the form.'),
    '#parents' => array('extra', 'optrand')
  );
  $form['display']['qrand'] = array(
    '#type' => 'checkbox',
    '#title' => t('Randomize Questions'),
    '#default_value' => $component['extra']['qrand'],
    '#description' => t('Randomize the order of the questions on the side when they are displayed in the form.'),
    '#parents' => array('extra', 'qrand')
  );
  $form['display']['sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky table header'),
    '#default_value' => $component['extra']['sticky'],
    '#description' => t('Use a sticky (non-scrolling) table header.'),
    '#parents' => array('extra', 'sticky')
  );

  $form['validation']['unique'] = array(
    '#type' => 'checkbox',
    '#title' => t('Unique'),
    '#return_value' => 1,
    '#description' => t('Check that all entered values for this field are unique. The same value is not allowed to be used twice.'),
    '#weight' => 1,
    '#default_value' => $component['extra']['unique'],
    '#parents' => array('extra', 'unique'),
  );

  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_grid($component, $value = NULL, $filter = TRUE, $submission = NULL) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $questions = _webform_select_options_from_text($component['extra']['questions'], TRUE);
  $options = _webform_select_options_from_text($component['extra']['options'], TRUE);
  if ($filter) {
    $questions = _webform_select_replace_tokens($questions, $node);
    $options = _webform_select_replace_tokens($options, $node);
  }

  $element = array(
    '#type' => 'webform_grid',
    '#title' => $filter ? webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#required' => $component['required'],
    '#weight' => $component['weight'],
    '#description' => $filter ? webform_filter_descriptions($component['extra']['description'], $node) : $component['extra']['description'],
    '#grid_questions' => $questions,
    '#grid_options' => $options,
    '#default_value' => isset($value) || !strlen($component['value']) ? $value : array_fill_keys(array_keys($questions), $component['value']),
    '#grid_default' => $component['value'],
    '#optrand' => $component['extra']['optrand'],
    '#qrand' => $component['extra']['qrand'],
    '#sticky' => $component['extra']['sticky'],
    '#theme' => 'webform_grid',
    '#theme_wrappers' => array('webform_element'),
    '#process' => array('webform_expand_grid'),
    '#element_validate' => array('webform_validate_grid'),
    '#translatable' => array('title', 'description', 'grid_options', 'grid_questions'),
  );

  // Enforce uniqueness.
  if ($component['extra']['unique']) {
    $element['#element_validate'][] = '_webform_edit_grid_unique_validate';
  }

  return $element;
}

/**
 * A Form API #process function for Webform grid fields.
 */
function webform_expand_grid($element) {
  $options = $element['#grid_options'];
  $questions = $element['#grid_questions'];
  $weights = array();

  // Process questions and options from nested components.
  foreach (element_children($element) as $key) {
    $question = $element[$key];
    // Both forms and grid displays have #webform_component.
    if (isset($question['#webform_component']) &&
        $question['#webform_component']['type'] == 'select' &&
        !$question['#webform_component']['extra']['aslist'] &&
        !$question['#webform_component']['extra']['other_option']) {
      $options = webform_grid_merge_options($options, $question['#options']);
      $weights[$key] = $question['#weight'];
    }
  }

  // Add the internal grid questions.
  $weight = -1000;
  $value = isset($element['#default_value']) ? $element['#default_value'] : array();
  foreach ($questions as $key => $question) {
    if ($question != '') {
      $question_value = isset($value[$key]) && $value[$key] !== '' ? $value[$key] : NULL;
      $element[$key] = array(
        '#grid_question' => TRUE,
        '#title' => $question,
        '#required' => $element['#required'],
        '#options' => $element['#grid_options'],
        '#type' => 'radios',
        '#default_value' => $question_value,
        '#value' => $question_value,
        '#process' => array('form_process_radios', 'webform_expand_select_ids'),

        // Webform handles validation manually.
        '#validated' => TRUE,
        '#webform_validated' => FALSE,
        '#translatable' => array('title'),
        '#weight' => $weight,
      );

      // Add HTML5 required attribute, if needed.
      if ($element['#required']) {
        $element[$key]['#attributes']['required'] = 'required';
      }

      $weights[$key] = $weight;
      $weight++;
    }
  }

  if (!empty($element['#optrand'])) {
    _webform_shuffle_options($options);
  }
  $element['#grid_options'] = $options;

  asort($weights);
  if (!empty($element['#qrand'])) {
    _webform_shuffle_options($weights);
  }
  if ($weights) {
    $weight = min($weights);
  }
  foreach ($weights as $key => $old_weight) {
    $element[$key]['#options'] = webform_grid_remove_options($options, $element[$key]['#options']);
    $element[$key]['#weight'] = $weight++;
    $element['#grid_questions'][$key] = $element[$key]['#title'];
  }

  return $element;
}

/**
 * Helper. Merge select component options in order.
 *
 * @param array $existing
 *   An array of existing values into which any values from $new that aren't in
 *   $existing are inserted.
 * @param array $new
 *   Values to be inserted into $existing.
 * @return array
 *   The merged array.
 */
function webform_grid_merge_options($existing, $new) {
  $insert = NULL;
  $queue = array();
  foreach ($new as $key => $value) {
    if (isset($existing[$key])) {
      // Insert the queue before the found item.
      $insert = array_search($key, array_keys($existing));
      if ($queue) {
        $existing = array_slice($existing, 0, $insert, TRUE) +
                    $queue +
                    array_slice($existing, $insert, NULL, TRUE);
        $insert += count($queue);
        $queue = array();
      }
      $insert++;
    }
    elseif (is_null($insert)) {
      // It is not yet clear yet where to put this item. Add it to the queue.
      $queue[$key] = $value;
    }
    else {
      // PHP array_splice does not preserved the keys of the inserted array,
      // but array_slice does (if the preserve keys parameter is TRUE).
      $existing = array_slice($existing, 0, $insert, TRUE) +
                  array($key => $value) +
                  array_slice($existing, $insert, NULL, TRUE);
      $insert++;
    }
  }
  // Append any left over queued items.
  $existing += $queue;
  return $existing;
}

/**
 * Helper. Replace missing options with empty values.
 *
 * @param array $header
 *   An array of options to be used at the grid table header.
 * @param array $row_options
 *   An array of options to be used for this row.
 * @return array
 *   The $row_options with any missing options replaced with empty values.
 **/
function webform_grid_remove_options($header, $row_options) {
  foreach ($header as $key => $value) {
    if (!isset($row_options[$key])) {
      $header[$key] = '';
    }
  }
  return $header;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_grid($component, $value, $format = 'html', $submission = array()) {
  $node = node_load($component['nid']);
  $questions = _webform_select_options_from_text($component['extra']['questions'], TRUE);
  $questions = _webform_select_replace_tokens($questions, $node);
  $options = _webform_select_options_from_text($component['extra']['options'], TRUE);
  $options = _webform_select_replace_tokens($options, $node);

  $element = array(
    '#title' => $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#weight' => $component['weight'],
    '#format' => $format,
    '#grid_questions' => $questions,
    '#grid_options' => $options,
    '#default_value' => $value,
    '#sticky' => $component['extra']['sticky'],
    '#theme' => 'webform_display_grid',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#sorted' => TRUE,
    '#translatable' => array('#title', '#grid_questions', '#grid_options'),
  );

  foreach ($questions as $key => $question) {
    if ($question !== '') {
      $element[$key] = array(
        '#title' => $question,
        '#value' => isset($value[$key]) ? $value[$key] : NULL,
        '#translatable' => array('#title', '#value'),
      );
    }
  }

  return $element;
}

/**
 * Preprocess function for displaying a grid component.
 */
function template_preprocess_webform_display_grid(&$variables) {
  $element =& $variables['element'];
  // Expand the grid, suppressing randomization. This builds the grid
  // questions and options.
  $element['#qrand'] = FALSE;
  $element['#optrand'] = FALSE;
  $element['#required'] = FALSE;
  $element = webform_expand_grid($element);
}

/**
 * Format the text output for this component.
 */
function theme_webform_display_grid($variables) {
  $element = $variables['element'];

  $component = $element['#webform_component'];
  $format = $element['#format'];

  if ($format == 'html') {
    $right_titles = _webform_grid_right_titles($element);
    $rows = array();

    // Set the header for the table.
    $header = _webform_grid_header($element, $right_titles);

    foreach (element_children($element) as $question_key) {
      $question_element = $element[$question_key];
      $row = array();
      $questions = explode('|', $question_element['#title'], 2);
      $values = $question_element['#value'];
      $values = is_array($values) ? $values : array($values);
      $row[] = array('data' => webform_filter_xss($questions[0]), 'class' => array('webform-grid-question'));
      if (isset($element['#grid_questions'][$question_key])) {
        foreach ($element['#grid_options'] as $option_value => $option_label) {
          if (in_array($option_value, $values)) {
            $row[] = array('data' => '<strong>X</strong>', 'class' => array('checkbox', 'webform-grid-option'));
          }
          else {
            $row[] = array('data' => '&nbsp;', 'class' => array('checkbox', 'webform-grid-option'));
          }
        }
      }
      else {
        $question_element['#title_display'] = 'none';
        $row[] = array(
          'data' => drupal_render($question_element),
          'colspan' => count($element['#grid_options']),
        );
      }
      if ($right_titles) {
        $row[] = array('data' => isset($questions[1]) ? webform_filter_xss($questions[1]) : '', 'class' => array('webform-grid-question'));
      }
      $rows[] = $row;
    }

    $option_count = count($header) - 1;
    $output = theme('table', array('header' => $header, 'rows' => $rows, 'sticky' => $element['#sticky'], 'attributes' => array('class' => array('webform-grid', 'webform-grid-' . $option_count))));
  }
  else {
    $items = array();
    foreach (element_children($element) as $question_key) {
      $question_element = $element[$question_key];
      if (isset($element['#grid_questions'][$question_key])) {
        $values = $question_element['#value'];
        $values = is_array($values) ? $values : array($values);
        foreach ($values as $value_key => $value) {
          if (isset($element['#grid_options'][$value])) {
            $values[$value_key] = $element['#grid_options'][$value];
          }
          else {
            unset($values[$value_key]);
          }
        }
        $value = implode(', ', $values);
      }
      else {
        $element[$question_key]['#title'] = '';
        $value = drupal_render($element[$question_key]);
      }
      $items[] = ' - ' . _webform_grid_question_header($question_element['#title']) . ': ' . $value;
   }
    $output = implode("\n", $items);
  }

  return $output;
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_grid($component, $sids = array(), $single = FALSE, $join = NULL) {
  // Generate the list of options and questions.
  $node = node_load($component['nid']);
  $questions = _webform_select_options_from_text($component['extra']['questions'], TRUE);
  $questions = _webform_select_replace_tokens($questions, $node);
  $options = _webform_select_options_from_text($component['extra']['options'], TRUE);
  $options = _webform_select_replace_tokens($options, $node);

  // Generate a lookup table of results.
  $query = db_select('webform_submitted_data', 'wsd')
    ->fields('wsd', array('no', 'data'))
    ->condition('wsd.nid', $component['nid'])
    ->condition('wsd.cid', $component['cid'])
    ->condition('wsd.data', '', '<>')
    ->groupBy('wsd.no')
    ->groupBy('wsd.data');
  $query->addExpression('COUNT(wsd.sid)', 'datacount');

  if (count($sids)) {
    $query->condition('wsd.sid', $sids, 'IN');
  }

  if ($join) {
    $query->innerJoin($join, 'ws2_', 'wsd.sid = ws2_.sid');
  }

  $result = $query->execute();
  $counts = array();
  foreach ($result as $data) {
    $counts[$data->no][$data->data] = $data->datacount;
  }

  // Create an entire table to be put into the returned row.
  $rows = array();
  $header = array('');

  // Add options as a header row.
  foreach ($options as $option) {
    $header[] = webform_filter_xss($option);
  }

  // Add questions as each row.
  foreach ($questions as $qkey => $question) {
    $row = array(webform_filter_xss($question));
    foreach ($options as $okey => $option) {
      $row[] = !empty($counts[$qkey][$okey]) ? $counts[$qkey][$okey] : 0;
    }
    $rows[] = $row;
  }

  // Return return the table unless there are no internal questions in the grid.
  if ($rows) {
    return array(
      'table_header' => $header,
      'table_rows' => $rows,
    );
  }
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_grid($component, $value) {
  $node = node_load($component['nid']);
  $questions = _webform_select_options_from_text($component['extra']['questions'], TRUE);
  $questions = _webform_select_replace_tokens($questions, $node);
  $options = _webform_select_options_from_text($component['extra']['options'], TRUE);
  $options = _webform_select_replace_tokens($options, $node);

  $output = '';
  // Set the value as a single string.
  foreach ($questions as $key => $label) {
    if (isset($value[$key]) && isset($options[$value[$key]])) {
      $output .= webform_filter_xss(_webform_grid_question_header($label)) . ': ' . webform_filter_xss($options[$value[$key]]) . '<br />';
    }
  }

  // Return output if the grid contains internal questions.
  if (count($questions)) {
    return $output;
  }
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_grid($component, $export_options) {
  $node = node_load($component['nid']);
  $items = _webform_select_options_from_text($component['extra']['questions'], TRUE);
  $items = _webform_select_replace_tokens($items, $node);

  $header = array();
  $header[0] = array('');
  $header[1] = array($export_options['header_keys'] ? $component['form_key'] : $component['name']);
  $count = 0;
  foreach ($items as $key => $item) {
    // Empty column per sub-field in main header.
    if ($count != 0) {
      $header[0][] = '';
      $header[1][] = '';
    }
    // The value for this option.
    $header[2][] = $export_options['header_keys'] ? $key : _webform_grid_question_header($item);
    $count++;
  }

  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_grid($component, $export_options, $value) {
  $node = node_load($component['nid']);
  $questions = _webform_select_options_from_text($component['extra']['questions'], TRUE);
  $questions = _webform_select_replace_tokens($questions, $node);
  $options = _webform_select_options_from_text($component['extra']['options'], TRUE);
  $options = _webform_select_replace_tokens($options, $node);

  $return = array();
  foreach ($questions as $key => $question) {
    if (isset($value[$key]) && isset($options[$value[$key]])) {
      $return[] = $export_options['select_keys'] ? $value[$key] : $options[$value[$key]];
    }
    else {
      $return[] = '';
    }
  }
  return $return;
}

/**
 * A Form API element validate function to check that all choices are unique.
 */
function _webform_edit_grid_unique_validate($element) {
  // Grids may contain nested multiple value select components.
  // Create a flat array of values.
  $values = array();
  array_walk_recursive($element['#value'], function($a) use (&$values) { $values[] = $a; });

  $nr_unique = count(array_unique($values));
  $nr_values = count($values);
  $nr_possible = count($element['#grid_options']);
  if (strlen($element['#grid_default']) && isset($element['#grid_options'][$element['#grid_default']])) {
    // A default is defined and is one of the options. Don't count default values
    // toward uniqueness.
    $nr_defaults = count(array_keys($element['#value'], $element['#grid_default']));
    if ($nr_defaults) {
      $nr_values -= $nr_defaults;
      $nr_unique--;
    }
  }
  if ($nr_unique < $nr_values && $nr_unique < $nr_possible) {
    // Fewer unique values than values means that at least one value is duplicated.
    // Fewer unique values than possible values means that there is a possible choice
    // that wasn't used.
    form_error($element, t('!title is not allowed to have the same answer for more than one question.', array('!title' => $element['#title'])));
  }
}

function theme_webform_grid($variables) {
  $element = $variables['element'];
  $right_titles = _webform_grid_right_titles($element);

  $rows = array();

  // Set the header for the table.
  $header = _webform_grid_header($element, $right_titles);

  foreach (element_children($element) as $key) {
    $question_element = $element[$key];
    $title_element =& $question_element;
    if ($question_element['#type'] == 'select_or_other') {
      $title_element =& $question_element['select'];
    }
    $question_titles = explode('|', $title_element['#title'], 2);

    // Create a row with the question title.
    $required = !empty($question_element['#required']) ? theme('form_required_marker', array('element' => $question_element)) : '';
    $row = array(array('data' => t('!title !required', array('!title' => webform_filter_xss($question_titles[0]), '!required' => $required)), 'class' => array('webform-grid-question')));

    // Render each radio button in the row.
    if ($question_element['#type'] == 'radios' || $question_element['#type'] == 'checkboxes') {
      $radios = form_process_radios($question_element);
      foreach (element_children($radios) as $key) {
        $radio_title = $radios[$key]['#title'];
        if (!strlen($radio_title)) {
          $row[] = '&nbsp;';
        }
        else {
          $radios[$key]['#title'] = $question_element['#title'] . ' - ' . $radio_title;
          $radios[$key]['#title_display'] = 'invisible';
          $row[] = array('data' => drupal_render($radios[$key]), 'class' => array('checkbox', 'webform-grid-option'), 'data-label' => array($radio_title));
        }
      }
    }
    else {
      $title_element['#title_display'] = 'none';
      $row[] = array(
        'data' => drupal_render($question_element),
        'colspan' => count($element['#grid_options']),
      );
    }
    if ($right_titles) {
      $row[] = array('data' => isset($question_titles[1]) ? webform_filter_xss($question_titles[1]) : '', 'class' => array('webform-grid-question'));
    }

    // Convert the parents array into a string, excluding the "submitted" wrapper.
    $nested_level = $question_element['#parents'][0] == 'submitted' ? 1 : 0;
    $parents = str_replace('_', '-', implode('--', array_slice($question_element['#parents'], $nested_level)));

    $rows[] = array(
      'data' => $row,
      'class' => empty($question_element['#grid_question'])
                    ? array('webform-component', 'webform-component-' . str_replace('_', '-', $question_element['#type']), 'webform-component--' . $parents)
                    : array(),
    );
  }

  $option_count = count($header) - 1;
  return theme('table', array('header' => $header, 'rows' => $rows, 'sticky' => $element['#sticky'], 'attributes' => array('class' => array('webform-grid', 'webform-grid-' . $option_count))));
}

/**
 * Generate a table header suitable for form or html display.
 */
function _webform_grid_header($element, $right_titles) {
  $titles = explode('|', $element['#title'], 2);
  $header = array(array('data' => _webform_grid_header_title($element, $titles[0]), 'class' => array('webform-grid-question')));
  foreach ($element['#grid_options'] as $option) {
    $header[] = array('data' => webform_filter_xss($option), 'class' => array('checkbox', 'webform-grid-option'));
  }
  if ($right_titles) {
    $header[] = array('data' => _webform_grid_header_title($element, isset($titles[1]) ? $titles[1] : ''), 'class' => array('webform-grid-question'));
  }
  return $header;
}

/**
 * Create internal component title for table header, if any.
 */
function _webform_grid_header_title($element, $title) {
  $header_title = '';
  if ($element['#title_display'] == 'internal') {
    $variables = array('element' => $element);
    $variables['element']['#title_display'] = 'before';
    $variables['element']['#title'] = $title;
    $header_title = theme('form_element_label', $variables);
  }
  return $header_title;
}

/**
 * Determine if a right-side title column has been specified.
 */
function _webform_grid_right_titles($element) {
  if ($element['#title_display'] == 'internal' && substr_count($element['#title'], '|')) {
    return TRUE;
  }
  foreach ($element['#grid_questions'] as $question_key => $question) {
    if (substr_count($question, '|')) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Create a question header for left, right or left/right question headers.
 */
function _webform_grid_question_header($text) {
  return implode('/', array_filter(explode('|', $text)));
}

/**
 * Element validation for Webform grid fields.
 *
 * Requires a component implementation because the default required validation
 * passes when at least one value is supplied, rather than every value. This
 * makes the server validation match the browser validation.
 */
function webform_validate_grid($element, $form_state) {
  if ($element['#required']) {
    $values = $form_state['input'];
    foreach ($element['#parents'] as $key) {
      $values = isset($values[$key]) ? $values[$key] : $values;
    }
    $nr_values = array_reduce($values, function($count, $item) { return $count + (int)!is_null($item); }, 0);
    if (count($element['#grid_questions']) != $nr_values) {
      form_error($element, t('!name field is required.', array('!name' => $element['#title'])));
    }
  }
}
