<?php

/**
 * @file
 *  Migrations for commerce_kickstart_slideshow.
 */

class CommerceKickstartSlideshow extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import slideshow nodes.');
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
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_slideshow') . '/import/slideshow.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationNode('slideshow');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('field_headline', 'headline');
    $this->addFieldMapping('field_tagline', 'tagline');
    $this->addFieldMapping('field_link', 'link');
    // Images
    $this->addFieldMapping('field_image', 'images');
    $this->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_slideshow') . '/import/images');

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);
  }

  function csvcolumns() {
    // "Title", "Headline", "Tagline", "Link", "Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('headline', 'Headline');
    $columns[2] = array('tagline', 'Tagline');
    $columns[3] = array('link', 'Link');
    $columns[4] = array('images', 'Images');
    return $columns;
  }
}
