<?php
/**
 * @file
 * message_og_example.features.inc
 */

/**
 * Implements hook_views_api().
 */
function message_og_example_views_api() {
  list($module, $api) = func_get_args();
  if ($module == "views" && $api == "views_default") {
    return array("version" => "3.0");
  }
}

/**
 * Implements hook_default_message_type().
 */
function message_og_example_default_message_type() {
  $items = array();
  $items['example_og_post_in_group'] = entity_import('message_type', '{
    "name" : "example_og_post_in_group",
    "description" : "Example - OG post in group",
    "argument_keys" : [ "!groups" ],
    "category" : "message_type",
    "language" : "en",
    "arguments" : null,
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "message_text" : { "und" : [
        {
          "value" : "[message:user:picture]",
          "format" : "filtered_html",
          "safe_value" : "\\u003Cp\\u003E[message:user:picture]\\u003C\\/p\\u003E\\n"
        },
        {
          "value" : "\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E[message:user:name]\\u003C\\/a\\u003E posted \\u003Ca href=\\u0022[message:field-node-ref:url]\\u0022\\u003E[message:field-node-ref:title]\\u003C\\/a\\u003E in !groups.",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E[message:user:name]\\u003C\\/a\\u003E posted \\u003Ca href=\\u0022[message:field-node-ref:url]\\u0022\\u003E[message:field-node-ref:title]\\u003C\\/a\\u003E in !groups.\\u003C\\/p\\u003E\\n"
        },
        {
          "value" : "[message:field-node-ref:node-teaser]",
          "format" : "filtered_html",
          "safe_value" : "\\u003Cp\\u003E[message:field-node-ref:node-teaser]\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  $items['example_og_user_membership'] = entity_import('message_type', '{
    "name" : "example_og_user_membership",
    "description" : "Example - OG user membership",
    "argument_keys" : [],
    "category" : "message_type",
    "language" : "en",
    "arguments" : null,
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "message_text" : { "und" : [
        {
          "value" : "[message:user:picture]",
          "format" : "filtered_html",
          "safe_value" : "\\u003Cp\\u003E[message:user:picture]\\u003C\\/p\\u003E\\n"
        },
        {
          "value" : "\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E joined \\u003Ca href=\\u0022[message:field-node-groups-ref:0:url]\\u0022\\u003E[message:field-node-groups-ref:0:title]\\u003C\\/a\\u003E group.",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E joined \\u003Ca href=\\u0022[message:field-node-groups-ref:0:url]\\u0022\\u003E[message:field-node-groups-ref:0:title]\\u003C\\/a\\u003E group.\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  return $items;
}
