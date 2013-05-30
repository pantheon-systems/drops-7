<?php

/**
 * @file
 * Products data alteration callback.
 */

/**
 * Data alteration callback that filters out products that aren't
 * published
 */
class CommerceSearchApiAlterProductStatus extends SearchApiAbstractAlterCallback {

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
   * Exclude products that aren't published.
   */
  public function alterItems(array &$items) {
    if ($bundles = commerce_product_reference_node_types()) {
      $product_reference_fields = commerce_info_fields('commerce_product_reference', 'node');
      foreach ($items as $id => &$item) {
        if (!isset($bundles[$item->type])) {
          continue;
        }
        foreach ($product_reference_fields as $field_name => $field) {
          if (empty($item->{$field_name})) {
            continue;
          }
          $node_wrapper = entity_metadata_wrapper('node', $item);
          $nb_products = ($field['cardinality'] == 1) ? 1 : $node_wrapper->{$field_name}->count();
          $products_removed = 0;
          if ($field['cardinality'] == 1) {
            if ($node_wrapper->{$field_name}->status->raw() == 0) {
              $products_removed = 1;
            }
          }
          else {
            foreach ($node_wrapper->{$field_name} as $delta => $product_wrapper) {
              if ($product_wrapper->status->raw() == 0) {
                $products_removed++;
                if ($products_removed != $nb_products) {
                  $node_wrapper->{$field_name}->offsetUnset($delta);
                }
              }
            }
          }
          // Remove the product display if all the associated products are
          // unpublished.
          if ($products_removed == $nb_products) {
            unset($items[$id]);
          }
        }
      }
    }
  }

}
