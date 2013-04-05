<?php

/**
 * @file
 * Support for paths in core Drupal objects
 */

class MigratePathEntityHandler extends MigrateDestinationHandler {
  public function __construct() {
    $this->registerTypes(array('entity'));
  }

  public function fields($entity_type, $bundle) {
    if (module_exists('path')) {
      switch ($entity_type) {
        case 'node':
          return array('path' => t('Node: Path alias'));
        case 'user':
          return array('path' => t('User: Path alias'));
        case 'taxonomy_term':
          return array('path' => t('Term: Path alias'));
      }
    }
    return array();
  }

  public function prepare($entity, stdClass $row) {
    if (module_exists('path') && isset($entity->path)) {
      $path = $entity->path;
      $entity->path = array();
      $entity->path['alias'] = $path;
    }
  }
}
