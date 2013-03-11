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


/*
 *
 * civicrm_handler_field_file.inc
 * Provides URL to a file
 *
 */
class civicrm_handler_field_file extends views_handler_field {
  function construct() {
    parent::construct();
    if (!civicrm_initialize()) {
      return;
    }

    require_once 'CRM/Core/DAO/EntityFile.php';
    require_once 'api/api.php';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $id = $values->{$this->field_alias};
    if ($file_id = CRM_Core_DAO::getFieldValue('CRM_Core_DAO_EntityFile', $id, 'file_id', 'entity_id')) {
      $params = array(
        'id' => $file_id,
        'file_type_id' => NULL,
        'version' => 3,
      );
      $file = civicrm_api('File', 'get', $params);

      if (!civicrm_error($file)) {
        $filepath = file_directory_path() . '/civicrm/custom/' . $file['values'][$file_id]['uri'];
        // Prevent affecting the original
        $v = clone$values;
        $v->{$this->field_alias} = $filepath;
        return parent::render($v);
      }
      else {
        return NULL;
      }
    }
    else {
      return NULL;
    }
  }
}

