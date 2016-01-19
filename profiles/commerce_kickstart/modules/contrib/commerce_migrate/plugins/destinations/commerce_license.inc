<?php

/**
 * @file
 * Support for commerce products.
 */

/**
 * Destination class implementing migration into commerce products.
 */
class MigrateDestinationCommerceLicense extends MigrateDestinationEntityAPI {

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

    unset($fields['type']);

    $fields['granted'] = t('The date when the license was granted.');
    $fields['expires'] = t('The date when the license expires. 0 for never.');

    return $fields;
  }

}
