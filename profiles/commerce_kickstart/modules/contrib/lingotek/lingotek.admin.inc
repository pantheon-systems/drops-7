<?php

/**
 * @file
 * Administrative Settings for the module.
 */
include_once('lingotek.session.inc');

/**
 * Form constructor for the administration form.
 *
 * @return array
 *   A FAPI form array.
 */
function lingotek_admin_form() {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $force = isset($_GET['test_connection']) ? TRUE : FALSE;
  $connected = $api->testAuthentication($force);
  $site = variable_get('site_name', 'Drupal Site');
  $is_enterprise = $account->isEnterprise();

  /*
   * Account Information
   */
  $form['information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Account'),
    '#description' => t('Account information, Lingotek identifiers, and Lingotek servers.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
  );

  $activation_details = variable_get('lingotek_activation_first_name', '') . ' ' . variable_get('lingotek_activation_last_name', '');
  $activation_email = variable_get('lingotek_activation_email', '');
  $activation_details .= strlen($activation_email) ? ' (' . $activation_email . ')' : '';
  $activation_details = strlen(trim($activation_details)) ? $activation_details : 'NA';

  $form['information'][] = array(
    '#type' => 'item',
    '#title' => t("Account Status") . ' - ' . l(t('Update'), 'lingotek/get-account-status', array('query' => array('token' => md5(time())))),
    '#markup' => theme('table', array('header' => array(), 'rows' => array(
        array('Status:', $account->getStatusText())
        //,array('Plan:', $account->getPlanTypeText())
        , array('Enterprise:', ($account->isEnterprise() ? '<span style="color: green;">' . t('Yes') . '</span>' : '<span>' . t('No') . '</span>'))
        )))
  );



  $form['information'][] = array(
    '#type' => 'item',
    '#title' => t("Lingotek Identifiers"),
    '#markup' => theme('table', array('header' => array(), 'rows' => array(
        array('Activation Name:', $activation_details),
        array('Community Identifier:', variable_get('lingotek_community_identifier', '')),
        array('OAuth Key:', variable_get('lingotek_oauth_consumer_id', '')),
        array('OAuth Secret:', variable_get('lingotek_oauth_consumer_secret', '')),
        array('Workflow ID:', variable_get('lingotek_workflow', '')),
        array('External ID:', variable_get('lingotek_login_id', '')),
        array('Project ID:', variable_get('lingotek_project', '')),
        array('Vault ID:', variable_get('lingotek_vault', '')),
        array('Notify URL:', lingotek_get_notifications_url()),
        )))
  );

  //if (LINGOTEK_DEV) {
  $form['information'][] = array(
    '#type' => 'item',
    '#title' => t('Lingotek Servers'),
    '#markup' => theme('table', array('header' => array(), 'rows' => array(
        array('TMS:', LINGOTEK_API_SERVER),
        array('GMC:', LINGOTEK_GMC_SERVER),
        array('Billing:', LINGOTEK_BILLING_SERVER),
      //array('AP Key:', LINGOTEK_AP_OAUTH_KEY),
      //array('AP Secret:', LINGOTEK_AP_OAUTH_SECRET),
        )))
  );
  //}

  /*
   * Lingotek connection settings.
   */

  $connection_group_description = ($connected) ? '' :
      t('Connect this site to your Lingotek account by filling in the fields below. If you do not yet have a Lingotek account, you can <a href="@signup_url">sign up</a> to create an ID and collect OAuth credentials. If all fields are complete, there is a problem with one or more of the values.', array('@signup_url' => url(LINGOTEK_API_SERVER . '/lingopoint/portal/communitySignup.action'))
      );

  $edit_connection = isset($_GET['edit_connection']) || !$connected;

  $form['connection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Connection'),
    '#description' => filter_xss($connection_group_description),
    '#collapsible' => TRUE,
    '#collapsed' => !$edit_connection,
    '#group' => 'administrative_settings',
  );

  $status_message = ($connected) ?
      t('<strong>Connection Status</strong>: <span style="color: green;">OK</span>') :
      t('<strong>Connection Status</strong>: <span style="color: red;">Not Connected</span>');

  $form['connection']['status'] = array(
    '#prefix' => '<h4 class="connection-status">',
    '#markup' => $status_message,
    '#suffix' => '</h4>'
  );


  if ($is_enterprise || $edit_connection) {
    $form['connection']['lingotek_login_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Lingotek ID'),
      '#description' => t('Enter the Lingotek ID you use to access the Lingotek Dashboard and Workbench.'),
      '#default_value' => variable_get('lingotek_login_id', ''),
    );

    $form['connection']['lingotek_oauth_consumer_id'] = array(
      '#type' => 'textfield',
      '#title' => t('OAuth Key'),
      '#description' => t('The OAuth Key used to connect with the Lingotek server.'),
      '#default_value' => variable_get('lingotek_oauth_consumer_id', ''),
    );

    $form['connection']['lingotek_oauth_consumer_secret'] = array(
      '#type' => 'textfield',
      '#title' => t('OAuth Secret'),
      '#description' => t('The OAuth Secret used to connect with the Lingotek server.'),
      '#default_value' => variable_get('lingotek_oauth_consumer_secret', ''),
    );
  }

  if ($is_enterprise || $edit_connection) {
    // Note: The Lingotek servers may easily be set in code or by setting a 'lingotek_use_stage_servers' drupal variable
    $form['connection']['lingotek_use_stage_servers'] = array(
      '#type' => 'select',
      '#title' => t('Environment'),
      '#options' => array(
        0 => 'Production',
        1 => 'Staging'
      ),
      '#description' => LINGOTEK_DEV ? t('Note: The above Environment setting above will be overriden by your local configuration (i.e., settings.php).') : '',
      '#default_value' => variable_get('lingotek_use_stage_servers', 0),
    );
  }

  // STOP HERE IF:
  // If the connection isn't yet established.
  // If this isn't an enterprise account.
  if (!$connected) {
    return system_settings_form($form);
  }



  /*
   * Default Settings
   */
  $form['defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Defaults'),
    '#description' => t('Translation management defaults used when creating new nodes. At the node level, these settings can be adjusted.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
  );

  // Upload
  $form['defaults']['lingotek_create_documents_by_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Upload Content Automatically'),
    '#default_value' => variable_get('lingotek_create_documents_by_default', 0),
    //'#disabled' => !$is_enterprise,
    '#description' => t('When enabled, your Drupal content (including saved edits) will automatically be uploaded to Lingotek for translation.<br/>When disabled, you are required to manually upload your content by clicking the "Upload" button on the Lingteok tab.'),
  );

  // Download  
  $form['defaults']['lingotek_sync'] = array(
    '#type' => 'checkbox',
    '#title' => t('Download Translations Automatically'),
    '#default_value' => (variable_get('lingotek_sync', 1) !== 0) ? 1 : 0,
    //'#disabled' => !$is_enterprise,
    '#description' => t('When enabled, completed translations will automatically be downloaded from Lingotek.<br/>When disabled, you are required to manually download translations by clicking the "Download" button on the Lingotek tab.'),
  );

  /*
   * Utilities
   */

  $form['utilities'] = array(
    '#type' => 'fieldset',
    '#title' => t('Utilities'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
  );

  $btn_reset_translations = array(
    '#type' => 'submit',
    '#value' => t('Reset Translations'),
    '#submit' => array('lingotek_batch_reset_content'),
    '#attributes' => array(
      'onclick' => 'return confirm("' . t('Are you sure?\n\nAll of your content will have to be retranslated.') . '");'
    ),
  );

  $btn_cleanup = array(
    '#type' => 'submit',
    '#value' => t('Run Cleanup'),
    '#submit' => array('lingotek_cleanup_utility')
  );

  // Start: Not sure why, but without these below, the table button do not work properly
  $form['utilities'][] = array_merge($btn_reset_translations, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_cleanup, array('#attributes' => array('style' => 'display: none;')));
  // End: Not sure why

  $rows = array(
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Reset Translations') . '<div class="description">',
          '#markup' => t("Disassociates all locally-published translations from their counterparts on Lingotek's servers. This allows for your Drupal content to be re-uploaded to Lingotek, so that it can be retranslated entirely using the currently-defined workflow. Also resets the translation management settings for all nodes to those defined in the Content Defaults Settings."),
          '#suffix' => '</div>'
      )),
      array('data' => $btn_reset_translations)
    ),
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Cleanup') . '<div class="description">',
          '#markup' => t("The cleanup utility identifies translatable content and processes all existing field data for translation-enabled fields on nodes, ensuring that if data was entered before enabling field translation on a field that the existing field data is copied over to the parent node's current language."),
          '#suffix' => '</div>'
      )),
      array('data' => $btn_cleanup),
    )
  );
  $header = array();
  $form['utilities']['utility_table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows))
  );

  if ($is_enterprise) {

    // Community Translation
    $form['defaults']['lingotek_allow_community_translation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Community Translation'),
      '#description' => t('When enabled, anonymous site visitors will be presented with a link allowing them to contribute translations for this node.'),
      '#default_value' => variable_get('lingotek_allow_community_translation', 0),
    );

    /*
     * Misc. Options
     */

    // Options
    $options = array();
    $options[FALSE] = t("Disabled");
    $options[TRUE] = t("Enabled");



    // URL Alias Translation.
    $form['defaults']['lingotek_url_alias_translation'] = array(
      '#type' => 'select',
      '#title' => t('URL Alias Translation'),
      '#description' => t("Choose how you would like to translate the URL alias. The last option requires that you install both the Title and Pathauto modules, and define a path pattern, and check \"Enable Lingotek Translation\" for the Title field."),
      '#options' => lingotek_get_url_alias_translations(),
      '#default_value' => variable_get('lingotek_url_alias_translation', 1),
    );


    // Projects
    $projects = (class_exists('LingotekApi')) ? $api->listProjects() : array();
    $id = variable_get('lingotek_project', '');

    if ($id == '' || !array_key_exists($id, $projects)) { //No project id set, project deleted, or community changed to one without that project.  Try to find the Drupal project
      $id = array_search($site, $projects);
      if ($id === False) { //Setup a default Drupal project
        $id = lingotek_add_project($site);
        $projects = (class_exists('LingotekApi')) ? $api->listProjects() : array();
      }
      else { //Assign to an existing Drupal project
        variable_set('lingotek_project', $id);
      }
    }

    $form['defaults']['lingotek_project'] = array(
      '#type' => 'select',
      '#title' => t('Default Project'),
      '#options' => $projects,
      '#description' => t('The default Lingotek Project with which translations will be associated.'),
      '#default_value' => $id,
    );


    // Workflows
    if ($workflows = $api->listWorkflows()) {
      $form['defaults']['lingotek_workflow'] = array(
        '#type' => 'select',
        '#title' => t('Default Workflow'),
        '#description' => t('The default Workflow to use when translating content.'),
        '#default_value' => variable_get('lingotek_workflow', ''),
        '#options' => $workflows,
      );
    }


    $vaults = $api->listVaults();
    $current_vault_id = variable_get('lingotek_vault', '');
    $personal_vault_count = ( isset($vaults['Personal Vaults']) ) ? count($vaults['Personal Vaults']) : 0;
    $community_vault_count = ( isset($vaults['Community Vaults']) ) ? count($vaults['Community Vaults']) : 0;

    // If no vault id is set, and we don't have any personal vaults, then create one and add it to our project.
    if (( $current_vault_id == '' ) && ( $personal_vault_count == 0 ) && ( $community_vault_count == 0 )) {
      $current_project_id = variable_get('lingotek_project', '');
      // But only if we have a ProjectID.
      if ($current_project_id != '') {
        $current_vault_id = lingotek_add_vault($site);
        lingotek_add_vault_to_project();
      }
    }

    $form['defaults']['lingotek_vault'] = array(
      '#type' => 'select',
      '#title' => t('Default Vault'),
      '#options' => $vaults,
      '#description' => t('The default Translation Memory Vault where translations are saved.'),
      '#default_value' => $current_vault_id,
    );


    /*
     * Comment translation
     */
    $form['comments'] = array(
      '#type' => 'fieldset',
      '#title' => t('Comment Translation'),
      '#description' => t('Enable/disable and set defaults for comment translation.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['comments']['lingotek_translate_comments'] = array(
      '#type' => 'select',
      '#title' => t('Translate comments'),
      '#description' => t('When enabled, comments on nodes of the specified types will be automatically translated.'),
      '#options' => array(
        1 => t('Enabled'),
        0 => t('Disabled'),
      ),
      '#default_value' => variable_get('lingotek_translate_comments', 0),
    );

    $type_options = array();
    foreach (node_type_get_types() as $type => $type_data) {
      $type_options[$type] = $type_data->name;
    }
    $form['comments']['lingotek_translate_comments_node_types'] = array(
      '#type' => 'select',
      '#title' => t('Comment translation node types'),
      '#description' => t('When comment translation is enabled, only automatically translate comments on the selected node types.'),
      '#options' => $type_options,
      '#multiple' => TRUE,
      '#default_value' => variable_get('lingotek_translate_comments_node_types', ''),
    );

    $form['comments']['lingotek_translate_comments_workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Comment translation Workflow'),
      '#description' => t('When comment translation is enabled, use this workflow for translating 
      comments. Since there is no Drupal UI for managing workflow phases for comments, it is 
      recommended that you choose a Workflow that only consists of a single machine translation Phase.'),
      '#options' => $api->listWorkflows(),
      '#default_value' => variable_get('lingotek_translate_comments_workflow_id', ''),
    );


    /*
     * Advanced XML Configuration
     */
    $form['advanced-parsing'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced Content Parsing'),
      '#description' => t('Settings to support advanced parsing of translatable content.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['advanced-parsing']['lingotek_advanced_xml_config1'] = array(
      '#type' => 'textarea',
      '#title' => t('Configuration Settings (Primary)'),
      '#description' => t('Paste in the contents of an advanced configuration file (.fprm). This will be used as the primary set of advanced settings when sending content to Lingotek.'),
      '#default_value' => variable_get('lingotek_advanced_xml_config1'),
    );

    $form['advanced-parsing']['lingotek_advanced_xml_config2'] = array(
      '#type' => 'textarea',
      '#title' => t('Configuration Settings (Secondary)'),
      '#description' => t('Paste in the contents of an advanced configuration file (.fprm). This will be used as the secondary set of advanced settings when sending content to Lingotek.'),
      '#default_value' => variable_get('lingotek_advanced_xml_config2'),
    );

    if (!variable_get('lingotek_advanced_parsing', FALSE)) {
      $form['advanced-parsing']['lingotek_advanced_parsing'] = array(
        '#type' => 'checkbox',
        '#title' => t('Upgrade to advanced content parsing.'),
        '#description' => t('This site is currently using Simple content parsing.
        Check this box to upgrade your site to use advanced content parsing for existing and future content. <strong>Warning:</strong> This will update all current Lingotek-associated content on the site, possibly modifying the state of in-progress translations.'),
      );

      $form['#submit'][] = 'lingotek_handle_advanced_xml_upgrade';
    }
  }
  /*
   * Troubleshooting and Logging - Developer Settings
   */
  $form['troubleshooting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Troubleshooting'),
    '#description' => t('Help troubleshoot any issues with the module.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
  );

  $form['troubleshooting']['lingotek_error_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Error Log'),
    '#description' => t('This prints errors and warnings to the web server\'s error logs in addition to adding them to watchdog.'),
    '#default_value' => variable_get('lingotek_error_log', LINGOTEK_DEV ? TRUE : FALSE),
  );

  $form['troubleshooting']['lingotek_warning_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Warnings Log'),
    '#description' => t('This logs any warnings in watchdog and the web server\'s error logs.'),
    '#default_value' => variable_get('lingotek_warning_log', LINGOTEK_DEV ? TRUE : FALSE),
  );

  $form['troubleshooting']['lingotek_trace_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Trace Log'),
    '#description' => t('This logs trace debug messages to watchdog and the web server\'s error logs. (This logging is extremely verbose.)'),
    '#default_value' => variable_get('lingotek_trace_log', LINGOTEK_DEV ? FALSE : FALSE),
  );

  $form['troubleshooting']['lingotek_api_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable API debugging'),
    '#description' => t('Logs the timing and request/response details of both successful and failing Lingotek API
      calls to the Drupal <a href="@watchdog_path">watchdog</a> in category "lingotek - api".', array('@watchdog_path' => url('admin/reports/dblog'))),
    '#default_value' => variable_get('lingotek_api_debug', LINGOTEK_DEV ? TRUE : FALSE),
  );

  $form['troubleshooting']['lingotek_flush_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Never cache'),
    '#description' => t('Skips caching so you can test easier.  This avoids frequent polling of fresh data from Lingotek.  Only those with Developer permissions will have caching disabled.'),
    '#default_value' => variable_get('lingotek_flush_cache', LINGOTEK_DEV ? TRUE : FALSE),
  );

  $form = system_settings_form($form);

  $form['#submit'][] = 'lingotek_admin_form_submit';

  return $form;
}

/**
 * Submit handler for the admin form.
 */
function lingotek_admin_form_submit($form, $form_state) {
  // Enabling/disabling Lingotek comment translation will have an effect on the comment entity.
  entity_info_cache_clear();
  entity_get_info('comment');
}

/**
 * Custom form handler for upgrading a site from using Lingotek's simple to advanced XML parsing of content.
 */
function lingotek_handle_advanced_xml_upgrade($form, $form_state) {
  if ($form_state['values']['lingotek_advanced_parsing']) {
    $results = db_select('lingotek', 'l')->fields('l', array('nid'))->distinct()->execute();
    $operations = array();
    foreach ($results as $result) {
      $node = node_load($result->nid, NULL, TRUE);
      if (!empty($node->nid)) {
        $operations[] = array('lingotek_advanced_parsing_update_node', array($node->nid));
      }
    }

    $batch = array(
      'title' => t('Lingotek Advanced Parsing Updater'),
      'operations' => $operations,
      'file' => 'lingotek.admin.inc',
      'finished' => 'lingotek_advanced_parsing_update_finished',
    );

    // The admin form might not have finished processing yet, but if we're here, we're moving to advanced processing.
    // Ensure the appropriate variable is already set.
    variable_set('lingotek_advanced_parsing', TRUE);

    batch_set($batch);
  }
}

/**
 * Manually Triggered Account Status Check
 */
function lingotek_get_account_status() {

  $account = LingotekAccount::instance();
  $result = $account->getAccountStatus(); // Returns false on fail.

  if ($result === FALSE) { // There was a problem retrieving the account status.
    drupal_set_message(t('There was an error retrieving your account status.  Please try again later.'), 'error');
    drupal_goto(LINGOTEK_MENU_LANG_BASE_URL . '/settings');
  }
  else { // We got a valid account status.  Send the user back to the Lingotek settings page, via the second cache clearer.
    drupal_set_message(t('Your account status has been updated.'));
    drupal_goto('lingotek/flush-cache');
  }
}

/**
 * Clean out the Drupal Cache so menus will rebuild.
 */
function lingotek_flush_cache() {
  menu_rebuild();
  drupal_goto(LINGOTEK_MENU_LANG_BASE_URL);
}
