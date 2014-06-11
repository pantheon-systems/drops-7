<?php

/**
 * @file
 * Lingotek Dashboard.
 */

/**
 * Tab:  Dashboard - The main Lingotek dashboard page.
 */
function lingotek_dashboard() {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance(); //remove?

  if (lingotek_is_config_missing() === TRUE) {
    $output = array();
    // Check the login ID to see if this is a Lingopoint user, if so, provide information about setting up their oauth keys.
    $login_id = variable_get('lingotek_login_id', 'community_admin');

    if ($login_id != 'community_admin') {
      drupal_set_message(t('The Lingotek Translation module is not fully configured.'), 'warning');
      $output['instructions'] = array(
        '#type' => 'markup',
        '#markup' => theme('item_list', array(
          'title' => 'Complete configuration by performing the following:',
          'type' => 'ol',
          'items' => array(
            array('data' => t('Login to the') . ' ' . l(t('Lingotek account dashboard'), LINGOTEK_API_SERVER, array('attributes' => array('target' => '_blank')))),
            array('data' => t('Select the <i>Community &gt; Integrations</i> menu item')),
            array('data' => t('Setup an <i>Inbound OAuth integration</i> method to create an <i>OAuth Key</i> and <i>OAuth Secret</i>')),
            array('data' => t('Copy and paste the <i>OAuth Key</i> and <i>OAuth Secret</i> that were created into the') . ' ' . l(t('Lingotek Drupal module settings'), LINGOTEK_MENU_LANG_BASE_URL . '/settings', array('query' => array('edit_connection' => 1))))
          )
        ))
      );
    }
    else { // Show the standard misconfiguration message.
      drupal_set_message(t('The Lingotek Translation module is not fully configured. To complete setup, run the') . ' ' . l(t('Lingotek Configuration Wizard'), 'admin/config/lingotek/setup'), 'warning');
    }
    $output[] = lingotek_support_footer();
    return $output;
  }

  return array(
    'script' => array(
      '#type' => 'markup',
      '#markup' => lingotek_get_dashboard_code() // Generates the dashboard javascript.
    )
  );
  return array(
    'script' => array(
      '#type' => 'markup',
      '#markup' => lingotek_get_dashboard_code() // Generates the dashboard javascript.
    ),
  );
}

// END:  lingotek_dashboard()

/**
 * Generates the code for the embedded Javascript dashboard.
 */
function lingotek_get_dashboard_code() {
  global $base_url, $language;

  $output = '';
  $guid = lingotek_get_guid(); // 16 char string

  $script = '
      <script> 
        var cms_data = { 
          // lingotek
          "community": "%s",
          "external_id": "%s",
          "tm_vault_id": "%s",
          "workflow_id": "%s",
          "project_id": "%s",
          "first_name": "%s",
          "last_name": "%s",
          "email": "%s",
          // cms
          "cms_site_id": "%s",
          "cms_site_key": "%s",
          "cms_site_name": "%s",
          "cms_type": "%s",
          "cms_version": "%s",
          "cms_tag": "%s",
          "locale": "%s",
          "module_version": %s,
          "endpoint_url": "%s"
        };
      </script>
      <link rel="stylesheet" href="' . LINGOTEK_GMC_SERVER . '/v2/styles/ltk.css"> 
      <script src="' . LINGOTEK_GMC_SERVER . '/v2/ltk.min.js"></script>  
      <div ltk-dashboard ng-app="LingotekApp"></div>
    ';

  $endpoint = $base_url . '/lingotek/language'; //(variable_get('clean_url', 0) ? '' : '?q=') . '/lingotek/target';
  $sync_url = $base_url . '/' . LINGOTEK_MENU_MAIN_BASE_URL . '/manage/node'; //(variable_get('clean_url', 0) ? '' : '?q=') . '/' . LINGOTEK_MENU_LANG_BASE_URL . '/sync';

  $output .= sprintf($script,
      // lingotek 
      variable_get('lingotek_community_identifier', ''), //community
      variable_get('lingotek_login_id', ''), //external_id
      variable_get('lingotek_vault', ''), //tm_vault_id
      variable_get('lingotek_workflow', ''), //workflow_id
      variable_get('lingotek_project', ''), //project_id
      variable_get('lingotek_activation_first_name', ''), //first_name
      variable_get('lingotek_activation_last_name', ''), //last_name
      variable_get('lingotek_activation_email', ''), //email
      // cms
      $guid, // installation GUID
      url('<front>', array('absolute' => TRUE)), // site url
      variable_get('site_name', 'Unidentified Drupal Site'), // sitename
      'Drupal', // cms type
      VERSION, // drupal version
      variable_get('install_profile', ''), // CMS tag variable_get('lingotek_cms_tag','')
      isset($language->lingotek_locale) ? $language->lingotek_locale : Lingotek::convertDrupal2Lingotek($language->language, FALSE), // locale requested,
      json_encode(lingotek_get_module_info('version')), // module version --- get all module info:  lingotek_get_module_info()
      $endpoint
  );
  return $output;
}

// END:  lingotek_get_dashboard_code()

/**
 * Ajax Command Processing for the Lingotek dashboard.
 */
function lingotek_dashboard_command_ajax() {

  $parameters = array();
  $message = 'Not doing anything.';
  $method = $_SERVER['REQUEST_METHOD'];

  $response = array();

  switch ($method) {
    case 'POST': // Add/Insert Target Language
      $message = 'POST: Insert a new Target Language';
      $lingotek_locale = $_POST['code'];
      $parameters = $_POST;
      if (strlen($lingotek_locale) > 1) {
        // Adds the language locale to the local list of languages and to the Lingotek project (via the API).
        $add = lingotek_add_target_language($lingotek_locale);
        $response = lingotek_get_language_details($lingotek_locale);
        $response['tms_added'] = $add;
      }
      else {
        $response['error'] = "locale code required";
      }
      break;
    case 'DELETE': // Remove/Delete Target Language
      // Initiate Language Delete
      parse_str(file_get_contents("php://input"), $parameters);
      $lingotek_locale = $parameters['code'];
      $message = t('DELETE: remove language (@code)', array('@code' => $lingotek_locale));

      // Removes the language to the local list of languages we do translation for, and also removes the target language from the Lingotek project.
      $delete = lingotek_delete_target_language($lingotek_locale); // Mark this language as Active => False
      $response = array(
        'code' => $lingotek_locale,
        'active' => FALSE,
        'success' => $delete
      ); // Format:  {'code':'es','active':'FALSE','success':TRUE}
      break;
    case 'GET': // 4. Retrieve the Target Languages and Status
    default:
      // Retrieve the Target Languages and Status
      $message = 'GET: retrieve the details of the locale(s)';
      $parameters = $_GET;
      $response = isset($_GET['code']) ? lingotek_get_language_details($_GET['code']) : lingotek_get_language_details();
      break;
  }
  //lingotek_json_output_cors(array('response' => $response, 'message' => $message, 'data' => $data, 'method' => $method  ));
  $general = array('request' => $message);
  $full_response = array_merge($general, $response);
  if ($method !== 'GET') { //skip logging GETS (since they occur frequently)
    LingotekLog::info('<h2>Target - @method</h21>
      <h3>Parameters:</h3>
      <div>!parameters</div>
      <h3>Response:</h3>
      <div>!response</div>
      ', array(
      '@method' => $method,
      '!parameters' => ($parameters),
      '!response' => ($response)
        ), 'endpoint');
  }

  return lingotek_json_output_cors($full_response);
}

/**
 * Get the details of each language
 */
function lingotek_get_language_details($lingotek_locale_requested = NULL) {

  $response = array();
  $available_languages = Lingotek::getLanguages(NULL, TRUE);
  $source_total = 0;
  $target_total = 0;
  $source_totals = array();
  $target_totals = array();
  // If we get a parameter, only return that language. Otherwise return all languages.
  foreach ($available_languages as $l) {
    if (!is_null($lingotek_locale_requested) && $lingotek_locale_requested != $l->lingotek_locale)
      continue;
    $drupal_language_code = $l->language;
    $lingotek_locale = $l->lingotek_locale;
    $source_counts = LingotekSync::getSourceCounts($lingotek_locale);
    $source_count = $source_counts['total']; //unset($source_counts['total']);
    $target_statuses_to_include = array(LingotekSync::STATUS_CURRENT);
    $target_counts = LingotekSync::getCountsByStatus($target_statuses_to_include, $lingotek_locale);
    $target_count = $target_counts['total'];

    $target_status = array(
      'locale' => $lingotek_locale, // Return this language code as the Lingotek language code.
      'dcode' => $drupal_language_code,
      'active' => intval($l->lingotek_enabled), //lingotek_enabled,
      'enabled' => intval($l->enabled), // drupal enabled
      'source' => $source_counts,
      'target' => $target_counts
    );
    if ($lingotek_locale_requested == $l->lingotek_locale) {
      $response = $target_status;
    }
    else {
      $response[$lingotek_locale] = $target_status;
    }
    $source_total += $source_count;
    $target_total += $target_count;
    $source_totals = LingotekSync::arraySumValues($source_totals, $source_counts['types']);
    $target_totals = LingotekSync::arraySumValues($target_totals, $target_counts['types']);
  }
  if (is_null($lingotek_locale_requested)) {
    $response = array(
      'languages' => $response,
      'source' => array('types' => $source_totals, 'total' => $source_total),
      'target' => array('types' => $target_totals, 'total' => $target_total),
      'count' => count($available_languages),
    );
  }
  return $response;
}

/**
 * Generates Lingotek GUID for this installation.
 */
function lingotek_get_guid() {
  $guid = variable_get('lingotek_guid', '');
  if ($guid == '') {
    $guid = substr(hash('sha256', drupal_get_hash_salt()), 0, 16);
    variable_set('lingotek_guid', $guid);
  }
  return $guid;
}
