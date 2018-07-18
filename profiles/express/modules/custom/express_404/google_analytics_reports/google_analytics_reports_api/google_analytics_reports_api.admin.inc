<?php

/**
 * @file
 * Admin and OAuth callbacks for Google Analytics Reports API module.
 */

/**
 * Menu callback - admin form for OAuth and other settings.
 */
function google_analytics_reports_api_admin() {
  $form = array();
  $account = google_analytics_reports_api_gafeed();

  // There are no profiles, and we should just leave it at setup.
  if (!$account) {
    $setup_help = t('To access data from Google Analytics you have to create a new project in Google Developers Console.');
    $setup_help .= '<ol>';
    $setup_help .= '<li>' . t('Open !google_developers_console.',
        array(
          '!google_developers_console' => l(t('Google Developers Console'), 'https://console.developers.google.com',
            array('attributes' => array('target' => '_blank'))),
        )) . '</li>';
    $setup_help .= '<li>' . t('Along the toolbar click the pull down arrow and the press <strong>Create a Project</strong> button, enter project name and press <strong>Create</strong>.') . '</li>';
    $setup_help .= '<li>' . t('Click <strong>Enable and manage APIs</strong>.') . '</li>';
    $setup_help .= '<li>' . t('In the search box type <strong>Analytics</strong> and then press <strong>Analytics API</strong>, this opens the API page, press <strong>Enable</strong>.') . '</li>';
    $setup_help .= '<li>' . t('Click on <strong>Go to Credentials</strong>') . '</li>';
    $setup_help .= '<li>' . t('Under <strong>Where will you be calling the API from?</strong> select <strong>Web Browser Javascript</strong> and then select <strong>User Data</strong>') . '</li>';
    $setup_help .= '<li>' . t('Hit <strong>What credentials do I need</strong>, edit the name if necessary.') . '</li>';
    $setup_help .= '<li>' . t('Leave empty <strong>Authorized JavaScript origins</strong>, fill in <strong>Authorized redirect URIs</strong> with <code>@url</code> and press <strong>Create Client ID</strong> button.', array('@url' => url(request_path(), array('absolute' => TRUE)))) . '</li>';
    $setup_help .= '<li>' . t('Type a Product name to show to users and hit <strong>Continue</strong> and then <strong>Done</strong>') . '</li>';
    $setup_help .= '<li>' . t('Click on the name of your new client ID to be shown both the <strong>Client ID</strong> and <strong>Client Secret</strong>.') . '</li>';
    $setup_help .= '<li>' . t('Copy <strong>Client ID</strong> and <strong>Client secret</strong> from opened page to the form below.') . '</li>';
    $setup_help .= '<li>' . t('Press <strong>Start setup and authorize account</strong> in the form below and allow the project access to Google Analytics data.') . '</li>';
    $setup_help .= '</ol>';

    $form['setup'] = array(
      '#type' => 'fieldset',
      '#title' => t('Initial setup'),
      '#description' => $setup_help,
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $form['setup']['client_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Client ID'),
      '#default_value' => variable_get('google_analytics_reports_client_id', ''),
      '#size' => 75,
      '#description' => t('Client ID from your project in Google Developers Console.'),
      '#required' => TRUE,
    );

    $form['setup']['client_secret'] = array(
      '#type' => 'textfield',
      '#title' => t('Client Secret'),
      '#default_value' => variable_get('google_analytics_reports_client_secret', ''),
      '#size' => 30,
      '#description' => t('Client Secret from your project in Google Developers Console'),
      '#required' => TRUE,
    );
    $form['setup']['setup_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Start setup and authorize account'),
      '#submit' => array('google_analytics_reports_api_admin_submit_setup'),
    );
  }

  elseif ($account->isAuthenticated()) {

    // Load profiles list.
    $profile_list = google_analytics_reports_api_profiles_list();

    $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Settings'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $form['settings']['google_analytics_reports_api_profile_id'] = array(
      '#type' => 'select',
      '#title' => t('Reports profile'),
      '#options' => $profile_list['options'],
      '#default_value' => $profile_list['profile_id'],
      '#description' => t('Choose your Google Analytics profile.  The currently active profile is: %profile.', array(
        '%profile' => theme('google_analytics_reports_api_profile_label',
          array(
            'profile' => $profile_list['current_profile'],
            'active' => TRUE,
          )),
      )),
    );

    // Default cache periods.
    $times = array();
    // 1-6 days.
    for ($days = 1; $days <= 6; $days++) {
      $times[] = $days * 60 * 60 * 24;
    }
    // 1-4 weeks.
    for ($weeks = 1; $weeks <= 4; $weeks++) {
      $times[] = $weeks * 60 * 60 * 24 * 7;
    }

    $form['settings']['google_analytics_reports_api_cache_length'] = array(
      '#type' => 'select',
      '#title' => t('Query cache'),
      '#description' => t('The <a href="!link">Google Analytics Quota Policy</a> restricts the number of queries made per day. This limits the creation of new reports on your site.  We recommend setting this cache option to at least three days.', array(
        '!link' => url('https://developers.google.com/analytics/devguides/reporting/core/v3/limits-quotas', array(
          'fragment' => 'core_reporting',
        )),
      )),
      '#options' => drupal_map_assoc($times, 'format_interval'),
      '#default_value' => variable_get('google_analytics_reports_api_cache_length', 259200),
    );
    $form['settings']['settings_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
      '#submit' => array('google_analytics_reports_api_admin_submit_settings'),
    );
    $form['revoke'] = array(
      '#type' => 'fieldset',
      '#title' => t('Revoke access and logout'),
      '#description' => t('Revoke your access token from Google Analytics. This action will log you out of your Google Analytics account and stop all reports from displaying on your site.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['revoke']['revoke_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Revoke access token'),
      '#submit' => array('google_analytics_reports_api_admin_submit_revoke'),
    );
  }

  return $form;
}

/**
 * Google Analytics reports profiles for current authorized user.
 *
 * @return array/null
 *   An associative array containing:
 *   - options: list of current available profiles.
 *   - profile_id: current default profile id.
 *   - current_profile: current default profile object.
 */
function google_analytics_reports_api_profiles_list() {
  $account = google_analytics_reports_api_gafeed();
  if (($account) && ($account->isAuthenticated())) {
    $web_properties = $account->queryWebProperties()->results->items;
    $profiles = $account->queryProfiles()->results->items;
    $options = array();
    $profile_id = variable_get('google_analytics_reports_api_profile_id', 0);
    $google_analytics_account = variable_get('googleanalytics_account');
    $set_default = FALSE;

    // Add optgroups for each web property.
    if (!empty($profiles)) {
      foreach ($profiles as $profile) {
        $web_property = NULL;
        foreach ($web_properties as $web_property_value) {
          if ($web_property_value->id == $profile->webPropertyId) {
            $web_property = $web_property_value;
            break;
          }
        }

        $options[$web_property->name][$profile->id] = theme('google_analytics_reports_api_profile_label', array('profile' => $profile));
        // Find current site in the account list.
        if (empty($profile_id)) {
          // If Google Analytics module is enabled check it first.
          if (isset($google_analytics_account) && ($google_analytics_account == $profile->webPropertyId)) {
            $profile_id = $profile->id;
            $set_default = TRUE;
          }
          // Rough attempt to see if the current site is in the account list.
          elseif (parse_url($web_property->websiteUrl, PHP_URL_HOST) == $_SERVER['HTTP_HOST']) {
            $profile_id = $profile->id;
            $set_default = TRUE;
          }
        }
      }
    }

    // If no profile ID is set yet, set the first profile in the list.
    if (empty($profile_id)) {
      $profile_id = key($options[key($options)]);
      $set_default = TRUE;
    }

    if ($set_default) {
      variable_set('google_analytics_reports_api_profile_id', $profile_id);
    }

    $current_profile = NULL;

    // Load current profile object.
    foreach ($profiles as $profile) {
      if ($profile->id == $profile_id) {
        $current_profile = $profile;
        variable_set('google_analytics_reports_api_default_page', isset($current_profile->defaultPage) ? '/' . $current_profile->defaultPage : '/');
        break;
      }
    }
    $return = array(
      'options' => $options,
      'profile_id' => $profile_id,
      'current_profile' => $current_profile,
    );

    return $return;
  }
}

/**
 * Start authorization process with Google Analytics API.
 */
function google_analytics_reports_api_admin_submit_setup(&$form, &$form_state) {
  $client_id = $form_state['values']['client_id'];
  $client_secret = $form_state['values']['client_secret'];
  $redirect_uri = GoogleAnalyticsReportsApiFeed::currentUrl();
  variable_set('google_analytics_reports_api_client_id', $client_id);
  variable_set('google_analytics_reports_api_client_secret', $client_secret);
  variable_set('google_analytics_reports_api_redirect_uri', $redirect_uri);
  $gafeed = new GoogleAnalyticsReportsApiFeed();
  $gafeed->beginAuthentication($client_id, $redirect_uri);
}

/**
 * Save Google Analytics Reports API settins.
 */
function google_analytics_reports_api_admin_submit_settings(&$form, &$form_state) {
  variable_set('google_analytics_reports_api_profile_id', $form_state['values']['google_analytics_reports_api_profile_id']);
  variable_set('google_analytics_reports_api_cache_length', $form_state['values']['google_analytics_reports_api_cache_length']);
  drupal_set_message(t('Settings have been saved successfully.'));
}

/**
 * Revoke Google Analytics access token.
 */
function google_analytics_reports_api_admin_submit_revoke(&$form, &$form_state) {
  google_analytics_reports_api_revoke();
  drupal_set_message(t('Access token has been successfully revoked.'));
}
