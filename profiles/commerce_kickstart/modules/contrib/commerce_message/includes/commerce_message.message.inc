<?php

/**
 * Implements hook_default_message_type_category().
 */
function commerce_message_default_message_type_category() {
  $items = array();
  $items['commerce_order_message'] = entity_import('message_type_category', '{
    "category" : "commerce_order_message",
    "description" : "A message produced during the life-cycle of an order",
    "language" : "",
    "rdf_mapping" : []
  }');
  return $items;
}

/**
 * Implements hook_default_message_type().
 */
function commerce_message_default_message_type() {
  $items['commerce_order_created'] = entity_import('message_type', '{
    "name" : "commerce_order_created",
    "description" : "Commerce Order: created",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Order", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "<p>Order has been created.</p>",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');

  $items['commerce_order_state'] = entity_import('message_type', '{
    "name" : "commerce_order_state",
    "description" : "Commerce Order: state change of the order",
    "argument_keys" : [ "@order_status", "@order_status_previous" ],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Order", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "<p>Status has been set to @order_status (previously: @order_status_previous).</p>",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');

  $items['commerce_order_payment_entered'] = entity_import('message_type', '{
    "name" : "commerce_order_payment_entered",
    "description" : "Commerce Order: payment entered",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Payment", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "<p>Payment of @{message:message-commerce-payment:amount-formatted} submitted via @{message:message-commerce-payment:payment-method-title}.</p>",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');

  $items['commerce_order_payment_full'] = entity_import('message_type', '{
    "name" : "commerce_order_payment_full",
    "description" : "Commerce Order: order paid in full",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Payment", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "<p>Order has been paid in full.</p>",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');

  $items['commerce_order_cart_add'] = entity_import('message_type', '{
    "name" : "commerce_order_cart_add",
    "description" : "Commerce Order: product added to cart",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Cart", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "<p>Product @{message:message-commerce-line-item:commerce-product:sku} added to the cart.</p>",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  $items['commerce_order_cart_remove'] = entity_import('message_type', '{
    "name" : "commerce_order_cart_remove",
    "description" : "Commerce Order: product removed from cart",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Cart", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "<p>Product @{message:message-commerce-line-item:commerce-product:sku} removed from the cart.</p>",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');

  // User and admin comments.
  $items['commerce_order_user_comment'] = entity_import('message_type', '{
    "name" : "commerce_order_user_comment",
    "description" : "Commerce Order: user comment",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "User", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "[message:message-commerce-body:value]",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  $items['commerce_order_admin_comment'] = entity_import('message_type', '{
    "name" : "commerce_order_admin_comment",
    "description" : "Commerce Order: admin comment",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_order_message",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_order_display_name" : { "und" : [ { "value" : "Admin", "format" : null } ] },
    "message_text" : { "und" : [
        {
          "value" : "[message:message-commerce-body:value]",
          "format" : "commerce_order_message"
        }
      ]
    },
    "rdf_mapping" : []
  }');

  $items['commerce_order_order_confirmation'] = entity_import('message_type', '{
    "name" : "commerce_order_order_confirmation",
    "description" : "Commerce Order: order confirmation",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "Order @{message:message-commerce-order:order-number} at [site:name]",
          "format" : null,
          "safe_value" : "Order @{message:message-commerce-order:order-number} at [site:name]"
        },
        {
          "value" : "Thanks for your order @{message:message-commerce-order:order-number} at [site:name].\r\n\r\nIf this is your first order with us, you will receive a separate e-mail with login instructions. You can view your order history with us at any time by logging into our website at:\r\n\r\n[site:login-url]\r\n\r\nYou can find the status of your current order at:\r\n\r\n[message:message-commerce-order:view-url]\r\n\r\nPlease contact us if you have any questions about your order.",
          "format" : "commerce_order_message",
          "safe_value" : "Thanks for your order @{message:message-commerce-order:order-number} at [site:name].\n\nIf this is your first order with us, you will receive a separate e-mail with login instructions. You can view your order history with us at any time by logging into our website at:\n\n[site:login-url]\n\nYou can find the status of your current order at:\n\n[message:message-commerce-order:view-url]\n\nPlease contact us if you have any questions about your order."
        }
      ]
    },
    "rdf_mapping" : []
  }');

  return $items;
}

/**
 * Implements hook_default_message_type_alter().
 * Use the LANGUAGE_NONE values for the other languages in multilingual sites.
 */
function commerce_message_default_message_type_alter(&$items) {
  if (module_exists('locale')) {
    $languages = locale_language_list();
    foreach ($languages as $langcode => $langname) {
      foreach ($items as $message_type => $item) {
        if ($item->module != 'commerce_message') {
          continue;
        }
        if (isset($items[$message_type]->message_text[LANGUAGE_NONE])) {
          $items[$message_type]->message_text[$langcode] = $items[$message_type]->message_text[LANGUAGE_NONE];
        }
        if (isset($items[$message_type]->message_order_display_name[LANGUAGE_NONE])) {
          $items[$message_type]->message_order_display_name[$langcode] = $items[$message_type]->message_order_display_name[LANGUAGE_NONE];
        }
      }
    }
  }
}

/**
 * Refresh the fields attached to the message types we support.
 */
function commerce_message_message_field_refresh() {
  $fields['message_commerce_order']['field'] = array (
    'type' => 'entityreference',
    'module' => 'entityreference',
    'cardinality' => '1',
    'translatable' => FALSE,
    'settings' => array(
      'target_type' => 'commerce_order',
      'handler' => 'base',
      'handler_settings' => array(
        'target_bundles' => array(),
        'sort' => array(
          'type' => 'property',
          'property' => 'order_id',
          'direction' => 'ASC',
        ),
      ),
    ),
    'locked' => TRUE,
  );
  $bundles = array(
    'commerce_order_created',
    'commerce_order_state',
    'commerce_order_payment_entered',
    'commerce_order_payment_full',
    'commerce_order_cart_add',
    'commerce_order_cart_remove',
    'commerce_order_user_comment',
    'commerce_order_admin_comment',
    'commerce_order_order_confirmation',
  );
  $fields['message_commerce_order']['instances'][] = array(
    'entity_type' => 'message',
    'bundles' => $bundles,
    'label' => 'Order',
    'required' => TRUE,
    'widget' => array(
      'type' => 'entityreference_autocomplete',
      'module' => 'entityreference',
      'settings' => array(
        'match_operator' => 'CONTAINS',
        'size' => '60',
        'path' => '',
      ),
    ),
    'settings' => array(),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'entityreference_label',
        'settings' => array(
          'link' => FALSE,
        ),
        'module' => 'entityreference',
        'weight' => 0,
      ),
    ),
  );
  $fields['message_commerce_line_item']['field'] = array (
    'type' => 'entityreference',
    'module' => 'entityreference',
    'cardinality' => '1',
    'translatable' => FALSE,
    'settings' => array(
      'target_type' => 'commerce_line_item',
      'handler' => 'base',
      'handler_settings' => array(
        'target_bundles' => array(),
        'sort' => array(
          'type' => 'property',
          'property' => 'line_item_id',
          'direction' => 'ASC',
        ),
      ),
    ),
    'locked' => TRUE,
  );
  $fields['message_commerce_line_item']['instances'][] = array(
    'entity_type' => 'message',
    'bundles' => array('commerce_order_cart_add', 'commerce_order_cart_remove'),
    'label' => 'Line item',
    'required' => TRUE,
    'widget' => array(
      'type' => 'entityreference_autocomplete',
      'module' => 'entityreference',
      'settings' => array(
        'match_operator' => 'CONTAINS',
        'size' => '60',
        'path' => '',
      ),
    ),
    'settings' => array(),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'entityreference_label',
        'settings' => array(
          'link' => FALSE,
        ),
        'module' => 'entityreference',
        'weight' => 0,
      ),
    ),
  );
  $fields['message_commerce_payment']['field'] = array (
    'type' => 'entityreference',
    'module' => 'entityreference',
    'cardinality' => '1',
    'translatable' => FALSE,
    'settings' => array(
      'target_type' => 'commerce_payment_transaction',
      'handler' => 'base',
      'handler_settings' => array(
        'target_bundles' => array(),
        'sort' => array(
          'type' => 'property',
          'property' => 'transaction_id',
          'direction' => 'ASC',
        ),
      ),
    ),
    'locked' => TRUE,
  );
  $fields['message_commerce_payment']['instances'][] = array(
    'entity_type' => 'message',
    'bundles' => array('commerce_order_payment_entered'),
    'label' => 'Line item',
    'required' => TRUE,
    'widget' => array(
      'type' => 'entityreference_autocomplete',
      'module' => 'entityreference',
      'settings' => array(
        'match_operator' => 'CONTAINS',
        'size' => '60',
        'path' => '',
      ),
    ),
    'settings' => array(),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'entityreference_label',
        'settings' => array(
          'link' => FALSE,
        ),
        'module' => 'entityreference',
        'weight' => 0,
      ),
    ),
  );
  $fields['message_commerce_body']['field'] = array (
    'type' => 'text_long',
    'module' => 'text',
    'cardinality' => '1',
    'translatable' => FALSE,
    'settings' => array(),
    'locked' => TRUE,
  );
  $bundles = array(
    'commerce_order_user_comment',
    'commerce_order_admin_comment',
  );
  $fields['message_commerce_body']['instances'][] = array(
    'entity_type' => 'message',
    'bundles' => $bundles,
    'label' => 'Message',
    'required' => TRUE,
    'widget' => array(
      'type' => 'text_textarea',
      'module' => 'text',
      'settings' => array(
        'rows' => '3',
      ),
    ),
    'settings' => array(
      'text_processing' => TRUE,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'text_default',
        'settings' => array(),
        'module' => 'text',
        'weight' => 1,
      ),
    ),
  );

  drupal_alter('commerce_message_message_fields', $fields);

  // Create the missing fields.
  foreach ($fields as $field_name => $info) {
    $field = $info['field'];
    $field += array(
      'field_name' => $field_name,
    );
    if (!field_info_field($field_name)) {
      field_create_field($field);
    }

    foreach ($info['instances'] as $instance) {
      foreach ($instance['bundles'] as $bundle) {
        $instance['bundle'] = $bundle;
        unset($instance['bundles']);
        $instance['field_name'] = $field_name;
        if (!field_info_instance($instance['entity_type'], $instance['field_name'], $instance['bundle'])) {
          field_create_instance($instance);
        }
      }
    }
  }
}

/**
 * Implements hook_field_access().
 */
function commerce_message_field_access($op, $field, $entity_type, $entity, $account) {
  if ($op == 'edit' && $field['field_name'] == 'message_commerce_order') {
    return FALSE;
  }
}

/**
 * Form callback: create/edit a message.
 */
function commerce_message_message_form($form, &$form_state, $entity, $valid_types = array(), $current_display = 'order_view') {
  $form['#entity'] = $entity;
  $form['content'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new comment'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // Store the current_display in order to return the currend view display.
  $form['#current_display'] = array(
    '#type' => 'value',
    '#value' => $current_display,
  );

  $options = array();
  foreach (message_type_load() as $name => $message_type) {
    if (empty($valid_types) || in_array($name, $valid_types)) {
      $options[$name] = check_plain($message_type->description);
    }
  }

  if (count($options) == 1) {
    $option_names = array_keys($options);
    $form['#entity']->type = $option_names[0];
  }
  else {
    $form['content']['type'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $options,
      '#default_value' => $entity->type,
      '#process' => array('_commerce_message_message_type_select_process', 'ajax_process_form'),
      '#element_validate' => array('_commerce_message_message_type_select_element_validate'),
      '#ajax' => array(
        'callback' => '_commerce_message_message_type_select_ajax_callback',
        'method' => 'replace',
      ),
      '#weight' => -10,
    );
  }

  field_attach_form('message', $form['#entity'], $form['content'], $form_state);

  $form['content']['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $form['content']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => (!empty($form['#submit']) ? $form['#submit'] : array()) + array('commerce_message_message_form_submit'),
    '#ajax' => array(
      'callback' => '_commerce_message_message_submit_ajax_callback',
    ),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'commerce_message_message_form_validate';

  return $form;
}

function _commerce_message_message_type_select_process($element, &$form_state, &$form) {
  $element['#ajax']['wrapper'] = $form['#id'];
  return $element;
}

function _commerce_message_message_type_select_element_validate($element, &$form_state, &$form) {
  if ($element['#value'] != $element['#default_value']) {
    $form['#entity']->type = $element['#value'];
  }
}

function _commerce_message_message_type_select_ajax_callback($form, $form_state) {
  return $form;
}

/**
 * Ajax callback for the save button.
 */
function _commerce_message_message_submit_ajax_callback($form, $form_state) {
  $commands = array();
  $order_id = $form_state['values']['message_commerce_order'][LANGUAGE_NONE][0]['target_id'];
  $view = commerce_embed_view('commerce_message_messages', $form['#current_display']['#value'], array($order_id));
  $commands[] = ajax_command_replace('.view-commerce-message-messages', $view);
  $commands[] = ajax_command_prepend('.view-commerce-message-messages', theme('status_messages'));
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Validation callback for commerce_message_message_form().
 */
function commerce_message_message_form_validate($form, &$form_state) {
  // Notify field widgets to validate their data.
  field_attach_form_validate('message', $form['#entity'], $form['content'], $form_state);
}

/**
 * Submit callback for commerce_message_message_form().
 */
function commerce_message_message_form_submit($form, &$form_state) {
  // Notify field widgets.
  field_attach_submit('message', $form['#entity'], $form['content'], $form_state);
  entity_save('message', $form['#entity']);
  drupal_set_message(t('Message saved.'), 'status', FALSE);
  // Kill the user input so that we can save a new message.
  $form_state['input'] = array();
  $form_state['rebuild'] = TRUE;
}
