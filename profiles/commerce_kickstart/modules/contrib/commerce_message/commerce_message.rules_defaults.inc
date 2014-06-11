<?php

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_message_default_rules_configuration() {
  $rules['rules_rules_commerce_order_message_create_order'] = entity_import('rules_config', '{ "rules_rules_commerce_order_message_create_order" : {
      "LABEL" : "Commerce order message: creating order",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "entity" ],
      "ON" : [ "commerce_order_insert" ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_order_created",
              "param_user" : [ "site:current-user" ]
            },
            "PROVIDE" : { "entity_created" : { "message" : "Message" } }
          }
        },
        { "data_set" : {
            "data" : [ "message:message-commerce-order" ],
            "value" : [ "commerce-order" ]
          }
        }
      ]
    }
  }');

  $rules['rules_commerce_order_message_changing_status'] = entity_import('rules_config', '{ "rules_commerce_order_message_changing_status": {
      "LABEL" : "Commerce order message: changing status",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "entity" ],
      "ON" : [ "commerce_order_update" ],
      "IF" : [
        { "NOT data_is" : {
            "data" : [ "commerce-order:status" ],
            "value" : [ "commerce-order-unchanged:status" ]
          }
        }
      ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_order_state",
              "param_user" : [ "site:current-user" ]
            },
            "PROVIDE" : { "entity_created" : { "message" : "Message" } }
          }
        },
        { "data_set" : {
            "data" : [ "message:message-commerce-order" ],
            "value" : [ "commerce-order" ]
          }
        },
        { "data_set" : {
            "data" : [ "message:arguments:order-status" ],
            "value" : "[commerce-order:status-title]"
          }
        },
        { "data_set" : {
            "data" : [ "message:arguments:order-status-previous" ],
            "value" : "[commerce-order-unchanged:status-title]"
          }
        }
      ]
    }
  }');

  // Payment events.
  if (module_exists('commerce_payment')) {
    $rules['rules_commerce_order_message_payment_entered'] = entity_import('rules_config', '{ "rules_commerce_order_message_payment_entered" : {
        "LABEL" : "Commerce order message: payment entered",
        "PLUGIN" : "reaction rule",
        "REQUIRES" : [ "rules", "commerce_payment" ],
        "ON" : [ "commerce_payment_transaction_insert" ],
        "DO" : [
          { "entity_create" : {
              "USING" : {
                "type" : "message",
                "param_type" : "commerce_order_payment_entered",
                "param_user" : [ "site:current-user" ]
              },
              "PROVIDE" : { "entity_created" : { "message" : "Message" } }
            }
          },
          { "data_set" : {
              "data" : [ "message:message-commerce-order" ],
              "value" : [ "commerce-payment-transaction:order" ]
            }
          },
          { "data_set" : {
              "data" : [ "message:message-commerce-payment" ],
              "value" : [ "commerce-payment-transaction" ]
            }
          }
        ]
      }
    }');

    $rules['commerce_order_payment_full'] = entity_import('rules_config', '{ "commerce_order_payment_full" : {
        "LABEL" : "Commerce order message: paid in full",
        "PLUGIN" : "reaction rule",
        "REQUIRES" : [ "rules", "commerce_payment" ],
        "ON" : [ "commerce_payment_order_paid_in_full" ],
        "DO" : [
          { "entity_create" : {
              "USING" : {
                "type" : "message",
                "param_type" : "commerce_order_payment_full",
                "param_user" : [ "site:current-user" ]
              },
              "PROVIDE" : { "entity_created" : { "message" : "Message" } }
            }
          },
          { "data_set" : {
              "data" : [ "message:message-commerce-order" ],
              "value" : [ "commerce-order" ]
            }
          }
        ]
      }
    }');
  }

  // Cart events (add / remove product).
  $rules['rules_commerce_order_message_cart_add'] = entity_import('rules_config', '{ "rules_commerce_order_message_cart_add" : {
      "LABEL" : "Commerce order message: product added",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "commerce_cart" ],
      "ON" : [ "commerce_cart_product_add" ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_order_cart_add",
              "param_user" : [ "site:current-user" ]
            },
            "PROVIDE" : { "entity_created" : { "message" : "Message" } }
          }
        },
        { "data_set" : {
            "data" : [ "message:message-commerce-order" ],
            "value" : [ "commerce-order" ]
          }
        },
        { "data_set" : {
            "data" : [ "message:message-commerce-line-item" ],
            "value" : [ "commerce-line-item" ]
          }
        }
      ]
    }
  }');

  $rules['rules_commerce_order_message_cart_remove'] = entity_import('rules_config', '{ "rules_commerce_order_message_cart_remove" : {
      "LABEL" : "Commerce order message: product removed",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "commerce_cart" ],
      "ON" : [ "commerce_cart_product_remove" ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_order_cart_remove",
              "param_user" : [ "site:current-user" ]
            },
            "PROVIDE" : { "entity_created" : { "message" : "Message" } }
          }
        },
        { "data_set" : {
            "data" : [ "message:message-commerce-order" ],
            "value" : [ "commerce-order" ]
          }
        },
        { "data_set" : {
            "data" : [ "message:message-commerce-line-item" ],
            "value" : [ "commerce-line-item" ]
          }
        }
      ]
    }
  }');

  // Order confirmation.
  $rules['rules_commerce_order_message_order_notification'] = entity_import('rules_config', '{ "rules_commerce_order_message_order_notification" : {
      "LABEL" : "Commerce order message: order notification e-mail",
      "PLUGIN" : "reaction rule",
      "WEIGHT" : "3",
      "REQUIRES" : [ "rules", "message_notify", "commerce_checkout" ],
      "ON" : [ "commerce_checkout_complete" ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "message",
              "param_type" : "commerce_order_order_confirmation",
              "param_user" : [ "commerce-order:owner" ]
            },
            "PROVIDE" : { "entity_created" : { "entity_created" : "Created entity" } }
          }
        },
        { "data_set" : {
            "data" : [ "entity-created:message-commerce-order" ],
            "value" : [ "commerce-order" ]
          }
        },
        { "entity_save" : { "data" : [ "entity-created" ], "immediate" : 1 } },
        { "message_notify_process" : {
            "message" : [ "entity-created" ],
            "save_on_fail" : 0,
            "save_on_success" : 0
          }
        }
      ]
    }
  }');

  return $rules;
}

/**
 * Implements hook_default_rules_configuration_alter().
 *
 * Remove Commerce's default order confirmation rule, as we provide a
 * superior version of it.
 */
function commerce_message_default_rules_configuration_alter(&$configs) {
  unset($configs['commerce_checkout_order_email']);
}
