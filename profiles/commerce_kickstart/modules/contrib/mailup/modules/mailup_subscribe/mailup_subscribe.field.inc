<?php
/**
 * @file
 * Field related functionality for mailup_subscribe module
 */

/**
 * Implements hook_field_info().
 */
function mailup_subscribe_field_info() {

  return array(
    'mailup_list_reference' => array(
      'label' => t('MailUp List Reference'),
      'description' => t('Reference a MailUp List and its groups.'),
      'settings' => array(),
      'instance_settings' => array(),
      'default_widget' => 'mailup_list_reference',
      'default_formatter' => 'mailup_list_reference_default',
      // 'no_ui' => FALSE,
      'property_type' => 'mailup_list_reference',
      'property_callbacks' => array('mailup_list_reference_field_property_info_callback'),
    ),

    'mailup_subscription' => array(
      'label' => t('MailUp Subscription'),
      'description' => t('Allow updating MailUp Subscriptions for multiple lists'),
      'settings' => array(),
      'instance_settings' => array(),
      'default_widget' => 'mailup_subscription',
      'default_formatter' => 'mailup_subscription_formatter_default',
      'no_ui' => FALSE,
      // 'property_type' => 'mailup_subscription',
      // 'property_callbacks' => array('mailup_subscription_field_property_info_callback'),
    )
  );
}

/**
 * Implements hook_field_extra_fields().
 */
function mailup_subscribe_field_extra_fields() {
  $extra['mailup_subscription']['mailup_subscription']['display'] = array(
    'status' => array(
      'label' => t('Subscription Status'),
      'description' => t('The subscription status for the user on this target'),
      'weight' => 50,
    ),
  );

  return $extra;
}


function mailup_subscription_field_property_info_callback() {}


/**
 * Attempts to directly activate a field that was disabled due to its module
 * being disabled.
 *
 * The normal API function for updating fields, field_update_field(), will not
 * work on disabled fields. As a workaround, this function directly updates the
 * database, but it is up to the caller to clear the cache.
 *
 * @param $field_name
 *   The name of the field to activate.
 *
 * @return
 *   Boolean indicating whether or not the field was activated.
 */
function mailup_subscribe_activate_field($field_name) {
  // Set it to active via a query because field_update_field() does
  // not work on inactive fields.
  $updated = db_update('field_config')
    ->fields(array('active' => 1))
    ->condition('field_name', $field_name, '=')
    ->condition('deleted', 0, '=')
    ->execute();

  return !empty($updated) ? TRUE : FALSE;
}

/**
 * Ensures the address field is present on the specified customer profile bundle.
 */
function mailup_subscribe_configure_fields() {

  $field_bases = array();

  $field_bases['mailup_list_reference'] = array(
    'field_name' => 'mailup_list_reference',
    'type' => 'mailup_list_reference',
    'cardinality' => 1,
    'entity_types' => array('mailup_subscription'),
    'translatable' => TRUE,
    'locked' => TRUE,
    'indexes' => array(
      'mailup_list' => array(
        0 => 'target_id',
      ),
    ),
  );  

  $field_bases['mailup_subscriptions'] = array(
    'field_name' => 'mailup_subscriptions',
    'type' => 'mailup_subscription',
    'cardinality' => -1,
    'entity_types' => array('user'),
    'indexes' => array(),
    'module' => 'mailup_subscribe',
    'settings' => array(
      'entity_translation_sync' => FALSE,
    ),
    'translatable' => 0,
  );

  $field_instances = array();

  $field_instances['mailup_subscription-mailup_list_reference'] = array(
    'bundle' => 'mailup_subscription',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => NULL,
        'settings' => array(),
        'type' => 'mailup_list_reference_default',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'mailup_subscription',
    'field_name' => 'mailup_list_reference',
    'label' => 'List Reference',
    'required' => 0,
    'settings' => array(
      'entity_translation_sync' => FALSE,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'mailup_subscribe',
      'settings' => array(),
      'type' => 'mailup_list',
      'weight' => 3,
    ),
  );

  $field_instances['user-mailup_subscriptions'] = array(
    'bundle' => 'user',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => NULL,
        'settings' => array(),
        'type' => 'mailup_subscription_formatter_default',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'user',
    'field_name' => 'mailup_subscriptions',
    'label' => 'Subscriptions',
    'required' => 0,
    'settings' => array(
      'entity_translation_sync' => FALSE,
      'limit_lists' => array(),
      'user_register_form' => 0,
      'view_mode' => 'full',
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'mailup_subscribe',
      'settings' => array(),
      'type' => 'mailup_subscription',
      'weight' => 8,
    ),
  );

  // Look for or add the MailUp list field for MailUp Subscription entities.
  foreach ($field_bases as $field_base) {
    mailup_subscribe_activate_field($field_base['field_name']);
  }
  field_cache_clear();


  // The list reference field
  $list_reference_field = field_info_field('mailup_list_reference');
  $list_reference_instance = field_info_instance('mailup_list', 'mailup_list_reference', 'mailup_list');
  
  if (empty($list_reference_field)) {
    $list_reference_field = field_create_field($field_bases['mailup_list_reference']);
  }
  if (empty($list_reference_instance)) {
    $list_reference_instance = field_create_instance($field_instances['mailup_subscription-mailup_list_reference']);
  }

  // The Subscriptions field
  $subscriptions_field = field_info_field('mailup_subscriptions');
  $subscriptions_instance = field_info_instance('user', 'mailup_subscriptions', 'user');

  if (empty($subscriptions_field)) {
    $subscriptions_field = field_create_field($field_bases['mailup_subscriptions']);
  }
  if (empty($subscriptions_instance)) {
    $subscriptions_instance = field_create_instance($field_instances['user-mailup_subscriptions']);
  }
}


/*
  List entity
    - remote guid
    - remote id
    - groups

  Subscription entity
    - list reference (with groups)

  Subsciption field
    - subscription reference
 */

/**
 * Property callback for mailup_list field.
 */
function mailup_list_reference_field_property_info_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $name = $field['field_name'];
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];

  $property['type'] = 'mailup_subscribe_reference';
  $property['getter callback'] = 'entity_metadata_field_verbatim_get';
  $property['setter callback'] = 'entity_metadata_field_verbatim_set';
  $property['property info'] = mailup_list_data_property_info();

  unset($property['query callback']);
}

/**
 * Defines info for the properties of the mailup_list field data structure.
 */
function mailup_list_data_property_info($name = NULL) {
  // Build an array of basic property information for the mailup_list field.
  $properties = array(
    'target_id' => array(
      'label' => t('MailUp list ID'),
    ),
    'name' => array(
      'label' => t('Name'),
    ),
    'description_value' => array(
      'label' => t('Description value'),
    ),
    'description_format' => array(
      'label' => t('Description format'),
    ),
  );

  // Add the default values for each of the mailup_list field properties.
  foreach ($properties as $key => &$value) {
    $value += array(
      'description' => !empty($name) ? t('!label of field %name', array('!label' => $value['label'], '%name' => $name)) : '',
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    );
  }

  return $properties;
}


/**
 * Implements hook_field_widget_info().
 */
function mailup_subscribe_field_widget_info() {
  return array(
    'mailup_list' => array(
      'label' => t('default'),
      'field types' => array('mailup_list_reference'),
      'settings' => array(),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      )
    ),

    'mailup_subscription' => array(
      'label' => t('default'),
      'field types' => array('mailup_subscription'),
      'settings' => array(),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE
      )
    )
  );
}

/**
 * Implements hook_field_formatter_info().
 */
function mailup_subscribe_field_formatter_info() {

  return array(
    'mailup_subscription_formatter_default' => array(
      'label' => t('Default (Subscription Status)'),
      'field types' => array('mailup_subscription'),
      'settings' => array(
        'view_mode' => 'full',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function mailup_subscribe_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {

  if ($display['type'] == 'mailup_subscription_formatter_default') {

    // Get current subscription state for user
    $user = $entity;

    // Get available subscription targets
    $limit_targets = empty($instance['settings']['limit_lists']) ? NULL : array_filter($instance['settings']['limit_lists']);
    
    $available_targets = mailup_subscription_load_multiple($limit_targets ? $limit_targets : FALSE);

    if (empty($available_targets)) {
      return array();
    }

    // Pass the user object into the subscription entity
    // so the 'status' field can be built
    foreach ($available_targets as $id => $mailup_subscription) {
      $available_targets[$id]->mailup_user = $entity;
    }

    // Extract target configuration for simplicity
    $renderable_subscriptions = entity_view('mailup_subscription', $available_targets);
    $renderable_subscriptions = $renderable_subscriptions['mailup_subscription'];

    $element = array();
    foreach ($available_targets as $id => $target) {      
      $element[$id] = $renderable_subscriptions[$id]['mailup_subscription'][$id];
    }

    return array(0 => $element);
  }

}

/**
 * Implements hook_field_instance_settings_form()
 */
function mailup_subscribe_field_instance_settings_form($field, $instance) {

  // Delegate form building to functions for each field type
  $call = 'mailup_subscribe_field_' . $field['type'] . '_instance_settings_form';
  if (function_exists($call)) {
    return $call($field, $instance);
  }
}


/**
 * Implements our pseudo-hook hook_field_FIELD_TYPE_instance_settings_form().
 *
 * @see mailup_subscribe_field_instance_settings_form()
 */
function mailup_subscribe_field_mailup_subscription_instance_settings_form($field, $instance) {

  $settings = $instance['settings'];

  // Build mailup list options
  $targets = mailup_subscription_load_multiple();
  foreach ($targets as $id => $target) {
    $list_options[$id] = entity_label('mailup_subscription', $target);
  }

  if (empty($list_options)) {
    $form['limit_lists'] = array(
      '#type' => 'fieldset',
      '#title' => t('Configure Target Lists')
    );
    $form['limit_lists']['info'] = array(
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('You do not have any lists configured as subscription targets. !link', array('!link' => l(t('Configure MailUp Lists'), 'admin/config/services/mailup/lists')))
    );
  }
  else {
    $form['limit_lists'] = array(
      '#type' => 'checkboxes',
      '#options' => $list_options,
      '#default_value' => !empty($settings['limit_lists']) ? $settings['limit_lists'] : array(),
      '#title' => t('Limit Subscription Options'),
      '#description' => t('Optionally limit subscription options.')
    );
  }

  // Get available view modes for 'mailup_list' entities
  $entity_info = entity_get_info('mailup_subscription');
  $view_mode_options = array();
  foreach ($entity_info['view modes'] as $view_mode => $view_mode_info) {
    $view_mode_options[$view_mode] = $view_mode_info['label'];
  }

  $form['view_mode'] = array(
    '#type' => 'select',
    '#title' => t('View mode'),
    '#default_value' => isset($settings['view_mode']) ? $settings['view_mode'] : FALSE,
    '#options' => $view_mode_options,
    '#description' => t('The view mode used for displaying each subscription option'),
    '#required' => TRUE,
  );

  return $form;
}


/**
 * Implements hook_field_prepare_view().
 *
 * Our field has no actual data in the database, so we have to push a dummy
 * value into $items, or the render system will assume we have nothing to
 * display. See https://api.drupal.org/comment/48043#comment-48043
 */
function mailup_subscribe_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {

  if ($field['type'] == 'mailup_subscription') {
    foreach ($entities as $id => $entity) {
      $items[$id][0] = array(
        'value' => 'Dummy value'
      );
    }
  }
}

/**
 * Implements hook_field_widget_form().
 */
function mailup_subscribe_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  // Pseudo-hook allows each of our fields to define widget form separately
  $call = 'mailup_subscribe_field_' . $field['type'] . '_widget_form';
  return $call($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
}

/**
 * Widget callback for mailup_subscription fields
 * Implements our pseudo-hook hook_field_FIELD_TYPE_widget_form()
 *
 * @see mailup_subscribe_field_widget_form()
 */
function mailup_subscribe_field_mailup_subscription_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $settings = $instance['settings'];

  // All subscription targets known to us
  $all_targets = mailup_subscription_load_multiple();
  $total_targets = count($all_targets);

  // Nothing to do if there are no list targets
  if (!$total_targets) {
    return array();
  }

  // Limit to targets selected on instance settings
  $limit_targets = empty($settings['limit_lists']) ? array() : array_filter($settings['limit_lists']);

  // Either take all of the targets, or the selected targets
  $form_state['subscription_targets'] = empty($limit_targets) ? $all_targets : array_intersect_key($all_targets, $limit_targets);

  // Our field is limited to 'user' entity
  $user = $element['#entity'];

  // Get existing subscription states
  $form_state['user_subscriptions'] = $user->uid ? mailup_subscribe_load_user_subscriptions($user) : array();

  foreach ($form_state['subscription_targets'] as $id => $subscription) {

    $language = $subscription->language;
    $config_field_name = 'mailup_list_reference';
    $config = $subscription->{$config_field_name}[$language][0];

    $target_list_id = $config['target_id'];

    // Extract subscription status
    $status = MAILUP_STATUS_UNKNOWN;
    if (!empty($form_state['user_subscriptions'][$target_list_id])) {
      $status = (int) $form_state['user_subscriptions'][$target_list_id]['status'];
    }

    // Extract group status
    $groups = empty($form_state['user_subscriptions'][$target_list_id]['groups']) ? array() : $form_state['user_subscriptions'][$target_list_id]['groups'];

    $element[$delta][$id] = array(
      '#type' => 'mailup_subscribe_checkbox',
      '#subscription_target' => $subscription,
      '#default_value' => array(
        'subscribe' => mailup_subscribe_status_to_boolean($status),
        'groups' => $groups
      ),
    );

    if ($status == MAILUP_STATUS_PENDING) {
      $element[$delta][$id]['#description'] = t('Pending confirmation. Please check your email inbox.');
    }
  }

  $element['#element_validate'] = array('mailup_subscribe_field_mailup_subscription_widget_form_validate');

  return $element;
}


function mailup_subscribe_field_mailup_subscription_widget_form_validate($element, &$form_state) {

  return;

  $field = $form_state['field'][$element['#field_name']][$element['#language']]['field'];
  $field_name = $field['field_name'];

  $values = &$form_state['values'][$field_name][$element['#language']];
}


/**
 * Widget callback for mailup_list_reference fields
 * Implements our pseudo-hook hook_field_FIELD_TYPE_widget_form()
 *
 * @see mailup_subscribe_field_widget_form()
 */
function mailup_subscribe_field_mailup_list_reference_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  // Don't display the widget on field edit form
  if ($form_state['build_info']['form_id'] == 'field_ui_field_edit_form') {
    return;
  }

  // Store our list entities
  if (!isset($form_state['#lists'])) {
    $form_state['#lists'] = mailup_list_load_multiple();
  }

  // Store the #parents of this element,
  // So form-level handlers can extract and easily manipulate field values
  if (!isset($form_state['#widget_parents'])) {
    $form_state['#widget_parents'] = array_merge($form['#parents'], array($field['field_name'], $element['#language']), array($delta));
  }

  // Extract submitted values from state
  // For easy setting of #default_value on elements
  if (isset($form_state['values'])) {
    $values = drupal_array_get_nested_value($form_state['values'], $form_state['#widget_parents']);
    // And merge with default values
    $item_values = array_merge($items[$delta], $values);
  }
  else {
    $item_values = $items[$delta];
  }

  // Build mailup list options
  $list_options = array('' => t('-- Select --'));
  foreach ($form_state['#lists'] as $list_entity_id => $list_entity) {
    $list_options[$list_entity_id] = entity_label('mailup_list', $list_entity);
  }

  $element['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $item_values['name'],
  );

  $element['tools'] = array(
    '#type' => 'fieldset',
    '#title' => t('MailUp Service Actions'),
    'update_groups' => array(
      '#type' => 'submit',
      '#submit' => array('mailup_subscribe_update_list_groups_submit'),
      '#limit_validation_errors' => array(),
      '#value' => t('Update list groups from mailup'),
    )
  ); 

  $element['list'] = array(
    '#type' => 'fieldset',
    '#title' => t('Target List'),
  );

  $element['list']['target_id'] = array(
    '#type' => 'select',
    '#options' => $list_options,
    '#default_value' => $item_values['target_id'],
    '#title' => t('MailUp List'),
    '#ajax' => array(
      'callback' => 'mailup_subscribe_change_list_ajax',
      'wrapper' => 'list-groups'
    ),
    '#element_validate' => array('mailup_subscribe_change_list_validate'),
  );

  // Build group optons
  $list_entity = $form_state['#lists'][ $element['list']['target_id']['#default_value'] ];
  $group_options = array();
  foreach ($list_entity->groups as $group) { 
    $group_options[$group['idGroup']] = $group['Name'];
  }

  $element['list']['target_groups'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Groups'),
    '#options' => $group_options,
    '#prefix' => '<div id="list-groups">',
    '#suffix' => '</div>',
    '#default_value' => empty($item_values['target_groups']) ? array() : $item_values['target_groups']
  );

  $element['settings']['expose_groups'] = array(
    '#type' => 'checkbox',
    '#title' => t('Expose group selection on subscription form'),
    '#prefix' => '<h4>Group Selection</h4>',
    '#default_value' => !empty($item_values['settings']['expose_groups']),
    '#description' => t('When checked, user will be presented checkboxes for each of group selected above.')
  );

  // Add validate handler to flatten element values
  $element['#element_validate'] = array('mailup_list_reference_validate');

  return $element;
}


function mailup_subscribe_update_list_groups_submit($form, &$form_state) {

  if (!empty($form_state['mailup_update_groups'])) {
    $sync_groups_batch = mailup_subscribe_sync_list_groups_batch($form_state['mailup_update_groups']);
    batch_set($sync_groups_batch); 
  }
}

/**
 * Validate handler for our widget form
 *
 * @see mailup_subscribe_field_mailup_list_reference_widget_form()
 */
function mailup_list_reference_validate($element, &$form_state) {

  $values = drupal_array_get_nested_value($form_state['values'], $form_state['#widget_parents']);

  if (!empty($form_state['triggering_element']['#array_parents'])) {

    if (end($form_state['triggering_element']['#array_parents']) == 'update_groups') {

      $list_id = $values['list']['target_id'];
      $selected_lists = array($list_id => $list_id);

      $form_state['mailup_update_groups'] = $selected_lists;
    }
  }

  // Our list id and groups are nested in a fieldset called 'list'
  // Flatten them into the main element for saving
  foreach ($values['list'] as $key => $value) {
    $values[$key] = $value;
  }
  unset($values['list']);

  // Set the updated value in form state
  form_set_value($element, $values, $form_state);
}


/**
 * Validate callback for list reference ajax select
 */
function mailup_subscribe_change_list_validate($element, &$form_state, &$form) {

  if ($element['#id'] == $form_state['triggering_element']['#id']) {
    
    // Parents of main widget form, as stored by the builder function
    $parents = $form_state['#widget_parents'];

    // Pseudo element representing 'target_groups'
    $target_groups = array(
      '#parents' => array_merge($parents, array('list', 'target_groups'))
    );

    // Unset the groups value, as we're swapping the list out
    form_set_value($target_groups, array(), $form_state);
    $form_state['rebuild'] = TRUE;
  }
}


/**
 * AJAX framework form callback for changing list groups
 * 
 * @see mailup_subscribe_field_mailup_list_reference_widget_form()
 *
 * @return array
 *   The rebuilt 'target_groups' form element
 */
function mailup_subscribe_change_list_ajax($form, &$form_state) {

  // Get the path to the containing fieldset,
  // from the select element's parents
  $array_parents = $form_state['triggering_element']['#array_parents'];
  // by removing the last part
  array_pop($array_parents);

  // Now fetch that fieldset
  $element = $form;
  foreach ($array_parents as $key) {
    $element = &$element[$key];
  }

  return $element['target_groups'];
}


/**
 * Helper for serializing field item values
 *
 * @see mailup_subscribe_field_insert()
 * @see mailup_subscribe_field_update()
 */
function _mailup_subscribe_process_list_reference_field_values(&$items) {
  foreach ($items as $delta => $value) {
    $items[$delta]['target_groups'] = serialize($value['target_groups']);
    $items[$delta]['settings'] = serialize($value['settings']);
  }
}


function _mailup_subscribe_process_mailup_subscription_field_values($entity_type, $entity, $field, $instance, $langcode, &$items) {

  if (empty($items[0])) {
    return;
  }

  $values = $items[0];

  // Get current subscription state for user
  $user = $entity;
  $user_subscriptions = $user->uid ? mailup_subscribe_load_user_subscriptions($user) : FALSE;

  // Get all configured subscription targets
  $targets = mailup_subscription_load_multiple(FALSE, array(), TRUE);

  // Extract target configuration for simplicity
  $target_config = array();
  foreach ($targets as $id => $target) {
    $config = mailup_subscribe_extract_target_config($targets[$id]);
    $target_config[$id] = $config;
  }

  // We only want to act on any list for which input has been given
  $actionable_lists = array();
  foreach ($values as $id => $value) {
    $mailup_list_id = $target_config[$id]['target_id'];
    $actionable_lists[$mailup_list_id] = TRUE;
  }

  // Build current state for comparison
  $current_state = array();
  if (!empty($user_subscriptions)) {
    foreach ($user_subscriptions as $list_id => $list_state) {
      if (!empty($actionable_lists[$list_id])) {
        // Convert detailed status into basic subscribed/not-subscribed
        $subscribed = mailup_subscribe_status_to_boolean($list_state['status']);
        if ($subscribed) {
          $groups = empty($list_state['groups']) ? array() : array_combine($list_state['groups'], $list_state['groups']);
          $current_state[$list_id] = $groups;
        }
      }
    }
  }
  
  // Build the requested state
  $requested_state = array();
  foreach ($values as $id => $value) {

    $config = $target_config[$id];

    $list_id = $config['target_id'];
    $list_groups = empty($config['target_groups']) ? array() : array_filter($config['target_groups']);
    $expose_groups = !empty($config['settings']['expose_groups']);

    if (!empty($value['subscribe'])) {
      $groups = $expose_groups ? array_filter($value['groups']) : $list_groups;
      $requested_state[$list_id] = $groups;
    }
  }

  // Determine actions
  $subscribe = array_diff_key($requested_state, $current_state);
  $unsubscribe = array_diff_key($current_state, $requested_state);


  $api_actions = array();

  foreach ($subscribe as $list_id => $v) {
    $api_actions['subscribe'][$list_id] = array('groups' => $requested_state[$list_id]);
  }

  foreach ($unsubscribe as $list_id => $v) {
    $api_actions['unsubscribe'][$list_id] = TRUE;
  }

  // Determine added or removed groups
  foreach ($requested_state as $list_id => $groups) {
    if (array_key_exists($list_id, $current_state)) {

      if ($added_groups = array_diff_key($groups, $current_state[$list_id])) {
        // We use the subscribe action to add groups
        if (empty($api_actions['subscribe'][$list_id])) {
          $api_actions['subscribe'][$list_id] = array('groups' => $added_groups);
        }
      }
      if ($removed_groups = array_diff_key($current_state[$list_id], $groups)) {
        $api_actions['remove_groups'][$list_id] = $removed_groups;
      }
    }
  }

  // Make API calls, or queue (depending on settings)
  _mailup_subscribe_process_api_actions($api_actions, $user);
}

/**
 * Implements hook_field_insert().
 */
function mailup_subscribe_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  if ($field['type'] == 'mailup_subscribe_reference') {
    _mailup_subscribe_process_list_reference_field_values($items);
  }

  if ($field['type'] == 'mailup_subscription') {
    _mailup_subscribe_process_mailup_subscription_field_values($entity_type, $entity, $field, $instance, $langcode, $items);
  }
}


function mailup_subscribe_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {

  if ($field['type'] == 'mailup_subscription') {

  }
}

function _mailup_subscribe_process_api_actions($api_actions, $user) {

  $list_entities = mailup_list_load_multiple(FALSE, array(), TRUE);

  if (!empty($api_actions)) {

    foreach ($api_actions as $type => $details) {

      switch ($type) {
        case 'subscribe':

          $lists = array();
          $groups = array();

          $subscribe_to = array();

          // We can action multiple subscribes in a singe API call
          foreach ($details as $list_id => $params) {
            $list_entity = $list_entities[$list_id];

            $subscribe_to[$list_id] = array(
              'id' => $list_id,
              'guid' => $list_entity->guid,
              'group_ids' => array_keys($params['groups'])
            );
          }

          $coi = variable_get('mailup_subscribe_coi_enabled', 0);

          // Call the API method
          mailup_subscribe_subscribe_user($subscribe_to, $user, $coi);
          break;
        case 'unsubscribe':
          
          // By nature we will always have a recipient when unsubscribing
          if ($recipient_meta = mailup_subscribe_load_recipient_metadata(array('uid' => $user->uid))) {

            foreach ($details as $list_id => $dummy) {
              $list_entity = $list_entities[$list_id];
              // @todo: build recipient here for mail
              if (mailup_unsubscribe_front_end($list_id, $list_entity->guid, $user->mail)) {
                // Delete the subscription info
                db_delete('mailup_recipient_subscriptions')
                  ->condition('recipient_id', $recipient_meta['recipient_id'])
                  ->condition('list_id', $list_id)
                  ->execute();
              }
            }
          }
          break;

        case 'remove_groups':
          foreach ($details as $list_id => $group_ids) {
            mailup_subscribe_remove_groups($list_id, $group_ids, $user);
          }
          break;
      }
    }

    return;
  }
}

/**
 * Implements hook_field_update().
 */
function mailup_subscribe_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  if ($field['type'] == 'mailup_list_reference') {
    _mailup_subscribe_process_list_reference_field_values($items);
  }

  if ($field['type'] == 'mailup_subscription') {
    _mailup_subscribe_process_mailup_subscription_field_values($entity_type, $entity, $field, $instance, $langcode, $items);
  }
}


/**
 * Implements hook_field_load().
 */
function mailup_subscribe_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  if ($field['type'] == 'mailup_list_reference') {
    foreach ($entities as $id => $entity) {
      foreach ($items[$id] as $delta => $item) {
        $items[$id][$delta]['target_groups'] = unserialize($item['target_groups']);
        $items[$id][$delta]['settings'] = unserialize($item['settings']);
      }
    }
  }

  // if ($field['type'] == 'mailup_subscription') {
  //   foreach ($entities as $id => $entity) {
  //     foreach ($items[$id] as $delta => $item) {
  //       $items[$id][$delta]['groups'] = unserialize($item['groups']);
  //     }
  //   }
  // }
}

/**
 * Implements hook_field_is_empty().
 */
function mailup_subscribe_field_is_empty($item, $field) {
  return FALSE;
}

