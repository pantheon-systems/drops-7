<?php

/**
 * @file
 * Support for commerce products.
 */

/**
 * Destination class implementing migration into commerce products.
 */
class MigrateDestinationCommerceProduct extends MigrateDestinationEntityAPI {

  /**
   * Returns a list of fields available to be mapped.
   *
   * @param Migration $migration
   *  Optionally, the migration containing this destination.
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields($migration = NULL) {
    $fields = parent::fields($migration);

    // Commerce Product module declares various properties that either are
    // irrelevant to migrations or are pseudoproperties that don't exist in the
    // database. Hence we remove them as potential destinations here.
    // @see commerce_product_entity_property_info()
    unset($fields['type']);
    unset($fields['creator']);

    return $fields;
  }

}
