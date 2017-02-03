<?php

/**
 * @file
 * Insert support for File module.
 */

/**
 * Implementation of hook_insert_widgets().
 */
function file_insert_widgets() {
  return array(
    'file_generic' => array(
      'element_type' => 'managed_file',
      'wrapper' => '.file-widget',
      'fields' => array(
        'description' => 'input[name$="[description]"]',
      ),
    ),
  );
}
