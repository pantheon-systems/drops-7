<?php

/**
 * @file
 *  Migrations for commerce_kickstart_blog.
 */

class CommerceKickstartBlogPost extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import blog-post nodes.');
    $this->dependencies = array('CommerceKickstartPages');

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
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_blog') . '/import/blog_posts.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationNode('blog_post', array('text_format' => 'full_html'));

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('body:summary', 'excerpt');
    $this->addFieldMapping('field_tags', 'tags')->separator(', ');
    $this->addFieldMapping('field_tags:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_blog_category', 'category')->separator(', ');
    $this->addFieldMapping('field_blog_category:create_term')
      ->defaultValue(TRUE);
    // Images
    $this->addFieldMapping('field_image', 'images');
    $this->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_blog') . '/import/images');

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);
  }

  function csvcolumns() {
    // "Title", "Images", "Tags", "Body"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('images', 'Images');
    $columns[2] = array('tags', 'Tags');
    $columns[3] = array('category', 'Category');
    $columns[4] = array('body', 'Body');
    return $columns;
  }
}
