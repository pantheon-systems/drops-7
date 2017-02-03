<?php

/**
 * @file
 * Current Search plugin to provide Reset filters link.
 */

class FacetapiExtraCurrentSearchResetFiltersLink extends CurrentSearchItemText {
  /**
   * Implements CurrentSearchItem::execute().
   */
  public function execute(FacetapiAdapter $adapter) {
    $data = array('facetapi_adapter' => $adapter);

    // Determines plurality of string.
    if ($this->settings['plural']) {
      $condition = '[' . $this->settings['plural_condition'] . ']';
      $count = (int) token_replace($condition, $data);
      $raw_text = ($count != 1) ? $this->settings['text_plural'] : $this->settings['text'];
    }
    else {
      $raw_text = $this->settings['text'];
    }

    // Translates text, returns themed output.
    $translated_text = $this->translate('text', $raw_text);

    // Makes sure facet builds are initialized.
    $adapter->processFacets();

    $query = array();
    $path = '';

    // Build intesection of all filters. This will remove all filters.
    foreach ($adapter->getAllActiveItems() as $item) {
      if (empty($query)) {
        $query = $this->getQueryString($item, $adapter);
      }
      else {
        $facet_query = $this->getQueryString($item, $adapter);
        $query['f'] = array_intersect($query['f'], $facet_query['f']);
      }
      // Save the path.
      if (empty($path)) {
        $path = $this->getFacetPath($item, $adapter);
      }
    }

    // If no facets were selected.
    if (empty($path)) {
      return;
    }

    $variables = array(
      'text' => l(token_replace($translated_text, $data), $this->getFacetPath($item, $adapter), array('query' => $query)),
      'wrapper' => $this->settings['wrapper'],
      'element' => $this->settings['element'],
      'css' => $this->settings['css'],
      'class' => current_search_get_classes($this->settings['classes'], $data),
      'options' => array('html' => FALSE),
    );
    return array('#markup' => theme('current_search_text', $variables));
  }
}
