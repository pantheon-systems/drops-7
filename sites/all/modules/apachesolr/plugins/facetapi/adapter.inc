<?php

/**
 * @file
 * Classes used by the Facet API module.
 */

/**
 * Facet API adapter for the Apache Solr Search Integration module.
 */
class ApacheSolrFacetapiAdapter extends FacetapiAdapter {
  /**
   * Returns the path to the admin settings for a given realm.
   *
   * @param $realm_name
   *   The name of the realm.
   *
   * @return
   *   The path to the admin settings.
   */
  public function getPath($realm_name) {
    $base_path = 'admin/config/search/apachesolr';
    return $base_path . '/facets/' . $realm_name;
  }

  /**
   * Requests facets and applies facet filters to the query.
   *
   * @param stdClass $query
   *   The query in the backend's native API.
   */
  function addFacetsFilters($query) {
    $enabled_facets = facetapi_get_enabled_facets($this->info['name']);
    if ($enabled_facets) {
      $query->addParam('facet', 'true');
      $query->addParam('facet.sort', 'count');
      $query->addParam('facet.mincount', '1');
      foreach ($enabled_facets as $facet) {
        $this->addFacetQuery($facet, $query);
        $this->addActiveFilters($facet, $query);
      }
    }
  }

  /**
   * Back-end specific function for adding filters.
   *
   * Typically will call $this->getActiveItems($facet);
   *
   * @param array $facet
   *   The facet definition.
   * @param mixed $query
   *   A mixed value containing the query in the backend's native API.
   *
   * @return FacetapiFacet
   *   The facet rendering object object.
   */
   public function addActiveFilters(array $facet, $query) {

     $active = $this->getActiveItems($facet);
     $settings = $this->getFacet($facet)->getSettings();
     $operator = $settings->settings['operator'];

     if (FACETAPI_OPERATOR_OR != $operator) {
       foreach ($active as $value => $item) {
         $query->addFilter($facet['field'], $value);
       }
     }
     else {
       // OR facet.
       $local = "tag={$facet['field']}";
       $values = array_keys($active);
       if ($values) {
         $query->addFilter($facet['field'], '(' . implode(' OR ', $values) . ')', FALSE, $local);
       }
     }
   }

  /**
   * Returns a boolean flagging whether $this->_searcher executed a search.
   */
  public function searchExecuted() {
    // Initial check - has ANY solr query run.
    $this_has_searched = apachesolr_has_searched();
    // Secondary check - do we have results for this searcher?
    $this_has_searched = $this_has_searched && apachesolr_static_response_cache($this->getSearcher());
    return $this_has_searched;
  }

  public function suppressOutput($realm_name) {
    $flag = FALSE;
    if ($realm_name == 'block') {
      $flag = apachesolr_suppress_blocks();
    }
    return $flag || !$this->searchExecuted();
  }

  /**
   * Returns the search keys.
   */
  public function getSearchKeys() {
    if (NULL === $this->keys) {
      if ($query = apachesolr_current_query()) {
        return $query->getParam('q');
      }
    }
    else {
      return $this->keys;
    }
  }

  /**
   * Returns the nmber of total results found for the current search.
   */
  public function getResultCount() {
    $response = apachesolr_static_response_cache($this->getSearcher());
    if ($response) {
      return $response->response->numFound;
    }
  }
}
