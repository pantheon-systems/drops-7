<?php

/**
 * @file
 * Front-end form handler base class.
 */

module_load_include('inc', 'webform_civicrm', 'includes/utils');

abstract class wf_crm_webform_base {
  protected $node;
  protected $settings = array();
  protected $enabled = array();
  protected $data = array();
  protected $id = array();
  protected $events = array();
  protected $contribution_page;
  protected $line_items = array();
  protected $membership_types = array();

  /**
   * Fetch all relevant data for a given contact
   * Used to load contacts for pre-filling a webform, and also to fill in a contact via ajax
   *
   * @param int $c
   *   Contact #
   * @param array $exclude
   *   Fields to ignore
   *
   * @return array of contact data
   */
  protected function loadContact($c, $exclude = array()) {
    $info = array();
    $cid = $this->id['cid'][$c];
    $contact = $this->data['contact'][$c];
    $prefix = 'civicrm_' . $c . '_contact_1_';
    foreach (array('contact', 'address', 'email', 'phone', 'website') as $ent) {
      if ((!empty($contact['number_of_' . $ent]) && !in_array($ent, $exclude)) || $ent == 'contact') {
        $params = array('contact_id' => $cid);
        if ($ent != 'contact' && $ent != 'website') {
          $params['options']['sort'] = 'is_primary DESC';
        }
        $result = wf_civicrm_api($ent, 'get', $params);
        if (!empty($result['values'])) {
          $result = array_merge(array(0), array_values($result['values']));
          unset($result[0]);
          if ($ent == 'contact' && isset($this->enabled[$prefix . 'contact_privacy'])) {
            // Privacy fields
            foreach (array_keys(wf_crm_get_privacy_options()) as $key) {
              if (!empty($result[1][$key])) {
                $result[1]['privacy'][] = $key;
              }
            }
          }
          // Extra processing for addresses
          if ($ent == 'address') {
            foreach ($result as &$address) {
              // Translate to abbr
              if (!empty($address['state_province_id'])) {
                $address['state_province_id'] = wf_crm_state_abbr($address['state_province_id']);
              }
              // Load custom data
              $custom = $this->getCustomData($address['id'], 'address');
              if (!empty($custom['address'])) {
                $address += $custom['address'][1];
              }
            }
          }
          $info[$ent] = $result;
        }
      }
    }
    // Get custom contact data if needed
    foreach ($contact as $k => $v) {
      if (substr($k, 0, 12) == 'number_of_cg' && !empty($v)) {
        if (!in_array(substr($k, 10), $exclude)) {
          $info += $this->getCustomData($cid);
          break;
        }
      }
    }
    // Retrieve group and tag data
    if (!in_array('other', $exclude)) {
      $api = array('tag' => 'entity_tag', 'group' => 'group_contact');
      foreach (array_keys($this->enabled) as $fid) {
        // This way we support multiple tag fields (for tagsets)
        if (strpos($fid, $prefix . 'other') !== FALSE) {
          list(, , , , , $ent) = explode('_', $fid);
          list(, , , , , $field) = explode('_', $fid, 6);
          // Cheap way to avoid fetching the same data twice from the api
          if (!is_array($api[$ent])) {
            $api[$ent] = wf_civicrm_api($api[$ent], 'get', array('contact_id' => $cid));
          }
          foreach (wf_crm_aval($api[$ent], 'values') as $val) {
            $info['other'][1][$field][] = $val[$ent . '_id'];
          }
        }
      }
    }
    // Retrieve relationship data
    if (!in_array('relationship', $exclude) && !empty($contact['number_of_relationship'])) {
      $this->enabled = wf_crm_enabled_fields($this->node);
      for ($r = 1; $r <= $contact['number_of_relationship']; ++$r) {
        $types = array();
        $prefix = "civicrm_{$c}_contact_{$r}_relationship_";
        if (!empty($this->id['cid'][$r])) {
          if (!empty($contact['relationship'][$r]['relationship_type_id']) && $contact['relationship'][$r]['relationship_type_id'] != 'create_civicrm_webform_element') {
            $types = (array) $contact['relationship'][$r]['relationship_type_id'];
          }
          if (!empty($this->enabled[$prefix . 'relationship_type_id'])) {
            $types += array_keys($this->getExposedOptions($prefix . 'relationship_type_id'));
          }
        }
        $rel = $this->getRelationship($types, $cid, $this->id['cid'][$r]);
        if ($rel) {
          $info['relationship'][$r] = $rel;
          // Fetch custom data
          $len = strlen($prefix . 'custom_');
          foreach ($this->enabled as $k => $v) {
            if (substr($k, 0, $len) == $prefix . 'custom_') {
              $custom = wf_civicrm_api('custom_value', 'get', array('entity_id' => $rel['id'], 'entity_table' => 'Relationship'));
              foreach ($custom['values'] as $k => $v) {
                if (isset($v[0])) {
                  $info['relationship'][$r]["custom_$k"] = $v[0];
                }
              }
              break;
            }
          }
        }
      }
    }
    return $info;
  }

  /**
   * Fetch relationship for a pair of contacts
   *
   * @param $r_types
   *   Array of relationship type ids
   * @param $cid1
   *   Contact id
   * @param $cid2
   *   Contact id
   * @return array
   */
  protected function getRelationship($r_types, $cid1, $cid2) {
    $found = array();
    if ($r_types && $cid1 && $cid2) {
      $types = array();
      foreach ($r_types as $r_type) {
        list($type, $side) = explode('_', $r_type);
        $types[$type] = $type;
      }
      $sql = "SELECT * FROM civicrm_relationship
      WHERE relationship_type_id IN (" . implode(',', $types) . ")
      AND ((contact_id_a = $cid1 AND contact_id_b = $cid2) OR (contact_id_a = $cid2 AND contact_id_b = $cid1))
      ORDER BY is_active DESC, IF(end_date, 1, 0), end_date DESC";
      $dao = CRM_Core_DAO::executeQuery($sql);
      while ($dao->fetch()) {
        $type = $dao->relationship_type_id;
        $side = $dao->contact_id_a == $cid1 ? 'a' : 'b';
        // Verify this is the correct orientation for the relationship
        if (in_array("{$type}_$side", $r_types) || in_array("{$type}_r", $r_types)) {
          // Discard metadata from the query
          foreach ((array) $dao as $k => $v) {
            if ($k[0] != '_' && $k != 'N' && $k != 'relationship_type_id') {
              $found[$k] = $v;
            }
          }
          // Support multi-valued relationship type fields, fudge the rest
          $found['relationship_type_id'][] = in_array("{$type}_r", $r_types) ? "{$type}_r" : "{$type}_$side";
          $found['relationship_permission'] = ($found['is_permission_a_b'] ? 1 : 0) + ($found['is_permission_b_a'] ? 2 : 0);
        }
      }
      $dao->free();
    }
    return $found;
  }

  /**
   * For a given field, find the options that are exposed to the webform.
   *
   * @param $field_key
   *   Field key
   * @param array $exclude
   *   Options to ignore
   *
   * @return array
   */
  protected function getExposedOptions($field_key, $exclude = array()) {
    if ($field_key && isset($this->enabled[$field_key])) {
      $field = $this->node->webform['components'][$this->enabled[$field_key]];
      if ($field['type'] == 'select') {
        // Fetch static options
        if (empty($field['extra']['civicrm_live_options'])) {
          $exposed = wf_crm_str2array($field['extra']['items']);
        }
        // Fetch live options
        else {
          $exposed = wf_crm_field_options($field, 'live_options', $this->data);
        }
        foreach ($exclude as $i) {
          unset($exposed[$i]);
        }
        return $exposed;
      }
    }
    return array();
  }

  /**
   * Fetch an existing activity for a contact based on activity & case criteria
   * Maybe someday this will be possible through the CiviCRM API
   *
   * @param $params
   *   Array of activity & case info
   * @return string
   */
  protected function findActivity($params) {
    if (!empty($params['status_id'])) {
      $cid = wf_crm_aval($params, 'contact_id');
      unset($params['contact_id']);
      $sql = 'SELECT civicrm_activity.id FROM civicrm_activity';
      if (!empty($params['case_id'])) {
        $sql .= ' INNER JOIN civicrm_case_activity ON civicrm_activity.id = civicrm_case_activity.activity_id';
      }
      if ($cid) {
        $sql .= ' INNER JOIN civicrm_activity_contact ON civicrm_activity.id = civicrm_activity_contact.activity_id';
      }
      $sql .= " WHERE is_current_revision = 1 AND is_deleted = 0";
      if ($cid) {
        // Find activity target & assignee
        $targetTypes = '2, 3';
        $sql .= " AND civicrm_activity_contact.contact_id = $cid AND civicrm_activity_contact.record_type_id IN ($targetTypes)";
      }
      foreach ($params as $field => $value) {
        if ($value && is_array($value)) {
          $sql .= " AND $field IN (" . implode(',', $value) . ")";
        }
        elseif ($value) {
          $sql .= " AND $field = $value";
        }
      }
      $sql .= ' LIMIT 0, 1';
      return CRM_Core_DAO::singleValueQuery($sql);
    }
  }

  /**
   * Fetch an existing case for client(s)
   * Based on url param or status_id and case_type_id
   *
   * @param $ids
   *   Passed if client id refers to a contact number and not a contact id
   * @return mixed
   */
  protected function findCase($ids = NULL) {
    $params = $this->data['case'][1];
    // Look for case id passed in url
    if (!empty($_GET['caseid']) && is_numeric($_GET['caseid'])) {
      $result = wf_civicrm_api('case', 'get', array('case_id' => $_GET['caseid']));
      if (isset($result['values'][$_GET['caseid']])) {
        $case = $result['values'][$_GET['caseid']];
        // Verify that case is correct type and user has access to this case
        if ($case['case_type_id'] == $params['case'][1]['case_type_id']) {
          // Respect admin privledges
          if (user_access('access all cases and activities')) {
            return $case;
          }
          $case_cids = array();
          foreach ($case['contacts'] as $contact) {
            $case_cids[] = $contact['contact_id'];
          }
          foreach ((array) wf_crm_aval($ids, 'cid', $params['case'][1]['client_id']) as $cid) {
            if (in_array($cid, $case_cids)) {
              // User is involved in the case therefore the found case is correct
              return $case;
            }
          }
        }
      }
    }
    // Match existing case based on client and status
    if (!empty($params['existing_case_status']) && !empty($params['case'][1]['client_id'])) {
      foreach ((array) $params['case'][1]['client_id'] as $client_id) {
        if ($ids !== NULL) {
          $client_id = wf_crm_aval($ids, "cid:$client_id");
        }
        if ($client_id) {
          $result = wf_civicrm_api('case', 'get', array('client_id' => $client_id));
          foreach (wf_crm_aval($result, 'values', array()) as $case) {
            if (empty($case['is_deleted']) && in_array($case['status_id'], $params['existing_case_status'])) {
              $case_types = $case['case_type_id'];
              if (is_string($case_types)) {
                $case_types = explode(',', $case_types);
              }
              if (in_array($params['case'][1]['case_type_id'], $case_types)) {
                return $case;
              }
            }
          }
        }
      }
    }
  }

  /**
   * Fetch info and remaining spaces for events
   *
   * @param $events
   *   Array of event info to fill (reference)
   */
  protected function loadEvents() {
    if (!empty($this->events)) {
      $now = time();
      // Fetch event info
      $dao = CRM_Core_DAO::executeQuery('SELECT id, title, start_date, end_date, event_type_id, max_participants, financial_type_id
      FROM civicrm_event WHERE id IN (' . implode(',', array_keys($this->events)) . ')');
      while ($dao->fetch()) {
        $this->events[$dao->id]['title'] = $dao->title;
        $this->events[$dao->id]['start_date'] = $dao->start_date;
        $this->events[$dao->id]['end_date'] = $dao->end_date;
        $this->events[$dao->id]['event_type_id'] = $dao->event_type_id;
        $this->events[$dao->id]['financial_type_id'] = $dao->financial_type_id;
        $this->events[$dao->id]['full'] = FALSE;
        $this->events[$dao->id]['ended'] = $dao->end_date && strtotime($dao->end_date) < $now;
        if ($this->events[$dao->id]['max_participants'] = $dao->max_participants) {
          $remaining = CRM_Event_BAO_Participant::eventFull($dao->id, TRUE, FALSE);
          if (is_string($remaining)) {
            $this->events[$dao->id]['full'] = TRUE;
            $this->events[$dao->id]['remaining'] = 0;
            $this->events[$dao->id]['full_message'] = $remaining;
          }
          else {
            $this->events[$dao->id]['remaining'] = $remaining ? $remaining : $dao->max_participants;
          }
        }
      }
      $dao->free();
    }
  }

  /**
   * Get custom data for an entity
   *
   * @param $entity_id
   *   Numeric id of entity
   * @param $entity_type
   *   Type of crm entity. 'contact' is assumed
   * @param $normalize
   *   Default true: if true shift all arrays to start at index 1
   *
   * @return array
   */
  protected function getCustomData($entity_id, $entity_type = NULL, $normalize = TRUE) {
    static $parents = array();
    if (empty($parents)) {
      // Create matching table to sort fields by group
      foreach (wf_crm_get_fields() as $key => $value) {
        list($group, $field) = explode('_', $key, 2);
        if (substr($field, 0, 7) == 'custom_') {
          $parents[$field] = $group;
        }
      }
    }
    $params = array('entityID' => $entity_id);
    if ($entity_type) {
      $params['entityType'] = ucfirst($entity_type);
    }
    $result = CRM_Core_BAO_CustomValueTable::getValues($params);
    if (!empty($result['is_error'])) {
      return array();
    }
    unset($result['is_error'], $result['entityID']);
    $values = array();
    // Convert multi-value strings to arrays and sort by group
    foreach ($result as $key => $value) {
      $pieces = explode('_', $key);
      if ($pieces[0] == 'custom') {
        $name = 'custom_' . $pieces[1];
        if (empty($pieces[2])) {
          $pieces[2] = ($normalize ? 1 : 0);
        }
        if (isset($parents[$name])) {
          $values[$parents[$name]][$pieces[2]][$name] = $value;
        }
      }
    }
    if ($normalize) {
      // Normalize array keys
      foreach ($values as &$value) {
        array_unshift($value, 0);
        unset($value[0]);
      }
    }
    return $values;
  }

  /**
   * Save custom data for an entity
   *
   * @param $entity
   *   Array of values
   * @param $entity_id
   *   Numeric id of entity
   * @param $entity_type
   *   Type of crm entity, e.g. "Contact"
   * @param $known
   *   Is this a known record (as opposed to a contact matched via dedupe rules)?
   *   We only allow saving blank fields for known contacts.
   */
  protected function saveCustomData($entity, $entity_id, $entity_type, $known = TRUE) {
    $existing = $this->getCustomData($entity_id, $entity_type, FALSE);
    $params = array('entityID' => $entity_id);
    foreach ($entity as $table => $values) {
      if (substr($table, 0, 2) == 'cg' && is_array($values)) {
        if (empty($existing[$table])) {
          $existing[$table] = array();
        }
        $insert = 0;
        foreach ($values as $custom) {
          if ($id = each($existing[$table])) {
            $suf = $id['key'];
          }
          else {
            $suf = --$insert;
          }
          foreach ($custom as $k => $v) {
            // Only save if this is not blank or data already exists and record is known
            if ($v !== '' || ($suf > 0 && $known)) {
              $params[$k . '_' . $suf] = $v;
            }
          }
        }
      }
    }
    if (count($params) > 1) {
      $result = CRM_Core_BAO_CustomValueTable::setValues($params);
      // Prevent wholesale failure by saving each param individually if there was an error while trying to save them all at once
      if (!empty($result['is_error'])) {
        $bt = debug_backtrace();
        array_shift($params);
        foreach ($params as $k => $v) {
          $single_param = array('entityID' => $entity_id, $k => $v);
          $result = CRM_Core_BAO_CustomValueTable::setValues($single_param);
          if (!empty($result['is_error'])) {
            $file = explode('/', $bt[0]['file']);
            watchdog('webform_civicrm', 'The CiviCRM "CustomValueTable::setValues" function returned the error: "%msg" when called by line !line of !file with the following parameters: "!params"', array('%msg' => $result['error_message'], '!line' => $bt[0]['line'], '!file' => array_pop($file), '!params' => print_r($single_param, TRUE)), WATCHDOG_ERROR);
          }
        }
      }
    }
  }

  /**
   * @param string $fid
   * @param mixed $default
   * @param bool $strict
   * @return mixed
   */
  protected function getData($fid, $default = NULL, $strict = FALSE) {
    if ($pieces = wf_crm_explode_key($fid)) {
      list( , $c, $ent, $n, $table, $name) = $pieces;
      return wf_crm_aval($this->data, "{$ent}:{$c}:{$table}:{$n}:{$name}", $default, $strict);
    }
  }

  /**
   * Loads the contribution page used for this webform
   */
  protected function getContributionPage() {
    $params = array('id' => wf_crm_aval($this->data, 'contribution:1:contribution:1:contribution_page_id'));
    if ($params['id']) {
      $result = wf_crm_apivalues('contribution_page', 'get', $params);
      $this->contribution_page = wf_crm_aval($result, $params['id']);
    }
    return $this->contribution_page;
  }

  protected function getMembershipTypeField($type, $field) {
    if (!$this->membership_types) {
      $this->membership_types = wf_crm_apivalues('membership_type', 'get');
    }
    return wf_crm_aval($this->membership_types, $type . ':' . $field);
  }

  /**
   * CiviCRM JS can't be attached to a drupal form so have to manually re-add this during validation
   */
  function addPaymentJs() {
    $credit_card_types = CRM_Core_Payment_Form::getCreditCardCSSNames();
    CRM_Core_Resources::singleton()
      ->addCoreResources()
      ->addSetting(array('config' => array('creditCardTypes' => $credit_card_types)))
      ->addScriptFile('civicrm', 'templates/CRM/Core/BillingBlock.js', -10, 'html-header');
  }

}

