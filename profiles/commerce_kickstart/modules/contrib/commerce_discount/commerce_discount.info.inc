<?php

/**
 * @file
 * Provides Entity metadata integration.
 */

/**
 * Extend the defaults meta data properties for Commerce discount entity.
 */
class CommerceDiscountMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['status']['type'] = 'boolean';

    return $info;
  }
}
