<?php

/**
 * @file
 * FeedsLdapQueryFetcher
 */


/**
 * FeedsLdapQueryFetcherResult
 */
class FeedsLdapQueryFetcherResult extends FeedsFetcherResult {
  public $query_ids = array();
  public $ldap_result;

  /**
   * Constructor.
   */
  public function __construct($source_config) {
    $this->query_ids = (isset($source_config['query_ids']) && is_array($source_config['query_ids'])) ? $source_config['query_ids'] : array();
    parent::__construct('');
    $this->ldap_result = $this->getRaw();
    return $this->ldap_result;
  }

  /**
   * Overrides parent::getRaw();
   */
  public function getRaw() {

    $results = array();
    foreach ($this->query_ids as $i => $query_id) {
      $ldapQuery = ldap_query_get_queries($query_id, 'enabled', TRUE);
      $more_results = $ldapQuery->query();
      if (is_array($more_results)) {
        $results = array_merge($results, $more_results);
      }
    }
    return $results;
  }
}

/**
 * Fetches data via LDAP Query.
 */
class FeedsLdapQueryFetcher extends FeedsFetcher {

  /**
   * Implements FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $result = new FeedsLdapQueryFetcherResult($source_config);
    return $result;
  }


  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    return array(
      'query_ids' => array(),
    );
  }

    /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $queries = ldap_query_get_queries(NULL, 'enabled');
    $query_options = array(0 => '--- select one or more queries ---');
    foreach ($queries as $qid => $query) {
      $query_options[$qid] = $query->name;
    }
    $form = array();
    $form['query_ids'] = array(
      '#type' => 'select',
      '#title' => t('LDAP Query'),
      '#multiple' => TRUE,
      '#size' => min(10, count($query_options)),
      '#required' => TRUE,
      '#default_value' => $this->config['query_ids'],
      '#description' => t('If more than one query is selected, results from all the queries will be returned.') .
        ' ' .
        t('Queries can be added and edited at !link', array('!link' => l(t('LDAP Query Admin'), LDAP_QUERY_INDEX_BASE_PATH))),
      '#options' => $query_options,
    );

    return $form;
  }

  /**
   * Override parent::sourceForm().
   */
  public function sourceForm($source_config) {

    $tokens = array(
      '!edit_link' => l(t('Edit Feed'), 'admin/structure/feeds/edit/' . $this->id),
    );

    $form_state = array();
    $form = $this->configForm($form_state);
    $form['preamble'] = array(
      '#type' => 'markup',
      '#markup' => t('This import is configured at !edit_link.', $tokens),
    );

    return $form;
  }

  /**
   * Override parent::sourceFormValidate().
   */
  public function sourceFormValidate(&$values) {
     // could execute query and see if it returns anything for validation
  }

}
