<?php

/**
 * @file
 * Pages and forms for ODSM.
 */

$module_path = drupal_get_path('module', 'open_data_schema_map');
require_once $module_path . '/open_data_schema_map.output.inc';
require_once $module_path . '/open_data_schema_map.file_cache.inc';

/**
 * Callback for ODSM overiew page.
 */
function open_data_schema_map_page_overview() {
  $form = array();
  $apis = open_data_schema_map_api_table();
  $list = array();
  foreach ($apis as $num => $data) {
    $data = (array) $data;
    $data['endpoint'] = isset($data['endpoint']) ? l($data['endpoint'], $data['endpoint']) : '';
    // If there is a callback this has no UI.
    $data['edit'] = isset($data['callback']) ? '' : $data['edit'];
    $data['delete'] = isset($data['callback']) ? '' : l(t('delete'), 'admin/config/services/odsm/delete/' . $data['machine_name']);
    unset($data['machine_name']);
    unset($data['callback']);

    foreach (module_implements('odsm_page_overview_alter') as $module) {
      $function = $module . '_odsm_page_overview_alter';
      $function($data);
    }
    // TODO: report features status.
    $data['enabled'] = $data['enabled'] ? t('Enabled') : t('Disabled');
    $list[] = $data;
  }
  $header = array();
  $header[] = t('Title');
  $header[] = t('Status');
  $header[] = t('Schema');
  $header[] = t('Entity');
  $header[] = t('Bundle');
  $header[] = t('Output Format');
  $header[] = t('Endpoint');
  $header[] = t('Edit');
  $header[] = t('Cache');
  $header[] = t('Delete');
  $rows = $list;
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('class' => array('search-api-overview')),
    '#empty' => t('There are no search servers or indexes defined yet.'),
  );
}

/**
 * Callback for delete page.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 * @param object $api
 *   API Object
 *
 * @return array
 *   Form Array
 */
function open_data_schema_map_delete(array $form, array &$form_state, $api = NULL) {
  if (isset($api->callback)) {
    $form['in_code'] = array(
      '#type' => 'item',
      '#markup' => t('This endpoint has been created in code. It cannot be edited through the user interface.'),
    );
  }
  else {
    $form = confirm_form($form, t('Delete this API?'), 'admin/config/services/odsm', '', t('Delete'), t('Cancel'), 'administer open data schema mapper.');
    $form['#api'] = $api;
  }
  return $form;
}

/**
 * Submit function for deleting an API.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 */
function open_data_schema_map_delete_submit(array $form, array &$form_state) {
  $api = $form['#api'];
  $id = $api->id;
  db_delete('open_data_schema_map')
    ->condition('id', $id)
    ->execute();

  // Rebuild menu so the API is not available anymore.
  menu_rebuild();

  drupal_set_message(t('API %name has been deleted', array('%name' => $api->name)));
  drupal_goto('admin/config/services/odsm');
}

/**
 * Callback for primary menu page.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 * @param object $api
 *   API Object
 *
 * @return array
 *   Form Array
 */
function open_data_schema_map_manage(array $form, array &$form_state, $api = NULL) {
  $entity_list = array();
  if (isset($api->callback)) {
    $form['in_code'] = array(
      '#type' => 'item',
      '#markup' => t('This endpoint has been created in code. It cannot be edited through the user interface.'),
    );
    return $form;
  }
  $schemas = open_data_schema_map_register();
  if (!$schemas) {
    $form['empty'] = array(
      '#type' => 'item',
      '#title' => 'No Available Schemas',
      '#markup' => t('You must enable an Open Data schema to add an endpoint.'),
    );
    return $form;
  }
  $list = array();
  foreach ($schemas as $name => $data) {
    $list[$name] = $data['title'];
  }
  $markup = t('No APIs created.');
  if ($list) {
    $markup = t('Choose from the available APIs:') . theme('item_list', $list);
  }

  $entity_types = entity_get_info();
  foreach ($entity_types as $entity_name => $entity_info) {
    $entity_list[$entity_name] = $entity_info['label'];
  }
  $selected_entity = '';
  $selected_schema = '';
  $selected_outputformat = '';
  if (isset($form_state['triggering_element']['#name']) && $name = $form_state['triggering_element']['#name']) {
    $selected_entity = $form_state['triggering_element']['#value'];
  }
  elseif ($api) {
    $selected_entity = $api->type;
  }
  $output_formats = open_data_schema_map_form_output_formats();
  if (isset($form_state['triggering_element']['#outputformat']) && $outputformat = $form_state['triggering_element']['#outputformat']) {
    $selected_outputformat = $form_state['triggering_element']['#value'];
  }
  else {
    $selected_outputformat = $output_formats['json'];
  }
  $form['name'] = array(
    '#title' => 'Title',
    '#type' => 'textfield',
    '#description' => t('Add an administrative title.'),
    '#required' => TRUE,
    '#weight' => 1,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 50,
    '#machine_name' => array(
      'exists' => 'open_data_schema_map_api_exist',
    ),
    '#weight' => 1,
  );
  $form['type'] = array(
    '#title' => 'Entity Type',
    '#type' => 'select',
    '#description' => t('Entity type.'),
    '#options' => $entity_list,
    '#required' => TRUE,
    '#weight' => 1,
  );
  if ($api) {
    $form['type']['#disabled'] = TRUE;
  }
  else {
    $form['type']['#ajax'] = array(
      'callback' => 'open_data_schema_map_bundle_ajax_callback',
      'wrapper' => 'open-data-schema-map-bundle-options',
    );
  }
  $bundle_list = array();
  if ($selected_entity) {
    foreach ($entity_types[$selected_entity]['bundles'] as $bundle_name => $bundle_data) {
      $bundle_list[$bundle_name] = $bundle_data['label'];
    }
    $form['bundle'] = array(
      '#title' => 'Bundle',
      '#type' => 'select',
      '#description' => t('Entity type bundle.'),
      '#options' => $bundle_list,
      '#required' => TRUE,
      '#weight' => 1,
    );
    if ($api) {
      $form['bundle']['#disabled'] = TRUE;
    }
  }
  else {
    $form['bundle'] = array(
      '#title' => 'Bundle',
      '#type' => 'item',
      '#markup' => t('Select an entity type'),
      '#required' => TRUE,
      '#weight' => 1,
    );
  }
  $form['bundle']['#prefix'] = '<div id="open-data-schema-map-bundle-options">';
  $form['bundle']['#suffix'] = '</div>';
  $form['endpoint'] = array(
    '#title' => 'Endpoint',
    '#type' => 'textfield',
    '#description' => t('Add and enpoint ie "api/3/action/package_show".'),
    '#required' => TRUE,
    '#weight' => 1,
  );
  $form['outputformat'] = array(
    '#title' => 'Output Format',
    '#type' => 'select',
    '#description' => t('Select from the available output formats.'),
    '#options' => $output_formats,
    '#required' => TRUE,
    '#weight' => 2,
  );

  $form['enabled'] = array(
    '#title' => 'Enabled',
    '#type' => 'checkbox',
    '#description' => t('Click to toggle endpoint.'),
    '#weight' => 1,
  );
  $form['api_schema'] = array(
    '#title' => 'Schema',
    '#type' => 'select',
    '#description' => t('Select from the available schemas.'),
    '#options' => $list,
    '#required' => TRUE,
    '#weight' => 5,
  );
  // Only add Schema if we've already saved the initial config.
  if ($api) {
    $selected_schema = $api->api_schema;
    $schema = open_data_schema_map_schema_load($selected_schema);

    $options_callback = open_data_schema_map_schema_options_callback($schema['$schema']);
    // Arguments!
    $form['arg_id_options'] = array(
      '#type' => 'hidden',
      '#value' => array('' => t('-- none --')) + open_data_schema_map_special_arguments() + $options_callback($schema),
    );
    $form['arguments'] = array(
      '#title' => 'Arguments',
      '#type' => 'fieldset',
      '#description' => t('Add an argument to the API. Must be one of the fields from the bundle.'),
      '#collapsible' => TRUE,
      '#weight' => 5,
    );
    $form['args']['#weight'] = 5;
    $form['args']['submit'] = array(
      '#type' => 'button',
      '#value' => t('Add another argument'),
      '#ajax' => array(
        'callback' => 'open_data_schema_mapper_args_ajax_callback',
        'wrapper' => 'edit-arguments',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
    $args = $api->arguments;
    $num_args = count($args) ? count($args) : 1;
    if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#value'] == 'Add another argument') {
      $num_args++;
    }
    $form['arguments'] = $form['arguments'] + open_data_schema_map_args_form($num_args, $form['arg_id_options']['#value'], $args, $api);

    $form['api_schema']['#disabled'] = TRUE;
    $title = isset($schema['title']) ? $schema['title'] : t('Mapping');
    $form['mapping'] = array(
      '#title' => $title,
      '#description' => $schema['description'] ? $schema['description'] : t('Fields for selected schema.'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#required' => TRUE,
      '#weight' => 10,
    );
    if (module_exists('token')) {
      $form['mapping']['token_help'] = array(
        '#title' => t('Replacement patterns'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#description' => t('Prefer raw-text replacements for text to avoid problems with HTML entities!'),
      );
      $form['mapping']['token_help']['tokens'] = array(
        '#theme' => 'token_tree',
        '#token_types' => array('node'),
      );
      $form['mapping']['token_help']['help'] = array(
        '#type' => 'item',
        '#markup' => t('Insert tokens or static values into the text fields below. Use "|" or "&" to indicate "either" or "and" between
        token values. For example, "[node:uuid] | [node:nid]" would print the nid if it is available and if not the nid.'),
      );
    }
    $form['mapping'] = $form['mapping'] + open_data_schema_map_schema_map_form($schema, $api);

    if (in_array($api->machine_name, _open_data_schema_map_get_available_filters('machine_name'))) {
      $form['filter_enabled'] = array(
        '#type' => 'checkbox',
        '#title' => t('Apply Data Federation Filters'),
        '#description' => t('Check to apply filters configured in the') . ' ' . l(t('Data Federation Filters'), '/admin/config/services/odsm/settings') . ' ' . t('settings'),
        '#weight' => 6,
      );
    }

    // Load defaults.
    $form['name']['#default_value'] = $api->name;
    $form['machine_name']['#default_value'] = $api->machine_name;
    $form['type']['#default_value'] = $api->type;
    $form['bundle']['#default_value'] = $api->bundle;
    $form['endpoint']['#default_value'] = $api->endpoint;
    $form['outputformat']['#default_value'] = $api->outputformat;
    $form['enabled']['#default_value'] = $api->enabled;
    $form['api_schema']['#default_value'] = $api->api_schema;
    $form['arguments']['#default_value'] = $api->arguments;
    $form['filter_enabled']['#default_value'] = $api->filter_enabled;

    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $api->id,
    );
  }
  if ($api) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update API'),
      '#weight' => 10,
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create API'),
      '#weight' => 10,
    );
  }
  $form['#tree'] = TRUE;

  foreach (module_implements('odsm_form_alter') as $module) {
    $function = $module . '_odsm_form_alter';
    $function($form, $api);
  }
  return $form;
}

/**
 * Creates form for schema fields.
 *
 * @param array $schema
 *   Schema array
 * @param object $api
 *   API Object
 *
 * @return array
 *   Form array
 */
function open_data_schema_map_schema_map_form($schema, $api) {
  $function = open_data_schema_map_schema_types_callback($schema['$schema']);
  $defaults = '';
  if (isset($api->mapping)) {
    $defaults = $api->mapping;
  }
  $form = $function($schema, $api, $defaults);
  return $form;
}

/**
 * Form AJAX handler.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 *
 * @return array
 *   Bundle array
 */
function open_data_schema_map_bundle_ajax_callback(array $form, array &$form_state) {
  return $form['bundle'];
}

/**
 * Form AJAX handler.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 *
 * @return array
 *   Arguments array
 */
function open_data_schema_mapper_args_ajax_callback(array $form, array &$form_state) {
  return $form['arguments'];
}

/**
 * Submit function for api map.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 */
function open_data_schema_map_manage_submit($form, $form_state) {
  $db_schema = drupal_get_schema('open_data_schema_map');
  $values = $form_state['values'];
  if (isset($values['arguments'])) {
    // Remove empty arguments.
    foreach ($values['arguments'] as $num => $arg) {
      if (!$arg['field']) {
        unset($values['arguments'][$num]);
      }
    }
  }
  $record = array();
  // Prepare values for insertion.
  foreach ($db_schema['fields'] as $name => $data) {
    if (isset($values[$name])) {
      if ($name == 'mapping') {
        open_data_schema_map_add_type($values[$name], $form['mapping']);
      }
      $record[$name] = $values[$name];
    }
  }
  // Decide if we are updating or inserting.
  if (isset($record['id']) && $record['id']) {
    $update = array('id');
    $message = t('API %name updated.', array('%name' => $record['name']));
  }
  else {
    $update = array();
    $message = t('Created API %name. You may now add arguments and field mappings.', array('%name' => $record['name']));
  }
  drupal_static_reset('open_data_schema_map_api_load_all');
  if (drupal_write_record('open_data_schema_map', $record, $update)) {
    // Rebuild menus in order to update/add the API.
    menu_rebuild();
    drupal_set_message($message);
  }
  drupal_goto('admin/config/services/odsm/edit/' . $values['machine_name']);
}

/**
 * Adds field type to field definition.
 *
 * @param array $values
 *   Values array
 * @param array $mapping
 *   Mapping array
 */
function open_data_schema_map_add_type(&$values, $mapping) {
  foreach ($values as $api_field => $token) {
    if (!is_array($values[$api_field])) {
      $type = isset($mapping[$api_field]['#field_type']) ? $mapping[$api_field]['#field_type'] : '';
      $values[$api_field] = array('value' => $token, 'type' => $type);
    }
    else {
      open_data_schema_map_add_type($values[$api_field], $mapping[$api_field]);
    }
  }
}

/**
 * Main endpoint callback.
 *
 * @param object $api
 *   API object
 */
function open_data_schema_map_endpoint($api) {
  // Start by setting the content type header based on the API settings.
  if ($output_format = open_data_schema_map_output_format_load($api->outputformat)) {
    foreach ($output_format['headers'] as $key => $value) {
      drupal_add_http_header($key, $value);
    }
  }

  // Check if a file cache exists, and simply return this if it does.
  if (open_data_schema_map_file_cache_exits($api)) {
    $filename = open_data_schema_map_file_cache_name_helper($api);
    drupal_add_http_header('Expires', gmdate('D, d M Y H:i:s \G\M\T', time() + 3600));
    ob_clean();
    flush();
    readfile($filename);
  }

  // If not, render the results and output with the appropriate formatter.
  else {
    $render = open_data_schema_map_render_api($api);
    $headers = $render['headers'];
    $result = $render['result'];

    if ($output_format = open_data_schema_map_output_format_load($api->outputformat)) {
      $function = $output_format['callback'];
      $response = $function($api, $result);
      foreach ($headers as $key => $value) {
        drupal_add_http_header($key, $value);
      }
      echo $response;
    }
    else {
      drupal_set_message("Invalid output format", "error");
    }
  }
}


/**
 * Callback for validation page.
 */
function open_data_schema_map_validation_page($validator_class, $api_endpoint) {
  $render = array();

  $results = _open_data_schema_map_process_validate($validator_class);
  $cache_date = isset($results['cache_date']) ? $results['cache_date'] : '';
  $cache_form = drupal_get_form('open_data_schema_map_validate_cache_clear', $cache_date, $validator_class, $api_endpoint);

  $total = array(
    array(
      $results['count'],
      count($results['error_datasets']),
      $results['total_errors'],
    ),
  );

  $total_table = theme('table', array(
    'header' => array(
      t('Total Datasets'),
      t('Datasets with Errors'),
      t('Errors'),
    ), 'rows' => $total,
  ));

  $render['total'] = array(
    '#type' => 'markup',
    '#markup' => $total_table,
  );

  if ($results['errors']) {
    $headers = array('id', 'property', 'error', 'link');
    $table = theme('table', array('header' => $headers, 'rows' => $results['errors']));
    $render['fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => 'errors',
      '#attributes' => array('class' => array('collapsible', 'collapsed')),
      'content' => array(
        '#markup' => $table,
      ),
      '#attached' => array('js' => array('misc/collapse.js', 'misc/form.js')),
    );
  }

  return drupal_render($cache_form) . drupal_render($render);
}

/**
 * Returns the render array for the form.
 */
function open_data_schema_map_validate_cache_clear($form, $form_state, $created, $validator_class, $api_endpoint) {
  if (!empty($created)) {
    $form['cache'] = array(
      '#type' => 'markup',
      '#markup' => t('Result cached %date.', array('%date' => date('F j, Y, g:i a', $created))),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Clear cache'),
    );
  }

  $form['validator_class'] = array(
    '#type' => 'hidden',
    '#value' => $validator_class,
  );
  $form['api_endpoint'] = array(
    '#type' => 'hidden',
    '#value' => $api_endpoint,
  );
  return $form;
}

/**
 * Add a submit handler/function to the form.
 */
function open_data_schema_map_validate_cache_clear_submit($form, &$form_state) {
  if ($form_state['triggering_element']['#id'] == 'edit-submit') {
    $validator_class = $form_state['input']['validator_class'];
    $cache_key = strtolower($validator_class) . '_validate_result';
    cache_clear_all($cache_key, 'cache');

    drupal_set_message(t('The validator cache has been cleared.'));
  }
}

