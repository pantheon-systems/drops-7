<?php

/**
 * @file
 * General administration functions.
 */

/**
 * Menu callback for the name autocomplete.
 *
 * Retrieve a JSON object containing autocomplete suggestions for existing
 * values as defined by the widget.
 *
 * This is complicated by the fact that there can be multiple sources and that
 * multiple components may want to be searched.
 */
function name_autocomplete($field_name, $target, $string = '') {
  $matches = array();
  $limit = 10;

  $all_components = array('given', 'middle', 'family', 'title', 'credentials', 'generational');
  if ($string) {
    $field_name = str_replace('-', '_', $field_name);
    $namedb_exists = module_exists('namedb');
    if ($field = field_info_field($field_name)) {
      $settings = $field['settings'];
      foreach ($all_components as $component) {
        if (!isset($settings['autocomplete_source'][$component])) {
          $settings['autocomplete_source'][$component] = array();
        }
        $settings['autocomplete_source'][$component] = array_filter($settings['autocomplete_source'][$component]);
      }
    }
    else {
      $src = $namedb_exists ? array('namedb' => 'namedb') : array();
      $settings = array(
        'autocomplete_source' => array(
          'title' => $src,
          'given' => $src,
          'middle' => $src,
          'family' => $src,
          'generational' => $src,
          'credentials' => $src,
        ),
        'autocomplete_separator' => array(
          'title' => ' ',
          'given' => ' -',
          'middle' => ' -',
          'family' => ' -',
          'generational' => ' ',
          'credentials' => ', ',
        ),
      );
    }

    $action = array();
    switch ($target) {
      case 'name':
        $action['components'] = drupal_map_assoc(array('given', 'middle', 'family'));
        break;

      case 'name-all':
        $action['components'] = drupal_map_assoc($all_components);
        break;

      case 'title':
      case 'given':
      case 'middle':
      case 'family':
      case 'credentials':
      case 'generational':
        $action['components'] = array($target => $target);
        break;

      default:
        $action['components'] = array();
        foreach (explode('-', $target) as $component) {
          if (in_array($component, array('title', 'given', 'middle', 'family', 'credentials', 'generational'))) {
            $action['components'][$component] = $component;
          }
        }
        break;

    }

    $action['source'] = array(
      // 'vocab-123' => array(), etc
      'namedb' => array(),
      'title' => array(),
      'generational' => array(),
//      'data' => array(),
    );

    $action['separater'] = '';

    foreach ($action['components'] as $component) {
      if (empty($settings['autocomplete_source'][$component])) {
        unset($action['components'][$component]);
      }
      else {
        $sep = (string)$settings['autocomplete_separator'][$component];
        if (empty($sep)) {
          $sep = ' ';
        }
        for ($i = 0; $i <= count($sep); $i++) {
          if (strpos($action['separater'], $sep{$i}) === FALSE) {
            $action['separater'] .= $sep{$i};
          }
        }
        $found_source = FALSE;

        foreach ((array)$settings['autocomplete_source'][$component] as $src) {
          if ($src == 'namedb' && !$namedb_exists) {
            continue;
          }
          if ($src == 'data' && !$field) {
            continue;
          }
          if ($src == 'title' || $src == 'generational') {
            if (!$field || $component != $src) {
              continue;
            }
          }
          $found_source = TRUE;
          $action['source'][$src][] = $component;
        }

        if (!$found_source) {
          unset($action['components'][$component]);
        }
      }
    }

    $pieces = preg_split('/[' . preg_quote($action['separater']) . ']+/', $string);

    // We should have nice clean parameters to query.
    if (!empty($pieces) && !empty($action['components'])) {
      $test_string = strtolower(array_pop($pieces));
      $base_string = substr($string, 0, strlen($string) - strlen($test_string));

      // Query order is: vocab, namedb, title, generational, data
      if (count($action['source']['namedb'])) {
        $attributes = array(
          'status' => 1,
          'banned' => 0,
          'limit' => $limit,
        );
        $results = namedb_query_names($test_string, $action['source']['namedb'], $attributes);
        foreach ($results as $name => $safe_name) {
          $matches[$base_string . $name] = $safe_name;
          $limit--;
        }
      }

      if ($limit > 0 && count($action['source']['title'])) {
        $options = name_field_get_options($field, 'title');
        foreach ($options as $key => $option) {
          if (strpos(strtolower($key), $test_string) === 0 || strpos(strtolower($option), $test_string) === 0) {
            $matches[$base_string . $key] = $key;
            $limit--;
          }
        }
      }

      if ($limit > 0 && count($action['source']['generational'])) {
        $options = name_field_get_options($field, 'generational');
        foreach ($options as $key => $option) {
          if (strpos(strtolower($key), $test_string) === 0 || strpos(strtolower($option), $test_string) === 0) {
            $matches[$base_string . $key] = $key;
            $limit--;
          }
        }
      }

      if ($limit > 0 && isset($action['source']['data']) && count($action['source']['data'])) {
      }
    }
  }

  drupal_json_output($matches);
}

/**
 * Form builder function for module settings.
 */
function name_admin_settings_form($form, &$form_state) {
  $settings = name_settings();

  $form['#validate'][] = 'name_admin_settings_form_validate';
  $form['name_settings'] = array('#tree' => TRUE);
  $form['name_settings']['default_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Default format'),
    '#default_value' => $settings['default_format'],
    '#description' => t('See help on drupal.org for more info.'),
    '#required' => TRUE,
  );
  $form['name_settings']['sep1'] = array(
    '#type' => 'textfield',
    '#title' => t('Separator 1 replacement token'),
    '#default_value' => $settings['sep1'],
  );
  $form['name_settings']['sep2'] = array(
    '#type' => 'textfield',
    '#title' => t('Separator 2 replacement token'),
    '#default_value' => $settings['sep2'],
  );
  $form['name_settings']['sep3'] = array(
    '#type' => 'textfield',
    '#title' => t('Separator 3 replacement token'),
    '#default_value' => $settings['sep3'],
  );
  // As the fieldset does not have the #input flag, this is not saved.
  $form['name_format_help'] = _name_get_name_format_help_form();

  return system_settings_form($form);
}

function name_admin_settings_form_validate($form, &$form_state) {
  $default_format = trim($form_state['values']['name_settings']['default_format']);
  if (empty($default_format) && strlen($default_format) == 0) {
      form_set_error('name_settings][default_format', t('%title field is required.', array('%title' => $form['name_settings']['default_format']['#title'])));
  }
}
/**
 * Lists the known custom formats.
 */
function name_list_custom_formats() {
  $header = array(t('Name'), t('System code'), t('Format'), t('Examples'), t('Actions'));
  $rows = array();

  $example_names = array(
    1 => array(
      'title' => 'Mr',
      'given' => 'Joe',
      'middle' => 'John Peter Mark',
      'family' => 'Doe',
      'generational' => 'Jnr.',
      'credentials' => 'B.Sc., Ph.D.',
    ),
    2 => array(
      'title' => '',
      'given' => 'JOAN',
      'middle' => 'SUE',
      'family' => 'DOE',
      'generational' => '',
      'credentials' => '',
    ),
    3 => array(
      'title' => '',
      'given' => 'Prince',
      'middle' => '',
      'family' => '',
      'generational' => '',
      'credentials' => '',
    ),
  );
  $default_format = new stdClass();
  $default_format->ncfid = 0;
  $default_format->name = t('Default');
  $default_format->machine_name = 'default';
  $default_format->format = name_settings('default_format');
  $custom_formats = array('0' => $default_format) + name_get_custom_formats();

  foreach ($custom_formats as $ncfid => $tag) {
    $row = array();
    $row[] = l($tag->name, 'admin/config/regional/name/' . ($ncfid ? $ncfid : 'settings'));
    $row[] = $tag->machine_name;
    $row[] = check_plain($tag->format);

    $examples = array();
    foreach ($example_names as $index => $example_name) {
      $formatted = check_plain(name_format($example_name, $tag->format));
      if (empty($formatted)) {
        $formatted = '<em>&lt;&lt;empty&gt;&gt;</em>';
      }
      $examples[] = $formatted . " <sup>{$index}</sup>";
    }
    $row[] = implode('<br/>', $examples);

    if ($ncfid) {
      $links = array();
      $links[] = l(t('Edit'), 'admin/config/regional/name/' . $ncfid);
      $links[] = l(t('Delete'), 'admin/config/regional/name/' . $ncfid . '/delete');
      $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $links);
    }
    else {
      $row[] = l(t('Edit'), 'admin/config/regional/name/settings');
    }
    $rows[] = array('data' => $row, 'id' => 'name-' . $ncfid);
  }

  $help = '<p><strong>' . t('The three examples are for the following users:') . '</strong><p>';
  $help_items = array();
  foreach ($example_names as $example_name) {
    // TODO make the labels generic
    $help_items[] = t('The example %user has the following components; title is "%title", given is "%given", middle is "%middle", family is "%family", generational is "%generational", credentials is "%credentials"',
    array(
      '%user' => name_format($example_name, 't+ g+ m+ f+ s+ c'),
      '%title' => $example_name['title'] ? $example_name['title'] : '<<empty>>',
      '%given' => $example_name['given'] ? $example_name['given'] : '<<empty>>',
      '%middle' => $example_name['middle'] ? $example_name['middle'] : '<<empty>>',
      '%family' => $example_name['family'] ? $example_name['family'] : '<<empty>>',
      '%generational' => $example_name['generational'] ? $example_name['generational'] : '<<empty>>',
      '%credentials' => $example_name['credentials'] ? $example_name['credentials'] : '<<empty>>',
    ));
  }

  // A workaround to display a collapable fieldset inside a standard page.
  // Revisit and remove this once the issue is sorted.
  $form_state = array();
  $fieldset = _name_get_name_format_help_form();
  $fieldset = form_process_fieldset($fieldset, $form_state);

  return array(
    'table' => array(
       '#markup' => theme('table', array('header' => $header, 'rows' => $rows)) . $help . theme('item_list', array('items' => $help_items, 'type' => 'ol')),
    ),
    'name_format_help' => $fieldset,
  );

}

/**
 * A helper function to generate the format string parameter help fieldset.
 */
function _name_get_name_format_help_form() {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Format string help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#parents' => array(),
    'format_parameters' => array(
      '#markup' => theme('name_format_parameter_help'),
    ),
  );
  return $form;
}

/**
 * Form callback to edit or add a new custom name format.
 */
function name_custom_formats_form($form, $form_state, $edit = array()) {
  $edit += array(
    'ncfid' => NULL,
    'name' => '',
    'machine_name' => '',
    'format' => '',
  );

  $form = array();
  $form['ncfid'] = array(
    '#type' => 'value',
    '#value' => $edit['ncfid'],
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $edit['name'],
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $form['machine_name'] = array(
    '#title' => t('Machine-readable name'),
    '#description' => t('The unique machine-readable name for this format. This can only contain lowercase letters, numbers and underscores. The keyword %default is reserved for internal usage.', array('%default' => 'default' /* do not translate */)),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $edit['machine_name'],
  );
  $form['format'] = array(
    '#type' => 'textfield',
    '#title' => t('Format'),
    '#default_value' => $edit['format'],
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $form['name_format_help'] = _name_get_name_format_help_form();

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  if (!empty($edit['ncfid'])) {
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  return $form;
}


/**
 * Custom validation for name_custom_formats_form().
 */
function name_custom_formats_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  // Ensure that the name is unique
  if (empty($values['ncfid'])) {
    $count = db_query("SELECT count(*) FROM {name_custom_format} WHERE name = :name", array(':name' => $values['name']))->fetchField();
    $mcount = db_query("SELECT count(*) FROM {name_custom_format} WHERE machine_name = :machine_name", array(':machine_name' => $values['machine_name']))->fetchField();
  }
  else {
    $count = db_query("SELECT count(*) FROM {name_custom_format} WHERE name = :name AND ncfid <> :ncfid", array(':name' => $values['name'], ':ncfid' => $values['ncfid']))->fetchField();
    $mcount = db_query("SELECT count(*) FROM {name_custom_format} WHERE machine_name = :machine_name AND ncfid <> :ncfid", array(':machine_name' => $values['machine_name'], ':ncfid' => $values['ncfid']))->fetchField();
  }
  if ($count) {
    form_set_error('name', t('The name you have chosen is already in use.'));
  }
  if ($mcount) {
    form_set_error('machine_name', t('The machine-readable name you have chosen is already in use.'));
  }
  elseif ($values['machine_name'] == 'default') {
    form_set_error('machine_name', t('The machine-readable name you have chosen is reserved.'));
  }
  elseif (!preg_match("/^[a-z0-9_]+$/", $values['machine_name'])) {
    form_set_error('machine_name', t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'));
  }
}

/**
 * Submit handler for name_custom_formats_form().
 */
function name_custom_formats_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  if (empty($values['ncfid'])) {
    drupal_write_record('name_custom_format', $values);
    $message = 'Custom format %name has been created.';
  }
  else {
    drupal_write_record('name_custom_format', $values, 'ncfid');
    $message = 'Custom format %name has been updated.';
  }
  drupal_set_message(t($message, array('%name' => $values['name'])));

  $form_state['redirect'] = 'admin/config/regional/name';
}

/**
 * Page to edit a custom format.
 */
function name_custom_format_edit($ncfid) {
  if ((isset($_POST['op']) && $_POST['op'] == t('Delete')) || isset($_POST['confirm'])) {
    return drupal_get_form('name_custom_format_delete_form', $ncfid);
  }

  if ($name = db_query("SELECT ncf.* FROM {name_custom_format} ncf WHERE ncfid = :ncfid", array(':ncfid' => $ncfid))->fetchAssoc()) {
    return drupal_get_form('name_custom_formats_form', $name);
  }
  drupal_set_message(t('The custom format could not be found.'), 'error');
  drupal_goto('admin/config/regional/name');
}

/**
 * Custom name format deletion form.
 */
function name_custom_format_delete_form($form, $form_state, $ncfid) {
  $name = db_query("SELECT ncf.* FROM {name_custom_format} ncf WHERE ncfid = :ncfid", array(':ncfid' => $ncfid))->fetchAssoc();

  if (!$name) {
    drupal_set_message(t('The custom format could not be found.'), 'error');
    drupal_goto('admin/config/regional/name');
  }
  $form = array();
  $form['ncfid'] = array(
    '#type' => 'value',
    '#value' => $name['ncfid'],
  );
  $form['#name'] = $name;
  return confirm_form(
    $form,
    t('Are you sure you want to delete the custom format %name ("%format")?',
      array('%name' => $name['name'], '%format' => $name['format'])
    ),
    'admin/config/regional/name',
    t('This action cannot be undone.'),
    t('Delete'),   t('Cancel')
  );
}

/**
 * Submit handler for name_custom_format_delete_form().
 */
function name_custom_format_delete_form_submit($form, &$form_state) {
  db_delete('name_custom_format')
      ->condition('ncfid', $form_state['values']['ncfid'])
      ->execute();
  drupal_set_message(t('The custom name format %name was deleted.',
      array('%name' => $form['#name']['name'])));
  $form_state['redirect'] = 'admin/config/regional/name';
}

/**
 * Themes the instance settings of the name component into a nice table, rather
 * than a long list of individual elements.
 */
function _name_field_instance_settings_pre_render($form) {

  $form['styled_settings'] = array(
    '#prefix' => '<table>',
    '#suffix' => '</table>',
    '#weight' => 1,
    'thead' => array(
      '#prefix' => '<thead><tr><th>' . t('Field') . '</th>',
      '#suffix' => '</tr></thead>',
      '#weight' => 0,
    ),
    'tbody' => array(
      '#prefix' => '<tbody>',
      '#suffix' => '</tbody>',
      '#weight' => 1,
      'title_display' => array(
        '#prefix' => '<tr><td><strong>' . t('Title display') . '<sup>1</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 1,
      ),
      'field_type' => array(
        '#prefix' => '<tr><td><strong>' . t('Field type') . '<sup>2</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 2,
      ),
      'size' => array(
        '#prefix' => '<tr><td><strong>' . t('HTML size') . '<sup>3</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 3,
      ),
      'inline_css' => array(
        '#prefix' => '<tr><td><strong>' . t('Inline styles') . '<sup>4</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 4,
      ),
    ),
    'tfoot' => array(
      '#markup' => '<tfoot><tr><td colspan="7"><ol>'
        . '<li>' .  t('The title display controls how the label of the name component is displayed in the form. "%above" is the standard title; "%below" is the standard description; "%hidden" removes the label.',
                      array('%above' => t('above'), '%below' => t('below'), '%hidden' => t('hidden'))) . '</li>'
        . '<li>' .  t('The Field type controls how the field is rendered. Autocomplete is a text field with autocomplete, and the behaviour of this is controlled by the field settings.') . '</li>'
        . '<li>' .  t('The HTML size property tells the browser what the width of the field should be when it is rendered. This gets overriden by the themes CSS properties. This must be between 1 and 255.') . '</li>'
        . '<li>' .  t('Additional inline styles for the input element. For example, "width: 45px; background-color: #f3f3f3"') . '</li>'
        . '</ol></td></tr></tfoot>',
      '#weight' => 2,
    ),
    'extra_fields' => array(
      '#weight' => 3,
    ),
  );

  $i = 0;
  foreach (_name_translations() as $key => $title) {
    // Adds the table header for the particullar field.
    $form['styled_settings']['thead'][$key]['#markup'] = '<th>' . $title . '</th>';
    $form['styled_settings']['thead'][$key]['#weight'] = ++$i;

    // Strip the title & description.
    unset($form['instance_size'][$key]['#description']);
    unset($form['instance_size'][$key]['#title']);
    $form['instance_size'][$key]['#size'] = 5;

    unset($form['instance_title_display'][$key]['#description']);
    unset($form['instance_title_display'][$key]['#title']);

    unset($form['instance_field_type'][$key]['#description']);
    unset($form['instance_field_type'][$key]['#title']);

    unset($form['instance_inline_css'][$key]['#description']);
    unset($form['instance_inline_css'][$key]['#title']);

    // Moves the size element into the table.
    $form['styled_settings']['tbody']['size'][$key] = $form['instance_size'][$key];
    $form['styled_settings']['tbody']['size'][$key]['#prefix'] = '<td>';
    $form['styled_settings']['tbody']['size'][$key]['#suffix'] = '</td>';
    $form['styled_settings']['tbody']['size'][$key]['#weight'] = $i;

    $form['styled_settings']['tbody']['title_display'][$key] = $form['instance_title_display'][$key];
    $form['styled_settings']['tbody']['title_display'][$key]['#prefix'] = '<td>';
    $form['styled_settings']['tbody']['title_display'][$key]['#suffix'] = '</td>';
    $form['styled_settings']['tbody']['title_display'][$key]['#weight'] = $i;

    $form['styled_settings']['tbody']['field_type'][$key] = $form['instance_field_type'][$key];
    $form['styled_settings']['tbody']['field_type'][$key]['#prefix'] = '<td>';
    $form['styled_settings']['tbody']['field_type'][$key]['#suffix'] = '</td>';
    $form['styled_settings']['tbody']['field_type'][$key]['#weight'] = $i;

    $form['styled_settings']['tbody']['inline_css'][$key] = $form['instance_inline_css'][$key];
    $form['styled_settings']['tbody']['inline_css'][$key]['#prefix'] = '<td>';
    $form['styled_settings']['tbody']['inline_css'][$key]['#suffix'] = '</td>';
    $form['styled_settings']['tbody']['inline_css'][$key]['#weight'] = $i;

    // Clean up the leftovers.
    unset($form['instance_size'][$key]);
    $form['instance_size']['#access'] = FALSE;

    unset($form['instance_title_display'][$key]);
    $form['instance_title_display']['#access'] = FALSE;

    unset($form['instance_field_type'][$key]);
    $form['instance_field_type']['#access'] = FALSE;

    unset($form['instance_inline_css'][$key]);
    $form['instance_inline_css']['#access'] = FALSE;

  }

  return $form;
}

/**
 * Themes the global field settings of the name component into a nice table,
 * rather than a long list of individual elements.
 */
function _name_field_settings_pre_render($form) {

  // $warning = t('<strong>Warning! Changing this setting after data has been created could result in the loss of data!</strong>');

  $extra_max_info = '<div>' . t('This can not be less than the longest value in the database. The minimum values are:') . '</div>';
  $extra_max_items = array();
  foreach (_name_translations() as $key => $title) {
    $extra_max_items[] = t('!title is !size', array('!title' => $title,
        '!size' => $form['max_length'][$key]['#min_size']));
  }
  $extra_max_info .= theme('item_list', array('items' => $extra_max_items));
  $form['field_properties'] = array(
    '#prefix' => '<table>',
    '#suffix' => '</table>',
    '#weight' => 1,
    'thead' => array(
      '#prefix' => '<thead><tr><th>' . t('Field') . '</th>',
      '#suffix' => '</tr></thead>',
      '#weight' => 0,
    ),
    'tbody' => array(
      '#prefix' => '<tbody>',
      '#suffix' => '</tbody>',
      '#weight' => 1,
      'components' => array(
        '#prefix' => '<tr><td><strong>' . t('Components') . '<sup>1</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 1,
      ),
      'minimum_components' => array(
        '#prefix' => '<tr><td><strong>' . t('Minimum components') . '<sup>2</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 2,
      ),
      'max_length' => array(
        '#prefix' => '<tr><td><strong>' . t('Maximum length') . '<sup>3</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 3,
      ),
      'labels' => array(
        '#prefix' => '<tr><td><strong>' . t('Labels') . '<sup>4</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 4,
      ),
      'sort_options' => array(
        '#prefix' => '<tr><td><strong>' . t('Sort options') . '<sup>5</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 5,
      ),
      'autocomplete_source' => array(
        '#prefix' => '<tr><td><strong>' . t('Autocomplete sources') . '<sup>6</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 6,
      ),
      'autocomplete_separator' => array(
        '#prefix' => '<tr><td><strong>' . t('Autocomplete separator') . '<sup>7</sup></strong></td>',
        '#suffix' => '</tr>',
        '#weight' => 7,
      ),
    ),
    'tfoot' => array(
      '#markup' => '<tfoot><tr><td colspan="7"><ol>'
        . '<li>' . t('Only selected components will be activated on this field. All non-selected components / component settings will be ignored.') . '</li>'
        . '<li>' . t('The minimal set of components required before the field is considered completed enough to save.') . '</li>'
        . '<li>' . t('The maximum length of the field in characters. This must be between 1 and 255.')
        . $extra_max_info . '</li>'
        . '<li>' . t('The labels are used to distinguish the fields.') . '</li>'
        . '<li>' . t('This enables sorting on the options after the vocabulary terms are added and duplicate values are removed.') . '</li>'
        . '<li>' . t('At least one value must be selected before you can enable the autocomplete option on the input textfields.')
        . '<li>' . t('This allows you to override the default handling that the autocomplete uses to handle separations between components. If empty, this defaults to a single space.')
        . '</ol></td></tr></tfoot>',
      '#weight' => 2,
    ),
  );

  $i = 0;
  foreach (_name_translations() as $key => $title) {
    // Adds the table header for the particullar field.
    $form['field_properties']['thead'][$key]['#markup'] = '<th>' . $title . '</th>';
    $form['field_properties']['thead'][$key]['#weight'] = ++$i;

    // Strip the title & description.
    unset($form['components'][$key]['#description']);
    unset($form['components'][$key]['#title']);

    unset($form['minimum_components'][$key]['#description']);
    unset($form['minimum_components'][$key]['#title']);

    unset($form['max_length'][$key]['#description']);
    unset($form['max_length'][$key]['#title']);
    $form['max_length'][$key]['#size'] = 10;

    unset($form['labels'][$key]['#description']);
    unset($form['labels'][$key]['#title']);
    $form['labels'][$key]['#size'] = 10;

    if (isset($form['sort_options'][$key])) {
      unset($form['sort_options'][$key]['#description']);
      unset($form['sort_options'][$key]['#title']);
    }

    unset($form['autocomplete_source'][$key]['#description']);
    unset($form['autocomplete_source'][$key]['#title']);

    unset($form['autocomplete_separator'][$key]['#description']);
    unset($form['autocomplete_separator'][$key]['#title']);

    // Moves the elements into the table.
    $form['field_properties']['tbody']['components'][$key] = $form['components'][$key];
    $form['field_properties']['tbody']['components'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['components'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['components'][$key]['#weight'] = $i;

    $form['field_properties']['tbody']['minimum_components'][$key] = $form['minimum_components'][$key];
    $form['field_properties']['tbody']['minimum_components'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['minimum_components'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['minimum_components'][$key]['#weight'] = $i;

    $form['field_properties']['tbody']['max_length'][$key] = $form['max_length'][$key];
    $form['field_properties']['tbody']['max_length'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['max_length'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['max_length'][$key]['#weight'] = $i;

    $form['field_properties']['tbody']['labels'][$key] = $form['labels'][$key];
    $form['field_properties']['tbody']['labels'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['labels'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['labels'][$key]['#weight'] = $i;

    if (isset($form['sort_options'][$key])) {
      $form['field_properties']['tbody']['sort_options'][$key] = $form['sort_options'][$key];
    }
    else {
      $form['field_properties']['tbody']['sort_options'][$key] = array('#markup' => '&nbsp;');
    }

    $form['field_properties']['tbody']['sort_options'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['sort_options'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['sort_options'][$key]['#weight'] = $i;

    $form['field_properties']['tbody']['autocomplete_source'][$key] = $form['autocomplete_source'][$key];
    $form['field_properties']['tbody']['autocomplete_source'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['autocomplete_source'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['autocomplete_source'][$key]['#weight'] = $i;

    $form['field_properties']['tbody']['autocomplete_separator'][$key] = $form['autocomplete_separator'][$key];
    $form['field_properties']['tbody']['autocomplete_separator'][$key]['#prefix'] = '<td>';
    $form['field_properties']['tbody']['autocomplete_separator'][$key]['#suffix'] = '</td>';
    $form['field_properties']['tbody']['autocomplete_separator'][$key]['#weight'] = $i;

    // Clean up the leftovers.
    unset($form['components'][$key]);
    $form['components']['#access'] = FALSE;

    unset($form['minimum_components'][$key]);
    $form['minimum_components']['#access'] = FALSE;

    unset($form['max_length'][$key]);
    $form['max_length']['#access'] = FALSE;

    unset($form['labels'][$key]);
    $form['labels']['#access'] = FALSE;

    if (isset($form['sort_options'][$key])) {
      unset($form['sort_options'][$key]);
      $form['sort_options']['#access'] = FALSE;
    }

    unset($form['autocomplete_source'][$key]);
    $form['autocomplete_source']['#access'] = FALSE;

    unset($form['autocomplete_separator'][$key]);
    $form['autocomplete_separator']['#access'] = FALSE;
  }

  // Move the additional options under the table.
  $form['extra_fields'] = array(
    '#weight' => 2,
  );
  $form['title_options']['#weight'] = 0;
  $form['generational_options']['#weight'] = 1;
  $form['extra_fields']['title_options'] = $form['title_options'];
  $form['extra_fields']['generational_options'] = $form['generational_options'];
  unset($form['title_options']);
  unset($form['generational_options']);

  return $form;
}
