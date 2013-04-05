<?php

/*
 * Implementation of hook_migrate_api().
 */
function migrate_extras_migrate_api() {
  $api = array(
    'api' => 2,
    'destination handlers' => array(
      'MigratePathautoHandler',
      'MigrateExtrasNodeFlagHandler',
      'MigrateExtrasUserFlagHandler',
    ),
    'field handlers' => array(
      'MigrateAddressFieldHandler',
      'MigrateCckPhoneHandler',
      'MigrateGeoFieldHandler',
      'MigrateIntervalFieldHandler',
      'MigrateNameHandler',
      'MigrateMediaFieldHandler',
    )
  );
  return $api;
}
