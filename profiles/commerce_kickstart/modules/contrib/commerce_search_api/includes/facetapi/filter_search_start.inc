<?php

/**
 * @file
 * FacetApiFilter, remove items that should not appear on empty search.
 */

/**
 * Filter out items should not appear on empty search.
 */
class CommerceSearchApiSearchStart extends FacetapiFilter {

  /**
   * Returns an empty array.
   */
  public function execute(array $build) {
    if (!count($this->adapter->getAllActiveItems())) {
      return array();
    }
    return $build;
  }

}
