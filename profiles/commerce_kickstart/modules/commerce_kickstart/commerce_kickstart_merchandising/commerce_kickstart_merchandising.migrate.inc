<?php

/**
 * @file
 *  Migrations for commerce_kickstart_merchandising.
 */

class CommerceKickstartAdPush extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import ad-push nodes.');
    $this->softDependencies = array('CommerceKickstartPages');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'title' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_merchandising') . '/import/ad_push.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationNode('ad_push');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('field_tagline', 'tagline');
    $this->addFieldMapping('field_link', 'link');
    // Images
    $this->addFieldMapping('field_image', 'images');
    $this->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_merchandising') . '/import/images');

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);
  }

  function csvcolumns() {
    // "Title", "Tagline", "Link", "Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('tagline', 'Tagline');
    $columns[2] = array('link', 'Link');
    $columns[3] = array('images', 'Images');
    return $columns;
  }
}
