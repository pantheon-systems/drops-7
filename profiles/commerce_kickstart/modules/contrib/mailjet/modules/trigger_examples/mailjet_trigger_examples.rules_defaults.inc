<?php
/**
 * @file
 * Defines default rule examples for MailJet Trigger Examples.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function mailjet_trigger_examples_default_rules_configuration() {
  $rules = array();

  $rules['rules_send_emails_to_customers_who_have_not_purchase_x_months_we'] = entity_import('rules_config', '{ "rules_send_emails_to_customers_who_have_not_purchase_x_months_we" : {
      "LABEL" : "Send emails to customers who have not purchase x months week",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "views_bulk_operations", "message_notify" ],
      "ON" : [ "cron" ],
      "DO" : [
        { "entity_fetch" : {
            "USING" : { "type" : "user", "id" : "1" },
            "PROVIDE" : { "entity_fetched" : { "entity_fetched" : "Fetched entity" } }
          }
        },
        { "views_bulk_operations_action_load_list" : {
            "USING" : {
              "view" : "mailjet_customers_who_have_not_ordered_for_X_weeks_months|default",
              "view_user" : [ "entity-fetched" ]
            },
            "PROVIDE" : { "entity_list" : { "entity_list" : "A list of entities" } }
          }
        },
        { "LOOP" : {
            "USING" : { "list" : [ "entity-list" ] },
            "ITEM" : { "list_item" : "Current list item" },
            "DO" : [
              { "entity_create" : {
                  "USING" : {
                    "type" : "message",
                    "param_type" : "customer_not_ordered_x_months_weeks",
                    "param_user" : [ "entity-fetched" ]
                  },
                  "PROVIDE" : { "entity_created" : { "entity_created" : "Created entity" } }
                }
              },
                { "message_notify_process" : {
                  "message" : [ "entity-created" ],
                  "save_on_fail" : 0,
                  "save_on_success" : 0,
                  "mail" : [ "entity-created:user:mail" ]
                }
              }
            ]
          }
        }
      ]
    }
  }');


  $rules['rules_anniversary_emails'] = entity_import('rules_config', '{ "rules_anniversary_emails" : {
      "LABEL" : "Anniversary emails",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "views_bulk_operations", "message_notify" ],
      "ON" : [ "cron" ],
      "DO" : [
        { "entity_fetch" : {
            "USING" : { "type" : "user", "id" : "1" },
            "PROVIDE" : { "entity_fetched" : { "entity_fetched" : "Fetched entity" } }
          }
        },
        { "views_bulk_operations_action_load_list" : {
            "USING" : {
              "view" : "purchase_anniversary_promo_view|default",
              "view_user" : [ "entity-fetched" ]
            },
            "PROVIDE" : { "entity_list" : { "entity_list" : "A list of entities" } }
          }
        },
        { "LOOP" : {
            "USING" : { "list" : [ "entity-list" ] },
            "ITEM" : { "list_item" : "Current list item" },
            "DO" : [
              { "entity_create" : {
                  "USING" : {
                    "type" : "message",
                    "param_type" : "anniversary_message",
                    "param_user" : [ "entity-fetched" ]
                  },
                  "PROVIDE" : { "entity_created" : { "entity_created" : "Created entity" } }
                }
              },
              { "message_notify_process" : {
                  "message" : [ "entity-created" ],
                  "save_on_fail" : 0,
                  "save_on_success" : 0,
                  "mail" : [ "entity-created:user:mail" ]
                }
              }
            ]
          }
        }
      ]
    }
  }');

  $rules['rule_order_exceeded_30_min'] = entity_import('rules_config', '{ "rules_customers_who_exceeded_their_order_with_30_min" : {
    "LABEL" : "customers_who_exceeded_their_order_with_30_min",
    "PLUGIN" : "reaction rule",
    "REQUIRES" : [ "rules", "views_bulk_operations", "message_notify" ],
    "ON" : [ "cron" ],
    "DO" : [
      { "entity_fetch" : {
          "USING" : { "type" : "user", "id" : "1" },
          "PROVIDE" : { "entity_fetched" : { "entity_fetched" : "Fetched entity" } }
        }
      },
      { "views_bulk_operations_action_load_list" : {
          "USING" : {
            "view" : "last_product_exceeded_30_min|default",
            "view_user" : [ "entity-fetched" ]
          },
          "PROVIDE" : { "entity_list" : { "entity_list" : "A list of entities" } }
        }
      },
      { "LOOP" : {
          "USING" : { "list" : [ "entity-list" ] },
          "ITEM" : { "list_item" : "Current list item" },
          "DO" : [
            { "entity_create" : {
                "USING" : {
                  "type" : "message",
                  "param_type" : "customer_order_exceeded_30_minutes",
                  "param_user" : [ "entity-fetched" ]
                },
                "PROVIDE" : { "entity_created" : { "entity_created" : "Created entity" } }
              }
            },
            { "message_notify_process" : {
                "message" : [ "entity-created" ],
                "save_on_fail" : 0,
                "save_on_success" : 0,
                "mail" : [ "entity-created:user:mail" ]
                }
              }
            ]
          }
        }
      ]
    }
  }');

  return $rules;
}
