<?php

/**
 * @file
 * Integration with page_title module.
 */

/**
 * Implements hook_page_title_pattern_alter().
 */
function facetapi_bonus_page_title_pattern_alter(&$pattern, &$types) {
  if ($searchers = facetapi_get_active_searchers()) {
    $pattern = variable_get('page_title_facetapi_bonus', '');

    $searcher = reset($searchers);
    $adapter = facetapi_adapter_load($searcher);

    $types['facetapi_results'] = array('facetapi_adapter' => $adapter);

    // If we meet list<...[facetapi_active:XXX]...> string we replace it with
    // coma separated list of tokenized string.
    // So list<[facetapi_active:facet-label]: [facetapi_active:active-value]>
    // will be replaced with "type: node, author: admin" string if type and
    // author facets active.
    if (strpos($pattern, 'list<') !== FALSE && strpos($pattern, 'facetapi_active') !== FALSE) {
      preg_match_all('/list<[^>]*\[facetapi_active:[^>]*>/', $pattern, $matches);
      $replacements = array();
      foreach ($matches[0] as $subpattern) {
        $string = substr($subpattern, 5, strlen($subpattern) - 6);
        // @see CurrentSearchItemActive::execute().
        $subreplacements = array();
        foreach ($adapter->getAllActiveItems() as $item) {
          // Adds adapter to the active item for token replacement.
          $item['adapter'] = $adapter;
          // Builds variables to pass to theme function.
          $data = array('facetapi_active_item' => $item);

          $subreplacements[] = token_replace($string, $data);
        }
        $replacements[$subpattern] = implode(', ', $subreplacements);
      }
      $pattern = str_replace(array_keys($replacements), array_values($replacements), $pattern);
    }
  }
}

/**
 * Implements hook_page_title_settings().
 */
function facetapi_bonus_page_title_settings() {
  return array(
    'page_title_facetapi_bonus' => array(
      'label' => 'Facet API',
      'scopes' => array('global', 'facetapi_results', 'facetapi_active'),
      'show field' => TRUE,
      'description' => 'This pattern will be used for pages that uses Facet API module (e.g. search pages, views based on search index)',
    ),
  );
}

/**
 * Alter Page Title settings form.
 *
 * Add facetapi tokens to help.
 */
function facetapi_bonus_form_page_title_admin_settings_alter(&$form, $form_state) {
  $form['token_help']['content']['#token_types'][] = 'facetapi_results';
  $form['token_help']['content']['#token_types'][] = 'facetapi_active';
}
