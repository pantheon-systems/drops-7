<?php

/**
 * @file
 *
 * Provides the Parser for an ldap entry array.
 */

class FeedsLdapEntryParser extends FeedsParser {
  public $ldap_result;

   /**
   * Implements FeedsParser::parse().
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {

    $mappings = feeds_importer($this->id)->processor->config['mappings'];
    $ldap_entries = $fetcher_result->ldap_result;
    $parsed_items = array();
    for ($i = 0; $i < $ldap_entries['count']; $i++) {
      $ldap_entry = $ldap_entries[$i];
      $parsed_item = array('dn' => (string)$ldap_entry['dn']);
      foreach ($mappings as $j => $map) {
        $source_lcase = drupal_strtolower($map['source']);
        // dn is already parsed
        if ($source_lcase == 'dn') {
          continue;
        }
        $source = $map['source'];
        if (isset($ldap_entry['attr'])) {
          // exception need because of unconvential format of ldap data returned from $ldap_server->userUserNameToExistingLdapEntry
          $ldap_attributes = $ldap_entry['attr'];
        }
        else {
          $ldap_attributes = $ldap_entry;
        }
        if (isset($ldap_attributes[$source_lcase][0])) {
          if ($ldap_attributes[$source_lcase]['count'] == 1 && is_scalar($ldap_attributes[$source_lcase][0])) {
            $parsed_item[$source] = (string)$ldap_attributes[$source_lcase][0];
          }
          else {
            unset($ldap_attributes[$source_lcase]['count']);
            $parsed_item[$source] = $ldap_attributes[$source_lcase];
          }
        }
        else {
          $parsed_item[$source] = '';
        }
      }
      $parsed_items[] = $parsed_item;
    }
    $result = new FeedsParserResult();
    $result->items = $parsed_items;
    return $result;
  }


  /**
   * Source form.
   */
  public function sourceForm($source_config) {
    $form = array();
    $mappings = feeds_importer($this->id)->processor->config['mappings'];
    if (empty($source_config)) {
      $source_config = $this->config;
    }
    return $form;
  }

  /**
   * Override parent::configFormValidate().
   */
  public function configFormValidate(&$values) {
    $this->setConfig(array('sources' => $values));
    $this->save();
  }


  /**
  * Override parent::getMappingSources().
  */
  public function getMappingSources() {
    return FALSE;
  }

  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    /** removed until design decisions on multivalued attributes are made
    return array(
      'multivalued' => LDAP_FEEDS_QUERY_FETCHER_MULTI_DEFAULT,
    );

  */
    return array();
  }

   /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $form = array();
    return $form;
  }

  public function sourceDefaults() {
    return array();
  }

}
