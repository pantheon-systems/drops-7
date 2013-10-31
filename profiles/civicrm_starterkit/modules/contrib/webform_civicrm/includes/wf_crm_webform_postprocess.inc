<?php

/**
 * @file
 * Front-end form validation and post-processing.
 */

module_load_include('inc', 'webform_civicrm', 'includes/wf_crm_webform_base');

class wf_crm_webform_postprocess extends wf_crm_webform_base {
  // Variables used during validation
  private $form;
  private $form_state;
  private $crmValues;
  private $rawValues;
  private $multiPageDataLoaded;
  private $billing_params = array();
  private $totalContribution = 0;
  private $contributionIsPayLater = FALSE;

  // During validation this contains an array of known contact ids and the placeholder 0 for valid contacts
  // During submission processing this contains an array of known contact ids
  private $existing_contacts = array();

  // Variables used during submission processing
  private $submission;
  private $update = array();
  private $all_fields;
  private $all_sets;
  private $shared_address = array();

  // Cache an instance of this object
  // @see self::singleton()
  static $singleton;

  function __construct($node) {
    civicrm_initialize();
    $this->node = $node;
    $this->settings = $node->webform_civicrm;
    $this->data = $this->settings['data'];
    $this->enabled = wf_crm_enabled_fields($node);
    $this->all_fields = wf_crm_get_fields();
    $this->all_sets = wf_crm_get_fields('sets');
  }

  /**
   * This is first called during form validation. We create an instance of this object and stash it in a static variable.
   * It is destroyed and rebuilt between each page submission, but after (successful) validation of the final page,
   * this cache allows the object instance to persist throughout (final) validate, preSave and postSave operations.
   * @param stdClass $node
   * @return wf_crm_webform_postprocess
   */
  static function singleton($node) {
    if (!self::$singleton) {
      self::$singleton = new wf_crm_webform_postprocess($node);
    }
    return self::$singleton;
  }

  /**
   * Called after a webform is submitted
   * Or, for a multipage form, called after each page
   * @param array $form
   * @param array $form_state (reference)
   */
  public function validate($form, &$form_state) {
    $this->form = $form;
    $this->form_state = &$form_state;
    $this->rawValues = _webform_client_form_submit_flatten($this->node, wf_crm_aval($this->form_state, 'values:submitted'));
    $this->crmValues = wf_crm_enabled_fields($this->node, $this->rawValues);
    // Even though this object is destroyed between page submissions, this trick allows us to persist some data - see below
    $this->id = wf_crm_aval($form_state, 'civicrm:id', array());

    $this->hiddenFieldValidation();
    $this->validateThisPage($this->form['submitted']);

    if (!empty($this->data['participant']) && !empty($this->data['participant_reg_type'])) {
      $this->loadMultiPageData();
      $this->validateParticipants();
    }

    // Process live contribution. If the transaction is unsuccessful it will trigger a form validation error.
    if ($this->getContributionPage()) {
      $this->loadMultiPageData();
      if ($this->tallyLineItems()) {
        if ($this->isPaymentPage() && !form_get_errors()) {
          if ($this->validateBillingFields()) {
            if ($this->createBillingContact()) {
              $this->submitPayment();
            }
          }
        }
      }
    }
    // Even though this object is destroyed between page submissions, this trick allows us to persist some data - see above
    $form_state['civicrm']['id'] = $this->id;
    $form_state['civicrm']['line_items'] = $this->line_items;
  }

  /**
   * Process webform submission when it is about to be saved. Called by the following hook:
   * @see webform_civicrm_webform_submission_presave
   */
  public function preSave(&$submission) {
    $this->submission = &$submission;
    $this->data = $this->settings['data'];
    // Check for existing submission
    $this->setUpdateParam();
    // Fill $this->id from existing contacts
    $this->getExistingContactIds();

    // While saving a draft, just skip to the end and write the record
    if (!empty($this->submission->is_draft)) {
      return;
    }

    $this->fillDataFromSubmission();

    // Create/update contacts
    foreach ($this->data['contact'] as $c => $contact) {
      if (empty($this->id['cid'][$c])) {
        // Don't create contact if we don't have a name or email
        if ($this->isContactEmpty($contact)) {
          $this->id['cid'][$c] = 0;
          continue;
        }
        $this->id['cid'][$c] = $this->findDuplicateContact($contact);
      }

      // Current employer must wait for ContactRef ids to be filled
      unset($contact['contact'][1]['employer_id']);

      // Create new contact
      if (empty($this->id['cid'][$c])) {
        $this->id['cid'][$c] = $this->createContact($contact);
      }
      // Update existing contact
      else {
        $this->updateContact($contact, $c);
      }
    }
    // $this->id['cid'] will now contain all contact ids in order, with 0 as a placeholder for any contact not saved
    ksort($this->id['cid']);

    // Once all contacts are saved we can fill contact ref fields
    $this->fillContactRefs();

    // Save a non-live transaction
    if (empty($this->id['contribution']) && $this->totalContribution) {
      $this->payLaterContribution();
    }

    // Create/update other data associated with contacts
    foreach ($this->data['contact'] as $c => $contact) {
      if (!($cid = $this->id['cid'][$c])) {
        continue;
      }
      $this->saveCurrentEmployer($contact, $cid);

      $this->saveCustomData($contact, $cid, 'Contact', !empty($this->existing_contacts[$c]));

      $this->fillHiddenContactFields($cid, $c);

      $this->saveContactLocation($contact, $cid, $c);

      $this->saveGroupsAndTags($contact, $cid, $c);

      // Process relationships
      foreach (wf_crm_aval($contact, 'relationship', array()) as $n => $params) {
        $this->processRelationship($params, $cid, $this->id['cid'][$n]);
      }
      // Process event participation
      if (isset($this->all_sets['participant']) && !empty($this->data['participant_reg_type'])) {
        $this->processParticipants($c, $cid);
      }
      // Process memberships
      if (isset($this->all_sets['membership']) && !empty($this->data['membership'][$c]['number_of_membership'])) {
        $this->processMemberships($c, $cid);
      }
    }
    // We do this last after all contacts and addresses exist.
    $this->processSharedAddresses();
  }

  /**
   * Process webform submission when it is has been saved. Called by the following hooks:
   * @see webform_civicrm_webform_submission_insert
   * @see webform_civicrm_webform_submission_update
   */
  public function postSave($submission) {
    $this->submission = $submission;
    if (empty($this->submission->is_draft)) {
      // Process case
      if (!empty($this->data['case'][1]['case'][1]['client_id'])) {
        $this->processCase();
      }
      // Process activity
      if (!empty($this->data['activity'][1]['activity'])) {
       $this->processActivity();
      }
      // Save contribution custom data & line-items
      if (!empty($this->id['contribution'])) {
        $this->processContribution();
      }
    }
    // Write record; we do this when creating, updating, or saving a draft of a webform submission.
    $record = array(
      'sid' => $this->submission->sid,
      'contact_id' => '-' . implode('-', $this->id['cid']) . '-',
      'activity_id' => empty($this->id['act'][1]) ? 0 : $this->id['act'][1],
    );
    drupal_write_record('webform_civicrm_submissions', $record, $this->update);
  }

  /**
   * Force Drupal to ignore errors for fields hidden by this module
   */
  private function hiddenFieldValidation() {
    $errors = form_get_errors();
    foreach ((array) $errors as $key => $error) {
      $pieces = wf_crm_explode_key(substr($key, strrpos($key, '][') + 2));
      if($pieces) {
        list( , $c, $ent, $n, $table, $name) = $pieces;
        if ($this->isFieldHiddenByExistingContactSettings($ent, $c, $table, $n, $name)) {
          $this->unsetError($key);
        }
        elseif ($table == 'address' && !empty($this->crmValues["civicrm_{$c}_contact_{$n}_address_master_id"])) {
          $master_id = $this->crmValues["civicrm_{$c}_contact_{$n}_address_master_id"];
          // If widget is checkboxes, need to filter the array
          if (!is_array($master_id) || array_filter($master_id)) {
            $this->unsetError($key);
          }
        }
      }
    }
  }

  /**
   * Recursive validation callback for webform page submission
   *
   * @param array $elements
   *   FAPI form array
   */
  private function validateThisPage($elements) {
    // Recurse through form elements.
    foreach (element_children($elements) as $key) {
      if (is_array($elements[$key]) && ($element = $elements[$key])) {
        $this->validateThisPage($elements[$key]);
        if (!empty($element['#civicrm_data_type'])
          && substr(wf_crm_aval($element, '#type', ''), 0, 4) === 'text'
          && $element['#value'] !== NULL
          && $element['#value'] !== '') {
          $dt = $element['#civicrm_data_type'];
          // Validate state/prov abbreviation
          if ($dt == 'state_province_abbr') {
            $ckey = str_replace('state_province', 'country', $key);
            if (!empty($this->crmValues[$ckey]) && is_numeric($this->crmValues[$ckey])) {
              $country_id = $this->crmValues[$ckey];
            }
            else {
              $config = CRM_Core_Config::singleton();
              $country_id = $config->defaultContactCountry;
            }
            $states = wf_crm_get_states($country_id);
            if ($states && !array_key_exists(strtoupper($element['#value']), $states)) {
              $countries = wf_crm_apivalues('address', 'getoptions', array('field' => 'country_id'));
              form_error($element, t('Mismatch: "@state" is not a state/province of %country. Please enter a valid state/province abbreviation for %field.', array('@state' => $element['#value'], '%country' => $countries[$country_id], '%field' => $element['#title'])));
            }
          }
          // Strings and files don't need any validation
          elseif ($dt !== 'String' && $dt !== 'Memo' && $dt !== 'File'
            && CRM_Utils_Type::escape($element['#value'], $dt, FALSE) === NULL) {
            // Allow data type names to be translated
            switch ($dt) {
              case 'Int':
                $dt = t('an integer');
                break;
              case 'Float':
                $dt = t('a number');
                break;
              case 'Link':
                $dt = t('a web address starting with http://');
                break;
              case 'Money':
                $dt = t('a currency value');
                break;
            }
            form_error($element, t('Please enter @type for %field.', array('@type' => $dt, '%field' => $element['#title'])));
          }
        }
      }
    }
  }

  /**
   * Validate event participants and add line items
   */
  private function validateParticipants() {
    // If we have no valid contacts on the form, don't bother continuing
    if (!$this->existing_contacts) {
      return;
    }
    $count = $this->data['participant_reg_type'] == 'all' ? count($this->existing_contacts) : 1;
    // Collect selected events
    foreach ($this->data['participant'] as $c => $par) {
      if ($this->data['participant_reg_type'] == 'all') {
        $contacts = $this->existing_contacts;
      }
      elseif (isset($this->existing_contacts[$c])) {
        $contacts = array($this->existing_contacts[$c]);
      }
      else {
        continue;
      }
      foreach (wf_crm_aval($par, 'participant', array()) as $n => $p) {
        foreach (array_filter(wf_crm_aval($p, 'event_id', array())) as $id_and_type) {
          list($eid) = explode('-', $id_and_type);
          if (is_numeric($eid)) {
            $this->events[$eid]['ended'] = TRUE;
            $this->events[$eid]['title'] = t('this event');
            $this->events[$eid]['count'] = wf_crm_aval($this->events, "$eid:count", 0) + $count;
            if (!empty($p['fee_amount'])) {
              $this->line_items[] = array(
                'qty' => $count,
                'entity_table' => 'civicrm_participant',
                'event_id' => $eid,
                'contact_ids' => $contacts,
                'unit_price' => $p['fee_amount'],
                'element' => "civicrm_{$c}_participant_{$n}_participant_{$id_and_type}",
              );
            }
          }
        }
      }
    }
    // Subtract events already registered for - this only works with known contacts
    $cids = array_filter($this->existing_contacts);
    if ($this->events && $cids) {
      $dao = CRM_Core_DAO::executeQuery("SELECT event_id, contact_id
      FROM civicrm_participant p, civicrm_participant_status_type s
      WHERE s.id = p.status_id AND s.is_counted = 1
      AND event_id IN (" . implode(',', array_keys($this->events)) . ")
      AND contact_id IN (" . implode(',', $cids) . ")
      AND is_test = 0");
      while ($dao->fetch()) {
        if (isset($this->events[$dao->event_id])) {
          if (!(--$this->events[$dao->event_id]['count'])) {
            unset($this->events[$dao->event_id]);
          }
        }
        foreach ($this->line_items as $k => &$item) {
          if ($dao->event_id == $item['event_id'] && in_array($dao->contact_id, $item['contact_ids'])) {
            unset($this->line_items[$k]['contact_ids'][array_search($dao->contact_id, $item['contact_ids'])]);
            if (!(--$item['qty'])) {
              unset($this->line_items[$k]);
            }
          }
        }
      }
      $dao->free();
    }
    $this->loadEvents();
    // Add event info to line items
    foreach ($this->line_items as &$item) {
      $item['label'] = $this->events[$item['event_id']]['title'];
      $item['financial_type_id'] = wf_crm_aval($this->events[$item['event_id']], 'financial_type_id', 'Event Fee');
    }
    // Form Validation
    if (!empty($this->data['reg_options']['validate'])) {
      foreach ($this->events as $eid => $event) {
        if ($event['ended']) {
          form_set_error($eid, t('Sorry, you can no longer register for %event.', array('%event' => $event['title'])));
        }
        elseif ($event['max_participants'] && $event['count'] > $event['remaining']) {
          if (!empty($event['full'])) {
            form_set_error($eid, '<em>' . $event['title'] . '</em>: ' . $event['full_message']);
          }
          else {
            form_set_error($eid, format_plural($event['remaining'],
              'Sorry, you tried to register !count people for %event but there is only 1 space remaining.',
              'Sorry, you tried to register !count people for %event but there are only @count spaces remaining.',
              array('%event' => $event['title'], '!count' => $event['count'])));
          }
        }
      }
    }
  }

  /**
   * Load entire webform submission during validation, including contact ids and $this->data
   * Used when validation for one page needs access to submitted values from other pages
   */
  private function loadMultiPageData() {
    if (!$this->multiPageDataLoaded) {
      $this->multiPageDataLoaded = TRUE;
      if (!empty($this->form_state['storage']['submitted'])
        && wf_crm_aval($this->form_state, 'storage:page_num', 1) > 1)
      {
        $this->rawValues += $this->form_state['storage']['submitted'];
        $this->crmValues = wf_crm_enabled_fields($this->node, $this->rawValues);
      }

      // Check how many valid contacts we have
      foreach ($this->data['contact'] as $c => $contact) {
        // Check if we have a contact_id
        $fid = "civicrm_{$c}_contact_1_contact_existing";
        if ($this->verifyExistingContact(wf_crm_aval($this->crmValues, $fid), $fid)) {
          $this->existing_contacts[$c] = $this->crmValues[$fid];
        }
        // Or else see if enough info was entered to create a contact - use 0 as a placeholder for unknown cid
        elseif (wf_crm_name_field_exists($this->crmValues, $c, $contact['contact'][1]['contact_type'])) {
          $this->existing_contacts[$c] = 0;
        }
      }

      // Fill data array with submitted form values
      $this->fillDataFromSubmission();
    }
  }

  /**
   * If this is an update op, set param for drupal_write_record()
   */
  private function setUpdateParam() {
    if (!empty($this->submission->sid)) {
      $submitted = array($this->submission->sid => new stdClass());
      webform_civicrm_webform_submission_load($submitted);
      if (isset($submitted[$this->submission->sid]->civicrm)) {
        $this->update = 'sid';
      }
    }
  }

  /**
   * Fetch contact ids from "existing contact" fields
   */
  private function getExistingContactIds() {
    foreach ($this->enabled as $field_key => $fid) {
      if (substr($field_key, -8) == 'existing') {
        list(, $c, ) = explode('_', $field_key, 3);
        $cid = wf_crm_aval($this->submissionValue($fid), 0);
        $this->id['cid'][$c] = $this->verifyExistingContact($cid, $field_key);
      }
    }
    $this->existing_contacts = $this->id['cid'];
  }

  /**
   * Ensure we have a valid contact id in a contact ref field
   * @param $cid
   * @param $fid
   * @return int
   */
  private function verifyExistingContact($cid, $fid) {
    if ($cid && is_numeric($cid) && !empty($this->enabled[$fid])) {
      module_load_include('inc', 'webform_civicrm', 'includes/contact_component');
      $component = $this->node->webform['components'][$this->enabled[$fid]];
      $filters = wf_crm_search_filters($this->node, $component);
      // Verify access to this contact
      if (wf_crm_contact_access($component, $filters, $cid) !== FALSE) {
        return $cid;
      }
    }
    return 0;
  }

  /**
   * Check if at least one required field was filled for a contact
   * @param array $contact
   * @return bool
   */
  private function isContactEmpty($contact) {
    $contact_type = $contact['contact'][1]['contact_type'];
    foreach (wf_crm_required_contact_fields($contact_type) as $f) {
      if (!empty($contact[$f['table']][1][$f['name']])) {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Search for an existing contact using default strict rule
   * @param array $contact
   * @return int
   */
  private function findDuplicateContact($contact) {
    $params = array('check_permission' => FALSE);
    foreach ($contact as $table => $field) {
      if (is_array($field) && !empty($field[1])) {
        if (substr($table, 0, 2) == 'cg') {
          //TODO pass custom data to deduper
        }
        // If sharing an address, use the master
        elseif ($table == 'address' && !empty($field[1]['master_id'])) {
          $m = $field[1]['master_id'];
          // If master address is exposed to the form, use it
          if (!empty($contact[$m]['address'][1])) {
            $params['civicrm_address'] = $contact[$m]['address'][1];
          }
          // Else look up the master contact's address
          elseif (!empty($this->existing_contacts[$m])) {
            $masters = wf_civicrm_api('address', 'get',
              array('contact_id' => $this->id['cid'][$m], 'sort' => 'is_primary DESC'));
            if (!empty($masters['values'])) {
              $params['civicrm_address'] = array_shift($masters['values']);
            }
          }
        }
        elseif (in_array($table, array('contact', 'address', 'email', 'phone', 'website'))) {
          $params['civicrm_' . $table] = $field[1];
        }
      }
    }
    if ($dupes = CRM_Dedupe_Finder::dupesByParams($params, ucfirst($contact['contact'][1]['contact_type']))) {
      return $dupes[0];
    }
    return 0;
  }

  /**
   * Create a new contact
   * @param array $contact
   * @return int
   */
  private function createContact($contact) {
    $params = $contact['contact'][1];
    // CiviCRM API is too picky about this, imho
    $params['contact_type'] = ucfirst($params['contact_type']);
    unset($params['contact_id'], $params['id']);
    $params['source'] = $this->settings['new_contact_source'];
    // If creating individual with no first/last name,
    // set display name and sort_name
    if ($params['contact_type'] == 'Individual' && empty($params['first_name']) && empty($params['last_name'])) {
      $params['display_name'] = $params['sort_name'] = empty($params['nick_name']) ? $contact['email'][1]['email'] : $params['nick_name'];
    }
    $result = wf_civicrm_api('contact', 'create', $params);
    return wf_crm_aval($result, 'id', 0);
  }

  /**
   * Update a contact
   * @param array $contact
   * @param int $c
   */
  private function updateContact($contact, $c) {
    $params = $contact['contact'][1];
    unset($params['contact_type'], $params['contact_id']);
    // Fetch data from existing multivalued fields
    $fetch = $multi = array();
    foreach ($this->all_fields as $fid => $field) {
      if (!empty($field['extra']['multiple']) && substr($fid, 0, 7) == 'contact') {
        list(, $name) = explode('_', $fid, 2);
        if ($name != 'privacy' && isset($params[$name])) {
          $fetch["return.$name"] = 1;
          $multi[] = $name;
        }
      }
    }
    // Merge data from existing multivalued fields
    if ($multi) {
      $existing = wf_civicrm_api('contact', 'get', array('id' => $this->id['cid'][$c]) + $fetch);
      $existing = wf_crm_aval($existing, 'values:' . $this->id['cid'][$c], array());
      foreach ($multi as $name) {
        $exist = array_filter(drupal_map_assoc((array) wf_crm_aval($existing, $name, array())));
        // Only known contacts are allowed to empty a field
        if (!empty($this->existing_contacts[$c])) {
          foreach ($this->getExposedOptions("civicrm_{$c}_contact_1_contact_$name") as $k => $v) {
            unset($exist[$k]);
          }
        }
        $params[$name] = array_unique(array_merge($params[$name], $exist));
      }
    }
    $params['id'] = $this->id['cid'][$c];
    wf_civicrm_api('contact', 'create', $params);
  }

  /**
   * Save current employer for a contact
   * @param array $contact
   * @param int $cid
   */
  function saveCurrentEmployer($contact, $cid) {
    if ($contact['contact'][1]['contact_type'] == 'individual' && !empty($contact['contact'][1]['employer_id'])) {
      wf_civicrm_api('contact', 'create', array(
        'id' => $cid,
        'employer_id' => $contact['contact'][1]['employer_id'],
      ));
    }
  }

  /**
   * Fill values for hidden ID & CS fields
   * @param int $c
   * @param int $cid
   */
  private function fillHiddenContactFields($cid, $c) {
    $fid = 'civicrm_' . $c . '_contact_1_contact_';
    if (!empty($this->enabled[$fid . 'contact_id'])) {
      $this->submissionValue($this->enabled[$fid . 'contact_id'], $cid);
    }
    if (!empty($this->enabled[$fid . 'existing'])) {
      $this->submissionValue($this->enabled[$fid . 'existing'], $cid);
    }
    if (!empty($this->enabled[$fid . 'external_identifier']) && !empty($this->existing_contacts[$c])) {
      $exid = wf_civicrm_api('contact', 'get', array('contact_id' => $cid, 'return.external_identifier' => 1));
      $this->submissionValue($this->enabled[$fid . 'external_identifier'], wf_crm_aval($exid, "values:$cid:external_identifier"));
    }
    if (!empty($this->enabled[$fid . 'cs'])) {
      $cs = $this->submissionValue($this->enabled[$fid . 'cs']);
      $life = !empty($cs[0]) ? intval(24 * $cs[0]) : 'inf';
      $cs = CRM_Contact_BAO_Contact_Utils::generateChecksum($cid, NULL, $life);
      $this->submissionValue($this->enabled[$fid . 'cs'], $cs);
    }
  }

  /**
   * Save location data for a contact
   * @param array $contact
   * @param int $cid
   * @param int $c
   */
  private function saveContactLocation($contact, $cid, $c) {
    foreach (wf_crm_location_fields() as $location) {
      if (!empty($contact[$location])) {
        $existing = array();
        $params = array('contact_id' => $cid);
        if ($location != 'website') {
          $params['options'] = array('sort' => 'is_primary DESC');
        }
        $result = wf_civicrm_api($location, 'get', $params);
        if (!empty($result['values'])) {
          // start array index at 1
          $existing = array_merge(array(array()), $result['values']);
        }
        foreach ($contact[$location] as $i => $params) {
          // Translate state/prov abbr to id
          if (!empty($params['state_province_id'])) {
            $config = CRM_Core_Config::singleton();
            if (!($params['state_province_id'] = wf_crm_state_abbr($params['state_province_id'], 'id', wf_crm_aval($params, 'country_id', $config->defaultContactCountry)))) {
              $params['state_province_id'] = '';
            }
          }
          // Update drupal email address
          if ($location == 'email' && !empty($params['email']) && $i == 1) {
            $uid = wf_crm_user_cid($cid, 'contact');
            if ($uid) {
              $user = user_load($uid);
              if ($params['email'] != $user->mail) {
                // Verify this email is unique before saving it to user
                $args = array(':mail' => $params['email']);
                if (!(db_query("SELECT count(uid) FROM {users} WHERE mail = :mail", $args)->fetchField())) {
                  user_save($user, array('mail' => $params['email']));
                }
              }
            }
          }
          // Check if anything was changed, else skip the update
          if (!empty($existing[$i])) {
            $same = TRUE;
            foreach ($params as $param => $val) {
              if ($val != (string) wf_crm_aval($existing[$i], $param, '')) {
                $same = FALSE;
              }
            }
            if ($same) {
              continue;
            }
          }
          if ($location == 'address') {
            // Store shared addresses for later since we haven't necessarily processed
            // the contact this address is shared with yet.
            if (!empty($params['master_id'])) {
              $this->shared_address[$cid][$i] = array(
                'id' => wf_crm_aval($existing, "$i:id"),
                'mc' => $params['master_id'],
                'loc' => $params['location_type_id'],
              );
              continue;
            }
            // Reset calculated values when updating an address
            $params['master_id'] = $params['geo_code_1'] = $params['geo_code_2'] = 'null';
          }
          $params['contact_id'] = $cid;
          if (!empty($existing[$i])) {
            $params['id'] = $existing[$i]['id'];
          }
          if ($this->locationIsEmpty($location, $params)) {
            // Delete this location if nothing was entered and this is a known contact
            if (!empty($this->existing_contacts[$c]) && !empty($params['id'])) {
              wf_civicrm_api($location, 'delete', $params);
            }
            continue;
          }
          if ($location != 'website') {
            if (empty($params['location_type_id'])) {
              $params['location_type_id'] = wf_crm_aval($existing, "$i:location_type_id", 1);
            }
            $params['is_primary'] = $i == 1 ? 1 : 0;
          }
          wf_civicrm_api($location, 'create', $params);
        }
      }
    }
  }

  /**
   * Save groups and tags for a contact
   * @param array $contact
   * @param int $cid
   * @param int $c
   */
  private function saveGroupsAndTags($contact, $cid, $c) {
    // Process groups & tags
    foreach ($this->all_fields as $fid => $field) {
      list($set, $type) = explode('_', $fid, 2);
      if ($set == 'other') {
        $field_name = 'civicrm_' . $c . '_contact_1_' . $fid;
        if (!empty($contact['other'][1][$type]) || isset($this->enabled[$field_name])) {
          $add = wf_crm_aval($contact, "other:1:$type", array());
          $remove = empty($this->existing_contacts[$c]) ? array() : $this->getExposedOptions($field_name, $add);
          $this->addOrRemoveMultivaluedData($field['table'], 'contact', $cid, $add, $remove);
        }
      }
    }
  }

  /**
   * Handle adding/removing multivalued data for a contact/activity/etc.
   * Currently used only for groups and tags, but written with expansion in mind.
   *
   * @param $data_type
   *   'group' or 'tag'
   * @param $entity_type
   *   Parent entity: 'contact' etc.
   * @param $id
   *   Entity id
   * @param $add
   *   Groups/tags to add
   * @param array $remove
   *   Groups/tags to remove
   */
  private function addOrRemoveMultivaluedData($data_type, $entity_type, $id, $add, $remove = array()) {
    $confirmations_sent = $existing = $params = array();
    $add = drupal_map_assoc($add);
    static $mailing_lists = array();

    switch ($data_type) {
      case 'group':
        $api = 'group_contact';
        break;
      case 'tag':
        $api = 'entity_tag';
        break;
      default:
        $api = $data_type;
    }
    if (!empty($add) || !empty($remove)) {
      // Retrieve current records for this entity
      if ($entity_type == 'contact') {
        $params['contact_id'] = $id;
      }
      else {
        $params['entity_id'] = $id;
        $params['entity_type'] = 'civicrm_' . $entity_type;
      }
      $fetch = wf_civicrm_api($api, 'get', $params);
      foreach (wf_crm_aval($fetch, 'values', array()) as $i) {
        $existing[] = $i[$data_type . '_id'];
        unset($add[$i[$data_type . '_id']]);
      }
      foreach ($remove as $i => $name) {
        if (!in_array($i, $existing)) {
          unset($remove[$i]);
        }
      }
    }
    if (!empty($add)) {
      // Prepare for sending subscription confirmations
      if ($data_type == 'group' && !empty($this->settings['confirm_subscription'])) {
        // Retrieve this contact's primary email address and perform error-checking
        $result = wf_civicrm_api('email', 'get', array('contact_id' => $id, 'options' => array('sort' => 'is_primary DESC')));
        if (!empty($result['values'])) {
          foreach ($result['values'] as $value) {
            if (($value['is_primary'] || empty($email)) && strpos($value['email'], '@')) {
              $email = $value['email'];
            }
          }
          $mailer_params = array(
            'contact_id' => $id,
            'email' => $email,
          );
          if (empty($mailing_lists)) {
            $mailing_lists = wf_crm_apivalues('group', 'get', array('visibility' => 'Public Pages', 'group_type' => 2), 'title');
          }
        }
      }
      foreach ($add as $a) {
        $params[$data_type . '_id'] = $mailer_params['group_id'] = $a;
        if ($data_type == 'group' && isset($mailing_lists[$a]) && !empty($email)) {
          $result = wf_civicrm_api('mailing_group', 'event_subscribe', $mailer_params);
          if (empty($result['is_error'])) {
            $confirmations_sent[] = check_plain($mailing_lists[$a]);
          }
          else {
            wf_civicrm_api($api, 'create', $params);
          }
        }
        else {
          wf_civicrm_api($api, 'create', $params);
        }
      }
      if ($confirmations_sent) {
        drupal_set_message(t('A message has been sent to %email to confirm subscription to !group.', array('%email' => $email, '!group' => '<em>' . implode('</em> ' . t('and') . ' <em>', $confirmations_sent) . '</em>')));
      }
    }
    // Remove data from entity
    foreach ($remove as $a => $name) {
      $params[$data_type . '_id'] = $a;
      wf_civicrm_api($api, 'delete', $params);
    }
    if (!empty($remove) && $data_type == 'group') {
      $display_name = wf_civicrm_api('contact', 'get', array('contact_id' => $id, 'return.display_name' => 1));
      $display_name = wf_crm_aval($display_name, "values:$id:display_name", t('Contact'));
      drupal_set_message(t('%contact has been removed from !group.', array('%contact' => $display_name, '!group' => '<em>' . implode('</em> ' . t('and') . ' <em>', $remove) . '</em>')));
    }
  }

  /**
   * Add/update relationship for a pair of contacts
   *
   * @param $params
   *   Params array for relationship api
   * @param $cid1
   *   Contact id
   * @param $cid2
   *   Contact id
   */
  private function processRelationship($params, $cid1, $cid2) {
    if (!empty($params['relationship_type_id']) && $cid2 && $cid1 != $cid2) {
      list($type, $side) = explode('_', $params['relationship_type_id']);
      $existing = $this->getRelationship(array($params['relationship_type_id']), $cid1, $cid2);
      $perm = wf_crm_aval($params, 'relationship_permission');
      // Swap contacts if this is an inverse relationship
      if ($side == 'b' || ($existing && $existing['contact_id_a'] != $cid1)) {
        list($cid1, $cid2) = array($cid2, $cid1);
        if ($perm == 1 || $perm == 2) {
          $perm = $perm == 1 ? 2 : 1;
        }
      }
      $params += $existing;
      $params['contact_id_a'] = $cid1;
      $params['contact_id_b'] = $cid2;
      $params['relationship_type_id'] = $type;
      if ($perm) {
        $params['is_permission_a_b'] = $params['is_permission_b_a'] = $perm == 3 ? 1 : 0;
        if ($perm == 1 || $perm == 2) {
          $params['is_permission_' . ($perm == 1 ? 'a_b' : 'b_a')] = 1;
        }
      }
      unset($params['relationship_permission']);
      wf_civicrm_api('relationship', 'create', $params);
    }
  }

  /**
   * Process event participation for a contact
   * @param int $c
   * @param int $cid
   */
  private function processParticipants($c, $cid) {
    static $registered_by_id = array();
    $n = $this->data['participant_reg_type'] == 'separate' ? $c : 1;
    if ($p = wf_crm_aval($this->data, "participant:$n:participant")) {
      // Fetch existing participant records
      $existing = array();
      $dao = CRM_Core_DAO::executeQuery("SELECT id, event_id FROM civicrm_participant WHERE contact_id = $cid AND is_test = 0");
      while ($dao->fetch()) {
        $existing[$dao->event_id] = $dao->id;
      }
      foreach ($p as $e => $params) {
        $remove = array();
        $fid = "civicrm_{$c}_participant_{$e}_participant_event_id";
        // Automatic status - de-selected events will be cancelled
        if (empty($params['status_id'])) {
          foreach ($this->getExposedOptions($fid) as $eid => $title) {
            list($eid) = explode('-', $eid);
            if (isset($existing[$eid])) {
              $remove[$eid] = $title;
            }
          }
        }
        if (!empty($params['event_id'])) {
          $params['contact_id'] = $cid;
          if (empty($params['campaign_id']) || empty($this->all_fields['participant_campaign_id'])) {
            unset($params['campaign_id']);
          }
          // Reformat custom data from nested arrays
          $custom = array();
          foreach ($this->data['participant'][$n] as $key => $vals) {
            if (substr($key, 0, 2) == 'cg' && isset($vals[$e])) {
              $custom[$key][1] = $vals[$e];
            }
          }
          // Loop through event ids to support multi-valued form elements
          $this->events = (array) $params['event_id'];
          foreach ($this->events as $i => $id_and_type) {
            if (!empty($id_and_type)) {
              list($eid) = explode('-', $id_and_type);
              $params['event_id'] = $eid;
              unset($remove[$eid], $params['registered_by_id'], $params['id'], $params['source']);
              // Is existing participant?
              if (!empty($existing[$eid])) {
                $params['id'] = $existing[$params['event_id']];
              }
              else {
                $params['source'] = $this->settings['new_contact_source'];
                if ($c > 1 && !empty($registered_by_id[$e][$i])) {
                  $params['registered_by_id'] = $registered_by_id[$e][$i];
                }
              }
              // Automatic status
              if (empty($params['status_id']) && empty($params['id'])) {
                $params['status_id'] = 'Registered';
                // Pay later status
                if ($this->contributionIsPayLater && !empty($params['fee_amount'])) {
                  $params['status_id'] = 'Pending from pay later';
                }
              }
              // Do not update status of existing participant in "Automatic" mode
              if (empty($params['status_id'])) {
                unset($params['status_id']);
              }
              $result = wf_civicrm_api('participant', 'create', $params);
              // Update line-item
              foreach ($this->line_items as &$item) {
                if ($item['element'] == "civicrm_{$n}_participant_{$e}_participant_{$id_and_type}") {
                  if (empty($item['participant_id'])) {
                    $item['participant_id'] = $item['entity_id'] = $result['id'];
                  }
                  $item['participant_count'] = wf_crm_aval($item, 'participant_count', 0) + 1;
                  break;
                }
              }
              // When registering contact 1, store id to apply to other contacts
              if ($c == 1) {
                $registered_by_id[$e][$i] = $result['id'];
              }
              if ($custom) {
                $this->saveCustomData($custom, $result['id'], 'Participant');
              }
            }
          }
        }
        foreach ($remove as $eid => $title) {
          wf_civicrm_api('participant', 'create', array('status_id' => "Cancelled", 'id' => $existing[$eid]));
          drupal_set_message(t('Registration cancelled for !event', array('!event' => $title)));
        }
      }
    }
  }

  /**
   * Process memberships for a contact
   * Called during webform submission
   * @param int $c
   * @param int $cid
   */
  private function processMemberships($c, $cid) {
    static $types;
    if (!isset($types)) {
      $types = wf_crm_apivalues('membership_type', 'get');
    }
    $existing = wf_crm_apivalues('membership', 'get', array('contact_id' => $cid, 'filters' => array('is_current' => 1)));
    foreach (wf_crm_aval($this->data, "membership:$c:membership", array()) as $n => $params) {
      if (empty($params['membership_type_id'])) {
        continue;
      }
      // Search for existing membership to renew - must belong to same domain and organization
      // But not necessarily the same membership type to allow for upsell
      if (!empty($params['num_terms'])) {
        $type = $types[$params['membership_type_id']];
        foreach ($existing as $id => $mem) {
          $existing_type = $types[$mem['membership_type_id']];
          if ($type['domain_id'] == $existing_type['domain_id'] && $type['member_of_contact_id'] == $existing_type['member_of_contact_id']) {
            $params['id'] = $id;
            // If we have an exact match, look no further
            if ($mem['membership_type_id'] == $params['membership_type_id']) {
              break;
            }
          }
        }
      }
      if (empty($params['id'])) {
        $params['source'] = $this->settings['new_contact_source'];
      }
      // Automatic status
      if (empty($params['status_id'])) {
        unset($params['status_id']);
        // Pay later status
        if ($this->contributionIsPayLater && $this->getMembershipTypeField($params['membership_type_id'], 'minimum_fee')) {
          $params['status_id'] = 'Pending';
        }
      }
      $params['contact_id'] = $cid;
      // The api won't let us manually set status without this weird param
      $params['skipStatusCal'] = !empty($params['status_id']);

      $result = wf_civicrm_api('membership', 'create', $params);
      foreach ($this->line_items as &$item) {
        if ($item['element'] == "civicrm_{$c}_membership_{$n}") {
          $item['membership_id'] = $result['id'];
          break;
        }
      }
    }
  }

  /**
   * Process shared addresses
   */
  private function processSharedAddresses() {
    foreach ($this->shared_address as $cid => $shared) {
      foreach ($shared as $i => $addr) {
        if (!empty($this->id['cid'][$addr['mc']])) {
          $masters = wf_civicrm_api('address', 'get', array('contact_id' => $this->id['cid'][$addr['mc']], 'options' => array('sort' => 'is_primary DESC')));
          if (!empty($masters['values'])) {
            $masters = array_values($masters['values']);
            // Pick the address with the same location type; default to primary.
            $params = $masters[0];
            foreach ($masters as $m) {
              if ($m['location_type_id'] == $addr['loc']) {
                $params = $m;
                break;
              }
            }
            $params['master_id'] = $params['id'];
            $params['id'] = $addr['id'];
            $params['contact_id'] = $cid;
            $params['is_primary'] = $i == 1;
            wf_civicrm_api('address', 'create', $params);
          }
        }
      }
    }
  }

  /**
   * Save case data
   */
  private function processCase() {
    if (empty($this->id['case'][1])) {
      // Search for case
      $case = $this->findCase();
      if ($case) {
        $this->id['case'][1] = $case['id'];
      }
    }
    // Default subject
    if (empty($this->id['case'][1]) && empty($this->data['case'][1]['case'][1]['subject'])) {
      $this->data['case'][1]['case'][1]['subject'] = check_plain($this->node->title);
    }
    if (!empty($this->id['case'][1])) {
      $this->data['case'][1]['case'][1]['id'] = $this->id['case'][1];
      unset($this->data['case'][1]['case'][1]['creator_id'], $this->data['case'][1]['case'][1]['case_type_id']);
    }
    // Save case
    $result = wf_civicrm_api('case', 'create', $this->data['case'][1]['case'][1]);
    if (!empty($result['id'])) {
      $this->saveCustomData($this->data['case'][1], $result['id'], 'Case');
      $this->id['case'][1] = $result['id'];
    }
  }

  /**
   * Save activity data
   */
  private function processActivity() {
    // Search for activity
    if ((!empty($this->id['cid'][1]) && empty($this->data['case'][1])) || (!empty($this->data['case'][1]) && !empty($this->id['case'][1]))) {
      $params = array(
        'activity_type_id' => $this->data['activity'][1]['activity'][1]['activity_type_id'],
        'status_id' => $this->data['activity'][1]['existing_activity_status'],
      );
      if (!empty($this->data['case'][1])) {
        $params['case_id'] = $this->id['case'][1];
      }
      else {
        $params['contact_id'] = $this->id['cid'][1];
      }
      if (empty($this->id['act'][1])) {
        $this->id['act'][1] = $this->findActivity($params);
      }
    }
    // Save activity
    $params = $this->data['activity'][1]['activity'][1];
    if (is_array($params['status_id'])) {
      $params['status_id'] = reset($params['status_id']);
    }
    // Existing activity - set id
    if (!empty($this->id['act'][1])) {
      $params['id'] = $this->id['act'][1];
    }
    // New activity - set case id and source_contact
    else {
      if (!empty($this->id['case'][1])) {
        $params['case_id'] = $this->id['case'][1];
        $params['medium_id'] = $this->data['case'][1]['case'][1]['medium_id'];
      }
      $cid = wf_crm_user_cid();
      if ($cid) {
        $params['source_contact_id'] = $cid;
      }
      else {
        foreach ($this->id['cid'] as $cid) {
          if ($cid) {
            $params['source_contact_id'] = $cid;
            break;
          }
        }
      }
    }
    // Can't pass an empty contact id to the api
    if (empty($params['assignee_contact_id'])) {
      unset($params['assignee_contact_id']);
    }
    // Format details as html
    $params['details'] = nl2br(wf_crm_aval($params, 'details', ''));
    if (empty($params['subject'])) {
      $params['subject'] = $this->data['activity'][1]['activity'][1]['subject'];
    }
    if (!empty($this->data['activity'][1]['details']['entire_result'])) {
      module_load_include('inc', 'webform', 'includes/webform.submissions');
      $params['details'] .= webform_submission_render($this->node, $this->submission, NULL, 'html');
    }
    if (!empty($this->data['activity'][1]['details']['view_link'])) {
      $params['details'] .= '<p>' . l(t('View Webform Submission'), 'node/' . $this->node->nid . '/submission/' . $this->submission->sid, array('absolute' => TRUE, 'alias' => TRUE)) . '</p>';
    }
    if (!empty($this->data['activity'][1]['details']['edit_link'])) {
      $params['details'] .= '<p>' . l(t('Edit Submission'), 'node/' . $this->node->nid . '/submission/' . $this->submission->sid . '/edit',  array('absolute' => TRUE, 'alias' => TRUE)) . '</p>';
    }
    if (empty($params['campaign_id']) || empty($this->all_fields['activity_campaign_id'])) {
      unset($params['campaign_id']);
    }
    if (!empty($this->data['activity'][1]['activity'][1]['survey_id'])) {
      $params['source_record_id'] = $this->data['activity'][1]['activity'][1]['survey_id'];
    }
    $result = wf_civicrm_api('activity', 'create', $params);

    if (!empty($result['id'])) {
      $this->id['act'][1] = $result['id'];
      $this->saveCustomData($this->data['activity'][1], $this->id['act'][1], 'Activity');
    }
    if (!empty($this->id['act'][1]) && !empty($params['assignee_contact_id'])) {
      if (CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME, 'activity_assignee_notification')) {
        // Send email to assignees. TODO: Move to CiviCRM API?
        $assignee = wf_civicrm_api('contact', 'get', array('id' => $params['assignee_contact_id']));
        $assignee = wf_crm_aval($assignee, 'values:' . $params['assignee_contact_id']);
        if (!empty($assignee['email'])) {
          $mail = array($assignee['email'] => $assignee);
          // Include attachments while sending a copy of activity.
          $attachments = &CRM_Core_BAO_File::getEntityFile('civicrm_activity', $this->id['act'][1]);
          CRM_Case_BAO_Case::sendActivityCopy(NULL, $this->id['act'][1], $mail, $attachments, NULL);
        }
      }
    }
  }

  /**
   * Calculate line-items for this webform submission
   */
  private function tallyLineItems() {
    // Contribution
    $fid = 'civicrm_1_contribution_1_contribution_total_amount';
    if (isset($this->enabled[$fid]) || $this->getData($fid) > 0) {
      $this->line_items[] = array(
        'qty' => 1,
        'unit_price' => $this->getData($fid),
        'financial_type_id' => $this->contribution_page['financial_type_id'],
        'label' => wf_crm_aval($this->node->webform['components'], $this->enabled[$fid] . ':name', t('Contribution')),
        'element' => 'civicrm_1_contribution_1',
        'entity_table' => 'civicrm_contribution',
      );
    }
    // Membership
    foreach (wf_crm_aval($this->data, 'membership', array()) as $c => $memberships) {
      if (isset($this->existing_contacts[$c]) && !empty($memberships['number_of_membership'])) {
        foreach ($memberships['membership'] as $n => $item) {
          if (!empty($item['num_terms']) && !empty($item['membership_type_id'])) {
            $type = $item['membership_type_id'];
            $price = $this->getMembershipTypeField($type, 'minimum_fee');
            if ($price) {
              $this->line_items[] = array(
                'qty' => $item['num_terms'],
                'unit_price' => $price,
                'financial_type_id' => $this->getMembershipTypeField($type, 'financial_type_id'),
                'label' => $this->getMembershipTypeField($type, 'name'),
                'element' => "civicrm_{$c}_membership_{$n}",
                // Seems strange to not be civicrm_membership, but this is how it's done in core
                'entity_table' => 'civicrm_contribution',
              );
            }
          }
        }
      }
    }
    // Calculate totals
    $this->totalContribution = 0;
    foreach ($this->line_items as &$item) {
      $this->totalContribution += $item['line_total'] = $item['unit_price'] * (int) $item['qty'];
    }
    return $this->totalContribution;
  }

  /**
   * Are billing fields exposed to this webform page?
   * @return bool
   */
  private function isPaymentPage() {
    return !empty($this->crmValues['civicrm_1_contribution_1_contribution_contribution_page_id']) &&
      !empty($this->data['contribution'][1]['contribution'][1]['payment_processor_id']);
  }

  /**
   * Normalize and validate billing input
   * @return bool
   */
  private function validateBillingFields() {
    $valid = TRUE;
    $params = $card_errors = array();
    // These are hard-coded in CiviCRM so we may as well hard-code them here
    // Value = translated label to be shown during validation or FALSE if not required
    $billing_fields = array(
      'credit_card_number' => ts('Card Number'),
      'cvv2' => ts('Security Code'),
      'credit_card_type' => ts('Card Type'),
      'billing_first_name' => ts('Billing First Name'),
      'billing_middle_name' => FALSE,
      'billing_last_name' => ts('Billing Last Name'),
      'billing_street_address-5' => ts('Street Address'),
      'billing_city-5' => ts('City'),
      'billing_country_id-5' => ts('Country'),
      'billing_state_province_id-5' => FALSE,
      'billing_postal_code-5' => ts('Postal Code'),
    );
    foreach ($billing_fields as $field => $label) {
      if (empty($_POST[$field]) && $label !== FALSE) {
        form_set_error($field, t('!name field is required.', array('!name' => check_plain($label))));
        $valid = FALSE;
      }
      if (!empty($_POST[$field])) {
        $name = str_replace('billing_', '', str_replace('-5', '', $field));
        $submitted[$name] = $params[$name] = $params[$field] = $_POST[$field];
      }
    }
    // Validate country
    if (!empty($params['country_id'])) {
      if (!array_key_exists($params['country_id'], wf_crm_apivalues('address', 'getoptions', array('field' => 'country_id')))) {
        form_set_error('billing_country_id-5', t('Illegal value entered for Country'));
        $valid = $params['country_id'] = FALSE;
      }
    }
    // Validate state/province
    if (!empty($params['country_id'])) {
      $states = wf_crm_apivalues('address', 'getoptions', array('field' => 'state_province_id', 'country_id' => $params['country_id']));
      if ($states && (empty($params['state_province_id']) || !isset($states[$params['state_province_id']]))) {
        form_set_error('billing_state_province_id-5', t('!name field is required.', array('!name' => check_plain(ts('State/Province')))));
        $valid = FALSE;
      }
    }
    // Validate credit card number & cvv2
    CRM_Core_Payment_Form::validateCreditCard($params, $card_errors);
    foreach ($card_errors as $field => $msg) {
      form_set_error($field, $msg);
      $valid = FALSE;
    }
    // Check expiration date
    $submitted['credit_card_exp_date[Y]'] = $params['year'] = wf_crm_aval($_POST, 'credit_card_exp_date:Y', 0);
    // There seems to be some inconsistency with capitalization here
    $params['month'] = (int) wf_crm_aval($_POST, 'credit_card_exp_date:M', wf_crm_aval($_POST, 'credit_card_exp_date:m', 0));
    $submitted['credit_card_exp_date[M]'] = $submitted['credit_card_exp_date[m]'] = $params['month'];
    if ($params['year'] < date('Y') || ($params['year'] == date('Y') && $params['month'] < date('n'))) {
      form_set_error('billing', ts('Credit card expiration date cannot be a past date.'));
      $valid = FALSE;
    }
    // Email
    for ($i = 1; $i <= $this->data['contact'][1]['number_of_email']; ++$i) {
      if (!empty($this->crmValues["civicrm_1_contact_{$i}_email_email"])) {
        $params['email'] = $this->crmValues["civicrm_1_contact_{$i}_email_email"];
        break;
      }
    }
    if (empty($params['email'])) {
      form_set_error('billing_email', ts('An email address is required to complete this transaction.'));
      $valid = FALSE;
    }
    if ($valid) {
      $this->billing_params = $params;
    }
    // Since billing fields are not "real" form fields they get cleared if the page reloads.
    // We add a bit of js to fix this annoyance.
    drupal_add_js(array('webform_civicrm' => array('billingSubmission' => $submitted)), 'setting');
    return $valid;
  }

  /**
   * Create contact 1 if not already existing (required by contribution.transact)
   * @return int
   */
  private function createBillingContact() {
    $cid = wf_crm_aval($this->existing_contacts, 1);
    if (!$cid) {
      $contact = $this->data['contact'][1];
      // Only use middle name from billing if we are using the rest of the billing name as well
      if (empty($contact['contact'][1]['first_name']) && !empty($this->billing_params['middle_name'])) {
        $contact['contact'][1]['middle_name'] = $this->billing_params['middle_name'];
      }
      $contact['contact'][1] += array(
        'first_name' => $this->billing_params['first_name'],
        'last_name' => $this->billing_params['last_name'],
      );
      $cid = $this->findDuplicateContact($contact);
    }
    $address = array(
      'street_address' => $this->billing_params['street_address'],
      'city' => $this->billing_params['city'],
      'country_id' => $this->billing_params['country_id'],
      'state_province_id' => wf_crm_aval($this->billing_params, 'state_province_id'),
      'postal_code' => $this->billing_params['postal_code'],
      'location_type_id' => 'Billing',
    );
    $email = array(
      'email' => $this->billing_params['email'],
      'location_type_id' => 'Billing',
    );
    if (!$cid) {
      $cid = $this->createContact($contact);
    }
    else {
      foreach (array('address', 'email') as $loc) {
        $result = wf_civicrm_api($loc, 'get', array(
          'contact_id' => $cid,
          'location_type_id' => 'Billing',
        ));
        // Use first id if we have any results
        if (!empty($result['values'])) {
          $ids = array_keys($result['values']);
          ${$loc}['id'] = $ids[0];
        }
      }
    }
    if ($cid) {
      $address['contact_id'] = $email['contact_id'] = $this->id['cid'][1] = $cid;
      wf_civicrm_api('address', 'create', $address);
      wf_civicrm_api('email', 'create', $email);
    }
    return $cid;
  }

  /**
   * Execute payment processor transaction
   * This happens during form validation
   */
  private function submitPayment() {
    $result = wf_civicrm_api('contribution', 'transact', $this->contributionParams());
    if (empty($result['id'])) {
      form_set_error('contribution', t('Transaction failed. Please verify all billing fields are correct.'));
      return;
    }
    $this->id['contribution'] = $result['id'];
  }

  /**
   * Create Pay-Later Contribution
   */
  private function payLaterContribution() {
    $params = $this->contributionParams();
    $params['receive_date'] = 'now';
    $params['contribution_status_id'] = 'Pending';
    $result = wf_civicrm_api('contribution', 'create', $params);
    $this->id['contribution'] = $result['id'];
    $this->contributionIsPayLater = TRUE;
  }

  /**
   * Format contribution params for transact/pay-later
   * @return array
   */
  private function contributionParams() {
    $params = $this->billing_params + $this->data['contribution'][1]['contribution'][1];
    $params['financial_type_id'] = $this->contribution_page['financial_type_id'];
    $params['currency'] = $this->contribution_page['currency'];
    $params['skipRecentView'] = $params['skipLineItem'] = 1;
    $params['contact_id'] = $this->id['cid'][1];
    $params['source'] = $this->settings['new_contact_source'];
    $params['total_amount'] = $this->totalContribution;

    // Save this stuff for later
    unset($params['soft'], $params['honor_contact_id'], $params['honor_type_id']);
    return $params;
  }

  /**
   * Post-processing of contribution
   * This happens during form post-processing
   */
  private function processContribution() {
    $contribution = $this->data['contribution'][1]['contribution'][1];
    $id = $this->id['contribution'];
    // Save custom data
    if (count($this->data['contribution'][1]) > 1) {
      $this->saveCustomData($this->data['contribution'][1], $id, 'Contribution', FALSE);
    }
    // Save soft credits
    if (!empty($contribution['soft'])) {
      foreach (array_filter($contribution['soft']) as $cid) {
        wf_civicrm_api('contribution_soft', 'create', array(
          'contact_id' => $cid,
          'contribution_id' => $id,
          'amount' => $contribution['total_amount'],
          'currency' => $this->contribution_page['currency'],
        ));
      }
    }
    // Save honoree
    if (!empty($contribution['honor_contact_id']) && !empty($contribution['honor_type_id'])) {
      wf_civicrm_api('contribution', 'create', array(
        'id' => $id,
        'honor_contact_id' => $contribution['honor_contact_id'],
        'honor_type_id' => $contribution['honor_type_id'],
      ));
    }
    // Save line-items
    foreach ($this->line_items as &$item) {
      if (empty($item['line_total'])) {
        continue;
      }
      if (empty($item['entity_id'])) {
        $item['entity_id'] = $id;
      }
      wf_civicrm_api('line_item', 'create', $item);
      // Create participant/membership payment records
      if (isset($item['membership_id']) || isset($item['participant_id'])) {
        $type = isset($item['participant_id']) ? 'participant' : 'membership';
        wf_civicrm_api("{$type}_payment", 'create', array(
          "{$type}_id" => $item["{$type}_id"],
          'contribution_id' => $id,
        ));
      }
    }
    // Send receipt
    if (!empty($this->contribution_page['is_email_receipt'])) {
      wf_civicrm_api('contribution', 'sendconfirmation', array('id' => $id) + $this->contribution_page);
    }
  }

  /**
   * Recursive function to fill ContactRef fields with contact IDs
   *
   * @internal param $values null|array
   *   Leave blank - used internally to recurse through data
   * @internal param $depth int
   *   Leave blank - used internally to track recursion level
   */
  private function fillContactRefs($values = NULL, $depth = 0) {
    $order = array('ent', 'c', 'table', 'n', 'name');
    static $ent = '';
    static $c = '';
    static $table = '';
    static $n = '';
    if ($values === NULL) {
      $values = $this->data;
    }
    foreach ($values as $key => $val) {
      ${$order[$depth]} = $key;
      if ($depth < 4 && is_array($val)) {
        $this->fillContactRefs($val, $depth + 1);
      }
      elseif ($depth == 4 && $val && wf_crm_aval($this->all_fields, "{$table}_$name:data_type") == 'ContactReference') {
        if (is_array($val)) {
          $this->data[$ent][$c][$table][$n][$name] = array();
          foreach ($val as $v) {
            if (is_numeric($v) && !empty($this->id['cid'][$v])) {
              $this->data[$ent][$c][$table][$n][$name][] = $this->id['cid'][$v];
            }
          }
        }
        else {
          unset($this->data[$ent][$c][$table][$n][$name]);
          if (!empty($this->id['cid'][$val])) {
            $this->data[$ent][$c][$table][$n][$name] = $this->id['cid'][$val];
          }
        }
      }
    }
  }

  /**
   * Fill data array with submitted form values
   */
  private function fillDataFromSubmission() {
    $sp = CRM_Core_DAO::VALUE_SEPARATOR;
    foreach ($this->enabled as $field_key => $fid) {
      $val = $this->submissionValue($fid);
      if ($val !== FALSE) {
        list( , $c, $ent, $n, $table, $name) = explode('_', $field_key, 6);
        // Fieldsets and existing contact fields are not strictly CiviCRM fields, so ignore
        if ($name === 'existing' || $name === 'fieldset') {
          continue;
        }
        // Ignore values from fields hidden by existing contact component
        if ($this->isFieldHiddenByExistingContactSettings($ent, $c, $table, $n, $name)) {
          // Also remove the value from the webform submission
          $this->submissionValue($fid, array(NULL));
          continue;
        }
        $field = $this->all_fields[$table . '_' . $name];
        $component = $this->node->webform['components'][$this->enabled[$field_key]];
        // Ignore values from hidden fields
        if ($field['type'] == 'hidden') {
          continue;
        }
        // Translate privacy options into separate values
        if ($name === 'privacy') {
          foreach (array_keys($this->getExposedOptions($field_key)) as $key) {
            $this->data[$ent][$c][$table][$n][$key] = in_array($key, $val);
          }
          continue;
        }
        if (!empty($field['extra']['multiple'])) {
          // Merge with existing data
          if (!empty($this->data[$ent][$c][$table][$n][$name]) && is_array($this->data[$ent][$c][$table][$n][$name])) {
            $val = array_unique(array_merge($val, $this->data[$ent][$c][$table][$n][$name]));
          }
          // Implode data that will be stored as a string
          if ($table !== 'other' && $name !== 'event_id' && $table !== 'contact' && wf_crm_aval($field, 'data_type') != 'ContactReference') {
            $val = $sp . implode($sp, $val) . $sp;
          }
        }
        elseif ($name === 'image_URL') {
          if (empty($val[0]) || !($val = wf_crm_filepath($val[0]))) {
            continue;
          }
        }
        elseif ($field['type'] === 'date') {
          $val = empty($val[0]) ? '' : str_replace('-', '', $val[0]);
          // Add time field value
          $time = wf_crm_aval($this->data, "$ent:$c:$table:$n:$name", '');
          // Remove default date if it has been added
          if (strlen($time) == 14) {
            $time = substr($time, -6);
          }
          $val .= $time;
        }
        // The admin can change a number field to use checkbox/radio/select/grid widget and we'll sum the result
        elseif ($field['type'] === 'number') {
          $sum = 0;
          foreach ((array) $val as $k => $v) {
            // Perform multiplication across grid elements
            if ($component['type'] == 'grid' && is_numeric($k)) {
              $v = $v * $k;
            }
            if (is_numeric($v)) {
              $sum += $v;
            }
          }
          // We don't allow negative payments
          $val = $sum < 0 ? 0 : $sum;
        }
        else {
          $val = isset($val[0]) ? $val[0] : '';
        }
        // Fudge together date and time fields
        if ($field['type'] === 'time' && substr($name, -8) === 'timepart') {
          $name = str_replace('_timepart', '', $name);
          // Add date (default to today)
          $date = wf_crm_aval($this->data, "$ent:$c:$table:$n:$name", date('Ymd'));
          $val = $date . str_replace(':', '', $val);
        }
        // Only known contacts are allowed to empty a field
        if (($val !== '' && $val !== NULL) || !empty($this->existing_contacts[$c])) {
          $this->data[$ent][$c][$table][$n][$name] = $val;
        }
      }
    }
  }

  /**
   * Test whether a field has been hidden due to existing contact settings
   * @param $ent
   * @param $c
   * @param $table
   * @param $n
   * @param $name
   * @return bool
   */
  private function isFieldHiddenByExistingContactSettings($ent, $c, $table, $n, $name) {
    if ($ent == 'contact' && isset($this->enabled["civicrm_{$c}_contact_1_contact_existing"])) {
      $component = $this->node->webform['components'][$this->enabled["civicrm_{$c}_contact_1_contact_existing"]];
      $existing_contact_val = $this->submissionValue($component['cid']);
      // Fields are hidden if value is empty (no selection) or a numeric contact id
      if (!$existing_contact_val[0] || is_numeric($existing_contact_val[0])) {
        $type = ($table == 'contact' && strpos($name, 'name')) ? 'name' : $table;
        if (in_array($type, $component['extra']['hide_fields'])) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  /**
   * Test if any relevant data has been entered for a location
   * @param string $location
   * @param array $params
   * @return bool
   */
  private function locationIsEmpty($location, $params) {
    switch ($location) {
      case 'address':
        return empty($params['street_address'])
          && empty($params['city'])
          && empty($params['state_province_id'])
          && empty($params['country_id'])
          && empty($params['postal_code'])
          && empty($params['master_id']);
      case 'website':
        return empty($params['url']);
      default:
        return empty($params[$location]);
    }
  }

  /**
   * Clears an error against a form element.
   * Used to disable validation when this module hides a field
   * @see https://api.drupal.org/comment/49163#comment-49163
   *
   * @param $name string
   */
  private function unsetError($name) {
    $errors = &drupal_static('form_set_error', array());
    $removed_messages = array();
    if (isset($errors[$name])) {
      $removed_messages[] = $errors[$name];
      unset($errors[$name]);
    }
    $_SESSION['messages']['error'] = array_diff($_SESSION['messages']['error'], $removed_messages);
    if (empty($_SESSION['messages']['error'])) {
      unset ($_SESSION['messages']['error']);
    }
  }

  /**
   * Get or set a value from a webform submission
   * During validation phase we use $this->crmValues
   * During submission processing we use $this->submission
   *
   * @param $fid
   *   Numeric webform component id
   * @param $value
   *   Value to set - leave empty to get a value rather than setting it
   *
   * @return array|null field value if found
   */
  protected function submissionValue($fid, $value = NULL) {
    // In submission processing context
    if ($this->submission) {
      if (!isset($this->submission->data[$fid])) {
        return NULL;
      }
      $field =& $this->submission->data[$fid];
      // During submission preprocessing this is used to alter the submission
      if ($value !== NULL) {
        $field = (array) $value;
      }
      return $field;
    }
    // In validation context - no need to ever change the value so just return it
    else {
      if (!isset($this->rawValues[$fid])) {
        return NULL;
      }
      // rawValues is slightly different from submission->data in that empty values are present in arrays
      if (is_array($this->rawValues[$fid])) {
        $val = array_filter($this->rawValues[$fid]);
        return $val;
      }
      else {
        return (array) $this->rawValues[$fid];
      }
    }
  }

}
