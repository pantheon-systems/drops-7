<?php

/**
 * @file
 * Administrative Settings for the module.
 */
include_once('lingotek.session.inc');
module_load_include('batch.inc', 'l10n_update');
module_load_include('admin.inc', 'i18n_string');

/**
 * Form constructor for the administration form.
 *
 * @return array
 *   A FAPI form array.
 */
function lingotek_admin_account_status_form($form, &$form_state, $show_fieldset = FALSE) {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance();

  $is_enterprise = $account->isEnterprise();

  // Account Status

  $account_status = $account->getStatusText();
  //$account_status .= '<br>Enterprise: ' . ($is_enterprise ? 'Yes' : 'No');

  $form['status'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Account'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    /*'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Update account status')
      )
    ),
    '#submit' => array('lingotek_admin_account_status_form_submit')*/
  );

  $form['status']['account_summary'] = array(
    '#type' => 'hidden',
    '#value' => $account_status,
    '#attributes' => array('id' => array('account_summary')),
  );

  // Account Information

  $activation_details = variable_get('lingotek_activation_first_name', '') . ' ' . variable_get('lingotek_activation_last_name', '');
  $activation_email = variable_get('lingotek_activation_email', '');
  $activation_details .= strlen($activation_email) ? ' (' . $activation_email . ')' : '';
  $activation_details = strlen(trim($activation_details)) ? $activation_details : 'NA';

  $form['status'][] = array(
    '#type' => 'item',
    '#markup' => theme('table', array('header' => array(), 'rows' => array(
        array(t('Status:'), $account_status),
        //,array('Plan:', $account->getPlanTypeText()),
        array(t('Enterprise:'), ($is_enterprise ? '<span style="color: green;">' . t('Yes') . '</span>' : '<span>' . t('No') . '</span>')),
        array(t('Activation Name:'), $activation_details),
        array(t('Community Identifier:'), variable_get('lingotek_community_identifier', '')),
        array(t('OAuth Key:'), variable_get('lingotek_oauth_consumer_id', '')),
        array(t('OAuth Secret:'), variable_get('lingotek_oauth_consumer_secret', '')),
        array(t('Workflow ID:'), variable_get('lingotek_workflow', '')),
        array(t('Integration Method ID:'), variable_get('lingotek_integration_method', '')),
        array(t('External ID:'), variable_get('lingotek_login_id', '')),
        array(t('Project ID:'), variable_get('lingotek_project', '')),
        array(t('Vault ID:'), variable_get('lingotek_vault', '')),
        array(t('Notify URL:'), '<span title="' . variable_get('lingotek_notify_url', '') . '">' . truncate_utf8(variable_get('lingotek_notify_url', ''), 45, FALSE, TRUE) . '</span>'),
  )))
  );

  return $form;
}

/**
 * Account Status Submit Handler
 * @param type $form
 * @param type $form_state
 */
function lingotek_admin_account_status_form_submit($form, &$form_state) {
  $account = LingotekAccount::instance();
  $result = $account->getAccountStatus(); // Returns false on fail.

  if ($result === FALSE) { // There was a problem retrieving the account status.
    drupal_set_message(t('There was an error retrieving your account status.  Please try again later.'), 'error');
  }
  else { // We got a valid account status.  Send the user back to the Lingotek settings page, via the second cache clearer.
    drupal_set_message(t('Your account status has been updated.'));
    menu_rebuild();
  }
}

/**
 * Content translation form
 */
function lingotek_admin_node_translation_settings_form($form, &$form_state, $entity_type,  $show_fieldset = FALSE) {

  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $entity_type_info = entity_get_info($entity_type);
  $bundles = $entity_type_info['bundles'];

  $types = array();
  $translate = variable_get('lingotek_enabled_fields', array());
  $translate = isset($translate[$entity_type]) ? $translate[$entity_type] : array();
  $profiles = lingotek_get_profiles();

  $profile_options = array();
  foreach ($profiles as $key => $profile) {
    $profile_options[$key] = $profile['name'];
  }
  $profile_options[LingotekSync::PROFILE_CUSTOM] = 'Custom';
  $profile_options[LingotekSync::PROFILE_DISABLED] = 'Disabled';

  if ($entity_type == 'field_collection_item') {
    $profile_options = array(
      'ENABLED' => 'Enabled',
      LingotekSync::PROFILE_DISABLED => 'Disabled',
    );
    $entity_type_info['label'] = 'Field Collection';
  }

  $entity_profiles = variable_get('lingotek_entity_profiles');

  // What types of fields DO we translate?
  $translatable_field_types = lingotek_get_translatable_field_types();

  $form['entity_type'] = array(
    '#type' => 'hidden',
    '#value' => $entity_type,
  );

  $form['node_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => ($setup_complete ? t('Translate @types', array('@type' => $entity_type_info['label'])) : t('Which content types do you want translated?')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
    ),
    '#submit' => array('lingotek_admin_node_translation_settings_form_submit'),
  );

  if ($setup_complete) {
    $form['node_translation'][] = array(
      '#type' => 'item',
      '#description' => t('Choose what you want translated:'),
    );
  }

  $rows = array();

  foreach ($bundles as $bundle_name => $bundle) {
    $fr = array();
    foreach (field_info_instances($entity_type, $bundle_name) as $field) {
      $field_label = $field['label'];
      $field_machine_name = $field['field_name'];
      $field_type = $field['widget']['type'];
      if (array_search($field_type, $translatable_field_types)) {
        $fr[$field_machine_name] = array(
          '#type' => 'checkbox',
          '#title' => check_plain($field_label),
          '#attributes' => array(
            'id' => array('edit-form-item-' . $bundle_name . '-seperator-' . $field_machine_name),
            'name' => $bundle_name . '_SEPERATOR_' . $field_machine_name,
            'class' => array('field'),
          ),  
          '#id' => 'edit-form-item-' . $bundle_name . '-seperator-' . $field_machine_name,
          '#states' => array(
            'invisible' => array(
              ':input[name="profile_' . $bundle_name . '"]' => array('value' => 'DISABLED'),
            ),
          ),
        );

        $is_enabled = !empty($translate[$bundle_name]) && array_search($field_machine_name, $translate[$bundle_name]) !== FALSE;
        if (!$setup_complete || $is_enabled) {
          $fr[$field_machine_name]['#attributes']['checked'] = 'checked';
        }
      }
    }
    $missing_title = !isset($fr['title_field']) && $entity_type == 'node';
    $missing_subject = !isset($fr['subject_field']) && $entity_type == 'comment';
    if ($missing_title || $missing_subject) {
      $message = $entity_type == 'node' ? 'Title (Note: field will be created.)' : 'Subject (Note: field will be created.)';
      $fr['title_field'] = array(
        '#type' => 'checkbox',
        '#title' => $message,
        '#attributes' => array(
          'id' => array('edit-form-item-' . $bundle_name . '-seperator-title'),
          'name' => 'title_swap_' . $bundle_name,
          'class' => array('field'),
        ),
        '#id' => 'edit-form-item-' . $bundle_name . '-seperator-title',
        '#states' => array(
          'invisible' => array(
            ':input[name="profile_' . $bundle_name . '"]' => array('value' => 'DISABLED'),
          ),
        ),
      );
      if (!$setup_complete) {
        $fr['title_field']['#attributes']['checked'] = 'checked';
      }
    }
    
    $default_profile = $setup_complete ? LingotekSync::PROFILE_DISABLED : 0;
    
    $fr2 = array();
    $fr2['profile_' . $bundle_name] = array(
      '#type' => 'select',
      '#options' => $profile_options,
      '#value' => isset($entity_profiles[$entity_type][$bundle_name]) ? $entity_profiles[$entity_type][$bundle_name] : $default_profile,
      '#attributes' => array(
        'id' => array('edit-form-item-profile-' . $bundle_name),
        'name' => 'profile_' . $bundle_name,
        'class' => array('field'),
      ),
    );
    $rows[$bundle_name] = array(
      array('data' => $bundle['label'], 'width' => '20%'),
      array('data' => drupal_render($fr2)),
      array('data' => drupal_render($fr), 'width' => '65%'),
    );
  }

  $header = array(t('Content Type'), t('Translation Profile *'), t('Fields'));

  if ($entity_type == 'field_collection_item') {
    $header = array(t('Collection Name'), t('Status *'), t('Fields'));
  }

  $variables = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('class' => array('lingotek-content-settings-table')),
  );

  $form['node_translation']['types'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', $variables),
    '#suffix' => '* Note: changing the profile will update existing nodes for all settings except the project, workflow, vault, and method (e.g. node/field)',
  );

  if ($entity_type == 'field_collection_item') {
    $form['node_translation']['types']['#suffix'] = '* Note: Field collections will be uploaded and downloaded at the same time as their parent.';
  }

  $form['node_translation']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Node Translation Settings - Form Submit
 */
function lingotek_admin_node_translation_settings_form_submit($form, &$form_state) {

  $entity_type = $form_state['input']['entity_type'];

  if (isset($form_state['values']['lingotek_nodes_translation_method'])) {
    variable_set('lingotek_nodes_translation_method', $form_state['values']['lingotek_nodes_translation_method']);
  }

  $translate = variable_get('lingotek_enabled_fields', array());
  unset($translate[$entity_type]);
  $operations = array();

  $entity_profiles = variable_get('lingotek_entity_profiles', array());

  foreach ($form_state['input'] as $key => $value) {

    // Look for Selected Content Types and Fields.
    if (FALSE !== strpos($key, '_SEPERATOR_')) {
      // And only if set to translate
      if ($value != 0) {
        $parts = explode('_SEPERATOR_', $key);
        $content_type = $parts[0];
        $content_field = $parts[1];

        //check to make sure that the content type is enabled
        if ($form_state['input']['profile_' . $content_type] != LingotekSync::PROFILE_DISABLED) {
          $translate[$entity_type][$content_type][] = $content_field;

          // Set this content type to be Lingotek translated.
          variable_set('language_content_type_' . $content_type, '1');
          // Set this field to 'translatable'.
          // Update the field via the Field API (Instead of the direct db_update)
          $field = field_info_field($content_field);
          $field['translatable'] = 1;
          field_update_field($field);
        }
      }
    } // END:  Selected Content Types and Fields
    // Look for any nodes we need to do the Title swap for.
    if (FALSE !== strpos($key, 'title_swap_')) {
      // And only if set to swap
      if ($value != 0) {
        $content_type = substr($key, strlen('title_swap_'));

        //check to make sure that the content type is enabled
        if ($form_state['input']['profile_' . $content_type] != LingotekSync::PROFILE_DISABLED) {
          // Do the actual title replacement
          $bundle = $content_type;
          $legacy_field = $entity_type == 'node' ? 'title' : 'subject';

          // Use the Title module to migrate the content.
          if (title_field_replacement_toggle($entity_type, $bundle, $legacy_field)) {
            //title_field_replacement_batch_set($title_entity, $title_bundle, $title_field);
            $operations[] = array('title_field_replacement_batch', array($entity_type, $bundle, $legacy_field));
            $translate[$entity_type][$content_type][] = $legacy_field . '_field';
            $field = field_info_field($legacy_field . '_field');
            $field['translatable'] = 1;
            $operations[] = array('field_update_field', array($field)); //field_update_field($field);
          }
        }
      }
    }
    // Look for any profiles
    if (FALSE !== strpos($key, 'profile_')) {
      $content_type = substr($key, strlen('profile_'));
      $entity_profiles[$entity_type][$content_type] = $value;
    }
  }
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/node-translation-settings';
  variable_set('lingotek_enabled_fields', $translate);
  variable_set('lingotek_entity_profiles', $entity_profiles);
  drupal_set_message(t('Your content types have been updated.'));

  if (count($operations)) {
    $batch = array(
      'title' => t('Preparing content for translation'),
      'operations' => $operations
    );

    batch_set($batch);

    if (is_null(variable_get('lingotek_translate_comments', NULL)) || is_null(variable_get('lingotek_translate_config', NULL))) {
      $redirect = 'admin/config/lingotek/comment-translation-settings';
    }
    else {
      $redirect = LINGOTEK_MENU_LANG_BASE_URL . '/settings';
    }
    batch_process($redirect);
  }
}

function lingotek_admin_profile_usage($id) {
  $query = db_select('node', 'n');
  $query->fields('n', array('nid'));
  $query->leftJoin('lingotek', 'l_profile', 'n.nid = l_profile.nid  and l_profile.lingokey = \'profile\'');
  $query->leftJoin('lingotek', 'l_auto_upload', 'n.nid = l_auto_upload.nid  and l_auto_upload.lingokey = \'create_lingotek_document\'');
  $query->leftJoin('lingotek', 'l_status', 'n.nid = l_status.nid  and l_status.lingokey = \'node_sync_status\'');
  $query->condition('l_status.lingovalue', 'TARGET', '<>');
  $or = lingotek_profile_condition('n', 'l_auto_upload', 'l_profile' , $id);
  $query->condition($or);

  return $query->countQuery()->execute()->fetchField();
}

function lingotek_admin_get_nids_by_profile($id) {
  $query = db_select('node', 'n');
  $query->fields('n', array('nid'));
  $query->leftJoin('lingotek', 'l_profile', 'n.nid = l_profile.nid  and l_profile.lingokey = \'profile\'');
  $query->leftJoin('lingotek', 'l_auto_upload', 'n.nid = l_auto_upload.nid  and l_auto_upload.lingokey = \'create_lingotek_document\'');
  $query->leftJoin('lingotek', 'l_status', 'n.nid = l_status.nid  and l_status.lingokey = \'node_sync_status\'');
  $query->condition('l_status.lingovalue', 'TARGET', '<>');
  $or = lingotek_profile_condition('n', 'l_auto_upload', 'l_profile' , $id);
  $query->condition($or);

  return $query->execute()->fetchCol();
}

function lingotek_admin_profile_usage_by_types($id) {
  $profile_mapping = variable_get('lingotek_entity_profiles');
  $profile_mapping_associations = isset($profile_mapping['node']) ? $profile_mapping['node'] : array();
  $profile_usage = array_count_values($profile_mapping_associations);
  $node_info = entity_get_info('node');
  $node_bundles = array_keys($node_info['bundles']);
  if (!isset($profile_usage['DISABLED'])) {
    $profile_usage['DISABLED'] = 0;
  }
  foreach ($node_bundles as $bundle) {
    if (isset($profile_mapping['node']) && !array_key_exists($bundle, $profile_mapping['node'])) {
      $profile_usage['DISABLED']++;
    }
  }

  return isset($profile_usage[$id]) ? $profile_usage[$id] : 0;
}

/**
 * Additional translation form
 */
function lingotek_admin_comment_translation_settings_form($form, &$form_state, $show_fieldset = FALSE) {
  global $base_url;
  $api = LingotekApi::instance();
  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $account = LingotekAccount::instance();
  $is_enterprise = $account->isEnterprise();
  /*
   * Comment translation
   */
  $form['comment_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => ($setup_complete ? t('Translate Comments') : t('Which comment items do you want translated?')),
    //'#description' => t('Enable/disable and set defaults for comment translation.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save'),
      )
    ),
    '#submit' => array('lingotek_admin_additional_translation_settings_form_submit'),
  );

  $form['comment_translation'][] = array(
    '#type' => 'item',
    '#description' => t('Automatically translate comments on the selected content types.'),
  );

  $type_options = array();
  $type_defaults = array();
  foreach (node_type_get_types() as $type => $type_data) {
    $type_options[$type] = $type_data->name;
    $type_defaults[$type] = $type;
  }
  $form['comment_translation']['lingotek_translate_comments_options']['lingotek_translate_comments_node_types'] = array(
    '#type' => 'checkboxes',
    '#options' => $type_options,
    '#multiple' => TRUE,
    '#default_value' => variable_get('lingotek_translate_comments_node_types', $setup_complete ? array() : $type_defaults),
  );

  if ($is_enterprise) {
    $form['comment_translation']['lingotek_translate_comments_options']['lingotek_translate_comments_workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Workflow'),
      '#description' => t('This workflow will be used to translate comments.
        Since there is no Drupal UI for managing workflow phases for comments, it is
        recommended that you choose a Workflow that only consists of a single machine translation Phase.'),
      '#options' => $api->listWorkflows(),
      '#default_value' => variable_get('lingotek_translate_comments_workflow_id', ''),
    );
  }

  return $form;
}

function lingotek_admin_comment_translation_settings_form_submit($form, &$form_state) {
  $form_state['values']['lingotek_translate_comments'] = FALSE;

  foreach ($form_state['values']['lingotek_translate_comments_node_types'] as $val) {
    if (!empty($val))
      $form_state['values']['lingotek_translate_comments'] = TRUE;
  }

  system_settings_form_submit($form, $form_state);

  //This code configures the content entity and it's fields so that they can be translated.
  if ($form_state['values']['lingotek_translate_comments']) {
    foreach ($form_state['values']['lingotek_translate_comments_node_types'] as $content_type => $value) {
      if (!title_field_replacement_enabled('comment', 'comment_node_' . $content_type, 'subject')) {
        title_field_replacement_toggle('comment', 'comment_node_' . $content_type, 'subject');
        title_field_replacement_batch_set('comment', 'comment_node_' . $content_type, 'subject');
      }
    }

    $field = field_info_field('subject_field');
    $field['translatable'] = TRUE;
    field_update_field($field);

    $field = field_info_field('comment_body');
    $field['translatable'] = TRUE;
    field_update_field($field);

    // This is needed for versions of Drupal core 7.10 and lower. See http://drupal.org/node/1380660 for details.
    drupal_static_reset('field_available_languages');
  }

  // Enabling/disabling Lingotek comment translation will have an effect on the comment entity.
  entity_info_cache_clear();
  entity_get_info('comment');
}

/**
 * Additional translation form
 */
function lingotek_admin_additional_translation_settings_form($form, &$form_state, $show_fieldset = FALSE) {
  global $base_url;
  $api = LingotekApi::instance();
  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $account = LingotekAccount::instance();
  $is_enterprise = $account->isEnterprise();

  // Configuration translation (ie. taxonomies, menus, etc.)
  $form['additional_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => ($setup_complete ? t('Configuration Translation') : t('Which additional items do you want translated?')),
    //'#description' => t('Enable/disable and set defaults for comment translation.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_additional_translation_settings_form_submit'),
  );

  $header = array(
    'Type',
    'Description',
  );

  $fr = array(
    '#id' => 'lingotek_use_translation_from_drupal',
    '#type' => 'checkbox',
    '#title' => t('Use Drupal community translations when available from') . ' ' . l('localize.drupal.org', 'http://localize.drupal.org', array('attributes' => array('target' => '_blank'))),
    '#attributes' => array(
            'id' => array('lingotek_use_translation_from_drupal'),
            'name' => 'lingotek_use_translation_from_drupal',
            'class' => array('field'),
          ),
  );

  if (variable_get('lingotek_use_translation_from_drupal', 1)) {
    $fr['#attributes']['checked'] = 'checked';
  }

  $options = array(
    'lingotek_translate_config_builtins' => array(t('Built-in Interface'), t('When enabled, all built-in strings will be automatically translated.  (Note: this will search all enabled modules for translatable strings, which can take several minutes to process.)') . drupal_render($fr)),
    'lingotek_translate_config_blocks' => array(t('Blocks'), t('When enabled, all blocks will be updated automatically to be translatable in the Languages settings.')),
    'lingotek_translate_config_taxonomies' => array(t('Taxonomy'), t('When enabled, all taxonomy vocabularies will be updated automatically to use translation mode \'Localize\' in the Multilingual Options, in preparation for handling string-based translations of taxonomy vocabularies and terms.')),
    'lingotek_translate_config_menus' => array(t('Menus'), t('When enabled, all menus will be updated to use \'Translate and Localize\' in the Multilingual Options.')),
    'lingotek_translate_config_views' => array(t('Views'), t('When enabled, all configured views will be searched for translatable strings.  Note this will not generally include translation of all results generated by each view.')),
  );

  $defaults = array();

  foreach (array_keys($options) as $config_type) {
    $defaults[$config_type] = variable_get($config_type, 1);
  }

  $form['additional_translation']['config'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $defaults,
  );

  if ($is_enterprise) {
    $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Workflow'),
      '#description' => t('This workflow will be used for handling these additional translation items.'),
      '#options' => $api->listWorkflows(),
      '#default_value' => variable_get('lingotek_translate_config_workflow_id', ''),
    );
  }

  if ($setup_complete) {
    $form['additional_translation']['lingotek_translate_config_options']['view_status'] = array(
      '#type' => 'item',
      '#description' => t('You can view the progress of the configuration translations on the ' . l(t('Translate Interface'), 'admin/config/regional/translate') . ' page.'),
    );
  }
  return $form;
}

/*
 * This function is not used. Should we remove it?
 */

function lingotek_verify_modules_enabled($module_list) {
  $missing_modules = FALSE;
  foreach ($module_list as $mod) {
    if (!(module_exists($mod))) {
      form_set_error(t('missing:@mod', array('@mod' => $mod)), t('Module \'@mod\' must first be enabled.', array('@mod' => $mod)));

      $missing_modules = TRUE;
    }
  }
  if ($missing_modules) {
    return FALSE;
  }
  return TRUE;
}

function lingotek_admin_additional_translation_settings_form_submit($form, &$form_state, $additional_operations=array()) {
  $form_state['values'] = array_merge($form_state['values'], $form_state['values']['config']);

  system_settings_form_submit($form, $form_state);

  $lingotek_use_translation_from_drupal = isset($form_state['input']['lingotek_use_translation_from_drupal']) ? $form_state['input']['lingotek_use_translation_from_drupal'] : 0;
  variable_set('lingotek_use_translation_from_drupal', $lingotek_use_translation_from_drupal);

  if ($lingotek_use_translation_from_drupal) {
    variable_set('l10n_update_rebuild_projects', 1);
  }
  else {
    variable_del('l10n_update_rebuild_projects');
  }

  //if any configuration options are set then config translation is enabled
  if (variable_get('lingotek_translate_config_taxonomies', 0) || variable_get('lingotek_translate_config_blocks', 0) ||
      variable_get('lingotek_translate_config_menus', 0) || variable_get('lingotek_translate_config_views', 0) ||
      variable_get('lingotek_translate_config_builtins', 0)) {
    variable_set('lingotek_translate_config', TRUE);
  }
  else {
    // mark lingotek_translate_config to false if no config is to be translated
    variable_set('lingotek_translate_config', FALSE);
  }

  // prepare for any configuration translation, if applicable
  $config_groups = array();
  if (variable_get('lingotek_translate_config_blocks', 0)) {
    lingotek_admin_prepare_blocks();
    $config_groups['blocks'] = 'blocks';
  }
  if (variable_get('lingotek_translate_config_taxonomies', 0)) {
    lingotek_admin_prepare_taxonomies();
    $config_groups['taxonomy'] = 'taxonomy';
  }
  if (variable_get('lingotek_translate_config_menus', 0)) {
    lingotek_admin_prepare_menus();
    $config_groups['menu'] = 'menu';
  }
  if (variable_get('lingotek_translate_config_views', 0)) {
    lingotek_admin_prepare_views();
    $config_groups['views'] = 'views';
  }
  // refresh all strings for each config type
  if (count($config_groups)) {
    // combine string refresh operations with other additional operations
    $config_refresh_batch = i18n_string_refresh_batch($config_groups, $delete = FALSE);
    if (isset($config_refresh_batch['operations'])) {
      $additional_operations = array_merge($additional_operations, $config_refresh_batch['operations']);
    }
  }
  if (variable_get('lingotek_translate_config_builtins')) {
    lingotek_admin_prepare_builtins($additional_operations);
  }
  else {
    lingotek_admin_setup_nonbuiltins_batch($additional_operations);
  }

  // if this function is being called as part of the setup process, continue to
  // the dashboard to make sure they get a language configured.  Otherwise,
  // put them back to the settings page when finished.
  $final_destination = 'admin/settings/lingotek/settings';
  if (  isset($_SESSION['lingotek_setup_path']) 
        && is_array($_SESSION['lingotek_setup_path'])
        && end($_SESSION['lingotek_setup_path']) == 'admin/config/lingotek/additional-translation-settings') {
    $final_destination = 'admin/settings/lingotek';
  }

  if (count(batch_get())) {
    batch_process($final_destination);
  }
  else {
    drupal_goto($final_destination);
  }
}

function lingotek_admin_setup_nonbuiltins_batch($operations) {
  $batch = array(
    'operations' => $operations,
    'title' => 'Identifying translatable content',
    'init_message' => t('Completing setup process'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('An error has occurred during setup.  Please review the log for more information.'),
    'file' => drupal_get_path('module', 'lingotek') . '/lingotek.admin.inc',
  );
  batch_set($batch);
}

function lingotek_admin_load_l10n_update_batch($modules) {

    // attempt to pull translations using l10n_update module
    module_load_include('inc', 'l10n_update', 'l10n_update.batch');
    $updates = lingotek_check_for_l10n_translations($modules);
    if ($updates) {
      $batch = l10n_update_batch_multiple($updates, variable_get('l10n_update_import_mode', LOCALE_IMPORT_KEEP));
      $batch['title'] = t('Syncing Content and Translations');
      $batch['init_message'] = t('Checking for community translations from Drupal...');
      $batch['finished'] = 'lingotek_admin_module_indexing_finished';
      $batch['error_message'] = t('String-gathering process has encountered an error.');
      $batch['file'] = drupal_get_path('module', 'lingotek') . '/lingotek.admin.inc';
      batch_set($batch);
    }
}

function lingotek_admin_prepare_builtins($additional_operations=array()) {

  // search for all new built-in strings using the potx module, and add
  // them to the locales_source table for lingotek to translate
  $modules_list = lingotek_admin_get_enabled_modules();

  $batch = array(
    'operations' => $additional_operations,
    'title' => t('Gathering translatable strings from all enabled modules'),
    'init_message' => t('Preparing list of modules to be searched for built-in strings...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('String-gathering process has encountered an error.'),
    'file' => drupal_get_path('module', 'lingotek') . '/lingotek.admin.inc',
  );

  $indexed_modules = variable_get('lingotek_translate_config_indexed_modules'); // modules previously scanned
  if (!$indexed_modules) {
    $indexed_modules = array();
  }

  // index modules that have not yet been indexed
  foreach ($modules_list as $module) {
    if (!array_key_exists($module->name, $indexed_modules)) {
      $batch['operations'][] = array('lingotek_admin_add_module_index_job', array($module));
    }
  }
  variable_set('lingotek_translate_config_indexed_modules', $modules_list); // save current list for next time
  if (isset($batch['operations']) && count($batch['operations'] > 0)) {
    $batch['operations'][] = array('lingotek_admin_index_plural_targets', array());
    batch_set($batch);
  }
}

function lingotek_admin_add_module_index_job($module, &$context) {
  $context['message'] = t('Gathering translatable strings in @type: @name', array('@type' => $module->type, '@name' => $module->name));
  $real_uri = $_SERVER['REQUEST_URI']; // save the real URI in order to record module names for the location
  module_load_include('inc', 'potx', 'potx');
  // translations *not* available from localize.drupal.org
  $_SERVER['REQUEST_URI'] = 'module:' . $module->name;
  $path = pathinfo($module->filename);
  $path = DRUPAL_ROOT . '/' . drupal_get_path('module', $module->name);
  $files = _potx_explore_dir($path = $path);
  foreach ($files as $file) {
    set_time_limit(LINGOTEK_FILE_PROCESS_TIMEOUT);
    _potx_process_file($file); // populates the global variables '$potx_strings' and '$potx_install'
  }
  lingotek_admin_save_queued_strings();
  $_SERVER['REQUEST_URI'] = $real_uri;
}

function lingotek_admin_save_queued_strings() {
  $plural_map = variable_get('lingotek_config_plural_mapping', array());
  global $_potx_strings, $_potx_install;

  foreach ($_potx_strings as $k => $v) {
    // handle format_plural
    if (strpos($k, "\0")) {
      list($singular, $plural) = explode("\0", $k);
      locale($singular, $context=NULL, $langcode='und');
      $single_lid = LingotekConfigChunk::getLidBySource($singular);
      locale($plural, $context=NULL, $langcode='und');
      $plural_lid = LingotekConfigChunk::getLidBySource($plural);
      $plural_map[$plural_lid] = array('plid' => $single_lid, 'plural' => 1);
    }
    else {
      // handle regular t-function content
      locale($k, $context=NULL, $langcode='und');
    }
  }
  // TODO: do something with the _potx_install strings

  variable_set('lingotek_config_plural_mapping', $plural_map);
  $_potx_strings = array();
  $_potx_install = array();
}

function lingotek_admin_index_plural_targets(&$context=NULL) {
  if ($context) {
    $context['message'] = t('Indexing translation targets to preserve plurality');
  }
  $plurals = db_select('locales_target', 't')
      ->fields('t', array('lid', 'plid', 'plural'))
      ->condition('plid', 0, '!=')
      ->execute()
      ->fetchAll();
  if ($plurals) {
    $plural_map = variable_get('lingotek_config_plural_mapping', array());
    foreach ($plurals as $p) {
      $plural_map[$p->lid] = array('plid' => $p->plid, 'plural' => $p->plural);
    }
    variable_set('lingotek_config_plural_mapping', $plural_map);
  }
}

function lingotek_check_for_l10n_translations($modules) {
  // return the translations available through Drupal translation and a list of the ones still needed
  module_load_include('project.inc', 'l10n_update');
  module_load_include('check.inc', 'l10n_update');
  $projects = array();
  $available = array();
  $updates = array();

  // Get all current projects, including the recently installed.
  $current_projects = l10n_update_build_projects(TRUE);
  // Collect project data of newly installed projects.
  foreach ($modules as $k => $v) {
    if (isset($current_projects[$k])) {
      $projects[$k] = $current_projects[$k];
    }
  }

  // If a translation is available and if update is required, lets go.
  if ($projects && $available = l10n_update_check_projects($projects)) {
    $history = l10n_update_get_history();
    if ($updates = l10n_update_build_updates($history, $available)) {
      module_load_include('batch.inc', 'l10n_update');
      // Filter out updates in other languages. If no languages, all of them will be updated
      $updates = _l10n_update_prepare_updates($updates);
    }
  }
  return $updates;
}

function lingotek_admin_module_indexing_finished($success, $results) {
  // TODO: consider rewriting more of this function (adapted from l10n_update module)
  $totals = array();    // Sum of added, updated and deleted translations.
  $total_skip = 0;      // Sum of skipped translations
  $messages = array();  // User feedback messages.
  $project_fail = $project_success = array(); // Project names of succesfull and failed imports.
  $t = get_t();

  if ($success) {
    // Summarize results of added, updated, deleted and skiped translations.
    // Added, updated and deleted are summarized per language to be displayed accordingly.
    foreach ($results as $result) {
      // convert to array as needed
      if (is_a($result, 'stdClass')) {
        $result = get_object_vars($result);
      }
      if (isset($result['fail'])) {
        // Collect project names of the failed imports.
        $project_fail[$result['file']->name] = $result['file']->name;
      }
      else {
        $language = $result['language'];

        // Initialize variables to prevent PHP Notices.
        if (!isset($totals[$language])) {
          $totals[$language] = array();
          $totals[$language]['add'] = $totals[$language]['update'] = $totals[$language]['delete'] = 0;
        }

        // Summarize added, updated, deleted and skiped translations.
        $totals[$language]['add'] += $result['add'];
        $totals[$language]['update'] += $result['update'];
        $totals[$language]['delete'] += $result['delete'];
        $total_skip += $result['skip'];

        // Collect project names of the succesfull imports.
        $project_success[$result['file']->name] = $result['file']->name;
      }
    }

    // Messages of succesfull translation update results.
    if ($project_success) {
      $messages[] = format_plural(count($project_success), 'One project updated: @projects.', '@count projects updated: @projects.', array('@projects' => implode(', ', $project_success)));
      $languages = language_list();
      foreach ($totals as $language => $total) {
        $messages[] = $t('%language translation strings added: !add, updated: !update, deleted: !delete.', array(
          '%language' => $languages[$language]->name,
          '!add' => $total['add'],
          '!update' => $total['update'],
          '!delete' => $total['delete'],
        ));
      }
      drupal_set_message(implode("<br />\n", $messages));

      // Warning for disallowed HTML.
      if ($total_skip) {
        drupal_set_message(
          format_plural(
            $total_skip,
            'One translation string was skipped because it contains disallowed HTML. See !log_messages for details.',
            '@count translation strings were skipped because they contain disallowed HTML. See !log_messages for details.',
            array('!log_messages' => l(t('Recent log messages'), 'admin/reports/dblog'))),
        'warning');
      }
    }

    // Error for failed imports.
    if ($project_fail) {
      drupal_set_message(
        format_plural(
          count($project_fail),
          'Translations of one project were not imported: @projects.',
          'Translations of @count projects were not imported: @projects',
          array('@projects' => implode(', ', $project_fail))),
      'error');
    }
  }
  else {
    drupal_set_message($t('Error gathering built-in strings for translation.'), 'error');
  }
}

function lingotek_admin_get_enabled_modules() {
  $query = db_select('system', 's')
      ->fields('s', array('name', 'filename', 'type'))
      ->condition('s.type', array('module', 'theme'), 'IN')
      ->condition('s.status', '0', '!=')
      //->condition('s.filename', 'modules%', 'LIKE')
      ->execute();
  return $query->fetchAllAssoc('name');
}

function lingotek_admin_prepare_blocks() {
  // update all blocks to be translatable in the Languages settings
  db_update('block')
      ->fields(array('i18n_mode' => LINGOTEK_I18N_ENABLED_VALUE))
      ->execute();
  return TRUE;
}

function lingotek_admin_prepare_taxonomies() {
  // update all taxonomies to be updated to use translation mode 'Localize'
  db_update('taxonomy_vocabulary')
      ->fields(array('i18n_mode' => LINGOTEK_TAXONOMY_LOCALIZE_VALUE))
      ->execute();
  return TRUE;
}

function lingotek_admin_prepare_menus() {
  // update all menus to use 'Translate and Localize' in the Multilingual Options
  db_update('menu_custom')
      ->fields(array('i18n_mode' => LINGOTEK_MENU_LOCALIZE_TRANSLATE_VALUE))
      ->execute();
  return TRUE;
}

function lingotek_admin_prepare_views() {
  // WTD:
  return TRUE;
}

/**
 * Advanced Parsing - XML Configuration
 */
function lingotek_admin_advanced_parsing_form($form, &$form_state, $show_fieldset = FALSE) {
  /*
   * Advanced XML Configuration
   */
  $form['advanced-parsing'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Advanced Content Parsing'),
    '#group' => 'administrative_settings',
    '#submit' => array('lingotek_admin_advanced_parsing_form_submit')
  );

  $form['advanced-parsing'][] = array(
    '#type' => 'item',
    '#description' => t('Settings to support advanced parsing of translatable content.'),
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

    $form['advanced-parsing']['#submit'][] = 'lingotek_handle_advanced_xml_upgrade';
  }

  $form['advanced-parsing']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['advanced-parsing']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to Defaults'),
    '#attributes' => array(
      'onclick' => 'return confirm("' . t('Are you sure?\n\nAll of your configuration settings will be reset to system defaults.') . '")',
      ),
  );

  return $form;
}

function lingotek_admin_advanced_parsing_form_submit(&$form, &$form_state) {
  if (isset($form_state['values']['op']) && $form_state['values']['op'] == 'Reset to Defaults') {
    lingotek_set_default_advanced_xml(TRUE);
    drupal_set_message(t('The configuration settings have been reset to system defaults.'));
  }
  else {
    system_settings_form_submit($form, $form_state);
  }
 }

/**
 * Lingotek prefs Form
 */
function lingotek_admin_prefs_form($form, &$form_state, $show_fieldset = FALSE) {
  /*
   * Configuration
   */
  $form['prefs'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Preferences'),
    //'#description' => t('Module preferences.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_prefs_form_submit')
  );

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
    '#title' => t('Enable the default language switcher'),
    '#default_value' => $block_enabled,
  );

  $default_region = "sidebar_first";
  $default_region_value = array_key_exists($block['region'], $block_regions) ? $block['region'] : (array_key_exists($default_region, $block_regions) ? $default_region : NULL);

  $ls_select_region = array(
    '#type' => 'select',
    //'#title' => t('The region where the switcher will be displayed.'),
    '#description' => 'The region where the switcher will be displayed.',
    '#options' => $block_regions,
    '#default_value' => $default_region_value,
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $fkey = 'prefs'; // form key
//  $form[$fkey] = array(
//    '#type' => $show_fieldset ? 'fieldset' : 'item',
//    '#title' => t('Default Language Switcher'),
//    '#collapsible' => TRUE,
//    '#collapsed' => TRUE,
//    '#group' => 'administrative_settings',
//    'actions' => array(
//      '#type' => 'actions',
//      'submit' => array(
//        '#type' => 'submit',
//        '#value' => t('Save configuration')
//      ),
//    /* 'note' => array(
//      '#markup' => t('Note: The default language switcher block is only shown if at least two languages are enabled and language negotiation is set to <i>URL</i> or <i>Session</i>.')
//      ) */
//    )
//  );
//  $form[$fkey][] = array(
//    '#type' => 'item',
//    '#description' => t('Would you like to enable the default language switcher?')
//  );
  $form[$fkey]['theme'] = array(
    '#type' => 'hidden',
    '#value' => $current_theme
  );

  $form[$fkey]['enabled'] = $ls_chkbox_enabled;
  $form[$fkey]['region'] = $ls_select_region;

  $lang_negotiation_info = locale_language_negotiation_info();
  $general_detection_config_url = substr($lang_negotiation_info['locale-url']['config'], 0, strrpos($lang_negotiation_info['locale-url']['config'], "/"));

  $form[$fkey][] = array(
    '#type' => 'item',
    '#description' => t('Note: The default language switcher block is only shown if at least two languages are enabled and language negotiation is set to <i>URL</i> or <i>Session</i>.'
        . '<br>Go to ' . l(t('Language detection and selection'), $general_detection_config_url) . ' to change this.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  /* $language_negotiation = variable_get('language_negotiation_language', array());
    $url_detection_enabled = array_key_exists('locale-url', $language_negotiation);
    $ls_chkbox_detection = array(
    '#type' => 'checkbox',
    '#title' => t('URL detection Enabled'),
    '#default_value' => $url_detection_enabled,
    '#disabled' => TRUE
    );
    $form[$form_key]['url_detection'] = $ls_chkbox_detection;

    $form[$form_key][] = array(
    '#type' => 'markup',
    '#markup' => json_encode($block),
    '#attributes' => array('class' => 'description')
    ); */

  if (module_exists('navbar')) {
    $form[$fkey]['lingotek_navbar_switcher'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable the language switcher in the navbar'),
      '#default_value' => variable_get('lingotek_navbar_switcher'),
    );
  }

  $form['prefs']['lingotek_hide_tlmi'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide top-level menu item'),
    '#description' => t('When hidden, the module can still be accessed under <i>Configuration &gt; Regional and language</i>'),
    '#default_value' => variable_get('lingotek_hide_tlmi', 0),
  );

  $form[$fkey]['lingotek_show_language_label'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show language label on node pages'),
    '#default_value' => variable_get('lingotek_show_language_label'),
    '#description' => t('If checked, language labels will be displayed for nodes that have the \'language selection\' field set to be visible.'),
  );

  $form[$fkey]['lingotek_account_plan_type'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Enterprise Features'),
    '#default_value' => (variable_get('lingotek_account_plan_type') == 'enterprise' ? 1 : 0),
    '#description' => t('Some features may not be available without an') . '&nbsp;<a href="http://www.lingotek.com/drupal_pricing" target="_blank">Enterprise license</a>&nbsp;' . t('for the Lingotek TMS. Call 801.331.7777 for details.'),
  );

  return $form;
}

function lingotek_admin_prefs_form_submit($form, &$form_state) {
  lingotek_admin_language_switcher_form_submit($form, $form_state);
  $form_state['values']['lingotek_account_plan_type'] = ($form['prefs']['lingotek_account_plan_type']['#checked'] ? 'enterprise' : 'standard');
  system_settings_form_submit($form, $form_state);
  menu_rebuild();
}

/**
 * Lingotek Connection Settings Form
 */
function lingotek_admin_connection_form($form, &$form_state, $show_fieldset = FALSE) {

  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $force = isset($_GET['test_connection']) ? TRUE : FALSE;
  $connected = $api->testAuthentication($force);
  $is_enterprise = $account->isEnterprise();


  $connection_error = t('Connect this site to your Lingotek account by filling in the fields below. If you do not yet have a Lingotek account, you can <a href="@signup_url">sign up</a> to create an ID and collect OAuth credentials. If all fields are complete, there is a problem with one or more of the values.', array('@signup_url' => url(LINGOTEK_API_SERVER . '/lingopoint/portal/communitySignup.action')));
  if (!$connected) {
    drupal_set_message($connection_error, 'error');
  }
  else {
    // clear the prior error message
    $errors = drupal_get_messages('error');
    if (isset($errors['error'])) {
      foreach ($errors['error'] as $error) {
        if ($error != $connection_error) {
          drupal_set_message(check_plain($error), 'error');
        }
      }
    }
  }

  $edit_connection = isset($_GET['edit_connection']) || !$connected;

  $status_message = ($connected) ?
      '<span style="color: green;">' . t('Connected') . '</span>' :
      '<span style="color: red;">' . t('Not Connected') . '</span>';

  $form['connection']['connection_summary'] = array(
    '#type' => 'hidden',
    '#value' => $status_message,
    '#attributes' => array('id' => array('connection_summary')),
  );

  /* $form['connection'][] = array(
    '#type' => 'item',
    '#description' => filter_xss($connection_group_description),
    ); */


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
    // Note: The Lingotek servers may easily be set in code or by setting a 'lingotek_use_stage_servers' drupal variable
    $form['connection']['lingotek_use_stage_servers'] = array(
      '#type' => 'select',
      '#title' => t('Lingotek Environment'),
      '#options' => array(
        0 => t('Production'),
        1 => t('Staging'),
      ),
      '#description' => LINGOTEK_DEV ? t('Note: The above Environment setting above will be overriden by your local configuration (i.e., settings.php).') : '',
      '#default_value' => variable_get('lingotek_use_stage_servers', 0),
    );

    $form['connection']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }
  else {
    $form['connection'][] = array(
      '#type' => 'item',
      '#title' => t('Lingotek Servers'),
      '#markup' => theme('table', array('header' => array(), 'rows' => array(
          array('TMS:', LINGOTEK_API_SERVER),
          array('GMC:', LINGOTEK_GMC_SERVER),
        //array('Billing:', LINGOTEK_BILLING_SERVER),
        //array('AP Key:', LINGOTEK_AP_OAUTH_KEY),
        //array('AP Secret:', LINGOTEK_AP_OAUTH_SECRET),
    )))
    );
  }

  //$form = system_settings_form($form);
  if (!($is_enterprise || $edit_connection)) {
    unset($form['connection']['actions']);
  }
  return $form;
}

function lingotek_admin_connection_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);
}

function lingotek_admin_profiles_form($form, &$form_state, $show_fieldset = FALSE) {
  $profiles = lingotek_get_profiles();

  $form['header'] = array(
    '#type' => 'item',
    '#title' => 'Translation Profiles',
    '#description' => 'Translation management defaults used when creating new nodes. At the node level, these settings can be adjusted.'
  );

  $headers = array(
    'Profile Name',
    'Usage',
    'Actions',
  );

  $total_count_with_profiles = 0;
  $rows = array();

  foreach ($profiles as $key => $profile) {
    $edit_link =  l(t('Edit'), LINGOTEK_MENU_MAIN_BASE_URL . '/settings/profile/' . $key, array('attributes' => array('class' => array('ctools-use-modal'))));

    $count = lingotek_admin_profile_usage($key);
    $count_types = lingotek_admin_profile_usage_by_types($key);
    $total_count_with_profiles += $count;

    $rows[] = array($profile['name'], $count . ' ' . format_plural($count, 'node', 'nodes') . ' &sdot;  ' . $count_types . ' ' . format_plural($count_types, 'content type', 'content types') . '', $edit_link);
  }

  $custom_count = lingotek_admin_profile_usage(LingotekSync::PROFILE_CUSTOM);
  $custom_count_types = lingotek_admin_profile_usage_by_types(LingotekSync::PROFILE_CUSTOM);
  $rows[] = array(t('Custom'), $custom_count . ' ' . format_plural($custom_count, 'node', 'nodes') . ' &sdot; ' . $custom_count_types . ' ' . format_plural($count_types, 'content type', 'content types'), '');

  $total_node_count = current(db_query("SELECT COUNT(DISTINCT(n.nid)) FROM {node} n WHERE n.status=1")->fetchCol());
  $disabled_count = ($total_node_count - ($total_count_with_profiles + $custom_count)); // lingotek_admin_profile_usage(LingotekSync::PROFILE_DISABLED);
  $disabled_count_types = lingotek_admin_profile_usage_by_types(LingotekSync::PROFILE_DISABLED);
  $rows[] = array(t('Disabled'), $disabled_count . ' ' . format_plural($disabled_count, 'node', 'nodes') . ' &sdot; ' . $disabled_count_types . ' ' . format_plural($disabled_count_types, 'content type', 'content types'), '');

  $variables = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array('id' => 'lingotek-profiles'),
  );

  $form['profiles'] = array(
    '#markup' => theme('table', $variables),
  );

  $form['add'] = array(
    '#markup' => '<p>' . l(t('Add new profile'), LINGOTEK_MENU_MAIN_BASE_URL . '/settings/profile/add', array('attributes' => array('class' => 'ctools-use-modal'))) . '</p>',
  );

  return $form;
}

function lingotek_admin_profile_manage($profile_id) {
  $profiles = lingotek_get_profiles();

  if ($profile_id == 'add') {
    $profile = (object) array(
      'name' => '',
      'lingotek_nodes_translation_method' => 'field',
      'create_lingotek_document' => 1,
      'sync_method' => 1,
      'allow_community_translation' => 0,
      'url_alias_translation' => 0,
    );
    end($profiles);
    $profile_id = key($profiles) + 1; //set the id to one more than the last
  }
  else {
     $profile = lingotek_get_global_profile();
     $profile = array_merge($profile, $profiles[$profile_id]);
  }

  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
  );

  $form_state['values']['profile_id'] = $profile_id;

  foreach ($profile as $key => $value) {
    $form_state['values'][$key] = $value;
  }

  $output = ctools_modal_form_wrapper('lingotek_admin_profile_form', $form_state);

  if (!empty($form_state['executed'])) {
    // Create ajax command array, dismiss the modal window.
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    drupal_exit();
  }

  print ajax_render($output);
}

/**
 * Content defaults Form
 */
function lingotek_admin_profile_form($form, &$form_state, $show_fieldset = FALSE) {

  $account = LingotekAccount::instance();
  $api = LingotekApi::instance();
  $site = variable_get('site_name', 'Drupal Site');
  $is_enterprise = $account->isEnterprise();
  $workbench_moderation_enabled = FALSE;
  if (module_exists('workbench_moderation')) {
    $workbench_moderation_enabled = TRUE;
  }

  $form['defaults']['profile_id'] = array(
    '#type' => 'value',
    '#value' => $form_state['values']['profile_id'],
  );

  $form['defaults']['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Profile Name',
    '#default_value' => $form_state['values']['name'],
  );

  $form['defaults']['current_future_note'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>' . t('Profile settings impacting all nodes (new and existing)') . '</h3><hr />',
  );
  // Upload
  $form['defaults']['create_lingotek_document'] = array(
    '#type' => 'checkbox',
    '#title' => t('Upload Content Automatically'),
    '#default_value' => $form_state['values']['create_lingotek_document'],
    //'#disabled' => !$is_enterprise,
    '#description' => t('When enabled, your Drupal content (including saved edits) will automatically be uploaded to Lingotek for translation.<br/>When disabled, you are required to manually upload your content by clicking the "Upload" button on the Translations tab.'),
  );

  if ($workbench_moderation_enabled) {
    $form['defaults']['create_lingotek_document_workbench_moderation'] = array(
      '#type' => 'select',
      '#title' => 'Workbench Moderation Uploads',
      '#options' => lingotek_get_workbench_moderation_states(),
      '#default_value' => isset($form_state['values']['create_lingotek_document_workbench_moderation']) ? $form_state['values']['create_lingotek_document_workbench_moderation'] : workbench_moderation_state_published(),
      '#states' => array(
        'invisible' => array(
          ':input[name="create_lingotek_document"]' => array('checked' => FALSE),
        ),
      ),
      '#description' => 'The most recent revision will be automatically uploaded when changed to this state.',
    );
  }

  // Download
  $form['defaults']['sync_method'] = array(
    '#type' => 'checkbox',
    '#title' => t('Download Translations Automatically'),
    '#default_value' => $form_state['values']['sync_method'],
    '#description' => t('When enabled, completed translations will automatically be downloaded from Lingotek.<br/>When disabled, you are required to manually download translations by clicking the "Download" button on the Translations tab.'),
  );

  if ($workbench_moderation_enabled) {
    $transition_select = lingotek_workbench_moderation_get_mult_transitions();

    $form['defaults']['lingotek_sync_wb']['sync_method_workbench_moderation'] = array(
      '#type' => 'select',
      '#title' => t('Workbench Moderation Downloads'),
//      '#field_prefix' => t('After translations have downloaded'),
      '#field_suffix' => t('after translations have downloaded.'),
      '#options' => lingotek_get_workbench_moderation_options(),
      '#default_value' => isset($form_state['values']['sync_method_workbench_moderation']) ? $form_state['values']['sync_method_workbench_moderation'] : 'no_moderation',
      '#description' => t("Transitions will not occur until <i>all</i> of a node's translations have downloaded."),
      '#states' => array(
        'invisible' => array(
          ':input[name="sync_method"]' => array('checked' => FALSE),
        ),
      ),
    );

    if (!empty($transition_select)) {
      $form['defaults']['lingotek_sync_wb']['wb_select'] = array(
        '#type' => 'item',
        '#states' => array(
          'visible' => array(
            'select[name="sync_method_workbench_moderation"]' => array('value' => 'increment'),
            ':input[name="sync_method"]' => array('checked' => TRUE),
          ),
        ),
      );
      $form['defaults']['lingotek_sync_wb']['wb_select']['helper'] = array(
        '#type' => 'item',
        '#description' => t('Your configuration of the Workbench Moderation module has multiple transition paths.<br>Choose the state that you would like the translations to transition to below.'),
      );
      $header = array('Current State', 'Transitions to');
      $form['defaults']['lingotek_sync_wb']['wb_select']['wb_table'] = array(
        '#type' => 'container',
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => array(),
      );
      foreach ($transition_select as $from_state => $to_states) {
        $selected = isset($form_state['values']['lingotek_sync_wb_select_' . $from_state]) ? $form_state['values']['lingotek_sync_wb_select_' . $from_state] : ''; // get default
        $select_str = '<div class="form-item form-type-select form-item-lingotek-sync-wb-select-' . $from_state . '">'; // HTML for a Drupal form select element
        $select_str .= '<select id="lingotek_sync_wb_select_' . $from_state . '" name="lingotek_sync_wb_select_' . $from_state . '"';
        $select_str .= ' class="form-select">';
        foreach ($to_states as $state) {
          $select_str .= '<option ';
          $select_str .= 'value="' . $state . '"';
          if ($selected && $selected == $state) { // select default
            $select_str .= ' selected="selected"';
          }
          $select_str .= '>' . $state . '</option>';
        }
        // foreach option add to string
        $select_str .= '</select></div>';
        $form['defaults']['lingotek_sync_wb']['wb_select']['wb_table']['#rows'][] = array(
          array('data' => $from_state, 'width' => '20%'),
          array('data' => $select_str, 'width' => '80%'),
        ); // Get out of _POST
      }
    }
  }

  if ($is_enterprise) {

    // Community Translation
    $form['defaults']['allow_community_translation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Crowdsourced Translation'),
      '#description' => t('When enabled, anonymous site visitors will be presented with a link allowing them to contribute translations for this node.'),
      '#default_value' => $form_state['values']['allow_community_translation'],
    );

    /*
     * Misc. Options
     */

    // URL Alias Translation.
    $form['defaults']['url_alias_translation'] = array(
      '#type' => 'select',
      '#title' => t('URL Alias Translation'),
      '#description' => t("Choose how you would like to translate the URL alias. The last option requires that you install both the Title and Pathauto modules, and define a path pattern, and check \"Enable Lingotek Translation\" for the Title field."),
      '#options' => lingotek_get_url_alias_translations(),
      '#default_value' => $form_state['values']['url_alias_translation'],
    );
  }
  
  
  // Workflows
  $workflows = $api->listWorkflows();
  if ($workflows && count($workflows) > 1) {
    $profiles = variable_get('lingotek_profiles');
    $profile_id = (isset($form['defaults']['profile_id']['#value']) ? $form['defaults']['profile_id']['#value'] : -1);
    $curr_workflow_id = (isset($profiles[$profile_id]['workflow_id']) ? $profiles[$profile_id]['workflow_id'] : '');

    $form['defaults']['workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Default Workflow'),
      '#description' => t('The default Workflow to use when translating content.'),
      '#default_value' => $curr_workflow_id,
      '#options' => $workflows,
      '#ajax' => array(
        'callback' => 'lingotek_profile_default_workflow_form_callback',
        'wrapper' => 'prefill-phases-div',
        'method' => 'replace',
        'effect' => 'fade',
      ),
      '#prefix' => '<div id="prefill-phases-div">',
    );
    $form['defaults']['prefill_phases_checkbox'] = array(
      '#type' => 'checkbox',
      '#title' => t('Change all current content in this profile (@total in total) to use the new workflow', array('@total' => lingotek_admin_profile_usage($profile_id))),
      '#default_value' => FALSE,
      '#description' => t('All current translations will be removed and recreated using the new workflow, with translations pulled from the previous workflow'),
      '#states' => array(
        'invisible' => array(':input[name="workflow_id"]' => array('value' => $curr_workflow_id)),
      )
    );

    $form['defaults']['prefill_phase_select'] = array(
      '#title' => t("Desired Prefill Phase"),
      '#description' => t('Please select the highest phase which should be prefilled for the new workflow'),
      '#type' => 'select',
      '#states' => array(
          'visible' => array(':input[name="workflow_id"]' => array('!value' => $curr_workflow_id),
                             ':input[name="prefill_phases_checkbox"]' => array('checked' => true)
                        ),
        ),
      '#suffix' => '</div>',
    );
    $form['defaults']['prefill_phase_select']['#options'] = lingotek_get_phases_by_workflow_id($form_state['values']['workflow_id']);
  }

  // FUTURE-ONLY STUFF
  $form['defaults']['future_only_note'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>' . t('Profile settings impacting only new nodes') . '</h3><hr />',
  );
  
  $form['defaults']['lingotek_nodes_translation_method'] = array(
    '#type' => 'radios',
    '#title' => t('Translation Storage'),
    '#options' => array(
      'field' => '<strong>Field Translation</strong> (Recommended) - all translations are stored in a single node',
      'node' => '<strong>Node Translation</strong>  - create a new node per language ',
    ),
    'field' => array(
      '#description' => 'A newer and recommended method. Newer versions of Drupal are expected to use this method.',
    ),
    'node' => array(
      '#description' => 'The classical method. Use for backwards compatibility.',
    ),

    '#default_value' => isset($form_state['values']['lingotek_nodes_translation_method']) ? $form_state['values']['lingotek_nodes_translation_method'] : 'field',
  );

  if ($is_enterprise) {

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

    $sorted = asort($projects);
    $form['defaults']['project_id'] = array(
      '#type' => 'select',
      '#title' => t('Default Project'),
      '#options' => $projects,
      '#description' => t('The default Lingotek Project with which translations will be associated.'),
      '#default_value' => isset($form_state['values']['project_id']) ? $form_state['values']['project_id'] : $id,
    );

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

    $form['defaults']['vault_id'] = array(
      '#type' => 'select',
      '#title' => t('Default Vault'),
      '#options' => $vaults,
      '#description' => t('The default Translation Memory Vault where translations are saved.'),
      '#default_value' => isset($form_state['values']['vault_id']) ? $form_state['values']['vault_id'] : $current_vault_id,
    );
  }

  $form['defaults']['save'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  $profile_id = $form_state['values']['profile_id'];
  $first_custom_id = 2;
  $is_custom_profile_id = $profile_id >= $first_custom_id;
  if (lingotek_admin_profile_usage($profile_id) == 0 && lingotek_admin_profile_usage_by_types($profile_id) == 0 && $is_custom_profile_id) {
    $form['defaults']['delete'] = array(
      '#type' => 'submit',
      '#value' => 'Delete',
    );
  }
  else {
    $markup = $is_custom_profile_id ? t('You can only delete this profile when there are no nodes or content types using it.') : t('This profile cannot be deleted.');
    $form['delete'] = array(
      '#markup' => '<span class="ltk-muted" style="font-style: italic;">' . $markup . '</span>',
    );
  }
  $form['defaults']['cancel'] = array(
    '#type' => 'submit',
    '#value' => 'Cancel',
  );

  return $form;
}

function lingotek_profile_default_workflow_form_callback($form, $form_state) {
  return array(
    $form['defaults']['workflow_id'],
    $form['defaults']['prefill_phases_checkbox'],
    $form['defaults']['prefill_phase_select'],

  );
}

function lingotek_admin_profile_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == 'Save') {
    $profiles = variable_get('lingotek_profiles');
    $profile = array();

    $profile['name'] = $form_state['values']['name'];
    foreach (lingotek_get_profile_fields(TRUE, TRUE) as $key) {
      if (isset($form_state['values'][$key])) {
        $profile[$key] = $form_state['values'][$key];
      }
    }

    if (module_exists('workbench_moderation')) { // set or unset multiple transition globals based on form selection
      $states = lingotek_get_workbench_moderation_states();
      if ($form_state['values']['sync_method'] == 1) {
        if ($form_state['values']['sync_method_workbench_moderation'] == 'increment') {
          foreach ($states as $state) {
            if (isset($form_state['input']['lingotek_sync_wb_select_' . $state])) {
              $profile['lingotek_sync_wb_select_' . $state] = $form_state['input']['lingotek_sync_wb_select_' . $state];
            }
          }
        }
      }
    }
    
    // If the workflow has changed and if current translations should be changed,
    // then pull all nodes associated with this profile and update the workflow
    // on TMS, including the correct phase.
    if (isset($form_state['values']['profile_id'])
        && isset($form_state['values']['prefill_phases_checkbox']) 
        && $form_state['values']['prefill_phases_checkbox']) {
      $profile_id = $form_state['values']['profile_id'];
      $workflow_id = $form_state['values']['workflow_id'];
      $prefill_phase = $form_state['values']['prefill_phase_select'];

      $nids = lingotek_admin_get_nids_by_profile($profile_id);
      $document_ids = LingotekSync::getDocIdsFromNodeIds($nids);
      if (!$document_ids) {
        return;
      }
      $api = LingotekApi::instance();
      $api->changeWorkflow($document_ids, $workflow_id, $prefill_phase);

      // CREATE/UPDATE WORKFLOW ENTRIES IN THE LINGOTEK METADATA TABLE
      foreach ($nids as $nid) {
        lingotek_lingonode($nid, 'workflow_id', $workflow_id);
      }

    }

    $profiles[$form_state['values']['profile_id']] = $profile;

    variable_set('lingotek_profiles', $profiles);
  }
  elseif ($form_state['values']['op'] == 'Delete') {
    $profiles = variable_get('lingotek_profiles');
    unset($profiles[$form_state['values']['profile_id']]);
    variable_set('lingotek_profiles', $profiles);
  }
  elseif ($form_state['values']['op'] == 'Cancel') {
    //do nothing
  }
}

/**
 * Utilities Form
 */
function lingotek_admin_utilities_form($form, &$form_state, $show_fieldset = FALSE) {

  $form['utilities'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Utilities'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
      //'#group' => 'administrative_settings',
  );

  $btn_disassociate_translations = array(
    '#type' => 'submit',
    '#value' => t('Disassociate'),
    '#submit' => array('lingotek_batch_disassociate_content'),
    '#attributes' => array(
      'onclick' => 'return confirm("' . t('Are you sure?\n\nAll of your content will have to be retranslated.') . '");'
    ),
  );

  $btn_cleanup = array(
    '#type' => 'submit',
    '#value' => t('Cleanup'),
    '#submit' => array('lingotek_cleanup_utility')
  );

  $btn_callback_url_update = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#submit' => array('lingotek_notify_url_update')
  );

  $btn_callback_refresh_api_cache = array(
    '#type' => 'submit',
    '#value' => t('Refresh'),
    '#submit' => array('lingotek_refresh_api_cache')
  );

  // Start: Not sure why, but without these below, the table button do not work properly
  $form['utilities'][] = array_merge($btn_disassociate_translations, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_cleanup, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_callback_url_update, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_callback_refresh_api_cache, array('#attributes' => array('style' => 'display: none;')));
  // End: Not sure why

  $rows = array(
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Refresh Project, Workflow, and Vault Information') . '<div class="description">',
          '#markup' => t("This module locally caches the available projects, workflows, and vaults.  Use this utility whenever you need to pull down names for any newly created projects, workflows, or vaults from the Lingotek Translation Management System."),
          '#suffix' => '</div>'
        )),
      array('data' => $btn_callback_refresh_api_cache),
    ),
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Update Notification Callback URL') . '<div class="description">',
          '#markup' => t("Update the notification callback URL.  This can be run whenever your site is moved (e.g., domain name change or sub-directory re-location) or whenever you would like your security token re-generated.<br/>")
          . '<br/>' . t("Current notification callback URL:") . ' <i>' . variable_get('lingotek_notify_url', 'None') . '</i>',
          '#suffix' => '</div>'
        )),
      array('data' => $btn_callback_url_update),
    ),
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Cleanup and Identify Translatable Content') . '<div class="description">',
          '#markup' => t("The cleanup utility identifies translatable content and processes all existing data for translation-enabled fields on nodes, ensuring that if data was entered before enabling field translation on a field that the existing field data is copied over to the parent node's current language."),
          '#suffix' => '</div>'
        )),
      array('data' => $btn_cleanup),
    ),
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Disassociate All Translations') . ' <i>' . t('(use with caution)') . '</i>' . '<div class="description">',
          '#markup' => t("Should only be used to change the Lingotek project or TM vault associated with the node’s translation. Disassociates node translations on Lingotek’s servers from the copies downloaded to Drupal. Additional translation using Lingotek will require re-uploading the node’s content to restart the translation process."),
          '#suffix' => '</div>'
        )),
      array('data' => $btn_disassociate_translations)
    ),
  );
  $header = array();
  $form['utilities']['utility_table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows))
  );

  return $form;
}

/**
 * Troubleshooting and Logging Form
 */
function lingotek_admin_logging_form($form, &$form_state, $show_fieldset = FALSE) {

  $form['logging'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Logging'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_logging_form_submit')
  );

  $form['logging'][] = array(
    '#type' => 'item',
    '#description' => t('Help troubleshoot any issues with the module. The logging enabled below will be available in the') . ' ' . l(t('Drupal watchdog'), 'admin/reports/dblog') . '.'
  );

  $form['logging']['lingotek_error_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Error Logging'),
    '#description' => t('This prints errors and warnings to the web server\'s error logs in addition to adding them to watchdog.'),
    '#default_value' => variable_get('lingotek_error_log', 1),
  );

  $form['logging']['lingotek_warning_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Warning Logging'),
    '#description' => t('This logs any warnings in watchdog and the web server\'s error logs.'),
    '#default_value' => variable_get('lingotek_warning_log', 1),
  );

  $form['logging']['lingotek_api_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('API & Interaction Logging'),
    '#description' => t('Logs the timing and request/response details of all Lingotek API calls. Additionally, interaction calls (e.g., endpoint, notifications) made back to Drupal will be logged with this enabled.'),
    '#default_value' => variable_get('lingotek_api_debug', LINGOTEK_DEV ? 1 : 0),
  );

  $form['logging']['lingotek_trace_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Trace Logging'),
    '#description' => t('This logs trace debug messages to watchdog and the web server\'s error logs. (This logging is extremely verbose.)'),
    '#default_value' => variable_get('lingotek_trace_log', 0),
  );

  $form['logging']['lingotek_flush_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Never cache'),
    '#description' => t('Skips caching so you can test easier.  This avoids frequent polling of fresh data from Lingotek.  Only those with Developer permissions will have caching disabled.'),
    '#default_value' => variable_get('lingotek_flush_cache', 0),
  );
  return $form;
}

function lingotek_admin_logging_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);
}

function lingotek_admin_configuration_view($form_short_id = NULL, $show_fieldset = TRUE) {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $site = variable_get('site_name', 'Drupal Site');
  $is_enterprise = $account->isEnterprise();

  //$form_short_id values:  config, logging, utilities, language_switcher
  $form_id = "lingotek_admin_{$form_short_id}_form";
  if (!is_null($form_short_id) && function_exists($form_id)) {
    return drupal_get_form($form_id);
  }

  ctools_include('modal');
  ctools_modal_add_js();

  $show_fieldset = FALSE;

  $output = array();

  $output['lingotek'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'lingotek') . '/js/lingotek.admin.js'),
    ),
  );

  $account_summary = array(
    drupal_get_form('lingotek_admin_account_status_form', $show_fieldset),
    drupal_get_form('lingotek_admin_connection_form', $show_fieldset),
  );

  $output['lingotek'][] = lingotek_wrap_in_fieldset($account_summary, 'Account');

  $connected = $api->testAuthentication();

  if ($connected) {
    /*$arr = array(
      drupal_get_form('lingotek_admin_node_translation_settings_form', $show_fieldset),
    );
    if ($is_enterprise) {
      $arr[] = drupal_get_form('lingotek_admin_advanced_parsing_form', TRUE);
    }*/

    foreach (lingotek_managed_entity_types() as $machine_name => $entity_type) {
      $entity_types[] = $machine_name;
      $entity_settings = drupal_get_form('lingotek_admin_node_translation_settings_form', $machine_name, $show_fieldset);
      $output['lingotek'][] = lingotek_wrap_in_fieldset($entity_settings, t('Translate @types', array('@type' => $entity_type['label'])));
    }

    //$output['lingotek'][] = lingotek_wrap_in_fieldset($arr, 'Translate Content');
    
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_comment_translation_settings_form', $show_fieldset), 'Translate Comments');
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_additional_translation_settings_form', $show_fieldset), 'Translate Configuration');
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_profiles_form', $show_fieldset), 'Translation Profiles');
    if ($is_enterprise) {
      $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_advanced_parsing_form', TRUE), 'Content Parsing (Advanced)');
    }
   
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_prefs_form', $show_fieldset), 'Preferences');

    $logging_utilities = array(
      drupal_get_form('lingotek_admin_logging_form', $show_fieldset),
      drupal_get_form('lingotek_admin_utilities_form', $show_fieldset),
    );
    $output['lingotek'][] = lingotek_wrap_in_fieldset($logging_utilities, 'Logging & Utilities');
  }



  return $output;
}

function lingotek_wrap_in_fieldset($form, $title) {
  return array(
    '#type' => 'fieldset',
    '#group' => 'lingotek',
    '#title' => t($title),
    '#attributes' => array('class' => array('lingotek-' . strtolower(str_replace(' ', '-', $title)))),
    'children' => $form,
  );
}

function lingotek_admin_language_switcher_form_submit($form, $form_state) {

  $block = array(
    'status' => $form_state['values']['enabled'],
    'weight' => 0,
    'region' => $form_state['values']['region'],
    'module' => 'locale',
    'delta' => 'language',
    'theme' => $form_state['values']['theme']
  );

  db_update('block')
      ->fields(array(
        'status' => $block['status'],
        'weight' => $block['weight'],
        'region' => $block['region'],
      ))
      ->condition('module', $block['module'])
      ->condition('delta', $block['delta'])
      ->condition('theme', $block['theme'])
      ->execute();

  //If the site does not yet have more than 1 language, drupal doesn't bootstrap
  //some language functions. So here we force this file to be loaded.
  include_once DRUPAL_ROOT . '/includes/language.inc';

  //The array variable 'language_negotiation_language' has a entry for each detection method.
  //Thus if the max is 1 the only possible enabled method is default.
  //Note that the variable is empty on a new drupal install.
  if ($form_state['values']['enabled'] && count(variable_get('language_negotiation_language')) <= 1) {
    $types = array(
      LOCALE_LANGUAGE_NEGOTIATION_URL => 2,
      LOCALE_LANGUAGE_NEGOTIATION_BROWSER => 4,
    );
    language_negotiation_set(LANGUAGE_TYPE_INTERFACE, $types);
  }

  if (isset($form_state['values']['lingotek_navbar_switcher'])) {
    variable_set('lingotek_navbar_switcher', $form_state['values']['lingotek_navbar_switcher']);
  }

  // Flush cache
  cache_clear_all();

  $action = $form_state['values']['enabled'] ? t('now enabled') : t('not enabled');
  $message_type = $form_state['values']['enabled'] ? 'status' : 'warning';
  drupal_set_message(t('The default language switcher is @action.', array('@action' => $action)), $message_type);
}

/**
 * Custom form handler for upgrading a site from using Lingotek's simple to advanced XML parsing of content.
 */
function lingotek_handle_advanced_xml_upgrade($form, $form_state) {
  if ($form_state['values']['lingotek_advanced_parsing']) {
    $results = db_select('lingotek', 'l')->fields('l', array('nid'))->distinct()->execute();
    $operations = array();
    foreach ($results as $result) {
      $node = lingotek_node_load_default($result->nid, NULL, TRUE);
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

/*
 * AJAX callback to update transition options
 */

function lingotek_workbench_moderation_callback_transitions_default($form, &$form_state) {
  return $form['defaults']['lingotek_sync_wb_select'];
}
