Commerce Migrate is a general-purpose migration framework extending Migrate Module
for bringing store information into Drupal Commerce.

- Migrate destination field handlers for commerce fields (reference fields, price field)
- Migrate destination plugin for commerce product types.
Commerce Migrate depends on Migrate Extras for Entity API and Address Field integration.

Ubercart migration
------------------
Commerce Migrate Ubercart has moved to its own project,
http://drupal.org/project/commerce_migrate_ubercart
It can migrate 6.x and 7.x Ubercart stores from either the existing Drupal database
or a remote database.

Price fields
------------
Tax rates and currencies can be migrated either as arguments or subfields.

As an argument, these are set once in the mapping:

<?php
$arguments = MigrateCommercePriceFieldHandler::arguments('your_currency_code', 'some_vat_name'));
$this->addFieldMapping('commerce_price', 'price')
  ->arguments($arguments);
?>

As subfields, these can more easily be different for each record of the
migration:

<?php
$this->addFieldMapping('commerce_price', 'price');
$this->addFieldMapping('commerce_price:currency_code', NULL)
  ->defaultValue('GBP');
$this->addFieldMapping('commerce_price:tax_rate', 'price_tax')
  ->description(t('The tax rate is in the price_tax field in the source.'));
?>

Resources
---------
The Migrate handbook page at http://drupal.org/node/415260
http://cyrve.com/import
http://www.gizra.com/content/data-migration-part-1
http://www.gizra.com/content/data-migration-part-2

