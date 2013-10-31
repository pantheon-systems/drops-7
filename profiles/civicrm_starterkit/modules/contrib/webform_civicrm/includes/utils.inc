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
      $ret = wf_crm_get_surveys(wf_crm_aval($data, 'activity:1:activity:1', array()));
    }
    elseif ($name == 'event_id') {
      $ret = wf_crm_get_events($data['reg_options'], $context);
    }
    elseif ($table == 'contribution' && $name == 'is_test') {
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
  $format = explode(' ', wf_crm_aval($reg_options, 'title_display', 'title'));
  if (in_array('type', $format)) {
    $types = wf_crm_apivalues('event', 'getoptions', array('field' => 'event_type_id', 'context' => 'get'));
  }
  // Date format
  foreach ($format as $value) {
    if (strpos($value, 'dateformat') === 0) {
      $config = CRM_Core_Config::singleton();
      $date_format = $config->$value;
    }
  }
  $sql = "SELECT id, title, start_date, end_date, event_type_id FROM civicrm_event WHERE is_template = 0 AND is_active = 1";
  // 0 means only current events, 1 means show all past events, other values are relative date strings
  $date = wf_crm_aval($reg_options, 'show_past_events', 'now');
  if ($date != '1') {
    $date = date('Y-m-d H:i:s', strtotime($date));
    $sql .= " AND (end_date >= '$date' OR end_date IS NULL)";
  }
  if (is_numeric($reg_options['event_type'])) {
    $sql .= ' AND event_type_id = ' . $reg_options['event_type'];
  }
  $sql .= ' ORDER BY start_date ' . ($context == 'config_form' ? 'DESC' : '');
  $dao = CRM_Core_DAO::executeQuery($sql);
  while ($dao->fetch()) {
    $title = array();
    if (in_array('title', $format)) {
      $title[] = $dao->title;
    }
    if (in_array('type', $format)) {
      $title[] = $types[$dao->event_type_id];
    }
    if (in_array('start', $format) && $dao->start_date) {
      $title[] = CRM_Utils_Date::customFormat($dao->start_date, $date_format);
    }
    if (in_array('end', $format) && $dao->end_date) {
      // Avoid showing redundant end-date if it is the same as the start date
      $same_day = substr($dao->start_date, 0, 10) == substr($dao->end_date, 0, 10);
      if (!$same_day || in_array('dateformatDatetime', $format) || in_array('dateformatTime', $format)) {
        $end_format = (in_array('dateformatDatetime', $format) && $same_day) ? $config->dateformatTime : $date_format;
        $title[] = CRM_Utils_Date::customFormat($dao->end_date, $end_format);
      }
    }
    $ret[$dao->id . '-' . $dao->event_type_id] = implode(' - ', $title);
  }
  return $ret;
}

/**
 * Get list of surveys
 * @param array $act
 *
 * @return array
 */
function wf_crm_get_surveys($act = array()) {
  $params = array();
  if (!empty($act['activity_campaign_id'])) {
    $params['campaign_id'] = $act['activity_campaign_id'];
  }
  if (!empty($act['activity_type_id'])) {
    $params['activity_type_id'] = $act['activity_type_id'];
  }
  return wf_crm_apivalues('survey', 'get', $params, 'title');
}

/**
 * Get list of activity types
 * @param string $case_type_name
 *
 * @return array
 */
function wf_crm_get_activity_types($case_type_name = NULL) {
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
    if (empty($val['component_id']) || $val['component_id'] == $campaign_type) {
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
    && (in_array($t['sub_type_b'], $sub_type_b) || !$t['sub_type_b'])) {
      $ret[$t['id'] . ($reciprocal ? '_a' : '_r')] = $t['label_a_b'];
    }
    // Reciprocal form - only show if different from above
    if ($reciprocal
    && ($t['type_a'] == $type_b || !$t['type_a'])
    && ($t['type_b'] == $type_a || !$t['type_b'])
    && (in_array($t['sub_type_a'], $sub_type_b) || !$t['sub_type_a'])
    && (in_array($t['sub_type_b'], $sub_type_a) || !$t['sub_type_b'])) {
      $ret[$t['id'] . '_b'] = $t['label_b_a'];
    }
  }
  return $ret;
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
 * @return array - one of:
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
      'activity' => array('entity_type' => 'activity', 'label' => t('Activity')),
      'relationship' => array('entity_type' => 'contact', 'label' => t('Relationship')),
    );
    if (in_array('CiviCase', $components)) {
      $sets['case'] = array('entity_type' => 'case', 'label' => t('Case Settings'));
    }
    if (in_array('CiviEvent', $components)) {
      $sets['participant'] = array('entity_type' => 'participant', 'label' => t('Participant'), 'max_instances' => 9);
    }
    if (in_array('CiviContribute', $components)) {
      $sets['contribution'] = array('entity_type' => 'contribution', 'label' => t('Contribution'));
    }
    if (in_array('CiviMember', $components)) {
      $sets['membership'] = array('entity_type' => 'membership', 'label' => t('Membership'));
    }

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
        'search_prompt' => t('- Choose existing contact -'),
      ),
    );
    foreach (array('organization' => t('Organization Name'), 'legal' => t('Legal Name'), 'household' => t('Household Name')) as $key => $label) {
      $fields['contact_' . $key . '_name'] = array(
        'name' => $label,
        'type' => 'textfield',
        'contact_type' => $key == 'household' ? 'household' : 'organization',
      );
    }
    foreach (array('first_name' => t('First Name'), 'nick_name' => t('Nickname'), 'middle_name' => t('Middle Name'), 'last_name' => t('Last Name'), 'job_title' => t('Job Title')) as $key => $label) {
      $fields['contact_' . $key] = array(
        'name' => $label,
        'type' => 'textfield',
        'contact_type' => $key == 'nick_name' ? NULL : 'individual',
      );
    }
    foreach (array('prefix' => t('Name Prefix'), 'suffix' => t('Name Suffix'), 'gender' => t('Gender')) as $key => $label) {
      $fields['contact_' . $key . '_id'] = array(
        'name' => $label,
        'type' => 'select',
        'contact_type' => 'individual',
      );
    }
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
    $fields['address_county_id'] = array(
      'name' => t('District/County'),
      'type' => 'textfield',
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
    foreach (array('address' => t('Address # Location'), 'phone' => t('Phone # Location'), 'email' => t('Email # Location')) as $key => $label) {
      $fields[$key . '_location_type_id'] = array(
        'name' => $label,
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => '1',
      );
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
      'name' => t('Groups'),
      'type' => 'select',
      'extra' => array('multiple' => 1, 'civicrm_live_options' => 1),
      'table' => 'group',
      'expose_list' => TRUE,
    );
    $tagsets = array('' => t('Tags')) + CRM_Core_BAO_Tag::getTagSet('civicrm_contact');
    foreach ($tagsets as $pid => $name) {
      $fields['other_tag' . ($pid ? "_$pid" : '')] = array(
        'name' => $name,
        'type' => 'select',
        'extra' => array('multiple' => 1, 'civicrm_live_options' => 1),
        'table' => 'tag',
        'expose_list' => TRUE,
      );
    }
    $fields['activity_target_contact_id'] = array(
      'name' => t('Activity Participants'),
      'type' => 'select',
      'expose_list' => TRUE,
      'extra' => array(
        'multiple' => 1,
      ),
      'data_type' => 'ContactReference',
    );
    $fields['activity_subject'] = array(
      'name' => t('Activity Subject'),
      'type' => 'textfield',
    );
    $fields['activity_details'] = array(
      'name' => t('Activity Details'),
      'type' => module_exists('webform_html_textarea') ? 'html_textarea' : 'textarea',
    );
    foreach (array('status' => t('Activity Status'), 'priority' => t('Activity Priority')) as $key => $label) {
      $fields['activity_' . $key . '_id'] = array(
        'name' => $label,
        'type' => 'select',
        'value' => '2',
        'expose_list' => TRUE,
      );
    }
    $fields['activity_assignee_contact_id'] = array(
      'name' => t('Assign Activity to'),
      'type' => 'select',
      'expose_list' => TRUE,
      'empty_option' => t('No One'),
      'data_type' => 'ContactReference',
    );
    $fields['activity_location'] = array(
      'name' => t('Activity Location'),
      'type' => 'textfield',
    );
    $fields['activity_activity_date_time'] = array(
      'name' => t('Activity Date'),
      'type' => 'date',
      'value' => 'now',
    );
    $fields['activity_activity_date_time_timepart'] = array(
      'name' => t('Activity Time'),
      'type' => 'time',
      'value' => 'now',
    );
    $fields['activity_duration'] = array(
      'name' => t('Duration'),
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
      $fields['case_client_id'] = array(
        'name' => t('Case Client'),
        'type' => 'select',
        'expose_list' => TRUE,
        'extra' => array('required' => 1, 'multiple' => $case_info->getAllowMultipleCaseClients()),
        'data_type' => 'ContactReference',
        'value' => 1,
      );
      $fields['case_status_id'] = array(
        'name' => t('Case Status'),
        'type' => 'select',
        'expose_list' => TRUE,
      );
      $fields['case_medium_id'] = array(
        'name' => t('Medium'),
        'type' => 'select',
        'expose_list' => TRUE,
      );
      $fields['case_subject'] = array(
        'name' => t('Case Subject'),
        'type' => 'textfield',
      );
      $fields['case_creator_id'] = array(
        'name' => t('Case Manager'),
        'type' => 'select',
        'expose_list' => TRUE,
        'data_type' => 'ContactReference',
      );
    }
    $fields['relationship_relationship_type_id'] = array(
      'name' => t('Relationship Type'),
      'type' => 'select',
      'expose_list' => TRUE,
      'empty_option' => t('No Relationship'),
      'extra' => array('civicrm_live_options' => 1),
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
        'type' => 'number',
        'extra' => array(
          'field_prefix' => wf_crm_aval($config, 'defaultCurrencySymbol', '$'),
          'point' => wf_crm_aval($config, 'monetaryDecimalPoint', '.'),
          'separator' => wf_crm_aval($config, 'monetaryThousandSeparator', ','),
          'decimals' => 2,
          'min' => 0,
        ),
        'data_type' => 'Money',
        'weight' => 9991,
      );
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
      );
      $fields['participant_status_id'] = array(
        'name' => t('Registration Status'),
        'type' => 'select',
        'expose_list' => TRUE,
        'value' => 0,
        'exposed_empty_option' => '- ' . t('Automatic') . ' -',
      );
      if (isset($fields['contribution_total_amount'])) {
        $fields['participant_fee_amount'] = $fields['contribution_total_amount'];
        $fields['participant_fee_amount']['name'] = t('Participant Fee');
        unset($fields['participant_fee_amount']['weight']);
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
    );
    list($contact_types) = wf_crm_get_contact_types();
    $custom_extends = "'" . implode("','", array_keys($contact_types + $sets)) . "'";
    $sql = "
      SELECT cf.*, cg.title AS custom_group_name, LOWER(cg.extends) AS entity_type, cg.extends_entity_column_id, cg.extends_entity_column_value AS sub_types, cg.is_multiple, cg.max_multiple, cg.id AS custom_group_id
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
        }
        $id = $set . '_custom_' . $dao->id;
        $fields[$id] = $custom_types[$dao->html_type];
        $fields[$id]['name'] = $dao->label;
        $fields[$id]['required'] = $dao->is_required;
        $fields[$id]['extra']['description'] = $dao->help_pre;
        $fields[$id]['value'] = implode(',', wf_crm_explode_multivalue_str($dao->default_value));
        $fields[$id]['data_type'] = $dao->data_type;
        if (!empty($dao->help_pre) || !empty($dao->help_post)) {
          $fields[$id]['extra']['description'] = $dao->help_pre ? $dao->help_pre : $dao->help_post;
          $fields[$id]['has_help'] = TRUE;
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
        if (!empty($dao->option_group_id) && empty($fields[$id]['value'])) {
          $fields[$id]['value_callback'] = 'getCustomFieldDefault';
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
    $domain = wf_civicrm_api('domain', 'get', array('current_domain' => 1, 'return.id' => 1));
    $domain = wf_crm_aval($domain, 'id', 1);
  }
  $result = wf_civicrm_api('uf_match', 'get', array($type . '_id' => $id));
  if (!empty($result['values'])) {
    foreach ($result['values'] as $val) {
      if ($val['domain_id'] == $domain) {
        break;
      }
    }
    if (!empty($user_lookup)) {
      $current_user = $val['contact_id'];
    }
    return $type == 'uf' ? $val['contact_id'] : $val['uf_id'];
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
  if (!empty($result['is_error'])) {
    $bt = debug_backtrace();
    $file = explode('/', $bt[0]['file']);
    watchdog('webform_civicrm', 'The CiviCRM "%function" API function returned the error: "%msg" when called by line !line of !file with the following parameters: "!params"', array('%function' => $entity . ' ' . $operation, '%msg' => $result['error_message'], '!line' => $bt[0]['line'], '!file' => array_pop($file), '!params' => print_r($params, TRUE)), WATCHDOG_ERROR);
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
  $params += array('options' => array());
  // Work around the api's default limit of 25
  $params['options'] += array('limit' => 9999);
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
 * @return array
 */
function wf_crm_location_fields() {
  return array('address', 'email', 'phone', 'website');
}

/**
 * Fetch the path of a file
 * And format it as a public url that CiviCRM can read
 *
 * @param $id
 *   Drupal file id
 *
 * @return string|void: url of file if found
 */
function wf_crm_filepath($id) {
  if ($file = file_load($id)) {
    global $base_url;
    // Escape all characters except : and /
    return str_replace('public://', $base_url . '/' . variable_get('file_public_path', conf_path() . '/files') . '/', str_replace(array('%3A', '%2F'), array(':', '/'), rawurlencode($file->uri)));
  }
}

/**
 * Returns the children of a webform component
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
