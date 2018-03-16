<?php

/**
 * Plugin that filters items that do not narrow results.
 */
class FacetapiFilterNarrowResults extends FacetapiFilter {

  // Number of results.
  protected $count;

  /**
   * Filters items.
   */
  public function execute(array $build) {
    $this->count = $this->adapter->getResultCount();
    $this->removeNotNarrowItems($build);
    return $build;
  }

  /**
   * Removes itesm that do not narrow search results.
   */
  protected function removeNotNarrowItems(&$items) {
    foreach ($items as $key => $item) {
      // Run recursively cleanup on all children.
      if (!empty($item['#item_children'])) {
        $this->removeNotNarrowItems($item['#item_children']);
      }
      // If item does not narrow results, does not have children and is
      // not active we remove it.
      if ($item['#count'] == $this->count && empty($item['#item_children']) && !$item['#active']) {
        unset($items[$key]);
      }
    }
  }
}
