<?php
/**
 * @file
 * commerce_kickstart_product.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_kickstart_product_default_rules_configuration() {
  $items = array();
  $items['commerce_shipping_service_express_shipping'] = entity_import('rules_config', '{ "commerce_shipping_service_express_shipping" : {
      "LABEL" : "Rate Express Shipping",
      "PLUGIN" : "rule",
      "REQUIRES" : [ "commerce_shipping" ],
      "USES VARIABLES" : { "commerce_order" : { "type" : "commerce_order", "label" : "Order" } },
      "DO" : [
        { "commerce_shipping_service_rate_order" : {
            "shipping_service_name" : "express_shipping",
            "commerce_order" : [ "commerce-order" ]
          }
        }
      ]
    }
  }');
  $items['commerce_shipping_service_free_shipping'] = entity_import('rules_config', '{ "commerce_shipping_service_free_shipping" : {
      "LABEL" : "Rate Free Shipping",
      "PLUGIN" : "rule",
      "REQUIRES" : [ "rules", "commerce_shipping" ],
      "USES VARIABLES" : { "commerce_order" : { "label" : "Order", "type" : "commerce_order" } },
      "IF" : [
        { "NOT data_is" : {
            "data" : [ "commerce-order:commerce-order-total:amount" ],
            "op" : "\\u003C",
            "value" : "9999"
          }
        }
      ],
      "DO" : [
        { "commerce_shipping_service_rate_order" : {
            "shipping_service_name" : "free_shipping",
            "commerce_order" : [ "commerce-order" ]
          }
        }
      ]
    }
  }');
  $items['commerce_shipping_service_standard_shipping'] = entity_import('rules_config', '{ "commerce_shipping_service_standard_shipping" : {
      "LABEL" : "Rate Standard Shipping",
      "PLUGIN" : "rule",
      "REQUIRES" : [ "rules", "commerce_shipping" ],
      "USES VARIABLES" : { "commerce_order" : { "type" : "commerce_order", "label" : "Order" } },
      "IF" : [
        { "data_is" : {
            "data" : [ "commerce-order:commerce-order-total:amount" ],
            "op" : "\\u003C",
            "value" : "9999"
          }
        }
      ],
      "DO" : [
        { "commerce_shipping_service_rate_order" : {
            "shipping_service_name" : "standard_shipping",
            "commerce_order" : [ "commerce-order" ]
          }
        }
      ]
    }
  }');
  return $items;
}
