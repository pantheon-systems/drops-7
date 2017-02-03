<?php

/**
 * Plugin that removes all items that are parents of other items.
 */
class FacetapiFilterShowDeepestLevelItems extends FacetapiFilter {

  // Filtered build.
  protected $build;

  /**
   * Filters items.
   */
  public function execute(array $build) {
    $this->build = array();

    $this->collectNonParents($build);

    return $this->build;
  }

  /**
   * Collect only items that do not have children.
   */
  protected function collectNonParents($items) {
    foreach ($items as $key => $item) {
      // Run recursively on all children.
      if (!empty($item['#item_children'])) {
        $this->collectNonParents($item['#item_children']);
      }
      else {
        $this->build[] = $item;
      }
    }
  }
}
