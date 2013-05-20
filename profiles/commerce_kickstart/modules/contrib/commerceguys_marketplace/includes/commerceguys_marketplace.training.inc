<?php

/**
 * Defines the manager class for interacting with remote marketplace trainings.
 */
class CommerceGuysMarketplaceTrainingManager extends CommerceGuysMarketplaceManagerBase {

  /**
   * Constructs a CommerceGuysMarketplaceTrainingManager object.
   */
  function __construct($client, $endpoint, $resource = NULL) {
    parent::__construct($client, $endpoint, 'trainings');
  }

  /**
   * Performs a query against the remote service.
   *
   * @param $sorts
   *   (optional) An array of sorts to apply to the query, in the $field => $direction
   *   format.
   *
   * @return
   *   An array of results.
   */
  public function query($sorts = array()) {
    $params = array();
    if ($sorts) {
      // Add the sorts.
      $sort_by = array_keys($sorts);
      $sort_order = array_values($sorts);
      // The remote service only supports one active sort at a time.
      $params['sort_by'] = $sort_by[0];
      $params['sort_order'] = $sort_order[0];
    }

    $url = url($this->endpoint . '/trainings', array('query' => $params));
    $response = drupal_http_request($url);
    $result = json_decode($response->data);
    return array(
      'results' => $result->results,
      'result_count' => $result->result_count
    );
  }
}
