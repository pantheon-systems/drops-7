<?php
/**
 * @file
 * message_notify_example.features.inc
 */

/**
 * Implements hook_views_api().
 */
function message_notify_example_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_default_message_type().
 */
function message_notify_example_default_message_type() {
  $items = array();
  $items['comment_insert'] = entity_import('message_type', '{
    "name" : "comment_insert",
    "description" : "Comment insert",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "[message:field-comment-ref:author] commented on [message:field-comment-ref:node:title]",
          "format" : "filtered_html",
          "safe_value" : "\u003Cp\u003E[message:field-comment-ref:author] commented on [message:field-comment-ref:node:title]\u003C\/p\u003E\n"
        },
        {
          "value" : "Hello [message:field-comment-ref:node:author],\r\n\r\n\u003Ca href=\u0022[message:field-comment-ref:author:url]\u0022\u003E[message:field-comment-ref:author:name]\u003C\/a\u003E has commented on your post \u003Ca href=\u0022[message:field-comment-ref:node:url]\u0022\u003E[message:field-comment-ref:node:title]\u003C\/a\u003E.\r\n\r\nCheers,\r\nMessage notify example robot\r\n",
          "format" : "full_html",
          "safe_value" : "\u003Cp\u003EHello [message:field-comment-ref:node:author],\u003C\/p\u003E\n\u003Cp\u003E\u003Ca href=\u0022[message:field-comment-ref:author:url]\u0022\u003E[message:field-comment-ref:author:name]\u003C\/a\u003E has commented on your post \u003Ca href=\u0022[message:field-comment-ref:node:url]\u0022\u003E[message:field-comment-ref:node:title]\u003C\/a\u003E.\u003C\/p\u003E\n\u003Cp\u003ECheers,\u003Cbr \/\u003E\nMessage notify example robot\u003C\/p\u003E\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  return $items;
}
