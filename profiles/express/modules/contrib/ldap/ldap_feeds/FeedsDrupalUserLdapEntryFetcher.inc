<?php

/**
 * @file
 * FeedsDrupalUserLdapEntryFetcher
 */

define('LDAP_FEEDS_DRUPAL_USER_FETCHER_FILTER_AUTHENTICATED', FALSE);
define('LDAP_FEEDS_DRUPAL_USER_FETCHER_FILTER_ROLES', FALSE);

class FeedsDrupalUserLdapEntryFetcherResult extends FeedsFetcherResult {

  public $filterLdapAuthenticated;
  public $availableDrupalUserAttributes;
  public $filterRoles;

  /**
   * Constructor.
   */
  public function __construct($source_config) {
    $this->availableDrupalUserAttributes = ldap_feeds_drupal_user_attributes();
    $this->filterLdapAuthenticated = isset($source_config['filterLdapAuthenticated']) ? $source_config['filterLdapAuthenticated'] : LDAP_FEEDS_DRUPAL_USER_FETCHER_FILTER_AUTHENTICATED;
    $this->filterRoles = isset($source_config['filterRoles']) ? $source_config['filterRoles'] : LDAP_FEEDS_DRUPAL_USER_FETCHER_FILTER_ROLES;

    parent::__construct('');
    $this->ldap_result = $this->getRaw();
    return $this->ldap_result;
  }

  /**
   * Overrides parent::getRaw();
   */
  public function getRaw() {

    // needs to loop through all users, and query ldap for each, one at a time

    $query = new EntityFieldQuery;
    $entities = $query
      ->entityCondition('entity_type', 'user')
      ->execute();
    $users = entity_load('user', array_keys($entities['user']));
    $selectedRoles = array_filter($this->filterRoles);
    $filterOnRoles = (boolean)(count($selectedRoles));

    foreach ($users as $uid => $user) {
      if (
        $uid == 0 ||
        $uid == 1 ||
        ($this->filterLdapAuthenticated && !isset($user->data['ldap_authentication'])) ||
        ($filterOnRoles && !array_intersect(array_values($selectedRoles), array_keys($user->roles)))
        ) {
        continue;
      }

      if ($ldap_entry = ldap_servers_get_user_ldap_data($user)) {
        unset($ldap_entry['mail']);
        $ldap_entry['attr']['count'] = $ldap_entry['attr']['count'] + count($this->availableDrupalUserAttributes);
        foreach ($this->availableDrupalUserAttributes as $attr_name => $attr_conf) {
          $ldap_entry['attr'][] = $attr_conf['token'];
          $ldap_entry['attr'][$attr_conf['token']]['count'] = 1;
          $ldap_entry['attr'][$attr_conf['token']][0] = (string)$user->{$attr_name};
        }

        $results[] = $ldap_entry;
      }
    }
    $results['count'] = count($results);
   // drupal_set_message("<pre>" . print_r($results, TRUE));
    return $results;
  }
}

/**
 * Fetches data via LDAP Query.
 */
class FeedsDrupalUserLdapEntryFetcher extends FeedsFetcher {

  /**
   * Implements FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $result = new FeedsDrupalUserLdapEntryFetcherResult($source_config);
    return $result;
  }

  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    return array(
      'filterLdapAuthenticated' => array(),
      'availableDrupalUserAttributes' => ldap_feeds_drupal_user_attributes(),
      'filterRoles' => array(),
    );
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {

    $form = array();
    $form['filterLdapAuthenticated'] = array(
      '#type' => 'checkbox',
      '#title' => t('Only return ldap authenticated users.'),
      '#default_value' =>  $this->config['filterLdapAuthenticated'],
      '#description' => t('If checked, only users who are associated with ldap accounts will be returned.'),
    );

    return $form;
  }

  /**
   * Override parent::sourceFormValidate().
   */
  public function sourceFormValidate(&$values) {

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
    $form['addendum'] = array(
      '#type' => 'markup',
      '#markup' => t('This import is configured at !edit_link.', $tokens),
    );

    return $form;
  }



}
