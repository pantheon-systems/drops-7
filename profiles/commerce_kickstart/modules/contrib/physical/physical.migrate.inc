<?php

/**
 * Implementation of hook_migrate_api().
 */
function physical_migrate_api() {
  $api = array(
    'api' => 2,
    'field handlers' => array(
      'MigratePhysicalDimensionsFieldHandler',
      'MigratePhysicalWeightFieldHandler',
    ),
  );
  return $api;
}
