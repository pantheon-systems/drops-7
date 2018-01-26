<?php

/**
 * @file
 * Form elements and menu callbacks to provide conditional handling in Webform.
 */

/**
 * Form builder; Provide the form for adding conditionals to a webform node.
 */
function webform_conditionals_form($form, &$form_state, $node) {
  form_load_include($form_state, 'inc', 'webform', $name = 'includes/webform.components');
  form_load_include($form_state, 'inc', 'webform', $name = 'includes/webform.conditionals');

  // Add JavaScript settings to the page needed for conditional elements.
  _webform_conditional_expand_value_forms($node);

  if (isset($form_state['values']['conditionals'])) {
    // Remove the "new" conditional that always comes in.
    unset($form_state['values']['conditionals']['new']);

    $conditionals = $form_state['values']['conditionals'];
  }
  else {
    $conditionals = $node->webform['conditionals'];
  }
  // Empty out any conditionals that have no rules or actions.
  foreach ($conditionals as $rgid => &$conditional) {
    webform_delete_empty_subconditionals($conditional);
    if (empty($conditional['rules']) || empty($conditional['actions'])) {
      unset($conditionals[$rgid]);
    }
  }
  // Drop PHP reference.
  unset($conditional);

  // Check the current topological sort order for the conditionals and report any errors,
  // but only for actual form submissions and not for ajax-related form builds, such as
  // adding or removing a condition or conditional group.
  if (empty($form_state['triggering_element']['#ajax'])) {
    $node->webform['conditionals'] = $conditionals;
    webform_get_conditional_sorter($node)->reportErrors($conditionals);
  }

  $form['#tree'] = TRUE;
  $form['#node'] = $node;

  $form['#attached']['library'][] = array('webform', 'admin');
  $form['#attached']['css'][] = drupal_get_path('module', 'webform') . '/css/webform.css';

  // Wrappers used for AJAX addition/removal.
  $form['conditionals']['#theme'] = 'webform_conditional_groups';
  $form['conditionals']['#prefix'] = '<div id="webform-conditionals-ajax">';
  $form['conditionals']['#suffix'] = '</div>';

  // Keep track of the max conditional count to use as the range for weights.
  $form_state['conditional_count'] = isset($form_state['conditional_count']) ? $form_state['conditional_count'] : 1;
  $form_state['conditional_count'] = count($conditionals) > $form_state['conditional_count'] ? count($conditionals) : $form_state['conditional_count'];

  $source_list = webform_component_list($node, 'conditional', 'path', TRUE);
  $target_list = webform_component_list($node, TRUE, 'path', TRUE);
  $components = $node->webform['components'];
  $delta = $form_state['conditional_count'];
  $weight = -$delta - 1;
  $index = 0;
  foreach ($conditionals as $rgid => $conditional_group) {
    $weight = $conditional_group['weight'];
    $form['conditionals'][$rgid] = array(
      '#theme' => 'webform_conditional_group_row',
      '#even_odd' => ++$index % 2 ? 'odd' : 'even',
      '#weight' => $weight,
      'rgid' => array(
        '#type' => 'value',
        '#value' => $rgid,
      ),
      'conditional' => array(
        '#type' => 'webform_conditional',
        '#default_value' => $conditional_group,
        '#nid' => $node->nid,
        '#sources' => $source_list,
        '#actions' => array(
          'show' => t('shown'),
          'require' => t('required'),
          'set' => t('set to'),
        ),
        '#targets' => $target_list,
        '#parents' => array('conditionals', $rgid),
      ),
    );
    foreach ($conditional_group['actions'] as $action) {
      $cid = $action['target'];
      if ($action['action'] == 'require' && !$components[$cid]['required']) {
        drupal_set_message(t('Component %title must be configured as Required for Webform to conditionally change its required status. <a href="!url">Configure %title.</a>',
          array(
            '%title' => $components[$cid]['name'],
            '!url' => url("node/{$node->nid}/webform/components/$cid", array('query' => array('destination' => "node/{$node->nid}/webform/conditionals"))),
          )
          ), 'error');
      }
    }
    $form['conditionals'][$rgid]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight for rule group !rgid', array('!rgid' => $rgid)),
      '#title_display' => 'invisible',
      '#default_value' => $weight,
      '#delta' => $delta,
    );
  }

  $form['conditionals']['new']['#weight'] = $weight + 1;
  $form['conditionals']['new']['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight for new rule group'),
    '#title_display' => 'invisible',
    '#default_value' => $weight + 1,
    '#delta' => $delta,
  );
  $form['conditionals']['new']['new'] = array(
    '#type' => 'submit',
    '#value' => t('+'),
    '#submit' => array('webform_conditionals_form_add'),
    '#ajax' => array(
      'progress' => 'none',
      'effect' => 'fade',
      'callback' => 'webform_conditionals_ajax',
    ),
  );
  // Create dummy remove button for form alignment only.
  $form['conditionals']['new']['remove'] = array(
    '#type' => 'submit',
    '#value' => t('-'),
    '#disabled' => TRUE,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    '#tree' => FALSE,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save conditions'),
    '#validate' => array('webform_conditionals_form_validate'),
    '#submit' => array('webform_conditionals_form_submit'),
  );

  // Estimate if the form is too long for PHP max_input_vars and detect whether a previous submission was truncated.
  // The estimate will be accurate because the form elements for this page are well known. Ajax use of this
  // page will not generate user-visible errors, so a preflight may be the only indication to the user that
  // the page is too long.
  webform_input_vars_check($form, $form_state, 'conditionals', '');
  return $form;
}

/**
 * Submit handler for webform_conditionals_form(). Add an additional choice.
 */
function webform_conditionals_form_add($form, &$form_state) {
  // Build a default new conditional.
  unset($form_state['values']['conditionals']['new']);
  $weight = count($form_state['values']['conditionals']) > 10 ? -count($form_state['values']['conditionals']) : -10;
  foreach ($form_state['values']['conditionals'] as $key => $conditional) {
    $weight = max($weight, $conditional['weight']);
  }

  // Add the conditional to form state and rebuild the form.
  $form_state['values']['conditionals'][] = array(
    'rules' => array(
      array(
        'source_type' => 'component',
        'source' => NULL,
        'operator' => NULL,
        'value' => NULL,
      ),
    ),
    'andor' => 'and',
    'actions' => array(
      array(
        'target_type' => 'component',
        'target' => NULL,
        'invert' => NULL,
        'action' => NULL,
        'argument' => NULL,
      ),
    ),
    'weight' => $weight + 1,
  );
  $form_state['rebuild'] = TRUE;
}

/**
 * Validate handler for webform_conditionals_form().
 *
 * Prohibit the source and target of a conditional rule from being the same.
 */
function webform_conditionals_form_validate($form, &$form_state) {
  // Skip validation unless this is saving the form.
  $button_key = end($form_state['triggering_element']['#array_parents']);
  if ($button_key !== 'submit') {
    return;
  }

  $node = $form['#node'];
  $components = $node->webform['components'];
  $component_options = webform_component_options();
  foreach ($form_state['complete form']['conditionals'] as $conditional_key => $element) {
    if (substr($conditional_key, 0, 1) !== '#' && $conditional_key !== 'new') {
      $conditional = $element['conditional'];
      $targets = array();
      foreach ($conditional['actions'] as $action_key => $action) {
        if (is_numeric($action_key)) {
          $operation = $action['action']['#value'];
          $target_id = $action['target']['#value'];
          if (isset($targets[$target_id][$operation])) {
            form_set_error('conditionals][' . $conditional_key . '][actions][' . $action_key . '][target',
              t('A operation %op cannot be made for a component more than once. (%target).',
              array(
                '%op' => $action['action']['#options'][$operation],
                '%target' => $components[$action['target']['#value']]['name'],
              )));
          }
          $component_type = $node->webform['components'][$action['target']['#value']]['type'];
          if (!webform_conditional_action_able($component_type, $action['action']['#value'])) {
            form_set_error('conditionals][' . $conditional_key . '][actions][' . $action_key . '][action',
              t('A component of type %type can\'t be %action. (%target)',
              array(
                '%action' => $action['action']['#options'][$action['action']['#value']],
                '%type' => $component_options[$component_type],
                '%target' => $components[$action['target']['#value']]['name'],
              )));
          }
          $targets[$target_id][$operation] = $target_id;
        }
      }
      foreach ($conditional['rules'] as $rule_key => $rule) {
        // Validate component rules, but not conditional_start/end rules.
        if (is_numeric($rule_key) && $rule['source_type']['#value'] == 'component' && isset($targets[$rule['source']['#value']])) {
          form_set_error('conditionals][' . $conditional_key . '][rules][' . $rule_key . '][source',
            t('The subject of the conditional cannot be the same as the component that is changed (%target).',
            array('%target' => $components[$rule['source']['#value']]['name'])));
        }
      }
    }
  }

  // Form validation will not rebuild the form, so we need to ensure
  // necessary JavaScript will still exist.
  _webform_conditional_expand_value_forms($node);
}

/**
 * Submit handler for webform_conditionals_form().
 */
function webform_conditionals_form_submit($form, &$form_state) {
  $node = $form['#node'];

  // Remove the new conditional placeholder.
  unset($form_state['values']['conditionals']['new']);

  $node->webform['conditionals'] = $form_state['values']['conditionals'];
  node_save($node);
  drupal_set_message(t('Conditionals for %title saved.', array('%title' => $node->title)));
}

/**
 * AJAX callback to render out adding a new condition.
 */
function webform_conditionals_ajax($form, $form_state) {
  $rgids = element_children($form['conditionals']);
  $new_rgid = max($rgids);
  $form['conditionals'][$new_rgid]['#ajax_added'] = TRUE;

  $commands = array('#type' => 'ajax');
  $commands['#commands'][] = ajax_command_before('.webform-conditional-new-row', drupal_render($form['conditionals'][$new_rgid]));
  $commands['#commands'][] = ajax_command_restripe('#webform-conditionals-table');
  return $commands;
}

/**
 * Theme the $form['conditionals'] of webform_conditionals_form().
 */
function theme_webform_conditional_groups($variables) {
  $element = $variables['element'];
  drupal_add_tabledrag('webform-conditionals-table', 'order', 'sibling', 'webform-conditional-weight');
  drupal_add_js('Drupal.theme.prototype.tableDragChangedMarker = function() { return ""; }', 'inline');
  drupal_add_js('Drupal.theme.prototype.tableDragChangedWarning = function() { return "<span>&nbsp;</span>"; }', 'inline');

  $output = '<table id="webform-conditionals-table"><tbody>';
  $element_children = element_children($element, TRUE);
  $element_count = count($element_children);
  foreach ($element_children as $index => $key) {
    if ($key === 'new') {
      $even_odd = ($index + 1) % 2 ? 'odd' : 'even';
      $element[$key]['weight']['#attributes']['class'] = array('webform-conditional-weight');
      $data = '<div class="webform-conditional-new">';
      if ($element_count === 1) {
        $data .= t('There are no conditional actions on this form.') . ' ';
      }
      $data .= t('Add a new condition:') . ' ' . drupal_render($element[$key]['new']) . drupal_render($element[$key]['remove']);
      $data .= '</div>';
      $output .= '<tr class="webform-conditional-new-row ' . $even_odd . '">';
      $output .= '<td>' . $data . '</td>';
      $output .= '<td>' . drupal_render($element[$key]['weight']) . '</td>';
      $output .= '</tr>';
    }
    else {
      $output .= drupal_render($element[$key]);
    }
  }
  $output .= '</tbody></table>';
  $output .= drupal_render_children($element);

  return $output;
}

/**
 * Theme an individual conditional row of webform_conditionals_form().
 */
function theme_webform_conditional_group_row($variables) {
  $element = $variables['element'];

  $element['weight']['#attributes']['class'] = array('webform-conditional-weight');
  $weight = drupal_render($element['weight']);
  $classes = array('draggable');
  if (!empty($element['#even_odd'])) {
    $classes[] = $element['#even_odd'];
  }
  if (!empty($element['#ajax_added'])) {
    $classes[] = 'ajax-new-content';
  }

  $output = '';
  $output .= '<tr class="' . implode(' ', $classes) . '">';
  $output .= '<td>' . drupal_render_children($element) . '</td>';
  $output .= '<td>' . $weight . '</td>';
  $output .= '</tr>';

  return $output;
}

/**
 * Form API #process function to expand a webform conditional element.
 */
function _webform_conditional_expand($element) {
  $element['#tree'] = TRUE;
  $element['#default_value'] += array(
    'andor' => 'and',
  );

  $wrapper_id = drupal_clean_css_identifier(implode('-', $element['#parents'])) . '-ajax';
  $element['#prefix'] = '<div id="' . $wrapper_id . '">';
  $element['#suffix'] = '</div>';
  $element['#wrapper_id'] = $wrapper_id;

  // Note: When rules or actions are added, the new rules are inserted into
  // $form_state['values']. So that FAPI can merge data from the post,
  // $form_state['input'] must be adjusted to. To make this easier, hidden
  // fields are added to the conditional_start and _end rules to ensure that
  // each rule is represented in the POST.
  $level = 0;
  $andor_stack[0] = array(
    'value' => $element['#default_value']['andor'],
    'parents' => array_merge($element['#parents'], array('andor')),
    'rid' => 0,
    'first' => TRUE,
  );

  $last_rid = -1;
  foreach ($element['#default_value']['rules'] as $rid => $conditional) {
    switch ($conditional['source_type']) {
      case 'conditional_start':
        $element['rules'][$rid] = array(
          '#level' => $level,
          'source_type' => array(
            '#type' => 'hidden',
            '#value' => 'conditional_start',
          ),
          // The andor operator is located in the first child, which is
          // guaranteed to exist. Therefore, don't add a 'value' element here.
          'add_subconditional' => _webform_conditional_add_expand($element, $rid, TRUE),
          'add' => _webform_conditional_add_expand($element, $rid, FALSE),
          'remove' => _webform_conditional_remove_expand($element, $rid),
        );
        $andor_stack[++$level] = array(
          'value' => $conditional['operator'],
          'parents' => array_merge($element['#parents'], array('rules', $rid, 'operator')),
          'rid' => $rid,
          'first' => TRUE,
        );
        break;

      case 'conditional_end':
        --$level;
        $element['rules'][$rid] = array(
          '#level' => $level,
          'source_type' => array(
            '#type' => 'hidden',
            '#value' => 'conditional_end',
          ),
          'add_subconditional' => _webform_conditional_add_expand($element, $rid, TRUE),
          'add' => _webform_conditional_add_expand($element, $rid, FALSE),
          'remove' => _webform_conditional_remove_expand($element, $rid),
          'andor' => _webform_conditional_andor_expand($andor_stack[$level]),
        );
        // Remove the last nested and/or.
        unset($element['rules'][$last_rid]['andor']);
        break;

      case 'component':
        $element['rules'][$rid] = _webform_conditional_rule_expand($element, $rid, $conditional, $level, $andor_stack[$level]);
        break;

      default:
        drupal_set_message(t('Unexpected conditional rule source type found (rule id @rid). Contact the administrator.', array('@rid' => $rid)), 'error');
    }
    $last_rid = $rid;
  }

  // Remove the last and/or.
  unset($element['rules'][$rid]['andor']);

  foreach ($element['#default_value']['actions'] as $aid => $action) {
    $element['actions'][$aid] = _webform_conditional_action_expand($element, $aid, $action);
  }

  return $element;
}

/**
 * Helper. Generate the and/or select or static text.
 */
function _webform_conditional_andor_expand(&$andor) {
  if ($andor['first']) {
    $andor['first'] = FALSE;
    return array(
      '#type' => 'select',
      '#title' => t('And/or'),
      '#options' => array(
        'and' => t('and'),
        'or' => t('or'),
      ),
      '#parents' => $andor['parents'],
      '#default_value' => $andor['value'],
      '#attributes' => array('data-rid' => $andor['rid']),
    );
  }
  else {
    return array(
      '#type' => 'container',
      '#attributes' => array('class' => array('webform-andor'), 'data-rid' => $andor['rid']),
      'andor_text' => array(
        '#markup' => $andor['value'] == 'or' ? t('or') : t('and'),
      ),
    );
  }
}

/**
 * Helper. Generate the add_subconditional (+) or add + button.
 */
function _webform_conditional_add_expand($element, $rid, $subconditional) {
  return array(
    '#type' => 'submit',
    '#value' => $subconditional ? t('(+)') : t('+'),
    '#submit' => array('webform_conditional_element_add'),
    '#subconditional' => $subconditional,
    '#name' => implode('_', $element['#parents']) . '_rules_' . $rid .
    ($subconditional ? '_add_subconditional' : '_add'),
    '#attributes' => array('class' => array('webform-conditional-rule-add')),
    '#ajax' => array(
      'progress' => 'none',
      'callback' => 'webform_conditional_element_ajax',
      'wrapper' => $element['#wrapper_id'],
      'event' => 'click',
    ),
  );
}

/**
 * Helper. Generate the add_subconditional (+), add + or remove - button.
 */
function _webform_conditional_remove_expand($element, $rid) {
  return array(
    '#type' => 'submit',
    '#value' => t('-'),
    '#submit' => array('webform_conditional_element_remove'),
    '#name' => implode('_', $element['#parents']) . '_rules_' . $rid . '_remove',
    '#attributes' => array('class' => array('webform-conditional-rule-remove')),
    '#ajax' => array(
      'progress' => 'none',
      'callback' => 'webform_conditional_element_ajax',
      'wrapper' => $element['#wrapper_id'],
      'event' => 'click',
    ),
  );
}

/**
 * Helper. Generate form elements for one rule.
 */
function _webform_conditional_rule_expand($element, $rid, $conditional, $level, &$andor) {
  return array(
    '#level' => $level,
    'source_type' => array(
      '#type' => 'value',
      '#value' => $conditional['source_type'],
    ),
    'source' => array(
      '#type' => 'select',
      '#title' => t('Source'),
      '#options' => $element['#sources'],
      '#default_value' => $conditional['source'],
    ),
    'operator' => array(
      '#type' => 'select',
      '#title' => t('Operator'),
      '#options' => webform_conditional_operators_list(),
      '#default_value' => $conditional['operator'],
    ),
    'value' => array(
      '#type' => 'textfield',
      '#title' => t('Value'),
      '#size' => 20,
      '#default_value' => $conditional['value'],
    ),
    'add_subconditional' => _webform_conditional_add_expand($element, $rid, TRUE),
    'add' => _webform_conditional_add_expand($element, $rid, FALSE),
    'remove' => _webform_conditional_remove_expand($element, $rid),
    'andor' => _webform_conditional_andor_expand($andor),
  );
}

/**
 * Helper. Generate form elements for one action.
 */
function _webform_conditional_action_expand($element, $aid, $action) {
  return array(
    'target_type' => array(
      '#type' => 'value',
      '#value' => $action['target_type'],
    ),
    'target' => array(
      '#type' => 'select',
      '#title' => t('Target'),
      '#options' => $element['#targets'],
      '#default_value' => $action['target'],
    ),
    'invert' => array(
      '#type' => 'select',
      '#title' => t('Is/Isn\'t'),
      '#options' => array(
        '0' => t('is'),
        '1' => t('isn\'t'),
      ),
      '#default_value' => $action['invert'],
    ),
    'action' => array(
      '#type' => 'select',
      '#title' => t('Action'),
      '#options' => $element['#actions'],
      '#default_value' => $action['action'],
    ),
    'argument' => array(
      '#type' => 'textfield',
      '#title' => t('Argument'),
      '#size' => 20,
      '#maxlength' => NULL,
      '#default_value' => $action['argument'],
    ),
    'add' => array(
      '#type' => 'submit',
      '#value' => t('+'),
      '#submit' => array('webform_conditional_element_add'),
      '#name' => implode('_', $element['#parents']) . '_actions_' . $aid . '_add',
      '#attributes' => array('class' => array('webform-conditional-action-add')),
      '#ajax' => array(
        'progress' => 'none',
        'callback' => 'webform_conditional_element_ajax',
        'wrapper' => $element['#wrapper_id'],
        'event' => 'click',
      ),
    ),
    'remove' => array(
      '#type' => 'submit',
      '#value' => t('-'),
      '#submit' => array('webform_conditional_element_remove'),
      '#name' => implode('_', $element['#parents']) . '_actions_' . $aid . '_remove',
      '#attributes' => array('class' => array('webform-conditional-action-remove')),
      '#ajax' => array(
        'progress' => 'none',
        'callback' => 'webform_conditional_element_ajax',
        'wrapper' => $element['#wrapper_id'],
        'event' => 'click',
      ),
    ),
  );
}

/**
 * Expand out all the value forms that could potentially be used.
 *
 * These forms are added to the page via JavaScript and swapped in only when
 * needed. Because the user may change the source and operator at any time,
 * all these forms need to be generated ahead of time and swapped in. This
 * could have been done via AJAX, but having all forms available makes for a
 * faster user experience.
 *
 * Added to the JavaScript settings is conditionalValues which contains
 * an array settings suitable for adding to the page via JavaScript. This
 * array contains the following keys:
 *   - operators: An array containing a map of data types, operators, and form
 *     keys. This array is structured as follows:
 * @code
 *   - sources[$source_key] = array(
 *       'data_type' => $data_type,
 *     );
 *     $operators[$data_type][$operator] = array(
 *       'form' => $form_key,
 *     );
 * @endcode
 *   - forms[$form_key]: A string representing an HTML form for an operator.
 *   - forms[$form_key][$source]: Or instead of a single form for all components,
 *     if each component requires its own form, key each component by its source
 *     value (currently always the component ID).
 *
 * @param $node
 *   The Webform node for which these forms are being generated.
 */
function _webform_conditional_expand_value_forms($node) {
  $operators = webform_conditional_operators();
  $data = array();
  foreach ($operators as $data_type => $operator_info) {
    foreach ($operator_info as $operator => $data_operator_info) {
      $data['operators'][$data_type][$operator]['form'] = 'default';
      if (isset($data_operator_info['form callback'])) {
        $form_callback = $data_operator_info['form callback'];
        $data['operators'][$data_type][$operator]['form'] = $form_callback;
        if ($form_callback !== FALSE && !isset($data['forms'][$form_callback])) {
          $data['forms'][$form_callback] = $form_callback($node);
        }
      }
    }
  }

  foreach ($node->webform['components'] as $cid => $component) {
    if (webform_component_feature($component['type'], 'conditional')) {
      $data['sources'][$cid]['data_type'] = webform_component_property($component['type'], 'conditional_type');
    }
  }

  drupal_add_js(array('webform' => array('conditionalValues' => $data)), 'setting');
}

/**
 * Helper. Find the matching end of a given subconditional.
 *
 * @param array $rules
 *   Array of conditional rules to be searched.
 * @param int $origin_rid
 *   The starting rule id for the search.
 * @param int $target_delta_level
 *   The level that is sought. 0 for current left. -1 for parent.
 *
 * @return int
 *   The rid of the found rule, or -1 if none. Note that NULL is not used as a
 *   semaphore for "not found" because it casts to 0, which is a valid rule id.
 */
function _webform_conditional_find_end($rules, $origin_rid, $target_delta_level = 0) {
  $rids = array_keys($rules);
  $offset = array_search($origin_rid, $rids);
  $delta_level = 0;
  foreach (array_slice($rules, $offset, NULL, TRUE) as $rid => $conditional) {
    switch ($conditional['source_type']) {
      case 'conditional_start':
        $delta_level++;
        break;

      case 'conditional_end':
        $delta_level--;
        break;
    }
    if ($delta_level == $target_delta_level) {
      return $rid;
    }
  }
  // Mis-matched conditional_start / _end. Return -1.
  return -1;
}

/**
 * Helper. Find the matching start or end of a given subconditional.
 *
 * @see _webform_conditional_find_end()
 */
function _webform_conditional_find_start($rules, $origin_rid, $target_delta_level = 0) {
  $rids = array_keys($rules);
  $offset = array_search($origin_rid, $rids);
  $delta_level = 0;
  foreach (array_reverse(array_slice($rules, 0, $offset + 1, TRUE), TRUE) as $rid => $conditional) {
    switch ($conditional['source_type']) {
      case 'conditional_end':
        $delta_level++;
        break;

      case 'conditional_start':
        $delta_level--;
        break;
    }
    if ($delta_level == $target_delta_level) {
      return $rid;
    }
  }
  // Mis-matched conditional_start / _end. Return -1.
  return -1;
}

/**
 * Submit handler for webform_conditional elements to add a new rule or action.
 */
function webform_conditional_element_add($form, &$form_state) {
  $button = $form_state['clicked_button'];
  $parents = $button['#parents'];
  array_pop($parents);
  $rid = array_pop($parents);

  // Recurse through the form values until we find the Webform conditional rules
  // or actions. Save the conditional prior to descending to rules/actions.
  $parent_values = &$form_state['values'];
  $input_values = &$form_state['input'];
  foreach ($parents as $key) {
    if (array_key_exists($key, $parent_values)) {
      $conditional = $parent_values;
      $parent_values = &$parent_values[$key];
    }
    if (array_key_exists($key, $input_values)) {
      $input_values = &$input_values[$key];
    }
  }

  // Split the list of rules/actions in this conditional and inject into the
  // right spot.
  $rids = array_keys($parent_values);
  $offset = array_search($rid, $rids);
  $default_rule = isset($button['#subconditional'])
                    ? array(
                      'source' => NULL,
                      'source_type' => 'component',
                      'operator' => NULL,
                      'value' => NULL,
                    )
                    : array(
                      'target_type' => 'component',
                      'target' => NULL,
                      'invert' => NULL,
                      'action' => NULL,
                      'argument' => NULL,
                    );

  if (empty($button['#subconditional'])) {
    $new[0] = $parent_values[$rid]['source_type'] == 'component' ? $parent_values[$rid] : $default_rule;
  }
  else {
    // The default andor operator is opposite of current subconditional's
    // operatior.
    $parent_rid = _webform_conditional_find_start($parent_values, $rid, -1);
    $current_op = $parent_rid < 0 ? $conditional['andor'] : $parent_values[$parent_rid]['operator'];
    $current_op = $current_op == 'and' ? 'or' : 'and';
    $new = array(
      array('source_type' => 'conditional_start', 'operator' => $current_op) + $default_rule,
      $default_rule,
      $default_rule,
      array('source_type' => 'conditional_end') + $default_rule,
    );
  }

  // Update both $form_state['values'] and ['input] so that FAPI can merge
  // input values from the POST into the new form.
  $parent_values = array_merge(array_slice($parent_values, 0, $offset + 1), $new, array_slice($parent_values, $offset + 1));
  $input_values = array_merge(array_slice($input_values, 0, $offset + 1), $new, array_slice($input_values, $offset + 1));
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for webform_conditional elements to remove a rule or action.
 */
function webform_conditional_element_remove($form, &$form_state) {
  $button = $form_state['clicked_button'];
  $parents = $button['#parents'];
  $action = array_pop($parents);
  $rid = array_pop($parents);

  // Recurse through the form values until we find the root Webform conditional.
  $parent_values = &$form_state['values'];
  foreach ($parents as $key) {
    if (array_key_exists($key, $parent_values)) {
      $parent_values = &$parent_values[$key];
    }
  }
  switch ($parent_values[$rid]['source_type']) {
    case 'conditional_start':
      unset($parent_values[_webform_conditional_find_end($parent_values, $rid)]);
      break;

    case 'conditional_end':
      unset($parent_values[_webform_conditional_find_start($parent_values, $rid)]);
      break;
  }
  // Remove this rule or action from the list of conditionals.
  unset($parent_values[$rid]);

  $form_state['rebuild'] = TRUE;
}

/**
 * Helper. Delete any subconditionals which contain no rules.
 *
 * @param array $conditional
 *   Conditional array containing the rules.
 *
 * @return array
 *   Array of deleted subconditionals. Empty array if none were deleted.
 */
function webform_delete_empty_subconditionals(&$conditional) {
  $deleted = array();
  do {
    $empty_deleted = FALSE;
    $open_rid = NULL;
    foreach ($conditional['rules'] as $rid => $rule) {
      switch ($rule['source_type']) {
        case 'conditional_start':
          $open_rid = $rid;
          break;

        case 'conditional_end':
          if ($open_rid) {
            // A conditional_start rule was immediately followed by a
            // conditional_end rule. Delete them both. Repeat the check in case
            // the parent is now empty.
            $deleted[$open_rid] = $open_rid;
            $deleted[$rid] = $rid;
            unset($conditional['rules'][$open_rid], $conditional['rules'][$rid]);
            $open_rid = NULL;
            $empty_deleted = TRUE;
          }
          break;

        default:
          $open_rid = NULL;
      }
    }
  } while ($empty_deleted);
  return $deleted;
}

/**
 * AJAX callback to render out adding a new condition.
 */
function webform_conditional_element_ajax($form, $form_state) {
  $button = $form_state['clicked_button'];
  $parents = $button['#parents'];

  // Trim down the parents to go back up to the level of this elements wrapper.
  // The button name (add/remove).
  array_pop($parents);
  // The rule ID.
  array_pop($parents);
  // The "rules" grouping.
  array_pop($parents);

  $element = $form;
  foreach ($parents as $key) {
    if (!isset($element[$key])) {
      // The entire conditional has been removed.
      return '';
    }
    $element = $element[$key];
  }

  return drupal_render($element['conditional']);
}

/**
 * Theme the form for a conditional action.
 */
function theme_webform_conditional($variables) {
  $element = $variables['element'];

  $output = '';
  $output .= '<div class="webform-conditional">';
  $output .= '<span class="webform-conditional-if">' . t('If') . '</span>';

  foreach (element_children($element['rules']) as $rid) {
    $rule = &$element['rules'][$rid];
    switch ($rule['source_type']['#value']) {
      case 'conditional_start':
        $source_phrase = '<div class="webform-subconditional">' . t('(') . '</div>';
        break;

      case 'conditional_end':
        $source_phrase = '<div class="webform-subconditional">' . t(')') . '</div>';
        break;

      default:
        // Hide labels.
        $rule['source']['#title_display'] = 'none';
        $rule['operator']['#title_display'] = 'none';
        $rule['value']['#title_display'] = 'none';

        $source = '<div class="webform-conditional-source">' . drupal_render($rule['source']) . '</div>';
        $operator = '<div class="webform-conditional-operator">' . drupal_render($rule['operator']) . '</div>';
        $value = '<div class="webform-conditional-value">' . drupal_render($rule['value']) . '</div>';

        $source_phrase = t('!source !operator !value', array(
          '!source' => $source,
          '!operator' => $operator,
          '!value' => $value,
        ));
    }

    $output .= '<div class="webform-conditional-rule">';
    // Can't use theme('indentation') here because it causes the draghandle to
    // be located after the last indentation div.
    $output .= str_repeat('<div class="webform-indentation">&nbsp;</div>', $rule['#level']);
    $output .= drupal_render($rule['source_type']);
    $output .= '<div class="webform-container-inline webform-conditional-condition">';
    $output .= $source_phrase;
    $output .= '</div>';

    if (isset($rule['andor'])) {
      $rule['andor']['#title_display'] = 'none';
      $output .= '<div class="webform-conditional-andor webform-container-inline">';
      $output .= drupal_render($rule['andor']);
      $output .= '</div>';
    }

    if (isset($rule['add']) || isset($rule['remove'])) {
      $output .= '<span class="webform-conditional-operations webform-container-inline">';
      $output .= drupal_render($rule['add_subconditional']);
      $output .= drupal_render($rule['add']);
      $output .= drupal_render($rule['remove']);
      $output .= '</span>';
    }

    $output .= '</div>';
  }

  // Hide labels.
  foreach (element_children($element['actions']) as $aid) {
    // Hide labels.
    $element['actions'][$aid]['target']['#title_display'] = 'none';
    $element['actions'][$aid]['invert']['#title_display'] = 'none';
    $element['actions'][$aid]['action']['#title_display'] = 'none';
    $element['actions'][$aid]['argument']['#title_display'] = 'none';

    $target = '<div class="webform-conditional-target">' . drupal_render($element['actions'][$aid]['target']) . '</div>';
    $invert = '<div class="webform-conditional-invert">' . drupal_render($element['actions'][$aid]['invert']) . '</div>';
    $action = '<div class="webform-conditional-action">' . drupal_render($element['actions'][$aid]['action']) . '</div>';
    $argument = '<div class="webform-conditional-argument">' . drupal_render($element['actions'][$aid]['argument']) . '</div>';

    $target_phrase = t('then !target !invert !action !argument', array(
      '!target' => $target,
      '!invert' => $invert,
      '!action' => $action,
      '!argument' => $argument,
    ));

    $output .= '<div class="webform-conditional-action">';
    $output .= '<div class="webform-container-inline webform-conditional-condition">';
    $output .= $target_phrase;
    $output .= '</div>';

    if (isset($element['actions'][$aid]['add']) || isset($element['actions'][$aid]['remove'])) {
      $output .= '<span class="webform-conditional-operations webform-container-inline">';
      $output .= drupal_render($element['actions'][$aid]['add']);
      $output .= drupal_render($element['actions'][$aid]['remove']);
      $output .= '</span>';
    }

    $output .= '</div>';
  }

  $output .= '</div>';

  return $output;
}

/**
 * Return a list of all Webform conditional operators.
 */
function webform_conditional_operators() {
  static $operators;

  if (!isset($operators)) {
    $operators = module_invoke_all('webform_conditional_operator_info');
    drupal_alter('webform_conditional_operators', $operators);
  }

  return $operators;
}

/**
 * Return a nested list of all available operators, suitable for a select list.
 */
function webform_conditional_operators_list() {
  $options = array();
  $operators = webform_conditional_operators();

  foreach ($operators as $data_type => $type_operators) {
    $options[$data_type] = array();
    foreach ($type_operators as $operator => $operator_info) {
      $options[$data_type][$operator] = $operator_info['label'];
    }
  }

  return $options;
}

/**
 * Implements hook_webform_conditional_operator_info().
 *
 * Called from webform.module's webform_webform_conditional_operator_info().
 */
function _webform_conditional_operator_info() {
  // General operators:
  $operators['string']['equal'] = array(
    'label' => t('is'),
    'comparison callback' => 'webform_conditional_operator_string_equal',
    'js comparison callback' => 'conditionalOperatorStringEqual',
    // A form callback is not needed here, since we can use the default,
    // non-JavaScript textfield for all text and numeric fields.
    // @code
    // 'form callback' => 'webform_conditional_operator_text',
    // @endcode
  );
  $operators['string']['not_equal'] = array(
    'label' => t('is not'),
    'comparison callback' => 'webform_conditional_operator_string_not_equal',
    'js comparison callback' => 'conditionalOperatorStringNotEqual',
  );
  $operators['string']['contains'] = array(
    'label' => t('contains'),
    'comparison callback' => 'webform_conditional_operator_string_contains',
    'js comparison callback' => 'conditionalOperatorStringContains',
  );
  $operators['string']['does_not_contain'] = array(
    'label' => t('does not contain'),
    'comparison callback' => 'webform_conditional_operator_string_does_not_contain',
    'js comparison callback' => 'conditionalOperatorStringDoesNotContain',
  );
  $operators['string']['begins_with'] = array(
    'label' => t('begins with'),
    'comparison callback' => 'webform_conditional_operator_string_begins_with',
    'js comparison callback' => 'conditionalOperatorStringBeginsWith',
  );
  $operators['string']['ends_with'] = array(
    'label' => t('ends with'),
    'comparison callback' => 'webform_conditional_operator_string_ends_with',
    'js comparison callback' => 'conditionalOperatorStringEndsWith',
  );
  $operators['string']['empty'] = array(
    'label' => t('is blank'),
    'comparison callback' => 'webform_conditional_operator_string_empty',
    'js comparison callback' => 'conditionalOperatorStringEmpty',
    // No value form at all.
    'form callback' => FALSE,
  );
  $operators['string']['not_empty'] = array(
    'label' => t('is not blank'),
    'comparison callback' => 'webform_conditional_operator_string_not_empty',
    'js comparison callback' => 'conditionalOperatorStringNotEmpty',
    // No value form at all.
    'form callback' => FALSE,
  );

  // Numeric operators.
  $operators['numeric']['equal'] = array(
    'label' => t('is equal to'),
    'comparison callback' => 'webform_conditional_operator_numeric_equal',
    'js comparison callback' => 'conditionalOperatorNumericEqual',
  );
  $operators['numeric']['not_equal'] = array(
    'label' => t('is not equal to'),
    'comparison callback' => 'webform_conditional_operator_numeric_not_equal',
    'js comparison callback' => 'conditionalOperatorNumericNotEqual',
  );
  $operators['numeric']['less_than'] = array(
    'label' => t('is less than'),
    'comparison callback' => 'webform_conditional_operator_numeric_less_than',
    'js comparison callback' => 'conditionalOperatorNumericLessThan',
  );
  $operators['numeric']['less_than_equal'] = array(
    'label' => t('is less than or equal'),
    'comparison callback' => 'webform_conditional_operator_numeric_less_than_equal',
    'js comparison callback' => 'conditionalOperatorNumericLessThanEqual',
  );
  $operators['numeric']['greater_than'] = array(
    'label' => t('is greater than'),
    'comparison callback' => 'webform_conditional_operator_numeric_greater_than',
    'js comparison callback' => 'conditionalOperatorNumericGreaterThan',
  );
  $operators['numeric']['greater_than_equal'] = array(
    'label' => t('is greater than or equal'),
    'comparison callback' => 'webform_conditional_operator_numeric_greater_than_equal',
    'js comparison callback' => 'conditionalOperatorNumericGreaterThanEqual',
  );
  $operators['numeric']['empty'] = array(
    'label' => t('is blank'),
    'comparison callback' => 'webform_conditional_operator_string_empty',
    'js comparison callback' => 'conditionalOperatorStringEmpty',
    // No value form at all.
    'form callback' => FALSE,
  );
  $operators['numeric']['not_empty'] = array(
    'label' => t('is not blank'),
    'comparison callback' => 'webform_conditional_operator_string_not_empty',
    'js comparison callback' => 'conditionalOperatorStringNotEmpty',
    // No value form at all.
    'form callback' => FALSE,
  );

  // Select operators.
  $operators['select']['equal'] = array(
    'label' => t('is'),
    'comparison callback' => 'webform_conditional_operator_string_equal',
    'js comparison callback' => 'conditionalOperatorStringEqual',
    'form callback' => 'webform_conditional_form_select',
  );
  $operators['select']['not_equal'] = array(
    'label' => t('is not'),
    'comparison callback' => 'webform_conditional_operator_string_not_equal',
    'js comparison callback' => 'conditionalOperatorStringNotEqual',
    'form callback' => 'webform_conditional_form_select',
  );
  $operators['select']['less_than'] = array(
    'label' => t('is before'),
    'comparison callback' => 'webform_conditional_operator_select_less_than',
    'js comparison callback' => 'conditionalOperatorSelectLessThan',
    'form callback' => 'webform_conditional_form_select',
  );
  $operators['select']['less_than_equal'] = array(
    'label' => t('is or is before'),
    'comparison callback' => 'webform_conditional_operator_select_less_than_equal',
    'js comparison callback' => 'conditionalOperatorSelectLessThanEqual',
    'form callback' => 'webform_conditional_form_select',
  );
  $operators['select']['greater_than'] = array(
    'label' => t('is after'),
    'comparison callback' => 'webform_conditional_operator_select_greater_than',
    'js comparison callback' => 'conditionalOperatorSelectGreaterThan',
    'form callback' => 'webform_conditional_form_select',
  );
  $operators['select']['greater_than_equal'] = array(
    'label' => t('is or is after'),
    'comparison callback' => 'webform_conditional_operator_select_greater_than_equal',
    'js comparison callback' => 'conditionalOperatorSelectGreaterThanEqual',
    'form callback' => 'webform_conditional_form_select',
  );
  $operators['select']['empty'] = array(
    'label' => t('is empty'),
    'comparison callback' => 'webform_conditional_operator_string_empty',
    'js comparison callback' => 'conditionalOperatorStringEmpty',
    // No value form at all.
    'form callback' => FALSE,
  );
  $operators['select']['not_empty'] = array(
    'label' => t('is not empty'),
    'comparison callback' => 'webform_conditional_operator_string_not_empty',
    'js comparison callback' => 'conditionalOperatorStringNotEmpty',
    // No value form at all.
    'form callback' => FALSE,
  );

  // Date operators:
  $operators['date']['equal'] = array(
    'label' => t('is on'),
    'comparison callback' => 'webform_conditional_operator_datetime_equal',
    'comparison prepare js' => 'webform_conditional_prepare_date_js',
    'js comparison callback' => 'conditionalOperatorDateEqual',
    'form callback' => 'webform_conditional_form_date',
  );
  $operators['date']['not_equal'] = array(
    'label' => t('is not on'),
    'comparison callback' => 'webform_conditional_operator_datetime_not_equal',
    'comparison prepare js' => 'webform_conditional_prepare_date_js',
    'js comparison callback' => 'conditionalOperatorDateNotEqual',
    'form callback' => 'webform_conditional_form_date',
  );
  $operators['date']['before'] = array(
    'label' => t('is before'),
    'comparison callback' => 'webform_conditional_operator_datetime_before',
    'comparison prepare js' => 'webform_conditional_prepare_date_js',
    'js comparison callback' => 'conditionalOperatorDateBefore',
    'form callback' => 'webform_conditional_form_date',
  );
  $operators['date']['before_equal'] = array(
    'label' => t('is on or before'),
    'comparison callback' => 'webform_conditional_operator_datetime_before_equal',
    'comparison prepare js' => 'webform_conditional_prepare_date_js',
    'js comparison callback' => 'conditionalOperatorDateBeforeEqual',
    'form callback' => 'webform_conditional_form_date',
  );
  $operators['date']['after'] = array(
    'label' => t('is after'),
    'comparison callback' => 'webform_conditional_operator_datetime_after',
    'comparison prepare js' => 'webform_conditional_prepare_date_js',
    'js comparison callback' => 'conditionalOperatorDateAfter',
    'form callback' => 'webform_conditional_form_date',
  );
  $operators['date']['after_equal'] = array(
    'label' => t('is on or after'),
    'comparison callback' => 'webform_conditional_operator_datetime_after_equal',
    'comparison prepare js' => 'webform_conditional_prepare_date_js',
    'js comparison callback' => 'conditionalOperatorDateAfterEqual',
    'form callback' => 'webform_conditional_form_date',
  );

  // Time operators:
  $operators['time']['equal'] = array(
    'label' => t('is at'),
    'comparison callback' => 'webform_conditional_operator_datetime_equal',
    'comparison prepare js' => 'webform_conditional_prepare_time_js',
    'js comparison callback' => 'conditionalOperatorTimeEqual',
    'form callback' => 'webform_conditional_form_time',
  );
  $operators['time']['not_equal'] = array(
    'label' => t('is not at'),
    'comparison callback' => 'webform_conditional_operator_datetime_not_equal',
    'comparison prepare js' => 'webform_conditional_prepare_time_js',
    'js comparison callback' => 'conditionalOperatorTimeNotEqual',
    'form callback' => 'webform_conditional_form_time',
  );
  $operators['time']['before'] = array(
    'label' => t('is before'),
    'comparison callback' => 'webform_conditional_operator_datetime_before',
    'comparison prepare js' => 'webform_conditional_prepare_time_js',
    'js comparison callback' => 'conditionalOperatorTimeBefore',
    'form callback' => 'webform_conditional_form_time',
  );
  $operators['time']['before_equal'] = array(
    'label' => t('is at or before'),
    'comparison callback' => 'webform_conditional_operator_datetime_before_equal',
    'comparison prepare js' => 'webform_conditional_prepare_time_js',
    'js comparison callback' => 'conditionalOperatorTimeBeforeEqual',
    'form callback' => 'webform_conditional_form_time',
  );
  $operators['time']['after'] = array(
    'label' => t('is after'),
    'comparison callback' => 'webform_conditional_operator_datetime_after',
    'comparison prepare js' => 'webform_conditional_prepare_time_js',
    'js comparison callback' => 'conditionalOperatorTimeAfter',
    'form callback' => 'webform_conditional_form_time',
  );
  $operators['time']['after_equal'] = array(
    'label' => t('is at or after'),
    'comparison callback' => 'webform_conditional_operator_datetime_after_equal',
    'comparison prepare js' => 'webform_conditional_prepare_time_js',
    'js comparison callback' => 'conditionalOperatorTimeAfterEqual',
    'form callback' => 'webform_conditional_form_time',
  );

  return $operators;
}

/**
 * Form callback for select-type conditional fields.
 *
 * Unlike other built-in conditional value forms, the form callback for select
 * types provides an array of forms, keyed by the $cid, which is the "source"
 * for the condition.
 */
function webform_conditional_form_select($node) {
  static $count = 0;
  $forms = array();
  webform_component_include('select');
  foreach ($node->webform['components'] as $cid => $component) {
    if (webform_component_property($component['type'], 'conditional_type') == 'select') {
      // TODO: Use a pluggable mechanism for retrieving select list values.
      $options = _webform_select_options($component);
      $element = array(
        '#type' => 'select',
        '#multiple' => FALSE,
        '#size' => NULL,
        '#attributes' => array(),
        '#id' => NULL,
        '#name' => 'webform-conditional-select-' . $cid . '-' . $count,
        '#options' => $options,
        '#parents' => array(),
      );
      $forms[$cid] = drupal_render($element);
    }
  }
  $count++;
  return $forms;
}

/**
 * Form callback for date conditional fields.
 */
function webform_conditional_form_date($node) {
  static $count = 0;
  $element = array(
    '#title' => NULL,
    '#title_display' => 'none',
    '#size' => 24,
    '#attributes' => array('placeholder' => t('@format or valid date', array('@format' => webform_date_format('short')))),
    '#type' => 'textfield',
    '#name' => 'webform-conditional-date-' . $count++,
  );
  return drupal_render($element);
}

/**
 * Form callback for time conditional fields.
 */
function webform_conditional_form_time($node) {
  static $count = 0;
  $element = array(
    '#title' => NULL,
    '#title_display' => 'none',
    '#size' => 24,
    '#attributes' => array('placeholder' => t('HH:MMam or valid time')),
    '#type' => 'textfield',
    '#name' => 'webform-conditional-time-' . $count++,
  );
  return drupal_render($element);
}

/**
 * Load a conditional setting from the database.
 */
function webform_conditional_load($rgid, $nid) {
  $node = node_load($nid);

  $conditional = isset($node->webform['conditionals'][$rgid]) ? $node->webform['conditionals'][$rgid] : FALSE;

  return $conditional;
}

/**
 * Insert a conditional rule group into the database.
 */
function webform_conditional_insert($conditional) {
  drupal_write_record('webform_conditional', $conditional);
  foreach ($conditional['rules'] as $rid => $rule) {
    $rule['nid'] = $conditional['nid'];
    $rule['rgid'] = $conditional['rgid'];
    $rule['rid'] = $rid;
    drupal_write_record('webform_conditional_rules', $rule);
  }
  foreach ($conditional['actions'] as $aid => $action) {
    $action['nid'] = $conditional['nid'];
    $action['rgid'] = $conditional['rgid'];
    $action['aid'] = $aid;
    drupal_write_record('webform_conditional_actions', $action);
  }
}

/**
 * Update a conditional setting in the database.
 */
function webform_conditional_update($node, $conditional) {
  webform_conditional_delete($node, $conditional);
  webform_conditional_insert($conditional);
}

/**
 * Delete a conditional rule group.
 */
function webform_conditional_delete($node, $conditional) {
  db_delete('webform_conditional')
    ->condition('nid', $node->nid)
    ->condition('rgid', $conditional['rgid'])
    ->execute();
  db_delete('webform_conditional_rules')
    ->condition('nid', $node->nid)
    ->condition('rgid', $conditional['rgid'])
    ->execute();
  db_delete('webform_conditional_actions')
    ->condition('nid', $node->nid)
    ->condition('rgid', $conditional['rgid'])
    ->execute();
}

/**
 * Loop through all the conditional settings and add needed JavaScript settings.
 *
 * We do a bit of optimization for JavaScript before adding to the page as
 * settings. We remove unnecessary data structures and provide a "source map"
 * so that JavaScript can quickly determine if it needs to check rules when a
 * field on the page has been modified.
 *
 * @param object $node
 *   The loaded node object, containing the webform.
 * @param array $submission_data
 *   The cid-indexed array of existing submission values to be included for
 *   sources outside of the current page.
 * @param int $page_num
 *   The number of the page for which javascript settings should be generated.
 *
 * @return array
 *   Array of settings to be send to the browser as javascript settings.
 */
function webform_conditional_prepare_javascript($node, $submission_data, $page_num) {
  $settings = array(
    'ruleGroups' => array(),
    'sourceMap' => array(),
    'values' => array(),
  );
  $operators = webform_conditional_operators();
  $conditionals = $node->webform['conditionals'];
  $components = $node->webform['components'];
  $topological_order = webform_get_conditional_sorter($node)->getOrder();
  foreach ($topological_order[$page_num] as $conditional_spec) {
    $conditional = $conditionals[$conditional_spec['rgid']];
    $rgid_key = 'rgid_' . $conditional['rgid'];
    // Assemble the main conditional group settings.
    $settings['ruleGroups'][$rgid_key] = array(
      'andor' => $conditional['andor'],
    );
    foreach ($conditional['actions'] as $action) {
      if ($action['target_type'] == 'component') {
        $target_component = $components[$action['target']];
        $target_parents = webform_component_parent_keys($node, $target_component);
        $aid_key = 'aid_' . $action['aid'];
        $action_settings = array(
          'target' => 'webform-component--' . str_replace('_', '-', implode('--', $target_parents)),
          'invert' => (int) $action['invert'],
          'action' => $action['action'],
          'argument' => $components[$action['target']]['type'] == 'markup' ? filter_xss_admin($action['argument']) : $action['argument'],
        );
        $settings['ruleGroups'][$rgid_key]['actions'][$aid_key] = $action_settings;
      }
    }
    // Add on the list of rules to the conditional group.
    foreach ($conditional['rules'] as $rule) {
      $rid_key = 'rid_' . $rule['rid'];
      switch ($rule['source_type']) {
        case 'component':
          $source_component = $components[$rule['source']];
          $source_parents = webform_component_parent_keys($node, $source_component);
          $source_id = 'webform-component--' . str_replace('_', '-', implode('--', $source_parents));

          // If this source has a value set, add that as a setting.
          // NULL or array(NULL) should be sent as an empty array to simplify the jQuery.
          if (isset($submission_data[$source_component['cid']])) {
            $source_value = $submission_data[$source_component['cid']];
            $source_value = is_array($source_value) ? $source_value : array($source_value);
            $settings['values'][$source_id] = $source_value === array(NULL) ? array() : $source_value;
          }

          $conditional_type = webform_component_property($source_component['type'], 'conditional_type');
          $operator_info = $operators[$conditional_type][$rule['operator']];
          $rule_settings = array(
            'source_type' => $rule['source_type'],
            'source' => $source_id,
            'value' => $rule['value'],
            'callback' => $operator_info['js comparison callback'],
          );
          if (isset($operator_info['comparison prepare js'])) {
            $callback = $operator_info['comparison prepare js'];
            $rule_settings['value'] = $callback($rule['value']);
          }
          $settings['ruleGroups'][$rgid_key]['rules'][$rid_key] = $rule_settings;
          $settings['sourceMap'][$source_id][$rgid_key] = $rgid_key;
          break;

        case 'conditional_start':
          $settings['ruleGroups'][$rgid_key]['rules'][$rid_key] = array(
            'source_type' => $rule['source_type'],
            'andor' => $rule['operator'],
          );
          break;

        case 'conditional_end':
          $settings['ruleGroups'][$rgid_key]['rules'][$rid_key] = array(
            'source_type' => $rule['source_type'],
          );
          break;
      }
    }
  }

  return $settings;
}

/**
 * Determine whether a component type is capable of a given conditional action.
 */
function webform_conditional_action_able($component_type, $action) {
  switch ($action) {
    case 'show':
      return TRUE;

    case 'require':
      return webform_component_feature($component_type, 'required');

    default:
      return webform_component_feature($component_type, "conditional_action_$action");
  }
}

/**
 * Prepare a conditional value for adding as a JavaScript setting.
 */
function webform_conditional_prepare_date_js($rule_value) {
  // Convert the time/date string to a UTC timestamp for comparison. Note that
  // this means comparisons against immediate times (such as "now") may be
  // slightly stale by the time the comparison executes. Timestamps are in
  // milliseconds, as to match JavaScript's Date.toString() method.
  $date = webform_strtodate('c', $rule_value, 'UTC');
  return webform_strtotime($date);
}

/**
 * Prepare a conditional value for adding as a JavaScript setting.
 */
function webform_conditional_prepare_time_js($rule_value) {
  $date = webform_conditional_prepare_date_js($rule_value);
  $today = webform_strtodate('c', 'today', 'UTC');
  $today = webform_strtotime($today);
  return $date - $today;
}

/**
 * Conditional callback for string comparisons.
 */
function webform_conditional_operator_string_equal($input_values, $rule_value) {
  foreach ($input_values as $value) {
    // Checkbox values come in as 0 integers for unchecked boxes.
    $value = ($value === 0) ? '' : $value;
    if (strcasecmp($value, $rule_value) === 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Conditional callback for string comparisons.
 */
function webform_conditional_operator_string_not_equal($input_values, $rule_value) {
  return !webform_conditional_operator_string_equal($input_values, $rule_value);
}

/**
 * Conditional callback for string comparisons.
 */
function webform_conditional_operator_string_contains($input_values, $rule_value) {
  foreach ($input_values as $value) {
    if (stripos($value, $rule_value) !== FALSE) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Conditional callback for string comparisons.
 */
function webform_conditional_operator_string_does_not_contain($input_values, $rule_value) {
  return !webform_conditional_operator_string_contains($input_values, $rule_value);
}

/**
 * Conditional callback for string comparisons.
 */
function webform_conditional_operator_string_begins_with($input_values, $rule_value) {
  foreach ($input_values as $value) {
    if (stripos($value, $rule_value) === 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Conditional callback for string comparisons.
 */
function webform_conditional_operator_string_ends_with($input_values, $rule_value) {
  foreach ($input_values as $value) {
    if (strripos($value, $rule_value) === strlen($value) - strlen($rule_value)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Conditional callback for checking for empty fields.
 */
function webform_conditional_operator_string_empty($input_values, $rule_value) {
  $empty = TRUE;
  foreach ($input_values as $value) {
    if ($value !== '' && $value !== NULL && $value !== 0) {
      $empty = FALSE;
      break;
    }
  }
  return $empty;
}

/**
 * Conditional callback for checking for empty fields.
 */
function webform_conditional_operator_string_not_empty($input_values, $rule_value) {
  return !webform_conditional_operator_string_empty($input_values, $rule_value);
}

/**
 * Conditional callback for select comparisons.
 */
function webform_conditional_operator_select_less_than($input_values, $rule_value, $component) {
  return empty($input_values) ? FALSE : webform_compare_select($input_values[0], $rule_value, _webform_select_options($component, TRUE)) < 0;
}

/**
 * Conditional callback for select comparisons.
 */
function webform_conditional_operator_select_less_than_equal($input_values, $rule_value, $component) {
  $comparison = empty($input_values) ? NULL : webform_compare_select($input_values[0], $rule_value, _webform_select_options($component, TRUE));
  return $comparison < 0 || $comparison === 0;
}

/**
 * Conditional callback for select comparisons.
 */
function webform_conditional_operator_select_greater_than($input_values, $rule_value, $component) {
  return empty($input_values) ? FALSE : webform_compare_select($input_values[0], $rule_value, _webform_select_options($component, TRUE)) > 0;
}

/**
 * Conditional callback for select comparisons.
 */
function webform_conditional_operator_select_greater_than_equal($input_values, $rule_value, $component) {
  $comparison = empty($input_values) ? NULL : webform_compare_select($input_values[0], $rule_value, _webform_select_options($component, TRUE));
  return $comparison > 0 || $comparison === 0;
}

/**
 * Conditional callback for numeric comparisons.
 */
function webform_conditional_operator_numeric_equal($input_values, $rule_value) {
  return empty($input_values) ? FALSE : webform_compare_floats($input_values[0], $rule_value) === 0;
}

/**
 * Conditional callback for numeric comparisons.
 */
function webform_conditional_operator_numeric_not_equal($input_values, $rule_value) {
  return !webform_conditional_operator_numeric_equal($input_values, $rule_value);
}

/**
 * Conditional callback for numeric comparisons.
 */
function webform_conditional_operator_numeric_less_than($input_values, $rule_value) {
  return empty($input_values) ? FALSE : webform_compare_floats($input_values[0], $rule_value) < 0;
}

/**
 * Conditional callback for numeric comparisons.
 */
function webform_conditional_operator_numeric_less_than_equal($input_values, $rule_value) {
  $comparison = empty($input_values) ? NULL : webform_compare_floats($input_values[0], $rule_value);
  return $comparison < 0 || $comparison === 0;
}

/**
 * Conditional callback for numeric comparisons.
 */
function webform_conditional_operator_numeric_greater_than($input_values, $rule_value) {
  return empty($input_values) ? FALSE : webform_compare_floats($input_values[0], $rule_value) > 0;
}

/**
 * Conditional callback for numeric comparisons.
 */
function webform_conditional_operator_numeric_greater_than_equal($input_values, $rule_value) {
  $comparison = empty($input_values) ? NULL : webform_compare_floats($input_values[0], $rule_value);
  return $comparison > 0 || $comparison === 0;
}

/**
 * Conditional callback for date and time comparisons.
 */
function webform_conditional_operator_datetime_equal($input_values, $rule_value) {
  $input_values = webform_conditional_value_datetime($input_values);
  return empty($input_values) ? FALSE : webform_strtotime($input_values[0]) === webform_strtotime($rule_value);
}

/**
 * Conditional callback for date and time comparisons.
 */
function webform_conditional_operator_datetime_not_equal($input_values, $rule_value) {
  return !webform_conditional_operator_datetime_equal($input_values, $rule_value);
}

/**
 * Conditional callback for date and time comparisons.
 */
function webform_conditional_operator_datetime_after($input_values, $rule_value) {
  $input_values = webform_conditional_value_datetime($input_values);
  return empty($input_values) ? FALSE : webform_strtotime($input_values[0]) > webform_strtotime($rule_value);
}

/**
 * Conditional callback for date and time comparisons.
 */
function webform_conditional_operator_datetime_after_equal($input_values, $rule_value) {
  return webform_conditional_operator_datetime_after($input_values, $rule_value) ||
    webform_conditional_operator_datetime_equal($input_values, $rule_value);
}

/**
 * Conditional callback for date and time comparisons.
 */
function webform_conditional_operator_datetime_before($input_values, $rule_value) {
  $input_values = webform_conditional_value_datetime($input_values);
  return empty($input_values) ? FALSE : webform_strtotime($input_values[0]) < webform_strtotime($rule_value);
}

/**
 * Conditional callback for date and time comparisons.
 */
function webform_conditional_operator_datetime_before_equal($input_values, $rule_value) {
  return webform_conditional_operator_datetime_before($input_values, $rule_value) ||
    webform_conditional_operator_datetime_equal($input_values, $rule_value);
}

/**
 * Utility function to convert incoming time and dates into strings.
 */
function webform_conditional_value_datetime($input_values) {
  // Convert times into a string.
  $input_values = isset($input_values['hour']) ? array(webform_date_string(webform_time_convert($input_values, '24-hour'), 'time')) : $input_values;
  // Convert dates into a string.
  $input_values = isset($input_values['month']) ? array(webform_date_string($input_values, 'date')) : $input_values;
  return $input_values;
}

/**
 * Utility function to compare values of a select component.
 *
 * @param string $a
 *   First select option key to compare.
 * @param string $b
 *   Second select option key to compare.
 * @param array $options
 *   Associative array where the $a and $b are within the keys.
 *
 * @return integer based upon position of $a and $b in $options
 *   -N if $a above (<) $b
 *   0 if $a = $b
 *   +N if $a is below (>) $b
 */
function webform_compare_select($a, $b, $options) {
  // Select keys that are integer-like strings are numeric indices in PHP.
  // Convert the array keys to an array of strings.
  $options_array = array_map(function ($i) {
    return (string) $i;
  }, array_keys($options));
  $a_position = array_search($a, $options_array, TRUE);
  $b_position = array_search($b, $options_array, TRUE);
  return ($a_position === FALSE || $a_position === FALSE) ? NULL : $a_position - $b_position;

}
