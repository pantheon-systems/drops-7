<?php

/**
 * @file
 *  Migrations for commerce_kickstart_product.
 */

class CommerceKickstartBagsCases extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import bags cases from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product', 'bags_cases')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/bags_cases.csv', $this->csvcolumns(), array('header_rows' => 1), $this->fields());

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'bags_cases');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');

    // Size
    $this->addFieldMapping('field_bag_size', 'size');
    $this->addFieldMapping('field_bag_size:create_term')
      ->defaultValue(TRUE);

    // Images
    $this->addFieldMapping('field_images', 'product_images');
    $this->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');

    // Color
    $this->addFieldMapping('field_color', 'color');
    $this->addFieldMapping('field_color:additional_field', 'hexadecimal');
    $this->addFieldMapping('field_color:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('language', 'language');
  }

  function csvcolumns() {
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('color', 'Color');
    $columns[3] = array('size', 'Size');
    $columns[4] = array('price', 'Price');
    $columns[5] = array('images', 'Images');
    $columns[6] = array('hexadecimal', 'Hexadecimal');
    return $columns;
  }

  function fields() {
    return array(
      'product_images' => 'An array of images, populated during prepareRow().',
    );
  }

  function prepareRow($row) {
    $row->product_images = explode(', ', $row->images);
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
  }
}

class CommerceKickstartDrinks extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import drinks from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/drinks.csv', $this->csvcolumns(), array('header_rows' => 1), $this->fields());

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'drinks');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');

    // Images
    $this->addFieldMapping('field_images', 'product_images');
    $this->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');

    // Color
    $this->addFieldMapping('field_color', 'color');
    $this->addFieldMapping('field_color:additional_field', 'hexadecimal');
    $this->addFieldMapping('field_color:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('language', 'language');
  }

  function csvcolumns() {
    // "Title","SKU","Color","Price","Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('color', 'Color');
    $columns[3] = array('price', 'Price');
    $columns[4] = array('images', 'Images');
    $columns[5] = array('hexadecimal', 'Hexadecimal');
    return $columns;
  }

  function fields() {
    return array(
      'product_images' => 'An array of images, populated during prepareRow().',
    );
  }

  function prepareRow($row) {
    $row->product_images = explode(', ', $row->images);
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
  }
}

class CommerceKickstartHats extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import hats from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/hats.csv', $this->csvcolumns(), array('header_rows' => 1), $this->fields());

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'hats');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');

    // Images
    $this->addFieldMapping('field_images', 'product_images');
    $this->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');

    // Size
    $this->addFieldMapping('field_hat_size', 'size');
    $this->addFieldMapping('field_hat_size:create_term')
      ->defaultValue(TRUE);

    // Color
    $this->addFieldMapping('field_color', 'color');
    $this->addFieldMapping('field_color:additional_field', 'hexadecimal');
    $this->addFieldMapping('field_color:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('language', 'language');
  }

  function csvcolumns() {
    // "title","SKU","Color","Size","Price","Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('color', 'Color');
    $columns[3] = array('size', 'Size');
    $columns[4] = array('price', 'Price');
    $columns[5] = array('images', 'Images');
    $columns[6] = array('hexadecimal', 'Hexadecimal');
    return $columns;
  }

  function fields() {
    return array(
      'product_images' => 'An array of images, populated during prepareRow().',
    );
  }

  function prepareRow($row) {
    $row->product_images = explode(', ', $row->images);
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
  }
}

class CommerceKickstartTops extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import tops from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/tops.csv', $this->csvcolumns(), array('header_rows' => 1), $this->fields());

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'tops');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');

    // images
    $this->addFieldMapping('field_images', 'product_images');
    $this->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');

    // Size
    $this->addFieldMapping('field_top_size', 'size');
    $this->addFieldMapping('field_top_size:create_term')
      ->defaultValue(TRUE);

    // Color
    $this->addFieldMapping('field_color', 'color');
    $this->addFieldMapping('field_color:additional_field', 'hexadecimal');
    $this->addFieldMapping('field_color:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('language', 'language');
  }

  function csvcolumns() {
    // "Title","SKU","Color","Size","Price","Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('color', 'Color');
    $columns[3] = array('size', 'Size');
    $columns[4] = array('price', 'Price');
    $columns[5] = array('images', 'Images');
    $columns[6] = array('hexadecimal', 'Hexadecimal');
    return $columns;
  }

  function fields() {
    return array(
      'product_images' => 'An array of images, populated during prepareRow().',
    );
  }

  function prepareRow($row) {
    $row->product_images = explode(', ', $row->images);
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
  }
}

class CommerceKickstartShoes extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import shoes from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/shoes.csv', $this->csvcolumns(), array('header_rows' => 1), $this->fields());

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'shoes');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');

    // Images
    $this->addFieldMapping('field_images', 'product_images');
    $this->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');

    // Size
    $this->addFieldMapping('field_shoe_size', 'size');
    $this->addFieldMapping('field_shoe_size:create_term')
      ->defaultValue(TRUE);

    // Color
    $this->addFieldMapping('field_color', 'color');
    $this->addFieldMapping('field_color:additional_field', 'hexadecimal');
    $this->addFieldMapping('field_color:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('language', 'language');
  }

  function csvcolumns() {
    // "Title","SKU","Color","Size","Price","Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('color', 'Color');
    $columns[3] = array('size', 'Size');
    $columns[4] = array('price', 'Price');
    $columns[5] = array('images', 'Images');
    $columns[6] = array('hexadecimal', 'Hexadecimal');
    return $columns;
  }

  function fields() {
    return array(
      'product_images' => 'An array of images, populated during prepareRow().',
    );
  }

  function prepareRow($row) {
    $row->product_images = explode(', ', $row->images);
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
  }
}

class CommerceKickstartStorage extends CommerceKickstartMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import storage from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/storage_devices.csv', $this->csvcolumns(), array('header_rows' => 1), $this->fields());

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'storage_devices');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');

    // Images
    $this->addFieldMapping('field_images', 'product_images');
    $this->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');

    // Capacity ("size")
    $this->addFieldMapping('field_storage_capacity', 'capacity');
    $this->addFieldMapping('field_storage_capacity:weight', 'weight');
    $this->addFieldMapping('field_storage_capacity:create_term')
      ->defaultValue(TRUE);

    // Color
    $this->addFieldMapping('field_color', 'color');
    $this->addFieldMapping('field_color:additional_field', 'hexadecimal');
    $this->addFieldMapping('field_color:create_term')
      ->defaultValue(TRUE);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('language', 'language');
  }

  function csvcolumns() {
    // "Title","SKU","Color","Capacity","Price","Images"
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('color', 'Color');
    $columns[3] = array('capacity', 'Capacity');
    $columns[4] = array('price', 'Price');
    $columns[5] = array('images', 'Images');
    $columns[6] = array('hexadecimal', 'Hexadecimal');
    $columns[7] = array('weight', 'Weight');
    return $columns;
  }

  function fields() {
    return array(
      'product_images' => 'An array of images, populated during prepareRow().',
    );
  }

  function prepareRow($row) {
    $row->product_images = explode(', ', $row->images);
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
  }
}

class CommerceKickstartNode extends DynamicMigration {

  public function __construct(array $arguments) {
    $this->arguments = $arguments;
    parent::__construct();
    $this->description = t('Import product nodes.');
    $this->dependencies = array('CommerceKickstartBagsCases', 'CommerceKickstartDrinks', 'CommerceKickstartHats', 'CommerceKickstartShoes', 'CommerceKickstartStorage', 'CommerceKickstartTops', 'CommerceKickstartPages');

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
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/product_display_' . $arguments['type'] . '.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationNode($arguments['type']);

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('body', 'description');
    $this->addFieldMapping('body:summary', 'excerpt');
    $this->addFieldMapping('field_brand', 'brand');
    $this->addFieldMapping('field_brand:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_category', 'category');
    $this->addFieldMapping('field_category:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_collection', 'collection')
          ->separator(', ');
    $this->addFieldMapping('field_collection:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_gender', 'gender');
    $this->addFieldMapping('field_gender:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_product', 'skus');
  }

  public function prepareRow($row) {
    $products = array();
    foreach(explode(', ', $row->sku) as $sku) {
      $product = commerce_product_load_by_sku($sku);
      $products[] = $product->product_id;
    }
    $row->skus = $products;
  }

  function csvcolumns() {
    $columns[0] = array('title', 'Title');
    $columns[1] = array('description', 'Description');
    $columns[2] = array('collection', 'Collection');
    $columns[3] = array('category', 'Category');
    $columns[4] = array('gender', 'Gender');
    $columns[5] = array('brand', 'Brand');
    $columns[6] = array('sku', 'SKU');
    $columns[7] = array('product', 'Product');
    return $columns;
  }

  /**
   * Construct the machine name from the source file name.
   */
  protected function generateMachineName($class_name = NULL) {
    return drupal_strtolower('CommerceKickstartNode' . $this->arguments['type']);
  }

  public function processImport(array $options = array()) {
    parent::processImport($options);
    // Do not force menu rebuilding. Otherwise pathauto will try to rebuild
    // in each node_insert invocation.
    variable_set('menu_rebuild_needed', FALSE);
  }
}

class CommerceKickstartCollection extends Migration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import collections terms.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'name' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'commerce_kickstart_product') . '/import/collections.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationTerm('collection');

    $this->addFieldMapping('name', 'name');
    // Images
    $this->addFieldMapping('field_image', 'images');
    $this->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'commerce_kickstart_product') . '/import/images');
  }

  function csvcolumns() {
    // "Name", "Images"
    $columns[0] = array('name', 'Name');
    $columns[1] = array('images', 'Images');
    return $columns;
  }
}
