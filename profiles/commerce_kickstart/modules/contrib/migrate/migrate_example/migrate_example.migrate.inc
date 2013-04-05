<?php

/**
 * @file
 * Because the name of this file is the module name plus '.migrate.inc', when
 * hook_migrate_api is invoked by the Migrate module this file is automatically
 * loaded - thus, you don't need to implement your hook in the .module file.
 */

/*
 * You must implement hook_migrate_api(), setting the API level to 2, if you are
 * implementing any migration classes. As of Migrate 2.5, you should also
 * register your migration and handler classes explicitly here - the former
 * method of letting them get automatically registered on a cache clear will
 * break in certain environments (see http://drupal.org/node/1778952).
 */
function migrate_example_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      'BeerTerm' => array('class_name' => 'BeerTermMigration'),
      'BeerUser' => array('class_name' => 'BeerUserMigration'),
      'BeerNode' => array('class_name' => 'BeerNodeMigration'),
      'BeerComment' => array('class_name' => 'BeerCommentMigration'),
      'WinePrep' => array('class_name' => 'WinePrepMigration'),
      'WineVariety' => array('class_name' => 'WineVarietyMigration'),
      'WineRegion' => array('class_name' => 'WineRegionMigration'),
      'WineBestWith' => array('class_name' => 'WineBestWithMigration'),
      'WineFileCopy' => array('class_name' => 'WineFileCopyMigration'),
      'WineFileBlob' => array('class_name' => 'WineFileBlobMigration'),
      'WineRole' => array('class_name' => 'WineRoleMigration'),
      'WineUser' => array('class_name' => 'WineUserMigration'),
      'WineProducer' => array('class_name' => 'WineProducerMigration'),
      'WineProducerXML' => array('class_name' => 'WineProducerXMLMigration'),
      'WineProducerMultiXML' => array('class_name' => 'WineProducerMultiXMLMigration'),
      'WineProducerXMLPull' => array('class_name' => 'WineProducerXMLPullMigration'),
      'WineWine' => array('class_name' => 'WineWineMigration'),
      'WineComment' => array('class_name' => 'WineCommentMigration'),
      'WineTable' => array('class_name' => 'WineTableMigration'),
      'WineFinish' => array('class_name' => 'WineFinishMigration'),
      'WineUpdates' => array('class_name' => 'WineUpdatesMigration'),
      'WineCommentUpdates' => array('class_name' => 'WineCommentUpdatesMigration'),
      'WineVarietyUpdates' => array('class_name' => 'WineVarietyUpdatesMigration'),
      'WineUserUpdates' => array('class_name' => 'WineUserUpdatesMigration'),
    ),
  );
  return $api;
}
