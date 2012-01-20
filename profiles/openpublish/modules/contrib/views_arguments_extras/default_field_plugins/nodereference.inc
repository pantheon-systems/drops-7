<?php

$plugin =  array(
    'title' => 'Node Reference',
    'types' => array('node_reference'),
    'form callback' => 'views_plugin_argument_default_field_nodereference_form',
    'argument callback' => 'views_plugin_argument_default_field_nodereference_argument',

  );


function views_plugin_argument_default_field_nodereference_form() {
  return array(
    'argument_default_field_nodereference_format' => array(
      '#title' => 'Format',
      '#type' => 'select',
      '#options' => array('nid' => 'NID', 'title' => 'Title'),
    ),
    'argument_default_field_nodereference_delta' => array(
      '#title' => 'Delta',
      '#type' => 'select',
      '#options' => array('all', 1, 2, 3, 4, 5, 6, 7, 8, 9),
    ),
  );
}
function views_plugin_argument_default_field_nodereference_argument($field, $options) {
  $format = isset($options['argument_default_field_nodereference_format']) ? $options['argument_default_field_nodereference_format'] : 'tid';
  $delta  = isset($options['argument_default_field_nodereference_delta']) ? $options['argument_default_field_nodereference_delta'] : 0;
  $return = array();
  $field = $delta ? array($field[$delta]) : $field;
  foreach ($field as $item) {
    if (isset($item['node']->{$format})) {
      $value = $item['node']->{$format};
      $return[] = $value;
    }
  }
  //return False if we do not have any values
  if (empty($return)) {
    return FALSE;
  }
  return implode('+', $return);
}
