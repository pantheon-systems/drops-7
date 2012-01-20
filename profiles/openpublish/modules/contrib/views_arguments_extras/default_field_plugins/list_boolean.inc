<?php

$plugin = array(
    'title' => 'Bool Field',
    'types' => array('list_boolean'),
    'form callback' => 'views_plugin_argument_default_field_list_boolean_form',
    'argument callback' => 'views_plugin_argument_default_field_list_boolean_argument',

  );

function views_plugin_argument_default_field_list_boolean_form() {
  return array(
    'list_boolean_true' => array(
      '#title' => 'True Value',
      '#type' => 'textfield',
    ),
    'list_boolean_false' => array(
      '#title' => 'False Value',
      '#type' => 'textfield',
    ),
    'list_boolean_null' => array(
      '#title' => 'Does not exist Value',
      '#type' => 'textfield',
    ),
  );
}
function views_plugin_argument_default_field_list_boolean_argument($field, $options, $node, $handler) {
  foreach ($field as $item) {
    if (isset($item['value'])) {
      
      $return = $item['value'];
    }
  }
  return $return;
}
