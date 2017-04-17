<?php

/**
 * @file
 * Webform CiviCRM module's common utility functions.
 */

/**
 * Get options for a specific field
 *
 * @param array $field
 *   Webform component array
 * @param string $context
 *   Where is this being called from?
 * @param array $data
 *   Array of crm entity data
 *
 * @return array
 */
function wf_crm_field_options($field, $context, $data) {
  $ret = array();
  $fields = wf_crm_get_fields();
  if ($pieces = wf_crm_explode_key($field['form_key'])) {
    list( , $c, $ent, $n, $table, $name) = $pieces;
    // Ensure we have complete info for this field
    $field += $fields[$table . '_' . $name];
    if ($name === 'contact_sub_type') {
      list($contact_types, $sub_types) = wf_crm_get_contact_types();
      $ret = wf_crm_aval($sub_types, $data['contact'][$c]['contact'][1]['contact_type'], array());
    }
    elseif ($name === 'relationship_type_id') {
      $ret = wf_crm_get_contact_relationship_types($data['contact'][$c]['contact'][1]['contact_type'], $data['contact'][$n]['contact'][1]['contact_type'], $data['contact'][$c]['contact'][1]['contact_sub_type'], $data['contact'][$n]['contact'][1]['contact_sub_type']);
    }
    elseif ($name === 'relationship_permission') {
      $ret = array(
        1 => t('!a may view and edit !b', array('!a' => wf_crm_contact_label($c, $data, 'plain'), '!b' => wf_crm_contact_label($n, $data, 'plain'))),
        2 => t('!a may view and edit !b', array('!a' => wf_crm_contact_label($n, $data, 'plain'), '!b' => wf_crm_contact_label($c, $data, 'plain'))),
        3 => t('Both contacts may view and edit each other'),
      );
    }
    // If this is a contract reference or shared address field, list webform contacts
    elseif ($name === 'master_id' || wf_crm_aval($field, 'data_type') === 'ContactReference') {
      $contact_type = wf_crm_aval($field, 'reference_contact_type', 'contact');
      foreach ($data['contact'] as $num => $contact) {
        if ($num != $c || $name != 'master_id') {
          if ($contact_type == 'contact' || $contact_type == $contact['contact'][1]['contact_type']) {
            $ret[$num] = wf_crm_contact_label($num, $data, 'plain');
          }
        }
      }
    }
    elseif ($name == 'privacy') {
      $ret = wf_crm_get_privacy_options();
    }
    elseif ($table === 'other') {
      if ($field['table'] === 'tag') {
        $split = explode('_', $name);
        $ret = CRM_Core_BAO_Tag::getTags("civicrm_{$ent}", $ret, wf_crm_aval($split, 1), '- ');
      }
      elseif ($field['table'] === 'group') {
        $ret = wf_crm_apivalues('group', 'get', array('is_hidden' => 0), 'title');
      }
    }
    elseif ($name === 'survey_id') {
      $ret = wf_crm_get_surveys(wf_crm_aval($data, "activity:$c:activity:1", array()));
    }
    elseif ($name == 'event_id') {
      $ret = wf_crm_get_events($data['reg_options'], $context);
    }
    elseif ($table == 'contribution' && $name == 'is_test') {
      // Getoptions would return 'yes' and 'no' - this is a bit more descriptive
      $ret = array(0 => t('Live Transactions'), 1 => t('Test Mode'));
    }
    // Not a real field so can't call getoptions for this one
    elseif ($table == 'membership' && $name == 'num_terms') {
      $ret = drupal_map_assoc(range(1, 9));
    }
    // Aside from the above special cases, most lists can be fetched from api.getoptions
    else {
      $params = array('field' => $name, 'context' => 'create');
      // Custom fields - use main entity
      if (substr($table, 0, 2) == 'cg') {
        $table = $ent;
      }
      else {
        // Pass data into api.getoptions for contextual filtering
        $params += wf_crm_aval($data, "$ent:$c:$table:$n", array());
      }
      $ret = wf_crm_apivalues($table, 'getoptions', $params);

      // Hack to format money data correctly
      if (!empty($field['data_type']) && $field['data_type'] === 'Money') {
        $old = $ret;
        $ret = array();
        foreach ($old as $key => $val) {
          $ret[number_format(str_replace(',', '', $key), 2, '.', '')] = $val;
        }
      }
    }
    // Remove options that were set behind the scenes on the admin form
    if ($context != 'config_form' && !empty($field['extra']['multiple']) && !empty($field['expose_list'])) {
      foreach (wf_crm_aval($data, "$ent:$c:$table:$n:$name", array()) as $key => $val) {
        unset($ret[$key]);
      }
    }
  }
  if (!empty($field['exposed_empty_option'])) {
    $ret = array(0 => $field['exposed_empty_option']) + $ret;
  }
  return $ret;
}

/**
 * Get list of states, keyed by abbreviation rather than ID.
 * FIXME use the api for this.
 * @param null|int|string $param
 */
function wf_crm_get_states($param = NULL) {
  $ret = array();
  if (!$param || $param == 'default') {
    $config = CRM_Core_Config::singleton();
    if (!$param && !empty($config->provinceLimit)) {
      $param = implode(',', $config->provinceLimit);
    }
    else {
      $param = (int) $config->defaultContactCountry;
    }
  }
  else {
    $param = (int) $param;
  }
  $sql = "SELECT name AS label, UPPER(abbreviation) AS value FROM civicrm_state_province WHERE country_id IN ($param) ORDER BY name";
  $dao = CRM_Core_DAO::executeQuery($sql);
  while ($dao->fetch()) {
    $ret[$dao->value] = $dao->label;
  }
  // Localize the state/province names if in an non-en_US locale
  $tsLocale = CRM_Utils_System::getUFLocale();
  if ($tsLocale != '' and $tsLocale != 'en_US') {
    $i18n = CRM_Core_I18n::singleton();
    $i18n->localizeArray($ret, array('context' => 'province'));
    CRM_Utils_Array::asort($ret);
  }
  return $ret;
}

/**
 * Match a state/province id to its abbr. and vice-versa
 *
 * @param $input
 *   User input (state province id or abbr)
 * @param $ret
 *   String: 'abbreviation' or 'id'
 * @param $country_id
 *   Int: (optional) must be supplied if fetching id from abbr
 *
 * @return string or integer
 */
function wf_crm_state_abbr($input, $ret = 'abbreviation', $country_id = NULL) {
  $input_type = $ret == 'id' ? 'abbreviation' : 'id';
  $sql = "SELECT $ret FROM civicrm_state_province WHERE $input_type = %1";
  $vars = array(1 => array($input, $ret == 'id' ? 'String' : 'Integer'));
  if ($ret === 'id') {
    if (!$country_id || $country_id === 'default') {
      $config = CRM_Core_Config::singleton();
      $country_id = (int) $config->defaultContactCountry;
    }
    $sql .= ' AND country_id = %2';
    $vars[2] = array($country_id, 'Integer');
  }
  $sql .= ' LIMIT 0, 1';
  return CRM_Core_DAO::singleValueQuery($sql, $vars);
}

/**
 * Get list of events.
 * FIXME use the api for this.
 *
 * @param array $reg_options
 * @param string $context
 * @return array
 */
function wf_crm_get_events($reg_options, $context) {
  $ret = array();
  $format = wf_crm_aval($reg_options, 'title_display', 'title');
  $sql = "SELECT id, title, start_date, end_date, event_type_id FROM civicrm_event WHERE is_template = 0 AND is_active = 1";
  // 'now' means only current events, 1 means show all past events, other values are relative date strings
  $date_past = wf_crm_aval($reg_options, 'show_past_events', 'now');
  if ($date_past != '1') {
    $date_past = date('Y-m-d H:i:s', strtotime($date_past));
    $sql .= " AND (end_date >= '$date_past' OR end_date IS NULL)";
  }
  // 'now' means only past events, 1 means show all future events, other values are relative date strings
  $date_future = wf_crm_aval($reg_options, 'show_future_events', '1');
  if ($date_future != '1') {
    $date_future = date('Y-m-d H:i:s', strtotime($date_future));
    $sql .= " AND (end_date <= '$date_future' OR end_date IS NULL)";
  }
  if (is_numeric($reg_options['event_type'])) {
    $sql .= ' AND event_type_id = ' . $reg_options['event_type'];
  }
  $sql .= ' ORDER BY start_date ' . ($context == 'config_form' ? 'DESC' : '');
  $dao = CRM_Core_DAO::executeQuery($sql);
  while ($dao->fetch()) {
    $ret[$dao->id . '-' . $dao->event_type_id] = wf_crm_format_event($dao, $format);
  }
  return $ret;
}

/**
 * @param array|object $event
 * @param string $format
 * @return string
 */
function wf_crm_format_event($event, $format) {
  $format = explode(' ', $format);
  // Date format
  foreach ($format as $value) {
    if (strpos($value, 'dateformat') === 0) {
      $config = CRM_Core_Config::singleton();
      $date_format = $config->$value;
    }
  }
  $event = (object) $event;
  $title = array();
  if (in_array('title', $format)) {
    $title[] = $event->title;
  }
  if (in_array('type', $format)) {
    $types = wf_crm_apivalues('event', 'getoptions', array('field' => 'event_type_id', 'context' => 'get'));
    $title[] = $types[$event->event_type_id];
  }
  if (in_array('start', $format) && $event->start_date) {
    $title[] = CRM_Utils_Date::customFormat($event->start_date, $date_format);
  }
  if (in_array('end', $format) && $event->end_date) {
    // Avoid showing redundant end-date if it is the same as the start date
    $same_day = substr($event->start_date, 0, 10) == substr($event->end_date, 0, 10);
    if (!$same_day || in_array('dateformatDatetime', $format) || in_array('dateformatTime', $format)) {
      $end_format = (in_array('dateformatDatetime', $format) && $same_day) ? $config->dateformatTime : $date_format;
      $title[] = CRM_Utils_Date::customFormat($event->end_date, $end_format);
    }
  }
  return implode(' - ', $title);
}

/**
 * Get list of surveys
 * @param array $act
 *
 * @return array
 */
function wf_crm_get_surveys($act = array()) {
  return wf_crm_apivalues('survey', 'get', array_filter($act), 'title');
}

/**
 * Get list of activity types
 * @param string $case_type_name
 * @param array $campaign_act_types
 *
 * @return array
 */
function wf_crm_get_activity_types($case_type_name, &$campaign_act_types) {
  // Return activities only appropriate to this case
  if ($case_type_name) {
    $case_info = new CRM_Case_XMLProcessor_Process();
    return $case_info->get($case_type_name, 'ActivityTypes');
  }
  // Fetch activity types from the api
  $params = array(
    'option_group_id' => 'activity_type',
    'is_active' => 1,
    'options' => array(
      'order' => 'label',
    ),
  );
  // Include campaign types if CiviCampaign is enabled
  $comp = CRM_Core_Component::getEnabledComponents();
  $campaign_type = wf_crm_aval($comp, 'CiviCampaign:componentID');

  $ret = array();
  foreach (wf_crm_apivalues('option_value', 'get', $params) as $val) {
    if (empty($val['component_id'])) {
      $ret[$val['value']] = $val['label'];
    }
    elseif ($val['component_id'] == $campaign_type) {
      $campaign_act_types[$val['value']] = $ret[$val['value']] = $val['label'];
    }
  }

  return $ret;
}

/**
 * Get activity types related to CiviCampaign
 * @return array
 */
function wf_crm_get_campaign_activity_types() {
  $ret = array();
  if (array_key_exists('activity_survey_id', wf_crm_get_fields())) {
    $vals = wf_crm_apivalues('option_value', 'get', array(
      'option_group_id' => 'activity_type',
      'is_active' => 1,
      'component_id' => 'CiviCampaign',
    ));
    foreach ($vals as $val) {
      $ret[$val['value']] = $val['label'];
    }
  }
  return $ret;
}

/**
 * Get contact types and sub-types
 * Unlike pretty much every other option list CiviCRM wants "name" instead of "id"
 *
 * @return array
 */
function wf_crm_get_contact_types() {
  static $contact_types = array();
  static $sub_types = array();
  if (!$contact_types) {
    $data = wf_crm_apivalues('contact_type', 'get', array('is_active' => 1));
    foreach ($data as $type) {
      if (empty($type['parent_id'])) {
        $contact_types[strtolower($type['name'])] = $type['label'];
        continue;
      }
      $sub_types[strtolower($data[$type['parent_id']]['name'])][$type['name']] = $type['label'];
    }
  }
  return array($contact_types, $sub_types);
}

/**
 * In reality there is no contact field 'privacy' so this is not a real option list.
 * These are actually 5 separate contact fields that this module munges into 1 for better usability.
 *
 * @return array
 */
function wf_crm_get_privacy_options() {
  return array(
    'do_not_email' => ts('Do not email'),
    'do_not_phone' => ts('Do not phone'),
    'do_not_mail' => ts('Do not mail'),
    'do_not_sms' => ts('Do not sms'),
    'do_not_trade' => ts('Do not trade'),
    'is_opt_out' => ts('NO BULK EMAILS (User Opt Out)'),
  );
}

/**
 * Get relationship type data
 *
 * @return array
 */
function wf_crm_get_relationship_types() {
  static $types = array();
  if (!$types) {
    foreach (wf_crm_apivalues('relationship_type', 'get', array('is_active' => 1)) as $r) {
      $r['type_a'] = strtolower(wf_crm_aval($r, 'contact_type_a'));
      $r['type_b'] = strtolower(wf_crm_aval($r, 'contact_type_b'));
      $r['sub_type_a'] = wf_crm_aval($r, 'contact_sub_type_a');
      $r['sub_type_b'] = wf_crm_aval($r, 'contact_sub_type_b');
      $types[$r['id']] = $r;
    }
  }
  return $types;
}

/**
 * Get valid relationship types for a given pair of contacts
 *
 * @param $type_a
 *   Contact type
 * @param $type_b
 *   Contact type
 * @param $sub_type_a
 *   Contact sub-type
 * @param $sub_type_b
 *   Contact sub-type
 *
 * @return array
 */
function wf_crm_get_contact_relationship_types($type_a, $type_b, $sub_type_a, $sub_type_b) {
  $ret = array();
  foreach (wf_crm_get_relationship_types() as $t) {
    $reciprocal = ($t['label_a_b'] != $t['label_b_a'] && $t['label_b_a'] || $t['type_a'] != $t['type_b']);
    if (($t['type_a'] == $type_a || !$t['type_a'])
      && ($t['type_b'] == $type_b || !$t['type_b'])
      && (in_array($t['sub_type_a'], $sub_type_a) || !$t['sub_type_a'])
      && (in_array($t['sub_type_b'], $sub_type_b) || !$t['sub_type_b'])
    ) {
      $ret[$t['id'] . ($reciprocal ? '_a' : '_r')] = $t['label_a_b'];
    }
    // Reciprocal form - only show if different from above
    if ($reciprocal
      && ($t['type_a'] == $type_b || !$t['type_a'])
      && ($t['type_b'] == $type_a || !$t['type_b'])
      && (in_array($t['sub_type_a'], $sub_type_b) || !$t['sub_type_a'])
      && (in_array($t['sub_type_b'], $sub_type_a) || !$t['sub_type_b'])
    ) {
      $ret[$t['id'] . '_b'] = $t['label_b_a'];
    }
  }
  return $ret;
}

/**
 * List dedupe rules available for a contact type
 *
 * @param string $contact_type
 * @return array
 */
function wf_crm_get_matching_rules($contact_type) {
  static $rules;
  $contact_type = ucfirst($contact_type);
  if (!$rules) {
    $rules = array_fill_keys(array('Individual', 'Organization', 'Household'), array());
    $dao = CRM_Core_DAO::executeQuery('SELECT * FROM civicrm_dedupe_rule_group');
    while ($dao->fetch()) {
      $rules[$dao->contact_type][$dao->id] = $dao->title;
    }
  }
  return $rules[$contact_type];
}

/**
 * Get ids or values of enabled CiviCRM fields for a webform.
 *
 * @param stdClass $node
 *   Node object
 * @param array|null $submission
 *   (optional) if supplied, will match field keys with submitted values
 * @param boolean $show_all
 *   (optional) if true, get every field even if it belongs to a contact that does not exist
 *
 * @return array of enabled fields
 */
function wf_crm_enabled_fields($node, $submission = NULL, $show_all = FALSE) {
  $enabled = array();
  if (!empty($node->webform['components']) && (!empty($node->webform_civicrm) || $show_all)) {
    $fields = wf_crm_get_fields();
    foreach ($node->webform['components'] as $c) {
      $exp = explode('_', $c['form_key'], 5);
      if (count($exp) == 5) {
        list($lobo, $i, $ent, $n, $id) = $exp;
        if ((isset($fields[$id]) || $id == 'fieldset_fieldset') && $lobo == 'civicrm' && is_numeric($i) && is_numeric($n)) {
          if (!$show_all && ($ent == 'contact' || $ent == 'participant') && empty($node->webform_civicrm['data']['contact'][$i])) {
            continue;
          }
          if ($submission) {
            $enabled[$c['form_key']] = wf_crm_aval($submission, $c['cid'], NULL, TRUE);
          }
          else {
            $enabled[$c['form_key']] = $c['cid'];
          }
        }
      }
    }
  }
  return $enabled;
}

/**
 * Fetches CiviCRM field data.
 *
 * @param string $var
 *   Name of variable to return: fields, tokens, or sets
 *
 * @return array
 *   fields: The CiviCRM contact fields this module supports
 *   tokens: Available tokens keyed to field ids
 *   sets: Info on fieldsets (entities)
 */
function wf_crm_get_fields($var = 'fields') {
  static $fields = array();
  static $tokens;
  static $sets;

  if (!$fields) {
    $config = CRM_Core_Config::singleton();
    $components = $config->enableComponents;

    $sets = array(
      'contact' => array('entity_type' => 'contact', 'label' => t('Contact Fields')),
      'other' => array('entity_type' => 'contact', 'label' => t('Tags and Groups'), 'max_instances' => 1),
      'address' => array('entity_type' => 'contact', 'label' => t('Address'), 'max_instances' => 9),
      'phone' => array('entity_type' => 'contact', 'label' => t('Phone'), 'max_instances' => 9),
      'email' => array('entity_type' => 'contact', 'label' => t('Email'), 'max_instances' => 9),
      'website' => array('entity_type' => 'contact', 'label' => t('Website'), 'max_instances' => 9),
      'im' => array('entity_type' => 'contact', 'label' => t('Instant Message'), 'max_instances' => 9),
      'activity' => array('entity_type' => 'activity', 'label' => t('Activity'), 'max_instances' => 30,  'attachments' => TRUE),
      'relationship' => array('entity_type' => 'contact', 'label' => t('Relationship'), 'help_text' => TRUE),
    );
    $conditional_sets = array(
      'CiviCase' => array('entity_type' => 'case', 'label' => t('Case Settings'), 'max_instances' => 30),
      'CiviEvent' => array('entity_type' => 'participant', 'label' => t('Participant'), 'max_instances' => 9),
      'CiviContribute' => array('entity_type' => 'contribution', 'label' => t('Contribution')),
      'CiviMember' => array('entity_type' => 'membership', 'label' => t('Membership')),
      'CiviGrant' => array('entity_type' => 'grant', 'label' => t('Grant'), 'max_instances' => 30, 'attachments' => TRUE),
    );
    foreach ($conditional_sets as $component => $set) {
      if (in_array($component, $components)) {
        $sets[$set['entity_type']] = $set;
      }
    }

    $moneyDefaults = array(
      'type' => 'number',
      'data_type' => 'Money',
      'extra' => array(
        'field_prefix' => wf_crm_aval($config, 'defaultCurrencySymbol', '$'),
        'point' => wf_crm_aval($config, 'monetaryDecimalPoint', '.'),
        'separator' => wf_crm_aval($config, 'monetaryThousandSeparator', ','),
        'decimals' => 2,
        'min' => 0,
      ),
    );

    // Field keys are in the format table_column
    // Use a # sign as a placeholder for field number in the title (or by default it will be appended to the end)
    // Setting 'expose_list' allows the value to be set on the config form
    // Set label for 'empty_option' for exposed lists that do not require input
    $fields['contact_contact_sub_type'] = array(
      'name' => t('Type of @contact'),
      'type' => 'select',
      'extra' => array('multiple' => 1, 'civicrm_live_options' => 1),
      'expose_list' => TRUE,
    );
    $fields['contact_existing'] = array(
      'name' => t('Existing Contact'),
      'type' => 'civicrm_contact',
      'extra' => array(
        'search_prompt' => t('- Choose existing -'),
      ),
    );
    // Organization / household names
    foreach (array('organization' => t('Organization Name'), 'legal' => t('Legal Name'), 'household' => t('Household Name')) as $key => $label) {
      $fields['contact_' . $key . '_name'] = array(
        'name' => $label,
        'type' => 'textfield',
        'contact_type' => $key == 'household' ? 'household' : 'organization',
      );
    }
    $fields['contact_sic_code'] = array(
      'name' => t('SIC Code'),
      'type' => 'textfield',
      'contact_type' => 'organization',
    );
    // Individual names
    if (version_compare(CRM_Utils_System::version(), '4.5', '<')) {
      // This setting doesn't exist prior to 4.5 so hard-code it.
      $enabled_names = array('Prefix', 'Suffix', 'First Name', 'Middle Name', 'Last Name');
    }
    else {
      $enabled_names = wf_crm_explode_multivalue_str(CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME, 'contact_edit_options'));
      $name_options = CRM_Core_OptionGroup::values('contact_edit_options', FALSE, FALSE, FALSE, NULL, 'name');
      $enabled_names = array_intersect_key($name_options, array_flip($enabled_names));
    }
    foreach (array('prefix_id' => t('Name Prefix'), 'formal_title' => t('Formal Title'), 'first_name' => t('First Name'), 'middle_name' => t('Middle Name'), 'last_name' => t('Last Name'), 'suffix_id' => t('Name Suffix')) as $key => $label) {
      if (in_array(ucwords(str_replace(array('_id', '_'), array('', ' '), $key)), $enabled_names)) {
        $fields['contact_' . $key] = array(
          'name' => $label,
          'type' => strpos($key, '_id') ? 'select' : 'textfield',
          'contact_type' => 'individual',
        );
      }
    }
    $fields['contact_nick_name'] = array(
      'name' => t('Nickname'),
      'type' => 'textfield',
    );
    $fields['contact_gender_id'] = array(
      'name' => t('Gender'),
      // Gender should be textfield if using https://civicrm.org/extensions/gender-self-identify
      'type' => function_exists('genderselfidentify_civicrm_apiWrappers') ? 'textfield' : 'select',
      'contact_type' => 'individual',
    );
    $fields['contact_job_title'] = array(
      'name' => t('Job Title'),
      'type' => 'textfield',
      'contact_type' => 'individual',
    );
    $fields['contact_birth_date'] = array(
      'name' => t('Birth Date'),
      'type' => 'date',
      'extra' => array(
        'start_date' => '-100 years',
        'end_date' => 'now',
      ),
      'contact_type' => 'individual',
    );
    $fields['contact_preferred_communication_method'] = array(
      'name' => t('Preferred Communication Method(s)'),
      'type' => 'select',
      'extra' => array('multiple' => 1),
    );
    $fields['contact_privacy'] = array(
      'name' => t('Privacy Preferences'),
      'type' => 'select',
      'extra' => array('multiple' => 1),
    );
    $fields['contact_preferred_language'] = array(
      'name' => t('Preferred Language'),
      'type' => 'select',
      'value' => $config->lcMessages,
    );
    if (array_key_exists('file', webform_components())) {
      $fields['contact_image_URL'] = array(
        'name' => t('Upload Image'),
        'type' => 'file',
        'extra' => array('width' => 40),
        'data_type' => 'File',
      );
    }
    $fields['contact_contact_id'] = array(
      'name' => t('Contact ID'),
      'type' => 'hidden',
    );
    $fields['contact_external_identifier'] = array(
      'name' => t('External ID'),
      'type' => 'hidden',
    );
    $fields['contact_source'] = array(
      'name' => t('Source'),
      'type' => 'textfield',
    );
    $fields['contact_cs'] = array(
      'name' => t('Checksum'),
      'type' => 'hidden',
      'value_callback' => TRUE,
    );
    $fields['contact_employer_id'] = array(
      'name' => t('Current Employer'),
      'type' => 'select',
      'expose_list' => TRUE,
      'empty_option' => t('None'),
      'data_type' => 'ContactReference',
      'contact_type' => 'individual',
      'reference_contact_type' => 'organization'
    );
    $fields['contact_is_deceased'] = array(
      'name' => t('Is Deceased'),
      'type' => 'select',
      'extra' => array('aslist' => 0),
      'contact_type' => 'individual',
    );
    $fields['contact_deceased_date'] = array(
      'name' => t('Deceased Date'),
      'type' => 'date',
      'extra' => array(
        'start_date' => '-100 years',
        'end_date' => 'now',
      ),
      'contact_type' => 'individual',
    );
    $fields['email_email'] = array(
      'name' => t('Email'),
      'type' => 'email',
    );
    foreach (array('street_address' => t('Street Address'), 'supplemental_address_1' => t('Street Address # Line 2'), 'supplemental_address_2' => t('Street Address # Line 3'), 'city' => t('City')) as $key => $value) {
      $fields['address_' . $key] = array(
        'name' => $value,
        'type' => 'textfield',
        'extra' => array('width' => $key == 'city' ? 20 : 60),
      );
    }
    $fields['address_postal_code'] = array(
      'name' => t('Postal Code'),
      'type' => 'textfield',
      'extra' => array('width' => 7),
    );
    $fields['address_postal_code_suffix'] = array(
      'name' => t('Postal Code Suffix'),
      'type' => 'textfield',
      'extra' => array(
        'width' => 5,
        'description' => t('+4 digits of Zip Code'),
      ),
    );
    $fields['address_country_id'] = array(
      'name' => t('Country'),
      'type' => 'select',
      'extra' => array('civicrm_live_options' => 1),
      'value' => $config->defaultContactCountry,
    );
    $fields['address_state_province_id'] = array(
      'name' => t('State/Province'),
      'type' => 'textfield',
      'extra' => array(
        'maxlength' => 5,
        'width' => 4,
      ),
      'data_type' => 'state_province_abbr',
    );
    $fields['address_county_id'] = array(
      'name' => t('District/County'),
      'type' => 'textfield',
    );
    $fields['address_master_id'] = array(
      'name' => t('Share address of'),
      'type' => 'select',
      'expose_list' => TRUE,
      'extra' => array('aslist' => 0),
      'empty_option' => t('Do Not Share'),
    );
    $fields['phone_phone'] = array(
      'name' => t('Phone Number'),
      'type' => 'textfield',
    );
    $fields['phone_phone_ext'] = array(
      'name' => t('Phone Extension'),
      'type' => 'textfield',
      'extra' => array(
        'width' => 4,
      ),
    );
    $fields['phone_phone_type_id'] = array(
      'name' => t('Phone # Type'),
      'type' => 'select',
      'table' => 'phone',
      'expose_list' => TRUE,
    );
    $fields['im_name'] = array(
      'name' => t('Screen Name'),
      'type' => 'textfield',
    );
    $fields['im_provider_id'] = array(
      'name' => t('IM Provider'),
      'type' => 'select',
      'expose_list' => TRUE,
    );
    foreach (array('address' => t('Address # Location'), 'phone' => t('Phone # Location'), 'email' => t('Email # Location'), 'im' => t('IM # Location')) as $key => $label) {
      if (isset($sets[$key])) {
        $fields[$key . '_location_type_id'] = array(
          'name' => $label,
          'type' => 'select',
          'expose_list' => TRUE,
          'value' => '1',
        );
      }
    }
    $fields['website_url'] = array(
      'name' => t('Website'),
      'type' => 'textfield',
      'data_type' => 'Link',
    );
    $fields['website_website_type_id'] = array(
      'name' => t('Website # Type'),
      'type' => 'select',
      'expose_list' => TRUE,
    );
    $fields['other_group'] = array(
      'name' => t('Group(s)'),
      'type' => 'select',
      'extra' => array('multiple' => 1, 'civicrm_live_options' => 1),
      'table' => 'group',
      'expose_list' => TRUE,
    );
    $tagsets = array('' => t('Tag(s)')) + CRM_Core_BAO_Tag::getTagSet('civicrm_contact');
    foreach ($tagsets as $pid => $name) {
      $fields['other_tag' . ($pid ? "_$pid" : '')] = array(
        'name' => $name,
        'type' => 'select',
        'extra' => array('multiple' => 1, 'civicrm_live_options' => 1),
        'table' => 'tag',
        'expose_list' => TRUE,
      );
    }
    $fields['activity_activity_type_id'] = array(
      'name' => t('Activity # Type'),
      'type' => 'select',
      'expose_list' => TRUE,
    );
    $fields['activity_target_contact_id'] = array(
      'name' => t('Activity # Participant(s)'),
      'type' => 'select',
      'expose_list' => TRUE,
      'extra' => array(
        'multiple' => 1,
      ),
      'data_type' => 'ContactReference',
    );
    $fields['activity_source_contact_id'] = array(
      'name' => t('Activity # Creator'),
      'type' => 'select',
      'expose_list' => TRUE,
      'data_type' => 'ContactReference',
      'exposed_empty_option' => '- ' . t('Automatic') . ' -',
    );
    $fields['activity_subject'] = array(
      'name' => t('Activity # Subject'),
      'type' => 'textfield',
    );
    $fields['activity_details'] = array(
      'name' => t('Activity # Details'),
      'type' => module_exists('webform_html_textarea') ? 'html_textarea' : 'textarea',
    );
    $fields['activity_status_id'] = array(
      'name' => t('Activity # Status'),
      'type' => 'select',
      'expose_list' => TRUE,
      'exposed_empty_option' => '- ' . t('Automatic') . ' -',
    );
    $fields['activity_priority_id'] = array(
      'name' => t('Activity # Priority'),
      'type' => 'select',
      'expose_list' => TRUE,
      'exposed_empty_option' => '- ' . t('Automatic') . ' -',
    );
    $fields['activity_assignee_contact_id'] = array(
      'name' => t('Assign Activity # to'),
      'type' => 'select',
      'expose_list' => TRUE,
      'empty_option' => t('No One'),
      'data_type' => 'ContactReference',
    );
    $fields['activity_location'] = array(
      'name' => t('Activity # Location'),
      'type' => 'textfield',
    );
    $fields['activity_activity_date_time'] = array(
      'name' => t('Activity # Date'),
      'type' => 'date',
      'value' => 'now',
    );
    $fields['activity_activity_date_time_timepart'] = array(
      'name' => t('Activity # Time'),
      'type' => 'time',
      'value' => 'now',
    );
    $fields['activity_duration'] = array(
      'name' => t('Activity # Duration'),
      'type' => 'number',
      'extra' => array(
        'field_suffix' => t('min.'),
        'min' => 0,
        'step' => 5,
        'integer' => 1,
      ),
    );
    if (isset($sets['case'])) {
      $case_info = new CRM_Case_XMLProcessor_Process();
      $fields['case_case_type_id'] = array(
        'name' => t('Case # Type'),
        'type' => 'select',
        'expose_list' => TRUE,
      );
      $fields['case_client_id'] = array(
        'name' => t('Case # Client'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('required' => 1, 'multiple' => $case_info->getAllowMultipleCaseClients()),
        'data_type' => 'ContactReference',
        'set' => 'caseRoles',
        'value' => 1,
      );
      $fields['case_status_id'] = array(
        'name' => t('Case # Status'),
        'type' => 'select',
        'expose_list' => TRUE,
        'exposed_empty_option' => '- ' . t('Automatic') . ' -',
      );
      $fields['case_medium_id'] = array(
        'name' => t('Medium'),
        'type' => 'select',
        'expose_list' => TRUE,
      );
      $fields['case_subject'] = array(
        'name' => t('Case # Subject'),
        'type' => 'textfield',
      );
      $fields['case_creator_id'] = array(
        'name' => t('Case # Creator'),
        'type' => 'select',
        'expose_list' => TRUE,
        'data_type' => 'ContactReference',
        'set' => 'caseRoles',
        'exposed_empty_option' => '- ' . t('Automatic') . ' -',
      );
      // Fetch case roles
      $sets['caseRoles'] = array('entity_type' => 'case', 'label' => t('Case Roles'));
      // Use the vanilla civicrm_api for this because it will throw an error in CiviCRM 4.4 (api doesn't exist)
      $case_types = civicrm_api('case_type', 'get', array('version' => 3, 'options' => array('limit' => 0)));
      foreach (wf_crm_aval($case_types, 'values', array()) as $case_type) {
        foreach ($case_type['definition']['caseRoles'] as $role) {
          foreach (wf_crm_get_relationship_types() as $rel_type) {
            if ($rel_type['name_b_a'] == $role['name']) {
              if (!isset($fields['case_role_' . $rel_type['id']])) {
                $fields['case_role_' . $rel_type['id']] = array(
                  'name' => $rel_type['label_b_a'],
                  'type' => 'select',
                  'expose_list' => TRUE,
                  'data_type' => 'ContactReference',
                  'set' => 'caseRoles',
                  'empty_option' => t('None'),
                );
              }
              $fields['case_role_' . $rel_type['id']]['case_types'][] = $case_type['id'];
              break;
            }
          }
        }
      }
    }
    $fields['relationship_relationship_type_id'] = array(
      'name' => t('Relationship Type(s)'),
      'type' => 'select',
      'expose_list' => TRUE,
      'extra' => array(
        'civicrm_live_options' => 1,
        'multiple' => 1,
      ),
    );
    $fields['relationship_is_active'] = array(
      'name' => t('Is Active'),
      'type' => 'select',
      'expose_list' => TRUE,
      'value' => '1',
    );
    $fields['relationship_relationship_permission'] = array(
      'name' => t('Permissions'),
      'type' => 'select',
      'expose_list' => TRUE,
      'empty_option' => t('No Permissions'),
    );
    $fields['relationship_start_date'] = array(
      'name' => t('Start Date'),
      'type' => 'date',
      'extra' => array(
        'start_date' => '-50 years',
        'end_date' => '+10 years',
      ),
    );
    $fields['relationship_end_date'] = array(
      'name' => t('End Date'),
      'type' => 'date',
      'extra' => array(
        'start_date' => '-50 years',
        'end_date' => '+10 years',
      ),
    );
    $fields['relationship_description'] = array(
      'name' => t('Description'),
      'type' => 'textarea',
    );
    if (isset($sets['contribution'])) {
      $fields['contribution_contribution_page_id'] = array(
        'name' => ts('Contribution Page'),
        'type' => 'hidden',
        'expose_list' => TRUE,
        'empty_option' => t('None'),
        'extra' => array(
          'hidden_type' => 'hidden',
        ),
        'weight' => 9999,
      );
      $fields['contribution_total_amount'] = array(
        'name' => t('Contribution Amount'),
        'weight' => 9991,
      ) + $moneyDefaults;
      $fields['contribution_payment_processor_id'] = array(
        'name' => t('Payment Processor'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('aslist' => 0),
        'exposed_empty_option' => t('Pay Later'),
        'value_callback' => TRUE,
        'weight' => 9995,
      );
      $fields['contribution_note'] = array(
        'name' => t('Contribution Note'),
        'type' => 'textarea',
        'weight' => 9993,
      );
      $fields['contribution_soft'] = array(
        'name' => t('Soft Credit To'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('multiple' => TRUE),
        'data_type' => 'ContactReference',
      );
      $fields['contribution_honor_contact_id'] = array(
        'name' => t('In Honor/Memory of'),
        'type' => 'select',
        'expose_list' => TRUE,
        'empty_option' => t('No One'),
        'data_type' => 'ContactReference',
      );
      $fields['contribution_honor_type_id'] = array(
        'name' => t('Honoree Type'),
        'type' => 'select',
        'expose_list' => TRUE,
      );
      $fields['contribution_is_test'] = array(
        'name' => t('Payment Processor Mode'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('civicrm_live_options' => 1),
        'value' => 0,
        'weight' => 9997,
      );
      $fields['contribution_source'] = array(
        'name' => t('Contribution Source'),
        'type' => 'textfield',
      );
    }
    if (isset($sets['participant'])) {
      $fields['participant_event_id'] = array(
        'name' => t('Event(s)'),
        'type' => 'select',
        'extra' => array('multiple' => 1, 'civicrm_live_options' => 1),
        'expose_list' => TRUE,
      );
      $fields['participant_role_id'] = array(
        'name' => t('Participant Role'),
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => '1',
        'extra' => array('multiple' => 1, 'required' => 1),
      );
      $fields['participant_status_id'] = array(
        'name' => t('Registration Status'),
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => 0,
        'exposed_empty_option' => '- ' . t('Automatic') . ' -',
      );
      if (isset($sets['contribution'])) {
        $fields['participant_fee_amount'] = array(
          'name' => t('Participant Fee'),
        ) + $moneyDefaults;
      }
    }
    if (isset($sets['membership'])) {
      $fields['membership_membership_type_id'] = array(
        'name' => t('Membership Type'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('civicrm_live_options' => 1),
      );
      $fields['membership_status_id'] = array(
        'name' => t('Membership Status'),
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => 0,
        'exposed_empty_option' => '- ' . t('Automatic') . ' -',
      );
      $fields['membership_num_terms'] = array(
        'name' => t('Number of Terms'),
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => 1,
        'empty_option' => t('Enter Dates Manually'),
      );
      if (isset($sets['contribution'])) {
        $fields['membership_fee_amount'] = array(
          'name' => t('Membership Fee'),
        ) + $moneyDefaults;
      }
      $fields['membership_join_date'] = array(
        'name' => t('Member Since'),
        'type' => 'date',
      );
      $fields['membership_start_date'] = array(
        'name' => t('Start Date'),
        'type' => 'date',
      );
      $fields['membership_end_date'] = array(
        'name' => t('End Date'),
        'type' => 'date',
      );
    }
    // Add campaign fields
    if (in_array('CiviCampaign', $components)) {
      $fields['activity_engagement_level'] = array(
        'name' => t('Engagement Level'),
        'type' => 'select',
        'empty_option' => t('None'),
        'expose_list' => TRUE,
      );
      $fields['activity_survey_id'] = array(
        'name' => t('Survey/Petition'),
        'type' => 'select',
        'expose_list' => TRUE,
        'empty_option' => t('None'),
        'extra' => array('civicrm_live_options' => 1),
      );
      foreach (array_intersect(array('activity', 'membership', 'participant', 'contribution'), array_keys($sets)) as $ent) {
        $fields[$ent . '_campaign_id'] = array(
          'name' => t('Campaign'),
          'type' => 'select',
          'expose_list' => TRUE,
          'extra' => array('civicrm_live_options' => 1),
          'empty_option' => t('None'),
        );
      }
    }
    // CiviGrant fields
    if (isset($sets['grant'])) {
      $fields['grant_contact_id'] = array(
        'name' => t('Grant Applicant'),
        'type' => 'select',
        'expose_list' => TRUE,
        'data_type' => 'ContactReference',
      );
      $fields['grant_grant_type_id'] = array(
        'name' => t('Grant Type'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('civicrm_live_options' => 1),
      );
      $fields['grant_status_id'] = array(
        'name' => t('Grant Status'),
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => 0,
        'exposed_empty_option' => '- ' . t('Automatic') . ' -',
      );
      $fields['grant_application_received_date'] = array(
        'name' => t('Application Received Date'),
        'type' => 'date',
      );
      $fields['grant_decision_date'] = array(
        'name' => t('Decision Date'),
        'type' => 'date',
      );
      $fields['grant_money_transfer_date'] = array(
        'name' => t('Money Transfer Date'),
        'type' => 'date',
      );
      $fields['grant_grant_due_date'] = array(
        'name' => t('Grant Report Due'),
        'type' => 'date',
      );
      $fields['grant_grant_report_received'] = array(
        'name' => t('Grant Report Received?'),
        'type' => 'select',
        'extra' => array('aslist' => 0),
      );
      $fields['grant_rationale'] = array(
        'name' => t('Grant Rationale'),
        'type' => 'textarea',
      );
      $fields['grant_note'] = array(
        'name' => t('Grant Notes'),
        'type' => 'textarea',
      );
      $fields['grant_amount_total'] = array(
        'name' => t('Amount Requested'),
      ) + $moneyDefaults;
      $fields['grant_amount_granted'] = array(
        'name' => t('Amount Granted'),
      ) + $moneyDefaults;
    }

    // File attachment fields
    $numAttachments = CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME, 'max_attachments');
    foreach ($sets as $ent => $set) {
      if (!empty($set['attachments']) && $numAttachments) {
        $sets["{$ent}upload"] = array(
          'label' => t('File Attachments'),
          'entity_type' => $ent,
        );
        for ($i = 1; $i <= $numAttachments; $i++) {
          $fields["{$ent}upload_file_$i"] = array(
            'name' => t('Attachment !num', array('!num' => $i)),
            'type' => 'file',
            'data_type' => 'File',
          );
        }
      }
    }

    $tokens = array(
      'display_name' => t('display name'),
      'first_name' => t('first name'),
      'nick_name' => t('nickname'),
      'middle_name' => t('middle name'),
      'last_name' => t('last name'),
      'individual_prefix' => t('name prefix'),
      'individual_suffix' => t('name suffix'),
      'gender' => t('gender'),
      'birth_date' => t('birth date'),
      'job_title' => t('job title'),
      'current_employer' => t('current employer'),
      'contact_id' => t('contact id'),
      'street_address' => t('street address'),
      'city' => t('city'),
      'state_province' => t('state/province abbr'),
      'state_province_name' => t('state/province full'),
      'postal_code' => t('postal code'),
      'country' => t('country'),
      'world_region' => t('world region'),
      'phone' => t('phone number'),
      'email' => t('email'),
    );

    // Pull custom fields and match to Webform element types
    $custom_types = array(
      'Select' => array('type' => 'select'),
      'Multi-Select' => array('type' => 'select', 'extra' => array('multiple' => 1)),
      'AdvMulti-Select' => array('type' => 'select', 'extra' => array('multiple' => 1)),
      'Radio' => array('type' => 'select', 'extra' => array('aslist' => 0)),
      'CheckBox' => array('type' => 'select', 'extra' => array('multiple' => 1)),
      'Text'  => array('type' => 'textfield'),
      'TextArea' => array('type' => 'textarea'),
      'RichTextEditor' => array('type' => module_exists('webform_html_textarea') ? 'html_textarea' : 'textarea'),
      'Select Date' => array('type' => 'date'),
      'Link'  => array('type' => 'textfield'),
      'Select Country' => array('type' => 'select'),
      'Multi-Select Country' => array('type' => 'select', 'extra' => array('multiple' => 1)),
      'Select State/Province' => array('type' => 'select'),
      'Multi-Select State/Province' => array('type' => 'select', 'extra' => array('multiple' => 1)),
      'Autocomplete-Select' => array('type' => 'select'),
      'File' => array('type' => 'file'),
    );
    list($contact_types) = wf_crm_get_contact_types();
    $custom_extends = "'" . implode("','", array_keys($contact_types + $sets)) . "'";
    $sql = "
      SELECT cf.*, cg.title AS custom_group_name, LOWER(cg.extends) AS entity_type, cg.extends_entity_column_id, cg.extends_entity_column_value AS sub_types, cg.is_multiple, cg.max_multiple, cg.id AS custom_group_id, cg.help_pre as cg_help
      FROM civicrm_custom_field cf
      INNER JOIN civicrm_custom_group cg ON cg.id = cf.custom_group_id
      WHERE cf.is_active <> 0 AND cg.extends IN ($custom_extends) AND cg.is_active <> 0
      ORDER BY cf.custom_group_id, cf.weight";
    $dao = CRM_Core_DAO::executeQuery($sql);
    while ($dao->fetch()) {
      if (isset($custom_types[$dao->html_type])) {
        $set = 'cg' . $dao->custom_group_id;
        if ($dao->entity_type == 'address' || $dao->entity_type == 'relationship' || $dao->entity_type == 'membership') {
          $set = $dao->entity_type;
        }
        elseif (!isset($sets[$set])) {
          $sets[$set]['label'] = $dao->custom_group_name;
          if (isset($contact_types[$dao->entity_type]) || $dao->entity_type == 'contact') {
            $sets[$set]['entity_type'] = 'contact';
            if ($dao->entity_type != 'contact') {
              $sets[$set]['contact_type'] = $dao->entity_type;
            }
            if ($dao->is_multiple) {
              $sets[$set]['max_instances'] = ($dao->max_multiple ? $dao->max_multiple : 9);
            }
            else {
              $sets[$set]['max_instances'] = 1;
            }
          }
          else {
            $sets[$set]['entity_type'] = $dao->entity_type;
          }
          if ($dao->sub_types) {
            $sets[$set]['sub_types'] = wf_crm_explode_multivalue_str($dao->sub_types);
          }
          if ($dao->extends_entity_column_id) {
            $sets[$set]['extension_of'] = $dao->extends_entity_column_id;
          }
          $sets[$set]['help_text'] = $dao->cg_help;
        }
        $id = $set . '_custom_' . $dao->id;
        $fields[$id] = $custom_types[$dao->html_type];
        if ($dao->html_type == 'Text' && $dao->data_type == 'Money') {
          $fields[$id] = $moneyDefaults;
        }
        $fields[$id]['name'] = $dao->label;
        $fields[$id]['required'] = $dao->is_required;
        $fields[$id]['extra']['description'] = $dao->help_pre;
        $fields[$id]['value'] = implode(',', wf_crm_explode_multivalue_str($dao->default_value));
        $fields[$id]['data_type'] = $dao->data_type;
        if (!empty($dao->help_pre) || !empty($dao->help_post)) {
          $fields[$id]['extra']['description'] = $dao->help_pre ? $dao->help_pre : $dao->help_post;
          $fields[$id]['has_help'] = TRUE;
        }
        // Conditional rule - todo: support additional entities
        if ($sets[$set]['entity_type'] == 'contact' && !empty($sets[$set]['sub_types'])) {
          $fields[$id]['civicrm_condition'] = array(
            'andor' => 'or',
            'action' => 'show',
            'rules' => array(
              'contact_contact_sub_type' => $sets[$set]['sub_types'],
            ),
          );
        }

        if ($dao->entity_type == 'relationship' && $dao->sub_types) {
          $fields[$id]['attributes']['data-relationship-type'] = implode(',', wf_crm_explode_multivalue_str($dao->sub_types));
        }

        if ($fields[$id]['type'] == 'date') {
          $fields[$id]['extra']['start_date'] = ($dao->start_date_years ? '-' . $dao->start_date_years : '-50') . ' years';
          $fields[$id]['extra']['end_date'] = ($dao->end_date_years ? '+' . $dao->end_date_years : '+50') . ' years';
          // Add "time" component for datetime fields
          if (!empty($dao->time_format)) {
            $fields[$id]['name'] .= ' - ' . t('date');
            $fields[$id . '_timepart'] = array(
              'name' => $dao->label . ' - ' . t('time'),
              'type' => 'time',
              'extra' => array('hourformat' => $dao->time_format == 1 ? '12-hour' : '24-hour'),
            );
          }
        }
        elseif ($fields[$id]['data_type'] == 'ContactReference') {
          $fields[$id]['expose_list'] = TRUE;
          $fields[$id]['empty_option'] = t('None');
        }
        elseif ($fields[$id]['data_type'] !== 'Boolean' && $fields[$id]['type'] == 'select') {
          $fields[$id]['extra']['civicrm_live_options'] = 1;
        }
        elseif ($fields[$id]['type'] == 'textarea') {
          $fields[$id]['extra']['cols'] = $dao->note_columns;
          $fields[$id]['extra']['rows'] = $dao->note_rows;
        }
      }
    }
    $dao->free();
  }
  return $$var;
}

/**
 * Get a field based on its short or full name
 * @param string $key
 * @return array|null
 */
function wf_crm_get_field($key) {
  $fields = wf_crm_get_fields();
  if (isset($fields[$key])) {
    return $fields[$key];
  }
  if ($pieces = wf_crm_explode_key($key)) {
    list( , , , , $table, $name) = $pieces;
    if (isset($fields[$table . '_' . $name])) {
      return $fields[$table . '_' . $name];
    }
  }
}

/**
 * Lookup a uf ID from contact ID or vice-versa
 * With no arguments passed in, this function will return the contact_id of the current logged-in user
 *
 * @param $id
 *   (optional) uf or contact ID - defaults to current user
 * @param $type
 *   (optional) what type of ID is supplied - defaults to user id
 * @return null
 */
function wf_crm_user_cid($id = NULL, $type = 'uf') {
  static $current_user = NULL;
  if (!$id) {
    if ($current_user !== NULL) {
      return $current_user;
    }
    global $user;
    $id = $user_lookup = $user->uid;
  }
  if (!$id || !is_numeric($id)) {
    return NULL;
  }
  // Lookup current domain for multisite support
  static $domain = 0;
  if (!$domain) {
    $domain = wf_civicrm_api('domain', 'get', array('current_domain' => 1, 'return' => 'id'));
    $domain = wf_crm_aval($domain, 'id', 1);
  }
  $result = wf_crm_apivalues('uf_match', 'get', array(
    $type . '_id' => $id,
    'domain_id' => $domain,
    'sequential' => 1,
  ));
  if ($result) {
    if (!empty($user_lookup)) {
      $current_user = $result[0]['contact_id'];
    }
    return $type == 'uf' ? $result[0]['contact_id'] : $result[0]['uf_id'];
  }
}

/**
 * Fetch contact display name
 *
 * @param $cid
 *   Contact id
 *
 * @return string
 */
function wf_crm_display_name($cid) {
  if (!$cid || !is_numeric($cid)) {
    return '';
  }
  civicrm_initialize();
  $result = wf_civicrm_api('contact', 'get', array('id' => $cid, 'return.display_name' => 1, 'is_deleted' => 0));
  return check_plain(wf_crm_aval($result, "values:$cid:display_name", ''));
}

/**
 * @param integer $n
 * @param array $data Form data
 * @param string $html Controls how html should be treated. Options are:
 *  * 'escape': (default) Escape html characters
 *  * 'wrap': Escape html characters and wrap in a span
 *  * 'plain': Do not escape (use when passing into an FAPI options list which does its own escaping)
 * @return string
 */
function wf_crm_contact_label($n, $data = array(), $html = 'escape') {
  $label = trim(wf_crm_aval($data, "contact:$n:contact:1:webform_label", ''));
  if (!$label) {
    $label = t('Contact !num', array('!num' => $n));
  }
  if ($html != 'plain') {
    $label = check_plain($label);
  }
  if ($html == 'wrap') {
    $label = '<span class="contact-label number-' . $n . '">' . $label . '</span>';
  }
  return $label;
}

/**
 * Explodes form key into an array and verifies that it is in the right format
 *
 * @param $key
 *   Webform component field key (string)
 *
 * @return array or NULL
 */
function wf_crm_explode_key($key) {
  $pieces = explode('_', $key, 6);
  if (count($pieces) != 6 || $pieces[0] !== 'civicrm') {
    return FALSE;
  }
  return $pieces;
}

/**
 * Convert a | separated string into an array
 *
 * @param string $str
 *   String representation of key => value select options
 *
 * @return array of select options
 */
function wf_crm_str2array($str) {
  $ret = array();
  if ($str) {
    foreach (explode("\n", trim($str)) as $row) {
      if ($row && $row[0] !== '<' && strpos($row, '|')) {
        list($k, $v) = explode('|', $row);
        $ret[trim($k)] = trim($v);
      }
    }
  }
  return $ret;
}

/**
 * Convert an array into a | separated string
 *
 * @param array $arr
 *   Array of select options
 *
 * @return string
 *   String representation of key => value select options
 */
function wf_crm_array2str($arr) {
  $str = '';
  foreach ($arr as $k => $v) {
    $str .= ($str ? "\n" : '') . $k . '|' . $v;
  }
  return $str;
}

/**
 * Wrapper for all CiviCRM API calls
 * For consistency, future-proofing, and error handling
 *
 * @param string $entity
 *   API entity
 * @param string $operation
 *   API operation
 * @param array $params
 *   API params
 *
 * @return array
 *   Result of API call
 */
function wf_civicrm_api($entity, $operation, $params) {
  $params += array(
    'check_permissions' => FALSE,
    'version' => 3
  );
  $result = civicrm_api($entity, $operation, $params);
  // I guess we want silent errors for getoptions b/c we check it for failure separately
  if (!empty($result['is_error']) && $operation != 'getoptions') {
    $bt = debug_backtrace();
    $n = $bt[0]['function'] == 'wf_civicrm_api' ? 1 : 0;
    $file = explode('/', $bt[$n]['file']);
    if (isset($params['credit_card_number'])) {
      $params['credit_card_number'] = "xxxxxxxxxxxx".substr($params['credit_card_number'], -4);
    }
    watchdog('webform_civicrm',
      'The CiviCRM "%function" API returned the error: "%msg" when called by function "!fn" on line !line of !file with parameters: "!params"',
      array(
        '%function' => $entity . ' ' . $operation,
        '%msg' => $result['error_message'],
        '!fn' => $bt[$n+1]['function'],
        '!line' => $bt[$n]['line'],
        '!file' => array_pop($file),
        '!params' => print_r($params, TRUE),
      ),
      WATCHDOG_ERROR);
  }
  return $result;
}

/**
 * Get the values from an api call
 *
 * @param string $entity
 *   API entity
 * @param string $operation
 *   API operation
 * @param array $params
 *   API params
 * @param string $value
 *   Reduce each result to this single value
 *
 * @return array
 *   Values from API call
 */
function wf_crm_apivalues($entity, $operation, $params = array(), $value = NULL) {
  if (is_numeric($params)) {
    $params = array('id' => $params);
  }
  $params += array('options' => array());
  // Work around the api's default limit of 25
  $params['options'] += array('limit' => 0);
  $ret = wf_crm_aval(wf_civicrm_api($entity, $operation, $params), 'values', array());
  if ($value) {
    foreach ($ret as &$values) {
      $values = wf_crm_aval($values, $value);
    }
  }
  return $ret;
}

/**
 * Check if a name or email field exists for this contact.
 * This determines whether a new contact can be created on the webform.
 *
 * @param $enabled
 *   Array of enabled fields
 * @param $c
 *   Contact #
 * @param $contact_type
 *   Contact type
 * @return int
 */
function wf_crm_name_field_exists($enabled, $c, $contact_type) {
  foreach (wf_crm_required_contact_fields($contact_type) as $f) {
    $fid = 'civicrm_' . $c . '_contact_1_' . $f['table'] . '_' . $f['name'];
    if (!empty($enabled[$fid])) {
      return 1;
    }
  }
  return 0;
}

/**
 * At least one of these fields is required to create a contact
 *
 * @param string $contact_type
 * @return array of fields
 */
function wf_crm_required_contact_fields($contact_type) {
  if ($contact_type == 'individual') {
    return array(
      array('table' => 'email', 'name' => 'email'),
      array('table' => 'contact', 'name' => 'first_name'),
      array('table' => 'contact', 'name' => 'last_name'),
      array('table' => 'contact', 'name' => 'nick_name'),
    );
  }
  return array(array('table' => 'contact', 'name' => $contact_type . '_name'));
}

/**
 * These are the contact location fields this module supports
 *
 * @return array
 */
function wf_crm_location_fields() {
  return array('address', 'email', 'phone', 'website', 'im');
}

/**
 * These are the address fields this module supports
 *
 * @return array
 */
function wf_crm_address_fields() {
  return array(
    'street_address',
    'city',
    'state_province_id',
    'country_id',
    'postal_code'
  );
}

/**
 * Returns a count of children of a webform component
 *
 * @param int $nid
 * @param int $id
 * @return int
 */
function _wf_crm_child_components($nid, $id) {
  return db_select('webform_component', 'c')
    ->fields('c')
    ->condition('nid', $nid)
    ->condition('pid', $id)
    ->countQuery()
    ->execute()
    ->fetchField();
}

/**
 * @param string
 * @return array
 */
function wf_crm_explode_multivalue_str($str) {
  $sp = CRM_Core_DAO::VALUE_SEPARATOR;
  if (is_array($str)) {
    return $str;
  }
  return explode($sp, trim((string) $str, $sp));
}

/**
 * Check if value is a positive integer
 * @param mixed $val
 * @return bool
 */
function wf_crm_is_positive($val) {
  return is_numeric($val) && $val > 0 && round($val) == $val;
}
