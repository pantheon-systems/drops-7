<?php

/**
 * Hook to have modules report how to determine if a bean is being used.
 *
 * Default functionality looks for entity references.
 * Table: name of table to check
 * Column: Column name in table
 * Callback: Custom callback for checking things besides entity references.
 */
function hook_unused_beans($vars) {

  $vars['COMPONENT LABEL']['FIELD'] = array(
    'table' => 'TABLE NAME',
    'column' => 'FIELD/COLUMN NAME',
    'callback' => 'CALLBACK (optional)'
  );
