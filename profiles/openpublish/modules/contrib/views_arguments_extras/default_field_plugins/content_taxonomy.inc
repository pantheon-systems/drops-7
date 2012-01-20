<?php

$plugin = array(
    'title' => 'Taxonomy Term Reference',
    'types' => array('taxonomy_term_reference'),
    'form callback' => 'views_plugin_argument_default_field_content_taxonomy_form',
    'argument callback' => 'views_plugin_argument_default_field_content_taxonomy_argument',

  );

function views_plugin_argument_default_field_content_taxonomy_form() {
  return array(
    'content_taxonomy_format' => array(
      '#title' => 'Format',
      '#type' => 'select',
      '#options' => array('tid' => 'TID', 'name' => 'Term Name'),
    )
  );
}
function views_plugin_argument_default_field_content_taxonomy_argument($field, $options, $node, $handler) {
  $format = isset($options['content_taxonomy_format']) ? $options['content_taxonomy_format'] : 'tid';
//  drupal_set_message(print_r(array($format, $field, $item),true));
  $return = array();
  foreach ($field as $item) {
    //this might beable to go away if in d7 the taxonomy_term part is not needed
    //then just the else if would be need
    if (isset($item['taxonomy_term']->{$format})) {
      $value = $item['taxonomy_term']->{$format};
      $return[] = $value;
    }
    elseif (isset($item[$format])) {
      $value = $item[$format];
      $return[] = $value;
    }
    else {
      $term = taxonomy_term_load($item['tid']);
      $value = $term->{$format};
      $return[] = $value;
    }
  }
  return implode("+", array_values($return));
}
