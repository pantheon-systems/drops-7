<?php

/**
 * Plugin that removes all items if there are less than X items available.
 */
class FacetapiFilterShowIfMinimumItems extends FacetapiFilter {

  // Number of items.
  protected $count;
  // If there is active item in the list.
  protected $active;

  /**
   * Filters items.
   */
  public function execute(array $build) {
    $this->count = 0;
    $this->active = FALSE;
    $this->calculateItemsNumber($build);

    // If there is only X elements available we hide the facet.
    if (!$this->active && $this->count = $this->settings->settings['show_minimum_items']) {
      return array();
    }

    return $build;
  }

  /**
   * Calculate number of facet links.
   */
  protected function calculateItemsNumber($items) {
    foreach ($items as $key => $item) {
      // Run recursively on all children.
      if (!empty($item['#item_children'])) {
        $this->calculateItemsNumber($item['#item_children']);
      }

      $this->count++;
      $this->active = $this->active || $item['#active'];
    }
  }

  /**
   * Adds settings to the filter form.
   */
  public function settingsForm(&$form, &$form_state) {
    $form['show_minimum_items'] = array(
      '#title' => t('Minimum items'),
      '#type' => 'textfield',
      '#description' => t("Hide block if there is less than this number of items."),
      '#default_value' => $this->settings->settings['show_minimum_items'],
    );
  }

  /**
   * Returns an array of default settings.
   */
  public function getDefaultSettings() {
    return array(
      'show_minimum_items' => 2,
    );
  }
}