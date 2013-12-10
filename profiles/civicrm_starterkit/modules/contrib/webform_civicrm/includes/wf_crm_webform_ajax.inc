<?php

/**
 * @file
 * Front-end form ajax handler.
 */

module_load_include('inc', 'webform_civicrm', 'includes/wf_crm_webform_base');

class wf_crm_webform_ajax extends wf_crm_webform_base {
  
  function __construct() {
    civicrm_initialize();
  }

  /**
   * Populate a state list based on chosen country
   * @param string $input
   */
  function stateProvince($input) {
    if (!$input || (intval($input) != $input && $input != 'default')) {
      drupal_json_output(array('' => t('- first choose a country -')));
      exit();
    }
    drupal_json_output(wf_crm_get_states($input));
    exit();
  }

  /**
   * Populate a county list based on chosen state
   * @param string $input
   */
  function county($input) {
    if (strpos($input, '-')) {
      list($state, $country) = explode('-', $input);
      $params = array(
        'field' => 'county_id',
        'state_province_id' => wf_crm_state_abbr($state, 'id', $country)
      );
      drupal_json_output(wf_crm_apivalues('address', 'getoptions', $params));
      exit();
    }
  }

  /**
   * Load one or more contacts via ajax
   * @param $key
   */
  function contactAjax($key) {

    if (empty($_GET['str']) && (empty($_GET['load']) || empty($_GET['cid']))) {
      exit();
    }
    list($nid, $fid) = explode('-', $key, 2);
    $this->node = node_load($nid);
    if (!$this->autocompleteAccess($fid)) {
      drupal_access_denied();
    }
    $this->data = $this->node->webform_civicrm['data'];
    $component = $this->node->webform['components'][$fid];
    $filters = wf_crm_search_filters($this->node, $component);
    // Bypass filters when choosing contact on component edit form
    if (!empty($_GET['admin']) && wf_crm_admin_access($this->node)) {
      $filters = array('check_permissions' => 1, 'is_deleted' => 0, 'contact_type' => $filters['contact_type']);
      $component['extra']['allow_create'] = 0;
    }
    // Autocomplete contact names
    if (!empty($_GET['str'])) {
      if ($str = trim($_GET['str'])) {
        drupal_json_output(wf_crm_contact_search($this->node, $component, $filters, $str));
      }
      exit();
    }
    // Load contact by id
    $data = array();
    if ($name = wf_crm_contact_access($component, $filters, $_GET['cid'])) {
      if ($_GET['load'] == 'name') {
        if ($_GET['cid'][0] === '-') {
          // HTML hack to get prompt to show up different than search results
          $data = '<em><i>' . $component['extra']['none_prompt'] . '</i></em>';
        }
        else {
          $data = $name;
        }
      }
      // Fetch entire contact to populate form via ajax
      if ($_GET['load'] == 'full') {
        $sp = CRM_Core_DAO::VALUE_SEPARATOR;
        $this->enabled = wf_crm_enabled_fields($this->node);
        list(, $c, ) = explode('_', $component['form_key'], 3);
        foreach ($_GET as $k => $v) {
          if (substr($k, 0, 3) == 'cid' && $v && is_numeric($v)) {
            $this->id['cid'][substr($k, 3)] = (int) $v;
          }
          $this->id['cid'][$c] = (int) $_GET['cid'];
        }
        $contact = $this->loadContact($c, $component['extra']['hide_fields']);
        // Flatten data into simple keys & values
        foreach ($this->enabled as $fid => $f) {
          list(, $i, $ent, $n, $table, $field) = explode('_', $fid, 6);
          if ($i == $c && $ent == 'contact' && isset($contact[$table][$n][$field])) {
            $type = ($table == 'contact' && strpos($field, 'name')) ? 'name' : $table;
            // Exclude blank and hidden fields to save bandwidth
            if ($contact[$table][$n][$field] !== '' && $contact[$table][$n][$field] !== array() && !in_array($type, $component['extra']['hide_fields'])) {
              $val = $contact[$table][$n][$field];
              // Explode multivalue strings
              if (is_string($val) && strpos($val, $sp) !== FALSE) {
                $val = wf_crm_explode_multivalue_str($val);
              }
              $data[$fid] = $val;
            }
          }
        }
      }
    }
    drupal_json_output($data);
    exit();
  }

  /**
   * Access callback. Check if user has permission to view autocomplete results.
   *
   * @param int $fid
   *   Webform component id
   *
   * @return bool
   */
  private function autocompleteAccess($fid) {
    global $user;
    if (!$fid || empty($this->node->webform_civicrm)) {
      return FALSE;
    }
    if (empty($this->node->webform['components'][$fid]) || !node_access('view', $this->node)) {
      return FALSE;
    }
    if ($user->uid === 1 || webform_results_access($this->node)) {
      return TRUE;
    }
    if (!empty($this->node->webform['components'][$fid]['private'])) {
      return FALSE;
    }
    if (variable_get('webform_submission_access_control', 1)) {
      $allowed_roles = array();
      foreach ($this->node->webform['roles'] as $rid) {
        $allowed_roles[$rid] = isset($user->roles[$rid]) ? TRUE : FALSE;
      }
      if (array_search(TRUE, $allowed_roles) === FALSE) {
        return FALSE;
      }
    }
    return TRUE;
  }

}

/**
 * Drupal page callback to serve AJAX requests.
 *
 * @param $key
 *   Type of AJAX request
 * @param $input
 *   User input
 *
 * Prints json output
 */
function wf_crm_ajax($key, $input = '') {
  $processor = new wf_crm_webform_ajax();
  if ($key == 'stateProvince' || $key == 'county') {
    $processor->$key($input);
  }
  elseif (strpos($key, '-')) {
    $processor->contactAjax($key);
  }
  drupal_access_denied();
}
