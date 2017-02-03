<?php

/**
 * @file
 * Variable API hooks and functions for the Google Search Appliance module.
 */


/**
 * Implements hook_variable_group_info().
 */
function google_appliance_variable_group_info() {
  $groups['google_appliance'] = array(
    'title' => t('Google Search Appliance'),
    'description' => t('Google Search Appliance settings and configuration'),
    'access' => 'administer_google_appliance',
    'path' => array('admin/config/search/google_appliance/settings'),
  );

  return $groups;
}


/**
 * Implements hook_variable_info().
 */
function google_appliance_variable_info($options) {
  // Appliance hostname.
  $variables['google_appliance_hostname'] = array(
    'title' => t('Google Search Appliance Host Name', array(), $options),
    'description' => t('Valid URL or IP address of the GSA device, including <em>http://</em> or <em>https://</em>. Do <b>not</b> include <em>/search</em> at the end, or a trailing slash, but you should include a port number if needed. Example: <em>http://my.gsabox.com:8443</em>', array(), $options),
    'type' => 'string',
    'default' => SGA_DEFAULT_HOSTNAME,
    'required' => TRUE,
    'group' => 'google_appliance',
  );

  // Default collection.
  $variables['google_appliance_collection'] = array(
    'title' => t('Collection', array(), $options),
    'description' => t('The name of a valid collection on the GSA device (case sensitive).', array(), $options),
    'type' => 'string',
    'default' => SGA_DEFAULT_COLLECTION,
    'required' => TRUE,
    'group' => 'google_appliance',
  );

  // Default frontend.
  $variables['google_appliance_frontend'] = array(
    'title' => t('Frontend client', array(), $options),
    'description' => t('The name of a valid frontend client on the GSA device (case sensitive).', array(), $options),
    'type' => 'string',
    'default' => SGA_DEFAULT_FRONTEND,
    'required' => TRUE,
    'group' => 'google_appliance',
  );

  // Request timeout interval.
  $variables['google_appliance_timeout'] = array(
    'title' => t('Search Timeout', array(), $options),
    'description' => t('Length of time to wait for response from the GSA device before giving up (timeout in seconds).', array(), $options),
    'type' => 'number',
    'default' => SGA_DEFAULT_TIMEOUT,
    'required' => TRUE,
    'group' => 'google_appliance',
  );

  // Auto-filtering.
  $variables['google_appliance_autofilter'] = array(
    'title' => t('Search Results Auto-Filtering Options', array(), $options),
    'description' => t('Learn more about GSA auto-filtering <a href="@gsa-doc-af">here</a>. In general, employing both filters enhances results, but sites with smaller indexes may suffer from over-filtered results.', array('@gsa-doc-af' => 'http://code.google.com/apis/searchappliance/documentation/68/xml_reference.html#request_filter_auto'), $options),
    'type' => 'options',
    'options' => array(
      '0' => t('No filtering'),
      's' => t('Duplicate Directory Filter'),
      'p' => t('Duplicate Snippet Filter'),
      '1' => t('Both Duplicate Directory and Duplicate Snippet Filters')
    ),
    'default' => SGA_DEFAULT_AUTOFILTER,
    'group' => 'google_appliance',
  );

  // Language filter settings.
  if (module_exists('locale')) {
    // Toggle to enable/disable language filtering.
    $variables['google_appliance_language_filter_toggle'] = array(
      'title' => t('Enable Language Filtering', array(), $options),
      'type' => 'boolean',
      'default' => SGA_DEFAULT_LANGUAGE_FILTER_TOGGLE,
      'group' => 'google_appliance',
    );

    // Language filter selection
    $variables['google_appliance_language_filter_options'] = array(
      'title' => t('Restrict searches to specified languages', array(), $options),
      'description' => t('If there are no results in the specified language, the search appliance is expected to return results in all languages.', array(), $options),
      'type' => 'options',
      'options' => array(
        '***CURRENT_LANGUAGE***' => t("Current user's language"),
        '***DEFAULT_LANGUAGE***' => t("Default site language"),
      ) + locale_language_list(),
      'default' => SGA_DEFAULT_LANGUAGE_FILTER_OPTIONS,
      'localize' => TRUE,
      'group' => 'google_appliance',
    );
  }

  // Query inspection.
  $variables['google_appliance_query_inspection'] = array(
    'title' => t('Enable Query Inspection', array(), $options),
    'description' => t('Inspect the search query parameters sent to the GSA device in the Drupal message area every time a search is performed. Only really useful for sites not using the Devel module, as dsm() provides more information. The inspector is only shown to administrators, but should be disabled in a production environment.', array(), $options),
    'type' => 'boolean',
    'default' => SGA_DEFAULT_QUERY_INSPECTION,
    'group' => 'google_appliance',
  );

  // Search page path.
  $variables['google_appliance_search_title'] = array(
    'title' => t('Search path', array(), $options),
    'description' => t('The URL of the search page provided by this module. Include neither leading nor trailing slash.', array(), $options),
    'type' => 'string',
    'default' => SGA_DEFAULT_DRUPAL_PATH,
    'required' => TRUE,
    'group' => 'google_appliance',
  );

  // Search page title.
  $variables['google_appliance_search_title'] = array(
    'title' => t('Search Name', array(), $options),
    'description' => t('Serves as the page title on results pages and the default menu item title', array(), $options),
    'type' => 'string',
    'default' => SGA_DEFAULT_SEARCH_TITLE,
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'google_appliance',
  );

  // Results per page.
  $variables['google_appliance_results_per_page'] = array(
    'title' => t('Results per page', array(), $options),
    'description' => t('Number of results to show on the results page. More results will be available via a Drupal pager.', array(), $options),
    'type' => 'number',
    'default' => SGA_DEFAULT_RESULTS_PER_PAGE,
    'required' => TRUE,
    'group' => 'google_appliance',
  );

  // Spelling suggestions.
  $variables['google_appliance_spelling_suggestions'] = array(
    'title' => t('Display Spelling Suggestions', array(), $options),
    'type' => 'boolean',
    'default' => SGA_DEFAULT_SPELLING_SUGGESTIONS,
    'group' => 'google_appliance',
  );

  // Search reporting.
  $variables['google_appliance_advanced_search_reporting'] = array(
    'title' => t('Enable Advanced Search Reporting', array(), $options),
    'description' => t('Learn more about !this_feature. You need to enable Advanced Search Reporting on the front end client. The device should provide a file named "/clicklog_compiled.js" when using the search interface on the GSA.', array('!this_feature' => l(t('this feature'), 'http://www.google.com/support/enterprise/static/gsa/docs/admin/70/gsa_doc_set/xml_reference/advanced_search_reporting.html')), $options),
    'type' => 'boolean',
    'default' => SGA_DEFAULT_ADVANCED_SEARCH_REPORTING,
    'group' => 'google_appliance',
  );

  // Sitelinks search box.
  $variables['google_appliance_sitelinks_search_box'] = array(
    'title' => t('Enable Google.com Sitelinks Search Box', array(), $options),
    'description' => t('Learn more about !this_feature. Note: your site may not necessarily be a candidate for the google.com Sitelinks search box.', array('!this_feature' => l(t('this feature'), 'https://developers.google.com/structured-data/slsb-overview')), $options),
    'type' => 'boolean',
    'default' => SGA_DEFAULT_SITELINKS_SEARCH_BOX,
    'group' => 'google_appliance',
  );

  // Onebox modules.
  $variables['google_appliance_onebox_modules'] = array(
    'title' => t('Onebox modules', array(), $options),
    'description' => t('A list of Onebox modules, one per line. Each module listed will have a corresponding block. These blocks must be placed via the block configuration page, or another layout mechanism like Context or Panels.', array(), $options),
    'type' => 'text',
    'default' => SGA_DEFAULT_ONEBOX_MODULES,
    'localize' => FALSE,
    'group' => 'google_appliance',
  );

  // No results error message.
  $variables['google_appliance_error_gsa_no_results'] = array(
    'title' => t('No results error message', array(), $options),
    'description' => t('The message displayed to the user when no results are found for the given search query.', array(), $options),
    'type' => 'text',
    'default' => SGA_DEFAULT_ERROR_GSA_NO_RESULTS,
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'google_appliance',
  );

  // Connection error message.
  $variables['google_appliance_error_curl_error'] = array(
    'title' => t('Connection error message', array(), $options),
    'description' => t('The message displayed to the user when there is an error connecting to the search appliance.', array(), $options),
    'type' => 'text',
    'default' => SGA_DEFAULT_ERROR_CURL_ERROR,
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'google_appliance',
  );

  // Parse error message.
  $variables['google_appliance_error_lib_xml_parse_error'] = array(
    'title' => t('XML parse error message', array(), $options),
    'description' => t('The message displayed to the user when the XML returned by the appliance is malformed.', array(), $options),
    'type' => 'text',
    'default' => SGA_DEFAULT_ERROR_LIB_XML_PARSE_ERROR,
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'google_appliance',
  );

  return $variables;
}
