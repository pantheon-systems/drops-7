<?php

/**
 * @file
 * Lingotek Easy Install Process.
 */

/**
 * Routing:  Returning User, New Account or Already Setup
 */
function lingotek_setup() {

  // Check the current settings
  $current_community_identifier = variable_get('lingotek_community_identifier', '');
  $current_oauth_consumer_id = variable_get('lingotek_oauth_consumer_id', '');
  $current_oauth_consumer_secret = variable_get('lingotek_oauth_consumer_secret', '');
  $current_login_id = variable_get('lingotek_login_id', '');
  $current_project = variable_get('lingotek_project', '');
  $current_vault = variable_get('lingotek_vault', '');
  $current_workflow = variable_get('lingotek_workflow', '');

  // Does the install already have connection credentials?
  if ($current_community_identifier == '' || $current_oauth_consumer_id == '' || $current_oauth_consumer_secret == '' || $current_login_id == '' || $current_project == '' || $current_vault == '' || $current_workflow == '') {
    drupal_goto('admin/config/lingotek/new-account'); // If something is missing - Go though the Setup Process
  }
  else {
    drupal_goto('admin/lingotek'); // We already have credentials, goto the Dashboard
  }
}

// END:  Router

/**
 * New Account - Form
 */
function lingotek_setup_new_account_form() {

  $current_login_id = variable_get('lingotek_login_id', '');
  $current_login_key = variable_get('lingotek_login_key', '');
  $current_first_name = variable_get('lingotek_activation_first_name', '');
  $current_last_name = variable_get('lingotek_activation_last_name', '');
  $current_email = variable_get('lingotek_activation_email', '');

  $form = array();

  $form['description'] = array(
    '#type' => 'item',
    '#title' => 'Create Account',
    '#description' => 'New to Lingotek?  Create a free account. <p>A Lingotek account is required to process your language translations.  <strong>All fields are required.</strong></p>'
  );

  $form['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => $current_first_name,
    '#size' => 30,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => $current_last_name,
    '#size' => 30,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email address'),
    '#default_value' => $current_email,
    '#size' => 40,
    '#maxlength' => 128,
    '#required' => TRUE,
  );

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next') // Create Account
  );
  $form['lingotek_back_button'] = lingotek_setup_link('admin/config/lingotek/account-settings', t('Already have an account?'));
  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

// END:  New Account - Form

/**
 * New Account - Form Validator
 */
function lingotek_setup_new_account_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['email'])) {
    form_set_error('', t('You must enter a valid Email address.'));
  }
}

/**
 * Identify lead details 
 */
function lingotek_identify_lead($data) {
  global $base_url;

  $info = array(
    'first_name' => '',
    'last_name' => '',
    'email' => '',
    'company' => lingotek_get_site_name(), // Website name => company
    'website' => $base_url, // website url
    'type' => LINGOTEK_DEV ? 'QA' : 'Prospect',
    'cms' => 'Drupal',
    'distribution' => '',
    'lead_source' => '',
    'partner' => ''
  );
  $tags = lingotek_identify_tags();
  $lead = array_merge($info, $tags, $data);
  return $lead;
}

/**
 * Identify CMS tags (e.g., distribution channel)
 */
function lingotek_identify_tags() {
  $tags = array();
  $tags['distribution'] = variable_get('install_profile', 'standard'); // This should identify the distribution including:  'standard', 'acquia', 'commons', 'commerce_kickstart'
  $tags['lead_source'] = 'Module';

  switch ($tags['distribution']) {
    case 'commerce_kickstart':
      $tags['partner'] = 'Commerce Guys';
      $tags['lead_source'] = 'Distribution';
      break;
    case 'commons':
      $tags['partner'] = 'Acquia';
      $tags['lead_source'] = 'Distribution';
      break;
    default:
      $tags['partner'] = '';
      break;
  }
  return $tags;
}

/**
 * New Account - Form Processor
 * Provisions a Lingotek account and sends activation notice.
 */
function lingotek_setup_new_account_form_submit($form, &$form_state) {

  global $base_url;
  $extras = array();

  // Store the info so users dont have to retype stuff
  $first_name = $form_state['values']['first_name'];
  $last_name = $form_state['values']['last_name'];
  $email = $form_state['values']['email'];

  variable_set('lingotek_activation_first_name', $first_name);
  variable_set('lingotek_activation_last_name', $last_name);
  variable_set('lingotek_activation_email', $email);

  // Check the current settings
  $current_community_identifier = variable_get('lingotek_community_identifier', '');
  $current_oauth_consumer_id = variable_get('lingotek_oauth_consumer_id', '');
  $current_oauth_consumer_secret = variable_get('lingotek_oauth_consumer_secret', '');
  $current_login_id = variable_get('lingotek_login_id', '');
  $current_project = variable_get('lingotek_project', '');
  $current_vault = variable_get('lingotek_vault', '');
  $current_workflow = variable_get('lingotek_workflow', '');


  // Provision a new community if any critical data is missing.
  if ($current_community_identifier == '' || $current_oauth_consumer_id == '' || $current_oauth_consumer_secret == '' || $current_login_id == '' || $current_project == '' || $current_vault == '' || $current_workflow == '') {

    // Provision a new Community & Save the Credentials
    $lead = lingotek_identify_lead(array(
      'first_name' => $first_name,
      'last_name' => $last_name,
      'email' => $email
        ));
    $callback_url = lingotek_get_notifications_url();

    $parameters = array_merge($lead, array(
      'communityDisplayName' => lingotek_get_site_name(),
      'contactName' => $first_name . ' ' . $last_name,
      'contactEmail' => $email,
      'company' => lingotek_get_site_name(), //currently we are not asking for this.
      'website' => $base_url,
      'lead' => json_encode($lead)
        ));

    $api = LingotekApi::instance();
    $response = $api->createCommunity($parameters, $callback_url);

    if ($response && $response->results == "success") {
      $community = $response;
      /*
        stdClass::__set_state(array(
        'results' => 'success',
        'community' => 'EDZDDWXA',
        'oauth_key' => '9a7187ea-fad9-41c0-9c0c-3eedbf9dffd0',
        'oauth_secret' => '58f059b9-d167-4f87-b29a-acbc4e0c67c9',
        'external_id' => 'community_admin',
        'tm_vault_id' => 212,
        'workflow_id' => '22092f38-a1f9-4499-b8d4-9e260e7e5d16',
        'project_id' => 411,
        ))
       */
      variable_set('lingotek_community_identifier', $community->community);
      variable_set('lingotek_oauth_consumer_id', $community->oauth_key);
      variable_set('lingotek_oauth_consumer_secret', $community->oauth_secret);
      variable_set('lingotek_login_id', $community->external_id); // Used as the 'External ID'.  For old users, this is their login name / email.
      variable_set('lingotek_project', $community->project_id);
      variable_set('lingotek_vault', $community->tm_vault_id);
      variable_set('lingotek_workflow', $community->workflow_id);
      variable_set('lingotek_notifications_callback', $callback_url);
      variable_set('lingotek_cms_tag', $lead['distribution']);

      $_SESSION['lingotek_setup_path'] = array('admin/config/lingotek/new-account');
      drupal_set_message(t('Your new Lingotek account has been setup.'));
      drupal_goto('admin/config/lingotek/language-settings');
    } // END:  if 200: Success
    else {

      // If there were any setup or communication problems.
      drupal_set_message(t('There was an error contacting the Lingotek servers to create your account.  Please try again later. [Server: @server]', array('@server' => LINGOTEK_API_SERVER)), 'error');
    } // END:  if error.
  } // END:  Provision new Community
  else { // If the user already has all the required community credentials, just direct them to the next step.
    $_SESSION['lingotek_setup_path'] = array('admin/config/lingotek/new-account');
    drupal_set_message(t('Your Lingotek account settings have been saved.'));
    drupal_goto('admin/config/lingotek/language-settings');
  }
}

// END:  New Account - Form Processor

/**
 * Account Settings (for Current Users) - Form
 */
function lingotek_setup_account_settings_form() {

  $current_login_id = variable_get('lingotek_login_id', '');
  $current_password = variable_get('lingotek_password', '');

  $form = array();
  $form['lingotek_user_directions_1'] = array(
    '#type' => 'item',
    '#title' => 'Account Login',
    '#description' => 'Connect an existing Lingotek account.'
  );

  $form['lingotek_lid'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => $current_login_id,
    '#size' => 40,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array('autocomplete' => array('off')),
  );

  $form['lingotek_pid'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#default_value' => $current_password,
    '#size' => 40,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array('autocomplete' => array('off')),
  );

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  $form['save_settings'] = array(
    '#type' => 'submit',
    '#value' => t('Next'), // Login
  );
  $form['lingotek_back_button'] = lingotek_setup_link('admin/config/lingotek/new-account', t('Need to create an account?'));
  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

// END:  Account Settings - Form

/**
 * Account Settings (for Current Users) - Form Processing
 */
function lingotek_setup_account_settings_form_submit($form, $form_state) {
  $login_id = $form_state['values']['lingotek_lid'];
  $password = $form_state['values']['lingotek_pid'];
  //debug( $login_id );
  //debug( $password );
  // Validate the Account - Can we Login?
  // These are V4 Basic Auth API Calls for Legacy Users
  $communities = lingotek_get_communities_v4($login_id, $password, LINGOTEK_API_SERVER . '/lingopoint/api/4');
  if ($communities === FALSE) {
    drupal_set_message(t('Invalid login or password.'), 'error');
  }
  else {
    // Login Successful:  Valid Account
    $_SESSION['lingotek_setup_path'] = array('admin/config/lingotek/account-settings');
    drupal_set_message(t('Your account settings have been saved.'));
    variable_set('lingotek_login_id', $login_id);
    variable_set('lingotek_password', $password);
    drupal_goto('admin/config/lingotek/community-select');
  } // END:  Valid Login
}

// END:  Account Settings - Form Processing

/**
 * Community Select Screen (for Current Users) - Form
 */
function lingotek_community_select_form() {
  $login_id = variable_get('lingotek_login_id', '');
  $password = variable_get('lingotek_password', '');

  $form = array();

  // V3 API Calls for Legacy Users
  $communities = lingotek_get_communities_v4($login_id, $password, LINGOTEK_API_SERVER . '/lingopoint/api/4');

  if ($communities === FALSE) {

    drupal_set_message(t('Invalid login or password.'), 'error');
    drupal_goto('admin/config/lingotek/account-settings'); // Shouldnt be here.  So something messed up.  Go back.
  }
  else {
    $count = count($communities);
    if ($count == 1) { // Just one community, use that.
      $community_keys = array_keys($communities);
      $community_id = array_pop($community_keys);
      variable_set('lingotek_community_identifier', $community_id);
      drupal_goto('admin/config/lingotek/project-select'); // Default path, if they only have 1 community. 
    }
    elseif ($count > 1) { // More then 1 community
      // Stay on this page
    }
    else {
      // 0 Results, we have an error.  Or, we should create a community for them.
      drupal_set_message(t('Error Accessing Account. Please contact customer service.'), 'error');
      drupal_goto('admin/config/lingotek/account-settings'); // Shouldnt be here.  So something messed up.  Go back.
    }
  } // END:  Community Select Paths

  $form['lingotek_user_directions_1'] = array('#markup' => '<p>Your account is associated with multiple Lingotek communities.</p><p>Which community should we associate your Drupal site with?</p>');
  $form['lingotek_top_spacer'] = array('#markup' => '<div>&nbsp;</div>');

  $form['lingotek_site_community'] = array(
    '#title' => t('Community'),
    '#type' => 'select',
    '#options' => $communities,
    //'#default_value' => ,
    //'#description' => t( '' ),
    '#required' => TRUE,
  );

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/community-select') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

// END:  Community Select

/**
 * Community Select Screen - Form Submit
 */
function lingotek_community_select_form_submit($form, $form_state) {
  $community_identity = $form_state['values']['lingotek_site_community'];
  variable_set('lingotek_community_identifier', $community_identity);

  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/community-select';
  drupal_set_message(t('Your Lingotek Community Settings have been saved.'));
  drupal_goto('admin/config/lingotek/project-select'); // Move to Project Select Step. 
}

/**
 * Project Select Screen (for Current Users) - Form Layout
 */
function lingotek_project_select_form() {
  $login_id = variable_get('lingotek_login_id', '');
  $password = variable_get('lingotek_password', '');

  $form = array();

  // V3 API Call for Legacy Users
  $community_identity = variable_get('lingotek_community_identifier', NULL);
  $projects = lingotek_get_projects_v4($login_id, $password, $community_identity, LINGOTEK_API_SERVER . '/lingopoint/api/4');
  //To-do: $vaults = lingotek_get_vaults_v4( $login_id, $password, $community_identity, $lingotek_url . '/lingopoint/api/4' );

  if ($projects === FALSE) {
    drupal_set_message(t('Error Retrieving Account Information.'), 'error');
    drupal_goto('admin/config/lingotek/account-settings'); // Error geting projects.  Go back.
  }
  else {
    $count = count($projects);
    if ($count == 1) {

      // If there is just 1 Project, Auto-Select it.  Then move on to the next step.
      $project_id = '';
      $workflow_id = '';

      $project_keys = array_keys($projects);
      $project_string = array_pop($project_keys);

      // This can come back as just the ProjectID, OR the ProjectID ||| WorkflowID jammed together, so check the string.
      if (strpos($project_string, '|||')) { // Returns a position or false if not found.
        $parts = explode('|||', $project_string);
        $project_id = $parts[0];
        $workflow_id = $parts[1];
      }
      else {
        $project_id = $project_string;
      }

      variable_set('lingotek_project', $project_id);
      variable_set('lingotek_workflow', $workflow_id);

      drupal_goto('admin/config/lingotek/language-settings'); // Default path, if they only have 1 project. 
    }
    else {
      // Stay on this page.
    }
  }

  $form['lingotek_user_directions_1'] = array('#markup' => '<p>Your account is associated with multiple Lingotek projects.</p><p>Choose the project where you would like your translated content stored.</p>');
  $form['lingotek_top_spacer'] = array('#markup' => '<div>&nbsp;</div>');

  $form['lingotek_site_project'] = array(
    '#title' => t('Project'),
    '#type' => 'select',
    '#options' => $projects,
    //'#default_value' => ,
    //'#description' => t( '' ),
    '#required' => TRUE,
  );

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/project-select') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

// END:  Project Select

/**
 * Project Select Screen - Form Submit
 */
function lingotek_project_select_form_submit($form, $form_state) {

  $project_id = '';
  $workflow_id = '';

  $project_string = $form_state['values']['lingotek_site_project'];

  // This can come back as just the ProjectID, OR the ProjectID ||| WorkflowID jammed together, so check the string.
  if (strpos($project_string, '|||')) { // Returns a position or false if not found.
    $parts = explode('|||', $project_string);
    $project_id = $parts[0];
    $workflow_id = $parts[1];
  }
  else {
    $project_id = $project_string;
  }

  variable_set('lingotek_project', $project_id);
  variable_set('lingotek_workflow', $workflow_id);

  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/project-select';
  drupal_set_message(t('Your Lingotek project selection has been saved.'));
  drupal_goto('admin/config/lingotek/language-settings'); // Move to Source Language Select Step. 
}

/**
 * Language Selection - Form Layout
 */
function lingotek_setup_language_settings_form() {

  $languages = array(
    '0' => t('Select a Language...'),
    'ar' => t('Arabic'),
    'zh-hans' => t('Chinese (Simplified)'),
    'nl' => t('Dutch'),
    'en' => t('English'),
    'fr' => t('French'),
    'de' => t('German'),
    'it' => t('Italian'),
    'ja' => t('Japanese'),
    'ko' => t('Korean'),
    'pt-br' => t('Portuguese'),
    'ru' => t('Russian'),
    'es' => t('Spanish')
  );
  $default_language = language_default();
  $source_language = variable_get('lingotek_source_language', $default_language->language);

  $form['lingotek_current_language'] = array(
    '#type' => 'item',
    '#title' => t('Source language'),
    '#description' => t('To get started, select the source language that the website is written in.')
  );

  $form['lingotek_install_source_language'] = array(
    //'#title' => t('Source Language'),
    '#type' => 'select',
    '#options' => $languages,
    '#default_value' => $source_language,
    '#description' => t('<br/>In most cases, your source language should be the default language. (The current site default language is: <b>' . $default_language->name . '</b>)'),
    //t('The current language your site is written in.'),
    '#required' => TRUE,
  );

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/language-settings') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

/**
 * Language Selection - Form Validation
 * Requires a source and target language to be selected.
 */
function lingotek_setup_language_settings_form_validate($form, &$form_state) {

  if ($form_state['values']['lingotek_install_source_language'] == '0') {
    form_set_error('', t('You must select a valid source language.'));
  }
}

/**
 * Language Selection - Form Submit Processing
 * Save source and target language selections.
 * Activate a source or target language if it isn't currently active.
 */
function lingotek_setup_language_settings_form_submit($form, $form_state) {

  // These are the languages that are currently active in this Drupal installation.
  $active_languages = language_list('enabled');
  $source_language = $form_state['values']['lingotek_install_source_language'];

  // if the Source language is not active, activate it.
  if (!array_key_exists($source_language, $active_languages[1])) {
    locale_add_language($source_language);  // Function from the Locale module.
  }

  variable_set('lingotek_source_language', $form_state['values']['lingotek_install_source_language']);

  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/language-settings';
  drupal_set_message(t('Your language settings have been saved.'));


  drupal_goto('admin/config/lingotek/setup-language-switcher');
}

function lingotek_setup_language_switcher_form($form, $form_state) {
  $current_theme = variable_get('theme_default', 'none'); //global $theme_key;

  $query = db_select('block', 'b');
  $query->fields('b');
  $query->condition('module', 'locale');
  $query->condition('delta', 'language');
  $query->condition('theme', $current_theme);
  $result = $query->execute();

  $block = $result->fetchAssoc();
  $block_enabled = $block['status'];
  $block_regions = system_region_list($current_theme, REGIONS_VISIBLE);

  $ls_chkbox_enabled = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => TRUE //$block_enabled,
  );

  $default_region = "sidebar_first";
  $default_region_value = array_key_exists($block['region'], $block_regions) ? $block['region'] : (array_key_exists($default_region, $block_regions) ? $default_region : NULL);

  $ls_select_region = array(
    '#type' => 'select',
    //'#title' => 'Region',
    '#description' => t('When enabled, the region to be displayed in.'),
    '#options' => $block_regions,
    '#default_value' => $default_region_value
  );

  $form = array(
    '#type' => 'item',
    '#title' => t('Default Language Switcher'),
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Next')
      ),
      'lingotek_back_button' => lingotek_setup_link('admin/config/lingotek/language-settings', t('Previous Step'))
    )
  );
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/setup-language-switcher';

  $form[] = array(
    '#type' => 'item',
    '#description' => t('Would you like to enable the default language switcher?'),
      //'#suffix' => t('The default language switcher block is only shown if at least two languages are enabled and language negotiation is set to <i>URL</i> or <i>Session</i>.')
  );
  $form['theme'] = array(
    '#type' => 'hidden',
    '#value' => $current_theme
  );

  $form['enabled'] = $ls_chkbox_enabled;
  $form['region'] = $ls_select_region;
  $output = array(
    $form
  );
  return $output;
}

function lingotek_setup_language_switcher_form_submit($form, $form_state) {
  require_once('lingotek.admin.inc');
  lingotek_admin_language_switcher_form_submit($form, $form_state);
  drupal_goto('admin/config/lingotek/node-translation-settings');
}

/**
 * Node Translation Settings - Form Layout
 * Select the Content Types and Fields to be Translated.
 */
function lingotek_setup_node_translation_settings_form() {

  $raw_types = node_type_get_types();
  $types = array();
  $translate = variable_get('lingotek_translate_fields', array());

  if (count($translate) > 0) {
    $use_defaults = FALSE;
  }
  else {
    $use_defaults = TRUE;
  }

  // What types of fields DO we translate?
  $included_fields = array('text', 'text_long', 'text_textfield', 'text_textarea_with_summary');

  $form[] = array(
    '#type' => 'item',
    '#title' => t('Which content do you want translated?'),
  );
  //$form['lingotek_instructions'] = array('#markup' => '<div>Which content do you want translated?</div>');
  //$form['lingotek_top_spacer'] = array('#markup' => '<div>&nbsp;</div>');

  $form['lingotek_checkboxes_open'] = array('#markup' => '<div id="content-checkboxes" style="border: 0px solid red;">');
  // Arrange the Types nicely.
  foreach ($raw_types as $value) {
    $types[$value->type] = $value->name;
    $form['type_' . $value->type] = array(
      '#type' => 'checkbox',
      '#title' => t('@value_name (<span id="showhidelink_@value_type"><a id="showlink_@value_type" href="@url" class="use-ajax">choose fields</a></span>)', array('@value_name' => $value->name, '@value_type' => $value->type, '@url' => url('admin/config/lingotek/content-type-choose-fields-ajax/' . $value->type))),
      '#return_value' => $value->type,
      '#default_value' => ( array_key_exists($value->type, $translate) || ( $use_defaults === TRUE) ) ? TRUE : FALSE,
      '#prefix' => '<div id="content_type_' . $value->type . '">',
      '#suffix' => '</div>' . '',
      '#ajax' => array(
        'event' => 'click',
        'callback' => 'lingotek_content_type_checkbox_callback',
        'wrapper' => 'content_fields_' . $value->type,
      ),
    );

    // Grab the fields for this Content Type.
    $fields = field_info_instances('node', $value->type);

    $switch = ( array_key_exists($value->type, $translate) ) ? 'block' : 'none';
    $form['lingotek_' . $value->type . 'fieldlist_open'] = array('#markup' => '<div id="content_fields_' . $value->type . '" style="display: ' . $switch . ';">');
    $form['lingotek_fieldlist_details_' . $value->type] = array('#markup' => '<div style="padding-left: 25px; padding-bottom: 5px;">Which fields would you like to include:</div>');

    $title_found = FALSE; // Is there a title field present?
    // Loop the fields, outputting them.
    foreach ($fields as $field) {
      $field_label = $field['label'];
      $field_machine_name = $field['field_name'];
      $field_type = $field['widget']['type'];
      $field_name = $value->type . '_SEPERATOR_' . $field_machine_name;

      if (($field_label == 'Title') && ($field_type == 'text_textfield')) {
        $title_found = TRUE;
      }
      /*
        $rand = rand();
        $form['lingotek_fieldlist_debug_'.$rand] = array( '#markup' => '<div>'
        .'Name: ' .$field_name .'<br>'
        .'Label: ' .$field_label .'<br>'
        .'Type: ' .$field_type .'<br>'
        .'</div>' );
       */
      // Exclude field types we dont translate.
      if (TRUE == array_search($field_type, $included_fields)) {
        $form[$field_name] = array(
          '#type' => 'checkbox',
          '#title' => t('@fieldlabel [@fieldtype]', array('@fieldlabel' => $field_label, '@fieldtype' => $field_type)),
          // Set if your machine type is set to translate, and your field has been flagged for translation.   OR!  If use defaults == TRUE, and your field type has text in the name somewhere
          '#default_value' => ( ( isset($translate[$value->type]) && FALSE !== array_search($field_machine_name, $translate[$value->type]) ) || ( ( $use_defaults === TRUE) && (strpos($field_type, 'text') !== FALSE ) ) ) ? TRUE : FALSE,
          '#prefix' => '<div style="padding-left: 35px;">',
          '#suffix' => '</div>',
        );
      }
    } // END:  foreach field
    // if we did NOT find a title field for the specified content type, add it manually as an option, and we can do a title swap for the user.
    if ($title_found === FALSE) {
      $form['title_swap_' . $value->type] = array(
        '#type' => 'checkbox',
        '#title' => t('Title (Note: field will be created)'),
        '#default_value' => TRUE,
        '#prefix' => '<div style="padding-left: 35px;">',
        '#suffix' => '</div>',
      );
    }

    $form['lingotek_' . $value->type . 'fieldlist_close'] = array('#markup' => '</div>');

    $rand = rand();
    $form['lingotek_after_fields_' . $rand] = array('#markup' => '<div>&nbsp;</div>');
  } // END:  foreach content type

  $form['lingotek_checkboxes_close'] = array('#markup' => '</div>');


  if (variable_get('lingotek_setup_complete', 0)) {
    $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save')
    );
  }
  else {
    $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Next')
    );

    if (is_array($_SESSION['lingotek_setup_path'])) {
      if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/node-translation-settings') {
        $null = array_pop($_SESSION['lingotek_setup_path']);
      } // if the user went back, remove the last element, which is this page.
      $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
    }
    $form['lingotek_support_footer'] = lingotek_support_footer();
  }

  return $form;
}

/**
 * Node Translation Settings - Ajax Callback 1 (Primary)
 */
function lingotek_content_type_choose_fields_callback($content_type) {

  $selector = '#content_fields_' . $content_type;
  $commands = array();

  // <span id="showhidelink_' .$value->type .'"><a id="showlink_' .$value->type .'" 
  // The CHOOSE FIELDS text is currently shown, switch to HIDE FIELDS
  if (in_array('showlink_' . $content_type, $_REQUEST['ajax_html_ids'])) {
    $link = '<a id="hidelink_' . $content_type . '" href="' . url('admin/config/lingotek/content-type-choose-fields-ajax/' . $content_type) . '" class="use-ajax">hide fields</a>';
    $span_selector = '#showhidelink_' . $content_type;
    $commands[] = ajax_command_html($span_selector, $link);
  }
  if (in_array('hidelink_' . $content_type, $_REQUEST['ajax_html_ids'])) {
    $link = '<a id="showlink_' . $content_type . '" href="' . url('admin/config/lingotek/content-type-choose-fields-ajax/' . $content_type) . '" class="use-ajax">choose fields</a>';
    $span_selector = '#showhidelink_' . $content_type;
    $commands[] = ajax_command_html($span_selector, $link);
  }

  $commands[] = ajax_command_invoke($selector, 'toggle');
  //$commands[] = ajax_command_alert( 'Hiding: ' .$data  );

  return ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
}

/**
 * Node Translation Settings - Ajax Callback - Checkbox Check/Uncheck
 */
function lingotek_content_type_checkbox_callback($form, &$form_state) {

  $triggered_by = $form_state['input']['_triggering_element_name'];  // IE:  type_article
  $selected_type = substr($triggered_by, 5); // This was the content type that was selected.  IE:  article
  $selector = '#content_fields_' . $selected_type;

  $commands = array();

  // A checkbox was just selected.   If it is checked, its value will be its type name.  If it was unchecked, it will be blank.
  $checkbox_value = $form_state['input'][$triggered_by];

  // If the checkbox was un-checked, deselect child checkboxes.
  if ($checkbox_value == '') {
    // NAME: article_SEPERATOR_body
    // removeAttr('checked')
    // 'td[name^=tcol]'
    $checkbox_selector = ':checkbox[name^=' . $selected_type . '_SEPERATOR_]';
    $link_selector = '#togglelink_' . $selected_type;
    $commands[] = ajax_command_invoke($checkbox_selector, 'removeAttr', array('checked'));

    $checkbox_selector2 = ':checkbox[name=title_swap_' . $selected_type . ']';
    $commands[] = ajax_command_invoke($checkbox_selector2, 'removeAttr', array('checked'));

    $commands[] = ajax_command_invoke($selector, 'hide');

    $link = '<a id="showlink_' . $selected_type . '" href="' . url('admin/config/lingotek/content-type-choose-fields-ajax/' . $selected_type) . '" class="use-ajax">choose fields</a>';
    $span_selector = '#showhidelink_' . $selected_type;
    $commands[] = ajax_command_html($span_selector, $link);
  } // END: uncheck
  else { // If the checkbox was selected, activate the default fields.
    //$commands[] = ajax_command_invoke( $selector, 'show' );
    $checkbox_selector2 = ':checkbox[name=title_swap_' . $selected_type . ']';
    $commands[] = ajax_command_invoke($checkbox_selector2, 'attr', array('checked', 'checked'));

    // Grab the fields for this Content Type.
    $fields = field_info_instances('node', $selected_type);
    // Loop the fields, outputting them.
    foreach ($fields as $field) {
      $field_label = $field['label']; //  Label: Body
      $field_machine_name = $field['field_name'];
      $field_type = $field['widget']['type']; // Type: text_textarea_with_summary
      $field_name = $selected_type . '_SEPERATOR_' . $field_machine_name; // Name: article_SEPERATOR_body
      // If this is a text field, include it by default.
      if (strpos($field_type, 'text') !== FALSE) {
        $checkbox_selector = ':checkbox[name=' . $field_name . ']';
        $commands[] = ajax_command_invoke($checkbox_selector, 'attr', array('checked', 'checked'));
      }
    } // END:  foreach  $fields
  } // END:  checked
  //$commands[] = ajax_command_invoke( $selector, 'toggle' );
  //$commands[] = ajax_command_alert( 'Info: ' . $link_selector );

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Node Translation Settings - Form Submit
 */
function lingotek_setup_node_translation_settings_form_submit($form, &$form_state) {
  $translate = array();
  $operations = array();
  foreach ($form_state['values'] as $key => $value) {

    // Look for Selected Content Types and Fields.
    if (FALSE !== strpos($key, '_SEPERATOR_')) {
      // And only if set to translate
      if ($value != 0) {
        $parts = explode('_SEPERATOR_', $key);
        $content_type = $parts[0];
        $content_field = $parts[1];
        $translate[$content_type][] = $content_field;

        // Set this content type to be Lingotek translated.
        variable_set('language_content_type_' . $content_type, LINGOTEK_ENABLED);
        // Set this field to 'translatable'.
        // Update the field via the Field API (Instead of the direct db_update)
        $field = field_info_field($content_field);
        $field['translatable'] = 1;
        $field['lingotek_translatable'] = 1;
        field_update_field($field);
      }
    } // END:  Selected Content Types and Fields
    // Look for any nodes we need to do the Title swap for.
    if (FALSE !== strpos($key, 'title_swap_')) {
      // And only if set to swap
      if ($value != 0) {
        $content_type = substr($key, strlen('title_swap_'));
        //dpm( 'Title Swap: ' . $content_type );
        // Do the actual title replacement
        $entity_type = 'node';
        $bundle = $content_type;
        $legacy_field = 'title';

        // Use the Title module to migrate the content.
        if (title_field_replacement_toggle($entity_type, $bundle, $legacy_field)) {
          //title_field_replacement_batch_set($title_entity, $title_bundle, $title_field);
          $operations[] = array('title_field_replacement_batch', array($entity_type, $bundle, $legacy_field));
          $translate[$content_type][] = 'title_field';
          $field = field_info_field('title_field');
          $field['translatable'] = 1;
          $field['lingotek_translatable'] = 1; // Ensure the title field is flagged for Lingotek translation.
          $operations[] = array('field_update_field', array($field)); //field_update_field($field);
        }
      }
    }
  }
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/node-translation-settings';
  variable_set('lingotek_translate_fields', $translate);

  $operations[] = array('lingotek_cleanup_utility', array(FALSE));
  $operations[] = array('variable_set', array('lingotek_setup_complete', 1));

  $batch = array(
    'title' => t('Preparing content for translation'),
    'operations' => $operations
  );

  batch_set($batch);
  drupal_set_message(t('You have completed setup successfully!'));
  $redirect = LINGOTEK_MENU_LANG_BASE_URL;
  batch_process($redirect);
}

/**
 * Future Page - Form Layout
 */
function lingotek_setup_node_updates_form($form, $form_state) {

  $source_language = lingotek_get_source_language();

  $form['lingotek_header'] = array('#markup' => '<h1>Node Updates</h1>');
  //$form['lingotek_instructions'] = array( '#markup' => '<div>The Lingotek Translation module has be</div>' );
  $form['lingotek_header_line'] = array('#markup' => '<hr />');
  $form['lingotek_top_spacer'] = array('#markup' => '<div>&nbsp;</div>');


  $form['lingotek_message_1a'] = array('#markup' => '<p>To enable full multilingual functionality on your site we recommend the following:</p>');
  //$form['lingotek_message_1b'] = array( '#markup' => '<div>&nbsp;</div>' );

  $form['title_group'] = array('#type' => 'fieldset', '#title' => t('Enable Multilingual Node Titles'), '#collapsible' => FALSE, '#collapsed' => FALSE);
  $form['title_group']['lingotek_message_2a'] = array('#markup' => '<p>Drupal\'s default node titles do not support multiple languages and translations.  For this reason Lingotek uses the Title module.</p>');
  $form['title_group']['lingotek_message_2b'] = array('#markup' => '<p>The Title module modifies your content types and adds a new title field, and copies over the current node titles.  The original node titles are left where they are, so you can switch back at any time.</p>');
  $form['title_group']['lingotek_message_2c'] = array('#markup' => '<p>The Title module will then respond to requests for node titles.</p>');
  //$form['title_group']['lingotek_message_2n'] = array( '#markup' => '<div>&nbsp;</div>' );
  $form['title_group']['lingotek_message_2o'] = array('#markup' => '<p style="padding-top: 10px;"><strong>This change will be made to the following content types:</strong></p>');
  $form['title_group']['lingotek_message_2q'] = array('#markup' => '<ul style="padding-left: 0px;">');
  $form['title_group']['lingotek_message_2r'] = array('#markup' => '<li>A Node Name</li>');
  $form['title_group']['lingotek_message_2s'] = array('#markup' => '</ul>');
  $form['title_group']['transition_node_titles'] = array(
    '#type' => 'checkbox',
    '#title' => t('Yes, Enable Multilingual Node Titles'),
    '#prefix' => '<div style="padding-left: 30px; padding-top: 10px;">',
    '#suffix' => '</div>',
  );

  $form['language_group'] = array('#type' => 'fieldset', '#title' => t('Update Nodes with Undefined Languages'), '#collapsible' => FALSE, '#collapsed' => FALSE);
  $form['language_group']['lingotek_message_3a'] = array('#markup' => '<p>Currently you have X nodes that do not identify what language their content is in.</p>');
  $form['language_group']['lingotek_message_3b'] = array('#markup' => '<p>To properly translate a node it needs to have a language defined.</p>');
  $form['language_group']['lingotek_message_3c'] = array('#markup' => '<p>To simplify the process, we can set the language setting for any nodes with an undefined language.</p>');
  $form['language_group']['update_node_language'] = array(
    '#type' => 'checkbox',
    '#title' => t('Yes, Set Nodes with no Specified Language to @language.', array('@language' => $source_language)),
    '#prefix' => '<div style="padding-left: 30px; padding-top: 10px;">',
    '#suffix' => '</div>',
  );

  $form['lingotek_middle_spacer'] = array('#markup' => '<div>&nbsp;</div>');

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );
  $form['lingotek_back_button'] = lingotek_setup_link('admin/config/lingotek/language-settings', t('Previous Step'));

  return $form;
}

/**
 * Setup Button
 */
function lingotek_setup_link($path = 'admin/config/lingotek/language-settings', $text = 'Previous Step') {
  return array(
    '#markup' => l($text, $path, array('#attributes' => 'margin-left: 20px;'))
  );
}

/**
 * Future Page - Form Submit
 */
function lingotek_setup_node_updates_form_submit($form, $form_state) {

  $transition_node_titles = $form_state['values']['transition_node_titles'];
  $update_node_language = $form_state['values']['update_node_language'];

  if ($transition_node_titles == 1) {
    //dpm( 'Do the Node Titles' );
  }

  if ($update_node_language == 1) {
    //dpm( 'Update The Node Languages' );
  }

  //drupal_goto( 'admin/config/lingotek/setup-complete' );
}

/*
 * Get the Lingotek user's current Communities.  Use the V3 API.
 */

function lingotek_get_communities_v4($login, $passwd, $url) {

  // API V4 Connection
  $client = new LingotekSession();

  $client->login_id = $login;
  $client->password = $passwd;
  $client->url = $url;

  $options = array();

  if (!$client->canLogIn()) {
    return FALSE;
  }

  $list_communities = $client->request("listCommunities", array());
  if ($list_communities->results == "success") {
    foreach ($list_communities->communities as $community) {
      $options[$community->id] = $community->name;
    }
  }

  return $options;
}

/*
 * Get the Lingotek user's current Projects.  Uses the V4 API with Basic Auth.
 */

function lingotek_get_projects_v4($login, $passwd, $community_identity, $url) {

  // API V4 Connection
  $client = new LingotekSession();

  $client->login_id = $login;
  $client->password = $passwd;
  $client->url = $url;
  $client->community = $community_identity;

  if (!$client->canLogIn()) {
    return FALSE;
  }

  $options = array();
  $list_projects = $client->request("listProjects", array('community' => $community_identity));
  if ($list_projects->results == "success") {
    foreach ($list_projects->projects as $project) {
      if ($project->state == 'Active') {
        if (!empty($project->workflowId)) { // Send back the ProjectID AND WorkflowID.  Joined by |||
          $options[$project->id . '|||' . $project->workflowId] = $project->name;
        }
        else { // Just send back the Project ID
          $options[$project->id] = $project->name;
        }
      }
    }
  }

  return $options;
}

/**
 * Implement hook_mail()
 */
function lingotek_mail($key, &$message, $params) {

  switch ($key) {
    case 'activation': // Activation Email
      $date = date('m/d/Y h:i a');
      $message['subject'] = t('New Community Provisioned - @date', array('@date' => $date));
      $message['body'][] = t('A new community was provisioned on @date.', array('@date' => $date));
      $message['body'][] = t('First Name: @firstname', array('@firstname' => $params['first_name']));
      $message['body'][] = t('Last Name: @lastname', array('@lastname' => $params['last_name']));
      $message['body'][] = t('Email: @email', array('@email' => $params['email']));
      $message['body'][] = '';
      break;
  }
}