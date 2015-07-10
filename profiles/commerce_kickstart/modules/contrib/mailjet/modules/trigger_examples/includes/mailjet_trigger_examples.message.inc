<?php
/**
 * @file
 * Create the notification messages.
 */

/**
 * Implements hook_default_message_type_category().
 */
function mailjet_trigger_examples_default_message_type_category() {
  $items = array();
  $items['commerce_user_message'] = entity_import('message_type_category', '{
    "category" : "commerce_user_message",
    "description" : "A notification message",
    "language" : "en",
    "rdf_mapping" : []
  }');
  return $items;
}

/**
 * Implements hook_default_message_type().
 */
function mailjet_trigger_examples_default_message_type() {
  $items['customer_not_ordered_x_months_weeks'] = entity_import('message_type', '{
    "name" : "customer_not_ordered_x_months_weeks",
    "description" : "Send message to customers that have not ordered in X months\/weeks new",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_user_message",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "\u003Cp\u003ENew offers\u003C\/p\u003E\r\n",
          "format" : "full_html",
          "safe_value" : "\u003Cp\u003ENew offers\u003C\/p\u003E\n"
        },
        {
          "value" : "Dear customer,\u003Cbr\u003E\u003Cbr\u003E\u003Cbr\u003E\r\nWe would like to let you know about our new avaiable products. You can visit our web site and chose from various of new products and get your 50% discout. ",
          "format" : "commerce_order_message",
          "safe_value" : "Dear customer,\nWe would like to let you know about our new avaiable products. You can visit our web site and chose from various of new products and get your 50% discout."
        }
      ]
    }
  }');

  $items['anniversary_message'] = entity_import('message_type', '{
    "name" : "anniversary_message",
    "description" : "Send anniversary emails",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_user_message",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "\u003Cp\u003EAnniversary Offer\u003C\/p\u003E\r\n",
          "format" : "full_html",
          "safe_value" : "\u003Cp\u003EAnniversary Offer\u003C\/p\u003E\n"
        },
        {
          "value" : "Dear customer,\u003Cbr\u003E\u003Cbr\u003E\u003Cbr\u003E\r\nSince you were a loyal customer for one whole year we would like to let you know about our new avaiable products. You can visit our web site and chose from various of new products and get your 50% discout. ",
          "format" : "commerce_order_message",
          "safe_value" : "Dear customer,\nSince you were a loyal customer for one whole year we would like to let you know about our new avaiable products. You can visit our web site and chose from various of new products and get your 50% discout."
        }
      ]
    }
  }');

  $items['customer_order_exceeded_30_minutes'] = entity_import('message_type', '{
    "name" : "customer_order_exceeded_30_minutes",
    "description" : "Send emails to customers whose order exceeded 30 minutes.",
    "argument_keys" : [],
    "argument" : [],
    "category" : "commerce_user_message",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "\u003Cp\u003EYour order exceeded 30 minutes\u003C\/p\u003E\r\n",
          "format" : "full_html",
          "safe_value" : "\u003Cp\u003EYour order exceeded 30 minutes\u003C\/p\u003E\n"
        },
        {
          "value" : "Dear customer,\u003Cbr\u003E\u003Cbr\u003E\u003Cbr\u003E\r\nYour order exceeded 30 minutes,  please go back and complete your order",
          "format" : "commerce_order_message",
          "safe_value" : "Dear customer,\nYour order exceeded 30 minutes,  please go back and complete your order"
        }
      ]
    }
  }');

  return $items;
}


/**
 * Implements hook_default_message_type_alter().
 * Use the LANGUAGE_NONE values for the other languages in multilingual sites.
 */
function mailjet_trigger_examples_default_message_type_alter(&$items) {
  if (module_exists('locale')) {
    $languages = locale_language_list();
    foreach ($languages as $langcode => $langname) {
      $items['customer_not_ordered_x_months_weeks']->message_text[$langcode] = $items['customer_not_ordered_x_months_weeks']->message_text[LANGUAGE_NONE];
      $items['customer_not_ordered_x_months_weeks']->message_order_display_name[$langcode] = $items['customer_not_ordered_x_months_weeks']->message_order_display_name[LANGUAGE_NONE];

      $items['anniversary_message']->message_text[$langcode] = $items['anniversary_message']->message_text[LANGUAGE_NONE];
      $items['anniversary_message']->message_order_display_name[$langcode] = $items['anniversary_message']->message_order_display_name[LANGUAGE_NONE];

      $items['customer_order_exceeded_30_minutes']->message_text[$langcode] = $items['customer_order_exceeded_30_minutes']->message_text[LANGUAGE_NONE];
      $items['customer_order_exceeded_30_minutes']->message_order_display_name[$langcode] = $items['customer_order_exceeded_30_minutes']->message_order_display_name[LANGUAGE_NONE];
    }
  }
}

/**
 * Refresh the fields attached to the message types we support.
 */
function mailjet_trigger_examples_message_field_refresh() {
  $fields['message_commerce_order']['field'] = array(
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
    'locked' => FALSE,
  );
  $bundles = array(
    'customer_not_ordered_x_months_weeks',
    'anniversary_message',
    'customer_order_exceeded_30_minutes',
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
  $fields['message_commerce_line_item']['field'] = array(
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
  $fields['message_commerce_payment']['field'] = array(
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
  $fields['message_commerce_body']['field'] = array(
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

  drupal_alter('mailjet_trigger_examples_message_fields', $fields);

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
function mailjet_trigger_examples_field_access($op, $field, $entity_type, $entity, $account) {
  if ($op == 'edit' && $field['field_name'] == 'message_commerce_order') {
    return FALSE;
  }
}

/**
 * Form callback: create/edit a message.
 */
function mailjet_trigger_examples_message_form($form, &$form_state, $entity, $valid_types = array(), $current_display = 'order_view') {
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
      '#process' => array('_mailjet_trigger_examples_message_type_select_process', 'ajax_process_form'),
      '#element_validate' => array('_mailjet_trigger_examples_message_type_select_element_validate'),
      '#ajax' => array(
        'callback' => '_mailjet_trigger_examples_message_type_select_ajax_callback',
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
    '#submit' => (!empty($form['#submit']) ? $form['#submit'] : array()) + array('mailjet_trigger_examples_message_form_submit'),
    '#ajax' => array(
      'callback' => '_mailjet_trigger_examples_message_submit_ajax_callback',
    ),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'mailjet_trigger_examples_message_form_validate';

  return $form;
}

/**
 * Select an element to process.
 */
function _mailjet_trigger_examples_message_type_select_process($element, &$form_state, &$form) {
  $element['#ajax']['wrapper'] = $form['#id'];
  return $element;
}

/**
 * Validate element.
 */
function _mailjet_trigger_examples_message_type_select_element_validate($element, &$form_state, &$form) {
  if ($element['#value'] != $element['#default_value']) {
    $form['#entity']->type = $element['#value'];
  }
}

/**
 * Ajax callback.
 */
function _mailjet_trigger_examples_message_type_select_ajax_callback($form, $form_state) {
  return $form;
}

/**
 * Ajax callback for the save button.
 */
function _mailjet_trigger_examples_message_submit_ajax_callback($form, $form_state) {
  $commands = array();
  $order_id = $form_state['values']['message_commerce_order'][LANGUAGE_NONE][0]['target_id'];
  $view = commerce_embed_view('mailjet_trigger_examples_messages', $form['#current_display']['#value'], array($order_id));
  $commands[] = ajax_command_replace('.view-commerce-message-messages', $view);
  $commands[] = ajax_command_prepend('.view-commerce-message-messages', theme('status_messages'));
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Validation callback for mailjet_trigger_examples_message_form().
 */
function mailjet_trigger_examples_message_form_validate($form, &$form_state) {
  // Notify field widgets to validate their data.
  field_attach_form_validate('message', $form['#entity'], $form['content'], $form_state);
}

/**
 * Submit callback for mailjet_trigger_examples_message_form().
 */
function mailjet_trigger_examples_message_form_submit($form, &$form_state) {
  // Notify field widgets.
  field_attach_submit('message', $form['#entity'], $form['content'], $form_state);
  entity_save('message', $form['#entity']);
  drupal_set_message(t('Message saved.'), 'status', FALSE);
  // Kill the user input so that we can save a new message.
  $form_state['input'] = array();
  $form_state['rebuild'] = TRUE;
}
