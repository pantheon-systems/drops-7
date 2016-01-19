<?php

/**
 * @file
 *   A simple-as-possible migration of Commerce product and product display node
 *   information.
 *
 *   You can run this migration with
 *     drush mi CommerceExampleProduct
 *     drush mi CommerceExampleProductDisplay
 *
 *   To see the status of all migrations and what migrations are available:
 *     drush ms
 *
 *   To roll back migrations:
 *     drush migrate-rollback CommerceExampleProductDisplay
 *     drush migrate-rollback CommerceExampleProduct
 *
 *   Please make sure you don't get one of the products into the cart while
 *   you're rolling back, as this makes no end of messiness. Products in the cart
 *   are (not supposed to be able) to be deleted.
 */

/*
 * Implements hook_migrate_api().
 */
function commerce_migrate_example_migrate_api() {
  $api = array(
    'api' => 2,
    'groups' => array(
      'commerce_example' => array(
        'title' => t('Commerce Example Imports'),
      ),
    ),
    'migrations' => array(
      'CommerceExampleProduct' => array(
        'class_name' => 'CommerceMigrateExampleProductMigration',
        'group' => 'commerce_example',
      ),
      'CommerceExampleProductDisplay' => array(
        'class_name' => 'CommerceMigrateExampleDisplayNodeMigration',
        'group' => 'commerce_example',
        'dependencies' => array(
          'CommerceExampleProduct',
        ),
      ),
    ),
  );

  return $api;
}

/**
 * A simple base class that contains things common to all
 * CommerceMigrateExample classes.
 */
abstract class CommerceMigrateExampleMigration extends Migration {
  public function __construct($arguments = array()) {
    // Always call the parent constructor first for basic setup
    parent::__construct($arguments);
  }

  /**
   * Provide the names of the incoming CSV file columns.
   */
  function csvcolumns() {
    $columns[0] = array('SKU', 'SKU');
    $columns[1] = array('Title', 'Title');
    $columns[2] = array('Price', 'Price');
    $columns[3] = array('Image', 'Image');
    $columns[4] = array('Description', 'Description');
    return $columns;
  }
}

/**
 * Import Products into the "product" product type.
 *
 * This simple migration assumes that a product type called 'product'
 * already exists, and has the sku, title, commerce_price, and field_image
 * fields.
 */
class CommerceMigrateExampleProductMigration extends CommerceMigrateExampleMigration {
  public function __construct($arguments = array()) {
    parent::__construct($arguments);
    $this->description = t('Import products from CSV file (with no header).');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        // The 'SKU' field in the CSV file is the key.
        'SKU' => array('type' => 'varchar',
                       'length' => 24,
                       'not null' => TRUE,
                       'description' => 'SKU',
                      ),
      ),
      // Rather than specifying the type directly here, we would probably use
      // arguments, but instead this just specifies the 'product' product type
      // to make it obvious what's going on.
      MigrateDestinationEntityAPI::getKeySchema('commerce_product', 'product')
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $csv_file = drupal_get_path('module', 'commerce_migrate_example') . '/commerce_migrate_example.csv';
    $this->source = new MigrateSourceCSV($csv_file, $this->csvcolumns());

    // Again here we're hardwiring the product type to 'product'.
    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'product');

    $this->addFieldMapping('sku', 'SKU');
    $this->addFieldMapping('title', 'Title');
    $this->addFieldMapping('commerce_price', 'Price');
    $this->addFieldMapping('field_image', 'Image');
    $this->addFieldMapping('field_image:file_class')
         ->defaultValue('MigrateFileUri');
    $this->addFieldMapping('field_image:source_dir')
         ->defaultValue(drupal_get_path('module', 'commerce_migrate_example') . '/images');
  }
}

/**
 * Import nodes of type 'product_display' from the same CSV file.
 *
 * This is hard-wired to the node type 'product_display' with a
 * product reference field of type field_product.
 */
class CommerceMigrateExampleDisplayNodeMigration extends CommerceMigrateExampleMigration {
  public function __construct($arguments = array()) {
    parent::__construct($arguments);
    $this->description = t('Import product display nodes from CSV file (with no header).');
    $this->dependencies = array('CommerceExampleProduct');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'SKU' => array('type' => 'varchar',
                     'length' => 24,
                     'not null' => TRUE,
                     'description' => 'SKU',
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $csv_file = drupal_get_path('module', 'commerce_migrate_example') . '/commerce_migrate_example.csv';
    $this->source = new MigrateSourceCSV($csv_file, $this->csvcolumns());

    $this->destination = new MigrateDestinationNode('product_display');

    // For a multivalued import, see the field_migrate_example_country mapping
    // in the Migrate Example in beer.inc.
    // Here we do a single SKU per product node.
    $this->addFieldMapping('field_product', 'SKU')->sourceMigration('CommerceExampleProduct');

    $this->addFieldMapping('title', 'Title');
    $this->addFieldMapping('body', 'Description');
  }

}
