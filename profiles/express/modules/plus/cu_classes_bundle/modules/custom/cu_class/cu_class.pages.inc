<?php

/**
 * Class view callback.
 */
function cu_class_view($class) {
  //dsm($class);
  drupal_set_title(entity_label('cu_class', $class));
  return entity_view('cu_class', array(entity_id('cu_class', $class) => $class), 'full');
}
