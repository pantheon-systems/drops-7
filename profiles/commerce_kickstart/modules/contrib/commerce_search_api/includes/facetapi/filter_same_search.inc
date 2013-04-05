<?php

/**
 * @file
 * FacetApiFilter, filter out items that do not change search results.
 */

/**
 * Filter out items that do not change search results.
 */
class CommerceSearchApiSameSearch extends FacetapiFilter {

  /**
   * Executes the filter by returning an altered render array.
   */
  public function execute(array $build) {
    $this->filterItems($build, $this->adapter->getResultCount());
    return $build;
  }

  /**
   * Clears useless items.
   */
  protected function filterItems(&$items, $number) {
    foreach ($items as $key => $item) {
      if (!empty($item['#item_children'])) {
        $this->filterItems($item['#item_children'], $number);
      }
      if ($item['#count'] == $number && empty($item['#item_children']) && !$item['#active']) {
        unset($items[$key]);
      }
    }
  }
}
