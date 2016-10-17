<?php

/**
 * @file
 * Front-end form handler base class.
 */

module_load_include('inc', 'webform_civicrm', 'includes/utils');

/**
 * Class wf_crm_webform_base
 *
 * @property array $payment_processor
 * @property array $contribution_page
 * @property number $tax_rate
 * @property number $civicrm_version
 */
abstract class wf_crm_webform_base {
  protected $node;
  protected $settings = array();
  protected $enabled = array();
  protected $data = array();
  protected $ent = array();
  protected $events = array();
  protected $line_items = array();
  protected $membership_types = array();
  protected $loadedContacts = array();

  // No direct access - storage for variables fetched via __get
  private $_payment_processor;
  private $_contribution_page;
  // tax integration
  private $_tax_rate;

  /**
   * Magic method to retrieve otherwise inaccessible properties
   * @param $name
   * @throws Exception
   * @return mixed
   */
  function __get($name) {
    switch ($name) {
      case 'payment_processor':
        $payment_processor_id = wf_crm_aval($this->data, 'contribution:1:contribution:1:payment_processor_id');
        if ($payment_processor_id && !$this->_payment_processor) {
          $this->_payment_processor = wf_civicrm_api('payment_processor', 'getsingle', array('id' => $payment_processor_id));
        }
        return $this->_payment_processor;

      case 'contribution_page':
        $contribution_page_id = wf_crm_aval($this->data, 'contribution:1:contribution:1:contribution_page_id');
        if ($contribution_page_id && !$this->_contribution_page) {
          $this->_contribution_page = wf_civicrm_api('contribution_page', 'getsingle', array('id' => $contribution_page_id));
        }
        return $this->_contribution_page;

      case 'tax_rate':
        $taxSettings = CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::CONTRIBUTE_PREFERENCES_NAME, 'contribution_invoice_settings');
        if (is_array($taxSettings) && !empty($taxSettings['invoicing'])) {
          if ($this->contribution_page) {
            // tax integration
            $taxRates = CRM_Core_PseudoConstant::getTaxRates();
            $this->_tax_rate = $taxRates[$this->_contribution_page['financial_type_id']];
          }
          return $this->_tax_rate;
        }
        return NULL;

      case 'civicrm_version':
        return CRM_Utils_System::version();

      default:
        throw new Exception('Unknown property');
    }
  }

  /**
   * Fetch all relevant data for a given contact
   * Used to load contacts for pre-filling a webform, and also to fill in a contact via ajax
   *
   * @param int $c
   *   Contact #
   * @param array $exclude
   *   Fields to ignore
   *
   * @return array
   *   Contact data
   */
  protected function loadContact($c, $exclude = array()) {
    if (!empty($this->loadedContacts[$c])) {
      return $this->loadedContacts[$c];
    }
    $info = array();
    $cid = $this->ent['contact'][$c]['id'];
    if (!$cid) {
      return $info;
    }
    $contact = $this->data['contact'][$c];
    $prefix = 'civicrm_' . $c . '_contact_1_';
    $existing_contact_field = $this->getComponent($prefix . 'contact_existing');
    $exclude = array_merge($exclude, wf_crm_aval($existing_contact_field['extra'], 'no_autofill', array()));
    foreach (array_merge(array('contact'), wf_crm_location_fields()) as $ent) {
      if ((!empty($contact['number_of_' . $ent]) && !in_array($ent, $exclude)) || $ent == 'contact') {
        $params = array('contact_id' => $cid);
        if ($ent != 'contact' && $ent != 'website') {
          $params['options']['sort'] = 'is_primary DESC';
        }
        $result = wf_civicrm_api($ent, 'get', $params);
        // Handle location field sorting
        if(in_array($ent, wf_crm_location_fields())){
          $result['values'] = $this->reorderByLocationType($c, $ent, $result['values']);
        }
        if (!empty($result['values'])) {
          // Index array from 1 instead of 0
          $result = array_merge(array(0), array_values($result['values']));
          unset($result[0]);
          if ($ent == 'contact') {
            // Exclude name fields
            if (in_array('name', $exclude)) {
              unset($result[1]['first_name'], $result[1]['middle_name'], $result[1]['last_name'],
                $result[1]['formal_title'], $result[1]['prefix_id'], $result[1]['suffix_id'],
                $result[1]['nick_name'], $result[1]['organization_name'], $result[1]['household_name']
              );
            }
            // Privacy fields
            if (isset($this->enabled[$prefix . 'contact_privacy'])) {
              foreach (array_keys(wf_crm_get_privacy_options()) as $key) {
                if (!empty($result[1][$key])) {
                  $result[1]['privacy'][] = $key;
                }
              }
            }
            // Hack for gender as textfield. More general solution needed for all pseudoconsant fields
            $gender_field = $this->getComponent("civicrm_{$c}_contact_1_contact_gender_id");
            if ($gender_field && $gender_field['type'] == 'textfield') {
              $result[1]['gender_id'] = wf_crm_aval($result[1], 'gender');
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
              if(isset($address['id'])){
                $custom = $this->getCustomData($address['id'], 'address');
                if (!empty($custom['address'])) {
                  $address += $custom['address'][1];
                }
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
        if (!empty($this->ent['contact'][$r]['id'])) {
          if (!empty($contact['relationship'][$r]['relationship_type_id']) && $contact['relationship'][$r]['relationship_type_id'] != 'create_civicrm_webform_element') {
            $types = (array) $contact['relationship'][$r]['relationship_type_id'];
          }
          if (!empty($this->enabled[$prefix . 'relationship_type_id'])) {
            $types += array_keys($this->getExposedOptions($prefix . 'relationship_type_id'));
          }
        }
        $rel = $this->getRelationship($types, $cid, wf_crm_aval($this->ent['contact'], "$r:id"));
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
    $this->loadedContacts[$c] = $info;
    return $info;
  }

  /**
   * Reorder returned results according to settings chosen in wf_civicrm backend
   *
   * @param integer $c
   * @param string $ent
   * @param array $values
   * @return array $reorderedArray
   */
  protected function reorderByLocationType($c, $ent, $values = array()){
    $reorderedArray = array();

    if(isset($this->settings['data']['contact'][$c][$ent])){
      // First pass
      $reorderedArray = $this->matchLocationTypes($c, $ent, $values);

      // Second pass
      $reorderedArray = $this->handleRemainingValues($reorderedArray, $values);

      return $reorderedArray;
    } else {
      return $values;
    }
  }

  /**
   * Organize values according to location types
   *
   * @param integer $c
   * @param string $ent
   * @param array $values
   * @return array $reorderedArray
   */
  protected function matchLocationTypes($c, $ent, &$values){
    // create temporary settings array to include 'user-select' fields
    // on the right place in array
    $settingsArray = $this->add_user_select_field_placeholder($ent, $this->settings['data']['contact'][$c]);
    $userSelectIndex = 0;
    // Go through the array and match up locations by type
    // Put placeholder 'user-select' where location_type_id is empty for second pass
    foreach ($settingsArray[$ent] as $setting) {
      $valueFound = false;
      foreach($values as $key => $value){
        if((in_array($ent, array('address', 'email')) && $value['location_type_id'] == $setting['location_type_id'])
           ||
          ($value['location_type_id'] == $setting['location_type_id'] && $value[$ent.'_type_id'] == $setting[$ent.'_type_id'])     
          ){
            $reorderedArray[$key] = $value;
            $valueFound = true;
            unset($values[$key]);
            break;
        } else if(empty($setting['location_type_id'])) { // for 'user-select' fields
            $valueFound = true;
            $reorderedArray['us'.$userSelectIndex] = 'user-select';
            $userSelectIndex ++;
            break;
        }
      }

      // always keep number of returned values equal to chosen settings
      // if value is not found then set an empty array
      if(!$valueFound){
        $reorderedArray[] = array();
      }
    }
    return $reorderedArray;
  }

  /**
   * Put remaining values in 'user-select' fields
   *
   * @param array $reorderedArray
   * @param array $values
   * @return array $reorderedArray
   */
  protected function handleRemainingValues($reorderedArray, &$values){
    // Put leftover values in fields marked as 'user-select'
    foreach($reorderedArray as $key => $value){
      if($reorderedArray[$key] == 'user-select'){
        $reorderedArray[$key] = !empty($values) ? array_shift($values) : '';
      }
    }
    return $reorderedArray;
  }

  /**
   * Add location_type_id = NULL for user-select fields for identification later
   *
   * @param string $ent
   * @param array $settings
   * @return array $settings
   */
  protected function add_user_select_field_placeholder($ent, $settings = array()){
    if($settings['number_of_'.$ent] > count($settings[$ent])){
      for($i = 1; $i <= $settings['number_of_'.$ent]; $i++){
        if(!array_key_exists($i, $settings[$ent])){
          $settings[$ent][$i]['location_type_id'] = NULL;
        }
      }
      ksort($settings[$ent]);
    }
    return $settings;
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
    $field = $this->getComponent($field_key);

    if ($field && $field['type'] == 'hidden') {
      // Fetch live options
      $exposed = wf_crm_field_options($field, 'live_options', $this->data);
      foreach ($exclude as $i) {
        unset($exposed[$i]);
      }
      return $exposed;
    }

    if ($field && $field['type'] == 'select') {
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
    return array();
  }

  /**
   * Fetch a webform component given its civicrm field key
   * @param $field_key
   * @return array|null
   */
  protected function getComponent($field_key) {
    if ($field_key && isset($this->enabled[$field_key])) {
      return wf_crm_aval($this->node->webform, 'components:' . $this->enabled[$field_key]);
    }
    return NULL;
  }

  /**
   * Get memberships for a contact
   * @param $cid
   * @return array
   */
  protected function findMemberships($cid) {
    static $status_types;
    static $membership_types;
    if (!isset($membership_types)) {
      $membership_types = array_keys(wf_crm_apivalues('membershipType', 'get', array('is_active' => 1, 'return' => 'id')));
    }
    $existing = wf_crm_apivalues('membership', 'get', array(
      'contact_id' => $cid,
      // Limit to only enabled membership types
      'membership_type_id' => array('IN' => $membership_types),
    ));
    if (!$existing) {
      return array();
    }
    if (!$status_types) {
      $status_types = wf_crm_apivalues('membership_status', 'get');
    }
    // Attempt to order memberships by most recent and active
    $active = $expired = array();
    foreach ($existing as $membership) {
      $membership['is_active'] = $status_types[$membership['status_id']]['is_current_member'];
      $membership['status'] = $status_types[$membership['status_id']]['label'];
      $list = $membership['is_active'] ? 'active' : 'expired';
      array_unshift($$list, $membership);
    }
    return array_merge($active, $expired);
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
    $existing = FALSE;
    $params = array('entityID' => $entity_id);
    foreach ($entity as $table => $values) {
      if (substr($table, 0, 2) == 'cg' && is_array($values)) {
        if ($existing === FALSE) {
          $existing = $this->getCustomData($entity_id, $entity_type, FALSE);
        }
        $existing += array($table => array());
        $insert = 0;
        foreach ($values as $custom) {
          // Match to id of existing record (id will be 0 for non-multi-value custom sets, which is fine)
          if ($id = each($existing[$table])) {
            $suf = $id['key'];
          }
          // Create new record(s) using negative numbers
          else {
            $suf = --$insert;
          }
          foreach ($custom as $k => $v) {
            // Only save if this is not blank or data already exists and record is known
            if ($v !== '' || ($suf >= 0 && $known)) {
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
   * Find a case matching criteria
   *
   * Normally we could do this by passing filters into the api, but legacy case api doesn't support them
   * So instead we fetch every case for the contact and loop through them to test against filters.
   *
   * @param array|int $cid
   * @param array $filters
   * @return null|array
   */
  function findCaseForContact($cid, $filters) {
    $case = NULL;
    foreach (wf_crm_apivalues('case', 'get', array('client_id' => $cid)) as $item) {
      if (empty($item['is_deleted'])) {
        $match = TRUE;
        foreach (array_filter($filters) as $filter => $value) {
          if (!array_intersect((array)$item[$filter], (array)$value)) {
            $match = FALSE;
          }
        }
        // Note: this loop has no break on purpose - this way we find the most recent case instead of stopping at the first
        if ($match) {
          $case = $item;
        }
      }
    }
    return $case;
  }

  /**
   * @param $type
   * @param $field
   * @return array|null
   */
  protected function getMembershipTypeField($type, $field) {
    if (!$this->membership_types) {
      $this->membership_types = wf_crm_apivalues('membership_type', 'get');
    }
    return wf_crm_aval($this->membership_types, $type . ':' . $field);
  }

  /**
   * CiviCRM JS can't be attached to a drupal form so have to manually re-add this during validation
   *
   * @return int|null
   */
  function addPaymentJs() {
    $credit_card_types = CRM_Core_Payment_Form::getCreditCardCSSNames();
    CRM_Core_Resources::singleton()
      ->addCoreResources()
      ->addSetting(array('config' => array('creditCardTypes' => $credit_card_types)))
      ->addScriptFile('civicrm', 'templates/CRM/Core/BillingBlock.js', -10, 'html-header');
  }

  /**
   * Copies a drupal file into the Civi file system
   *
   * @param int $id: drupal file id
   * @return int|null Civi file id
   */
  static function saveDrupalFileToCivi($id) {
    $file = file_load($id);
    if ($file) {
      $config = CRM_Core_Config::singleton();
      $path = file_unmanaged_copy($file->uri, $config->customFileUploadDir);
      if ($path) {
        $result = wf_civicrm_api('file', 'create', array(
          'uri' => str_replace($config->customFileUploadDir, '', $path),
          'mime_type' => $file->filemime,
        ));
        return wf_crm_aval($result, 'id');
      }
    }
    return NULL;
  }

  /**
   * Retrieve info needed for pre-filling a webform file field
   * @param string $fieldName
   * @param string|int $val: url or civi file id
   * @param string|null $entity: entity name
   * @param int|null $n: entity id
   * @return array|null
   */
  function getFileInfo($fieldName, $val, $entity, $n) {
    if (!$val) {
      return NULL;
    }
    if ($fieldName === 'image_URL') {
      return array(
        'data_type' => 'File',
        'name' => NULL,
        'icon' => $val,
      );
    }
    $file = wf_crm_apivalues('file', 'get', $val);
    $entity_id = '';
    if ($entity && $n && (strpos($fieldName, 'custom_') === 0 || strpos($fieldName, 'file_') === 0)) {
      $entity_id = $this->ent[$entity][$n]['id'];
    }
    if (!empty($file[$val])) {
      return array(
        'data_type' => 'File',
        'name' => CRM_Utils_File::cleanFileName($file[$val]['uri']),
        'file_url'=> CRM_Utils_System::url('civicrm/file', "reset=1&id={$val}&eid={$entity_id}", TRUE),
        'icon' => file_icon_url((object) array('filemime' => $file[$val]['mime_type'])),
      );
    }
    return NULL;
  }

  /**
   * Fetch the public url of a file in the Drupal file system
   *
   * @param int $id Drupal file id
   *
   * @return string|bool: url of file if found
   */
  function getDrupalFileUrl($id) {
    $file = file_load($id);
    return $file ? file_create_url($file->uri) : FALSE;
  }

  /**
   * FIXME: Use the api for this
   * @param string $ent - entity type
   * @param int $id - entity id
   * @return array starting at index 1
   */
  public function getAttachments($ent, $id) {
    $n = 1;
    $attachments = array();
    $dao = CRM_Core_DAO::executeQuery("SELECT * FROM civicrm_entity_file WHERE entity_table = 'civicrm_$ent' AND entity_id = $id");
    while ($dao->fetch()) {
      $attachments[$n++] = array('id' => $dao->id, 'file_id' => $dao->file_id);
    }
    return $attachments;
  }

  /**
   * Generate the quickform key needed to access a contribution form
   * @return string
   */
  public function getQfKey() {
    return CRM_Core_Key::get('CRM_Contribute_Controller_Contribution', TRUE);
  }

}
