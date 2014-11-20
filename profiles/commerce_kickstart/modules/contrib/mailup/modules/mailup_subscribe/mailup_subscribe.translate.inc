<?php

/**
 * @file
 * Entity translation support
 */

/**
 * Subcription translation handler.
 */
class EntityTranslationMailUpSubscriptionHandler extends EntityTranslationDefaultHandler {

  public function __construct($entity_type, $entity_info, $entity) {
    parent::__construct('mailup_subscription', $entity_info, $entity);
  }

}