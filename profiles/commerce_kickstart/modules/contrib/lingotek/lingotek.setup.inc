<?php

/**
 * @file
 * Lingotek Easy Install Process.
 */
include_once('lingotek.admin.inc');

/**
 * Routing:  Returning User, New Account or Already Setup
 */
function lingotek_setup() {

  // Does the install already have connection credentials?
  if (lingotek_is_config_missing()) {
    drupal_goto('admin/config/lingotek/new-account'); // If something is missing - Go though the Setup Process
  }
  else {
    drupal_goto('admin/settings/lingotek/settings'); // We already have credentials, goto the Dashboard
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
    $notify_url = lingotek_notify_url_generate();

    $parameters = array_merge($lead, array(
      'communityDisplayName' => lingotek_get_site_name(),
      'contactName' => $first_name . ' ' . $last_name,
      'contactEmail' => $email,
      'company' => lingotek_get_site_name(), //currently we are not asking for this.
      'website' => $base_url,
      'lead' => json_encode($lead)
    ));

    $api = LingotekApi::instance();
    $response = $api->createCommunity($parameters, $notify_url);

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
      variable_set('lingotek_translate_comments_workflow_id', $community->workflow_id);
      variable_set('lingotek_translate_config_workflow_id', $community->workflow_id);
      variable_set('lingotek_integration_method', $community->integration_method_id);
      variable_set('lingotek_notify_url', $notify_url);
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

  // Validate the Account
  list($success, $msg) = lingotek_list_community_integrations($login_id, $password);
  if ($success == FALSE) {
    drupal_set_message($msg, 'error');
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

  list($success, $msg) = lingotek_list_community_integrations($login_id, $password);

  if ($success == FALSE) {

    drupal_set_message($msg, 'error');
    drupal_goto('admin/config/lingotek/account-settings'); // Shouldnt be here.  So something messed up.  Go back.
  }
  else {
    $community_integrations = $msg;

    $count = count($community_integrations);
    if ($count == 1) { // Just one community, use that.
      $ci = array_pop($community_integrations);
      variable_set('lingotek_community_identifier', $ci->community_id);
      variable_set('lingotek_oauth_consumer_id', $ci->key);
      variable_set('lingotek_oauth_consumer_secret', $ci->secret);
      drupal_goto('admin/config/lingotek/project-vault-select'); // Default path, if they belong to one community.
    }
    elseif ($count > 1) { // More than 1 community
      // Stay on this page
    }
    else {
      // 0 Results, we have an error.  Or, we should create a community for them.
      drupal_set_message(t('Error Accessing Account. Please contact customer service.'), 'error');
      drupal_goto('admin/config/lingotek/account-settings'); // Shouldnt be here.  So something messed up.  Go back.
    }
  } // END:  Community Select Paths

  $form['lingotek_user_directions_1'] = array(
    '#markup' => '<p>Your account is associated with multiple Lingotek communities.</p>
    <p>Select the community to associate this site with:</p>');
  $community_options = array();
  foreach ($community_integrations as $ci) {
    $community_options[$ci->community_id] = $ci->community_name . ' (' . $ci->community_id . ')';
  }

  $form['lingotek_site_community'] = array(
    '#title' => t('Community'),
    '#type' => 'select',
    '#options' => $community_options,//array_combine(array_keys($communities),array_keys($communities)),
    //'#default_value' => ,
    //'#description' => t( '' ),
    '#required' => TRUE,
  );
  
  $form['lingotek_communities'] = array(
    '#type' => 'hidden',
    '#value' => json_encode($community_integrations)
  );

  $form['lingotek_button_spacer'] = array('#markup' => '<div>&nbsp;</div>');
  
  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/community-select') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

// END:  Community Select

/**
 * Community Select Screen - Form Submit
 */
function lingotek_community_select_form_submit($form, $form_state) {
  $community_selected = $form_state['values']['lingotek_site_community'];
  variable_set('lingotek_community_identifier', $community_selected);
  $communities = (array)json_decode($form_state['values']['lingotek_communities']);
  $community = $communities[$community_selected];
  variable_set('lingotek_oauth_consumer_id', $community->key);
  variable_set('lingotek_oauth_consumer_secret', $community->secret);

  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/community-select';

  drupal_set_message(t('Your site has been securely connected to your community (@community).', array('@community' => $community_selected)));
  drupal_goto('admin/config/lingotek/project-vault-select'); // Move to Project Select Step.
}

/**
 * Project Select Screen (for Current Users) - Form Layout
 */
function lingotek_project_vault_select_form() {
  $form = array();

  $community_identity = variable_get('lingotek_community_identifier', NULL);
  list($community_settings, $full_community_data) = lingotek_get_community_settings($community_identity, TRUE);

  if ($community_settings === FALSE) {
    drupal_set_message(t('Error Retrieving Account Information.'), 'error');
    drupal_goto('admin/config/lingotek/account-settings'); // Error geting projects.  Go back.
  }
  elseif ($community_settings['project'] === FALSE || $community_settings['workflow'] === FALSE || $community_settings['vault'] === FALSE) {
    drupal_set_message(t('Error Retrieving Account Information.'), 'error');
    drupal_goto('admin/config/lingotek/account-settings'); // Error geting projects.  Go back.
  }

  $form['lingotek_user_directions'] = array('#markup' => '<p>Select or create the default project and vault that you would like to use.</p>');
  $form['lingotek_community_data'] = array(
    '#type' => 'hidden',
    '#value' => json_encode($full_community_data)
  );

  // Project

    asort($community_settings['project']);
    
    $project_already_set = !is_null(variable_get('lingotek_project', NULL));
    $projects_count = count($community_settings['project']);

    $project_options = array(
        1 => t('New') . (!$project_already_set ? ' <i>(' . t('recommended') . ')</i>' : ''),
        0 => t('Existing') . ($project_already_set ? ' <i>(' . t('recommended') . ')</i>' : '')
      );
    if ($projects_count == 0) {
      unset($project_options[0]);
    }
    $form['project_new_or_existing'] = array(
      '#type' => 'radios',
      '#title' => t('Project'),
      '#default_value' => $project_already_set ? 0 : 1,
      '#options' => $project_options,
      //'#description' => t('The Lingotek Project that you would like to use for this site.'),
      '#required' => TRUE,
      //'#attributes' => array('onclick' => "(function(){ console.log($); alert('hi'); })();"),
    );
    
    $form['project_new'] = array(
      '#type' => 'textfield',
      //'#title' => t('Project Name'),
      '#default_value' => lingotek_get_site_name(),
      '#description' => t('The name of the project where content will be uploaded'),
      '#size' => 20,
      '#states' => array(
        'invisible' => array(
          ':input[name="project_new_or_existing"]' => array('value' => 0)
        )
      )
    );
    
    $form['project_existing'] = array(
      '#type' => 'select',
      //'#title' => t('Project Selection'),
      '#options' => $community_settings['project'],
      '#default_value' => variable_get('lingotek_project', NULL),
      '#description' => t('Note: ALL documents uploaded to the project, including documents not from this site, will be affected by actions taken on this site (e.g., adding languages)'),
      '#states' => array(
        'invisible' => array(
          ':input[name="project_new_or_existing"]' => array('value' => 1)
        ),
      )
    );

  // Workflow (assume project default for workflow)
    
  /*
  if ($workflow_count > 1) {

    $form['lingotek_site_workflow'] = array(
      '#title' => t('Workflow'),
      '#type' => 'select',
      '#options' => $community_settings['workflow'],
      //'#default_value' => ,
      '#description' => t('The Workflow to use when translating content.'),
      '#required' => TRUE,
    );
  }
  */
  
  // Vault

  $form['vault_existing_or_new'] = array(
    '#type' => 'radios',
    '#title' => t('Vault'),
    '#default_value' => 0,
    '#options' => array(0 => t('Existing') . ' <i>(' . t('recommended') . ')</i>', 1 => t('New')),
    //'#description' => t('The Translation Memory (TM) vault where translations are saved.'),
    '#required' => TRUE
  );

  $form['vault_existing_or_new'][1]['#states'] = array(
    'invisible' => array(
      ':input[name="project_new_or_existing"]' => array('value' => 0)
    ),
  );

  $form['vault_existing'] = array(
    '#type' => 'select',
    '#options' => $community_settings['vault'],
    '#default_value' => variable_get('lingotek_vault', NULL),
    '#description' => t('Using a shared TM vault allows translations to be re-used.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="vault_existing_or_new"]' => array('value' => 1)
      ),
    )
  );
  
  $form['vault_new'] = array(
    '#type' => 'textfield',
    //'#title' => t('Vault'),
    '#default_value' => lingotek_get_site_name(),
    '#description' => t('The name of the vault where translation memory will be stored'),
    '#size' => 20,
    '#states' => array(
      'invisible' => array(
        ':input[name="vault_existing_or_new"]' => array('value' => 0)
      )
    )
  );

  

  // submit, etc.
    
  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/project-vault-select') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );
  //$form['#submit'] = 'lingotek_project_vault_select_form_submit';

  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

// END:  Project Select

/**
 * Project Select Screen - Form Submit
 */
function lingotek_project_vault_select_form_submit($form, $form_state) {
  $auto_provision_project = FALSE;
  $params = array();

  // Project
  if ($form_state['values']['project_new_or_existing']) { // new
    $auto_provision_project = TRUE;
    $project_name = $form_state['values']['project_new'];
    $params['projectName'] = $project_name;
  }
  else { // existing
    $project_id = $form_state['values']['project_existing'];
    $lingotek_community_data = json_decode($form_state['values']['lingotek_community_data']);
    $workflow_id = $lingotek_community_data->project->{$project_id}->workflowId;
    variable_set('lingotek_project', $project_id);
    variable_set('lingotek_workflow', $workflow_id);
    variable_set('lingotek_translate_comments_workflow_id', $workflow_id);
    variable_set('lingotek_translate_config_workflow_id', $workflow_id);
  }
  
  // Vault
  if ($form_state['values']['vault_existing_or_new']) { // new
    $vault_name = $form_state['values']['vault_new'];
    $params['tmVaultName'] = $vault_name;
  }
  else { // existing
    $vault_id = $form_state['values']['vault_existing'];
    variable_set('lingotek_vault', $vault_id);
    $params['tmVaultId'] = $vault_id;
  }
  
  if ($auto_provision_project) {
    $notify_url = lingotek_notify_url_generate();
    $params['callbackUrl'] = $notify_url;
    $api = LingotekApi::instance();
    $response = $api->request('autoProvisionProject', $params);

    if ($response->results == 'success') {
      $project_id = $response->project_id;
      $workflow_id = $response->workflow_id;
      $vault_id = $response->tm_vault_id;
      variable_set('lingotek_project', $project_id);
      variable_set('lingotek_workflow', $workflow_id);
      variable_set('lingotek_translate_comments_workflow_id', $workflow_id);
      variable_set('lingotek_translate_config_workflow_id', $workflow_id);
      variable_set('lingotek_vault', $vault_id);
      variable_set('lingotek_integration_method', $response->integration_method_id);
      variable_set('lingotek_notify_url', $notify_url);
    }
  }
  
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/project-vault-select';
  drupal_set_message(t('Your Lingotek project, workflow, and vault selections have been setup and saved.'));
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
    '#description' => t('<br/>In most cases, your source language should be the default language. (The current site default language is: <b>@default_language</b>', array('@default_language' => $default_language->name)),
    //t('The current language your site is written in.'),
    '#required' => TRUE,
  );

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/language-settings') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

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

  $default_region = "sidebar_first";
  $default_region_value = array_key_exists($block['region'], $block_regions) ? $block['region'] : (array_key_exists($default_region, $block_regions) ? $default_region : NULL);

  $form = array(
    '#type' => 'item',
    '#title' => t('Default Language Switcher'),
    'actions' => array(
      '#type' => 'actions',
    ),
  );


  $form['theme'] = array(
    '#type' => 'hidden',
    '#value' => $current_theme
  );

  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable the default language switcher'),
    '#default_value' => TRUE //$block_enabled,
  );

  $form['region'] = array(
    '#type' => 'select',
    '#description' => t('The region where the switcher will be displayed.'),
    '#options' => $block_regions,
    '#default_value' => $default_region_value,
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/setup-language-switcher') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  $output = array(
    $form
  );

  $output[] = lingotek_support_footer();

  return $output;
}

function lingotek_setup_language_switcher_form_submit($form, $form_state) {
  lingotek_admin_language_switcher_form_submit($form, $form_state);
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/setup-language-switcher';
  drupal_goto('admin/config/lingotek/node-translation-settings');
}

/**
 * Entity Translation Settings - Form Layout
 * Select the Content Types and Fields to be Translated.
 */
function lingotek_setup_node_translation_settings_form($form, $form_state) {
  drupal_add_js(drupal_get_path('module', 'lingotek') . '/js/lingotek.admin.js');

  $fs['values'] = lingotek_get_global_profile();
  $fs['values']['profile_id'] = LingotekSync::PROFILE_CUSTOM;
  $form = lingotek_admin_node_translation_settings_form($form, $form_state, 'node');
  $content_defaults = lingotek_admin_profile_form($form, $fs);
  $form['lingotek_nodes_translation_method'] = $content_defaults['defaults']['lingotek_nodes_translation_method'];
  $form['lingotek_nodes_translation_method']['#weight'] = -3;
  
  $c = db_select('node', 'n')
    ->fields('n', array('tnid'))
    ->condition('n.tnid', 0, '<>')
    ->distinct()
    ->countQuery()
    ->execute()
    ->fetchField();
  
  $form['lingotek_overwrite_note'] = array(
    '#type' => 'item',
    '#title' => t('You have @count nodes with existing translations', array('@count' => $c)),
    '#states' => array(
      'visible' => array(
        ':input[name="lingotek_nodes_translation_method"]' => array('checked' => FALSE),
      ),
    ),
    '#weight' => -2,
  );
  
  $form['lingotek_overwrite'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not allow Lingotek to translate these nodes. <b>NOTE: Unchecking this option will cause Lingotek to overwrite your current translated content.</b>'),
    '#default_value' => TRUE,
    '#states' => array(
      'visible' => array(
        ':input[name="lingotek_nodes_translation_method"]' => array('checked' => FALSE),
      ),
    ),
    '#weight' => -1,
  );
  
  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/node-translation-settings') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['node_translation']['actions']['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }

  unset($form['node_translation']['actions']['submit']);
  $form['node_translation']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  $form['lingotek_support_footer'] = lingotek_support_footer();

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
 * Node Translation Settings - Form Submit
 */
function lingotek_setup_node_translation_settings_form_submit($form, &$form_state) {
  lingotek_admin_node_translation_settings_form_submit($form, $form_state);
  
  if ($form_state['values']['lingotek_nodes_translation_method'] == 'node' && $form_state['values']['lingotek_overwrite']) {
    $nodes = db_select('node', 'n')
      ->fields('n', array('nid', 'tnid'))
      ->condition('n.tnid', 0, '<>')
      ->execute()
      ->fetchAll();
    
    $ops = array();
    
    foreach ($nodes as $row) {
      $ops[] = array('lingotek_lingonode', array($row->nid, 'profile', LingotekSync::PROFILE_DISABLED));
      if ($row->nid == $row->tnid) {
        $ops[] = array('LingotekSync::setNodeEnabled', array($row->nid, FALSE));
      }
      else {
        $ops[] = array('lingotek_lingonode', array($row->nid, 'node_sync_status', LingotekSync::STATUS_TARGET));
      }
    }
    
    $batch = array(
      'operations' => $ops,
    );
    
    batch_set($batch);
    batch_process('admin/config/lingotek/comment-translation-settings');
    
  }
  else {
    drupal_goto('admin/config/lingotek/comment-translation-settings');
  }
}

function lingotek_setup_comment_translation_settings_form($form, &$form_state) {
  $form = lingotek_admin_comment_translation_settings_form($form, $form_state);

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/comment-translation-settings') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['comment_translation']['actions']['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }
  $form['comment_translation']['#submit'] = 'lingotek_setup_comment_translation_settings_form_submit';

  unset($form['comment_translation']['actions']['submit']);
  $form['comment_translation']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  $form['lingotek_support_footer'] = lingotek_support_footer();

  return $form;
}

function lingotek_setup_comment_translation_settings_form_submit($form, &$form_state) {
  lingotek_admin_comment_translation_settings_form_submit($form, $form_state);
  drupal_goto('admin/config/lingotek/additional-translation-settings');
}

function lingotek_setup_additional_translation_settings_form($form, &$form_state) {
  $form = lingotek_admin_additional_translation_settings_form($form, $form_state);

  if (is_array($_SESSION['lingotek_setup_path'])) {
    if (end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/additional-translation-settings') {
      $null = array_pop($_SESSION['lingotek_setup_path']);
    } // if the user went back, remove the last element, which is this page.
    $form['additional_translation']['actions']['lingotek_back_button'] = lingotek_setup_link(end($_SESSION['lingotek_setup_path']), t('Previous Step'));
  }
  $form['additional_translation']['#submit'] = 'lingotek_setup_additional_translation_settings_form_submit';

  unset($form['additional_translation']['actions']['submit']);
  $form['additional_translation']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Finish'),
  );

  $form['lingotek_support_footer'] = lingotek_support_footer();
  return $form;
}

function lingotek_setup_additional_translation_settings_form_submit($form, &$form_state) {
  // WTD: process the submitted options
  
  $operations = array();
  $cleanup_batch = lingotek_cleanup_utility(FALSE, FALSE); // return the batch, rather than autoset to execute
  if ($cleanup_batch && isset($cleanup_batch['operations'])) {
    $operations = array_merge($operations, $cleanup_batch['operations']);
  }
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/additional-translation-settings';
  
  lingotek_admin_additional_translation_settings_form_submit($form, $form_state, $additional_operations=$operations);
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

  $form['lingotek_back_button'] = lingotek_setup_link('admin/config/lingotek/language-settings', t('Previous Step'));

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next')
  );

  return $form;
}

/**
 * Setup Button
 */
function lingotek_setup_link($path = 'admin/config/lingotek/language-settings', $text = 'Previous Step') {
  return array(
    '#markup' => '<span style="margin-right: 15px;">' . l($text, $path) . '</span>'
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

function lingotek_list_community_integrations($login, $passwd) {

  // API V4 Connection
  $client = new LingotekSession();

  $client->login_id = $login;
  $client->password = $passwd;
  $client->url = LINGOTEK_API_SERVER . '/lingopoint/api/4';

  $options = array();

  if (!$client->canLogIn()) {
    return array(FALSE, t('Failed to connect to the Lingotek service: @error', array('@error' => $client->getLastLoginMsg())));
  }

  $list_community_integrations = $client->request("listCommunityIntegrations", array());
  if ($list_community_integrations->results == "success") {
    foreach ($list_community_integrations->communities as $community) {
      foreach ($community->integration_methods as $integration_method) {
        if (!empty($integration_method->key) && !empty($integration_method->secret)) { // select first InboundOAuth Integration keys
          $integration_method->community_name = $community->name;
          $integration_method->community_id = isset($community->id) ? $community->id : $community->name;
          $options[$integration_method->community_id] = $integration_method;
          break;
        }
      }
    }
  }
  return array(TRUE, $options);
}

/*
 * Get the Lingotek user's current Projects.
 */
function lingotek_get_community_settings($community_identity, $return_complete = FALSE) {

  $api = LingotekApi::instance();

  $options = array();
  $complete = array();
  
  // get projects
  $options['project'] = array();
  $complete['project'] = array();
  $list_projects = $api->request("listProjects", array('community' => $community_identity));
  if ($list_projects->results == "success") {
    foreach ($list_projects->projects as $project) {
      $complete['project'][$project->id] = $project;
      if ($project->state == 'Active') {
        //if (!empty($project->workflowId)) { // Send back the ProjectID AND WorkflowID.  Joined by |||
        //  $options['project'][$project->id . '|||' . $project->workflowId] = $project->name;
        //}
        //else { // Just send back the Project ID
        $options['project'][$project->id] = $project->name;
        //}
      }
    }
  }

  // get workflows
  $options['workflow'] = array();
  $complete['workflow'] = array();
  $list_workflows = $api->request("listWorkflows", array('community' => $community_identity));
  if ($list_workflows->results == "success") {
    
    foreach ($list_workflows->workflows as $workflow) {
      $workflow_key = $workflow->id;
      $complete['workflow'][$workflow_key] = $workflow;
      foreach ($workflow->workflow_steps as $step) {
        if (isset($step->save_to_vault)) {
          $vault = array();
          $vault_match = preg_match('/doc_index_id="([0-9]*)"/', $step->save_to_vault, $vault);
          $workflow_key .= '|||' . $vault[1];
        }
      }
      if ($workflow->active && !$workflow->is_public) {
        $options['workflow'][$workflow_key] = $workflow->name;
      }
    }
  }

  // get TM vaults
  $options['vault'] = array();
  $complete['vault'] = array();
  $list_vaults = $api->request("listTMVaults", array('community' => $community_identity));
  if ($list_vaults->results == "success") {
    if (isset($list_vaults->personalVaults)) {
      foreach ($list_vaults->personalVaults as $personal_vault) {
        $complete['vault']['PersonalVaults'][$personal_vault->id] = $personal_vault;
        $options['vault']['Personal Vaults'][$personal_vault->id] = $personal_vault->name;
      }
    }
    if (isset($list_vaults->communityVaults)) {
      foreach ($list_vaults->communityVaults as $community_vault) {
        $complete['vault']['CommunityVaults'][$community_vault->id] = $community_vault;
        $options['vault']['Community Vaults'][$community_vault->id] = $community_vault->name;
      }
    }
    if (isset($list_vaults->publicVaults)) {
      foreach ($list_vaults->publicVaults as $public_vault) {
        $complete['vault']['PublicVaults'][$public_vault->id] = $public_vault;
        $options['vault']['Public Vaults'][$public_vault->id] = $public_vault->name;
      }
    }
  }

  return $return_complete ? array($options, $complete) : $options;
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
