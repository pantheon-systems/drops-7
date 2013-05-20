<?php

/**
 * Defines the manager class for interacting with remote marketplace addons.
 */
class CommerceGuysMarketplaceAddonManager extends CommerceGuysMarketplaceManagerBase {

  /**
   * Constructs a CommerceGuysMarketplaceAddonManager object.
   */
  function __construct($client, $endpoint, $resource = NULL) {
    parent::__construct($client, $endpoint, 'addons');
  }

  /**
   * Performs a query against the remote service.
   *
   * @param $search
   *   (optional) A search query string.
   * @param $only_featured
   *  (optional) Whether to limit the search to featured addons only.
   * @param $category
   *  (optional) The id of the category to filter the query by.
   * @param $sorts
   *   (optional) An array of sorts to apply to the query, in the $field => $direction
   *   format.
   * @param $offset
   *   The number of results to skip. Defaults 0.
   * @param $limit
   *   The number of results to return. Defaults to 10.
   *
   * @return
   *   An array of results.
   */
  public function query($search = '', $only_featured = FALSE, $category = 0, $sorts = array(), $offset = 0, $limit = 10) {
    $params =  array(
      'offset' => $offset,
      'limit' => $limit,
    );
    // Add the search query.
    if (!empty($search)) {
      $params['search'] = $search;
    }
    if (!empty($only_featured)) {
      $params['only_featured'] = $only_featured;
    }
    // Filter by category.
    if (!empty($category)) {
      $params['category'] = $category;
    }
    // Add the sorts.
    if ($sorts) {
      $sort_by = array_keys($sorts);
      $sort_order = array_values($sorts);
      // The remote service only supports one active sort at a time.
      $params['sort_by'] = $sort_by[0];
      $params['sort_order'] = $sort_order[0];
    }

    $url = url($this->endpoint . '/addons', array('query' => $params));
    $response = drupal_http_request($url);
    $result = json_decode($response->data);
    return array(
      'results' => $result->results,
      'result_count' => $result->result_count
    );
  }

  /**
   * Returns an array of all defined addon categories, used for filtering.
   *
   * @param $reset
   *   Boolean indicating whether or not the cache should be reset, causing
   *   data to be refetched from the remote service.
   *
   * @return
   *   An array of categories in the form of $id => $name.
   */
  public function getCategories($reset = FALSE) {
    if (!$reset && $cached_data = cache_get('marketplace_addon_categories')) {
      $categories = $cached_data->data;
    }
    else {
      $response = drupal_http_request($this->endpoint . '/addon_categories');
      $categories = json_decode($response->data);
      cache_set('marketplace_addon_categories', $categories, CACHE_TEMPORARY);
    }

    return $categories;
  }

  /**
   * Returns an array of addon licenses.
   *
   * @param $category
   *  (optional) The id of the category to filter the query by.
   */
  public function getLicenses($category = 0) {
    // Authorize the site and get an access token.
    $access_token = $this->getAccessToken();
    $options = array(
      'headers' => array(
        'Authorization' => 'Bearer ' . $access_token['token'],
      ),
    );
    // Filter by category if needed.
    $params = array();
    if (!empty($category)) {
      $params['category'] = $category;
    }

    $url = url($this->endpoint . '/addon_licenses', array('query' => $params));
    $response = drupal_http_request($url, $options);
    $result = json_decode($response->data);
    return array(
      'results' => $result->results,
      'result_count' => $result->result_count
    );
  }

  /**
   * Returns an array of all defined addon promotions.
   *
   * @param $reset
   *   Boolean indicating whether or not the cache should be reset, causing
   *   data to be refetched from the remote service.
   *
   * @return
   *   An array of promotions.
   */
  public function getPromotions($reset = FALSE) {
    if (!$reset && $cached_data = cache_get('marketplace_addon_promotions')) {
      $promotions = $cached_data->data;
    }
    else {
      $response = drupal_http_request($this->endpoint . '/addon_promotions');
      $promotions = json_decode($response->data);
      cache_set('marketplace_addon_categories', $promotions, CACHE_TEMPORARY);
    }

    return $promotions;
  }
}
