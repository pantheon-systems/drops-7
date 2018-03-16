<?php
/**
 * @file
 * visualization_entity_visualization_contributor_role.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function visualization_entity_visualization_contributor_role_default_rules_configuration() {
  $items = array();
  $items['rules_nucivic_data_portal_administrator_gets_the_visualization_c'] = entity_import('rules_config', '{ "rules_nucivic_data_portal_administrator_gets_the_visualization_c" : {
      "LABEL" : "NUCIVIC DATA - Portal Administrator gets the Visualization Contributor Role",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules" ],
      "ON" : [ "user_insert", "user_update" ],
      "IF" : [
        { "user_has_role" : {
            "account" : [ "account" ],
            "roles" : { "value" : { "27274083" : "27274083" } }
          }
        }
      ],
      "DO" : [
        { "user_add_role" : {
            "account" : [ "account" ],
            "roles" : { "value" : { "27481291" : "27481291" } }
          }
        }
      ]
    }
  }');
  return $items;
}
