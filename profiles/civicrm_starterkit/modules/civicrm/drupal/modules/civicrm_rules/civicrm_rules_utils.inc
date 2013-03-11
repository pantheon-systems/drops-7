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
 | Version 3, 19 November 2007.                                       |
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

/**
 * This file contains functions that calls civicrm apis for data manupulation / retrieval
 */

/**
 * Function to get the list of groups
 */
function _civicrm_get_groups() {
  civicrm_initialize();
  require_once 'api/v2/Group.php';
  $params = array('return.id' => 1, 'return.title' => 1, 'version' => 2);
  $groupInformation = civicrm_group_get($params);

  $groups = array();
  foreach ($groupInformation as $values) {
    $groups[$values['id']] = $values['title'];
  }

  return $groups;
}

/**
 * Function to add contacts to group
 */
function _civicrm_rules_group_contact($contactId, $groupId, $action = 'add') {
  civicrm_initialize();
  require_once 'api/v2/GroupContact.php';
  $params = array(
    'contact_id' => $contactId,
    'group_id' => $groupId,
    'version' => 2,
  );
  // This used to be civicrm_group_contact_common($params, $action);
  civicrm_group_contact_common($params, $action);
}

/**
 * Loads a contact into a version suitable for the rules actions
 */
function _civicrm_rules_load_contact($contactID) {
  require_once 'CRM/Contact/DAO/Contact.php';
  $contact = new CRM_Contact_DAO_Contact();
  $contact->id = $contactID;

  if ($contact->id) {
    // load the contact data here
    $contact->find(TRUE);
  }

  return $contact;
}

function _civicrm_rules_in_group($contact, $group) {
  // check if contact is in the group gid
  return TRUE;
}

/**
 * Loads a mailing into a version suitable for the rules actions
 */
function _civicrm_rules_load_mailing($mailingID) {
  require_once 'CRM/Mailing/DAO/Mailing.php';
  $mailing = new CRM_Mailing_DAO_Mailing();
  $mailing->id = $mailingID;

  if ($mailing->id) {
    // load the mailing data here
    $mailing->find(TRUE);
  }

  return $mailing;
}


/*
 * function to return labels for checkboxes & stuff
 */
function civicrm_rules_get_options($type) {
  if (!civicrm_initialize()) {
    return;
  }

  $result = civicrm_api('OptionValue', 'Get', array('version' => 3, 'option_group_name' => $type));
  foreach ($result['values'] as $options) {
    $values[$options['value']] = $options['label'];
  }
  return $values;
}

/**
 * Loads an entity into a version suitable for the rules actions
 */
function _civicrm_rules_load_fromAPI($entity_id, $entity) {
  require_once 'api/api.php';
  $entity = civicrm_api($entity_id, 'Get', array('version' => 3, 'id' => $entity_id));
  return $entity['values'][$entity['id']];
}

