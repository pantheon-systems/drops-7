<?php

/**   
 * @file
 *   Provides the page callback for user defined search pages.
 */ 

/**
 * Returns search results on user defined search pages.
 */
function apachesolr_search_user_defined_search_page($page_id, $keys = '') {
  $search_page = apachesolr_search_page_load($page_id);
  $build = array();
  $filters = isset($search_page->settings['fq']) ? $search_page->settings['fq'] : array();
  $solrsort = isset($_GET['solrsort']) ? $_GET['solrsort'] : '';
  // We may also have filters added by facet API module. The 'f'
  // is determined by constant FacetapiAdapter::FILTER_KEY. Hard
  // coded here to avoid extra class loading.
  $conditions = array();
  if (!empty($_GET['f']) && is_array($_GET['f'])) {
    if (module_exists('facetapi')) {
      $conditions['f'] = $_GET['f'];
    }
  }

  $empty_search_behavior = $search_page->settings['apachesolr_search_browse'];

  try {
    $solr = apachesolr_get_solr($search_page->env_id);
    // Adds the search form to the page.
    $build['search_form'] = drupal_get_form('apachesolr_search_user_defined_search_form', $search_page, $keys);

    if (!$keys && empty($conditions) && ($empty_search_behavior == 'browse' || $empty_search_behavior == 'blocks')) {
      // Pass empty search behavior as string on to apachesolr_search_search_page()
      $results = apachesolr_search_run('apachesolr', array('fq' => $filters), '', $search_page->search_path, 0, $solr);
      if ($empty_search_behavior == 'browse') {
        // Hide sidebar blocks for content-area browsing instead.
        apachesolr_suppress_blocks(TRUE);
      }

      $build['search_results'] = apachesolr_search_page_browse($empty_search_behavior);
    }
    elseif ($keys || !empty($conditions) || $empty_search_behavior == 'results') {
      $results = apachesolr_search_run('apachesolr', array('q' => $keys, 'fq' => $filters), $solrsort, $search_page->search_path, pager_find_page(), $solr);
      // Adds search results to the render array.
      $build['search_results'] = array(
        '#theme' => 'search_results',
        '#results' => $results,
        '#module' => 'apachesolr_search',
      );
    }
  }
  catch (Exception $e) {
    watchdog('Apache Solr', nl2br(check_plain($e->getMessage())), NULL, WATCHDOG_ERROR);
    apachesolr_failure(t('Solr search'), $keys);
  }

  return $build;
}

/**
 * Search for placed on user defined search pages.
 */
function apachesolr_search_user_defined_search_form($form, &$form_state, $search_page, $keys = '') {
  // Loads the core Search CSS file, use the core search module's classes.
  drupal_add_css(drupal_get_path('module', 'search') . '/search.css');
  $form['#id'] = 'search-form';
  $form['#attributes']['class'][] = 'search-form';

  $form['#search_page'] = $search_page;

  $form['basic'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('container-inline')),
  );

  $form['basic']['keys'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter terms'), 
    '#default_value' => $keys,
    '#size' => 20,
    '#maxlength' => 255,
  );

  $form['basic']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}

/**
 * Processes apachesolr_search_user_defined_search_form submissions.
 */
function apachesolr_search_user_defined_search_form_submit(&$form, &$form_state) {
  $page = $form['#search_page'];

  $redirect = $page->search_path;
  if (strlen($form_state['values']['keys'])) {
    $redirect .= '/' . $form_state['values']['keys'];
  }

  // Redirects to path set in configuration.
  $form_state['redirect'] = $redirect;
}
