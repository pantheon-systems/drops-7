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

require_once 'civicrm_rules_utils.inc';

/**
 * Participant role options
 */
function civicrm_rules_get_participant_role_options() {
  return civicrm_rules_get_options('participant_role');
}

function civicrm_rules_condition_participant_role($entityobj, $roleIDs) {
  if (!isset($roleIDs[$entityobj->role_id]) ||
    empty($roleIDs[$entityobj->role_id])
  ) {
    return FALSE;
  }

  return TRUE;
}

/*
 *get node related via cck  - this relies on there being a field called civicrm_event.
 *If you use this field on more than one event type it will load the last one (not something you should do)
 *I wanted to do it using a civicrm_event cck type but got a bit stuck.
 */
function civicrm_rules_events_argument_civicrm_eventnode($participantobj) {
  if (!module_exists('content') || !isset($participantobj['participant'])) {
    return NULL;
  }

  $participant = $participantobj['participant'];
  $eventID = $participant->event_id;
  //todo there must be a clever way to get a node based on the cck field but I don't know it yet
  //I looked into the search_index_links but that seems to rely on touching the node
  // really I am only needing type_name now as I am specifying the field name to be
  // field_civicrm_event_value but leaving this in in case I go back to the cck way
  $sql = "SELECT nf.field_name, i.type_name
FROM {content_node_field} nf
LEFT JOIN {content_node_field_instance} i ON nf.field_name = i.field_name
WHERE nf.field_name LIKE '%civicrm_event'
LIMIT 1";

  $results = db_query($sql);

  while ($result = db_fetch_object($results)) {
    //todo rewrite this with %s & figure out why cck field ends in contact_id
    $sql = "SELECT nid  FROM {content_type_" . $result->type_name . "} WHERE " . $result->field_name . "_value = %d";
    $nodeid = db_result(db_query($sql, $eventID));
    return $nodeid;
  }
  return NULL;
}

/**
 * Handler to get the user. This will work if $entityobj->contact_id is defined
 * e.g. for participant object, contribution object
 */
function civicrm_rules_events_argument_civicrm_contactID_load_user($entityobj) {
  require_once 'api/api.php';
  $contact = civicrm_api('UFMatch', 'get', array('version' => 3, 'contact_id' => $entityobj->contact_id, 'sequential' => 1));
  if (!empty($contact) && is_numeric($contact['values'][0]['uf_id'])) {
    return $contact['values'][0]['uf_id'];
  }
  return NULL;
}

