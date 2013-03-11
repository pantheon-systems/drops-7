<?php
/*
+--------------------------------------------------------------------+
| CiviCRM version 4.3                                                |
+--------------------------------------------------------------------+
| Copyright CiviCRM LLC (c) 2004-2013                                |
+--------------------------------------------------------------------+
| This file is a part of CiviCRM.                                    |
|                                                                    |
| CiviCRM is free software; you can copy, modify, and distribute it  |
| under the terms of the GNU Affero General Public License           |
| Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
|                                                                    |
| CiviCRM is distributed in the hope that it will be useful, but     |
| WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
| See the GNU Affero General Public License for more details.        |
|                                                                    |
| You should have received a copy of the GNU Affero General Public   |
| License and the CiviCRM Licensing Exception along                  |
| with this program; if not, contact CiviCRM LLC                     |
| at info[AT]civicrm[DOT]org. If you have questions about the        |
| GNU Affero General Public License or the licensing of CiviCRM,     |
| see the CiviCRM license FAQ at http://civicrm.org/licensing        |
+--------------------------------------------------------------------+
*/
/**
 *
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2013
 * $Id$
 *
 */
require_once 'CRM/Core/DAO.php';
require_once 'CRM/Utils/Type.php';
class CRM_Campaign_DAO_Survey extends CRM_Core_DAO
{
  /**
   * static instance to hold the table name
   *
   * @var string
   * @static
   */
  static $_tableName = 'civicrm_survey';
  /**
   * static instance to hold the field values
   *
   * @var array
   * @static
   */
  static $_fields = null;
  /**
   * static instance to hold the FK relationships
   *
   * @var string
   * @static
   */
  static $_links = null;
  /**
   * static instance to hold the values that can
   * be imported
   *
   * @var array
   * @static
   */
  static $_import = null;
  /**
   * static instance to hold the values that can
   * be exported
   *
   * @var array
   * @static
   */
  static $_export = null;
  /**
   * static value to see if we should log any modifications to
   * this table in the civicrm_log table
   *
   * @var boolean
   * @static
   */
  static $_log = false;
  /**
   * Survey id.
   *
   * @var int unsigned
   */
  public $id;
  /**
   * Title of the Survey.
   *
   * @var string
   */
  public $title;
  /**
   * Foreign key to the Campaign.
   *
   * @var int unsigned
   */
  public $campaign_id;
  /**
   * Implicit FK to civicrm_option_value where option_group = activity_type
   *
   * @var int unsigned
   */
  public $activity_type_id;
  /**
   * Recontact intervals for each status.
   *
   * @var text
   */
  public $recontact_interval;
  /**
   * Script instructions for volunteers to use for the survey.
   *
   * @var text
   */
  public $instructions;
  /**
   * Number of days for recurrence of release.
   *
   * @var int unsigned
   */
  public $release_frequency;
  /**
   * Maximum number of contacts to allow for survey.
   *
   * @var int unsigned
   */
  public $max_number_of_contacts;
  /**
   * Default number of contacts to allow for survey.
   *
   * @var int unsigned
   */
  public $default_number_of_contacts;
  /**
   * Is this survey enabled or disabled/cancelled?
   *
   * @var boolean
   */
  public $is_active;
  /**
   * Is this default survey?
   *
   * @var boolean
   */
  public $is_default;
  /**
   * FK to civicrm_contact, who created this Survey.
   *
   * @var int unsigned
   */
  public $created_id;
  /**
   * Date and time that Survey was created.
   *
   * @var datetime
   */
  public $created_date;
  /**
   * FK to civicrm_contact, who recently edited this Survey.
   *
   * @var int unsigned
   */
  public $last_modified_id;
  /**
   * Date and time that Survey was edited last time.
   *
   * @var datetime
   */
  public $last_modified_date;
  /**
   * Used to store option group id.
   *
   * @var int unsigned
   */
  public $result_id;
  /**
   * Bypass the email verification.
   *
   * @var boolean
   */
  public $bypass_confirm;
  /**
   * Title for Thank-you page (header title tag, and display at the top of the page).
   *
   * @var string
   */
  public $thankyou_title;
  /**
   * text and html allowed. displayed above result on success page
   *
   * @var text
   */
  public $thankyou_text;
  /**
   * class constructor
   *
   * @access public
   * @return civicrm_survey
   */
  function __construct()
  {
    $this->__table = 'civicrm_survey';
    parent::__construct();
  }
  /**
   * return foreign links
   *
   * @access public
   * @return array
   */
  function links()
  {
    if (!(self::$_links)) {
      self::$_links = array(
        'campaign_id' => 'civicrm_campaign:id',
        'created_id' => 'civicrm_contact:id',
        'last_modified_id' => 'civicrm_contact:id',
      );
    }
    return self::$_links;
  }
  /**
   * returns all the column names of this table
   *
   * @access public
   * @return array
   */
  static function &fields()
  {
    if (!(self::$_fields)) {
      self::$_fields = array(
        'id' => array(
          'name' => 'id',
          'type' => CRM_Utils_Type::T_INT,
          'required' => true,
        ) ,
        'title' => array(
          'name' => 'title',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Survey Title') ,
          'required' => true,
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
          'import' => true,
          'where' => 'civicrm_survey.title',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'campaign_id' => array(
          'name' => 'campaign_id',
          'type' => CRM_Utils_Type::T_INT,
          'default' => 'UL',
          'FKClassName' => 'CRM_Campaign_DAO_Campaign',
        ) ,
        'activity_type_id' => array(
          'name' => 'activity_type_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Activity Type ID') ,
          'import' => true,
          'where' => 'civicrm_survey.activity_type_id',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
          'default' => 'UL',
        ) ,
        'recontact_interval' => array(
          'name' => 'recontact_interval',
          'type' => CRM_Utils_Type::T_TEXT,
          'title' => ts('Recontact Interval') ,
          'rows' => 20,
          'cols' => 80,
        ) ,
        'instructions' => array(
          'name' => 'instructions',
          'type' => CRM_Utils_Type::T_TEXT,
          'title' => ts('Instructions') ,
          'rows' => 20,
          'cols' => 80,
        ) ,
        'release_frequency' => array(
          'name' => 'release_frequency',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Release Frequency') ,
          'default' => 'UL',
        ) ,
        'max_number_of_contacts' => array(
          'name' => 'max_number_of_contacts',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Maximum number of contacts') ,
          'default' => 'UL',
        ) ,
        'default_number_of_contacts' => array(
          'name' => 'default_number_of_contacts',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Default number of contacts') ,
          'default' => 'UL',
        ) ,
        'is_active' => array(
          'name' => 'is_active',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'default' => '',
        ) ,
        'is_default' => array(
          'name' => 'is_default',
          'type' => CRM_Utils_Type::T_BOOLEAN,
        ) ,
        'created_id' => array(
          'name' => 'created_id',
          'type' => CRM_Utils_Type::T_INT,
          'FKClassName' => 'CRM_Contact_DAO_Contact',
        ) ,
        'created_date' => array(
          'name' => 'created_date',
          'type' => CRM_Utils_Type::T_DATE + CRM_Utils_Type::T_TIME,
          'title' => ts('Campaign Created Date') ,
        ) ,
        'last_modified_id' => array(
          'name' => 'last_modified_id',
          'type' => CRM_Utils_Type::T_INT,
          'FKClassName' => 'CRM_Contact_DAO_Contact',
        ) ,
        'last_modified_date' => array(
          'name' => 'last_modified_date',
          'type' => CRM_Utils_Type::T_DATE + CRM_Utils_Type::T_TIME,
          'title' => ts('Last Modified Date') ,
        ) ,
        'result_id' => array(
          'name' => 'result_id',
          'type' => CRM_Utils_Type::T_INT,
          'default' => 'UL',
        ) ,
        'bypass_confirm' => array(
          'name' => 'bypass_confirm',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Bypass Confirm') ,
        ) ,
        'thankyou_title' => array(
          'name' => 'thankyou_title',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Thank-you Title') ,
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
        ) ,
        'thankyou_text' => array(
          'name' => 'thankyou_text',
          'type' => CRM_Utils_Type::T_TEXT,
          'title' => ts('Thank-you Text') ,
          'rows' => 8,
          'cols' => 60,
        ) ,
      );
    }
    return self::$_fields;
  }
  /**
   * returns the names of this table
   *
   * @access public
   * @static
   * @return string
   */
  static function getTableName()
  {
    return CRM_Core_DAO::getLocaleTableName(self::$_tableName);
  }
  /**
   * returns if this table needs to be logged
   *
   * @access public
   * @return boolean
   */
  function getLog()
  {
    return self::$_log;
  }
  /**
   * returns the list of fields that can be imported
   *
   * @access public
   * return array
   * @static
   */
  static function &import($prefix = false)
  {
    if (!(self::$_import)) {
      self::$_import = array();
      $fields = self::fields();
      foreach($fields as $name => $field) {
        if (CRM_Utils_Array::value('import', $field)) {
          if ($prefix) {
            self::$_import['survey'] = & $fields[$name];
          } else {
            self::$_import[$name] = & $fields[$name];
          }
        }
      }
    }
    return self::$_import;
  }
  /**
   * returns the list of fields that can be exported
   *
   * @access public
   * return array
   * @static
   */
  static function &export($prefix = false)
  {
    if (!(self::$_export)) {
      self::$_export = array();
      $fields = self::fields();
      foreach($fields as $name => $field) {
        if (CRM_Utils_Array::value('export', $field)) {
          if ($prefix) {
            self::$_export['survey'] = & $fields[$name];
          } else {
            self::$_export[$name] = & $fields[$name];
          }
        }
      }
    }
    return self::$_export;
  }
}
