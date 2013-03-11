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
class CRM_Financial_DAO_FinancialAccount extends CRM_Core_DAO
{
  /**
   * static instance to hold the table name
   *
   * @var string
   * @static
   */
  static $_tableName = 'civicrm_financial_account';
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
   * ID
   *
   * @var int unsigned
   */
  public $id;
  /**
   * Financial Account Name.
   *
   * @var string
   */
  public $name;
  /**
   * FK to Contact ID that is responsible for the funds in this account
   *
   * @var int unsigned
   */
  public $contact_id;
  /**
   * pseudo FK into civicrm_option_value.
   *
   * @var int unsigned
   */
  public $financial_account_type_id;
  /**
   * Optional value for mapping monies owed and received to accounting system codes.
   *
   * @var string
   */
  public $accounting_code;
  /**
   * Optional value for mapping account types to accounting system account categories (QuickBooks Account Type Codes for example).
   *
   * @var string
   */
  public $account_type_code;
  /**
   * Financial Type Description.
   *
   * @var string
   */
  public $description;
  /**
   * Parent ID in account hierarchy
   *
   * @var int unsigned
   */
  public $parent_id;
  /**
   * Is this a header account which does not allow transactions to be posted against it directly, but only to its sub-accounts?
   *
   * @var boolean
   */
  public $is_header_account;
  /**
   * Is this account tax-deductible?
   *
   * @var boolean
   */
  public $is_deductible;
  /**
   * Is this account for taxes?
   *
   * @var boolean
   */
  public $is_tax;
  /**
   * The percentage of the total_amount that is due for this tax.
   *
   * @var float
   */
  public $tax_rate;
  /**
   * Is this a predefined system object?
   *
   * @var boolean
   */
  public $is_reserved;
  /**
   * Is this property active?
   *
   * @var boolean
   */
  public $is_active;
  /**
   * Is this account the default one (or default tax one) for its financial_account_type?
   *
   * @var boolean
   */
  public $is_default;
  /**
   * class constructor
   *
   * @access public
   * @return civicrm_financial_account
   */
  function __construct()
  {
    $this->__table = 'civicrm_financial_account';
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
        'contact_id' => 'civicrm_contact:id',
        'parent_id' => 'civicrm_financial_account:id',
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
        'name' => array(
          'name' => 'name',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Name') ,
          'required' => true,
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
        ) ,
        'financial_account_contact_id' => array(
          'name' => 'contact_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Contact ID') ,
          'FKClassName' => 'CRM_Contact_DAO_Contact',
        ) ,
        'financial_account_type_id' => array(
          'name' => 'financial_account_type_id',
          'type' => CRM_Utils_Type::T_INT,
          'required' => true,
          'default' => '',
        ) ,
        'accounting_code' => array(
          'name' => 'accounting_code',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Accounting Code') ,
          'maxlength' => 64,
          'size' => CRM_Utils_Type::BIG,
          'export' => true,
          'where' => 'civicrm_financial_account.accounting_code',
          'headerPattern' => '',
          'dataPattern' => '',
        ) ,
        'account_type_code' => array(
          'name' => 'account_type_code',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Account Type Code') ,
          'maxlength' => 64,
          'size' => CRM_Utils_Type::BIG,
          'export' => true,
          'where' => 'civicrm_financial_account.account_type_code',
          'headerPattern' => '',
          'dataPattern' => '',
        ) ,
        'description' => array(
          'name' => 'description',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Description') ,
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
        ) ,
        'parent_id' => array(
          'name' => 'parent_id',
          'type' => CRM_Utils_Type::T_INT,
          'FKClassName' => 'CRM_Financial_DAO_FinancialAccount',
        ) ,
        'is_header_account' => array(
          'name' => 'is_header_account',
          'type' => CRM_Utils_Type::T_BOOLEAN,
        ) ,
        'is_deductible' => array(
          'name' => 'is_deductible',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'default' => '',
        ) ,
        'is_tax' => array(
          'name' => 'is_tax',
          'type' => CRM_Utils_Type::T_BOOLEAN,
        ) ,
        'tax_rate' => array(
          'name' => 'tax_rate',
          'type' => CRM_Utils_Type::T_MONEY,
          'title' => ts('Tax Rate') ,
        ) ,
        'is_reserved' => array(
          'name' => 'is_reserved',
          'type' => CRM_Utils_Type::T_BOOLEAN,
        ) ,
        'is_active' => array(
          'name' => 'is_active',
          'type' => CRM_Utils_Type::T_BOOLEAN,
        ) ,
        'is_default' => array(
          'name' => 'is_default',
          'type' => CRM_Utils_Type::T_BOOLEAN,
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
            self::$_import['financial_account'] = & $fields[$name];
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
            self::$_export['financial_account'] = & $fields[$name];
          } else {
            self::$_export[$name] = & $fields[$name];
          }
        }
      }
    }
    return self::$_export;
  }
}
