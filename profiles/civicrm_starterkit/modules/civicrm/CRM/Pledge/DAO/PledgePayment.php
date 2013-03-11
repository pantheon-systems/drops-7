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
class CRM_Pledge_DAO_PledgePayment extends CRM_Core_DAO
{
  /**
   * static instance to hold the table name
   *
   * @var string
   * @static
   */
  static $_tableName = 'civicrm_pledge_payment';
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
  static $_log = true;
  /**
   *
   * @var int unsigned
   */
  public $id;
  /**
   * FK to Pledge table
   *
   * @var int unsigned
   */
  public $pledge_id;
  /**
   * FK to contribution table.
   *
   * @var int unsigned
   */
  public $contribution_id;
  /**
   * Pledged amount for this payment (the actual contribution amount might be different).
   *
   * @var float
   */
  public $scheduled_amount;
  /**
   * Actual amount that is paid as the Pledged installment amount.
   *
   * @var float
   */
  public $actual_amount;
  /**
   * 3 character string, value from config setting or input via user.
   *
   * @var string
   */
  public $currency;
  /**
   * The date the pledge payment is supposed to happen.
   *
   * @var datetime
   */
  public $scheduled_date;
  /**
   * The date that the most recent payment reminder was sent.
   *
   * @var datetime
   */
  public $reminder_date;
  /**
   * The number of payment reminders sent.
   *
   * @var int unsigned
   */
  public $reminder_count;
  /**
   *
   * @var int unsigned
   */
  public $status_id;
  /**
   * class constructor
   *
   * @access public
   * @return civicrm_pledge_payment
   */
  function __construct()
  {
    $this->__table = 'civicrm_pledge_payment';
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
        'pledge_id' => 'civicrm_pledge:id',
        'contribution_id' => 'civicrm_contribution:id',
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
        'pledge_payment_id' => array(
          'name' => 'id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Payment ID') ,
          'required' => true,
          'import' => true,
          'where' => 'civicrm_pledge_payment.id',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'pledge_id' => array(
          'name' => 'pledge_id',
          'type' => CRM_Utils_Type::T_INT,
          'required' => true,
          'FKClassName' => 'CRM_Pledge_DAO_Pledge',
        ) ,
        'contribution_id' => array(
          'name' => 'contribution_id',
          'type' => CRM_Utils_Type::T_INT,
          'FKClassName' => 'CRM_Contribute_DAO_Contribution',
        ) ,
        'pledge_payment_scheduled_amount' => array(
          'name' => 'scheduled_amount',
          'type' => CRM_Utils_Type::T_MONEY,
          'title' => ts('Scheduled Amount') ,
          'required' => true,
          'import' => true,
          'where' => 'civicrm_pledge_payment.scheduled_amount',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'pledge_payment_actual_amount' => array(
          'name' => 'actual_amount',
          'type' => CRM_Utils_Type::T_MONEY,
          'title' => ts('Actual Amount') ,
          'import' => true,
          'where' => 'civicrm_pledge_payment.actual_amount',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'currency' => array(
          'name' => 'currency',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Currency') ,
          'maxlength' => 3,
          'size' => CRM_Utils_Type::FOUR,
          'default' => 'UL',
        ) ,
        'pledge_payment_scheduled_date' => array(
          'name' => 'scheduled_date',
          'type' => CRM_Utils_Type::T_DATE + CRM_Utils_Type::T_TIME,
          'title' => ts('Scheduled Date') ,
          'required' => true,
          'import' => true,
          'where' => 'civicrm_pledge_payment.scheduled_date',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'pledge_payment_reminder_date' => array(
          'name' => 'reminder_date',
          'type' => CRM_Utils_Type::T_DATE + CRM_Utils_Type::T_TIME,
          'title' => ts('Last Reminder') ,
          'import' => true,
          'where' => 'civicrm_pledge_payment.reminder_date',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'pledge_payment_reminder_count' => array(
          'name' => 'reminder_count',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Reminders Sent') ,
          'import' => true,
          'where' => 'civicrm_pledge_payment.reminder_count',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => true,
        ) ,
        'pledge_payment_status_id' => array(
          'name' => 'status_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Payment Status') ,
          'import' => true,
          'where' => 'civicrm_pledge_payment.status_id',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => false,
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
    return self::$_tableName;
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
            self::$_import['pledge_payment'] = & $fields[$name];
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
            self::$_export['pledge_payment'] = & $fields[$name];
          } else {
            self::$_export[$name] = & $fields[$name];
          }
        }
      }
    }
    return self::$_export;
  }
}
