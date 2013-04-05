<?php

/**
 * @file
 * Product display data alteration callback.
 */

/**
 * Data alteration callback that filters out nodes that aren't
 * product displays.
 */
class CommerceSearchApiProductDisplayFilter extends SearchApiAbstractAlterCallback {

  /**
   * Check whether this data-alter callback is applicable for a certain index.
   *
   * @param SearchApiIndex $index
   *   The SearchApiIndex this data alteration callback resides on.
   *
   * @return bool
   *   A boolean whether or not this index is supported.
   */
  public function supportsIndex(SearchApiIndex $index) {
    $bundles = commerce_product_reference_node_types();
    return ($index->item_type == 'node' && !empty($bundles));
  }

  /**
   * Exclude nodes that aren't product displays.
   */
  public function alterItems(array &$items) {
    if ($bundles = commerce_product_reference_node_types()) {
      foreach ($items as $id => $item) {
        if (!isset($bundles[$item->type])) {
          unset($items[$id]);
        }
      }
    }
  }

}
