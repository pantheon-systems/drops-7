<?php

/**
 * @file
 * Defines SchemaTestDatabaseConnection.
 */

class SchemaTestDatabaseConnection extends DatabaseConnection {
  function __construct(array $connection_options = array()) {
    $this->connectionOptions = $connection_options;
    $this->setPrefix(!empty($this->connectionOptions['prefix']) ? $this->connectionOptions['prefix'] : '');
  }

  public function queryRange($query, $from, $count, array $args = array(), array $options = array()) {
    return NULL;
  }

  function queryTemporary($query, array $args = array(), array $options = array()) {
    return NULL;
  }

  public function driver() {
    return 'schema_test';
  }

  public function databaseType() {
    return 'schema_test';
  }

  public function mapConditionOperator($operator) {
    return NULL;
  }

  public function nextId($existing_id = 0) {
    return NULL;
  }
}
