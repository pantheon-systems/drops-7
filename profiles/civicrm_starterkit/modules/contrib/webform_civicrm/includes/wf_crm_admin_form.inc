<?php

/**
 * @file
 * Webform CiviCRM module's admin form.
 */

class wf_crm_admin_form {
  private $form;
  private $form_state;
  private $node;
  private $fields;
  private $sets;
  private $settings;
  private $data;

  function __construct($form, &$form_state, $node) {
    module_load_include('inc', 'webform_civicrm', 'includes/utils');
    module_load_include('inc', 'webform_civicrm', 'includes/wf_crm_admin_help');
    civicrm_initialize();
    $this->form = $form;
    $this->form_state = &$form_state;
    $this->node = &$node;
    $this->fields = wf_crm_get_fields();
    $this->sets = wf_crm_get_fields('sets');
  }

  /**
   * Build admin form for civicrm tab of a webform
   * @return array
   */
  public function buildForm() {
    $this->form_state['storage']['nid'] = $this->node->nid;

    // Display confirmation message before deleting fields
    if (!empty($this->form_state['storage']['msg'])) {
      return $this->buildConfirmationForm();
    }

    // Add css & js
    $this->addResources();

    if (empty($this->form_state['values'])) {
      $this->initializeForm();
    }
    else {
      $this->rebuildForm();
    }
    // Merge in existing fields
    $existing = array_keys(wf_crm_enabled_fields($this->node, NULL, TRUE));
    $this->settings += array_fill_keys($existing, 'create_civicrm_webform_element');

    // Sort fields by set
    foreach ($this->fields as $fid => $field) {
      if (isset($field['set'])) {
        $set = $field['set'];
      }
      else {
        list($set) = explode('_', $fid, 2);
      }
      $this->sets[$set]['fields'][$fid] = $field;
    }

    // Build form fields
    $this->buildFormIntro();
    foreach ($this->data['contact'] as $n => $c) {
      $this->buildContactTab($n, $c);
    }
    $this->buildMessageTabs();

    // Component tabs
    $this->buildActivityTab();
    if (isset($this->sets['case'])) {
      $this->buildCaseTab();
    }
    if (isset($this->sets['participant'])) {
      $this->buildParticipantTab();
    }
    if (isset($this->sets['membership'])) {
      $this->buildMembershipTab();
    }
    if (isset($this->sets['contribution'])) {
      $this->buildContributionTab();
    }
    if (isset($this->sets['grant'])) {
      $this->buildGrantTab();
    }
    $this->buildOptionsTab();

    $this->form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Settings'),
    );
    return $this->form;
  }

  /**
   * Initialize form on first view
   */
  private function initializeForm() {
    $this->settings = isset($this->node->webform_civicrm) ? $this->node->webform_civicrm : $this->defaultSettings();
    $this->data = &$this->settings['data'];
    // Warn of incompatible modules
    $incompatibilities = array(
      'form_builder_webform' => t('Form builder Webform UI'),
      'webform_alt_ui' => t('Webform Alternate UI'),
    );
    foreach ($incompatibilities as $module => $label) {
      if (module_exists($module)) {
        drupal_set_message(t('The module %module is not compatible with Webform CiviCRM Integration. Please disable it before continuing.', array('%module' => $label)), 'error');
      }
    }
  }

  /**
   * On rebuilding the form
   */
  private function rebuildForm() {
    $this->settings = wf_crm_aval($this->form_state['storage'], 'vals', $this->form_state['values']);
    $this->rebuildData();
    // Hack for nicer UX: pre-check phone, email, etc when user increments them
    if (!empty($_POST['_triggering_element_name'])) {
      foreach (array('phone' => 'phone', 'email' => 'email', 'website' => 'url', 'im' => 'name') as $ent => $field) {
        if (strpos($_POST['_triggering_element_name'], "_number_of_$ent")) {
          list(, $c) = explode('_', $_POST['_triggering_element_name']);
          for ($n = 1; $n <= $this->data['contact'][$c]["number_of_$ent"]; ++$n) {
            $this->settings["civicrm_{$c}_contact_{$n}_{$ent}_{$field}"] = 1;
          }
        }
      }
    }
    unset($this->form_state['storage']['vals']);
  }

  /**
   * Display confirmation message and buttons before deleting webform components
   * @return array
   */
  private function buildConfirmationForm() {
    $this->form['#prefix'] = $this->form_state['storage']['msg'];
    $this->form['cancel'] = $this->form['disable'] = $this->form['delete'] = array('#type' => 'submit');
    $this->form['delete']['#value'] = t('Remove Fields and Save Settings');
    $this->form['disable']['#value'] = t('Leave Fields and Save Settings');
    $this->form['cancel']['#value'] = t('Cancel (go back)');
    return $this->form;
  }

  /**
   * Add necessary css & js
   */
  private function addResources() {
    $this->form['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/css/webform_civicrm_admin.css';
    $this->form['#attached']['js'][] = drupal_get_path('module', 'webform_civicrm') . '/js/webform_civicrm_admin.js';
    $this->form['#attached']['js'][] = array(
      'data' => array('webform_civicrm' => array('rTypes' => wf_crm_get_relationship_types())),
      'type' => 'setting',
    );
    // Add CiviCRM core css & js, which includes jQuery, jQuery UI + other plugins
    CRM_Core_Resources::singleton()->addCoreResources();
    // Markup needed by CRM popup notifications
    $this->form['#suffix'] = wf_crm_admin_help::helpTemplate();
  }

  /**
   * Build fields for form intro
   */
  private function buildFormIntro() {
    $this->form['nid'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable CiviCRM Processing'),
      '#default_value' => !empty($this->settings['nid']),
      '#return_value' => $this->node->nid,
    );
    $this->help($this->form['nid'], 'intro', t('Webform-CiviCRM Integration'));
    $this->form['number_of_contacts'] = array(
      '#type' => 'select',
      '#title' => t('Number of Contacts'),
      '#default_value' => count($this->data['contact']),
      '#options' => drupal_map_assoc(range(1, 30)),
    );
    $this->form['change_form_settings'] = array(
      '#type' => 'button',
      '#value' => t('Change Form Settings'),
      '#prefix' => '<div id="no-js-button-wrapper" class="messages warning">',
      '#suffix' => '<div>' . t('You have Javascript disabled. You will need to click this button after changing any option to see the result.') . '</div></div>',
    );
    $this->form['webform_civicrm'] = array('#type' => 'vertical_tabs');
    // Display intro help to virgins
    if (!variable_get('webform_civicrm_help_seen')) {
      variable_set('webform_civicrm_help_seen', TRUE);
      drupal_add_js('cj(function() {CRM.help("' . t('Welcome to Webform-CiviCRM') . '", {}, "' . url('webform-civicrm/help/intro') . '");});', array('type' => 'inline', 'scope' => 'footer'));
    }
  }

  /**
   * Build fields for a contact
   * @param int $n Contact number
   * @param array $c Contact info
   */
  private function buildContactTab($n, $c) {
    list($contact_types, $sub_types) = wf_crm_get_contact_types();
    $this->form['contact_' . $n] = array(
      '#type' => 'fieldset',
      '#title' => $n . '. ' . wf_crm_contact_label($n, $this->data),
      '#description' => $n > 1 ? NULL : t('Primary contact. Usually assumed to be the person filling out the form.') . '<br />' . t('Enable the "Existing Contact" field to autofill with the current user (or another contact).'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('contact-icon-' . $c['contact'][1]['contact_type'])),
    );
    $this->form['contact_' . $n][$n . '_contact_type'] = array(
      '#type' => 'select',
      '#title' => t('Contact Type'),
      '#default_value' => $c['contact'][1]['contact_type'],
      '#options' => $contact_types,
      '#prefix' => '<div class="contact-type-select">',
    );
    $this->form['contact_' . $n][$n . '_webform_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => wf_crm_contact_label($n, $this->data, 'plain'),
      '#suffix' => '</div>',
    );
    $this->help($this->form['contact_' . $n][$n . '_webform_label'], 'webform_label', t('Contact Label'));
    $this->addAjaxItem('contact_' . $n, $n . '_contact_type', 'contact_subtype_wrapper', 'contact-subtype-wrapper');

    // Contact sub-type
    $fid = 'civicrm_' . $n . '_contact_1_contact_contact_sub_type';
    $subTypeIsUserSelect = FALSE;
    if (!empty($sub_types[$c['contact'][1]['contact_type']])) {
      $field = $this->fields['contact_contact_sub_type'];
      $field['name'] = t('Type of @contact', array('@contact' => $contact_types[$c['contact'][1]['contact_type']]));
      $this->form['contact_' . $n]['contact_subtype_wrapper'][$fid] = $subTypeField = $this->addItem($fid, $field);
      $subTypeIsUserSelect = in_array('create_civicrm_webform_element', $subTypeField['#default_value']);
      $this->addAjaxItem('contact_' . $n . ':contact_subtype_wrapper', $fid, 'contact_custom_wrapper');
    }
    // If no sub-types
    else {
      $this->form['contact_' . $n]['contact_subtype_wrapper'][$fid] = array(
        '#type' => 'value',
        '#value' => array(),
      );
    }

    $this->form['contact_' . $n]['contact_subtype_wrapper']['clear'] = array(
      '#markup' => '<div class="clearfix"> </div>',
    );

    foreach ($this->sets as $sid => $set) {
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
        if (!$subTypeIsUserSelect && !array_intersect($c['contact'][1]['contact_sub_type'], $set['sub_types'])) {
          continue;
        }
        $pos = &$this->form['contact_' . $n]['contact_subtype_wrapper']['contact_custom_wrapper'];
        $path = 'contact_' . $n . ':contact_subtype_wrapper:contact_custom_wrapper';
      }
      elseif (!empty($set['contact_type']) || $sid == 'contact') {
        $pos = &$this->form['contact_' . $n]['contact_subtype_wrapper'];
        $path = 'contact_' . $n . ':contact_subtype_wrapper';
      }
      else {
        $pos = &$this->form['contact_' . $n];
        $path = 'contact_' . $n;
      }
      if (!empty($set['max_instances'])) {
        if (!isset($c['number_of_' . $sid])) {
          $c['number_of_' . $sid] = 0;
        }
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
        if (!empty($set['help_text'])) {
          $this->help($selector, "fieldset_$sid", $set['label']);
        }
        $pos['contact_' . $n . '_number_of_' . $sid] = $selector;
        $this->addAjaxItem($path, 'contact_' . $n . '_number_of_' . $sid, $n . $sid . '_wrapper');
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
          'js_select' => $this->addToggle($fsid),
        );
        if ($sid == 'relationship') {
          $fieldset['#title'] = t('Relationship to !contact', array('!contact' => wf_crm_contact_label($i, $this->data, 'wrap')));
        }
        elseif ((isset($set['max_instances']) && $set['max_instances'] > 1)) {
          $fieldset['#title'] .= ' ' . $i;
          if (in_array($sid, wf_crm_location_fields()) && $i == 1) {
            $fieldset['#title'] .= ' ' . t('(primary)');
          }
        }
        foreach ($set['fields'] as $fid => $field) {
          if ($fid == 'contact_contact_sub_type' ||
            ($fid == 'address_master_id' && count($this->data['contact']) == 1) ||
            (isset($field['contact_type']) && $field['contact_type'] != $c['contact'][1]['contact_type'])) {
            continue;
          }
          $fid = 'civicrm_' . $n . '_contact_' . $i . '_' . $fid;
          $fieldset[$fid] = $this->addItem($fid, $field);
        }
        if (isset($set['max_instances'])) {
          $pos[$n . $sid . '_wrapper'][$n . $sid . $i . '_fieldset'] = $fieldset;
        }
        else {
          $pos[$n . $sid . $i . '_fieldset'] = $fieldset;
        }
      }
      if ($sid == 'contact') {
        // Matching rule
        $rule_field = $this->form['contact_' . $n]['contact_subtype_wrapper']["contact_{$n}_settings_matching_rule"] = array(
          '#type' => 'select',
          '#options' => array(
              0 =>t('- None -'),
              'Unsupervised' => t('Default Unsupervised'),
              'Supervised' => t('Default Supervised'),
            ) + wf_crm_get_matching_rules($c['contact'][1]['contact_type']),
          '#title' => t('Matching Rule'),
          '#prefix' => '<div class="number-of">',
          '#suffix' => '</div>',
          '#default_value' => wf_crm_aval($this->data['contact'][$n], 'matching_rule', 'Unsupervised', TRUE),
        );
        $rule_field =& $this->form['contact_' . $n]['contact_subtype_wrapper']["contact_{$n}_settings_matching_rule"];
        // Reset to default if selected rule doesn't exist or isn't valid for this contact type
        if (!array_key_exists($rule_field['#default_value'], $rule_field['#options'])) {
          $rule_field['#default_value'] = $this->form_state['input']["contact_{$n}_settings_matching_rule"] = 'Unsupervised';
        }
        $this->help($rule_field, 'matching_rule');
      }
    }
  }

  /**
   * Configure messages
   */
  private function buildMessageTabs() {
    $tokens = '<strong>' . t('Tokens for !contact', array('!contact' => wf_crm_contact_label(1, $this->data, TRUE))) . ':</strong> [' . implode('], [', wf_crm_get_fields('tokens')) . '].';

    $this->form['prefix'] = array(
      '#type' => 'fieldset',
      '#title' => t('Introduction Text'),
      '#description' => t('This text will appear at the top of the form. You may configure separate messages for known contacts (logged in users, or users following a hashed link from civimail) and unknown (anonymous) users.'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-text')),
    );
    $this->form['prefix']['prefix_known'] = array(
      '#type' => 'textarea',
      '#title' => t('Introduction text for known contacts'),
      '#default_value' => wf_crm_aval($this->settings, 'prefix_known'),
      '#description' => $tokens,
    );
    $this->form['prefix']['prefix_unknown'] = array(
      '#type' => 'textarea',
      '#title' => t('Introduction text for unknown contacts'),
      '#default_value' => wf_crm_aval($this->settings, 'prefix_unknown'),
      '#description' => t('No tokens available for unknown contacts.'),
    );
    $this->form['st_message'] = array(
      '#type' => 'fieldset',
      '#title' => t('"Not You?" Message'),
      '#description' => t('Prompt for users who are logged in as, or following a hashed link for, someone else.'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-message')),
    );
    $this->form['st_message']['toggle_message'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display message to known contacts?'),
      '#default_value' => !empty($this->settings['message']),
    );
    $this->form['st_message']['message'] = array(
      '#type' => 'textfield',
      '#title' => t('Text (displayed as a status message)'),
      '#default_value' => wf_crm_aval($this->settings, 'message', t("You are viewing this form as [display name]. Please {click here if that's not you}.")),
      '#size' => 100,
      '#maxlength' => 255,
      '#attributes' => array('style' => 'max-width: 100%;'),
      '#description' => t('Enclose your "not you" link text in curly brackets {like this}.') . '<p>' . $tokens . '</p>',
    );
  }

  /**
   * Activity settings
   */
  private function buildActivityTab() {
    $this->form['activityTab'] = array(
      '#type' => 'fieldset',
      '#title' => t('Activities'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-activity')),
    );
    $num_acts = wf_crm_aval($this->data, "activity:number_of_activity", 0);
    $this->form['activityTab']["activity_number_of_activity"] = array(
      '#type' => 'select',
      '#title' => t('Number of Activities'),
      '#default_value' => $num_acts,
      '#options' => range(0, $this->sets['activity']['max_instances']),
      '#prefix' => '<div class="number-of">',
      '#suffix' => '</div>',
    );
    $this->addAjaxItem("activityTab", "activity_number_of_activity", "activity");
    for ($n = 1; $n <= $num_acts; ++$n) {
      $num = "activity_activity_{$n}_fieldset";
      $this->form['activityTab']['activity'][$num] = array(
        '#type' => 'fieldset',
        '#attributes' => array('class' => array('activity-wrapper')),
        '#title' => t('Activity !num', array('!num' => $n)),
      );
      $this->form['activityTab']['activity'][$num]["activity_{$n}_settings_existing_activity_status"] = array(
        '#type' => 'select',
        '#title' => t('Update Existing Activity'),
        '#options' => array('' => '- ' . t('None') . ' -') + wf_crm_apivalues('activity', 'getoptions', array('field' => 'status_id')),
        '#default_value' => wf_crm_aval($this->data, "activity:$n:existing_activity_status", array()),
        '#multiple' => TRUE,
        '#prefix' => '<div class="float-item">',
        '#suffix' => '</div>',
      );
      $this->help($this->form['activityTab']['activity'][$num]["activity_{$n}_settings_existing_activity_status"], 'existing_activity_status');
      $this->form['activityTab']['activity'][$num]["activity_{$n}_settings_details"] = array(
        '#type' => 'checkboxes',
        '#options' => array(
          'entire_result' => t('Include <em>entire</em> webform submission in activity details'),
          'view_link' => t('Include link to <em>view</em> webform submission in activity details'),
          'edit_link' => t('Include link to <em>edit</em> webform submission in activity details'),
        ),
        '#default_value' => wf_crm_aval($this->data, "activity:$n:details", array('view_link'), TRUE),
      );
      $this->form['activityTab']['activity'][$num]['wrap'] = array();
      $wrap = &$this->form['activityTab']['activity'][$num]['wrap'];
      if (isset($this->sets['case'])) {
        $case_types = wf_crm_apivalues('case', 'getoptions', array('field' => 'case_type_id'));
        if ($case_types) {
          $wrap['case']["activity_{$n}_settings_case_type_id"] = array(
            '#type' => 'select',
            '#title' => t('File On Case'),
            '#options' => array(t('- None -')) + $case_types,
            '#default_value' => $case_type = wf_crm_aval($this->data, "activity:$n:case_type_id"),
          );
          // Allow selection of webform cases
          $num_case = wf_crm_aval($this->data, "case:number_of_case", 0);
          if ($num_case) {
            $webform_cases = array();
            for ($i=1; $i<=$num_case; ++$i) {
              $webform_cases["#$i"] = t('Case !num', array('!num' => $i));
            }
            $wrap['case']["activity_{$n}_settings_case_type_id"]['#options'] = array(
              t('- None -'),
              t('This Webform') => $webform_cases,
              t('Find by Case Type') => $case_types,
            );
          }
          $this->help($wrap['case']["activity_{$n}_settings_case_type_id"], 'file_on_case');
          $this->addAjaxItem("activityTab:activity:$num:wrap:case", "activity_{$n}_settings_case_type_id", '..:..:wrap');
          if ($case_type && $case_type[0] !== '#') {
            $wrap['case']['#type'] = 'fieldset';
            $wrap['case']['#attributes'] = array('class' => array('web-civi-checkbox-set'));
            $wrap['case']['#title'] = t('File On Case');
            $wrap['case']['#description'] = t('File on existing case matching the following criteria:');
            $this->help($wrap['case'], 'file_on_case');
            $wrap['case']["activity_{$n}_settings_case_type_id"]['#title'] = t('Case Type');
            $status_options = wf_crm_apivalues('case', 'getoptions', array('field' => 'status_id'));
            $wrap['case']["activity_{$n}_settings_case_status_id"] = array(
              '#type' => 'select',
              '#title' => t('Case Status'),
              '#options' => $status_options,
              '#multiple' => TRUE,
              '#attributes' => array('class' => array('required')),
              '#default_value' => wf_crm_aval($this->data, "activity:$n:case_status_id", array_keys($status_options)),
            );
            $wrap['case']["activity_{$n}_settings_case_contact_id"] = array(
              '#type' => 'select',
              '#title' => t('Case Client'),
              '#attributes' => array('data-type' => 'ContactReference'),
              '#options' => $this->contactRefOptions(),
              '#default_value' => wf_crm_aval($this->data, "activity:$n:case_contact_id"),
            );
          }
        }
      }
      $wrap[$num . '_fields'] = array(
        '#type' => 'fieldset',
        '#title' => ts('Activity'),
        '#attributes' => array('id' => $num . '_fields', 'class' => array('web-civi-checkbox-set')),
        'js_select' => $this->addToggle($num . '_fields'),
      );
      foreach ($this->sets['activity']['fields'] as $fid => $field) {
        if ($fid != 'activity_survey_id') {
          $fid = "civicrm_{$n}_activity_1_$fid";
          $wrap[$num . '_fields'][$fid] = $this->addItem($fid, $field);
        }
      }
      $type = $wrap[$num . '_fields']["civicrm_{$n}_activity_1_activity_activity_type_id"]['#default_value'];
      $type = $type == 'create_civicrm_webform_element' ? 0 : $type;
      $this->addAjaxItem("activityTab:activity:$num:wrap:{$num}_fields", "civicrm_{$n}_activity_1_activity_activity_type_id", "..:custom");
      // Add ajax survey type field
      if (isset($this->fields['activity_survey_id'])) {
        $this->addAjaxItem("activityTab:activity:$num:wrap:{$num}_fields", "civicrm_{$n}_activity_1_activity_campaign_id", "..:custom");
        if ($type && array_key_exists($type, wf_crm_get_campaign_activity_types())) {
          $this->sets['activity_survey'] = array(
            'entity_type' => 'activity',
            'label' => $wrap[$num . '_fields']["civicrm_{$n}_activity_1_activity_activity_type_id"]['#options'][$type],
            'fields' => array(
              'activity_survey_id' => $this->fields['activity_survey_id'],
            )
          );
        }
      }
      // Add custom field sets appropriate to this activity type
      foreach ($this->sets as $sid => $set) {
        if ($set['entity_type'] == 'activity' && $sid != 'activity'
          && (!$type || empty($set['sub_types']) || in_array($type, $set['sub_types']))
        ) {
          $fs1 = "activity_activity_{$n}_fieldset_$sid";
          $wrap['custom'][$fs1] = array(
            '#type' => 'fieldset',
            '#title' => $set['label'],
            '#attributes' => array('id' => $fs1, 'class' => array('web-civi-checkbox-set')),
            'js_select' => $this->addToggle($fs1),
          );
          foreach ($set['fields'] as $fid => $field) {
            $fid = "civicrm_{$n}_activity_1_$fid";
            $wrap['custom'][$fs1][$fid] = $this->addItem($fid, $field);
          }
        }
      }
    }
  }

  /**
   * Case settings
   * FIXME: This is exactly the same code as buildGrantTab. More utilities and less boilerplate needed.
   */
  private function buildCaseTab() {
    $types = wf_crm_apivalues('case', 'getoptions', array('field' => 'case_type_id'));
    if (!$types) {
      return;
    }
    $this->form['caseTab'] = array(
      '#type' => 'fieldset',
      '#title' => t('Cases'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-case')),
    );
    $this->form['caseTab']["case_number_of_case"] = array(
      '#type' => 'select',
      '#title' => t('Number of Cases'),
      '#default_value' => $num = wf_crm_aval($this->data, "case:number_of_case", 0),
      '#options' => range(0, $this->sets['case']['max_instances']),
      '#prefix' => '<div class="number-of">',
      '#suffix' => '</div>',
    );
    $this->addAjaxItem("caseTab", "case_number_of_case", "case");
    for ($n = 1; $n <= $num; ++$n) {
      $fs = "case_case_{$n}_fieldset";
      $this->form['caseTab']['case'][$fs] = array(
        '#type' => 'fieldset',
        '#title' => t('Case !num', array('!num' => $n)),
        'wrap' => array('#weight' => 9),
      );
      $this->form['caseTab']['case'][$fs]["case_{$n}_settings_existing_case_status"] = array(
        '#type' => 'select',
        '#title' => t('Update Existing Case'),
        '#options' => array('' => '- ' . t('None') . ' -') + wf_crm_apivalues('case', 'getoptions', array('field' => 'status_id')),
        '#default_value' => wf_crm_aval($this->data, "case:{$n}:existing_case_status", array()),
        '#multiple' => TRUE,
      );
      $this->help($this->form['caseTab']['case'][$fs]["case_{$n}_settings_existing_case_status"], 'existing_case_status');
      $case_type = wf_crm_aval($this->data, "case:{$n}:case:1:case_type_id");
      foreach ($this->filterCaseSets($case_type) as $sid => $set) {
        $fs1 = "case_case_{$n}_fieldset_$sid";
        if ($sid == 'case') {
          $pos = &$this->form['caseTab']['case'][$fs];
        }
        else {
          $pos = &$this->form['caseTab']['case'][$fs]['wrap'];
        }
        $pos[$fs1] = array(
          '#type' => 'fieldset',
          '#title' => $set['label'],
          '#attributes' => array('id' => $fs1, 'class' => array('web-civi-checkbox-set')),
          'js_select' => $this->addToggle($fs1),
        );
        foreach ($set['fields'] as $fid => $field) {
          $fid = "civicrm_{$n}_case_1_$fid";
          if (!$case_type || empty($field['case_types']) || in_array($case_type, $field['case_types'])) {
            $pos[$fs1][$fid] = $this->addItem($fid, $field);
          }
        }
      }
      $this->addAjaxItem("caseTab:case:$fs:case_case_{$n}_fieldset_case", "civicrm_{$n}_case_1_case_case_type_id", "..:wrap");
    }
  }

  /**
   * Adjust case role fields to match creator/manager settings for a given case type
   *
   * @param int|null $case_type
   * @return array
   */
  private function filterCaseSets($case_type) {
    $case_sets = array();
    foreach ($this->sets as $sid => $set) {
      if ($set['entity_type'] == 'case' && (!$case_type || empty($set['sub_types']) || in_array($case_type, $set['sub_types']))) {
        if ($sid == 'caseRoles') {
          // Lookup case-role names
          $creator = $manager = NULL;
          // Use the vanilla civicrm_api for this because it will throw an error in CiviCRM 4.4 (api doesn't exist)
          $case_types = civicrm_api('case_type', 'get', array('version' => 3, 'id' => $case_type, 'options' => array('limit' => 0)));
          foreach (wf_crm_aval($case_types, 'values', array()) as $type) {
            foreach ($type['definition']['caseRoles'] as $role) {
              if (!empty($role['creator'])) {
                $creator = ($creator == $role['name'] || $creator === NULL) ? $role['name'] : FALSE;
              }
              if (!empty($role['manager'])) {
                $manager = ($manager == $role['name'] || $manager === NULL) ? $role['name'] : FALSE;
              }
            }
          }
          if ($creator) {
            $rel_type = wf_civicrm_api('relationshipType', 'getsingle', array('name_b_a' => $creator));
            $label = $creator == $manager ? ts('Case # Creator/Manager') : ts('Case # Creator');
            $set['fields']['case_creator_id']['name'] = $rel_type['label_b_a'] . ' (' . $label . ')';
            unset($set['fields']['case_role_' . $rel_type['id']]);
          }
          if ($manager && $manager != $creator) {
            $rel_type = wf_civicrm_api('relationshipType', 'getsingle', array('name_b_a' => $manager));
            $set['fields']['case_role_' . $rel_type['id']]['name'] .= ' (' . ts('Case # Manager') . ')';
          }
        }
        $case_sets[$sid] = $set;
      }
    }
    return $case_sets;
  }

  /**
   * Event participant settings
   */
  private function buildParticipantTab() {
    $this->form['participant'] = array(
      '#type' => 'fieldset',
      '#title' => t('Event Registration'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-participant')),
    );
    $reg_type = wf_crm_aval($this->data, 'participant_reg_type');
    $this->form['participant']['participant_reg_type'] = array(
      '#type' => 'select',
      '#title' => t('Registration Method'),
      '#default_value' => $reg_type,
      '#options' => array(
        t('- None -'),
        'all' => t('Register all contacts for the same event(s)'),
        'separate' => t('Register each contact separately'),
      ),
    );
    $this->help($this->form['participant']['participant_reg_type'], 'participant_reg_type');
    $this->form['participant']['event_type'] = array(
      '#type' => 'select',
      '#title' => t('Show Events of Type'),
      '#options' => array('any' => t('- Any Type -')) + wf_crm_apivalues('event', 'getoptions', array('field' => 'event_type_id')),
      '#default_value' => wf_crm_aval($this->data, 'reg_options:event_type', 'any'),
      '#prefix' => '<div id="event-reg-options-wrapper"><div class="web-civi-checkbox-set">',
      '#parents' => array('reg_options', 'event_type'),
      '#tree' => TRUE,
    );
    $this->form['participant']['show_past_events'] = array(
      '#type' => 'select',
      '#title' => t('Show Past Events'),
      '#default_value' => wf_crm_aval($this->data, 'reg_options:show_past_events', 'now'),
      '#parents' => array('reg_options', 'show_past_events'),
      '#tree' => TRUE,
      '#options' => array(
        'now' => t('- None -'),
        1 => t('All'),
        '-1 day' => t('Past Day'),
        '-1 week' => t('Past Week'),
        '-1 month' => t('Past Month'),
        '-2 month' => t('Past 2 Months'),
        '-3 month' => t('Past 3 Months'),
        '-6 month' => t('Past 6 Months'),
        '-1 year' => t('Past Year'),
        '-2 year' => t('Past 2 Years'),
      ),
    );
    $this->help($this->form['participant']['show_past_events'], 'reg_options_show_past_events');
    $this->form['participant']['show_future_events'] = array(
      '#type' => 'select',
      '#title' => t('Show Future Events'),
      '#default_value' => wf_crm_aval($this->data, 'reg_options:show_future_events', 1),
      '#parents' => array('reg_options', 'show_future_events'),
      '#tree' => TRUE,
      '#options' => array(
        'now' => t('- None -'),
        1 => t('All'),
        '+1 day' => t('Next Day'),
        '+1 week' => t('Next Week'),
        '+1 month' => t('Next Month'),
        '+2 month' => t('Next 2 Months'),
        '+3 month' => t('Next 3 Months'),
        '+6 month' => t('Next 6 Months'),
        '+1 year' => t('Next Year'),
        '+2 year' => t('Next 2 Years'),
      ),
    );
    $this->help($this->form['participant']['show_future_events'], 'reg_options_show_future_events');
    $this->form['participant']['title_display'] = array(
      '#type' => 'select',
      '#title' => t('Title Display'),
      '#default_value' => wf_crm_aval($this->data, 'reg_options:title_display', 'title'),
      '#suffix' => '</div>',
      '#parents' => array('reg_options', 'title_display'),
      '#tree' => TRUE,
      '#options' => array(
        'title' => t('Title Only'),
        'title type' => t('Title + Event Type'),
        'title start dateformatYear' => t('Title + Year'),
        'title start dateformatPartial' => t('Title + Month + Year'),
        'title start dateformatFull' => t('Title + Start-Date'),
        'title start dateformatTime' => t('Title + Start-Time'),
        'title start dateformatDatetime' => t('Title + Start-Date-Time'),
        'title start end dateformatFull' => t('Title + Start-Date + End'),
        'title start end dateformatTime' => t('Title + Start-Time + End'),
        'title start end dateformatDatetime' => t('Title + Start-Date-Time + End'),
      ),
    );
    $this->help($this->form['participant']['title_display'], 'reg_options_title_display');
    $this->form['participant']['reg_options'] = array(
      '#prefix' => '<div class="clearfix"> </div>',
      '#suffix' => '</div>',
      '#type' => 'fieldset',
      '#title' => t('Registration Options'),
      '#collapsible' => TRUE,
      '#collapsed' => isset($this->data['participant']),
      '#tree' => TRUE,
    );
    $field = array(
      '#type' => 'select',
      '#title' => t('Show Remaining Space in Events'),
      '#default_value' => wf_crm_aval($this->data, 'reg_options:show_remaining', 0),
      '#options' => array(
        t('Never'),
        'always' => t('Always'),
        '0_full' => t('When full - 0 spaces left'),
      ),
    );
    $this->help($field, 'reg_options_show_remaining');
    foreach (array(5, 10, 20, 50, 100, 200, 500, 1000) as $num) {
      $field['#options'][$num]['wrap'] = t('When under !num spaces left', array('!num' => $num));
    }
    $this->form['participant']['reg_options']['show_remaining'] = $field;
    $this->form['participant']['reg_options']['validate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Prevent Registration for Past/Full Events'),
      '#default_value' => (bool) wf_crm_aval($this->data, 'reg_options:validate'),
    );
    $this->help($this->form['participant']['reg_options']['validate'], 'reg_options_validate');
    $this->form['participant']['reg_options']['block_form'] = array(
      '#type' => 'checkbox',
      '#title' => t('Block Form Access when Event(s) are Full/Ended'),
      '#default_value' => (bool) wf_crm_aval($this->data, 'reg_options:block_form'),
    );
    $this->form['participant']['reg_options']['disable_unregister'] = array(
      '#type' => 'checkbox',
      '#title' => t('Disable unregistering participants from unselected events.'),
      '#default_value' => (bool) wf_crm_aval($this->data, 'reg_options:disable_unregister'),
    );
    $this->form['participant']['reg_options']['allow_url_load'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow events to be autoloaded from URL'),
      '#default_value' => (bool) wf_crm_aval($this->data, 'reg_options:allow_url_load'),
    );
    $this->help($this->form['participant']['reg_options']['block_form'], 'reg_options_block_form');
    $this->help($this->form['participant']['reg_options']['disable_unregister'], 'reg_options_disable_unregister');
    $this->help($this->form['participant']['reg_options']['allow_url_load'], 'reg_options_allow_url_load');
    $this->addAjaxItem('participant', 'participant_reg_type', 'participants');
    $this->addAjaxItem('participant', 'event_type', 'participants');
    $this->addAjaxItem('participant', 'show_past_events', 'participants');
    $this->addAjaxItem('participant', 'show_future_events', 'participants');
    $this->addAjaxItem('participant', 'title_display', 'participants');

    for ($n = 1; $reg_type && (($n <= count($this->data['contact']) && $reg_type != 'all') || $n == 1); ++$n) {
      $this->form['participant']['participants'][$n] = array(
        '#type' => 'fieldset',
        '#title' => $reg_type == 'all' ? t('Registration') : wf_crm_contact_label($n, $this->data, 'wrap'),
      );
      $num = wf_crm_aval($this->data, "participant:{$n}:number_of_participant");
      if (!$num || ($n > 1 && $reg_type == 'all')) {
        $num = 0;
      }
      $this->form['participant']['participants'][$n]['participant_' . $n . '_number_of_participant'] = array(
        '#type' => 'select',
        '#title' => $reg_type == 'all' ? t('Number of Event Sets') : t('Number of Event Sets for !contact', array('!contact' => wf_crm_contact_label($n, $this->data, 'wrap'))),
        '#default_value' => $num,
        '#options' => range(0, $this->sets['participant']['max_instances']),
        '#prefix' => '<div class="number-of">',
        '#suffix' => '</div>',
      );
      $this->addAjaxItem("participant:participants:{$n}", 'participant_' . $n . '_number_of_participant', 'div');
      $particpant_extensions = array(
        1 => 'role_id',
        2 => 'event_id',
        3 => 'event_type'
      );
      for ($e = 1; $e <= $num; ++$e) {
        $fs = "participant_{$n}_event_{$e}_fieldset";
        $this->form['participant']['participants'][$n]['div'][$fs] = array(
          '#type' => 'fieldset',
          '#title' => t('Event !num', array('!num' => $e)),
          '#attributes' => array('id' => $fs),
        );
        foreach ($this->sets as $sid => $set) {
          if ($set['entity_type'] == 'participant') {
            $sid = 'civicrm_' . $n . '_participant_' . $e . '_' . $sid . '_fieldset';
            $class = 'web-civi-checkbox-set';
            if (!empty($set['sub_types'])) {
              $role_id = wf_crm_aval($this->data, "participant:$n:particpant:$e:role_id", '');
              $event_id = wf_crm_aval($this->data, "participant:$n:particpant:$e:event_id", '');
              $event_type = wf_crm_aval($this->data, 'reg_options:event_type', '');
              if ($event_id && $event_id !== 'create_civicrm_webform_element') {
                list($event_id, $event_type) = explode('-', $event_id);
              }
              $ext = $particpant_extensions[$set['extension_of']];
              if (!in_array($$ext, $set['sub_types'])) {
                $class .= ' hidden';
              }
              $class .= ' extends-condition ' . str_replace('_', '', $ext) . '-' . implode('-', $set['sub_types']);
            }
            $this->form['participant']['participants'][$n]['div'][$fs][$sid] = array(
              '#type' => 'fieldset',
              '#title' => $set['label'],
              '#attributes' => array('id' => $sid, 'class' => array($class)),
              'js_select' => $this->addToggle($sid),
            );
            foreach ($set['fields'] as $fid => $field) {
              $id = 'civicrm_' . $n . '_participant_' . $e . '_' . $fid;
              $item = $this->addItem($id, $field);
              if ($fid == 'participant_event_id') {
                $item['#prefix'] = '<div class="auto-width">';
                $item['#suffix'] = '</div>';
              }
              if ($fid == 'participant_event_id' || $fid == 'participant_role_id') {
                $item['#attributes']['onchange'] = "wfCiviAdmin.participantConditional('#$fs');";
                $item['#attributes']['class'][] = $fid;
                $$fid = wf_crm_aval($item, '#default_value');
              }
              $this->form['participant']['participants'][$n]['div'][$fs][$sid][$id] = $item;
            }
          }
        }
      }
    }
  }

  /**
   * Membership settings
   */
  private function buildMembershipTab() {
    $this->form['membership'] = array(
      '#type' => 'fieldset',
      '#title' => t('Memberships'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-membership')),
    );
    for ($c = 1; $c <= count($this->data['contact']); ++$c) {
      $num = wf_crm_aval($this->data, "membership:{$c}:number_of_membership", 0);
      $this->form['membership'][$c]["membership_{$c}_number_of_membership"] = array(
        '#type' => 'select',
        '#title' => t('Number of Memberships for !contact', array('!contact' => wf_crm_contact_label($c, $this->data, 'wrap'))),
        '#default_value' => $num,
        '#options' => range(0, 9),
        '#prefix' => '<div class="number-of">',
        '#suffix' => '</div>',
      );
      $this->addAjaxItem("membership:$c", "membership_{$c}_number_of_membership", "membership");
      for ($n = 1; $n <= $num; ++$n) {
        $fs = "membership_{$c}_membership_{$n}_fieldset";
        $this->form['membership'][$c]['membership'][$fs] = array(
          '#type' => 'fieldset',
          '#title' => t('Membership !num for !contact', array('!num' => $n, '!contact' => wf_crm_contact_label($c, $this->data, 'wrap'))),
          '#attributes' => array('id' => $fs, 'class' => array('web-civi-checkbox-set')),
          'js_select' => $this->addToggle($fs),
        );
        foreach ($this->sets as $sid => $set) {
          if ($set['entity_type'] == 'membership') {
            foreach ($set['fields'] as $fid => $field) {
              $fid = "civicrm_{$c}_membership_{$n}_$fid";
              $this->form['membership'][$c]['membership'][$fs][$fid] = $this->addItem($fid, $field);
            }
          }
        }
      }
    }
  }

  /**
   * Contribution settings
   */
  private function buildContributionTab() {
    $this->form['contribution'] = array(
      '#type' => 'fieldset',
      '#title' => t('Contribution'),
      '#group' => 'webform_civicrm',
      '#description' => t('In order to process live transactions for events, memberships, or contributions, select a contribution page and its billing fields will be included on the webform.'),
      '#attributes' => array('class' => array('civi-icon-contribution')),
    );
    $fid = 'civicrm_1_contribution_1_contribution_contribution_page_id';
    $this->form['contribution'][$fid] = $this->addItem($fid, $this->fields['contribution_contribution_page_id']);
    unset($this->sets['contribution']['fields']['contribution_contribution_page_id']);
    $this->addAjaxItem('contribution', $fid, 'sets');
    $page_id = wf_crm_aval($this->data, 'contribution:1:contribution:1:contribution_page_id');
    if ($page_id) {
      $page = wf_civicrm_api('contribution_page', 'getsingle', array('id' => $page_id));
    }
    if (!$page_id || empty($page['financial_type_id'])) {
      return;
    }
    // Make sure webform is set-up to prevent credit card abuse.
    $this->checkSubmissionLimit();
    // Add contribution fields
    foreach ($this->sets as $sid => $set) {
      if ($set['entity_type'] == 'contribution' && (empty($set['sub_types']) || $set['sub_types'][0] == $page['financial_type_id'])) {
        $this->form['contribution']['sets'][$sid] = array(
          '#type' => 'fieldset',
          '#title' => $set['label'],
          '#attributes' => array('id' => $sid, 'class' => array('web-civi-checkbox-set')),
          'js_select' => $this->addToggle($sid),
        );
        foreach ($set['fields'] as $fid => $field) {
          $fid = "civicrm_1_contribution_1_$fid";
          $this->form['contribution']['sets'][$sid][$fid] = $this->addItem($fid, $field);
        }
      }
    }
  }

  /**
   * Grant settings
   * FIXME: This is nearly the same code as buildCaseTab. More utilities and less boilerplate needed.
   */
  private function buildGrantTab() {
    $types = wf_crm_apivalues('grant', 'getoptions', array('field' => 'grant_type_id'));
    if (!$types) {
      return;
    }
    $this->form['grantTab'] = array(
      '#type' => 'fieldset',
      '#title' => t('Grants'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-grant')),
    );
    $this->form['grantTab']["grant_number_of_grant"] = array(
      '#type' => 'select',
      '#title' => t('Number of Grants'),
      '#default_value' => $num = wf_crm_aval($this->data, "grant:number_of_grant", 0),
      '#options' => range(0, $this->sets['grant']['max_instances']),
      '#prefix' => '<div class="number-of">',
      '#suffix' => '</div>',
    );
    $this->addAjaxItem("grantTab", "grant_number_of_grant", "grant");
    for ($n = 1; $n <= $num; ++$n) {
      $fs = "grant_grant_{$n}_fieldset";
      $this->form['grantTab']['grant'][$fs] = array(
        '#type' => 'fieldset',
        '#title' => t('Grant !num', array('!num' => $n)),
        'wrap' => array('#weight' => 9),
      );
      $this->form['grantTab']['grant'][$fs]["grant_{$n}_settings_existing_grant_status"] = array(
        '#type' => 'select',
        '#title' => t('Update Existing Grant'),
        '#options' => array('' => '- ' . t('None') . ' -') + wf_crm_apivalues('grant', 'getoptions', array('field' => 'status_id')),
        '#default_value' => wf_crm_aval($this->data, "grant:{$n}:existing_grant_status", array()),
        '#multiple' => TRUE,
      );
      $this->help($this->form['grantTab']['grant'][$fs]["grant_{$n}_settings_existing_grant_status"], 'existing_grant_status');
      $grant_type = wf_crm_aval($this->data, "grant:{$n}:grant:1:grant_type_id");
      foreach ($this->sets as $sid => $set) {
        if ($set['entity_type'] == 'grant' && (!$grant_type || empty($set['sub_types']) || in_array($grant_type, $set['sub_types']))) {
          $fs1 = "grant_grant_{$n}_fieldset_$sid";
          if ($sid == 'grant') {
            $pos = &$this->form['grantTab']['grant'][$fs];
          }
          else {
            $pos = &$this->form['grantTab']['grant'][$fs]['wrap'];
          }
          $pos[$fs1] = array(
            '#type' => 'fieldset',
            '#title' => $set['label'],
            '#attributes' => array('id' => $fs1, 'class' => array('web-civi-checkbox-set')),
            'js_select' => $this->addToggle($fs1),
          );
          foreach ($set['fields'] as $fid => $field) {
            $fid = "civicrm_{$n}_grant_1_$fid";
            $pos[$fs1][$fid] = $this->addItem($fid, $field);
          }
        }
      }
      $this->addAjaxItem("grantTab:grant:$fs:grant_grant_{$n}_fieldset_grant", "civicrm_{$n}_grant_1_grant_grant_type_id", "..:wrap");
    }
  }

  /**
   * Configure additional options
   */
  private function buildOptionsTab() {
    $this->form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Additional Options'),
      '#group' => 'webform_civicrm',
      '#attributes' => array('class' => array('civi-icon-prefs')),
      '#description' => '<p>' .
        t('To have this form auto-filled for anonymous users, enable the "Existing Contact" field for !contact and send the following link from CiviMail:', array('!contact' => wf_crm_contact_label(1, $this->data, TRUE))) .
        '<br /><code>' . url("node/{$this->node->nid}", array('absolute' => TRUE, 'query' => array('cid1' => ''))) . '{contact.contact_id}&amp;{contact.checksum}</code></p>',
    );
    $this->form['options']['create_fieldsets'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create Fieldsets'),
      '#default_value' => (bool) $this->settings['create_fieldsets'],
      '#description' => t('Create a fieldset around each contact. Provides visual organization of your form. Also allows the contact clone feature to work.'),
    );
    $this->form['options']['confirm_subscription'] = array(
      '#type' => 'checkbox',
      '#title' => t('Confirm Subscriptions'),
      '#default_value' => (bool) $this->settings['confirm_subscription'],
      '#description' => t('Recommended. Send a confirmation email before adding contacts to publicly subscribable mailing list groups.') . '<br />' . t('Your public mailing lists:') . ' <em>',
    );
    $ml = wf_crm_apivalues('group', 'get', array('is_hidden' => 0, 'visibility' => 'Public Pages', 'group_type' => 2), 'title');
    if ($ml) {
      if (count($ml) > 4) {
        $ml = array_slice($ml, 0, 3);
        $ml[] = t('etc.');
      }
      $this->form['options']['confirm_subscription']['#description'] .= implode(', ', $ml) . '</em>';
    }
    else {
      $this->form['options']['confirm_subscription']['#description'] .= t('none') . '</em>';
    }
    $this->form['options']['block_unknown_users'] = array(
      '#type' => 'checkbox',
      '#title' => t('Block Unknown Users'),
      '#default_value' => !empty($this->settings['block_unknown_users']),
      '#description' => t('Only allow users to see this form if they are logged in or following a personalized link from CiviMail.'),
    );
    $this->form['options']['new_contact_source'] = array(
      '#type' => 'textfield',
      '#title' => t('Source Label'),
      '#maxlength' => 255,
      '#size' => 30,
      '#default_value' => $this->settings['new_contact_source'],
      '#description' => t('Optional "source" label for any new contact/participant/membership created by this webform.'),
    );
  }

  private function checkSubmissionLimit() {
    $webform = &$this->node->webform;
    // If anonymous users don't have access to the form, no need for a warning.
    if (!in_array('1', $webform['roles'])) {
      return;
    }
    // Handle ajax submission from "submit_limit" mini-form below
    if (!empty($_POST['submit_limit']) && !empty($_POST['submit_interval'])) {
      $submit_limit = (int) $_POST['submit_limit'];
      $submit_interval = (int) $_POST['submit_interval'];
      if ($submit_limit > 0 && $submit_interval != 0) {
        $webform['submit_limit'] = $submit_limit;
        $webform['submit_interval'] = $submit_interval;
        db_update('webform')
          ->condition('nid', $this->node->nid)
          ->fields(array('submit_limit' => $submit_limit, 'submit_interval' => $submit_interval))
          ->execute();
      }
      drupal_set_message(t('Per-user submission limit has been updated. You may revisit these options any time on the <em>Form Settings</em> tab of this webform.'));
    }
    // Handle ajax submission from "webform_tracking_mode" mini-form below
    if (!empty($_POST['webform_tracking_mode']) && $_POST['webform_tracking_mode'] == 'strict') {
      variable_set('webform_tracking_mode', 'strict');
      drupal_set_message(t('Webform anonymous user tracking has been updated to use the strict method. You may revisit this option any time on the global <a !link>Webform Settings</a> page.',
        array('!link' => 'href="/admin/config/content/webform" target="_blank"')
      ));
    }
    // Mini-form to configure submit limit without leaving the page
    if ($webform['submit_limit'] == -1) {
      $this->form['contribution']['sets']['submit_limit'] = array(
        '#markup' => '<div class="messages warning">' .
          t('To prevent Credit Card abuse, it is recommended to set the per-user submission limit for this form.') .
          ' &nbsp; <button id="configure-submit-limit" type="button">' . t('Configure') . '</button>' .
          '<div id="submit-limit-wrapper" style="display:none">' .
            t('Limit each user to') .
            ' <input class="form-text" type="number" maxlength="2" size="2" name="submit_limit"> ' .
            t('submission(s)') . ' <select class="form-select" name="submit_interval">' .
              '<option value="-1">' .t('ever') . '</option>' .
              '<option value="3600" selected="selected">' .t('every hour') . '</option>' .
              '<option value="86400">' .t('every day') . '</option>' .
              '<option value="604800">' .t('every week') . '</option>'.
            '</select> &nbsp; ' .
            ' <button id="configure-submit-limit-save" type="button">' . t('Save') . '</button>' .
            ' <button id="configure-submit-limit-cancel" type="button">' . t('Cancel') . '</button>' .
          '</div>' .
        '</div>',
      );
    }
    // Mini-form to conveniently update global cookie setting
    elseif (variable_get('webform_tracking_mode', 'cookie') == 'cookie') {
      $this->form['contribution']['sets']['webform_tracking_mode'] = array(
        '#markup' => '<div class="messages warning">' .
          t('Per-user submission limit is enabled for this form, however the webform anonymous user tracking method is configured to use cookies only, which is not secure enough to prevent Credit Card abuse.') .
          ' <button id="webform-tracking-mode" type="button">' . t('Change Now') . '</button>' .
          ' <input type="hidden" value="" name="webform_tracking_mode"> ' .
        '</div>'
      );
    }
  }

  /**
   * Set defaults when visiting the civicrm tab for the first time
   */
  private function defaultSettings() {
    return array(
      'data' => array(
        'contact' => array(
          1 => array(
            'contact' => array(
              1 => array(
                'contact_type' => 'individual',
                'contact_sub_type' => array(),
              ),
            ),
          ),
        ),
        'reg_options' => array(
          'validate' => 1,
        ),
      ),
      'confirm_subscription' => 1,
      'create_fieldsets' => 1,
      'new_contact_source' => check_plain($this->node->title),
      'civicrm_1_contact_1_contact_first_name' => 'create_civicrm_webform_element',
      'civicrm_1_contact_1_contact_last_name' => 'create_civicrm_webform_element',
      'civicrm_1_contact_1_contact_existing' => 'create_civicrm_webform_element',
    );
  }

  /**
   * Build a field item for the admin form
   *
   * @param string $fid
   *   civicrm field id
   * @param array $field
   *   Webform field info
   *
   * @return array
   *   FAPI form item array for the admin form
   */
  private function addItem($fid, $field) {
    list(, $c, $ent, $n, $table, $name) = explode('_', $fid, 6);
    $item = array(
      // We don't need numbers on the admin form since they are already grouped in fieldsets
      '#title' => str_replace('#', '', $field['name']),
      '#attributes' => wf_crm_aval($field, 'attributes'),
    );

    // Create dropdown list
    if (!empty($field['expose_list'])) {
      $field['form_key'] = $fid;
      // Retrieve option list
      $options = array();
      // Placeholder empty option - used by javascript when displaying multiselect as single
      if (!empty($field['extra']['multiple']) && empty($field['extra']['required'])) {
        $options += array('' => '- ' . t('None') . ' -');
      }
      // This prevents the multi-select js from adding an illegal empty option
      if (!empty($field['extra']['required'])) {
        $item['#attributes']['class'][] = 'required';
      }
      if ($field['type'] != 'hidden') {
        $options += array('create_civicrm_webform_element' => t('- User Select -'));
      }
      $options += wf_crm_field_options($field, 'config_form', $this->data);
      $item += array(
        '#type' => 'select',
        '#options' => $options,
        '#multiple' => !empty($field['extra']['multiple']),
        '#default_value' => !empty($field['empty_option']) ? 0 : NULL,
      );
      if (isset($field['empty_option'])) {
        $item['#empty_option'] = '- ' . $field['empty_option'] . ' -';
        $item['#empty_value'] = 0;
      }
      if (isset($field['data_type'])) {
        $item['#attributes']['data-type'] = $field['data_type'];
      }
      // Five ways to get default value...
      // 1: From current form state
      if (isset($this->settings[$fid]) && ($field['type'] != 'hidden')) {
        $item['#default_value'] = $this->settings[$fid];
      }
      // 2: From saved settings
      elseif (isset($this->data[$ent][$c][$table][$n][$name])) {
        $item['#default_value'] = $this->data[$ent][$c][$table][$n][$name];
      }
      // 3: From callback function
      elseif (isset($field['value_callback'])) {
        $method = 'get_default_' . $table . '_' . $name;
        $item['#default_value'] = self::$method($fid, $options);
      }
      // 4: From field default
      elseif (isset($field['value'])) {
        $item['#default_value'] = $field['value'];
      }
      // 5: For required fields like phone type, default to the first option
      elseif (empty($field['extra']['multiple']) && !isset($field['empty_option'])) {
        $options = array_keys($options);
        $item['#default_value'] = $options[1];
      }
      if (!empty($field['extra']['multiple'])) {
        $item['#default_value'] = (array) $item['#default_value'];
        if (isset($this->settings[$fid]) && !is_array($this->settings[$fid])
          && isset($this->data[$ent][$c][$table][$n][$name])) {
          $item['#default_value'] += (array) $this->data[$ent][$c][$table][$n][$name];
        }
      }
    }
    // Create checkbox
    else {
      $item += array(
        '#type' => 'checkbox',
        '#return_value' => 'create_civicrm_webform_element',
        '#default_value' => !empty($this->settings[$fid]),
      );
    }
    // Add help
    $topic = $table . '_' . $name;
    if (method_exists('wf_crm_admin_help', $topic)) {
      $this->help($item, $topic);
    }
    elseif (!empty($field['has_help'])) {
      $this->help($item, $name);
    }
    elseif (wf_crm_aval($field, 'data_type') == 'ContactReference') {
      $this->help($item, 'contact_reference');
    }
    elseif (!empty($field['expose_list']) && !empty($field['extra']['multiple'])) {
      $this->help($item, 'multiselect_options', t('Multiple Select Options'));
    }
    return $item;
  }

  /**
   * Boilerplate-reducing helper function for FAPI ajax.
   * Set an existing form element to control an ajax container.
   * The container will be created if it doesn't already exist.
   *
   * @param string $path
   *   A : separated string of nested array keys leading to the control element's parent
   * @param string $control_element
   *   Array key of the existing element to add ajax behavior to
   * @param string $container
   *   Path to the key of the container to be created (relative to $path) use '..' to go up a level
   * @param string $class
   *   Css class to add to target container
   */
  private function addAjaxItem($path, $control_element, $container, $class='civicrm-ajax-wrapper') {
    // Get a reference to the control container
    // For anyone who wants to call this evil - I challenge you to find a better way to accomplish this
    eval('$control_container = &$this->form[\'' . str_replace(':', "']['", $path) . "'];");
    // Now find the container element (may be outside the $path if .. is used)
    foreach (explode(':', $container) as $level) {
      if ($level == '..') {
        $path = substr($path, 0, strrpos($path, ':'));
      }
      else {
        $path .= ':' . $level;
      }
    }
    eval('$target_container = &$this->form[\'' . str_replace(':', "']['", substr($path, 0, strrpos($path, ':'))) . "'];");
    $id = 'civicrm-ajax-' . str_replace(array(':', '_'), '-', $path);
    $control_container[$control_element]['#ajax'] = array(
      'callback' => 'wf_crm_configure_form_ajax',
      'pathstr' => $path,
      'wrapper' => $id,
      'effect' => 'fade',
    );
    if (!isset($target_container[$level])) {
      $target_container[$level] = array();
    }
    $target_container[$level]['#prefix'] = '<div class="' . $class . '" id="' . $id . '">';
    $target_container[$level]['#suffix'] = '</div>';
  }

  /**
   * Build select all/none js links for a fieldset
   */
  private function addToggle($name) {
    return array('#markup' =>
    '<div class="web-civi-js-select">
      <a href="javascript:wfCiviAdmin.selectReset(' . "'all', '#$name'" . ')">' . t('Select All') . '</a> |
      <a href="javascript:wfCiviAdmin.selectReset(' . "'none', '#$name'" . ')">' . t('Select None') . '</a> |
      <a href="javascript:wfCiviAdmin.selectReset(' . "'reset', '#$name'" . ')">' . t('Restore') . '</a>
    </div>',
    );
  }

  /**
   * Build $this->data array for webform settings; called while rebuilding or post-processing the admin form.
   */
  private function rebuildData() {
    $this->settings['data'] = array('contact' => array());
    $this->data = &$this->settings['data'];
    list($contact_types, $sub_types) = wf_crm_get_contact_types();
    for ($c = 1; $c <= $this->settings['number_of_contacts']; ++$c) {
      // Contact settings
      if (isset($this->settings[$c . '_contact_type'])) {
        $this->data['contact'][$c] = array(
          'contact' => array(1 => array(
            'contact_type' => $this->settings[$c . '_contact_type'],
            'contact_sub_type' => array(),
            'webform_label' => $this->settings[$c . '_webform_label'],
          )),
        );
        if ($sub_type = wf_crm_aval($this->settings, 'civicrm_' . $c . '_contact_1_contact_contact_sub_type')) {
          $allowed = wf_crm_aval($sub_types, $this->settings[$c . '_contact_type'], array());
          foreach ($sub_type as $sub) {
            if (isset($allowed[$sub])) {
              $this->data['contact'][$c]['contact'][1]['contact_sub_type'][$sub] = $sub;
            }
          }
        }
      }
      // Add new contact to the form
      else {
        $this->data['contact'][$c] = array(
          'contact' => array(1 => array(
            'contact_type' => 'individual',
            'contact_sub_type' => array(),
          )),
          'matching_rule' => 'Unsupervised',
        );
        // Set defaults for new contact
        $this->settings += array(
          'civicrm_' . $c . '_contact_1_contact_first_name' => 'create_civicrm_webform_element',
          'civicrm_' . $c . '_contact_1_contact_last_name' => 'create_civicrm_webform_element',
        );
        $link = array('!link' => 'href="http://wiki.civicrm.org/confluence/display/CRMDOC/Webform+CiviCRM+Integration#WebformCiviCRMIntegration-CloningaContact" target="_blank"');
        drupal_set_message(t('Tip: Consider using the clone feature to add multiple similar contacts. (<a !link>more info</a>)', $link), 'status', FALSE);
      }
    }
    // Store meta settings, i.e. number of email for contact 1
    foreach ($this->settings as $key => $val) {
      if (strpos($key, '_number_of_') !== FALSE) {
        list($ent, $c, $key) = explode('_', $key, 3);
        if (isset($this->data[$ent][$c]) || $ent == 'participant' || $ent == 'membership') {
          $this->data[$ent][$c][$key] = $val;
        }
        // Standalone entities keep their own count independent of contacts
        elseif ($ent == 'grant' || $ent == 'activity' || $ent == 'case') {
          $this->data[$ent]["number_$key"] = $val;
        }
      }
      elseif (strpos($key, '_settings_') !== FALSE) {
        list($ent, $c, , $key) = explode('_', $key, 4);
        $val = is_array($val) ? array_filter($val) : $val;
        // Don't store settings for nonexistant contacts. Todo: check other entities
        if (isset($this->data[$ent][$c]) || $ent !== 'contact') {
          $this->data[$ent][$c][$key] = $val;
        }
      }
    }
    // Defaults when adding an activity
    for ($i=1; $i <= $this->settings['activity_number_of_activity']; ++$i) {
      if (!isset($this->settings["activity_{$i}_settings_existing_activity_status"])) {
        $this->data['activity'][$i]['activity'][1]['target_contact_id'] = range(1, $this->settings['number_of_contacts']);
      }
    }
    // Defaults when adding a case
    for ($i=1; $i <= wf_crm_aval($this->settings, 'case_number_of_case'); ++$i) {
      if (!isset($this->settings["civicrm_{$i}_case_1_case_case_type_id"])) {
        $case_types = array_keys(wf_crm_apivalues('Case', 'getoptions', array('field' => 'case_type_id')));
        $this->data['case'][$i]['case'][1]['case_type_id'] = $case_types[0];
      }
    }
    // Store event settings
    if (isset($this->settings['participant_reg_type'])) {
      $this->data['participant_reg_type'] = $this->settings['participant_reg_type'];
      $this->data['reg_options'] = $this->settings['reg_options'];
    }
    // Add settings exposed to the back-end to data
    foreach ($this->settings as $key => $val) {
      if (substr($key, 0, 7) == 'civicrm') {
        list(, $c, $ent, $n, $table, $name) = explode('_', $key, 6);
        if (is_array($val)) {
          // Git rid of the "User Select" and "None" options
          unset($val['create_civicrm_webform_element'], $val['']);
        }
        elseif ($val === 'create_civicrm_webform_element') {
          $val = '';
        }
        // Saves all non-empty values with a hack for fields which needs to be saved even when 0
        // FIXME: Really ought to change the select placeholder value to be '' instead of 0
        if (isset($this->fields[$table . '_' . $name]['expose_list']) &&
          (!empty($val) || (in_array($name, array('num_terms', 'is_active', 'is_test'))))) {
          // Don't add data for non-existent contacts
          if (!in_array($ent, array('contact', 'participant', 'membership')) || isset($this->data['contact'][$c])) {
            $this->data[$ent][$c][$table][$n][$name] = $val;
          }
        }
      }
    }
  }

  /**
   * Submission handler, saves CiviCRM options for a Webform node
   */
  public function postProcess() {
    $button = $this->form_state['clicked_button']['#id'];
    $nid = $this->node->nid;
    $this->settings = wf_crm_aval($this->form_state, 'storage:vals', $this->form_state['values']);

    if ((empty($this->node->webform_civicrm) && !$this->settings['nid']) || $button == 'edit-cancel') {
      $this->form_state['rebuild'] = TRUE;
      unset($this->form_state['storage']['msg']);
      return;
    }
    unset($this->form_state['storage']);

    $enabled = $existing = wf_crm_enabled_fields($this->node, NULL, TRUE);
    $delete_me = $this->getFieldsToDelete($enabled);

    // Display a confirmation before deleting fields
    if ($delete_me && $button == 'edit-submit') {
      $msg = '<p>' . t('These existing fields are no longer needed for CiviCRM processing based on your new form settings.') . '</p><ul>';
      foreach ($delete_me as $key => $id) {
        list(, $c, $ent, $n, $table, $name) = explode('_', $key, 6);
        $info = '';
        if ($ent == 'contact' || $ent == 'participant') {
          $info = '<em>' . wf_crm_contact_label($c, wf_crm_aval($this->node, 'webform_civicrm:data'));
        }
        if ($info && isset($this->sets[$table]['max_instances'])) {
          $info .= ' ' . $this->sets[$table]['label'] . ' ' . $n;
        }
        $info .= $info ? ':</em> ' : '';
        $msg .= '<li>' . $info . $this->node->webform['components'][$id]['name'] . '</li>';
      }
      $msg .= '</ul><p>' . t('Would you like them to be automatically removed from the webform? This is recommended unless you need to keep webform-results information from these fields. (They can still be deleted manually later if you choose not to remove them now.)') . '</p><p><em>' . t('Note: Deleting webform components cannot be undone, and will result in the loss of webform-results info for those elements. Data in the CiviCRM database will not be affected.') . '</em></p>';
      $this->form_state['storage']['msg'] = $msg;
      $this->form_state['storage']['vals'] = $this->settings;
      $this->form_state['rebuild'] = TRUE;
      return;
    }

    module_load_include('inc', 'webform', 'includes/webform.components');
    module_load_include('inc', 'webform_civicrm', 'includes/contact_component');
    $this->form_state['redirect'] = 'node/' . $nid . '/webform';

    // Delete/disable fields
    $deleted = 0;
    if ($button === 'edit-delete' || ($button === 'edit-disable' && $this->settings['nid'])) {
      foreach ($delete_me as $id) {
        $field = $this->node->webform['components'][$id];
        unset($enabled[$field['form_key']]);
        ++$deleted;
        if ($button === 'edit-delete') {
          webform_component_delete($this->node, $field);
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
            if ($ent == 'contact' && $i == 1 && (!$this->settings['nid'] || $c > $this->settings['number_of_contacts'])) {
              if (!_wf_crm_child_components($this->node->nid, $id)) {
                webform_component_delete($this->node, $this->node->webform['components'][$id]);
              }
            }
          }
        }
      }
    }

    // Disable CiviCRM for this form
    if (!$this->settings['nid']) {
      $this->disable();
      drupal_set_message(t('CiviCRM processing for this form is now disabled.'));
    }

    // CiviCRM enabled
    else {
      webform_ensure_record($this->node);
      $this->rebuildData();
      if (!$this->settings['toggle_message']) {
        $this->settings['message'] = '';
      }
      // Index disabled components
      $disabled = array();
      foreach (wf_crm_aval($this->node->webform, 'components', array()) as $field) {
        if (substr($field['form_key'], 0, 9) === 'disabled_') {
          $field['form_key'] = 'civicrm' . substr($field['form_key'], 8);
          $disabled[$field['form_key']] = $field;
        }
      }

      $i = 0;
      $created = array();
      foreach ($this->settings as $key => $val) {
        if (substr($key, 0, 7) == 'civicrm') {
          ++$i;
          $field = wf_crm_get_field($key);
          if (!isset($enabled[$key])) {
            $val = (array) $val;
            if (in_array('create_civicrm_webform_element', $val, TRUE) || (!empty($val[0]) && $field['type'] == 'hidden')) {
              // Restore disabled component
              if (isset($disabled[$key])) {
                webform_component_update($disabled[$key]);
                $enabled[$key] = $disabled[$key]['cid'];
                drupal_set_message(t('Re-enabled field: %name', array('%name' => $disabled[$key]['name'])));
              }
              // Create new component
              else {
                $field += array(
                  'nid' => $nid,
                  'form_key' => $key,
                  'weight' => $i,
                );
                self::insertComponent($field, $enabled, $this->settings);
                $created[] = $field['name'];
                if (isset($field['civicrm_condition'])) {
                  $this->addConditionalRule($field, $enabled);
                }
              }
            }
          }
          elseif ($field['type'] == 'hidden' && !empty($field['expose_list'])) {
            $component = $this->node->webform['components'][$enabled[$key]];
            $component['value'] = $val;
            webform_component_update($component);
          }
        }
      }
      if (count($created) == 1) {
        drupal_set_message(t('Added field: %name', array('%name' => $created[0])));
      }
      elseif ($created) {
        drupal_set_message(t('Added !num fields to the form.', array('!num' => count($created))));
      }

      // Create record
      if (empty($this->node->webform_civicrm)) {
        drupal_write_record('webform_civicrm_forms', $this->settings);
        drupal_set_message(t('CiviCRM processing for this form is now enabled.'));
      }
      // Update record
      else {
        drupal_write_record('webform_civicrm_forms', $this->settings, 'nid');
        drupal_set_message(t('Your CiviCRM form settings have been updated.'));
      }
      // Update existing contact fields
      foreach ($existing as $fid => $id) {
        if (substr($fid, -8) === 'existing') {
          wf_crm_update_existing_component($this->node->webform['components'][$id], $enabled, $this->data);
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
   * Create a conditional rule if the source and target fields both exist.
   * TODO: This is fairly minimal. It doesn't check if the rule already exists,
   * and doesn't work if both fields haven't been created yet.
   *
   * @param array $field
   * @param array $enabled
   */
  private function addConditionalRule($field, $enabled) {
    list(, $c, $ent, $n, $table, $name) = explode('_', $field['form_key'], 6);
    $rgid = $weight = -1;
    foreach ($this->node->webform['conditionals'] as $rgid => $existing) {
      $weight = $existing['weight'];
    }
    $rgid++;
    $rule_group = $field['civicrm_condition'] + array(
      'nid' => $this->node->nid,
      'rgid' => $rgid,
      'weight' => $weight,
      'target' => $enabled[$field['form_key']],
      'target_type' => 'component',
    );
    $rule_group['rules'] = array();
    foreach ($field['civicrm_condition']['rules'] as $source => $conditions) {
      $source_key = "civicrm_{$c}_{$ent}_{$n}_{$source}";
      $source_id = wf_crm_aval($enabled, $source_key);
      if ($source_id) {
        $options = wf_crm_field_options(array('form_key' => $source_key), '', $this->settings['data']);
        foreach ($conditions as $condition) {
          if (isset($options[$condition])) {
            $rule_group['rules'][] = array(
              'source_type' => 'component',
              'source' => $source_id,
              'operator' => 'equal',
              'value' => $condition,
            );
          }
        }
      }
    }
    if ($rule_group['rules']) {
      $this->node->webform['conditionals'][] = $rule_group;
      module_load_include('inc', 'webform', 'includes/webform.conditionals');
      webform_conditional_insert($rule_group);
    }
  }

  private function help(&$field, $topic, $title = NULL) {
    wf_crm_admin_help::addHelp($field, $topic, $title);
  }

  /**
   * Search for fields that should be deleted
   * @param array $fields
   * @return array
   */
  private function getFieldsToDelete($fields) {
    // Find fields to delete
    foreach ($fields as $key => $val) {
      $val = (array) wf_crm_aval($this->settings, $key);
      if (((in_array('create_civicrm_webform_element', $val, TRUE)) && $this->settings['nid'])
        || strpos($key, 'fieldset') !== FALSE) {
        unset($fields[$key]);
      }
      else {
        $field = wf_crm_get_field($key);
        if (!empty($val[0]) && $field['type'] == 'hidden') {
          unset($fields[$key]);
        }
      }
    }
    return $fields;
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
  static function insertComponent(&$field, &$enabled, $settings) {
    $options = NULL;
    list(, $c, $ent, $n, $table, $name) = explode('_', $field['form_key'], 6);
    $contact_type = $settings['data']['contact'][$c]['contact'][1]['contact_type'];
    // Replace the # token with set number (or append to the end if no token)
    if ($n > 1) {
      if (strpos($field['name'], '#') === FALSE) {
        $field['name'] .= " $n";
      }
      else {
        $field['name'] = str_replace('#', $n, $field['name']);
      }
    }
    elseif ($table == 'relationship') {
      $field['name'] = t('Relationship to !contact', array('!contact' => wf_crm_contact_label($n, $settings['data']))) . ' ' . $field['name'];
    }
    else {
      $field['name'] = str_replace(' #', '', $field['name']);
    }
    if ($name == 'contact_sub_type') {
      list($contact_types) = wf_crm_get_contact_types();
      $field['name'] = t('Type of @contact', array('@contact' => $contact_types[$contact_type]));
    }
    // Defaults for existing contact field
    if ($name == 'existing') {
      $vals = $enabled + $settings;
      // Set the allow_create flag based on presence of name or email fields
      $field['extra']['allow_create'] = $a = wf_crm_name_field_exists($vals, $c, $contact_type);
      $field['extra']['none_prompt'] = $a ? t('+ Create new +') : t('- None Found -');
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
    // Support html_textarea module
    if ($field['type'] == 'html_textarea') {
      $field['value']['format'] = filter_default_format();
      $field['value']['value'] = '';
    }
    // Retrieve option list
    if ($field['type'] == 'select') {
      if ($options = wf_crm_field_options($field, 'component_insert', $settings['data'])) {
        $field['extra']['items'] = wf_crm_array2str($options);
        // Default to select widget
        if (!isset($field['extra']['aslist']) && empty($field['extra']['multiple'])) {
          $field['extra']['aslist'] = 1;
        }
        // A single static radio should be shown as a checkbox
        if (count($options) == 1 && empty($field['extra']['aslist']) && empty($field['extra']['civicrm_live_options'])) {
          $field['extra']['multiple'] = 1;
        }
      }
    }
    if (isset($field['value_callback'])) {
      $method = 'get_default_' . $table . '_' . $name;
      $field['value'] = self::$method($field['form_key'], $options);
    }
    // For hidden+select fields such as contribution_page
    if ($field['type'] == 'hidden' && !empty($field['expose_list']) && !empty($settings[$field['form_key']])) {
      $field['value'] = $settings[$field['form_key']];
    }
    // Create fieldsets - for contact and contact-specific event fields only
    if ($ent != 'activity' && $ent != 'case' && $ent != 'contribution' && $ent != 'grant'
      && ($ent != 'participant' || wf_crm_aval($settings['data'], 'participant_reg_type') == 'separate')) {
      self::addFieldset($c, $field, $enabled, $settings);
    }
    // Create page break for contribution page
    if ($name == 'contribution_page_id') {
      self::addPageBreak($field);
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
   * Create a fieldset around a contact if it doesn't already exist
   * @param $c
   * @param $field
   * @param $enabled
   * @param $settings
   */
  static function addFieldset($c, &$field, &$enabled, $settings) {
    $sid = 'civicrm_' . $c . '_contact_1_fieldset_fieldset';
    if (!empty($settings['create_fieldsets']) && !isset($enabled[$sid])) {
      $new_set = array(
        'nid' => $field['nid'],
        'form_key' => $sid,
        'type' => 'fieldset',
        'name' => wf_crm_contact_label($c, $settings['data']),
        'weight' => $c,
      );
      $new_set += webform_component_invoke('fieldset', 'defaults');
      $enabled[$sid] = webform_component_insert($new_set);
    }
    $field['pid'] = wf_crm_aval($enabled, $sid, 0);
  }

  /**
   * Create a page-break before the contribution-page field
   * @param $field
   */
  static function addPageBreak($field) {
    $node = node_load($field['nid']);
    // Check if it already exists
    foreach (wf_crm_aval($node->webform, 'components', array()) as $component) {
      if ($component['form_key'] == 'contribution_pagebreak') {
        return;
      }
    }
    $pagebreak = array(
      'nid' => $field['nid'],
      'form_key' => 'contribution_pagebreak',
      'type' => 'pagebreak',
      'name' => t('Payment'),
      'weight' => $field['weight'] - 9,
    );
    $pagebreak += webform_component_invoke('pagebreak', 'defaults');
    webform_component_insert($pagebreak);
  }

  /**
   * Default value callback
   */
  static function get_default_contact_cs($fid, $options) {
    return CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME, 'checksum_timeout', NULL, 7);
  }

  /**
   * Default value callback
   */
  static function get_default_contribution_payment_processor_id($fid, $options) {
    $default = wf_civicrm_api('payment_processor', 'get', array('is_default' => 1, 'is_test' => 0));
    if (!empty($default['id']) && isset($options[$default['id']])) {
      return $default['id'];
    }
    unset($options[0]);
    return $options ? key($options) : 0;
  }

  /**
   * Delete civicrm settings for this webform.
   */
  private function disable() {
    db_delete('webform_civicrm_forms')
      ->condition('nid', $this->node->nid)
      ->execute();
  }

  private function contactRefOptions($exclude = NULL) {
    $ret = array();
    foreach ($this->data['contact'] as $num => $contact) {
      if ($num != $exclude) {
        $ret[$num] = wf_crm_contact_label($num, $this->data, 'plain');
      }
    }
    return $ret;
  }
}

/**
 * Drupal FAPI form builder callback
 */
function wf_crm_configure_form($form, &$form_state, $node) {
  $admin_form = new wf_crm_admin_form(array(), $form_state, $node);
  return $admin_form->buildForm();
}

/**
 * Drupal FAPI submit callback
 */
function wf_crm_configure_form_submit($form, &$form_state) {
  $node = node_load($form_state['storage']['nid']);
  $admin_form = new wf_crm_admin_form($form, $form_state, $node);
  $admin_form->postProcess();
}

/**
 * FAPI AJAX callback
 */
function wf_crm_configure_form_ajax($form, $form_state) {
  return wf_crm_aval($form, $form_state['triggering_element']['#ajax']['pathstr']);
}
