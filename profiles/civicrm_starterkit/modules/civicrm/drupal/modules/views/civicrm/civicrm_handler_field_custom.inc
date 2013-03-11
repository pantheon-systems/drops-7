<?php
/*
 +--------------------------------------------------------------------+
 | CiviCRM version 4.3                                                |
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
 * Field handler to render a custom field with options
 *
 * @ingroup civicrm_field_handlers
 */
class civicrm_handler_field_custom extends views_handler_field {
  function construct() {
    parent::construct();
    civicrm_initialize();
  }

  function render($values) {
    $value = $values->{$this->field_alias};
    if (!is_null($value)) {
      // extract the field id from the name
      if (preg_match('/_(\d+)$/', $this->real_field, $match)) {
        require_once 'CRM/Core/BAO/CustomOption.php';
        return CRM_Core_BAO_CustomOption::getOptionLabel($match[1], $value);
      }
      // could not get custom id, lets just return what we have
      return $value;
    }
    return NULL;
  }
}


