<?php
/**
 * @file
 *   Provides default rules needed by the module
 */

/**
 * Implementation of hook_default_rules_configuration().
 */
function commerce_node_checkout_expire_default_rules_configuration() {
  $items = array();
  $items['commerce_node_checkout_unpublish_node'] = entity_import('rules_config', '{ "commerce_node_checkout_unpublish_node" : {
      "LABEL" : "Unpublish the purchased nodes that have expired",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Commerce Node Checkout Expire" ],
      "REQUIRES" : [ "rules", "commerce_node_checkout_expire" ],
      "ON" : { "cron" : [] },
      "DO" : [
        { "commerce_node_checkout_expire_rules_load_expiring_nodes" : {
            "USING" : { "timestamp" : "now" },
            "PROVIDE" : { "nodes" : { "nodes" : "Unpublished Nodes" } }
          }
        },
        { "LOOP" : {
            "USING" : { "list" : [ "nodes" ] },
            "ITEM" : { "node" : "Expired node" },
            "DO" : [ { "node_unpublish" : { "node" : [ "node" ] } } ]
          }
        }
      ]
    }
  }');
  $items['commerce_node_checkout_reminder'] = entity_import('rules_config', '{ "commerce_node_checkout_reminder" : {
      "LABEL" : "Send a reminder about expiring nodes",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Commerce Node Checkout Expire" ],
      "REQUIRES" : [ "rules", "commerce_node_checkout_expire" ],
      "ON" : { "cron" : [] },
      "DO" : [
        { "commerce_node_checkout_expire_rules_load_expiring_nodes" : {
            "USING" : { "timestamp" : "+7 days" },
            "PROVIDE" : { "nodes" : { "nodes" : "Expiring nodes" } }
          }
        },
        { "commerce_node_checkout_expire_rules_filter_notification_nodes" : {
            "USING" : { "nodes" : [ "nodes" ] },
            "PROVIDE" : { "nodes" : { "notification_nodes" : "Nodes for notifications" } }
          }
        },
        { "LOOP" : {
            "USING" : { "list" : [ "notification-nodes" ] },
            "ITEM" : { "node" : "Expiring node" },
            "DO" : [
              { "mail" : {
                  "to" : [ "node:author:mail" ],
                  "subject" : "Your post is expiring in 7 days",
                  "message" : "Hi [node:author:name],\r\n\r\nThe post [node:title] you bought is expiring in 7 days ([node:url]).\r\n\r\nKind regards",
                  "language" : [ "" ]
                }
              },
              { "commerce_node_checkout_expire_rules_node_set_notification" : { "node" : [ "node" ] } }
            ]
          }
        }
      ]
    }
  }');
  $items['commerce_node_checkout_reset_reminder'] = entity_import('rules_config', '{ "commerce_node_checkout_reset_reminder" : {
      "LABEL" : "Reset the reminder status on purchased nodes",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Commerce Node Checkout Expire" ],
      "REQUIRES" : [
        "commerce_node_checkout",
        "commerce_node_checkout_expire",
        "commerce_checkout"
      ],
      "ON" : { "commerce_checkout_complete" : [] },
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
            "ITEM" : { "node" : "Node" },
            "DO" : [
              { "commerce_node_checkout_expire_rules_node_unset_notification" : { "node" : [ "node" ] } }
            ]
          }
        }
      ]
    }
  }');
  $items['commerce_node_checkout_set_node_expiration_date'] = entity_import('rules_config', '{ "commerce_node_checkout_set_node_expiration_date" : {
      "LABEL" : "Set the purchased node expiration date",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Commerce Node Checkout Expire" ],
      "REQUIRES" : [
        "commerce_node_checkout",
        "commerce_node_checkout_expire",
        "commerce_checkout"
      ],
      "ON" : { "commerce_checkout_complete" : [] },
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
            "USING" : { "list" : [ "line-items" ] },
            "ITEM" : { "line_item" : "Current list item" },
            "DO" : [
              { "commerce_node_checkout_expire_rules_set_line_item_expiration" : { "line_item" : [ "line_item" ] } }
            ]
          }
        }
      ]
    }
  }');
  return $items;
}
