<?php
/**
 * @file
 * Provides default rules needed by the module
 */

/**
 * Implementation of hook_default_rules_configuration().
 */
function commerce_node_checkout_default_rules_configuration() {
  $items = array();
  $items['commerce_node_checkout_complete_order'] = entity_import('rules_config', '{ "commerce_node_checkout_complete_order" : {
      "LABEL" : "Complete an order with only nodes",
      "PLUGIN" : "reaction rule",
      "WEIGHT" : "10",
      "OWNER" : "rules",
      "TAGS" : [ "Commerce Node Checkout" ],
      "REQUIRES" : [ "commerce_node_checkout", "commerce_order", "commerce_checkout" ],
      "ON" : { "commerce_checkout_complete" : [] },
      "IF" : [
        { "commerce_node_checkout_rules_order_has_only_nodes" : { "commerce_order" : [ "commerce_order" ] } }
      ],
      "DO" : [
        { "commerce_order_update_state" : { "commerce_order" : [ "commerce_order" ], "order_state" : "completed" } }
      ]
    }
  }');
  $items['commerce_node_checkout_publish_node'] = entity_import('rules_config', '{ "commerce_node_checkout_publish_node" : {
      "LABEL" : "Publish the nodes that were purchased",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce Node Checkout" ],
      "WEIGHT" : "10",
      "REQUIRES" : [ "commerce_node_checkout", "commerce_checkout" ],
      "ON" : [ "commerce_checkout_complete" ],
      "DO" : [
        { "commerce_node_checkout_rules_nodes_from_order" : {
            "USING" : { "order" : [ "commerce-order" ] },
            "PROVIDE" : {
              "line_items" : { "line_items" : "Associated Line items" },
              "nodes" : { "nodes" : "Associated Nodes" }
            }
          }
        },
        { "LOOP" : {
            "USING" : { "list" : [ "nodes" ] },
            "ITEM" : { "node" : "Nodes" },
            "DO" : [
              { "data_set" : { "data" : [ "node:author" ], "value" : [ "commerce-order:owner" ] } },
              { "node_publish" : { "node" : [ "node" ] } }
            ]
          }
        }
      ]
    }
  }');
  return $items;
}
