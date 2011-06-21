<?php

/**
 * @file
 * Term query type plugin for the Apache Solr adapter.
 */

/**
 * Plugin for "term" query types.
 */
class ApacheSolrFacetapiTerm extends FacetapiQueryType implements FacetapiQueryTypeInterface {

  /**
   * Returns the query type associated with the plugin.
   *
   * @return string
   *   The query type.
   */
  static public function getType() {
    return 'term';
  }

  /**
   * Adds the filter to the query object.
   *
   * @param DrupalSolrQueryInterface $query
   *   An object containing the query in the backend's native API.
   */
  public function execute($query) {
    $settings = $this->adapter->getFacet($this->facet)->getSettings();
    // Adds the operator parameter.
    $operator = $settings->settings['operator'];
    $ex = (FACETAPI_OPERATOR_OR != $operator) ? '' : "{!ex={$this->facet['field']}}";
    $query->addParam('facet.field', $ex . $this->facet['field']);

    // Adds "hard limit" parameter to prevent too many return values.
    $limit = empty($settings->settings['hard_limit']) ? 20 : (int) $settings->settings['hard_limit'];
    $query->addParam('f.' . $this->facet['field'] . '.facet.limit', $limit);
  }

  /**
   * Initializes the facet's build array.
   *
   * @return array
   *   The initialized render array.
   */
  public function build() {
    $build = array();
    if ($response = apachesolr_static_response_cache($this->adapter->getSearcher())) {
      if (isset($response->facet_counts->facet_fields->{$this->facet['field']})) {
        $values = (array) $response->facet_counts->facet_fields->{$this->facet['field']};
        foreach ($values as $value => $count) {
          $build[$value] = array('#count' => $count);
        }
      }
    }
    return $build;
  }
}
