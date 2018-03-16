<?php

/**
 * @file
 * Support for the Pathauto module.
 */

/**
 * Field handler.
 */
class MigratePathautoHandler extends MigrateDestinationHandler {
  public function __construct() {
    $this->registerTypes(array('node', 'user', 'taxonomy_term'));
  }

  /**
   * Make the destination field visible.
   */
  public function fields() {
    if (module_exists('pathauto')) {
      return array(
        'pathauto' =>
          t('Pathauto: Perform aliasing (set to 0 to prevent alias generation during migration'),
      );
    }
    else {
      return array();
    }
  }

  public function prepare($entity, stdClass $row) {
    if (isset($entity->pathauto)) {
      if (!isset($entity->path)) {
        $entity->path = array();
      }
      elseif (is_string($entity->path)) {
        // If MigratePathEntityHandler->prepare() hasn't run yet, support
        // the alias (set as $entity->path as a string) being formatted properly
        // in the path alias array.
        $path = $entity->path;
        $entity->path = array();
        $entity->path['alias'] = $path;
      }
      $entity->path['pathauto'] = $entity->pathauto;
      if (!isset($entity->path['alias'])) {
        $entity->path['alias'] = '';
      }
      unset($entity->pathauto);
    }
  }
}
