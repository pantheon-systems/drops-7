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
        $source = $map['source'];
        if (isset($ldap_entry['attr'])) {
          // exception need because of unconvential format of ldap data returned from $ldap_server->user_lookup
          $ldap_attributes = $ldap_entry['attr'];
        }
        else {
          $ldap_attributes = $ldap_entry;
        }
        if ($source != 'dn' && isset($ldap_attributes[$source][0])) {
          if ($ldap_attributes[$source]['count'] == 1 && is_scalar($ldap_attributes[$source][0])) {
            $parsed_item[$source] = (string)$ldap_attributes[$source][0];
          }
/** removed until design decisions on multivalued attributes are made
          elseif ($ldap_entry['count'] > 1) {
            switch ($this->config['multivalued']) {
              case LDAP_FEEDS_QUERY_FETCHER_MULTI_COMMA:
                unset($ldap_entry[$source]['count']);
                $parsed_item[$source] = join(',', $ldap_entry[$source]);
                break;
              case LDAP_FEEDS_QUERY_FETCHER_MULTI_SHOW_FIRST:
                $parsed_item[$source] = $ldap_entry[$source][0];
                break;

              case LDAP_FEEDS_QUERY_FETCHER_MULTI_IGNORE:
                break;

              case LDAP_FEEDS_QUERY_FETCHER_MULTI_ARRAY:
                for ($k = 0; $k < $ldap_entry[$source]['count'] - 1; $k++) {
                 if (is_scalar($ldap_entry[$source][$k])) {
                    $parsed_item[$source . '[' . $k . ']'] = (string)$ldap_entry[$source][$k];
                  }
                }
            }
          }
 */
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

    /** removed until design decisions on multivalued attributes are made
    $form['multivalued'] = array(
      '#type' => 'radios',
      '#title' => t('How should ldap attribute with mulitple values be parsed?'),
      '#options' => array(
          LDAP_FEEDS_QUERY_FETCHER_MULTI_COMMA => 'Flatten and separate with commas. The source name will be the attribute name such as "memberof"',
          LDAP_FEEDS_QUERY_FETCHER_MULTI_SHOW_FIRST => 'Only use first value.  The source name  will be the attribute name such as "memberof"',
          LDAP_FEEDS_QUERY_FETCHER_MULTI_IGNORE => 'Ignore these.  The data is problematic anyway.  Data will be discarded by parser.',
          LDAP_FEEDS_QUERY_FETCHER_MULTI_ARRAY => 'Give each value its own key, such as memberof[0] and memberof[1]. The source name will be memberof[0], memberof[1], etc.',
        ),
      '#default_value' => $this->config['multivalued'],
      '#description' => 'For example, in the Active Directory the memberof attribute will have many items, each representing a group.
        Generally this data is hard to map to anything other than a flattenned version such as a comma separated list.
      '#default_value' => LDAP_FEEDS_QUERY_FETCHER_DEFAULT,
    );

  */

    return $form;
  }

  public function sourceDefaults() {
    return array();
  }

}
