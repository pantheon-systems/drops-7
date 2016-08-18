<?php
/**
 * @file
 * message_example.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function message_example_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function message_example_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_default_message_type().
 */
function message_example_default_message_type() {
  $items = array();
  $items['example_arguments'] = entity_import('message_type', '{
    "name" : "example_arguments",
    "description" : "Example - arguments",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "Hi there!\\r\\n\\r\\nThis is an example for arguments in Message. Arguments are great! Why? \\r\\n\\u003Cul\\u003E\\r\\n  \\u003Cli\\u003EEasy to define\\u003C\\/li\\u003E\\r\\n  \\u003Cli\\u003EFlexible\\u003C\\/li\\u003E\\r\\n\\u003C\\/ul\\u003E\\r\\n\\r\\nFor example: Your are @name and the time is %time. And this is a !link to your profile page.\\r\\n\\r\\nCool, no?",
          "format" : "filtered_html",
          "safe_value" : "\\u003Cp\\u003EHi there!\\u003C\\/p\\u003E\\n\\u003Cp\\u003EThis is an example for arguments in Message. Arguments are great! Why? \\u003C\\/p\\u003E\\n\\u003Cul\\u003E\\u003Cli\\u003EEasy to define\\u003C\\/li\\u003E\\n\\u003Cli\\u003EFlexible\\u003C\\/li\\u003E\\n\\u003C\\/ul\\u003E\\u003Cp\\u003EFor example: Your are @name and the time is %time. And this is a !link to your profile page.\\u003C\\/p\\u003E\\n\\u003Cp\\u003ECool, no?\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  $items['example_create_comment'] = entity_import('message_type', '{
    "name" : "example_create_comment",
    "description" : "Example - Create comment",
    "argument_keys" : [ "!teaser" ],
    "argument" : [],
    "category" : "message_type",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "en",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E \\u003Ca href=\\u0022[message:field-comment-ref:url]\\u0022\\u003Ecommented\\u003C\\/a\\u003E on [message:field-node-ref:title]",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E \\u003Ca href=\\u0022[message:field-comment-ref:url]\\u0022\\u003Ecommented\\u003C\\/a\\u003E on [message:field-node-ref:title]\\u003C\\/p\\u003E\\n"
        },
        {
          "value" : "[message:field-comment-ref:comment-teaser]",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E[message:field-comment-ref:comment-teaser]\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  $items['example_create_node'] = entity_import('message_type', '{
    "name" : "example_create_node",
    "description" : "Example - Create node",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "en",
    "arguments" : [],
    "message_text" : { "und" : [
        {
          "value" : "\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E created \\u003Ca href=\\u0022[message:field-node-ref:url]\\u0022\\u003E[message:field-node-ref:title]\\u003C\\/a\\u003E",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E created \\u003Ca href=\\u0022[message:field-node-ref:url]\\u0022\\u003E[message:field-node-ref:title]\\u003C\\/a\\u003E\\u003C\\/p\\u003E\\n"
        },
        {
          "value" : "[message:field-node-ref:node-teaser]",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E[message:field-node-ref:node-teaser]\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  $items['example_user_register'] = entity_import('message_type', '{
    "name" : "example_user_register",
    "description" : "Example - User register",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "en",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E registered.",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E\\u003Ca href=\\u0022[message:user:url]\\u0022\\u003E@{message:user:name}\\u003C\\/a\\u003E registered.\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  return $items;
}
