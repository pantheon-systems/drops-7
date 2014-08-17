<?php

/**
 * @file
 * Support for migrate module
 */

/**
 * Implements hook_migrate_api().
 */
function telephone_migrate_api() {
  return array(
    'api' => 2,
    'field handlers' => array(
      'MigrateTelephoneFieldHandler',
    ),
  );
}

/**
 * Handler for telephone field migrations.
 */
class MigrateTelephoneFieldHandler extends MigrateSimpleFieldHandler {
  public function __construct() {
    parent::__construct();
    $this->registerTypes(array('telephone'));
  }
}
