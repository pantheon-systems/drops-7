<?php

/**
 * @file
 * API integration for the Schema module.
 */

/**
 * Alter the results from the getFieldTypeMap() methods in the schema class.
 *
 * @param array $map
 *   The array mapping of Drupal schema field names to DB-native field types.
 * @param DatabaseSchema $schema
 *   The database schema class.
 * @param DatabaseConnection $connection
 *   The database connection class since the $schema object doesn't offer
 *   $schema->connection as a public property.
 */
function hook_schema_field_type_map_alter(array &$map, DatabaseSchema $schema, DatabaseConnection $connection) {
  switch ($connection->getType()) {
    case 'mysql':
      $map['datetime:normal'] = 'DATETIME';
      break;
    case 'pgsql':
      $map['datetime:normal'] = 'timestamp without time zone';
      break;
    case 'sqlite':
      $map['datetime:normal'] = 'VARCHAR';
      break;
  }
}
