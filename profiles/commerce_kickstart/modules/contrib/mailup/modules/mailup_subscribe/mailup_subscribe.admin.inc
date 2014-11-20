<?php

/**
 * @file
 * Admin related functionality for mailup_subscribe module
 */

/**
 * Submit handler for field mapping form
 * Starts batch operation to import fields
 *
 * @see mailup_subscribe_recipient_field_mapping()
 */
function mailup_subscribe_recipient_field_mapping_refresh($form, &$form_state) {

  $batch = array(
    'operations' => array(
      // @see _mailup_subscribe_import_recipient_fields_op()
      array('_mailup_subscribe_import_recipient_fields_op', array(REQUEST_TIME))
    ),
    'finished' => 'mailup_subscribe_import_recipient_fields_finished',
    'title' => t('Importing Recipient Fields'),
    'init_message' => t('Querying MailUp for Recipient Field details.'),
    'progress_message' => t('Processed @current out of @total tasks.'),
    'error_message' => t('Import encountered an error'),
  );

  batch_set($batch);
}


/**
 * Form Builder for field mapping settings
 *
 * @ingroup forms
 * @see mailup_subscription_recipient_field_mapping_validate()
 * @see mailup_subscription_recipient_field_mapping_submit()
 */
function mailup_subscribe_recipient_field_mapping($form, &$form_state) {

  // Load existing mapping
  $map = mailup_subscribe_get_recipient_field_map();

  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available Recipient Fields')
  );

  $form['import']['refresh'] = array(
    '#type' => 'submit',
    '#value' => t('Import Recipient Field List from MailUp'),
    '#submit' => array('mailup_subscribe_recipient_field_mapping_refresh')
  );

  $form['recipient_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Recipient fields'),
    '#description' => t('Choose the data to be used for recipient fields.'),
    '#tree' => TRUE
  );

  $form['recipient_fields']['token_tree'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('user')
  );

  $recipient_fields = mailup_subscribe_get_imported_recipient_fields();

  $mismatches = 0;
  foreach ($recipient_fields as $field) {

    $name = $field['Description'];
    $id = $field['Id'];

    $value = '';
    if (isset($map[$id])) {
      $value = $map[$id]['value'];
    }

    if (empty($field['base_field'])) {
      $form['recipient_fields'][$id]['enable'] = array(
        '#type' => 'checkbox',
        '#title' => t('@label', array('@label' => $name)),
        '#default_value' => $value == '' ? FALSE : TRUE,
        '#required' => FALSE,
      );
      $form['recipient_fields'][$id]['token'] = array(
        '#type' => 'textfield',
        '#default_value' => $value,
        '#required' => FALSE,
        '#states' => array(
          'visible' => array(
            ':input[name="recipient_fields[' . $id . '][enable]"]' => array('checked' => TRUE),
          ),
        ),
      );
    }
    else {
      $form['recipient_fields'][$id]['enable'] = array(
        '#type' => 'value',
        '#value' => TRUE
      );
      $form['recipient_fields'][$id]['token'] = array(
        '#type' => 'textfield',
        '#title' => t('@label', array('@label' => $name)),
        '#default_value' => $value,
        '#required' => !empty($field['required'])
      );
    }

    // Store the remote field name to allow integrity checking
    $form['recipient_fields'][$id]['name'] = array(
      '#type' => 'value',
      '#value' => $name
    );

    // Check against stored field names for inconsistencies
    if ($value != '' && $name != $map[$id]['name']) {
      $mismatches++;
      $form['recipient_fields'][$id]['token']['#attributes'] = array(
        'class' => array('error')
      );
      $form['recipient_fields'][$id]['info'] = array(
        '#prefix' => '<p class="mapping-info">',
        '#suffix' => '</p>',
        '#markup' => t("Previously named <em>@original</em>", array('@original' => $map[$id]['name']))
      );
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings')
  );

  if ($mismatches) {
    drupal_set_message(format_plural($mismatches, 'A recipient field has been changed since the configuration was saved.', '@count recipient fields have been changed since the configuration was saved.'), 'warning');
  }

  return $form;
}


function mailup_subscribe_recipient_field_mapping_submit($form, &$form_state) {

  $map = array();
  foreach ($form_state['values']['recipient_fields'] as $id => $values) {
    if (!empty($values['enable']) && trim($values['token']) != '') {
      $map[$id] = array(
        'name' => $values['name'],
        'value' => $values['token']
      );
    }
  }
  
  variable_set('mailup_subscribe_recipient_field_map', $map);
}

function mailup_lists_admin_overview_form_confirmed_opt_in($form, &$form_state) {

  return $form['local']['coi_info'];
}

function mailup_lists_admin_overview_form_confirmed_opt_in_validate($form, &$form_state) {

  variable_set('mailup_subscribe_coi_enabled', $form_state['values']['mailup_subscribe_coi_enabled']);
  drupal_set_message(t('Confirmed opt-in @status', array('@status' => $form_state['values']['mailup_subscribe_coi_enabled'] ? 'enabled' : 'disabled')));
}


function mailup_lists_admin_overview_form($form, &$form_state) {

  $form['local'] = array(
    '#type' => 'fieldset',
    '#title' => t('Subscription Targets'),
    '#description' => t('A subscription target is a List/Group combination that a user can subscribe to.')
  );

  $form['local']['coi_info'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="mailup-subscribe-coi-wrapper">',
    '#suffix' => '</div>',
    '#title' => t('Confirmed Opt-In'),
    '#description' => t('When enabled, recipients receive a confirmation email with a link they must click to confirm their subscription. This is considered best practice.'),
    'mailup_subscribe_coi_enabled' => array(
      '#type' => 'checkbox',
      '#default_value' => variable_get('mailup_subscribe_coi_enabled', FALSE),
      '#title' => t('Enable confirmed opt-in'),
      '#element_validate' => array('mailup_lists_admin_overview_form_confirmed_opt_in_validate'),
      '#ajax' => array(
        'callback' => 'mailup_lists_admin_overview_form_confirmed_opt_in',
        'wrapper' => 'mailup-subscribe-coi-wrapper',
      ),
    ),
  );

  $subscription_entities = mailup_subscription_load_multiple();

  $header = array(t('Name'), t('Actions'));
  $rows = array();
  foreach ($subscription_entities as $id => $mailup_subscription) {

    $url = mailup_subscription_uri($mailup_subscription);

    $operations = array(
      array('title' => t('edit'), 'href' => $url['path'] . '/edit'),
      array('title' => t('delete'), 'href' => $url['path'] . '/delete')
    );

    $rows[] = array(
      entity_label('mailup_subscription', $mailup_subscription),
      theme('links', array(
        'links' => $operations,
        'attributes' => array('class' => array('links', 'inline')),
      ))
    );
  }

  if (empty($rows)) {
    $form['local']['lists'] = array(
      '#markup' => t('Choose a list from below to configure a new subscription target for it.')
    );
  }
  else {
    $form['local']['lists'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows
    );
  }

  $form['imported'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tracked MailUp Lists')
  );

  // See if user has requested an update of remote lists
  if (!empty($form_state['#refresh']) || !isset($form_state['#lists'])) {
    $form_state['#lists'] = mailup_list_load_multiple();
  }

  $header = array(t('Name'), t('Company'), t('Description'));

  $options = array();
  $imported_lists_remote_ids = array();
  
  foreach ($form_state['#lists'] as $key => $list) {
    $options[$key] = array(
      $list->name,
      $list->company,
      $list->description
    );

    // And note the mailup ids so we can limit the $form['remote']['lists'] too
    $imported_lists_remote_ids[$list->id] = $list->id; 
  }

  $lists_available = !empty($options);

  if ($lists_available) {
    $form['imported']['imported_lists'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#multiple' => FALSE,
      '#js_select' => FALSE
    );
  }
  else {
    $form['imported']['imported_lists'] = array(
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('You have not yet imported any lists to Drupal.')
    );
  }

  if ($lists_available) {
    $form['imported']['add'] = array(
      '#type' => 'submit',
      '#value' => t('Add a Subscription Target'),
      '#submit' => array('mailup_subscription_admin_overview_form_submit_add')
    );

    $form['imported']['sync'] = array(
      '#type' => 'submit',
      '#value' => t('Sync All Tracked Lists'),
      '#submit' => array('mailup_subscription_admin_overview_form_submit_sync')
    );

    // $form['imported']['sync_groups'] = array(
    //   '#type' => 'submit',
    //   '#value' => t('Update Groups for Tracked Lists'),
    //   '#submit' => array('mailup_subscription_admin_overview_form_submit_sync_groups')
    // );
  }

  // Build options for remote lists
  $remote_lists = variable_get('mailup_remote_lists', array());

  $form['remote'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import Lists from MailUp'),
    '#collapsible' => TRUE,
    // Collapse when the user has already imported one list
    '#collapsed' => $lists_available
  );

  $remote_lists_not_imported = array_diff_key($remote_lists, $imported_lists_remote_ids);

  if (!empty($remote_lists_not_imported)) {

    $remote_lists_header = array(t('List code'), t('Name'));
    $remote_lists_options = array();
    foreach ($remote_lists_not_imported as $remote_list) {
      $markup = '<strong>' . check_plain($remote_list['Name']) . '</strong><br>';
      $markup .= '<span>' . check_plain($remote_list['Description']) . '</span>';
      $remote_lists_options[$remote_list['idList']] = array($remote_list['idList'], $markup);
    }

    $form['remote']['remote_lists'] = array(
      '#type' => 'tableselect',
      '#header' => $remote_lists_header,
      '#options' => $remote_lists_options,
      '#multiple' => TRUE,
      '#js_select' => FALSE
    );

    $form['remote']['import'] = array(
      '#type' => 'submit',
      '#value' => t('Import Selected List to Drupal'),
      '#submit' => array('mailup_subscribe_list_admin_overview_submit_import_remote')
    );
  }
  else {
    if ($lists_available) {
      $form['remote']['info'] = array(
        '#prefix' => '<p>',
        '#suffix' => '</p>',
        '#markup' => t('All available lists have been imported.')
      );
    }
  }

  $form['remote']['refresh'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh List Options'),
    '#submit' => array('mailup_subscribe_list_admin_overview_submit_refresh_remote')
  );

  return $form;
}

function mailup_lists_admin_overview_form_validate($form, &$form_state) {

  if (!empty($form['remote']['import']) && $form_state['triggering_element']['#submit'][0] == $form['remote']['import']['#submit'][0]) {
    $selected_lists = array_filter($form_state['values']['remote_lists']);
    if (empty($selected_lists)) {
      form_set_error('import', t('Please select a list to import.'));
    }
  }
}

function mailup_subscribe_list_admin_overview_submit_import_remote($form, &$form_state) {

  // Only import/sync lists selected on the form
  $selected_lists = array_filter($form_state['values']['remote_lists']);

  $sync_lists_batch = mailup_subscribe_sync_list_entities_batch($selected_lists);
  batch_set($sync_lists_batch);
}

function mailup_subscribe_list_admin_overview_submit_refresh_remote($form, &$form_state) {

  try {
    $lists = mailup_service_get_lists();
    if (is_array($lists)) {
      variable_set('mailup_remote_lists', $lists);
      drupal_set_message(t('Successfully updated MailUp list selection.'));
    }
  }
  catch (DrupalMailUpException $e) {
    drupal_set_message(t('Unable to retrieve MailUp list selection: @error', array('@error' => $e->getMessage())), 'error');
  }
}


function mailup_subscription_admin_overview_form_submit_sync($form, &$form_state) {
  
  $list_entities = $form_state['#lists'];

  $selected_lists = array();
  foreach ($list_entities as $list) {
    $selected_lists[$list->id] = $list->id;
  }

  $sync_lists_batch = mailup_subscribe_sync_list_entities_batch($selected_lists);
  batch_set($sync_lists_batch);
}

function mailup_subscription_admin_overview_form_submit_sync_groups($form, &$form_state) {
  
  $list_entities = $form_state['#lists'];

  $selected_lists = array();
  foreach ($list_entities as $list) {
    $selected_lists[$list->id] = $list->id;
  }

  $sync_groups_batch = mailup_subscribe_sync_list_groups_batch($selected_lists);
  batch_set($sync_groups_batch);
}


/**
 * Form submit handler for adding a subscription based on list
 *
 */
function mailup_subscription_admin_overview_form_submit_add($form, &$form_state) {

  $values = $form_state['values'];
  $lists = $form_state['#lists'];

  if (empty($values['imported_lists'])) {
    drupal_set_message(t('Please select a list'), 'error');
    return;
  }

  $list_entity = $lists[ $values['imported_lists'] ];

  if ($subscription = mailup_subscribe_create_subscription_from_list($list_entity)) {
    $subscription->save();
    $form_state['redirect'] = 'admin/config/services/mailup/lists/subscription/' . $subscription->id . '/edit';
  }
}

/**
 * Form builder: edit a mailup list.
 *
 * @ingroup forms
 * @see mailup_list_form_validate()
 * @see mailup_list_form_submit()
 */
function mailup_subscribe_subscription_form($form, &$form_state, $mailup_subscription = NULL, $op = FALSE) {
  
  if (!$mailup_subscription) {
    $mailup_subscription = mailup_subscription_create();
  }
  if (empty($form_state['mailup_subscription'])) {
    $form_state['mailup_subscription'] = $mailup_subscription;
  }

  $langcode = entity_language('mailup_subscription', $mailup_subscription);

  if (empty($langcode)) {
    $langcode = LANGUAGE_NONE;
  }

  field_attach_form('mailup_subscription', $mailup_subscription, $form, $form_state, $langcode);

  // Simply use default language
  $form['language'] = array(
    '#type' => 'value',
    '#value' => $langcode,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validation function for the user account and profile editing form.
 */
function mailup_subscribe_subscription_form_validate($form, &$form_state) {
  entity_form_field_validate('mailup_subscription', $form, $form_state);
}

/**
 * Submit function for the user address editing form.
 */
function mailup_subscribe_subscription_form_submit($form, &$form_state) {
  $mailup_subscription = &$form_state['mailup_subscription'];
  $mailup_subscription->language = $form_state['values']['language'];

  // Notify field widgets.
  field_attach_submit('mailup_subscription', $mailup_subscription, $form, $form_state);

  // Save the mailup list.
  entity_save('mailup_subscription', $mailup_subscription);

  $form_state['redirect'] = 'admin/config/services/mailup/lists';

  drupal_set_message(t('The changes have been saved.'));
}

function mailup_subscribe_subscription_edit($mailup_subscription = NULL) {
  return drupal_get_form('mailup_subscribe_subscription_form', $mailup_subscription);
}


/**
 * Form constructor for the node deletion confirmation form.
 *
 * @see mailup_subscribe_subscription_delete_confirm_submit()
 */
function mailup_subscribe_subscription_delete_confirm($form, &$form_state, $mailup_subscription) {
  $form['#mailup_subscription'] = $mailup_subscription;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['id'] = array('#type' => 'value', '#value' => $mailup_subscription->id);
  return confirm_form($form,
    t('Are you sure you want to delete %label?', array('%label' => $mailup_subscription->label)),
    'admin/config/services/mailup/lists',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}


/**
 * Executes node deletion.
 *
 * @see node_delete_confirm()
 */
function mailup_subscribe_subscription_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $mailup_subscription = mailup_subscription_load($form_state['values']['id']);
    mailup_subscription_delete($mailup_subscription);
    cache_clear_all();
    drupal_set_message(t('Subscription Target %label has been deleted.', array('%label' => $mailup_subscription->label)));
  }

  $form_state['redirect'] = 'admin/config/services/mailup/lists';
}

function mailup_subscribe_webhook_settings() {

  global $base_url;

  $form = array();

  $form['mailup_webhook_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Encryption Key'),
    '#default_value' => variable_get('mailup_webhook_key', ''),
    '#description' => t('Enter the encryption key that MailUp webhooks will be configured to use.'),
    '#required' => TRUE
  );

  $form['info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration Values')
  );

  $callback_rows = array(
    array(
      array('data' => t('Callback URL'), 'header' => TRUE), array('data' => $base_url . '/mailup')
    )
  );

  $form['info']['callback_url'] = array(
    '#theme' => 'table',
    '#header' => NULL,
    '#rows' => $callback_rows
  );

  $header = array(
    t('Parameter Name'), t('Parameter Value')
  );
  
  $rows = array(
    array('list', 'ID List'),
    array('email', 'Email'),
    array('type', 'Event Type')
  );

  $form['info']['fields'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows
  );

  $form['info']['help'] = array(
    '#markup' => t('The above values are required for Webhooks to work correctly with Drupal. See README.txt for full instructions.')
  );

  return system_settings_form($form);
}
