<?php
/**
 * @file
 *    theme registry and preprocess functions for the default templates
 */

/**
 * Implements hook_theme().
 */
function google_appliance_theme() {

  $registry = array();
  $ga_template_dir = drupal_get_path('module', 'google_appliance') . '/theme';

  // Full search form on results pages.
  $registry['google_appliance_search_form'] = array(
    'arguments' => array('form' => NULL),
    'render element' => 'form',
    'template' => 'google-appliance-search-form',
    'path' => $ga_template_dir,
  );

  // Block search form.
  $registry['google_appliance_block_form'] = array(
    'render element' => 'form',
    'template' => 'google-appliance-block-form',
    'path' => $ga_template_dir,
  );

  // Results page (top level template for the rest).
  $registry['google_appliance_results'] = array(
    'variables' => array('search_query_data' => NULL, 'response_data' => NULL, 'synonyms_label' => t('You might also try:')),
    'template' => 'google-appliance-results',
    'path' => $ga_template_dir,
  );

  // Spelling suggestion.
  $registry['google_appliance_spelling_suggestion'] = array(
    'variables' => array('spelling_suggestion' => NULL),
    'template' => 'google-appliance-spelling-suggestion',
    'path' => $ga_template_dir,
  );

  // Single result within the results page.
  $registry['google_appliance_result'] = array(
    'variables' => array('result_data' => NULL),
    'template' => 'google-appliance-result',
    'path' => $ga_template_dir,
  );

  // Single keymatch within the results page.
  $registry['google_appliance_keymatch'] = array(
    'variables' => array('keymatch_data' => NULL),
    'template' => 'google-appliance-keymatch',
    'path' => $ga_template_dir,
  );

  // Single synonym within the results page.
  $registry['google_appliance_synonym'] = array(
    'variables' => array('synonyms' => NULL),
    'template' => 'google-appliance-synonym',
    'path' => $ga_template_dir,
  );

  // Single Onebox module within a block.
  $registry['google_appliance_onebox_module'] = array(
    'variables' => array('onebox' => NULL),
    'template' => 'google-appliance-onebox-module',
    'path' => $ga_template_dir,
  );

  // Single Onebox result within a Onebox module.
  $registry['google_appliance_onebox_result'] = array(
    'variables' => array('result' => NULL),
    'template' => 'google-appliance-onebox-result',
    'path' => $ga_template_dir,
  );

  // Pager.
  $registry['google_appliance_pager'] = array(
    'variables' => array('pager_data' => NULL),
  );

  // Sort headers.
  $registry['google_appliance_sort_headers'] = array(
    'variables' => array('sort_header_data' => NULL),
    'template' => 'google-appliance-sort-headers',
    'path' => $ga_template_dir,
  );

  // Search stats.
  $registry['google_appliance_search_stats'] = array(
    'variables' => array('search_stats_data' => NULL),
    'template' => 'google-appliance-search-stats',
    'path' => $ga_template_dir,
  );

  return $registry;
}


/**
 * Preprocess block search form
 */
function template_preprocess_google_appliance_block_form(&$vars) {

  $vars['block_search_form'] = array();
  $hidden = array();

  // Vars named after form keys so themers can print each element independently.
  foreach (element_children($vars['form']) as $key) {

    // token = FALSE on search form means '#type' wont be there
    // put the option check here while we wait and see if the tooken-to-false
    // solution for Varnish caching issue actually solves the problem in the wild
    $type = array_key_exists('#type', $vars['form'][$key]) ? $vars['form'][$key]['#type'] : '';
    if ($type == 'hidden' || $type == 'token') {
      $hidden[] = drupal_render($vars['form'][$key]);
    }
    else {
      $vars['block_search_form'][$key] = drupal_render($vars['form'][$key]);
    }
  }

  // No theming value on hidden elements, so collapse them.
  $vars['block_search_form']['hidden'] = implode($hidden);

  // Collect the whole shebang so the complete form can be rendered with one var.
  $vars['block_search_form_complete'] = implode($vars['block_search_form']);

  // Render template.
}

/**
 * Preprocess the full search form.
 */
function template_preprocess_google_appliance_search_form(&$vars) {

  $vars['search_form'] = array();
  $hidden = array();

  // Vars named after form keys so themers can print each element independently.
  foreach (element_children($vars['form']) as $key) {

    // token = FALSE on search form means '#type' wont be there
    // put the option check here while we wait and see if the tooken-to-false
    // solution for Varnish caching issue actually solves the problem in the wild
    $type = array_key_exists('#type', $vars['form'][$key]) ? $vars['form'][$key]['#type'] : '';
    if ($type == 'hidden' || $type == 'token') {
      $hidden[] = drupal_render($vars['form'][$key]);
    }
    else {
      $vars['search_form'][$key] = drupal_render($vars['form'][$key]);
    }
  }

  // No theming value on hidden elements, so collapse them.
  $vars['search_form']['hidden'] = implode($hidden);

  // Collect the whole shebang so the complete form can be rendered with one var.
  $vars['search_form_complete'] = implode($vars['search_form']);

  // Render template.
}

/**
 * Preprocess google-search-appliance-results.tpl.php (results page).
 */
function template_preprocess_google_appliance_results(&$vars) {

  // Grab module settings.
  $settings = _google_appliance_get_settings();

  // Report debug info to admins if module settings.
  if ($vars['is_admin'] && $settings['query_inspection'] == '1') {
    if (isset($vars['search_query_data']['debug_info'])) {
      foreach ($vars['search_query_data']['debug_info'] as $info_slice) {
        drupal_set_message(filter_xss_admin($info_slice));
      }
    }
  }

  // Grab the search form.
  $vars['search_form'] = drupal_get_form(
    'google_appliance_search_form',
    $vars['search_query_data']['gsa_query_params']['q']
  );

  // Get spelling suggestion.
  $vars['spelling_suggestion'] = $settings['spelling_suggestions'] && isset($vars['response_data']['spelling_suggestion'])
   ? theme('google_appliance_spelling_suggestion', $vars['response_data'])
   : '';

  // If we have errors, decode them and skip building results entities.
  if (isset($vars['response_data']['error'])) {

    $vars['results_heading'] = t('No Results Found');
    $vars['error_reason'] = '';
    $log_message_errors = '';

    // Build hook_help-based error messages for display.
    foreach ($vars['response_data']['error'] as $error_key => $error_response) {
      // Replace error responses with those configured.
      $help_message = check_markup($settings['error_' . $error_key], $settings['error_' . $error_key . '_format']);
      $vars['error_reason'] .= ($help_message != '') ? $help_message : $error_response;

      // Build error message for the log (ignore 'no results' condition).
      if ($error_key != 'gsa_no_results') {
        $log_message_errors .= '{ ' . $error_response . ' } ';
      }
    }

    // Report communication errors to the log.
    if ($log_message_errors != '') {
      _google_appliance_log_search_error(
        $vars['search_query_data']['gsa_query_params']['q'],
        $log_message_errors
      );
    }

  }
  else {
    // Build results entities.
    $vars['results_heading'] = t('Search Results');

    // Get themed sort headers.
    $vars['sort_headers'] = theme(
      'google_appliance_sort_headers', $vars['search_query_data']
    );

    // Get themed keymatch listing.
    $vars['keymatch_results'] = '';

    // GSA device may not be configured for keymatches
    if (array_key_exists('keymatch', $vars['response_data'])) {
      $count = 0;
      foreach ($vars['response_data']['keymatch'] as $keymatch) {
        $keymatch['keymatch_idx'] = ++$count;
        $vars['keymatch_results'] .= theme('google_appliance_keymatch', $keymatch);
      }
    }

    // Get themed synonym listing.
    $vars['synonyms'] = '';
    $vars['show_synonyms'] = FALSE;

    // GSA device may not be configured for synonyms
    if (array_key_exists('synonyms', $vars['response_data'])) {
      $count = 0;
      foreach ($vars['response_data']['synonyms'] as $synonym) {
        $synonym['synonym_idx'] = ++$count;
        $vars['synonyms'] .= theme('google_appliance_synonym', $synonym);
      }
      $vars['show_synonyms'] = $count;
    }

    // Get themed results listing.
    $vars['search_results'] = '';
    $count = 0;
    foreach ($vars['response_data']['entry'] as $result) {
      $result['result_idx'] = ++$count;
      $vars['search_results'] .= theme('google_appliance_result', $result);
    }

    // Get themed search stats.
    $search_stats_data = array(
      'response_data' => $vars['response_data'],
      'search_query_data' => $vars['search_query_data']
    );
    $vars['search_stats'] = theme('google_appliance_search_stats', $search_stats_data);

    // Get themed drupal pager.
    $pager_data = array(
      'total_results_count' => $vars['response_data']['total_results'],
      'last_result_index' => $vars['response_data']['last_result'],
    );
    $vars['pager'] = theme('google_appliance_pager', $pager_data);

    // Add Advanced Search Reporting if applicable.
    if ($settings['advanced_search_reporting']) {
      // Path to callback.
      // When clean URLs are disabled we do not want the '?q=' version which won't work.
      // Hence we do not use url() -- we build the path ourselves from $base_url.
      global $base_url;
      $callback_path = $base_url . '/' . drupal_get_path('module', 'google_appliance') . '/google_appliance.callback.click.php';
      // Add javascript.
      drupal_add_js(array(
        'google_appliance' => array(
          'clickTracking' => array(
            'host' => $callback_path,
            'pathPart' => '',
            'collection' => $vars['search_query_data']['gsa_query_params']['site'],
            'query' => $vars['search_query_data']['gsa_query_params']['q'],
            'start' => $vars['search_query_data']['gsa_query_params']['start'],
            'clickTypes' => array(
              'c' => '.google-appliance-result a',
              'cluster' => '#block-google-appliance-ga-related-searches .content a',
              'logo' => 'a#logo',
              'nav.page' => '.content .pager-item a',
              'nav.next' => '.content .pager-next a',
              'nav.prev' => '.content .pager-previous a, .content .pager-first a',
              'keymatch' => '.google-appliance-keymatch-results a',
              'onebox' => '.google-appliance-onebox-module a',
              'sort' => '.google-appliance-sorter a',
              'synonym' => '.google-appliance-synonym a',
              'spell' => '.google-appliance-spelling-suggestion a',
            ),
          ),
        ),
      ), 'setting');
      drupal_add_library('google_appliance', 'jquery.gsa-clicks');
      drupal_add_js(drupal_get_path('module', 'google_appliance') . '/js/clicklog.js');
    }
  }

  // Render template.
}

/**
 * Preprocess a spelling suggestion.
 */
function template_preprocess_google_appliance_spelling_suggestion(&$vars) {
  $menu_item = menu_get_item();

  // Pop the existing search query to get the base path for the search results.
  array_pop($menu_item['original_map']);
  $search_base = implode('/', $menu_item['original_map']);
  $query_plain = filter_xss($vars['spelling_suggestion'], array());

  $link = l($vars['spelling_suggestion'], $search_base . '/' . $query_plain, array(
    'html' => TRUE,
  ));
  $vars['spelling_suggestion'] = t('Did you mean !linked_suggestion?', array('!linked_suggestion' => $link));

  // Render template.
}

/**
 * Preprocess a single search result.
 */
function template_preprocess_google_appliance_result(&$vars) {

  // Sanatize urls.
  $vars['short_url'] = check_url($vars['short_url']);
  $vars['enc_url'] = check_url($vars['enc_url']);
  $vars['abs_url'] = check_url($vars['abs_url']);

  // if the result has a mime type value, generate file icon image
  // that themes can optionally display
  if ( $vars['mime']['type'] != '' ) {
    $file = new stdClass;
    $file->filemime = $vars['mime']['type'];
    $vars['mime']['icon'] = theme('file_icon', array(
      'file' => $file,
      'icon_directory' => variable_get('file_icon_directory', drupal_get_path('module', 'file') . '/icons'),
      'alt' => $vars['mime']['type']
    ));
  }

  // Sanatize snippet and title ...
  // Allow boldface through for keywork highlighting.
  $vars['snippet'] = filter_xss($vars['snippet'], array('b', 'strong'));
  $vars['title'] = filter_xss($vars['title'], array('b', 'strong'));

  // Sanitize crawl date.
  $vars['crawl_date'] = check_plain($vars['crawl_date']);

  // Add a google-appliance-levelx class if we're sub-levels
  if ((int) $vars['level'] > 1) {
    $vars['classes_array'][] = 'google-appliance-level' . $vars['level'];
  }

  // render template
}

/**
 * Preprocess a single keymatch.
 */
function template_preprocess_google_appliance_keymatch(&$vars) {

  // Sanatize urls.
  $vars['url'] = check_url($vars['url']);
  $vars['description'] = filter_xss($vars['description'], array('b', 'strong'));

  // Render template.
}

/**
 * Preprocess a single synonym.
 */
function template_preprocess_google_appliance_synonym(&$vars) {
  $settings = _google_appliance_get_settings();
  $description = filter_xss($vars['description'], array('b', 'strong'));
  $url = check_url($settings['drupal_path'] . '/' . $vars['url']);
  $vars['link'] = l($description, $url, array('attributes' => array('class' => array('synonym-description'))));
}

/**
 * Preprocess a single Onebox module.
 */
function template_preprocess_google_appliance_onebox_module(&$vars) {
  // Render results.
  $vars['results'] = array();
  foreach ($vars['onebox']['results'] as $key => $result) {
    $vars['results'][$key]['rendered'] = theme('google_appliance_onebox_result', array('result' => $result));
  }

  // Provide top-level variables for the onebox.
  $vars['module_name'] = $vars['onebox']['module_name'];
  $vars['provider'] = $vars['onebox']['provider'];
  $vars['url_text'] = $vars['onebox']['url_text'];
  $vars['url_link'] = $vars['onebox']['url_link'];
  $vars['image'] = $vars['onebox']['image'];
  $vars['description'] = $vars['onebox']['description'];
}

/**
 * Preprocess a single Onebox result.
 */
function template_preprocess_google_appliance_onebox_result(&$vars) {
  // Combine <U> and <Title> to build a linked title.
  $title_linked = empty($vars['result']['title']) ? '' : $vars['result']['title'];
  $title_linked = empty($vars['result']['abs_url']) ? $title_linked : l($title_linked, $vars['result']['abs_url']);

  // Provide top-level variables for this onebox result.
  $vars['abs_url'] = $vars['result']['abs_url'];
  $vars['title'] = $vars['result']['title'];
  $vars['title_linked'] = $title_linked;
  $vars['fields'] = $vars['result']['fields'];
}


/**
 * Set up Drupal pager for pagination of GSA-provided search results.
 *
 * We're not paging druppal-database content in this version, so the core
 * pager is not aware of the page-able results provided by the GSA ...
 * We're only requesting one page of the results at a time, but we get
 * enough stats from that query to fake the pager, and provide a familiar
 * interface.
 */
function theme_google_appliance_pager(&$vars) {

  // Grab module settings.
  $settings = _google_appliance_get_settings();

  // Globals required to manually configure the pager.
  global $pager_page_array, $pager_total, $pager_total_items;

  $control_tags = array();  // default labels
  $element = 0;
  $limit = $settings['results_per_page'];

  // Total # of pages in list.
  $total_pages = ceil($vars['total_results_count'] / $limit);
  /**
   * NOTE: the total results count from the GSA is unreliable. The docs
   * state that it is an *approximation*, but if you click around on
   * enough searches, you'll find that it's off by a considerable amount -
   * enough to break the math used to create pager links. It's not all that
   * noticeable unless you click on the "last" link in the pager, and
   * notice that in some searches, the last page is paginated at a number
   * less than was previously viewable when the first page of results came
   * up on the initial search view.
   *
   * The problem is rooted in public vs. access-controlled indexing.
   * @see groups.google.com/group/Google-Search-Appliance-Help/browse_thread/thread/019b77fb3e7950c7
   * Access-controled results are counted before we know if we can actually
   * view it. Device configuration can help.
   *
   * @TODO: perhaps a better solution is to just query the device for the
   * first X (up to 1000) results and cache them locally.
   */

  // Manually configure (fake) the pager.
  $page = ( isset($_GET['page']) ) ? check_plain($_GET['page']) : 0;

  // Convert page id to array.
  $pager_page_array = explode(',', $page);

  // Set the total results.
  $pager_total_items[$element] = (int)$vars['total_results_count'];

  // Set the total # of pages.
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min((int)$pager_page_array[$element], ((int)$pager_total[$element]) - 1));

  return theme('pager');
}

/**
 * Preprocess google-appliance-sort-headers.tpl.php.
 */
function template_preprocess_google_appliance_sort_headers(&$vars) {
  $settings = _google_appliance_get_settings();

  // Possible sort options.
  $vars['sort_options'] = array(
    array(  // default sort
      'sort' => 'rel',
      'label' => t('Relevance'),
      'gsa_key' => '',
    ),
    array(
      'sort' => 'date',
      'label' => t('Date'),
      'gsa_key' => 'date:D:S:d1'
    ),
    /*
    array(
      'sort' => 'date_asc',
      'label' => t('Oldest First'),
      'gsa_key' => 'date:A:S:d1',
    )
    */
  );

  // Figure out which page we are on.
  $cur_page = 0;
  if (isset($_GET['page']) ) {
    $cur_page = check_plain($_GET['page']);
  }


  $vars['sorters'] = array();
  $sort_idx = 0;
  $sort_found = FALSE;
  foreach ($vars['sort_options'] as $sort_opt ) {

    // If we haven't yet found the current sort, look for params again.
    $sort_is_current = FALSE;
    $sort_param = $vars['gsa_query_params']['sort'];
    // Was a sort requested in the query?
    if ((!$sort_found) && ($sort_param != '')) {

      // Is the sort param one of the valid ones?
      if (($sort_param == $sort_opt['gsa_key'])) {
        $sort_is_current = TRUE;
        $sort_found = TRUE;
      }
    }
    elseif ((!$sort_found) && ($sort_param == '')) {
      // We had no sort parameters.
      if ($sort_opt['sort'] == 'rel') {
        // And we are looking at the default sort.
        $sort_is_current = TRUE;
        $sort_found = TRUE;
      }
    }

    // If we are on the current sort, then we just create non-linked text
    // for the sort label.
    if ($sort_is_current) {
      $vars['sorters'][$sort_idx]['display'] = '<span class="active-sort">' . $sort_opt['label'] . '</span>';
    }
    else {
      // We're not on the current sort, so create a link.
      $link_attributes = array();
      $link_attributes['query'] = array(
        'page' => $cur_page,
      );

      // Append the query to the base url of the serach page.
      $link_path = $settings['drupal_path'] . '/' . $vars['gsa_query_params']['q'];

      // Append the sort param if any.
      $link_path .= '/' . $sort_opt['sort'];

      $vars['sorters'][$sort_idx]['display'] = l($sort_opt['label'], $link_path, $link_attributes);
    }
    $sort_idx++;
  }

  // Render template.
}

/**
 * Preprocess google-appliance-search-stats.tpl.php.
 */
function template_preprocess_google_appliance_search_stats(&$vars) {

  $vars['stat_entries'] = array(
    '@first' => $vars['response_data']['last_result'] - count($vars['response_data']['entry']) + 1,
    '@last' => $vars['response_data']['last_result'],
    '%query' => urldecode($vars['search_query_data']['gsa_query_params']['urlencoded_q']),
    '@total' => $vars['response_data']['total_results'],
    // The total results reported by the GSA is unreliable, display at your own risk
    // @see: theme_google_appliance_pager
  );

  // render
}

/**
 * Implements hook_preprocess_block().
 *
 * Used to target onebox links for advanced search click tracking.
 */
function google_appliance_preprocess_block(&$vars) {
  if (isset($vars['elements']['#onebox'])) {
    $vars['classes_array'][] = 'google-appliance-onebox-module';
  }
}
