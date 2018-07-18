<?php

/**
 * @file
 * Defines server classes and related functions.
 *
 */

/**
 * LDAP Server Class
 *
 *  This class is used to create, work with, and eventually destroy ldap_server
 * objects.
 *
 * @todo make bindpw protected
 */
class LdapQuery {
  // LDAP Settings

  public $query_numeric_id;
  public $qid;
  public $name;
  public $sid;
  public $status;

  public $baseDn = array();
  public $base_dn_str = NULL;
  public $filter;
  public $attributes_str = NULL;
  public $attributes = array();

  public $sizelimit = 0;
  public $timelimit = 0;
  public $deref = LDAP_DEREF_NEVER;
  public $scope = LDAP_SCOPE_SUBTREE;


  public $inDatabase = FALSE;
  public $detailedWatchdogLog = FALSE;


  /**
   * Constructor Method
   */
  function __construct($qid) {
    if (!is_scalar($qid)) {
      return;
    }

    $query_records = array();
    if (module_exists('ctools')) {
      ctools_include('export');
      $result = ctools_export_load_object('ldap_query', 'names', array($qid));
      if (isset($result[$qid])) {
        $query_record = $result[$qid];
        foreach ($query_record as $property_name => $value) {
          $this->{$property_name} = $value;
        }
      }
    }
    else {
      $select = db_select('ldap_query')
        ->fields('ldap_query')
        ->condition('ldap_query.qid',  $qid)
        ->execute();
      foreach ($select as $record) {
        $query_records[$record->qid] = $record;
      }
      if (!isset($query_records[$qid])) {
        $this->inDatabase = FALSE;
        return;
      }
      $query_record = $query_records[$qid];
      foreach ($this->fields() as $field_id => $field ) {
        if (isset($query_record->$field_id)) {
          $this->{$field['property_name']} = @$query_record->$field_id;
        }
      }
    }

    // special properties that don't map directly from storage and defaults
    $this->inDatabase = TRUE;
    $this->detailedWatchdogLog = variable_get('ldap_help_watchdog_detail', 0);

    $this->baseDn = $this->linesToArray($this->base_dn_str);
    $this->attributes = ($this->attributes_str) ? $this->csvToArray($this->attributes_str, TRUE) : array();

  }

  /**
   * Destructor Method
   */
  function __destruct() {

  }


  /**
   * Invoke Method
   */
  function __invoke() {

  }

//  function search($base_dn = NULL, $filter, $attributes = array(), $attrsonly = 0, $sizelimit = 0, $timelimit = 0, $deref = LDAP_DEREF_NEVER) {

  function query() {
    ldap_servers_module_load_include('php', 'ldap_servers', 'LdapServer.class');
    $ldap_server = new LdapServer($this->sid);
    $ldap_server->connect();
    $ldap_server->bind();
    $results = array();

    $count = 0;

    foreach ($this->baseDn as $base_dn) {
      $result = $ldap_server->search($base_dn, $this->filter, $this->attributes, 0, $this->sizelimit, $this->timelimit, $this->deref, $this->scope);
      if ($result !== FALSE && $result['count'] > 0) {
        $count = $count + $result['count'];
        $results = array_merge($results, $result);
      }
    }
    $results['count'] = $count;

    return $results;
  }

  /**
   * Error methods and properties.
   */

  protected $_errorMsg = NULL;
  protected $_hasError = FALSE;
  protected $_errorName = NULL;

  public function setError($_errorName, $_errorMsgText = NULL) {
    $this->_errorMsgText = $_errorMsgText;
    $this->_errorName = $_errorName;
    $this->_hasError = TRUE;
  }

  public function clearError() {
    $this->_hasError = FALSE;
    $this->_errorMsg = NULL;
    $this->_errorName = NULL;
  }

  public function hasError() {
    return ($this->_hasError || $this->ldapErrorNumber());
  }

  public function errorMsg($type = NULL) {
    if ($type == 'ldap' && $this->connection) {
      return ldap_err2str(ldap_errno($this->connection));
    }
    elseif ($type == NULL) {
      return $this->_errorMsg;
    }
    else {
      return NULL;
    }
  }

  public function errorName($type = NULL) {
    if ($type == 'ldap' && $this->connection) {
      return "LDAP Error: " . ldap_error($this->connection);
    }
    elseif ($type == NULL) {
      return $this->_errorName;
    }
    else {
      return NULL;
    }
  }

  public function ldapErrorNumber() {
   // if ($this->connection && ldap_errno($this->connection)) {
    //  return ldap_errno($this->connection);
   // }
   // else {
      return FALSE;
   // }
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

  protected function csvToArray($string, $strip_quotes = FALSE) {
    $items = explode(',', $string);
    foreach ($items as $i => $item) {
      $items[$i] = trim($item);
      if ($strip_quotes) {
        $items[$i] = trim($items[$i], '"');
      }
    }
    return $items;
  }

  public static function fields() {
    $fields = array(
      'query_numeric_id' => array(
          'property_name' => 'query_numeric_id',
          'schema' => array(
            'type' => 'serial',
            'unsigned' => TRUE,
            'not null' => TRUE,
            'description' => 'Primary ID field for the table.  Only used internally.',
            'no export' => TRUE,
          ),
        ),

      'qid' => array(
        'property_name' => 'qid',
        'schema' => array(
          'type' => 'varchar',
          'length' => 20,
          'description' => 'Machine name for query.',
          'not null' => TRUE,
          ),
        'form' => array(
          'field_group' => 'basic',
          '#type' => 'textfield',
          '#title' => t('Machine name for this query configuration.'),
          '#size' => 20,
          '#maxlength' => 20,
          '#description' => t('May only contain alphanumeric characters (a-z, A-Z, 0-9, and _)' ),
          '#required' => TRUE,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'name' => array(
        'property_name' => 'name',
        'schema' => array(
          'type' => 'varchar',
          'length' => '60',
          'not null' => TRUE
        ),
        'form' => array(
          'field_group' => 'basic',
          '#type' => 'textfield',
          '#title' => t('Name'),
          '#description' => t('Choose a name for this query configuration.'),
          '#size' => 50,
          '#maxlength' => 255,
          '#required' => TRUE,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'sid' => array(
        'property_name' => 'sid',
        'schema' => array(
          'type' => 'varchar',
          'length' => 20,
          'not null' => TRUE,
        ),
        'form' => array(
          'field_group' => 'basic',
          '#type' => 'radios',
          '#title' => t('LDAP Server used for query.'),
          '#required' => 1,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'status' => array(
        'property_name' => 'status',
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => TRUE,
          'default' => 0,
        ),
        'form' => array(
          'field_group' => 'basic',
          '#type' => 'checkbox',
          '#title' => t('Enabled'),
          '#description' => t('Disable in order to keep configuration without having it active.'),
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'base_dn_str' => array(
        'property_name' => 'base_dn_str',
        'schema' => array(
          'type' => 'text',
          'not null' => FALSE
        ),
        'form' => array(
          'field_group' => 'query',
          '#type' => 'textarea',
          '#title' => t('Base DNs to search in query.'),
          '#description' => t('Each Base DN will be queried and results merged. e.g. <code>ou=groups,dc=hogwarts,dc=edu</code>') . t('Enter one per line in case if you need more than one.'),
          '#cols' => 50,
          '#rows' => 6,
          '#required' => TRUE,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'baseDn' => array(
        'property_name' => 'baseDn',
        'exportable' => FALSE,
      ),

      'filter' => array(
        'property_name' => 'filter',
        'schema' => array(
          'type' => 'text',
          'not null' => FALSE
        ),
        'form' => array(
          'field_group' => 'query',
          '#type' => 'textarea',
          '#title' => t('Filter'),
          '#description' => t('LDAP query filter such as <code>(objectClass=group)</code> or <code>(&(objectClass=user)(homePhone=*))
</code>'),
          '#cols' => 50,
          '#rows' => 1,
          '#required' => TRUE,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'attributes_str' => array(
        'property_name' => 'attributes_str',
        'schema' => array(
          'type' => 'text',
          'not null' => FALSE
        ),
        'form' => array(
          'field_group' => 'query',
          '#type' => 'textarea',
          '#title' => t('Attributes to return.'),
          '#description' => t('Enter as comma separated list. DN is automatically returned. Leave empty to return all attributes. e.g. <code>objectclass,name,cn,samaccountname</code>'),
          '#cols' => 50,
          '#rows' => 6,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'attributes' => array(
        'property_name' => 'attributes',
        'exportable' => FALSE,
      ),

      'sizelimit' => array(
        'property_name' => 'sizelimit',
        'schema' => array(
          'type' => 'int',
          'size' => 'small',
          'not null' => TRUE,
          'default' => 0,
        ),
        'form' => array(
          'field_group' => 'query_advanced',
          '#type' => 'textfield',
          '#title' => t('Size Limit of returned data'),
          '#description' => t('This limit may be already set by the ldap server.  0 signifies no limit'),
          '#size' => 7,
          '#maxlength' => 5,
          '#required' => TRUE,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'timelimit' => array(
        'property_name' => 'timelimit',
        'schema' => array(
          'type' => 'int',
          'size' => 'small',
          'not null' => TRUE,
          'default' => 0,

        ),
        'form' => array(
          'field_group' => 'query_advanced',
          '#type' => 'textfield',
          '#title' => t('Time Limit in Seconds'),
          '#description' => t('The time limitset on this query.  This may be already set by the ldap server.  0 signifies no limit'),
          '#size' => 7,
          '#maxlength' => 5,
          '#required' => TRUE,
        ),
        'form_to_prop_functions' => array('trim'),
      ),

      'deref' => array(
        'property_name' => 'deref',
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => TRUE,
          'default' => LDAP_DEREF_NEVER,
        ),
        'form' => array(
          'field_group' => 'query_advanced',
          '#type' => 'radios',
          '#title' => t('How aliases should be handled during the search.'),
          '#required' => 1,
          '#options' => array(
            LDAP_DEREF_NEVER => t('(default) aliases are never dereferenced.'),
            LDAP_DEREF_SEARCHING => t('aliases should be dereferenced during the search but not when locating the base object of the search.'),
            LDAP_DEREF_FINDING => t('aliases should be dereferenced when locating the base object but not during the search.'),
            LDAP_DEREF_ALWAYS => t('aliases should be dereferenced always.'),
          ),
        ),
        'form_to_prop_functions' => array('trim'),
      ),
     'scope' => array(
        'property_name' => 'scope',
        'schema' => array(
          'type' => 'int',
          'size' => 'tiny',
          'not null' => TRUE,
          'default' => LDAP_SCOPE_SUBTREE,
        ),
        'form' => array(
          'field_group' => 'query_advanced',
          '#type' => 'radios',
          '#title' => t('Scope of search.'),
          '#required' => 1,
          '#options' => array(
            LDAP_SCOPE_BASE => t('BASE. This value is used to indicate searching only the entry at the base DN, resulting in only that entry being returned (keeping in mind that it also has to meet the search filter criteria!).'),
            LDAP_SCOPE_ONELEVEL => t('ONELEVEL. This value is used to indicate searching all entries one level under the base DN - but not including the base DN and not including any entries under that one level under the base DN.'),
            LDAP_SCOPE_SUBTREE => t('SUBTREE. (default) This value is used to indicate searching of all entries at all levels under and including the specified base DN.'),
          ),
        ),
        'form_to_prop_functions' => array('trim'),
      ),

    );
    return $fields;
  }


}
