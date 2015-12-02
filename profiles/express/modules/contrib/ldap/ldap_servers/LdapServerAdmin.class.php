<?php
// $Id: LdapServerAdmin.class.php,v 1.6 2011/01/12 21:51:37 npiacentine Exp $

/**
 * @file
 * LDAP Server Admin Class
 *
 */

module_load_include('php', 'ldap_servers', 'LdapServer.class');

class LdapServerAdmin extends LdapServer {

  public $bindpw_new = FALSE;
  public $bindpw_clear = FALSE;

  /**
   * @param $type = 'all', 'enabled'
   */
  public static function getLdapServerObjects($sid = NULL, $type = NULL, $class = 'LdapServer', $reset = FALSE) {
    $servers = array();
    if (module_exists('ctools')) {
      ctools_include('export');
      if ($reset) {
        ctools_export_load_object_reset('ldap_servers');
      }
      $select = ctools_export_load_object('ldap_servers', 'all');
    }
    else {
      try {
        $select = db_select('ldap_servers', 'ldap_servers')
          ->fields('ldap_servers')
          ->execute();
      }
      catch (Exception $e) {
        drupal_set_message(t('server index query failed. Message = %message, query= %query',
          array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        return array();
      }
    }
    foreach ($select as $result) {
      $servers[$result->sid] = ($class == 'LdapServer') ? new LdapServer($result->sid) : new LdapServerAdmin($result->sid);
    }
    return $servers;

  }

  function __construct($sid) {
    parent::__construct($sid);
  }

  protected function populateFromDrupalForm($op, $values) {
    $this->inDatabase = ($op == 'edit');
    $this->sid = trim($values['sid']);
    $this->name = trim($values['name']);
    $this->status = ($values['status']) ? 1 : 0;
    $this->ldap_type = trim($values['ldap_type']);
    $this->address = trim($values['address']);
    $this->port = trim($values['port']);
    $this->tls = trim($values['tls']);
    $this->bind_method = trim($values['bind_method']);
    $this->binddn = trim($values['binddn']);
    if (trim($values['bindpw'])) {
      $this->bindpw_new = trim($values['bindpw']);
    }
    $this->user_dn_expression = trim($values['user_dn_expression']);
    $this->basedn = $this->linesToArray(trim($values['basedn']));
    $this->user_attr = trim($values['user_attr']);
    $this->account_name_attr = trim($values['account_name_attr']);
    $this->mail_attr = trim($values['mail_attr']);
    $this->mail_template = trim($values['mail_template']);
    $this->unique_persistent_attr = trim($values['unique_persistent_attr']);
    $this->allow_conflicting_drupal_accts = trim($values['allow_conflicting_drupal_accts']);
    $this->ldapToDrupalUserPhp = $values['ldap_to_drupal_user'];
    $this->testingDrupalUsername = trim($values['testing_drupal_username']);
    $this->groupObjectClass = trim($values['group_object_category']);

    $this->searchPagination = ($values['search_pagination']) ? 1 : 0;
    $this->searchPageSize = trim($values['search_page_size']);

  }

  protected function entry() {

    $entry = $this;
    foreach ($this->field_to_properties_map() as $field_name => $property_name) {
      $entry->{$field_name} = $this->{$property_name};
    }
    if (isset($this->bindpw) && $this->bindpw) {
      $entry->bindpw = ldap_servers_encrypt($this->bindpw);
    }
    if ($this->bindpw_new) {
      $entry->bindpw =  ldap_servers_encrypt($this->bindpw_new);
    }
    elseif ($this->bindpw_clear) {
      $entry->bindpw = NULL;
    }
    $entry->tls = (int)$entry->tls;
    return $entry;

  }
  public function save($op) {

    $entry = $this->entry();

    $result = FALSE;
    if ($op == 'edit') {
      if (module_exists('ctools')) {
        ctools_include('export');
        $result = ctools_export_crud_save('ldap_servers', $entry);
         ctools_export_load_object_reset('ldap_servers'); // ctools_export_crud_save doesn't invalidate cache
      }
      else {
        $result = drupal_write_record('ldap_servers', $entry, 'sid');
      }
    }
    else {
      if (module_exists('ctools')) {
        ctools_include('export');
        // Populate our object with ctool's properties
        $object = ctools_export_crud_new('ldap_servers');
        foreach ($object as $property => $value) {
          if (!isset($entry->$property)) {
            $entry->$property = $value;
          }
        }
        $result = ctools_export_crud_save('ldap_servers', $entry);
        ctools_export_load_object_reset('ldap_servers'); // ctools_export_crud_save doesn't invalidate cache
      }
      else {
        $result = drupal_write_record('ldap_servers', $entry);
      }
    }
    if ($result) {
      $this->inDatabase = TRUE;
    }
    else {
      drupal_set_message(t('Failed to write LDAP Server to the database.'));
    }
  }

  public function delete($sid) {
    if ($sid == $this->sid) {
      $result = db_delete('ldap_servers')->condition('sid', $sid)->execute();
      if (module_exists('ctools')) {
        ctools_export_load_object_reset('ldap_servers'); // invalidate cache
      }
      $this->inDatabase = FALSE;
      return $result;
    }
    else {
      return FALSE;
    }
  }

  public function getLdapServerActions() {
    $switch = ($this->status ) ? 'disable' : 'enable';
    $actions = array();
    $actions[] =  l(t('edit'), LDAP_SERVERS_MENU_BASE_PATH . '/servers/edit/' . $this->sid);
    if (property_exists($this, 'type')) {
      if ($this->type == 'Overridden') {
          $actions[] = l(t('revert'), LDAP_SERVERS_MENU_BASE_PATH . '/servers/delete/' . $this->sid);
      }
      if ($this->type == 'Normal') {
          $actions[] = l(t('delete'), LDAP_SERVERS_MENU_BASE_PATH . '/servers/delete/' . $this->sid);
      }
    }
    else {
        $actions[] = l(t('delete'), LDAP_SERVERS_MENU_BASE_PATH . '/servers/delete/' . $this->sid);
    }
    $actions[] = l(t('test'), LDAP_SERVERS_MENU_BASE_PATH . '/servers/test/' . $this->sid);
    $actions[] = l($switch, LDAP_SERVERS_MENU_BASE_PATH . '/servers/' . $switch . '/' . $this->sid);
    return $actions;
  }

  public function drupalForm($op) {

    drupal_add_css(drupal_get_path('module', 'ldap_servers') . '/ldap_servers.admin.css', 'module', 'all');

  //  $form['#validate'] = array('ldap_servers_admin_form_validate');
    $form['#prefix'] = <<<EOF
<p>Setup an LDAP server configuration to be used by other modules such as LDAP Authentication,
LDAP Authorization, etc.</p>
<p>More than one LDAP server configuration can exist for a physical LDAP server.
Multiple configurations for the same physical ldap server are useful in cases such as: (1) different
base dns for authentication and authorization and (2) non anonymous bind users with different privileges
for different purposes.</p>
EOF;

  $form['#prefix'] = t($form['#prefix']);

  $form['server'] = array(
    '#type' => 'fieldset',
    '#title' => t('LDAP Server settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['bind_method'] = array(
    '#type' => 'fieldset',
    '#title' => t('Binding Method.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['binding_service_acct'] = array(
    '#type' => 'fieldset',
    '#title' => t('Service Account Binding Credentials'),
    '#description' => t('<p>Required when "Service Account Bind" selected above. </p>
      <p>Some LDAP configurations (specially common in <strong>Active Directory</strong>
      setups) restrict anonymous searches.</p><p>If your LDAP setup does not allow anonymous searches,
      or these are restricted in such a way that login names for users cannot be retrieved as a result
      of them, you have to specify a service account DN//password pair that will be used for these searches.</p>
      <p>For security reasons, this pair should belong to an LDAP account with stripped down permissions.</p>'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['users'] = array(
    '#type' => 'fieldset',
    '#title' => t('LDAP User to Drupal User Relationship'),
    '#description' => t('How are LDAP user entries found based on Drupal username or email?  And vice-versa?
       Needed for LDAP Authentication and Authorization functionality.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['groups'] = array(
    '#type' => 'fieldset',
    '#title' => t('LDAP Groups'),
    '#description' => t('How are groups defined on your LDAP server?  This varies slightly from one LDAP implementation to another
      such as Active Directory, Novell, OpenLDAP, etc.'),
    '#collapsible' => TRUE,
    '#collapsed' => !module_exists('ldap_authorization'),
  );

  $supports = (ldap_servers_php_supports_pagination()) ? t('support pagination!') : t('NOT support pagination.');
  $form['pagination'] = array(
    '#type' => 'fieldset',
    '#title' => t('LDAP Pagination'),
    '#description' => t('In PHP 5.4, pagination is supported in ldap queries.
      A patch to earlier versions of PHP also supports this.')
      . ' <strong>' . t('This PHP installation appears to ') . $supports . '</strong> '
      . '<p>' . t('The advantage to pagination support is that if an ldap server is setup to return only
      1000 entries at a time,
      you can use page through 1000 records at a time;
      without pagination you would never see more than the first 1000 entries.
      Pagination is most useful when large queries for batch creating or
      synching accounts are used.  If you are not using this server for such
      tasks, its recommended to leave pagination disabled.') . '</p>',
    '#collapsible' => TRUE,
    '#collapsed' => !ldap_servers_php_supports_pagination(),
  );


  $field_to_prop_maps = $this->field_to_properties_map();
  foreach ($this->fields() as $field_id => $field) {
    if (isset($field['form'])) {

      if (!isset($field['form']['required']) && isset($field['schema']['not null']) && $field['form']['#type'] != 'checkbox') {
        $field['form']['#required'] = (boolean)$field['schema']['not null'];
      }
      if (isset($field['schema']['length']) && !isset($field['form']['#maxlength'])) {
        $field['form']['#maxlength'] = $field['schema']['length'];
      }
      if (isset($field_to_prop_maps[$field_id])) {
        $field['form']['#default_value'] = $this->{$field_to_prop_maps[$field_id]};
      }
      $fieldset = @$field['form']['fieldset'];
      if ($fieldset) {
        unset($field['form']['fieldset']);
        $form[$fieldset][$field_id] = $field['form'];
      }
      else {
        $form[$field_id] = $field['form'];
      }
    }
  }

  $form['server']['sid']['#disabled'] = ($op == 'edit');
  $form['server']['tls']['#required'] = FALSE;
  $form['bind_method']['bind_method']['#default_value'] = ($this->bind_method) ? $this->bind_method : LDAP_SERVERS_BIND_METHOD_DEFAULT;
  $form['users']['basedn']['#default_value'] = $this->arrayToLines($this->basedn);

  if ($this->bindpw) {
    $pwd_directions = t('You currently have a password stored in the database.
      Leave password field empty to leave password unchanged.  Enter a new password
      to replace the current password.  Check the checkbox below to simply
      remove it from the database.');
    $pwd_class = 'ldap-pwd-present';
  }
  else {
    $pwd_directions = t('No password is currently stored in the database.
      If you are using a service account, enter one.');
    if ($this->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT) {
      $pwd_class = 'ldap-pwd-abscent';
    }
    else {
      $pwd_class = 'ldap-pwd-not-applicable';
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
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
      if (!$this->sid) {
        $errors['server_id_missing'] = 'Server id missing from delete form.';
      }
      $warnings = module_invoke_all('ldap_server_in_use', $this->sid, $this->name);
      if (count($warnings)) {
        $errors['status'] = join("<br/>", array_values($warnings));
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
      $ldap_servers = $this->getLdapServerObjects(NULL, 'all');
      if (count($ldap_servers)) {
        foreach ($ldap_servers as $sid => $ldap_server) {
          if ($this->name == $ldap_server->name) {
            $errors['name'] = t('An LDAP server configuration with the  name %name already exists.', array('%name' => $this->name));
          }
          elseif ($this->sid == $ldap_server->sid) {
            $errors['sid'] = t('An LDAP server configuration with the  id %sid  already exists.', array('%sid' => $this->sid));
          }
        }
      }
    }



    if ($this->status == 0) { // check that no modules use this server
      $warnings = module_invoke_all('ldap_server_in_use', $this->sid, $this->name);
      if (count($warnings)) {
        $errors['status'] = join("<br/>", array_values($warnings));
      }
    }


    if (!is_numeric($this->port)) {
      $errors['port'] =  t('The TCP/IP port must be an integer.');
    }

    if ($this->bind_method == LDAP_SERVERS_BIND_METHOD_USER && !$this->user_dn_expression) {
      $errors['user_dn_expression'] =  t('When using "Bind with Users Credentials", Expression for user DN is required');
    }

    if ($this->mail_attr && $this->mail_template) {
      $errors['mail_attr'] =  t('Mail attribute or Mail Template may be used.  Not both.');
    }

    if ($this->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT && !$this->binddn) {
      $errors['binddn'] =  t('When using "Bind with Service Account", Bind DN is required.');
    }
    if ($op == 'add') {
      if ($this->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT &&
        (($op == 'add' && !$this->bindpw_new) || ($op != 'add' && !$this->bindpw))
      ) {
        $errors['bindpw'] =  t('When using "Bind with Service Account", Bind password is required.');
      }
    }

    return $errors;
  }

public function drupalFormWarnings($op, $values, $has_errors = NULL)  {
    $errors = array();

    if ($op == 'delete') {
      if (!$this->sid) {
        $errors['server_id_missing'] = t('Server id missing from delete form.');
      }
    }
    else {
      $this->populateFromDrupalForm($op, $values);
      $warnings = $this->warnings($op, $has_errors);
    }
    return $warnings;
  }


protected function warnings($op, $has_errors = NULL) {

    $warnings = array();
    if ($this->ldap_type) {
      $defaults = ldap_servers_ldaps_option_array();
      if (isset($defaults['user']['user_attr']) && ($this->user_attr != $defaults['user']['user_attr'])) {
        $tokens = array('%name' => $defaults['name'], '%default' => $defaults['user']['user_attr'], '%user_attr' => $this->user_attr);
        $warnings['user_attr'] =  t('The standard UserName attribute in %name is %default.  You have %user_attr. This may be correct
          for your particular LDAP.', $tokens);
      }

      if (isset($defaults['user']['mail_attr']) && $this->mail_attr && ($this->mail_attr != $defaults['user']['mail_attr'])) {
        $tokens = array('%name' => $defaults['name'], '%default' => $defaults['user']['mail_attr'], '%mail_attr' => $this->mail_attr);
        $warnings['mail_attr'] =  t('The standard mail attribute in %name is %default.  You have %mail_attr.  This may be correct
          for your particular LDAP.', $tokens);
      }
    }
    if (!$this->status && $has_errors != TRUE) {
      $warnings['status'] =  t('This server configuration is currently disabled.');
    }

    if (!$this->mail_attr && !$this->mail_template) {
      $warnings['mail_attr'] =  t('Mail attribute or Mail Template should be used for most user account functionality.');
    }

   // commented out validation because too many false positives present usability errors.
   // if ($this->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT) { // Only for service account
     // $result = ldap_baddn($this->binddn, t('Service Account DN'));
     // if ($result['boolean'] == FALSE) {
     //   $warnings['binddn'] =  $result['text'];
     // }
   // }

   // foreach ($this->basedn as $basedn) {
    //  $result = ldap_baddn($basedn, t('User Base DN'));
     // if ($result['boolean'] == FALSE) {
     //   $warnings['basedn'] =  $result['text'];
    //  }
   // }

   // $result = ldap_badattr($this->user_attr, t('User attribute'));
   // if ($result['boolean'] == FALSE) {
    //  $warnings['user_attr'] =  $result['text'];
   // }

   // if ($this->mail_attr) {
  //    $result = ldap_badattr($this->mail_attr, t('Mail attribute'));
   //   if ($result['boolean'] == FALSE) {
    //    $warnings['mail_attr'] =  $result['text'];
   //   }
  //  }

   // $result = ldap_badattr($this->unique_persistent_attr, t('Unique Persistent Attribute'));
   // if ($result['boolean'] == FALSE) {
    //  $warnings['unique_persistent_attr'] =  $result['text'];
   // }

    return $warnings;
  }

public function drupalFormSubmit($op, $values) {

  $this->populateFromDrupalForm($op, $values);

  if ($values['clear_bindpw']) {
    $this->bindpw_clear = TRUE;
  }

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

  protected function linesToArray($lines) {
    $lines = trim($lines);

    if ($lines) {
      $array = preg_split('/[\n\r]+/', $lines);
      foreach ($array as $i => $value) {
        $array[$i] = trim($value);
      }
    }
    else {
      $array = array();
    }
    return $array;
  }


   public static function fields() {

     /**
     * consumer_type is tag (unique alphanumeric id) of consuming authorization such as
     *   drupal_roles, og_groups, civicrm_memberships
     */
    $fields = array(

      'sid' => array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'textfield',
          '#size' => 20,
          '#title' => t('Machine name for this server configuration.'),
          '#description' => t('May only contain alphanumeric characters (a-z, A-Z, 0-9, and _)'),
          '#required' => TRUE,
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 20,
          'not null' => TRUE,
        )
      ),

     'numeric_sid' => array(
        'schema' => array(
          'type' => 'serial',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Primary ID field for the table.  Only used internally.',
          'no export' => TRUE,
        ),
      ),

      'name' => array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'textfield',
          '#size' => 50,
          '#title' => 'Name',
          '#description' => t('Choose a <em><strong>unique</strong></em> name for this server configuration.'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),

      'status' => array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'checkbox',
          '#title' => t('Enabled'),
          '#description' => t('Disable in order to keep configuration without having it active.'),
          '#required' => FALSE,
        ),
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => TRUE,
          'default' => 0,
        ),
      ),

      'ldap_type' =>  array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'select',
          '#options' =>  ldap_servers_ldaps_option_array(),
          '#title' => t('LDAP Server Type'),
          '#description' => t('This field is informative.  It\'s purpose is to assist with default values and give validation warnings.'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 20,
          'not null' => FALSE,
        ),
      ),

      'address' => array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'textfield',
          '#title' => t('LDAP server'),
          '#description' => t('The domain name or IP address of your LDAP Server such as "ad.unm.edu". For SSL
        use the form ldaps://DOMAIN such as "ldaps://ad.unm.edu"'),
          '#size' => 50,
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),

      'port' => array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'textfield',
          '#title' => t('LDAP port'),
          '#size' => 5,
          '#description' => t('The TCP/IP port on the above server which accepts LDAP connections. Must be an integer.'),
        ),
        'schema' => array(
          'type' => 'int',
          'not null' => TRUE,
          'default' => 389,
        ),
      ),

      'tls' => array(
        'form' => array(
          'fieldset' => 'server',
          '#type' => 'checkbox',
          '#title' => t('Use Start-TLS'),
          '#description' => t('Secure the connection between the Drupal and the LDAP servers using TLS.<br /><em>Note: To use START-TLS, you must set the LDAP Port to 389.</em>'),
        ),
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => TRUE,
          'default' => 0,
        ),
      ),

      'bind_method' => array(
        'form' => array(
          'fieldset' => 'bind_method',
          '#type' => 'radios',
          '#title' => t('Binding Method for Searches (such as finding user object or their group memberships)'),
          '#options' => array(
            LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT => t('Service Account Bind.  Use credentials in following section to
            bind to ldap.  This option is usually a best practice. Service account is entered in next section.'),

            LDAP_SERVERS_BIND_METHOD_USER => t('Bind with Users Credentials.  Use users\' entered credentials
            to bind to LDAP.  This is only useful for modules that work during user logon such
            as ldap authentication and ldap authorization.  This option is not a best practice in most cases.
            The users dn must be of the form "cn=[username],[base dn]" for this option to work.'),

            LDAP_SERVERS_BIND_METHOD_ANON_USER => t('Anonymous Bind for search, then Bind with Users Credentials.
            Searches for user DN then uses users\' entered credentials to bind to LDAP.  This is only useful for
            modules that work during user logon such as ldap authentication and ldap authorization.
            The users dn must be discovered by an anonymous search for this option to work.'),

            LDAP_SERVERS_BIND_METHOD_ANON => t('Anonymous Bind. Use no credentials to bind to ldap server.
            Will not work on most ldaps.'),
          ),
        ),
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => TRUE,
          'default' => 0,
        ),
      ),

      'binddn' => array(
        'form' => array(
          'fieldset' => 'binding_service_acct',
          '#type' => 'textfield',
          '#title' => t('DN for non-anonymous search'),
          '#size' => 80,
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 511,
        ),
      ),

      'bindpw' => array(
        'form' => array(
          'fieldset' => 'binding_service_acct',
          '#type' => 'password',
          '#title' => t('Password for non-anonymous search'),
          '#size' => 20,
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
        ),
      ),

      'clear_bindpw' => array(
        'form' => array(
          'fieldset' => 'binding_service_acct',
          '#type' => 'checkbox',
          '#title' => t('Clear existing password from database.  Check this when switching away from service account binding.'),
          '#default_value' => 0,
        ),
      ),

      'basedn' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textarea',
          '#cols' => 50,
          '#rows' => 6,
          '#title' => t('Base DNs for LDAP users, groups, and other entries this server configuration.'),
          '#description' => t('What DNs have entries relavant to this configuration?
            e.g. <code>ou=campus accounts,dc=ad,dc=uiuc,dc=edu</code>
            Keep in mind that every additional basedn likely doubles the number of queries.  Place the
            more heavily used one first and consider using one higher base DN rather than 2 or more lower base DNs.
            Enter one per line in case if you need more than one.'),
        ),
        'schema' => array(
          'type' => 'text',
          'serialize' => TRUE,
        ),
      ),

      'user_attr' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('AuthName attribute'),
          '#description' => t('The attribute that holds the users\' login name. (eg. <code>cn</code> for eDir or <code>sAMAccountName</code> for Active Directory).'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),

      'account_name_attr' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('AccountName attribute'),
          '#description' => t('The attribute that holds the unique account name. Defaults to the same as the AuthName attribute.'),
        ),
        'schema' => array(
      	  'type' => 'varchar',
          'length' => 255,
          'not null' => FALSE,
          'default' => '',
        ),
      ),

      'mail_attr' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('Email attribute'),
          '#description' => t('The attribute that holds the users\' email address. (eg. <code>mail</code>). Leave empty if no such attribute exists'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => FALSE,
        ),
      ),

      'mail_template' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('Email template'),
          '#description' => t('If no attribute contains the user\'s email address, but it can be derived from other attributes,
            enter an email "template" here.
            Templates should have the user\'s attribute name in form such as [cn], [uin], etc.
            such as <code>[cn]@mycompany.com</code>.
            See http://drupal.org/node/997082 for additional documentation on ldap tokens.
            '),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => FALSE,
        ),
      ),

      'allow_conflicting_drupal_accts' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'checkbox',
          '#title' => t('Allow account conflicts'),
          '#description' => t('[Not implemented yet.  This is in the user interface, but not the code]. If selected, a user or admin created account could pick the same username as an entry in your LDAP,
            creating future conflicts. This option will affect any ldap modules that create accounts such as ldap authentication
            or ldap provision.'),
        ),
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => FALSE,
          'default' => 0,
        ),
      ),

      'unique_persistent_attr' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('Persistent and Unique User Attribute'),
          '#description' => t('[Not implemented yet.  This is in the user interface, but not the code]. In some LDAPs, a user\'s DN, CN, or mail value may change when a user\'s name changes or for other reasons.
            In order to avoid creation of multiple accounts for that user or other ambiguities,
            enter a unique and persistent ldap attribute for users.
            If no such attribute exists, leave this blank.'
            ),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 64,
          'not null' => FALSE,
        ),
      ),

      'user_dn_expression' => array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 80,
          '#title' => t('Expression for user DN. Required when "Bind with Users Credentials" method selected.'),
          '#description' => t('%username and %basedn are valid tokens in the expression.
            Typically it will be:<br/> <code>cn=%username,%basedn</code>
             which might evaluate to <code>cn=jdoe,ou=campus accounts,dc=ad,dc=mycampus,dc=edu</code>
             Base DNs are entered above.'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => FALSE,
        ),
      ),

      'ldap_to_drupal_user' =>  array(
        'form' => array(
          'fieldset' => 'users',
          '#disabled' => (!module_exists('php')),
          '#type' => 'textarea',
          '#cols' => 25,
          '#rows' => 5,
          '#title' => t('PHP to transform Drupal login username to LDAP UserName attribute.'),
          '#description' => t('This will appear as disabled unless the "PHP filter" core module is enabled. Enter PHP to transform Drupal username to the value of the UserName attribute.
            The code should print the UserName attribute.
            PHP filter module must be enabled for this to work.
            The variable $name is available and is the user\'s login username.
            Careful, bad PHP code here will break your site. If left empty, no name transformation will be done.
            <br/>Example:<br/>Given the user will logon with jdoe@xyz.com and you want the ldap UserName attribute to be
            jdoe.<br/><code>$parts = explode(\'@\', $name); if (count($parts) == 2) {print $parts[0]};</code>'),
          ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 1024,
          'not null' => FALSE,
        ),
      ),

     'testing_drupal_username' =>  array(
        'form' => array(
          'fieldset' => 'users',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('Testing Drupal Username'),
          '#description' => t('This is optional and used for testing this server\'s configuration against an actual username.  The user need not exist in Drupal and testing will not affect the user\'s LDAP or Drupal Account.'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => FALSE,
        ),
      ),


     'group_object_category' =>  array(
        'form' => array(
          'fieldset' => 'groups',
          '#type' => 'textfield',
          '#size' => 30,
          '#title' => t('Name of Group Object Class.'),
          '#description' => t('This is used in ldap modules that use groups such as LDAP Authorization.'),
        ),
        'schema' => array(
          'type' => 'varchar',
          'length' => 64,
          'not null' => FALSE,
        ),
      ),

      'search_pagination' => array(
        'form' => array(
          'fieldset' => 'pagination',
          '#type' => 'checkbox',
          '#title' => t('Use LDAP Pagination.'),
          '#disabled' => !ldap_servers_php_supports_pagination(),
        ),
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => FALSE,
          'default' => 0,
        ),
      ),

     'search_page_size' =>  array(
        'form' => array(
          'fieldset' => 'pagination',
          '#type' => 'textfield',
          '#size' => 10,
          '#disabled' => !ldap_servers_php_supports_pagination(),
          '#title' => t('Pagination size limit.'),
          '#description' => t('This should be equal to or smaller than the max
            number of entries returned at a time by your ldap server.
            1000 is a good guess when unsure. Other modules such as LDAP Query
            or LDAP Feeds will be allowed to set a smaller page size, but not
            a larger one.'),
          '#states' => array(
            'visible' => array(   // action to take.
              ':input[name="search_pagination"]' => array('checked' => TRUE),
            ),
      ),
        ),
        'schema' => array(
          'type' => 'int',
          'size' => 'medium',
          'not null' => FALSE,
          'default' => 1000,
        ),
      ),

      'weight' =>  array(
        'schema' => array(
          'type' => 'int',
          'not null' => TRUE,
          'default' => 0,
        ),
      ),

    );

  return $fields;

  }
}
