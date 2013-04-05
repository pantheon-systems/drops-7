<?php

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_backoffice_order_default_rules_configuration() {
  // Set order status : Canceled
  $rules['rules_commerce_order_status_canceled'] = entity_import('rules_config', '{ "rules_commerce_order_status_canceled" : {
      "LABEL" : "Set order status : Canceled",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "canceled" } }
      ]
    }
  }');

  // Set order status : Cart
  $rules['rules_commerce_order_status_cart'] = entity_import('rules_config', '{ "rules_commerce_order_status_cart" : {
      "LABEL" : "Set order status : Cart",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "cart" } }
      ]
    }
  }');

  // Set order status : Checkout (checkout)
  $rules['rules_order_status_checkout_checkout'] = entity_import('rules_config', '{ "rules_order_status_checkout_checkout" : {
      "LABEL" : "Set order status : Checkout (checkout)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "checkout_checkout" } }
      ]
    }
  }');

  // Set order status : Checkout (shipping)
  $rules['rules_order_status_checkout_shipping'] = entity_import('rules_config', '{ "rules_order_status_checkout_shipping" : {
      "LABEL" : "Set order status : Checkout (shipping)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "checkout_shipping" } }
      ]
    }
  }');

  // Set order status : Checkout (review)
  $rules['rules_order_status_checkout_review'] = entity_import('rules_config', '{ "rules_order_status_checkout_review" : {
      "LABEL" : "Set order status : Checkout (review)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "checkout_review" } }
      ]
    }
  }');

  // Set order status : Checkout (payment)
  $rules['rules_order_status_checkout_payment'] = entity_import('rules_config', '{ "rules_order_status_checkout_payment" : {
      "LABEL" : "Set order status : Checkout (payment)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "checkout_payment" } }
      ]
    }
  }');

  // Set order status : Checkout (complete)
  $rules['rules_order_status_checkout_complete'] = entity_import('rules_config', '{ "rules_order_status_checkout_complete" : {
      "LABEL" : "Set order status : Checkout (complete)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "checkout_complete" } }
      ]
    }
  }');

  // Set order status : Pending (pending)
  $rules['rules_commerce_order_status_pending'] = entity_import('rules_config', '{ "rules_commerce_order_status_pending" : {
      "LABEL" : "Set order status : Pending (pending)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "pending" } }
      ]
    }
  }');

  // Set order status : Pending (processing)
  $rules['rules_commerce_order_status_processing'] = entity_import('rules_config', '{ "rules_commerce_order_status_processing" : {
      "LABEL" : "Set order status : Pending (processing)",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "processing" } }
      ]
    }
  }');

  // Set order status : Completed
  $rules['rules_commerce_order_status_completed'] = entity_import('rules_config', '{ "rules_commerce_order_status_completed" : {
      "LABEL" : "Set order status : Completed",
      "PLUGIN" : "action set",
      "REQUIRES" : [ "commerce_order" ],
      "USES VARIABLES" : { "order" : { "label" : "Order", "type" : "commerce_order" } },
      "ACTION SET" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "completed" } }
      ]
    }
  }');

  return $rules;
}

