<?php

/**
 * @file
 * Webform CiviCRM module's administrative functions.
 */

module_load_include('inc', 'webform_civicrm', 'webform_civicrm_utils');

/**
 * Drupal form builder callback
 * Form to configure CiviCRM options for a Webform
 */
function wf_crm_configure_form($form, &$form_state, $node) {
  $form = array();
  $form_state['storage']['nid'] = $node->nid;

  // Display confirmtion message before deleting fields
  if (!empty($form_state['storage']['msg'])) {
    $form['#prefix'] = $form_state['storage']['msg'];
    $form['cancel'] = $form['disable'] = $form['delete'] = array('#type' => 'submit');
    $form['delete']['#value'] = t('Remove Fields and Save Settings');
    $form['disable']['#value'] = t('Leave Fields and Save Settings');
    $form['cancel']['#value'] = t('Cancel (go back)');
    return $form;
  }

  $form['#attached']['js'][] = drupal_get_path('module', 'webform_civicrm') . '/' . 'webform_civicrm_admin.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/webform_civicrm_admin.css';
  civicrm_initialize();
  require_once 'CRM/Core/BAO/Setting.php';
  $config = CRM_Core_Config::singleton();

  $fields = wf_crm_get_fields();
  $sets = wf_crm_get_fields('sets');

  // Initialize form on first view
  if (empty($form_state['values']))  {
    // For an existing civi-webform, load settings
    if (isset($node->webform_civicrm)) {
      $settings = $node->webform_civicrm;
    }
    // For a new civi-webform, set up some defaults
    else {
      $settings = array(
        'data' => array(
          'contact' => array(
            1 => array(
              'contact' => array(1 => array(
                  'contact_type' => 'individual',
                  'contact_sub_type' => array(),
                )),
            ),
          ),
        ),
        'confirm_subscription' => 1,
        'create_fieldsets' => 1,
        'new_contact_source' => check_plain($node->title),
        'civicrm_1_contact_1_contact_first_name' => 'create_civicrm_webform_element',
        'civicrm_1_contact_1_contact_last_name' => 'create_civicrm_webform_element',
        'civicrm_1_contact_1_contact_existing' => 'create_civicrm_webform_element',
      );
    }
  }
  // On rebuilding the form
  else {
    $settings = wf_crm_aval($form_state['storage'], 'vals', $form_state['values']);
    wf_crm_process_form_settings($settings);
    unset($form_state['storage']['vals']);
  }
  // Merge in existing fields
  $existing = array_keys(wf_crm_enabled_fields($node, NULL, TRUE));
  $settings += array_fill_keys($existing, 'create_civicrm_webform_element');

  $tokens = '<strong>' . t('Contact 1 Tokens:') . '</strong> [' . implode('], [', wf_crm_get_fields('tokens')) . '].';
  list($contact_types, $sub_types) = wf_crm_get_contact_types();
  $data = $settings['data'];
  $contacts = count($data['contact']);
  $form['#attached']['js'][] = array(
    'data' => array('webform_civicrm' => array('rTypes' => wf_crm_get_relationship_types())),
    'type' => 'setting',
  );

  // Sort fields by set
  foreach ($fields as $fid => $field) {
    list($group) = explode('_', $fid, 2);
    $sets[$group]['fields'][$fid] = $field;
  }
  $form['nid'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable CiviCRM Processing'),
    '#default_value' => !empty($settings['nid']),
    '#return_value' => $node->nid,
    '#description' => t('Contacts will be created or updated when users submit this webform.'),
    '#prefix' => '<a id="web-civi-help" class="module-link module-link-help" href="http://drupal.org/node/1615380" target="_blank">' . t('Webform CiviCRM Help') . '</a>',
  );
  $form['number_of_contacts'] = array(
    '#type' => 'select',
    '#title' => t('Number of Contacts'),
    '#default_value' => $contacts,
    '#options' => drupal_map_assoc(range(1, 30)),
  );
  $form['change_form_settings'] = array(
    '#type' => 'button',
    '#value' => t('Change Form Settings'),
    '#prefix' => '<div id="no-js-button-wrapper" class="messages warning">',
    '#suffix' => '<div>' . t('You have Javascript disabled. You will need to click this button after changing any option to see the result.') . '</div></div>',
  );
  $form['webform_civicrm'] = array('#type' => 'vertical_tabs');

  // Contact settings
  foreach ($data['contact'] as $n => $c) {
    $form['contact_' . $n] = array(
      '#type' => 'fieldset',
      '#title' => t('Contact !num', array('!num' => $n)),
      '#description' => $n > 1 ? NULL : t('Primary contact. Usually assumed to be the person filling out the form.') . '<br />' . t('Check the "Existing Contact" box to autofill with the current user (or another contact).'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('contact-icon-' . $c['contact'][1]['contact_type'])),
    );
    $form['contact_' . $n][$n . '_contact_type'] = array(
      '#type' => 'select',
      '#title' => t('Contact Type'),
      '#default_value' => $c['contact'][1]['contact_type'],
      '#options' => $contact_types,
      '#prefix' => '<div class="contact-type-select">',
      '#suffix' => '</div>',
    );
    wf_crm_ajax_item($form, 'contact_' . $n, $n . '_contact_type', 'contact_subtype_wrapper');

    // Contact sub-type
    $fid = 'civicrm_' . $n . '_contact_1_contact_contact_sub_type';
    if (!empty($sub_types[$c['contact'][1]['contact_type']])) {
      $field = wf_crm_configure_form_item($fid, $fields['contact_contact_sub_type'], $settings);
      $field['#title'] .= ' ' . $contact_types[$c['contact'][1]['contact_type']];
      $form['contact_' . $n]['contact_subtype_wrapper'][$fid] = $field;
      wf_crm_ajax_item($form, 'contact_' . $n . ':contact_subtype_wrapper', $fid, 'contact_custom_wrapper');
    }
    // If no sub-types
    else {
      $form['contact_' . $n]['contact_subtype_wrapper'][$fid] = array(
        '#type' => 'value',
        '#value' => array(),
      );
    }

    $form['contact_' . $n]['contact_subtype_wrapper']['clear'] = array(
      '#markup' => '<div class="clearfix"> </div>',
    );

    foreach ($sets as $sid => $set) {
      if ($set['entity_type'] != 'contact') {
        continue;
      }
      if ($sid == 'relationship' && !($set['max_instances'] = $n - 1)) {
        continue;
      }
      if (!empty($set['contact_type']) && $set['contact_type'] != $c['contact'][1]['contact_type']) {
        continue;
      }
      if (!empty($set['sub_types'])) {
        if (!array_intersect($c['contact'][1]['contact_sub_type'], $set['sub_types'])) {
          continue;
        }
        $pos = &$form['contact_' . $n]['contact_subtype_wrapper']['contact_custom_wrapper'];
        $path = 'contact_' . $n . ':contact_subtype_wrapper:contact_custom_wrapper';
      }
      elseif (!empty($set['contact_type']) || $sid == 'contact') {
        $pos = &$form['contact_' . $n]['contact_subtype_wrapper'];
        $path = 'contact_' . $n . ':contact_subtype_wrapper';
      }
      else {
        $pos = &$form['contact_' . $n];
        $path = 'contact_' . $n;
      }
      if (!empty($set['max_instances'])) {
        if (!isset($c['number_of_' . $sid])) {
          $c['number_of_' . $sid] = 0;
        }
        $set['label'] = $sid == 'relationship' ? t('Relationship') : $set['label'];
        $selector = array(
          '#type' => 'select',
          '#default_value' => $c['number_of_' . $sid],
          '#prefix' => '<div class="number-of">',
          '#suffix' => '</div>',
        );
        if ($set['max_instances'] > 1) {
          $selector['#options'] = range(0, $set['max_instances']);
          $selector['#title'] = t('Number of %type Fields', array('%type' => $set['label']));
        }
        else {
          $selector['#options'] = array(t('No'), t('Yes'));
          $selector['#title'] = t('Enable %type Fields', array('%type' => $set['label']));
        }
        $pos['contact_' . $n . '_number_of_' . $sid] = $selector;
        wf_crm_ajax_item($form, $path, 'contact_' . $n . '_number_of_' . $sid, $n . $sid . '_wrapper');
      }
      else {
        $c['number_of_' . $sid] = 1;
      }
      for ($i = 1; $i <= $c['number_of_' . $sid]; ++$i) {
        $fsid = 'civicrm_' . $n . $sid . $i . '_fieldset';
        $fieldset = array(
          '#type' => 'fieldset',
          '#title' => $set['label'],
          '#attributes' => array('id' => $fsid, 'class' => array('web-civi-checkbox-set')),
          'js_select' => wf_crm_js_select($fsid),
        );
        if ((isset($set['max_instances']) && $set['max_instances'] > 1) || $sid == 'relationship') {
          $fieldset['#title'] .= ' ' . $i;
          if (in_array($sid, array('email', 'address', 'phone', 'website')) && $i == 1) {
            $fieldset['#title'] .= ' ' . t('(primary)');
          }
        }
        foreach ($set['fields'] as $fid => $field) {
          if ($fid == 'contact_contact_sub_type' ||
          ($fid == 'address_master_id' && $contacts == 1) ||
          (isset($field['contact_type']) && $field['contact_type'] != $c['contact'][1]['contact_type'])) {
            continue;
          }
          $fid = 'civicrm_' . $n . '_contact_' . $i . '_' . $fid;
          $fieldset[$fid] = wf_crm_configure_form_item($fid, $field, $settings);
        }
        if (isset($set['max_instances'])) {
          $pos[$n . $sid . '_wrapper'][$n . $sid . $i . '_fieldset'] = $fieldset;
        }
        else {
          $pos[$n . $sid . $i . '_fieldset'] = $fieldset;
        }
      }
    }
  }

  // Configure messages
  $form['prefix'] = array(
    '#type' => 'fieldset',
    '#title' => t('Introduction Text'),
    '#description' => t('This text will appear at the top of the form. You may configure separate messages for known contacts (logged in users, or users following a hashed link from civimail) and unknown (anonymous) users.'),
    '#group' => 'webform_civicrm',
    '#attributes' => array('class' => array('civi-icon-text')),
  );
  $form['prefix']['prefix_known'] = array(
    '#type' => 'textarea',
    '#title' => t('Introduction text for known contacts'),
    '#default_value' => wf_crm_aval($settings, 'prefix_known'),
    '#description' => $tokens,
  );
  $form['prefix']['prefix_unknown'] = array(
    '#type' => 'textarea',
    '#title' => t('Introduction text for unknown contacts'),
    '#default_value' => wf_crm_aval($settings, 'prefix_unknown'),
    '#description' => t('No tokens available for unknown contacts.'),
  );
  $form['st_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('"Not You?" Message'),
    '#description' => t('Prompt for users who are logged in as, or following a hashed link for, someone else.'),
    '#group' => 'webform_civicrm',
    '#attributes' => array('class' => array('civi-icon-message')),
  );
  $form['st_message']['toggle_message'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display message to known contacts?'),
    '#default_value' => !empty($settings['message']),
  );
  $form['st_message']['message'] = array(
    '#type' => 'textfield',
    '#title' => t('Text (displayed as a status message)'),
    '#default_value' => wf_crm_aval($settings, 'message', t("You are viewing this form as [display name]. Please {click here if that's not you}.")),
    '#size' => 100,
    '#maxlength' => 255,
    '#description' => t('Enclose your "not you" link text in curly brackets {like this}.') . '<p>' . $tokens . '</p>',
  );
  // Case and activity settings
  $case_enabled = in_array('CiviCase', $config->enableComponents, TRUE) && ($case_types = wf_crm_get_options('case_type'));
  $form['act'] = array(
    '#type' => 'fieldset',
    '#title' => $case_enabled ? t('Activity/Case') : t('Activity'),
    '#group' => 'webform_civicrm',
    '#attributes' => array('class' => array('civi-icon-activity')),
  );
  if ($case_enabled) {
    $form['act']['case_type_id'] = array(
      '#type' => 'select',
      '#title' => t('CiviCase Type'),
      '#description' => t('Is this activity part of a case?'),
      '#options' => array(t('- No Case -')) + $case_types,
    );
    wf_crm_ajax_item($form, 'act', 'case_type_id', 'case');
  }
  $case_type = $activity_type = NULL;
  if ($case_enabled && !empty($data['case'][1]['case'][1]['case_type_id'])) {
    $case_type = $form['act']['case_type_id']['#default_value'] = $data['case'][1]['case'][1]['case_type_id'];
    $case_type_name = $case_types[$case_type];
    $form['act']['case']['case_fields'] = array(
      '#type' => 'fieldset',
      '#title' => $case_type_name . ' ' . t('Case'),
    );
    $form['act']['case']['case_fields']['existing_case_status'] = array(
      '#type' => 'select',
      '#title' => t('Update Existing Case'),
      '#description' => t('If any of the above options are selected and a %type case with that status already exists for the client, it will be autofilled and updated.', array('%type' => $case_type_name)),
      '#options' => wf_crm_get_options('case_status'),
      '#default_value' => wf_crm_aval($data, 'case:1:existing_case_status', array()),
      '#multiple' => TRUE,
    );
    require_once('CRM/Case/XMLProcessor/Process.php');
    $case_info = new CRM_Case_XMLProcessor_Process();
    $act_types = $case_info->get($case_type_name, 'ActivityTypes');
  }
  else {
    $param = NULL;
    // Include campaign types
    if (in_array('CiviCampaign', $config->enableComponents, TRUE)) {
      $param = CRM_Core_DAO::singleValueQuery("SELECT id FROM civicrm_component WHERE name = 'CiviCampaign'");
    }
    $act_types = wf_crm_get_options('activity_type', $param);
  }
  $form['act']['case']['activity_type_id'] = array(
    '#type' => 'select',
    '#title' => t('Activity Type'),
    '#options' => array(t('- No Activity -')) + $act_types,
  );
  wf_crm_ajax_item($form, 'act:case', 'activity_type_id', 'activity_fields');
  if (!empty($data['activity'][1]['activity'][1]['activity_type_id']) && isset($act_types[$data['activity'][1]['activity'][1]['activity_type_id']])) {
    $form['act']['case']['activity_fields']['#type'] = 'fieldset';
    $activity_type = $form['act']['case']['activity_type_id']['#default_value'] = $settings['activity_type_id'] = $data['activity'][1]['activity'][1]['activity_type_id'];
    $form['act']['case']['activity_fields']['#title'] = $act_types[$activity_type] . ' ' . t('Activity');

    $form['act']['case']['activity_fields']['existing_activity_status'] = array(
      '#type' => 'select',
      '#title' => t('Update Existing Activity'),
      '#description' => t('If any of the above options are selected and a %type activity with that status already exists for !ent, it will be autofilled and updated.', array('!ent' => $case_type ? t('the case') : t('contact 1'), '%type' => $act_types[$settings['activity_type_id']])),
      '#options' => wf_crm_get_options('activity_status'),
      '#default_value' => wf_crm_aval($data, 'activity:1:existing_activity_status', array()),
      '#multiple' => TRUE,
    );
    $form['act']['case']['activity_fields']['activity_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Activity Subject'),
      '#maxlength' => 255,
      '#required' => TRUE,
      '#description' => t('You can override this default by allowing users to enter a subject (enable "Activity Subject" field below)'),
      '#default_value' => wf_crm_aval($data, 'activity:1:activity:1:subject', check_plain($node->title)),
    );
    $form['act']['case']['activity_fields']['activity_details'] = array(
      '#type' => 'checkboxes',
      '#options' => array(
        'entire_result' => t('Include <em>entire</em> webform submission in activity details'),
        'view_link' => t('Include link to <em>view</em> webform submission in activity details'),
        'edit_link' => t('Include link to <em>edit</em> webform submission in activity details'),
      ),
      '#default_value' => wf_crm_aval($data, 'activity:1:details', array('view_link'), TRUE),
    );
  }
  foreach ($sets as $sid => $set) {
    $sid .= '_fieldset';
    $ent = $set['entity_type'];
    if (($ent == 'activity' && $type = $activity_type) || ($ent == 'case' && $type = $case_type)) {
      if ((!empty($set['sub_types']) && !in_array($type, $set['sub_types'])) || empty($set['fields'])) {
        continue;
      }
      $form['act']['case'][$ent . '_fields'][$sid] = array(
        '#type' => 'fieldset',
        '#title' => $set['label'],
        '#attributes' => array('id' => $sid, 'class' => array('web-civi-checkbox-set')),
        'js_select' => wf_crm_js_select($sid),
      );
      foreach ($set['fields'] as $fid => $field) {
        if ($fid == 'activity_assignee_contact_id' && CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME, 'activity_assignee_notification')) {
          $field['extra']['description'] = t('A copy of this activity will be emailed to the assignee.');
        }
        $fid = "civicrm_1_{$ent}_1_$fid";
        $form['act']['case'][$ent . '_fields'][$sid][$fid] = wf_crm_configure_form_item($fid, $field, $settings);
      }
    }
  }
  // Event participant settings
  if (in_array('CiviEvent', $config->enableComponents, TRUE)) {
    $form['event'] = array(
      '#type' => 'fieldset',
      '#title' => t('Event Registration'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-participant')),
    );
    $reg_type = wf_crm_aval($data, 'participant_reg_type');
    $form['event']['participant_reg_type'] = array(
      '#type' => 'select',
      '#title' => t('Registration Method'),
      '#default_value' => $reg_type,
      '#options' => array(
        t('- None -'),
        'all' => t('Register all contacts for the same event(s)'),
        'separate' => t('Register each contact separately'),
      ),
    );
    $form['event']['event_type'] = array(
      '#type' => 'select',
      '#title' => t('Show Events of Type'),
      '#options' => array('any' => t('- Any Type -')) + wf_crm_get_options('event_type'),
      '#default_value' => wf_crm_aval($data, 'reg_options:event_type', 'any'),
      '#prefix' => '<div id="event-reg-options-wrapper"><div class="web-civi-checkbox-set">',
      '#parents' => array('reg_options', 'event_type'),
      '#tree' => TRUE,
    );
    $form['event']['show_past_events'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show Past Events'),
      '#default_value' => (bool) wf_crm_aval($data, 'reg_options:show_past_events'),
      '#suffix' => '</div>',
      '#parents' => array('reg_options', 'show_past_events'),
      '#tree' => TRUE,
    );
    $form['event']['reg_options'] = array(
      '#prefix' => '<div class="clearfix"> </div>',
      '#suffix' => '</div>',
      '#type' => 'fieldset',
      '#title' => t('Registration Options'),
      '#collapsible' => TRUE,
      '#tree' => TRUE,
    );
    $form['event']['reg_options']['show_remaining'] = array(
      '#type' => 'select',
      '#title' => t('Show Remaining Space in Events'),
      '#default_value' => wf_crm_aval($data, 'reg_options:show_remaining', 0),
      '#description' => t('Display a message at the top of the form for each event with a registration limit or past end date.'),
      '#options' => array(
        t('Never'),
        'always' => t('Always'),
        '0_full' => t('When full - 0 spaces left'),
      ),
    );
    foreach (array(5, 10, 20, 50, 100, 200, 500, 1000) as $num) {
      $form['event']['reg_options']['show_remaining']['#options'][$num] = t('When under !num spaces left', array('!num' => $num));
    }
    $form['event']['reg_options']['validate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Prevent Registration for Past/Full Events'),
      '#default_value' => (bool) wf_crm_aval($data, 'reg_options:validate'),
      '#description' => t('Will not allow the form to be submitted if user registers for an event that is ended or full.'),
    );
    $form['event']['reg_options']['block_form'] = array(
      '#type' => 'checkbox',
      '#title' => t('Block Form Access when Event(s) are Full/Ended'),
      '#default_value' => (bool) wf_crm_aval($data, 'reg_options:block_form'),
      '#description' => t('Hide webform if all the events for the form are full or ended.'),
    );
    wf_crm_ajax_item($form, 'event', 'participant_reg_type', 'participants');
    wf_crm_ajax_item($form, 'event', 'event_type', 'participants');
    wf_crm_ajax_item($form, 'event', 'show_past_events', 'participants');

    for ($n = 1; $reg_type && (($n <= $contacts && $reg_type != 'all') || $n == 1); ++$n) {
      $form['event']['participants'][$n] = array(
        '#type' => 'fieldset',
        '#title' => $reg_type == 'all' ? t('Registration') : t('Contact !num', array('!num' => $n)),
      );
      if (!($num = wf_crm_aval($data, "participant:{$n}:number_of_participant"))
      || ($n > 1 && $reg_type == 'all')) {
        $num = 0;
      }
      $form['event']['participants'][$n]['participant_' . $n . '_number_of_participant'] = array(
        '#type' => 'select',
        '#title' => $reg_type == 'all' ? t('Number of Event Sets') : t('Number of Event Sets for Contact !num', array('!num' => $n)),
        '#default_value' => $num,
        '#options' => range(0, $sets['participant']['max_instances']),
        '#prefix' => '<div class="number-of">',
        '#suffix' => '</div>',
      );
      wf_crm_ajax_item($form, "event:participants:{$n}", 'participant_' . $n . '_number_of_participant', 'div');
      $particpant_extensions = array(
        1 => 'role_id',
        2 => 'event_id',
        3 => 'event_type'
      );
      for ($e = 1; $e <= $num; ++$e) {
        $fs = "participant_{$n}_event_{$e}_fieldset";
        $form['event']['participants'][$n]['div'][$fs] = array(
          '#type' => 'fieldset',
          '#title' => t('Event !num', array('!num' => $e)),
          '#attributes' => array('id' => $fs),
        );
        foreach ($sets as $sid => $set) {
          if ($set['entity_type'] == 'participant') {
            $sid = 'civicrm_' . $n . '_participant_' . $e . '_' . $sid . '_fieldset';
            $class = 'web-civi-checkbox-set';
            if (!empty($set['sub_types'])) {
              $role_id = wf_crm_aval($data, "participant:$n:particpant:$e:role_id", '');
              $event_id = wf_crm_aval($data, "participant:$n:particpant:$e:event_id", '');
              $event_type = wf_crm_aval($data, 'reg_options:event_type', '');
              if ($event_id && $event_id !== 'create_civicrm_webform_element') {
                list($event_id, $event_type) = explode('-', $event_id);
              }
              $ext = $particpant_extensions[$set['extension_of']];
              if (!in_array($$ext, $set['sub_types'])) {
                $class .= ' hidden';
              }
              $class .= ' extends-condition ' . str_replace('_', '', $ext) . '-' . implode('-', $set['sub_types']);
            }
            $form['event']['participants'][$n]['div'][$fs][$sid] = array(
              '#type' => 'fieldset',
              '#title' => $set['label'],
              '#attributes' => array('id' => $sid, 'class' => array($class)),
              'js_select' => wf_crm_js_select($sid),
            );
            foreach ($set['fields'] as $fid => $field) {
              $id = 'civicrm_' . $n . '_participant_' . $e . '_' . $fid;
              $item = wf_crm_configure_form_item($id, $field, $settings);
              if ($fid == 'participant_event_id' || $fid == 'participant_role_id') {
                $item['#attributes']['onchange'] = "wfCiviAdmin.participantConditional('#$fs');";
                $item['#attributes']['class'] = array($fid);
                $$fid = wf_crm_aval($item, '#default_value');
              }
              $form['event']['participants'][$n]['div'][$fs][$sid][$id] = $item;
            }
          }
        }
      }
    }
  }
  // Configure additional options
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Options'),
    '#group' => 'webform_civicrm',
    '#attributes' => array('class' => array('civi-icon-prefs')),
    '#description' => '<p>' .
    t('To have this form auto-filled for anonymous users, enable the contact 1 "existing contact" field and send the following link from CiviMail:') .
    '<br /><code>' . url('node/' . $node->nid, array('absolute' => TRUE, 'query' => array('cid1' => ''))) . '{contact.contact_id}&amp;{contact.checksum}</code></p>',
  );
  $form['options']['create_fieldsets'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create Fieldsets'),
    '#default_value' => (bool) $settings['create_fieldsets'],
    '#description' => t('Create a fieldset around each contact. Provides visual organization of your form. Also allows the contact clone feature to work.'),
  );
  $form['options']['confirm_subscription'] = array(
    '#type' => 'checkbox',
    '#title' => t('Confirm Subscriptions'),
    '#default_value' => (bool) $settings['confirm_subscription'],
    '#description' => t('Recommended. Send a confirmation email before adding contacts to publicly subscribable mailing list groups.') . '<br />' . t('Your public mailing lists:') . ' <em>',
  );
  if ($ml = wf_crm_get_options('mailing_lists')) {
    if (count($ml) > 4) {
      $ml = array_slice($ml, 0, 3);
      $ml[] = t('etc.');
    }
    $form['options']['confirm_subscription']['#description'] .= implode(', ', $ml) . '</em>';
  }
  else {
    $form['options']['confirm_subscription']['#description'] .= t('none') . '</em>';
  }
  $form['options']['block_unknown_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Block Unknown Users'),
    '#default_value' => !empty($settings['block_unknown_users']),
    '#description' => t('Only allow users to see this form if they are logged in or following a personalized link from CiviMail.'),
  );
  $form['options']['new_contact_source'] = array(
    '#type' => 'textfield',
    '#title' => t('New Contact Source'),
    '#maxlength' => 255,
    '#size' => 30,
    '#default_value' => $settings['new_contact_source'],
    '#description' => t('Optional "source" label for any new contact created by this webform.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  return $form;
}

/**
 * Build the $data array for webform settings; called while rebuilding or post-processing the configure form.
 *
 * @param $vals
 *   form_state[values] reference
 */
function wf_crm_process_form_settings(&$vals) {
  $data = array('contact' => array());
  list($contact_types, $sub_types) = wf_crm_get_contact_types();
  for ($c = 1; $c <= $vals['number_of_contacts']; ++$c) {
    // Contact settings
    if (isset($vals[$c . '_contact_type'])) {
      $data['contact'][$c] = array(
        'contact' => array(1 => array(
          'contact_type' => $vals[$c . '_contact_type'],
          'contact_sub_type' => array(),
        )),
      );
      if ($sub_type = wf_crm_aval($vals, 'civicrm_' . $c . '_contact_1_contact_contact_sub_type')) {
        $allowed = $sub_types[$vals[$c . '_contact_type']];
        foreach ($sub_type as $sub) {
          if (isset($allowed[$sub])) {
            $data['contact'][$c]['contact'][1]['contact_sub_type'][$sub] = $sub;
          }
        }
      }
    }
    // Add new contact to the form
    else {
      $data['contact'][$c] = array(
        'contact' => array(1 => array(
            'contact_type' => 'individual',
            'contact_sub_type' => array(),
          )),
      );
      // Set defaults for new contact
      $vals += array(
        'civicrm_' . $c . '_contact_1_contact_first_name' => 'create_civicrm_webform_element',
        'civicrm_' . $c . '_contact_1_contact_last_name' => 'create_civicrm_webform_element',
      );
      // Add as activity target
      if (isset($vals['activity_subject'])) {
        $vals['civicrm_1_activity_1_activity_target_contact_id']["$c"] = "$c";
      }
      drupal_set_message(t('Tip: Consider using the clone feature to add multiple similar contacts. (see help for more info)'), 'status', FALSE);
    }
  }
  // Store "number of foo for bar" fields, i.e. number of email for contact 1
  foreach ($vals as $key => $val) {
    if (strpos($key, '_number_of_') !== FALSE) {
      list($ent, $c, $k) = explode('_', $key, 3);
      if (isset($data[$ent][$c]) || $ent == 'participant') {
        $data[$ent][$c][$k] = $val;
      }
    }
  }
  if (!empty($vals['activity_type_id'])) {
    $data['activity'][1]['activity'][1]['activity_type_id'] = $vals['activity_type_id'];
    if (isset($vals['activity_subject'])) {
      $data['activity'][1]['activity'][1]['subject'] = $vals['activity_subject'];
      $data['activity'][1]['details'] = $vals['activity_details'];
      $data['activity'][1]['existing_activity_status'] = $vals['existing_activity_status'];
    }
    else {
      $data['activity'][1]['activity'][1]['target_contact_id'] = range(1, $vals['number_of_contacts']);
    }
  }
  if (!empty($vals['case_type_id'])) {
    if (empty($data['case'][1])) {
      $data['case'][1]['case'][1]['creator_id'] = 1;
    }
    $data['case'][1]['case'][1]['case_type_id'] = $vals['case_type_id'];
    $data['case'][1]['existing_case_status'] = $vals['existing_case_status'];
  }
  if (isset($vals['participant_reg_type'])) {
    $data['participant_reg_type'] = $vals['participant_reg_type'];
    $data['reg_options'] = $vals['reg_options'];
  }
  $vals['data'] = $data;
}

/**
 * Build a field item for the configure form
 *
 * @param $fid: string
 *   civicrm field id
 * @param $field: array
 *   Webform field info
 * @param $settings
 *   webform_civicrm configuration for this form
 *
 * @return array
 *   FAPI form item array for the configure form
 */
function wf_crm_configure_form_item($fid, $field, $settings) {
  list( , $c, $ent, $n, $table, $name) = explode('_', $fid, 6);
  $field['name'] = str_replace('#', $table == 'relationship' ? $n : '', $field['name']);

  // Create dropdown list
  if (!empty($field['expose_list'])) {
    // Retrieve option list
    $options = array('create_civicrm_webform_element' => t('- User Select -'));
    $field['form_key'] = $fid;
    $options += wf_crm_field_options($field, 'config_form', $settings['data']);
    $item = array(
      '#type' => 'select',
      '#title' => $field['name'],
      '#options' => $options,
      '#multiple' => !empty($field['extra']['multiple']),
      '#default_value' => !empty($field['empty_option']) ? 0 : NULL,
    );
    if (isset($field['empty_option'])) {
      $item['#empty_option'] = '- ' . $field['empty_option'] . ' -';
      $item['#empty_value'] = 0;
    }
    // Four ways to get default value...
    // 1: Based on current form state
    if (isset($settings[$fid])) {
      $item['#default_value'] = $settings[$fid];
    }
    // 2: Based on saved settings
    elseif (isset($settings['data'][$ent][$c][$table][$n][$name])) {
      $item['#default_value'] = $settings['data'][$ent][$c][$table][$n][$name];
    }
    // 3: Based on field default
    elseif (isset($field['value'])) {
      $item['#default_value'] = $field['value'];
    }
    // 4: For mandatory fields like phone type, default to the first option
    elseif (empty($field['extra']['multiple']) && !isset($field['empty_option'])) {
      $options = array_keys($options);
      $item['#default_value'] = $options[1];
    }
    if (!empty($field['extra']['multiple'])) {
      $item['#description'] = t('You may set options here and/or add this element to the webform ("user select"). If you do both, options set here will not appear on the form.');
      $item['#default_value'] = (array) $item['#default_value'];
      if (isset($settings[$fid]) && !is_array($settings[$fid])
      && isset($settings['data'][$ent][$c][$table][$n][$name])) {
        $item['#default_value'] += (array) $settings['data'][$ent][$c][$table][$n][$name];
      }
    }
  }
  // Create checkbox
  else {
    $item = array(
      '#type' => 'checkbox',
      '#title' => $field['name'],
      '#return_value' => 'create_civicrm_webform_element',
      '#default_value' => !empty($settings[$fid]),
    );
  }
  if ($d = wf_crm_aval($field, 'extra:description')) {
    $item['#description'] = strlen($d) > 75 ? substr($d, 0, 75) . '...' : $d;
  }
  if ($a = wf_crm_aval($field, 'attributes')) {
    $item['#attributes'] = $a;
  }
  return $item;
}

/**
 * Submission handler, saves CiviCRM options for a Webform node
 */
function wf_crm_configure_form_submit($form, &$form_state) {
  $button = $form_state['clicked_button']['#id'];
  $node = node_load($nid = $form_state['storage']['nid']);
  $vals = wf_crm_aval($form_state, 'storage:vals', $form_state['values']);

  if ((empty($node->webform_civicrm) && !$vals['nid']) || $button == 'edit-cancel') {
    $form_state['rebuild'] = TRUE;
    unset($form_state['storage']['msg']);
    return;
  }
  unset($form_state['storage']);

  civicrm_initialize();
  $delete_me = $enabled = $existing = wf_crm_enabled_fields($node, NULL, TRUE);
  $created = $deleted = 0;
  $fields = wf_crm_get_fields();
  $sets = wf_crm_get_fields('sets');

  // Fields to delete
  foreach ($enabled as $key => $val) {
    $val = (array) wf_crm_aval($vals, $key);
    if ((in_array('create_civicrm_webform_element', $val, TRUE) && $vals['nid'])
    || strpos($key, 'fieldset') !== FALSE) {
      unset($delete_me[$key]);
    }
  }

  // Display a confirmation before deleting fields
  if ($delete_me && $button == 'edit-submit') {
    $msg = '<p>' . t('These existing fields are no longer needed for CiviCRM processing based on your new form settings.') . '</p><ul>';
    foreach ($delete_me as $key => $id) {
      list(, $c, $ent, $n, $table, $name) = explode('_', $key, 6);
      $info = '';
      if ($ent == 'contact' || $ent == 'participant') {
        $info = '<em>' . t('Contact !num', array('!num' => $c));
      }
      if ($info && isset($sets[$table]['max_instances'])) {
        $info .= ' ' . $sets[$table]['label'] . ' ' . $n;
      }
      $info .= $info ? ':</em> ' : '';
      $msg .= '<li>' . $info . $node->webform['components'][$id]['name'] . '</li>';
    }
    $msg .= '</ul><p>' . t('Would you like them to be automatically removed from the webform? This is recommended unless you need to keep webform-results information from these fields. (They can still be deleted manually later if you choose not to remove them now.)') . '</p><p><em>' . t('Note: Deleting webform components cannot be undone, and will result in the loss of webform-results info for those elements. Data in the CiviCRM database will not be affected.') . '</em></p>';
    $form_state['storage']['msg'] = $msg;
    $form_state['storage']['vals'] = $vals;
    $form_state['rebuild'] = TRUE;
    return;
  }

  module_load_include('inc', 'webform', 'includes/webform.components');
  module_load_include('inc', 'webform_civicrm', 'contact_component');
  $form_state['redirect'] = 'node/' . $nid . '/webform';

  // Delete/disable fields
  if ($button === 'edit-delete' || ($button === 'edit-disable' && $vals['nid'])) {
    foreach ($delete_me as $id) {
      $field = $node->webform['components'][$id];
      unset($enabled[$field['form_key']]);
      ++$deleted;
      if ($button === 'edit-delete') {
        webform_component_delete($node, $field);
      }
      else {
        $field['form_key'] = 'disabled' . substr($field['form_key'], 7);
        webform_component_update($field);
      }
    }
    if ($deleted == 1) {
      $p = array('%name' => $field['name']);
      drupal_set_message($button === 'edit-delete' ? t('Deleted field: %name', $p) : t('Disabled field: %name', $p));
    }
    else {
      $p = array('!num' => $deleted);
      drupal_set_message($button === 'edit-delete' ? t('Deleted !num fields.', $p) : t('Disabled !num fields.', $p));
    }
    if ($button === 'edit-disable') {
      drupal_set_message(t('Disabled fields will still be processed as normal Webform fields, but they will not be autofilled from or saved to the CiviCRM database.'));
    }
    else {
      // Remove empty fieldsets for deleted contacts
      foreach ($enabled as $key => $id) {
        if (substr($key, -8) == 'fieldset') {
          list(, $c, $ent, $i) = explode('_', $key);
          if ($ent == 'contact' && $i == 1 && (!$vals['nid'] || $c > $vals['number_of_contacts'])) {
            if (!_wf_crm_child_components($node->nid, $id)) {
              webform_component_delete($node, $node->webform['components'][$id]);
            }
          }
        }
      }
    }
  }

  // Disable CiviCRM for this form
  if (!$vals['nid']) {
    wf_crm_disable($nid);
    drupal_set_message(t('CiviCRM processing for this form is now disabled.'));
  }

  // CiviCRM enabled
  else {
    webform_ensure_record($node);
    wf_crm_process_form_settings($vals);
    if (!$vals['toggle_message']) {
      $vals['message'] = '';
    }
    // Index disabled components
    $disabled = array();
    foreach (wf_crm_aval($node->webform, 'components', array()) as $field) {
      if (substr($field['form_key'], 0, 9) === 'disabled_') {
        $field['form_key'] = 'civicrm' . substr($field['form_key'], 8);
        $disabled[$field['form_key']] = $field;
      }
    }

    $i = 0;
    foreach ($vals as $key => $val) {
      if (substr($key, 0, 7) == 'civicrm') {
        ++$i;
        list(, $c, $ent, $n, $table, $name) = explode('_', $key, 6);
        $aval = (array) $val;
        if (is_array($val)) {
          unset($val['create_civicrm_webform_element']);
        }
        elseif ($val == 'create_civicrm_webform_element') {
          $val = '';
        }
        if (!isset($enabled[$key]) && in_array('create_civicrm_webform_element', $aval, TRUE)) {
          // Restore disabled component
          if (isset($disabled[$key])) {
            webform_component_update($disabled[$key]);
            $enabled[$key] = $disabled[$key]['cid'];
            drupal_set_message(t('Re-enabled field: %name', array('%name' => $disabled[$key]['name'])));
          }
          // Create new component
          else {
            $field = $fields[$table . '_' . $name];
            $field['nid'] = $nid;
            $field['form_key'] = $key;
            $field['weight'] = $i;
            wf_crm_component_insert($field, $enabled, $vals);
            ++$created;
          }
        }
        // Store exposed contact settings that don't create form elements
        elseif (isset($fields[$table . '_' . $name]['expose_list']) && !empty($val)) {
          $vals['data'][$ent][$c][$table][$n][$name] = $val;
        }
      }
    }
    if ($created == 1) {
      drupal_set_message(t('Created field: %name', array('%name' => $field['name'])));
    }
    elseif ($created) {
      drupal_set_message(t('Created !num new fields.', array('!num' => $created)));
    }

    // Create record
    if (empty($node->webform_civicrm)) {
      drupal_write_record('webform_civicrm_forms', $vals);
      drupal_set_message(t('CiviCRM processing for this form is now enabled.'));
    }
    // Update record
    else {
      drupal_write_record('webform_civicrm_forms', $vals, 'nid');
      drupal_set_message(t('Your CiviCRM form settings have been updated.'));
    }
    // Update existing contact fields
    foreach ($existing as $fid => $id) {
      if (substr($fid, -8) === 'existing') {
        wf_crm_update_existing_component($node->webform['components'][$id], $enabled, $vals['data']);
      }
    }
  }
  // Make sure the updates are visible to anonymous users.
  cache_clear_all();

  // Clear the entity cache.
  if (module_exists('entitycache')) {
    cache_clear_all($nid, 'cache_entity_node');
  }
}

/**
 * Alter back-end webform component edit forms.
 * Called by hook_form_alter() whenever editing a webform component.
 */
function _wf_crm_component_form_alter(&$form, $form_state) {
  $node = node_load($form['nid']['#value']);
  // Is this a CiviCRM-enabled webform?
  if (!empty($node->webform_civicrm)) {
    civicrm_initialize();
    $fid = $form['form_key']['#default_value'];
    // Is this a civicrm component?
    if (!($pieces = wf_crm_explode_key($fid))) {
      return;
    }
    list(, $i, $ent, $n, $table, $name) = $pieces;
    $fields = wf_crm_get_fields();
    // Is this component a CiviCRM field?
    if (($field = wf_crm_aval($fields, $table . '_' . $name)) || $table == 'fieldset') {
      $form['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/webform_civicrm_admin.css';
      $element = $node->webform['components'][$form['cid']['#value']];

      if (empty($form['clone']['#value']) || $name == 'fieldset') {
        // Prevent users from editing the form_key and breaking things
        $form['form_key']['#disabled'] = TRUE;
        $form['form_key']['#value'] = $fid;
        $form['form_key']['#description'] = t('Automatically set for use by CiviCRM processing.');
        // Clone an entire contact when cloning their fieldset
        if (!empty($form['clone']['#value'])) {
          $form['submit']['#value'] = t('Clone Contact');
          $new = count($node->webform_civicrm['data']['contact']) + 1;
          $form['form_key']['#value'] = implode('_', array('civicrm', $new, $ent, $n, $table, $name));
          $form['name']['#default_value'] = str_replace($i, $new, $form['name']['#default_value']);
          ++$form['position']['weight']['#default_value'];
          array_unshift($form['#submit'], 'wf_crm_contact_clone');
          if (empty($form_state['input'])) {
            drupal_set_message(t('Press the button below to clone this contact. A new CiviCRM contact will be added to the form with all the settings for contact !num. All fields from within this fieldset will be cloned (be careful, that may include non-contact !num fields).', array('!num' => $i)), 'status', FALSE);
          }
        }
      }
      elseif (empty($form_state['input'])) {
        // Clone a single CiviCRM field
        drupal_set_message(t('You are cloning a CiviCRM component. Refer to the Webform CiviCRM instructions for how to set the key if you want the new field to be processed by CiviCRM. You can also clone an entire contact by clicking the clone button by their fieldset.'), 'status', FALSE);
      }

      if ($table === 'address' && $name === 'state_province_id') {
        $form['validation']['maxlength']['#type'] = 'hidden';
        $form['validation']['maxlength']['#value'] = 5;
        $form['value']['#description'] = t('To set a default value, enter the state/province abbreviation.');
        $form['value']['#attributes']['onkeyup'] = 'this.value = this.value.toUpperCase();';
      }

      // Interface similar to options_element
      // but with the important difference that options already exist in the civi db
      // so this does not allow create/delete of options
      elseif (($element['type'] === 'hidden' || $element['type'] === 'select') && wf_crm_get_list($table, $name)) {
        $form['#attached']['js'] = array(drupal_get_path('module', 'webform_civicrm') . '/' . 'webform_civicrm_options.js');
        $options = $sort = wf_crm_field_options($element, 'component_edit', $node->webform_civicrm['data']);
        $defaults_selected = array();
        if (!empty($element['value'])) {
          foreach (explode(',', trim($element['value'])) as $v) {
            $defaults_selected[] = '_web_civi_option_selected_' . $v;
          }
        }
        // Get rid of stuff related to options_element module
        unset($form['items']);

        $form['value']['#type'] = 'hidden';
        $form['civicrm_options_fieldset'] = array(
          '#type' => 'fieldset',
          '#title' => t('Options'),
          '#theme' => 'webform_civicrm_options',
        );
        $option_keys = array();
        foreach ($options as $k => $v) {
          $option_keys['_web_civi_option_selected_' . $k] = '';
          $form['civicrm_options_fieldset']['civicrm_option_name_' . $k] = array(
            '#markup' => '<span class="civicrm-option-name">' . $v . '</span>',
          );
        }
        if ($element['type'] === 'select') {
          $form['civicrm_options_fieldset']['civicrm_live_options'] = array(
            '#type' => 'radios',
            '#options' => array(t('<strong>Static Options</strong> (fully configurable)'), t('<strong>Live Options</strong> (update automatically)')),
            '#default_value' => (int) !empty($element['extra']['civicrm_live_options']),
            '#parents' => array('extra', 'civicrm_live_options'),
          );
          $form['civicrm_options_fieldset']['intro'] = array(
            '#markup' => '<p><div class="live-options-hide">' .
              t('Drag the arrows to re-order these options. Click the "enabled" checkbox to show/remove an item from the form. Set the label as you want it to appear on the form.') .
              '</div><div class="live-options-show">' .
              t('You cannot control the presentation of live options. They will be loaded from the CiviCRM database every time the form is displayed.') .
              '</div><div>' .
              t('Check the "default" box for an option to be selected by default when a user views the form.') .
              '</div></p>',
          );
          $options_selected = wf_crm_str2array($element['extra']['items']);
          // Sort weights. Unselected options will be at the bottom.
          $option_keys = $option_selected_keys = array();
          foreach ($options_selected as $k => $v) {
            if (isset($options[$k])) {
              $option_keys['_web_civi_option_selected_' . $k] = '';
              $option_selected_keys[] = '_web_civi_option_selected_' . $k;
              unset($sort[$k]);
            }
          }
          foreach ($sort as $k => $v) {
            $option_keys['_web_civi_option_selected_' . $k] = '';
          }
          $form['extra']['items']['#type'] = 'hidden';
          $form['extra']['items']['#required'] = FALSE;
          $form['extra']['items']['#value'] = $element['extra']['items'];
          $form['extra']['options_source']['#access'] = FALSE;
          $form['civicrm_options_fieldset']['civicrm_options'] = array(
            '#type' => 'checkboxes',
            '#required' => TRUE,
            '#options' => $option_keys,
            '#default_value' => $option_selected_keys,
          );
          $w = 0;
          foreach ($option_keys as $k => $v) {
            $k = str_replace('_web_civi_option_selected_', '', $k);
            $form['civicrm_options_fieldset']['civicrm_option_label_' . $k] = array(
              '#type' => 'textfield',
              '#size' => 30,
              '#default_value' => !empty($options_selected[$k]) ? $options_selected[$k] : $options[$k],
            );
            $form['civicrm_options_fieldset']['civicrm_option_weight_' . $k] = array(
              '#type' => 'textfield',
              '#size' => 3,
              '#default_value' => ++$w,
            );
          }
        }
        $form['civicrm_options_fieldset']['civicrm_defaults'] = array(
          '#type' => 'checkboxes',
          '#options' => array('' => '') + $option_keys,
          '#default_value' => $defaults_selected,
        );
        // Auto set multi-value option for single-valued entities
        if (empty($field['extra']['multiple']) && $element['type'] === 'select') {
          $form['extra']['multiple']['#type'] = 'hidden';
          $form['extra']['multiple']['#value'] = 0;
        }
        // Restore multiple checkbox in case options_element module removed it
        elseif ($element['type'] === 'select') {
          $form['extra']['multiple']['#type'] = 'checkbox';
          $form['extra']['multiple']['#title'] = t('Multiple');
          $form['extra']['multiple']['#description'] = t('Check this option if the user should be allowed to choose multiple values.');
          $form['extra']['multiple']['#default_value'] = !empty($element['extra']['multiple']);
        }
        else {
          $form['extra']['multiple']['#type'] = 'hidden';
          $form['extra']['multiple']['#value'] = (int) !empty($field['extra']['multiple']);
        }
        array_unshift($form['#submit'], 'wf_crm_process_options_selection');
      }
      // Simplify form for hidden fields
      elseif (($name === 'contact_id' || $name === 'external_identifier') && $element['type'] === 'hidden') {
        $form['value']['#value'] = '';
        $form['value']['#type'] = 'textfield';
        $form['value']['#disabled'] = TRUE;
        $form['#prefix'] = '<p>'
        . t('There are no configuration options for this hidden field. You can use it for post processing, for example to include a link to the CiviCRM contact in an email.')
        . '</p><p>'
        . t('The webform token for this field is:')
        . '<br /><strong>' . wf_crm_component_token($node, $form['cid']['#value'])
        . '</strong><br /><em>'
        . t('Note: this token will change if you move the field in or out of a fieldset.')
        . '</em></p>';
      }
      // Customize form for checksum element
      elseif ($name == 'cs') {
        $form['value']['#title'] = t('Checksum Lifespan');
        $form['value']['#required'] = TRUE;
        $form['value']['#type'] = 'textfield';
        $form['value']['#description'] = t('Enter the number of days for which this checksum will be valid. Enter 0 to never expire.');
        $form['#validate'][] = 'wf_crm_cs_validate';
        $enabled = wf_crm_enabled_fields($node);
        $form['#prefix'] = '<p>'
        . t('This field will generate a checksum for contact !num after the form is submitted. You could use its webform token to create a personalized link for anonymous users.', array('!num' => $i))
        . '</p><p>';
        if ($cid_field = wf_crm_aval($enabled, 'civicrm_' . $i . '_contact_1_contact_contact_id')) {
          $form['#prefix'] .= t('Example: to link users back to this form, place this line in a webform-generated email:')
          . '<br /><code>'
          . url('node/' . $node->nid, array('absolute' => TRUE)) . '?cid' . $i . '=';
          $form['#prefix'] .= wf_crm_component_token($node, $cid_field)
          . '&amp;cs' . $i . '=' . wf_crm_component_token($node, $form['cid']['#value'])
          . '</code></p>';
          $form['#prefix'] .= '<p>' . t('Example: to redirect to a contribution page, paste this into the redirect field on the Webform "Form Settings" tab (change id from 1 to the real id of your contribution page):')
          . '<br /><code>'
          . 'civicrm/contribute/transact?reset=1&amp;id=1'
          . '&amp;cid=' . wf_crm_component_token($node, $cid_field)
          . '&amp;cs=' . wf_crm_component_token($node, $form['cid']['#value'])
          . '</code></p>';
        }
        else {
          $form['#prefix'] .= t('Consider enabling the hidden contact_id field - hashed links usually require this value.') . '</p>';
        }
      }
      // Alter weights for contact component
      if ($form['type']['#value'] == 'civicrm_contact') {
        $form['display']['#weight'] = -1;
      }
      // Allow widget to be altered
      elseif ($field && $field['type'] !== 'civicrm_contact' && empty($_GET['type'])) {
        $widgets = array();
        foreach (webform_components() as $k => $v) {
          if ($k !== 'civicrm_contact' && $k !== 'pagebreak' && $k !== 'markup' && empty($v['features']['group'])) {
            $widgets[$k] = $v['label'];
          }
        }
        if ($form['type']['#value'] == 'textfield' && $table === 'address' && $name === 'state_province_id') {
          $widgets['textfield'] = t('Textfield / AJAX Select');
        }
        $form['widget'] = array(
          '#type' => 'fieldset',
          '#title' => t('Widget: %type', array('%type' => wf_crm_aval($widgets, $form['type']['#value'], t('Unknown')))),
          '#description' => t('The default widget for this %type field is %widget. You may change it if you wish - for example, a hidden field allows you to set the value without exposing this field to the form.<br />Note: Not all widgets may be compatible with this CiviCRM field - test your form after changing widgets.', array('%type' => str_replace('#', '', $field['name']), '%widget' => wf_crm_aval($widgets, $field['type'], t('Disabled')))),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );
        $form['widget']['widget'] = array(
          '#type' => 'select',
          '#options' => $widgets,
          '#default_value' => $form['type']['#value'],
        );
        $form['widget']['change_widget'] = array(
          '#type' => 'submit',
          '#value' => t('Change Widget'),
          '#submit' => array('webform_component_edit_form_submit', 'wf_crm_change_widget'),
          '#states' => array(
            'invisible' => array(
              'select[name="widget[widget]"]' => array('value' => $form['type']['#value']),
            ),
            'disabled' => array(
              'select[name="widget[widget]"]' => array('value' => $form['type']['#value']),
            ),
          )
        );
      }
    }
  }
}

/**
 * Add a CiviCRM field to a webform
 *
 * @param $field: array
 *   Webform field info
 * @param $enabled: array
 *   Array of enabled fields (reference)
 * @param $settings
 *   webform_civicrm configuration for this form
 */
function wf_crm_component_insert(&$field, &$enabled, $settings) {
  list(, $c, $ent, $n, $table, $name) = explode('_', $field['form_key'], 6);
  $contact_type = $settings['data']['contact'][$c]['contact'][1]['contact_type'];
  // Replace the # token with set number (or append to the end if no token)
  if ($n > 1 || $table == 'relationship') {
    if (strpos($field['name'], '#') === FALSE) {
      $field['name'] .= " $n";
    }
    else {
      $field['name'] = str_replace('#', $n, $field['name']);
    }
  }
  else {
    $field['name'] = str_replace(' #', '', $field['name']);
  }
  if ($name == 'contact_sub_type') {
    list($contact_types, $sub_types) = wf_crm_get_contact_types();
    $field['name'] .= ' ' . $contact_types[$contact_type];
  }
  // Defaults for existing contact field
  if ($name == 'existing') {
    $vals = $enabled + $settings;
    // Set the allow_create flag based on presence of name or email fields
    $field['extra']['allow_create'] = $a = wf_crm_name_field_exists($vals, $c, $contact_type);
    $field['extra']['none_prompt'] = $a ? t('+ Create new contact +') : t('- None Found -');
    if ($c == 1 && $contact_type == 'individual') {
      // Default to hidden field for 1st contact
      $field['extra'] += array(
        'widget' => 'hidden',
        'default' => 'user',
      );
    }
  }
  // A width of 20 is more sensible than Drupal's default of 60
  if (($field['type'] == 'textfield' || $field['type'] == 'email') && empty($field['extra']['width'])) {
    $field['extra']['width'] = 20;
  }
  // Default checksum lifespan
  if ($name == 'cs') {
    require_once 'CRM/Core/BAO/Setting.php';
    $field['value'] = CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME,
      'checksum_timeout', NULL, 7);
  }
  // Support html_textarea module
  if ($field['type'] == 'html_textarea') {
    $field['value']['format'] = filter_default_format();
    $field['value']['value'] = '';
  }
  // Retrieve option list
  if ($options = wf_crm_field_options($field, 'component_insert', $settings['data'])) {
    $field['extra']['items'] = $options;
    $list = wf_crm_get_list($table, $name);
    if (!isset($field['extra']['aslist']) && empty($field['extra']['multiple'])) {
      $field['extra']['aslist'] = 1;
    }
    if (is_numeric($list)) {
      // Lookup defaults for custom fields
      $dao = &CRM_Core_DAO::executeQuery('SELECT value FROM civicrm_option_value
        WHERE is_default AND is_active AND option_group_id = ' . $list);
      $field['value'] = '';
      while ($dao->fetch()) {
        $field['value'] .= ($field['value'] ? ',' : '') . $dao->value;
      }
    }
  }
  if ($table == 'other' || $field['type'] == 'hidden') {
    unset($field['extra']['description']);
  }
  // Create fieldsets - for contact and contact-specific event fields only
  if (!empty($settings['create_fieldsets']) && $ent != 'activity' && $ent != 'case'
  && ($ent != 'participant' || wf_crm_aval($settings['data'], 'participant_reg_type') == 'separate')) {
    $sid = 'civicrm_' . $c . '_contact_1_fieldset_fieldset';
    if (!isset($enabled[$sid])) {
      // Create webform fieldset for this contact
      $new_set = array(
        'nid' => $field['nid'],
        'form_key' => $sid,
        'type' => 'fieldset',
        'name' => t('Contact !num', array('!num' => $c)),
        'weight' => $c,
      );
      $new_set += webform_component_invoke('fieldset', 'defaults');
      $enabled[$sid] = webform_component_insert($new_set);
    }
    $field['pid'] = $enabled[$sid];
  }
  // Merge defaults and create webform component
  $field += array('extra' => array());
  if ($defaults = webform_component_invoke($field['type'], 'defaults')) {
    $field += $defaults;
  }
  if (isset($enabled[$field['form_key']])) {
    $field['cid'] = $enabled[$field['form_key']];
    webform_component_update($field);
  }
  else {
    $enabled[$field['form_key']] = webform_component_insert($field);
  }
}

/**
 * Drupal theme callback
 * Format civicrm options form as a table
 */
function theme_webform_civicrm_options($variables) {
  $element = $variables['element'];
  $element['civicrm_defaults']['']['#attributes']['class'][] = 'select-all-civi-defaults';
  $default_box = drupal_render($element['civicrm_defaults']['']);
  $select_box = '<input class="select-all-civi-options" type="checkbox" checked="checked" title="' . t('Select All') . '"> ';
  $table = array(
    'rows' => array(),
    'attributes' => array('id' => 'civicrm-options-table'),
    'sticky' => FALSE,
  );
  if (empty($element['civicrm_options'])) {
    $table['header'] = array(t('Item'), $default_box . t('Selected'));
  }
  else {
    $table['header'] = array(
      t('Item'),
      t('Weight'),
      array('data' => $select_box . t('Enabled'), 'class' => array('live-options-hide')),
      array('data' => t('Label'), 'class' => array('live-options-hide')),
      $default_box . t('Default'),
    );
    drupal_add_tabledrag('civicrm-options-table', 'order', 'self', 'civicrm-option-weight');
  }
  foreach (element_children($element['civicrm_defaults']) as $k) {
    if ($k) {
      $v = str_replace('_web_civi_option_selected_', '', $k);
      $row = array(drupal_render($element['civicrm_option_name_' . $v]));
      if (!empty($element['civicrm_options'])) {
        $element['civicrm_option_weight_' . $v]['#attributes']['class'] = array('civicrm-option-weight');
        $element['civicrm_options'][$k]['#attributes']['class'] = array('civicrm-enabled');
        $element['civicrm_option_label_' . $v]['#attributes']['class'] = array('civicrm-label');
        $row[] = drupal_render($element['civicrm_option_weight_' . $v]);
        $row[] = array('data' => drupal_render($element['civicrm_options'][$k]), 'class' => array('live-options-hide'));
        $row[] = array('data' => drupal_render($element['civicrm_option_label_' . $v]), 'class' => array('live-options-hide'));
      }
      $element['civicrm_defaults'][$k]['#attributes']['class'] = array('civicrm-default');
      $row[] = drupal_render($element['civicrm_defaults'][$k]);
      $table['rows'][] = array(
        'data' => $row,
        'class' => array('draggable'),
      );
    }
  }
  return drupal_render_children($element) . theme('table', $table);
}

/**
 * Drupal FAPI form submit callback
 * Custom Processing for CiviCRM webform component option lists
 */
function wf_crm_process_options_selection($form, &$form_state) {
  $vals = &$form_state['values'];
  $vals['value'] = '';
  foreach ($vals['civicrm_options_fieldset']['civicrm_defaults'] as $k) {
    if ($k) {
      $vals['value'] .= ($vals['value'] ? ',' : '') . str_replace('_web_civi_option_selected_', '', $k);
    }
  }
  if (isset($vals['civicrm_options_fieldset']['civicrm_options'])) {
    if (empty($vals['extra']['civicrm_live_options'])) {
      $items = array();
      foreach ($vals['civicrm_options_fieldset']['civicrm_options'] as $k) {
        if ($k) {
          $v = str_replace('_web_civi_option_selected_', '', $k);
          if (!($label = $vals['civicrm_options_fieldset']['civicrm_option_label_' . $v])) {
            $label = $form['civicrm_options_fieldset']['civicrm_option_name_' . $v]['#value'];
          }
          $items[$vals['civicrm_options_fieldset']['civicrm_option_weight_' . $v]] = $v . '|' . $label;
        }
      }
      ksort($items);
      $vals['extra']['items'] = implode("\n", $items);

      // A single radio should be shown as a checkbox
      if (count($items) == 1 && empty($vals['extra']['aslist'])) {
        $vals['extra']['multiple'] = 1;
      }
    }
    else {
      civicrm_initialize();
      $node = node_load($vals['nid']);
      $items = wf_crm_field_options($vals, 'live_options', $node->webform_civicrm['data']);
      $items += wf_crm_str2array($vals['extra']['items']);
      $vals['extra']['items'] = wf_crm_array2str($items);
    }
  }
}

/**
 * Build select all/none js links for a fieldset
 */
function wf_crm_js_select($name) {
  return array('#markup' =>
    '<div class="web-civi-js-select">
      <a href="javascript:wfCiviAdmin.selectReset(' . "'all', '#$name'" . ')">' . t('Select All') . '</a> |
      <a href="javascript:wfCiviAdmin.selectReset(' . "'none', '#$name'" . ')">' . t('Select None') . '</a> |
      <a href="javascript:wfCiviAdmin.selectReset(' . "'reset', '#$name'" . ')">' . t('Restore') . '</a>
    </div>',
  );
}

/**
 * Look-up the webform token for a field
 */
function wf_crm_component_token($node, $cid) {
  module_load_include('inc', 'webform', 'includes/webform.components');
  $component = $node->webform['components'][$cid];
  $parents = webform_component_parent_keys($node, $component);
  if (wf_crm_webform_version() == 3) {
    return '%value[' . implode('][', $parents) . ']';
  }
  return '[submission:values:' . implode(':', $parents) . ':nolabel]';
}

/**
 * Validate checksum lifespan
 */
function wf_crm_cs_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['value']) || $form_state['values']['value'] < 0) {
    form_error($form['value'], t('Please enter a valid number of days.'));
  }
}

/**
 * Add an ajax container to a form, and set an existing form element to control it
 *
 * @param $form
 *   The entire form
 * @param $pathstr
 *   A : separated string of nested array keys leading to the relevant form snippet
 * @param $control_element: str
 *   Array key of the existing element to add ajax behavior to
 * @param $container: str
 *   Array key of the container to be created
 */
function wf_crm_ajax_item(&$form, $pathstr, $control_element, $container) {
  eval('$snippet = &$form[\'' . str_replace(':', "']['", $pathstr) . "'];");
  $pathstr .= ':' . $container;
  $id = 'civicrm-ajax-' . str_replace(array(':', '_'), '-', $pathstr);
  $snippet[$control_element]['#ajax'] = array(
    'callback' => 'wf_crm_configure_form_ajax',
    'pathstr' => $pathstr,
    'wrapper' => $id,
    'effect' => 'fade',
  );
  $snippet[$container] = array(
    '#prefix' => '<div class="civicrm-ajax-wrapper" id="' . $id . '">',
    '#type' => 'markup',
    '#suffix' => '</div>',
  );
}

/**
 * Delete civicrm settings for a webform.
 *
 * @param $nid
 *   Webform node id
 */
function wf_crm_disable($nid) {
  db_delete('webform_civicrm_forms')
    ->condition('nid', $nid)
    ->execute();
}

/**
 * Drupal FAPI form submit callback
 * Alter a webform component type.
 */
function wf_crm_change_widget($form, &$form_state) {
  unset($_SESSION['messages']['status']);
  drupal_set_message(t('Click "Save component" to change this field to %type (or go back to cancel). Test your form to ensure that the new widget works with this CiviCRM field.', array('%type' => $form['widget']['widget']['#options'][$form_state['values']['widget']['widget']])));
  drupal_goto($_GET['q'], array('query' => array('type' => $form_state['values']['widget']['widget'])));
}

/**
 * Theme override for webform components form.
 *
 * @param $variables
 *   Includes the form array.
 * @return
 *   Formatted HTML form, ready for display.
 */
function theme_webform_civicrm_components_form($variables) {
  $form = $variables['form'];

  $form['components']['#attached']['library'][] = array('webform', 'admin');

  // TODO: Attach these. See http://drupal.org/node/732022.
  drupal_add_tabledrag('webform-components', 'order', 'sibling', 'webform-weight');
  drupal_add_tabledrag('webform-components', 'match', 'parent', 'webform-pid', 'webform-pid', 'webform-cid');

  $node = $form['#node'];

  // Prepare for CiviCRM processing
  $enabled = array();
  unset($form['add']['type']['#options']['civicrm_contact']);
  if (!empty($node->webform_civicrm)) {
    civicrm_initialize();
    $enabled = wf_crm_enabled_fields($node);
    $form['components']['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/webform_civicrm_admin.css';
  }

  $header = array(t('Label'), t('Type'), t('Value'), t('Mandatory'), t('Weight'), array('data' => t('Operations'), 'colspan' => 3));
  $rows = array();

  // Add a row containing form elements for a new item.
  unset($form['add']['name']['#title'], $form['add_type']['#description']);
  $form['add']['name']['#attributes']['rel'] = t('New component name');
  $form['add']['name']['#attributes']['class'] = array('webform-default-value');
  $form['add']['cid']['#attributes']['class'] = array('webform-cid');
  $form['add']['pid']['#attributes']['class'] = array('webform-pid');
  $form['add']['weight']['#attributes']['class'] = array('webform-weight');

  $row_data = array(
    drupal_render($form['add']['name']),
    drupal_render($form['add']['type']),
    '',
    drupal_render($form['add']['mandatory']),
    drupal_render($form['add']['cid']) . drupal_render($form['add']['pid']) . drupal_render($form['add']['weight']),
    array('colspan' => 3, 'data' => drupal_render($form['add']['add'])),
  );
  $add_form = array('data' => $row_data, 'class' => array('draggable', 'webform-add-form'));
  $form_rendered = FALSE;

  if (!empty($node->webform['components'])) {
    $component_tree = array();
    $page_count = 1;
    _webform_components_tree_build($node->webform['components'], $component_tree, 0, $page_count);
    $component_tree = _webform_components_tree_sort($component_tree);
    // Build the table rows.
    function _webform_components_form_rows($node, $cid, $component, $level, &$form, &$rows, &$add_form, $enabled) {

      // CiviCRM additions
      $clone = t('Clone');
      $options = array();
      if ($component['type'] == 'civicrm_contact') {
        $types = array(
          'autocomplete' => t('Contact - Autocomplete'),
          'select' => t('Contact - Select List'),
          'hidden' => $component['extra']['show_hidden_contact'] ? t('Contact - Static') : t('Contact - Hidden'),
        );
        $type = $types[$component['extra']['widget']];
      }
      elseif ($component['type'] == 'select') {
        if ($component['extra']['aslist']) {
          $type = $component['extra']['multiple'] ? t('Multi-select') : t('Select');
        }
        else {
          $type = $component['extra']['multiple'] ? t('Checkboxes') : t('Radio buttons');
        }
      }
      else {
        $type = $form['add']['type']['#options'][$component['type']];
      }
      if (in_array($cid, $enabled)) {
        $fields = wf_crm_get_fields();
        $sets = wf_crm_get_fields('sets');
        $class = 'civi-icon';
        $title = $clone = '';
        list( , $c, $ent, $n, $table, $name) = explode('_', $component['form_key'], 6);
        $field = wf_crm_aval($fields, $table . '_' . $name, array('type' => 'fieldset'));
        if ($component['type'] == 'fieldset') {
          $title = t('Container for Contact !num', array('!num' => $c));
          $type = t('Contact !num Fieldset', array('!num' => $c));
          $clone = t('Clone Contact');
          $options['attributes']['title'] = t('Add a new contact to the form with the same fields and settings');
          $class .= ' fieldset';
        }
        elseif ($ent == 'contact') {
          $field_type = ($table == 'contact' || $table == 'other') ? $field['name'] : $sets[$table]['label'];
          $title = t('Contact !num !type Field', array('!num' => $c, '!type' => $field_type));
        }
        else {
          $title = t('Field for !type', array('!type' => $sets[$table]['label']));
        }
        if ($table === 'address' && $name === 'state_province_id' && $component['type'] === 'textfield') {
          $type = t('AJAX Select');
        }
        if ($component['type'] == 'civicrm_contact' || $component['type'] == 'fieldset') {
          $class .= ' ' . $node->webform_civicrm['data']['contact'][$c]['contact'][1]['contact_type'];
        }
        if ($component['type'] == 'select') {
          $type .= ' (' . (empty($component['extra']['civicrm_live_options']) ? t('static') : t('live')) . ')';
        }
        // Show defaults with labels instead of keys
        if ($component['type'] == 'civicrm_contact') {
          if ($component['extra']['default'] == 'contact_id') {
            $component['value'] = check_plain(wf_crm_display_name($component['extra']['default_contact_id']));
          }
          elseif ($component['extra']['default'] == 'user') {
            $component['value'] = t('Current User');
          }
          elseif ($component['extra']['default'] == 'auto') {
            $component['value'] = t('Auto - From Filters');
          }
          elseif ($component['extra']['default'] == 'relationship' && $component['extra']['default_relationship']) {
            $component['value'] = t('Relationship to Contact 1');
          }
        }
        elseif ($component['value'] && $field['type'] == 'select') {
          if ($component['type'] == 'select') {
            $items = wf_crm_str2array($component['extra']['items']);
          }
          else {
            $items = wf_crm_field_options($component, 'components_form', $node->webform_civicrm['data']);
          }
          $val = '';
          foreach (explode(',', $component['value']) as $v) {
            if (isset($items[trim($v)])) {
              $val .= ($val ? ', ' : '') . $items[trim($v)];
            }
          }
          $component['value'] = $val;
        }
        $type = array('data' => '<span class="' . $class . '"> </span>' . $type, 'title' => $title);
      }

      // Create presentable values.
      if (drupal_strlen($component['value']) > 30) {
        $component['value'] = drupal_substr($component['value'], 0, 30);
        $component['value'] .= '...';
      }
      $component['value'] = check_plain($component['value']);

      // Remove individual titles from the mandatory and weight fields.
      unset($form['components'][$cid]['mandatory']['#title']);
      unset($form['components'][$cid]['pid']['#title']);
      unset($form['components'][$cid]['weight']['#title']);

      // Add special classes for weight and parent fields.
      $form['components'][$cid]['cid']['#attributes']['class'] = array('webform-cid');
      $form['components'][$cid]['pid']['#attributes']['class'] = array('webform-pid');
      $form['components'][$cid]['weight']['#attributes']['class'] = array('webform-weight');

      // Build indentation for this row.
      $indents = '';
      for ($n = 1; $n <= $level; $n++) {
        $indents .= '<div class="indentation">&nbsp;</div>';
      }

      // Add each component to a table row.
      $row_data = array(
        $indents . filter_xss($component['name']),
        $type,
        ($component['value'] == '') ? '-' : $component['value'],
        drupal_render($form['components'][$cid]['mandatory']),
        drupal_render($form['components'][$cid]['cid']) . drupal_render($form['components'][$cid]['pid']) . drupal_render($form['components'][$cid]['weight']),
        l(t('Edit'), 'node/' . $node->nid . '/webform/components/' . $cid),
        $clone ? l($clone, 'node/' . $node->nid . '/webform/components/' . $cid . '/clone', $options) : ' ',
        l(t('Delete'), 'node/' . $node->nid . '/webform/components/' . $cid . '/delete'),
      );
      $row_class = array('draggable');
      if (!webform_component_feature($component['type'], 'group')) {
        $row_class[] = 'tabledrag-leaf';
      }
      if ($component['type'] == 'pagebreak') {
        $row_class[] = 'tabledrag-root';
        $row_class[] = 'webform-pagebreak';
        $row_data[0] = array('class' => array('webform-pagebreak'), 'data' => $row_data[0]);
      }
      $rows[] = array('data' => $row_data, 'class' => $row_class);
      if (isset($component['children']) && is_array($component['children'])) {
        foreach ($component['children'] as $cid => $component) {
          _webform_components_form_rows($node, $cid, $component, $level + 1, $form, $rows, $add_form, $enabled);
        }
      }

      // Add the add form if this was the last edited component.
      if (isset($_GET['cid']) && $component['cid'] == $_GET['cid'] && $add_form) {
        $add_form['data'][0] = $indents . $add_form['data'][0];
        $rows[] = $add_form;
        $add_form = FALSE;
      }
    }
    foreach ($component_tree['children'] as $cid => $component) {
      _webform_components_form_rows($node, $cid, $component, 0, $form, $rows, $add_form, $enabled);
    }
  }
  else {
    $message = wf_crm_admin_access($node) ? t('Add a webform component below, or click the CiviCRM tab to add CRM fields.') : t('No Components, add a component below.');
    $rows[] = array(array('data' => $message, 'colspan' => 9));
  }

  // Append the add form if not already printed.
  if ($add_form) {
    $rows[] = $add_form;
  }

  $output = '';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'webform-components')));
  $output .= drupal_render_children($form);
  return $output;
}
