<?php

/**
 * @file
 * Bean translation handler for the translation module.
 */


/**
 * Bean translation handler.
 *
 * Overrides default behaviours for Bean properties.
 */
class EntityTranslationBeanHandler extends EntityTranslationDefaultHandler {

  public function __construct($entity_type, $entity_info, $entity) {
    parent::__construct('bean', $entity_info, $entity);
  }

  public function getAccess($op) {
    return entity_access($op, 'bean', $this->entity);
  }

  protected function getEntityId() {
    if (isset($this->entity->delta)) {
      return $this->entity->delta;
    }
    return parent::getEntityId();
  }
}
