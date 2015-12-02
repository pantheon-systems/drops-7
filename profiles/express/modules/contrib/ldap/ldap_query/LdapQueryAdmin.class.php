<?php
// $Id: LdapQueryAdmin.class.php,v 1.6 2011/01/12 21:51:37 npiacentine Exp $

/**
 * @file
 * LDAP Query Admin Class
 *
 */



module_load_include('php', 'ldap_query', 'LdapQuery.class');

class LdapQueryAdmin extends LdapQuery {

  /**
   * @param string $sid either 'all' or the ldap server sid
   * @param $type = 'all', 'enabled'
   */
  public static function getLdapQueryObjects($sid = 'all', $type = 'enabled', $class = 'LdapQuery') {
    $queries = array();
    if (module_exists('ctools')) {
      ctools_include('export');
      $select = ctools_export_load_object('ldap_query', 'all');
    }
    else {
      try {
        $select = db_select('ldap_query', 'ldap_query')
          ->fields('ldap_query')
          ->execute();
      }
      catch (Exception $e) {
        drupal_set_message(t('query index query failed. Message = %message, query= %query',
          array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        return array();
      }
    }
    foreach ($select as $result) {
      $query = ($class == 'LdapQuery') ? new LdapQuery($result->qid) : new LdapQueryAdmin($result->qid);
      if (
          ($sid == 'all' || $query->sid == $sid)
          &&
          (!$type || $type == 'all' || ($query->status = 1 && $type == 'enabled'))
        )
      {
        $queries[$result->qid] = $query;
      }
    }
    return $queries;

  }

  function __construct($qid) {
    parent::__construct($qid);
  }

  protected function populateFromDrupalForm($op, $values) {

    foreach ($this->fields() as $field_id => $field) {
      if (isset($field['form']) && property_exists('LdapQueryAdmin', $field['property_name'])) {
        $value = $values[$field_id];
        if (isset($field['form_to_prop_functions'])) {
          foreach ($field['form_to_prop_functions'] as $function) {
            $value = call_user_func($function, $value);
          }
        }
        $this->{$field['property_name']} = $value;
      }
    }
    $this->inDatabase = ($op == 'edit');
  }

  public function save($op) {

    $op = $this->inDatabase ? 'edit' : 'insert';

    if (module_exists('ctools')) { // add or edit with ctolls

      ctools_include('export');
      $ctools_values = clone $this;

      foreach ($this->fields() as $field_id => $field) {
        $value = $this->{$field['property_name']};
        if (isset($field['exportable']) && $field['exportable'] === FALSE) { // field not exportable
          unset($ctools_values->{$field['property_name']});
        }
        elseif (isset($field['schema']) && $field['property_name'] != $field_id) { // field in property with different name
          $ctools_values->{$field_id} = $value;
          unset($ctools_values->{$field['property_name']});
        }
        else {
          // do nothing.  property is already in cloned objecat

        }
      }

      // Populate our object with ctool's properties.  copying all properties for backward compatibility
      $object = ctools_export_crud_new('ldap_query');

      foreach ($object as $property_name => $value) {
        if (!isset($ctools_values->{$property_name})) {
          $ctools_values->$property_name = $value;
        }
      }
      $result = ctools_export_crud_save('ldap_query', $ctools_values);
      ctools_export_load_object_reset('ldap_query'); // ctools_export_crud_save doesn't invalidate cache
    }
    else {
      $values = array();
      foreach ($this->fields() as $field_id => $field) {
        if (isset($field['schema'])) {
          $values[$field_id] = $this->{$field['property_name']};
        }
      }
      if ($op == 'edit') { // edit w/o ctools
        $result = drupal_write_record('ldap_query', $values, 'qid');
      }
      else { // insert
        $result = drupal_write_record('ldap_query', $values);
      }
    }

    if ($result) {
      $this->inDatabase = TRUE;
    }
    else {
      drupal_set_message(t('Failed to write LDAP Query to the database.'));
    }
  }

  public function delete($qid) {
    if ($qid == $this->qid) {
      $this->inDatabase = FALSE;
      return db_delete('ldap_query')->condition('qid', $qid)->execute();
    }
    else {
      return FALSE;
    }
  }

  public function getActions() {
    $switch = ($this->status ) ? 'disable' : 'enable';
    $actions = array();
    $actions[] =  l(t('edit'), LDAP_QUERY_MENU_BASE_PATH . '/query/edit/' . $this->qid);
    if (property_exists($this, 'type')) {
      if ($this->type == 'Overridden') {
          $actions[] = l(t('revert'), LDAP_QUERY_MENU_BASE_PATH . '/query/delete/' . $this->qid);
      }
      if ($this->type == 'Normal') {
          $actions[] = l(t('delete'), LDAP_QUERY_MENU_BASE_PATH . '/query/delete/' . $this->qid);
      }
    }
    else {
        $actions[] = l(t('delete'), LDAP_QUERY_MENU_BASE_PATH . '/query/delete/' . $this->qid);
    }
    $actions[] = l(t('test'), LDAP_QUERY_MENU_BASE_PATH . '/query/test/' . $this->qid);
    $actions[] = l($switch, LDAP_QUERY_MENU_BASE_PATH . '/query/' . $switch . '/' . $this->qid);
    return $actions;
  }

  public function drupalForm($op) {
    $form['#prefix'] = t('<p>Setup an LDAP query to be used by other modules such as LDAP Feeds, LDAP Provision, etc.</p>');

    $form['basic'] = array(
      '#type' => 'fieldset',
      '#title' => t('Basic LDAP Query Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['query'] = array(
      '#type' => 'fieldset',
      '#title' => t('Query'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['query_advanced'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced Query Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );


    foreach ($this->fields() as $field_id => $field) {
      $field_group = isset($field['form']['field_group']) ? $field['form']['field_group'] : FALSE;
      if (isset($field['form'])) {
        $form_item = $field['form'];
        $form_item['#default_value'] = $this->{$field['property_name']};
        if ($field_group) {
          $form[$field_group][$field_id] = $form_item;
          unset($form[$field_group][$field_id]['field_group']); // sirrelevant to form api
        }
        else {
          $form[$field_id] = $form_item;
        }
      }
    }

    $form['basic']['qid']['#disabled'] = ($op == 'edit');

    $servers = ldap_servers_get_servers(NULL, 'enabled');
    if (count($servers) == 0) {
      drupal_set_message(t('No ldap servers configured.  Please configure a server before an ldap query.'), 'error');
    }
    foreach ($servers as $sid => $server) {
      $server_options[$sid] = $server->name;
    }

    $form['basic']['sid']['#options'] = $server_options;

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Query'),
    );

    $action = ($op == 'add') ? 'Add' : 'Update';
      $form['submit'] = array(
      '#type' => 'submit',
      '#value' => $action,
      '#weight' => 100,
    );

    return $form;
  }


  public function drupalFormValidate($op, $values)  {
    $errors = array();

    if ($op == 'delete') {
      if (!$this->qid) {
        $errors['query_name_missing'] = 'Query name missing from delete form.';
      }
    }
    else {
      $this->populateFromDrupalForm($op, $values);
      $errors = $this->validate($op);
    }
    return $errors;
  }

  protected function validate($op) {
    $errors = array();
    if ($op == 'add') {
      $ldap_queries = $this->getLdapQueryObjects('all', 'all');
      if (count($ldap_queries)) {
        foreach ($ldap_queries as $qid => $ldap_query) {
          if ($this->qid == $ldap_query->qid) {
            $errors['qid'] = t('An LDAP Query with the name %qid already exists.', array('%qid' => $this->qid));
          }
        }
      }
    }

    return $errors;
  }

  public function drupalFormSubmit($op, $values) {

    $this->populateFromDrupalForm($op, $values);

    if ($op == 'delete') {
      $this->delete($this);
    }
    else { // add or edit
      try {
        $save_result = $this->save($op);
      }
      catch (Exception $e) {
        $this->setError('Save Error',
          t('Failed to save object.  Your form data was not saved.'));
      }
    }
  }

  protected function arrayToLines($array) {
    $lines = "";
    if (is_array($array)) {
      $lines = join("\n", $array);
    }
    elseif (is_array(@unserialize($array))) {
      $lines = join("\n", unserialize($array));
    }
    return $lines;
  }



  protected function arrayToCsv($array) {
    return join(",",$array);
  }

}
