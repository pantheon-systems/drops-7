<?php

/**
 * Implements hook_variable_info().
 */
function service_links_variable_info($options) {

  $variable['service_links_label_in_node'] = array(
    'title' => t('Print this label', array(), $options),
    'description' => t('Label for service links in nodes', array(), $options),
    'type' => 'string',
  );
  
  return $variable;
} 
