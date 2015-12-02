<?php
/**
 * @file
 *    admin callbacks for the Google Appliance module
 */

/**
 * Implements hook_admin_settings().
 *    displays the Search Google Appliance module settings page.
 *
 * @ingroup forms
 * @see google_appliance.helpers.inc
 * @see google_appliance_admin_settings_validate()
 * @see google_appliance_admin_settings_submit()
 */
function google_appliance_admin_settings($form) {

  // grab current settings
  $settings = _google_appliance_get_settings();

  // connection information
  $form['connection_info'] = array(
    '#title' => t('Connection Information'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['connection_info']['hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Search Appliance Host Name'),
    '#description' => t('Valid URL or IP address of the GSA device, including <em>http://</em> or <em>https://</em>. Do <b>not</b> include <em>/search</em> at the end, or a trailing slash, but you should include a port number if needed. Example: <em>http://my.gsabox.com:8443</em>'),
    '#default_value' => $settings['hostname'],
    '#required' => TRUE,
  );
  $form['connection_info']['collection'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection'),
    '#description' => t('The name of a valid collection on the GSA device (case sensitive).'),
    '#default_value' => $settings['collection'],
    '#required' => TRUE,
  );
  $form['connection_info']['frontend'] = array(
    '#type' => 'textfield',
    '#title' => t('Frontend client'),
    '#description' => t('The name of a valid frontend client on the GSA device (case sensitive).'),
    '#default_value' => $settings['frontend'],
    '#required' => TRUE,
  );
  $form['connection_info']['timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Search Timeout'),
    '#description' => t('Length of time to wait for response from the GSA device before giving up (timeout in seconds).'),
    '#default_value' => $settings['timeout'],
    '#required' => TRUE,
  );

  // search query parameter configuration
  $form['query_param'] = array(
    "#title" => t("Search Query Parameter Setup"),
    "#type" => "fieldset",
    "#collapsible" => TRUE,
    "#collapsed" => FALSE,
  );
  $form['query_param']['autofilter'] = array(
    '#type' => 'select',
    '#title' => t('Search Results Auto-Filtering Options'),
    '#default_value' => $settings['autofilter'],
    '#options' => array(
      '0' => t('No filtering'),
      's' => t('Duplicate Directory Filter'),
      'p' => t('Duplicate Snippet Filter'),
      '1' => t('Both Duplicate Directory and Duplicate Snippet Filters')
    ),
    '#description' => t('Learn more about GSA auto-filtering <a href="@gsa-doc-af">here</a>. In general, employing both filters enhances results, but sites with smaller indexes may suffer from over-filtered results.', array('@gsa-doc-af' => 'http://code.google.com/apis/searchappliance/documentation/68/xml_reference.html#request_filter_auto') ),
  );
  if (module_exists('locale')) {
    $form['query_param']['google_appliance_language_filter_toggle'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Language Filtering'),
      '#default_value' => $settings['language_filter_toggle'],
    );
    $form['query_param']['google_appliance_language_filter_options'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Restrict searches to specified languages'),
      '#default_value' => $settings['language_filter_options'],
      '#options' => array(
        '***CURRENT_LANGUAGE***' => t("Current user's language"),
        '***DEFAULT_LANGUAGE***' => t("Default site language"),
      ) + locale_language_list(),
      '#states' => array(
        'visible' => array(
          ':input[name=google_appliance_language_filter_toggle]' => array('checked' => TRUE),
        ),
      ),
      '#description' => t('If there are no results in the specified language, the search appliance is expected to return results in all languages.'),
    );
  }
  $form['query_param']['query_inspection'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Query Inspection'),
    '#description' => t('Inspect the search query parameters sent to the GSA device in the Drupal message area every time a search is performed. Only really useful for sites not using the Devel module, as dsm() provides more information. The inspector is only shown to administrators, but should be disabled in a production environment.'),
    '#default_value' => $settings['query_inspection'],
  );

  // search interface settings
  $form['display_settings'] = array(
    '#title' => t('Search Interface Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['display_settings']['search_title'] = array(
    '#title' => t('Search Name'),
    '#type' => 'textfield',
    '#default_value' => $settings['search_title'],
    '#description' => t('Serves as the page title on results pages and the default menu item title'),
    '#required' => TRUE,
  );
  $form['display_settings']['results_per_page'] = array(
    '#title' => t('Results per page'),
    '#type' => 'textfield',
    '#default_value' => $settings['results_per_page'],
    '#description' => t('Number of results to show on the results page. More results will be available via a Drupal pager.'),
    '#required' => TRUE,
  );
  $form['display_settings']['spelling_suggestions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display Spelling Suggestions'),
    '#default_value' => $settings['spelling_suggestions'],
  );
  $form['display_settings']['advanced_search_reporting'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Advanced Search Reporting'),
    '#default_value' => $settings['advanced_search_reporting'],
    '#description' => t('Learn more about !this_feature. You need to enable Advanced Search Reporting on the front end client. The device should provide a file named "/clicklog_compiled.js" when using the search interface on the GSA.', array('!this_feature' => l(t('this feature'), 'http://www.google.com/support/enterprise/static/gsa/docs/admin/70/gsa_doc_set/xml_reference/advanced_search_reporting.html'))),
  );
  $form['display_settings']['google_appliance_onebox_modules'] = array(
    '#type' => 'textarea',
    '#title' => t('Onebox modules'),
    '#description' => t('A list of Onebox modules, one per line. Each module listed will have a corresponding block. These blocks must be placed via the block configuration page, or another layout mechanism like Context or Panels.'),
    '#default_value' => implode("\n", $settings['onebox_modules']),
  );
  $form['#submit'][] = 'google_appliance_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Implements hook_admin_settings_validate().
 *    form validation beyond FAPI '#required'
 */
function google_appliance_admin_settings_validate($form, &$form_state) {

  // host name should be a valid-format URL or IPv4 address including 'http(s)://'
  if ( filter_var($form_state['values']['hostname'], FILTER_VALIDATE_URL) === FALSE) {
    form_set_error(
      'hostname',
      t('GSA Host name must be a valid-format URL or IPv4 address, including <em>http(s)://</em>. Example: http://my.googlebox.net.')
    );
  }

  // timeout should be a reasonable number seconds
  $timeout_validate_options = array( 'options' => array(
    'max_range' => 30,
    'min_range' => 3,
  ));
  if ( filter_var($form_state['values']['timeout'], FILTER_VALIDATE_INT, $timeout_validate_options) === FALSE ) {
    form_set_error(
      'timeout',
      t('Search Timeout should be an integer from @min - @max, indicating the number of seconds to wait before the search request times out.', array(
        '@min' => $timeout_validate_options['options']['min_range'],
        '@max' => $timeout_validate_options['options']['max_range'],
      ))
    );
  }

  // results per page must be on a range that will be accepted by the device upon querying
  $results_per_page_validate_options = array('options' => array(
    'max_range' => 1000,
    'min_range' => 1
  ));
  if ( filter_var($form_state['values']['results_per_page'], FILTER_VALIDATE_INT, $results_per_page_validate_options) === FALSE ) {
    form_set_error(
      'results_per_page',
      t('Results per page should be an integer from @min - @max.', array(
        '@min' => $results_per_page_validate_options['options']['min_range'],
        '@max' => $results_per_page_validate_options['options']['max_range'],
      ))
    );
  }
}

/**
 * Implements hook_admin_settings_submit().
 *    submit handler for admin settings
 */
function google_appliance_admin_settings_submit($form, &$form_state) {

  $field_keys =  array(
    'hostname',
    'collection',
    'frontend',
    'timeout',
    'autofilter',
    'query_inspection',
    'search_title',
    'spelling_suggestions',
    'results_per_page',
    'advanced_search_reporting',
  );

  // save settings
  foreach ($field_keys as $field) {
    variable_set('google_appliance_' . $field, trim($form_state['values'][$field]));
  }

  // refresh settings getter
  $settings = _google_appliance_get_settings(TRUE);

  // make the 'search_title' setting change take effect right away
  menu_rebuild();
}
