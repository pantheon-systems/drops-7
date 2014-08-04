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

  $show_advanced = $account->showAdvanced();

  // Account Status

  $account_status = $account->getStatusText();

  $form['status'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Account'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
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
        array(t('Enterprise:'), ($show_advanced ? '<span style="color: green;">' . t('Yes') . '</span>' : '<span>' . t('No') . '</span>')),
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
 * Content translation form
 */
function lingotek_admin_entity_bundle_profiles_form($form, &$form_state, $entity_type,  $show_fieldset = FALSE) {

  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $entity_type_info = entity_get_info($entity_type);
  $bundles = $entity_type_info['bundles'];

  // Add the default bundle related to message types, if applicable.
  if ($entity_type == 'message_type' && module_exists('message')) {
    $bundles['message_type'] = array(
      'label' => 'message_type',
      'rdf_mapping' => array(),
    );
  }

  $types = array();
  $translate = variable_get('lingotek_enabled_fields', array());
  $translate = isset($translate[$entity_type]) ? $translate[$entity_type] : array();

  $profiles_options = lingotek_get_profiles_by_name(); // these will come translated via the t-function

  if ($entity_type == 'field_collection_item') {
    $profiles_options = array(
      'ENABLED' => t('Enabled'),
      LingotekSync::PROFILE_DISABLED => t('Disabled'),
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

  $selector = '.lingotek-content-settings-table.' . $entity_type;
  $enable_all =  l(t('(set all to automatic)'), '#', array('attributes' => array('onclick' => "lingotekSetAll('" . $selector . "', '0'); return false;")));
  $enable_all_manual =  l(t('(set all to manual)'), '#', array('attributes' => array('onclick' => "lingotekSetAll('" . $selector . "', '1'); return false;")));  
  $disable_all =  l(t('(disable all)'), '#', array('attributes' => array('onclick' => "lingotekSetAll('" . $selector . "', 'DISABLED'); return false;")));

  $title_str = ($setup_complete ? t('Translate @types', array('@type' => $entity_type_info['label'])) : t('Which content types do you want translated?'));
  $links_str = t('') . ' ' . $enable_all . ' ' . $enable_all_manual . ' ' . $disable_all;

  $form['translation_' . $entity_type] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => check_plain($title_str),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
    ),
    '#submit' => array('lingotek_admin_entity_bundle_profiles_form_submit'),
  );

  $form['translation_' . $entity_type][] = array(
    '#type' => 'item',
    '#description' => $links_str,
  );

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
      $message = $entity_type == 'node' ? t('Title (Note: field will be created.)') : t('Subject (Note: field will be created.)');
      $fr['title_field'] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($message),
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
      '#options' => $profiles_options,
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
    'attributes' => array('class' => array('lingotek-content-settings-table', $entity_type)),
  );

  $form['translation_' . $entity_type]['types'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', $variables),
  );

  if ($setup_complete) {
    $form['translation_' . $entity_type]['types']['#suffix'] = t('Note: changing the profile will update all settings for existing @entity_types except for the project, workflow, vault, and storage method (e.g. node/field)', array('@entity_type' => strtolower($entity_type_info['label'])));
  }

  lingotek_admin_add_entity_specific_changes($form, $form_state, $entity_type);

  $form['translation_' . $entity_type]['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Modify general entity-translation form to include entity-specific changes.
 * 
 * @param array $form
 *   The form created in function lingotek_admin_entity_bundle_profiles_form
 * @param array $form_state
 *   The form state passed to function lingotek_admin_entity_bundle_profiles_form
 * @param string $entity_type
 *   The entity type for which to change the form or form state
 */
function lingotek_admin_add_entity_specific_changes(&$form, &$form_state, $entity_type) {
  $form['translation_' . $entity_type]['container'] = array(
    '#type' => 'container',
    '#prefix' => '<div class="spacer">&nbsp;</div>',
    '#attributes' => array('class' => array('js-utility-options')),
  );

  // Warning note when enabling nodes with existing Lingotek translations
  $overwrite_markup = '<div id="edit-lingotek-overwrite-warning"></div>';
  if ($entity_type != 'field_collection_item' && lingotek_previously_managed_translations($entity_type)) {
    $overwrite_markup = '<div id="edit-lingotek-overwrite-warning" style="color: red;">' . t('Note: If edits have been made to the local copy of translations since disabling Lingotek, those edits will be lost when synchronizing with Lingotek') . '</div>';
  }
  $form['translation_' . $entity_type]['container']['overwrite_warning'] = array(
    '#markup' => $overwrite_markup,
  );

  switch ($entity_type) {
    case 'node':
      // Optional clean up node languages.
      $node_languages = 'lingotek_function_node_languages';
      $form['translation_' . $entity_type]['container'][$node_languages] = array(
        '#type' => 'checkbox',
        '#title' => t('Prepare node languages'),
        '#description' => t('Set all <i>language neutral</i> nodes (and underlying fields and path aliases) for enabled content types to be @lang.', array('@lang' => language_default('name'))),
        '#default_value' => 1, // default to enable (for use in setup) --- js will disable on settings by default
      );
      // Optionally identify existing translations.
      $identify_translations = 'lingotek_function_identify_translations';
      $form['translation_' . $entity_type]['container'][$identify_translations] = array(
        '#type' => 'checkbox',
        '#title' => t('Identify existing translations'),
        '#description' => t('Identify existing node translations currently untracked by the Lingotek Translation module.  The translation status for all newly discovered translations will be set to <i>untracked</i>.'),
        '#default_value' => 1, // default to enable (for use in setup) --- js will disable on settings by default
      );
      break;
    case 'comment':
      // Optional clean up comment languages.
      $comment_languages = 'lingotek_function_comment_languages';
      $form['translation_' . $entity_type]['container'][$comment_languages] = array(
        '#type' => 'checkbox',
        '#title' => t('Prepare comment languages'),
        '#description' => t('Set all <i>language neutral</i> comments (and underlying fields) for enabled comment types to be @language.', array('@language' => language_default('name'))),
        '#default_value' => 1, // default to enable (for use in setup) --- js will disable on settings by default
      );
      //$form['translation_' . $entity_type]['types']['#suffix'] = t('Note: changing the profile will update all settings for existing comments except for the project, workflow, vault, and storage method (e.g. node/field).');
      break;
    case 'field_collection_item':
      // add note about field collections being coupled with their parent entities
      $form['translation_' . $entity_type]['types']['#suffix'] = t('Note: Field collections will be uploaded and downloaded at the same time as their parent.');
      break;
  }
}

/**
 * Node Translation Settings - Form Submit
 */
function lingotek_admin_entity_bundle_profiles_form_submit($form, &$form_state) {

  $entity_type = $form_state['input']['entity_type'];

  if (isset($form_state['values']['lingotek_nodes_translation_method'])) {
    variable_set('lingotek_nodes_translation_method', $form_state['values']['lingotek_nodes_translation_method']);
  }

  $enabled_fields = variable_get('lingotek_enabled_fields', array());
  unset($enabled_fields[$entity_type]);
  $operations = array();

  $entity_profiles = variable_get('lingotek_entity_profiles', array());
  $field_collection_field_types = field_read_fields(array('type' => 'field_collection'));

  $field_based_translation = variable_get('lingotek_nodes_translation_method') == 'field' ? TRUE : FALSE;

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
          $enabled_fields[$entity_type][$content_type][] = $content_field;

          // Set this content type to be translation-enabled only if currently disabled.
          $currently_enabled_content_type = variable_get('language_content_type_' . $content_type, '0');
          if ($currently_enabled_content_type == '0') {
            variable_set('language_content_type_' . $content_type, '1');
          }
          // Set this field to 'translatable' if using field-based translation.
          if ($field_based_translation) {
            // Update the field via the Field API (Instead of the direct db_update)
            $field = field_info_field($content_field);
            $is_field_collection = key_exists($content_field, $field_collection_field_types);
            $field['translatable'] = $is_field_collection ? 0 : 1;
            field_update_field($field);
          }
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
            $field_name = $legacy_field . '_field';
            $enabled_fields[$entity_type][$content_type][] = $field_name;
            $field = field_info_field($field_name);
            $is_field_collection = key_exists($field_name, $field_collection_field_types);
            if ($field_based_translation && !$is_field_collection) {
              $operations[] = array('field_update_field', array($field)); //field_update_field($field);
            }
          }
        }
      }
    }
    // Look for any profiles
    if (FALSE !== strpos($key, 'profile_')) {
      $content_type = substr($key, strlen('profile_'));
      $entity_profiles[$entity_type][$content_type] = $value;
      variable_set('lingotek_entity_profiles', $entity_profiles);
    }
    // Look for any prepare/cleanup functions to record for future running
    if (FALSE !== strpos($key, 'lingotek_function')) {
      // Add the preference for future reference in settings.
      variable_set($key, $value);
      if ($value) {
        lingotek_set_batch_function($key);
      }
    }
  }
  $_SESSION['lingotek_setup_path'][] = lingotek_get_entity_setup_path($entity_type);
  variable_set('lingotek_enabled_fields', $enabled_fields);
  drupal_set_message(t('Your content types have been updated.'));

  // This is needed for versions of Drupal core 7.10 and lower. See http://drupal.org/node/1380660 for details.
  drupal_static_reset('field_available_languages');

  if (count($operations)) {
    $batch = array(
      'title' => t('Preparing content for translation'),
      'operations' => $operations
    );

    batch_set($batch);
  }
  if (batch_get()) {
    // Reverse the order of the batches in order to put title manipulations before language prep
    $final_batch_set =& batch_get();
    $final_batch_set['sets'] = array_reverse($final_batch_set['sets']);

    // if coming from the settings page, stay there; otherwise, continue with setup pages
    $cur_path = current_path();
    if ($cur_path == 'admin/settings/lingotek/settings' ||
        $cur_path == LINGOTEK_MENU_LANG_BASE_URL . '/settings') {
      $redirect = $cur_path;
    }
    else {
      $redirect = lingotek_get_entity_setup_path($entity_type, TRUE);
    }
    batch_process($redirect);
  }
}

/*
 * Return an array of entity types with the number of each using the given profile
 */
function lingotek_admin_profile_usage($profile_id) {
  $entity_counts = array();
  $entities = lingotek_get_all_entities_by_profile($profile_id);
  // count up the entities by type
  foreach ($entities as $e) {
    if (isset($entity_counts[$e['type']])) {
      $entity_counts[$e['type']]++;
    }
    else {
      $entity_counts[$e['type']] = 1;
    }
  }
  return $entity_counts;
}

function lingotek_admin_profile_usage_by_types($profile_id) {
  $bundles_using_profile = lingotek_get_bundles_by_profile_id($profile_id);
  $count_types = 0;
  foreach ($bundles_using_profile as $bup) {
    $count_types += count($bup); 
  }
  return $count_types;
}

/**
 * Additional translation form
 */
function lingotek_admin_additional_translation_settings_form($form, &$form_state, $show_fieldset = FALSE) {
  global $base_url;
  $api = LingotekApi::instance();
  $setup_complete = (!lingotek_is_config_missing());
  $account = LingotekAccount::instance();
  $show_advanced = $account->showAdvanced();
  $title_str = ($setup_complete ? t('Configuration Translation') : t('Which additional items do you want translated?'));

  // Configuration translation (ie. taxonomies, menus, etc.)
  $form['additional_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => check_plain($title_str),
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
    '#validate' => array('lingotek_admin_additional_translation_settings_form_validate'),
    '#submit' => array('lingotek_admin_additional_translation_settings_form_submit'),
  );

  $header = array(
    'Type',
    'Description',
  );

  $fr = array(
    '#id' => 'lingotek_use_translation_from_drupal',
    '#type' => 'checkbox',
    '#title' => t('Use Drupal community translations when available from <a href="@link">localize.drupal.org</a>', array('@link' => url('http://localize.drupal.org', array('attributes' => array('target' => '_blank'))))),
    '#attributes' => array(
      'id' => array('lingotek_use_translation_from_drupal'),
      'name' => 'lingotek_use_translation_from_drupal',
      'class' => array('field'),
    ),
    '#default_value' => variable_get('lingotek_use_translation_from_drupal', TRUE) ? 'checked' : FALSE,
  );
  $prep_blocks_chbx = array(
    '#id' => 'lingotek_prepare_config_blocks',
    '#type' => 'checkbox',
    '#title' => t('Update all blocks to be translatable in the Languages settings.'),
    '#attributes' => array(
      'id' => array('lingotek_prepare_config_blocks'),
      'name' => 'lingotek_prepare_config_blocks',
      'class' => array('field'),
    ),
    '#default_value' => 1,
  );
  $prep_taxonomy_chbx = array(
    '#id' => 'lingotek_prepare_config_taxonomies',
    '#type' => 'checkbox',
    '#title' => t('Update all taxonomy vocabularies to use translation mode \'Localize\' in the Multilingual Options. (Note: This will prevent translation of custom fields within taxonomy terms.)'),
    '#attributes' => array(
      'id' => array('lingotek_prepare_config_taxonomies'),
      'name' => 'lingotek_prepare_config_taxonomies',
      'class' => array('field'),
    ),
    '#default_value' => 1,
  );
  $prep_menus_chbx = array(
    '#id' => 'lingotek_prepare_config_menus',
    '#type' => 'checkbox',
    '#title' => t('Update all menus to use \'Translate and Localize\' in the Multilingual Options.'),
    '#attributes' => array(
      'id' => array('lingotek_prepare_config_menus'),
      'name' => 'lingotek_prepare_config_menus',
      'class' => array('field'),
    ),
    '#default_value' => 1,
  );

  $hidden_i18nviews_value = array(
    '#id' => 'lingotek_i18nviews_enabled',
    '#type' => 'hidden',
    '#value' => module_exists('i18nviews'),
  );

  if (variable_get('lingotek_use_translation_from_drupal', 0)) {
    $fr['#attributes']['checked'] = 'checked';
  }

  $options = array(
    'lingotek_translate_config_blocks' => array(t('Blocks'), t('Include translation for all translatable blocks.') . drupal_render($prep_blocks_chbx)),
    'lingotek_translate_config_taxonomies' => array(t('Taxonomy'), t('Include translation for all translatable taxonomy vocabularies and terms.') . drupal_render($prep_taxonomy_chbx)),
    'lingotek_translate_config_menus' => array(t('Menus'), t('Include translation for all menus set to \'Translate and Localize\'.') . drupal_render($prep_menus_chbx)),
    'lingotek_translate_config_views' => array(t('Views'), t('Include translation of all translatable strings within views.  This will not generally include translation of all results generated by each view, which is usually configurable by selecting localizable fields within the view itself.  Note: You must have the <a href="https://drupal.org/project/i18nviews" target="_blank">i18nviews</a> module enabled in order to use this.') . drupal_render($hidden_i18nviews_value)),
    'lingotek_translate_config_fields' => array(t('Field Labels'), t('Include translation of all translatable field labels.  Note: You must have the <a href="https://drupal.org/project/i18n" target="_blank">i18n_field</a> sub-module enabled in order to use this.')),
    'lingotek_translate_config_builtins' => array(t('Built-in Interface'), t('When enabled, all built-in strings will be automatically translated.  (Note: this may take a while.)') . drupal_render($fr)),
    'lingotek_translate_config_misc' => array(t('Miscellaneous Strings'), t('Include translation of any other miscellaneous strings found by the Internationalization (i18n) module.')),
  );

  $defaults = array();

  foreach (array_keys($options) as $config_type) {
    $defaults[$config_type] = variable_get($config_type, 0);
  }

  $form['additional_translation']['config'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $defaults,
  );

  $workflows = $api->listWorkflows();
  if (is_array($workflows) && count($workflows) > 1) {
    $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Workflow'),
      '#description' => t('This workflow will be used for handling these additional translation items.'),
      '#options' => $workflows,
      '#default_value' => variable_get('lingotek_translate_config_workflow_id', ''),
    );
  }

  if ($setup_complete) {
    $form['additional_translation']['lingotek_translate_config_options']['view_status'] = array(
      '#type' => 'item',
      '#description' => t('You can view the progress of the configuration translations on the <a href="@link">Translate Interface</a> page', array('@link' => url('admin/config/regional/translate'))),
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

function lingotek_admin_additional_translation_settings_form_validate($form, &$form_state) {
  // check for required modules if config translation is enabled
  $conf_opt_part = $form['additional_translation']['config'];
  if (isset($conf_opt_part['lingotek_translate_config_views']['#value']) && $conf_opt_part['lingotek_translate_config_views']['#value'] !== 0) {
    lingotek_verify_modules_enabled(array('i18nviews'));
  }
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
      variable_get('lingotek_translate_config_builtins', 0) || variable_get('lingotek_translate_config_fields', 0) ||
      variable_get('lingotek_translate_config_misc', 0)) {
    variable_set('lingotek_translate_config', TRUE);
  }
  else {
    // mark lingotek_translate_config to false if no config is to be translated
    variable_set('lingotek_translate_config', FALSE);
  }

  // prepare for any configuration translation, if applicable
  $config_groups = array();
  if (variable_get('lingotek_translate_config_blocks', 0) && isset($form_state['input']['lingotek_prepare_config_blocks'])) {
    lingotek_admin_prepare_blocks();
    $config_groups['blocks'] = 'blocks';
  }
  if (variable_get('lingotek_translate_config_taxonomies', 0) && isset($form_state['input']['lingotek_prepare_config_taxonomies'])) {
    lingotek_admin_prepare_taxonomies();
    $config_groups['taxonomy'] = 'taxonomy';
  }
  if (variable_get('lingotek_translate_config_menus', 0) && isset($form_state['input']['lingotek_prepare_config_menus'])) {
    lingotek_admin_prepare_menus();
    $config_groups['menu'] = 'menu';
  }
  if (variable_get('lingotek_translate_config_views', 0) && isset($form_state['input']['lingotek_prepare_config_views'])) {
    lingotek_admin_prepare_views();
    $config_groups['views'] = 'views';
  }
  if (variable_get('lingotek_translate_config_fields', 0) && isset($form_state['input']['lingotek_prepare_config_fields'])) {
    lingotek_admin_prepare_fields();
    $config_groups['fields'] = 'fields';
  }
  if (variable_get('lingotek_translate_config_misc', 0) && isset($form_state['input']['lingotek_prepare_config_misc'])) {
    lingotek_admin_prepare_misc();
    $config_groups['misc'] = 'misc';
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
    'title' => 'Site translation preparation',
    'init_message' => t('Preparing to run batch actions'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('An error has occurred during setup.  Please review the log for more information.'),
    'file' => drupal_get_path('module', 'lingotek') . '/lingotek.admin.inc',
  );
  if ($operations && count($operations)) {
    batch_set($batch);
  }
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
  $plurals = db_select('{locales_target}', 't')
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
      drupal_set_message(filter_xss(implode("<br />\n", $messages)));

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
  $query = db_select('{system}', 's')
      ->fields('s', array('name', 'filename', 'type'))
      ->condition('s.type', array('module', 'theme'), 'IN')
      ->condition('s.status', '0', '!=')
      //->condition('s.filename', 'modules%', 'LIKE')
      ->execute();
  return $query->fetchAllAssoc('name');
}

function lingotek_admin_prepare_blocks() {
  // update all blocks to be translatable in the Languages settings
  $result = db_update('{block}')
      ->fields(array('i18n_mode' => LINGOTEK_I18N_ENABLED_VALUE))
      ->execute();
  if ($result) {
    drupal_set_message(format_plural($result, t('Enabled 1 block for translation.'), t('Enabled @num blocks for translation.', array('@num' => (int) $result))));
  }
  else {
    drupal_set_message(t('All blocks already enabled for translation.'));
  }
  return TRUE;
}

function lingotek_admin_prepare_taxonomies() {
  // update all taxonomies to be updated to use translation mode 'Localize'
  $result = db_update('{taxonomy_vocabulary}')
      ->fields(array('i18n_mode' => LINGOTEK_TAXONOMY_LOCALIZE_VALUE))
      ->execute();
  if ($result) {
    drupal_set_message(format_plural($result, t('Set 1 taxonomy vocabulary to <i>Localize</i>.'), t('Set @num taxonomy vocabularies to <i>Localize</i>.', array('@num' => (int) $result))));
  }
  else {
    drupal_set_message(t('All taxonomy vocabularies already set to <i>Localize</i>.'));
  }
  return TRUE;
}

function lingotek_admin_prepare_menus() {
  // update all menus to use 'Translate and Localize' in the Multilingual Options
  $result = db_update('{menu_custom}')
      ->fields(array('i18n_mode' => LINGOTEK_MENU_LOCALIZE_TRANSLATE_VALUE))
      ->execute();
  if ($result) {
    drupal_set_message(format_plural($result, t('Set 1 menu to <i>Translate and Localize</i>.'), t('Set @num menus to <i>Translate and Localize</i>.', array('@num' => (int) $result))));
  }
  else {
    drupal_set_message(t('All menus already set to <i>Translate and Localize</i>.'));
  }

  // find all menu links that are not language-neutral and not part of a
  // translation set and update them to be language-neutral (so they will
  // be added to the locales_source table as a string to be translated)
  $result = db_update('{menu_links}')
      ->fields(array('language' => LANGUAGE_NONE))
      ->condition('language', LANGUAGE_NONE, '!=')
      ->condition('i18n_tsid', 0)
      ->execute();
  if ($result) {
    drupal_set_message(format_plural($result, t('Set 1 menu link to <i>language neutral</i>.'), t('Set @num menu links to <i>Language neutral</i>.', array('@num' => (int) $result))));
  }
  else {
    drupal_set_message(t('All menu links already set to <i>language neutral</i>.'));
  }

  return TRUE;
}

function lingotek_admin_prepare_views() {
  // Stub for future preparations if necessary
  return TRUE;
}

function lingotek_admin_prepare_fields() {
  // Stub for future preparations if necessary
  return TRUE;
}

function lingotek_admin_prepare_misc() {
  // Stub for future preparations if necessary
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
      'onclick' => 'return confirm("' . t('Are you sure?\n\nAll of your content-parsing settings will be reset to system defaults.') . '");'
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

  $query = db_select('{block}', 'b');
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
    '#description' => t('The region where the switcher will be displayed.'),
    '#options' => $block_regions,
    '#default_value' => $default_region_value,
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $fkey = 'prefs'; // form key

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
    '#description' => t('Note: The default language switcher block is only shown if at least two languages are enabled and language negotiation is set to <i>URL</i> or <i>Session</i>.')
    . '<br>' . t('Go to') . ' ' . l(t('Language detection and selection'), $general_detection_config_url) . ' ' . t('to change this.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['prefs']['lingotek_advanced_taxonomy_terms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable advanced handling of taxonomy terms'),
    '#description' => t('This option is used to handle translation of custom fields assigned to taxonomy terms.'),
    '#default_value' => variable_get('lingotek_advanced_taxonomy_terms', FALSE),
  );

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
    '#title' => t('Enable Advanced Features'),
    '#default_value' => (variable_get('lingotek_account_plan_type') == 'advanced' ? 1 : 0),
    '#description' => t('Some features may not be available without an') . '&nbsp;<a href="http://www.lingotek.com/drupal_pricing" target="_blank">Enterprise license</a>&nbsp;' . t('for the Lingotek TMS. Call 801.331.7777 for details.'),
  );

  return $form;
}

function lingotek_admin_prefs_form_submit($form, &$form_state) {
  lingotek_admin_language_switcher_form_submit($form, $form_state);
  $form_state['values']['lingotek_account_plan_type'] = ($form['prefs']['lingotek_account_plan_type']['#checked'] ? 'advanced' : 'standard');
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
  $show_advanced = $account->showAdvanced();


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

  if ($show_advanced || $edit_connection) {
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
        //array('AP Key:', LINGOTEK_AP_OAUTH_KEY),
        //array('AP Secret:', LINGOTEK_AP_OAUTH_SECRET),
    )))
    );
  }

  //$form = system_settings_form($form);
  if (!($show_advanced || $edit_connection)) {
    unset($form['connection']['actions']);
  }
  return $form;
}

function lingotek_admin_connection_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);
}

function lingotek_admin_profiles_form($form, &$form_state, $show_fieldset = FALSE) {
  $profiles = lingotek_get_profiles_by_name(TRUE); // these will already be translated using the t-function

  $form['header'] = array(
    '#type' => 'item',
    '#title' => t('Translation Profiles'),
    '#description' => t('Translation management defaults used when creating new entities. At the entity level, these settings can be adjusted.')
  );

  $headers = array(
    t('Profile Name'),
    t('Usage'),
    t('Actions'),
  );

  $total_count_with_profiles = 0;
  $rows = array();
  $profiles_excluded_from_editing = array('CUSTOM', 'DISABLED');

  foreach ($profiles as $key => $profile_name) {
    if (in_array((string)$key, $profiles_excluded_from_editing)) {
      $edit_link = '<i class="fa fa-gear fa-lg ltk-disabled-icon"></i>';
    }
    else {
      $edit_link = '<a href="' . url(LINGOTEK_MENU_MAIN_BASE_URL . '/settings/profile/' . $key) . '" class="ctools-use-modal ctools-modal-lingotek-style" title="' . t('Edit') . '"><i class="fa fa-gear fa-lg"></i></a>';
    }

    // styling for modals
    drupal_add_js(array(
      'lingotek-style' => array(
        'closeImage' => theme('image', array('path' => drupal_get_path('module', 'lingotek') . '/images/close.png', 'alt' => t('Close window'), 'title' => t('Close window'))),
        'animation' => 'fadeIn',
      ),
    ), 'setting');

    $count_by_entity_type = lingotek_admin_profile_usage($key);
    $count_types = lingotek_admin_profile_usage_by_types($key);
    $total_count_with_profiles += array_sum($count_by_entity_type);
    $entities_str = '';
    $total_entities = 0;

    foreach ($count_by_entity_type as $entity_type => $count) {
      if (!empty($entities_str)) {
        $entities_str .= ', ';
      }
      $entities_str .= $count . ' ' . format_plural($count, $entity_type, $entity_type . 's');
      $total_entities += $count;
    }
    $entities_str = strlen(trim($entities_str)) ? " ($entities_str)" : "";
    $entities_str = $total_entities . ' ' . format_plural($total_entities, "entity", "entities") . $entities_str;

    // The custom profile line should not be shown unless there are entities assigned as 'custom'
    if ($key !== 'CUSTOM') {
      $rows[] = array($profile_name, $entities_str . ', ' . $count_types . ' ' . format_plural($count_types, 'content type', 'content types'), $edit_link);
    }
    elseif ($count_by_entity_type) {
      $rows[] = array($profile_name, $entities_str, $edit_link);
    }
  }

  $variables = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array('id' => 'lingotek-profiles'),
  );

  $form['profiles'] = array(
    '#markup' => theme('table', $variables),
  );

  $form['add'] = array(
    '#markup' => t('<p><a href="@url" class="ctools-use-modal"><i class="fa fa-plus" style="padding-right:3px"></i>Add new profile</a></p>', array('@url' => url(LINGOTEK_MENU_MAIN_BASE_URL . '/settings/profile/add'))),
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
  
  $profile_id = (isset($form_state['values']['profile_id']) ? $form_state['values']['profile_id']: -1);
  $stored_profile = lingotek_get_profile_settings($profile_id);

  $first_custom_id = 2;
  $is_custom_profile_id = $profile_id >= $first_custom_id;

  $show_advanced = $account->showAdvanced();

  $form['defaults']['profile_id'] = array(
    '#type' => 'value',
    '#value' => $form_state['values']['profile_id'],
  );

  $form['defaults']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Profile Name'),
    '#default_value' => $form_state['values']['name'],
    '#disabled' => $is_custom_profile_id ? FALSE : TRUE,
  );

  $form['defaults']['current_future_note'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>' . t('Profile settings impacting all entities (new and existing)') . '</h3><hr />',
  );
  // Upload
  $form['defaults']['create_lingotek_document'] = array(
    '#type' => 'checkbox',
    '#title' => t('Upload Content Automatically'),
    '#default_value' => $form_state['values']['create_lingotek_document'],
    '#description' => t('When enabled, your Drupal content (including saved edits) will automatically be uploaded to Lingotek for translation.<br/>When disabled, you are required to manually upload your content by clicking the "Upload" button on the Translations tab.'),
    '#disabled' => $is_custom_profile_id ? FALSE : TRUE,
  );

  // Download
  $form['defaults']['sync_method'] = array(
    '#type' => 'checkbox',
    '#title' => t('Download Translations Automatically'),
    '#default_value' => $form_state['values']['sync_method'],
    '#description' => t('When enabled, completed translations will automatically be downloaded from Lingotek.<br/>When disabled, you are required to manually download translations by clicking the "Download" button on the Translations tab.'),
    '#disabled' => $is_custom_profile_id ? FALSE : TRUE,
  );

  // URL Alias Translation.
  $form['defaults']['url_alias_translation'] = array(
    '#type' => 'select',
    '#title' => t('URL Alias Translation'),
    '#description' => t("Choose how you would like to translate the URL alias. The last option requires that you install both the Title and Pathauto modules, and define a path pattern."),
    '#options' => lingotek_get_url_alias_translations(),
    '#default_value' => $form_state['values']['url_alias_translation'],
  );

  if ($show_advanced) {

    // Community Translation
    $form['defaults']['allow_community_translation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Crowdsourced Translation'),
      '#description' => t('When enabled, anonymous site visitors will be presented with a link allowing them to contribute translations for this node.'),
      '#default_value' => $form_state['values']['allow_community_translation'],
    );
  }
  
  
  // Workflows
  $workflows = $api->listWorkflows();
  if ($workflows && count($workflows) > 1) {
    $stored_workflow_id =  (isset($stored_profile['workflow_id']) ? $stored_profile['workflow_id'] : '');
    $selected_workflow_id = (isset($form_state['values']['workflow_id']) ? $form_state['values']['workflow_id'] : $stored_workflow_id);

    $form['defaults']['workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Default Workflow'),
      '#description' => t('The default Workflow to use when translating content.'),
      '#default_value' => $stored_workflow_id,
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
      '#title' => t('Change all current content in this profile (@total in total) to use the new workflow', array('@total' => array_sum(lingotek_admin_profile_usage($profile_id)))),
      '#default_value' => FALSE,
      '#description' => t('All current translations will be removed and recreated using the new workflow, with translations pulled from the previous workflow'),
      '#states' => array(
        'invisible' => array(':input[name="workflow_id"]' => array('value' => $stored_workflow_id)),
      )
    );

    $form['defaults']['prefill_phase_select'] = array(
      '#title' => t("Desired Prefill Phase"),
      '#description' => t('Please select the highest phase which should be prefilled for the new workflow'),
      '#type' => 'select',
      '#states' => array(
          'visible' => array(':input[name="workflow_id"]' => array('!value' => $stored_workflow_id),
                             ':input[name="prefill_phases_checkbox"]' => array('checked' => TRUE)
        ),
        ),
      '#suffix' => '</div>',
    );
    $form['defaults']['prefill_phase_select']['#options'] = lingotek_get_phases_by_workflow_id($selected_workflow_id);
  }

  // FUTURE-ONLY STUFF
  $form['defaults']['future_only_note'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>' . t('Profile settings impacting only new nodes') . '</h3><hr />',
  );

  $content_translation_note = '';
  if (!lingotek_node_based_trans_ready()) {
    $content_translation_note = t("This method requires Content Translation module (translation) and Multilingual content module (i18n_node).");
  }
  
  $form['defaults']['lingotek_nodes_translation_method'] = array(
    '#type' => 'radios',
    '#title' => t('Translation Storage'),
    '#options' => array(
      'field' => t('<strong>Field Translation</strong> (Recommended) - all translations are stored in a single node'),
      'node' => t('<strong>Node Translation</strong>  - create a new node per language'),
    ),
    'field' => array(
      '#description' => t('A newer and recommended method. Newer versions of Drupal are expected to use this method.  Note: If this method is selected, all fields chosen for Lingotek translation (below) will be marked as translatable in the Drupal field configuration.'),
    ),
    'node' => array(
      '#description' => t('The classical method. Use for backwards compatibility.  Note: If this method is selected, fields chosen for Lingotek translation will *not* be marked as translatable in the Drupal field configuration.  Instead, translations will be saved as language-neutral, allowing the node to dictate the language.  This method will not work for translating other entity types, such as taxonomy terms and messages.  <i>@note</i>', array('@note' => $content_translation_note)),
    ),

    '#default_value' => isset($form_state['values']['lingotek_nodes_translation_method']) ? $form_state['values']['lingotek_nodes_translation_method'] : 'field',
    '#disabled' => lingotek_node_based_trans_ready() ? FALSE : TRUE,
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
  
  if ($projects && count($projects) > 1) {
    $sorted = asort($projects);
    $form['defaults']['project_id'] = array(
      '#type' => 'select',
      '#title' => t('Default Project'),
      '#options' => $projects,
      '#description' => t('The default Lingotek Project with which translations will be associated.'),
      '#default_value' => isset($form_state['values']['project_id']) ? $form_state['values']['project_id'] : $id,
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

  if ($personal_vault_count + $community_vault_count > 1) {
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
    '#value' => t('Save'),
  );

  // if no entities or bundles are assigned to the profile, and if it was created
  // by the site admin, then allow deletion.
  if (array_sum(lingotek_admin_profile_usage($profile_id)) == 0 && lingotek_admin_profile_usage_by_types($profile_id) == 0 && $is_custom_profile_id) {
    $form['defaults']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  else {
    $markup = $is_custom_profile_id ? t('You can only delete this profile when there are no entities or bundles using it.') : t('This profile cannot be deleted.');
    $form['delete'] = array(
      '#markup' => '<span class="ltk-muted" style="font-style: italic;">' . $markup . '</span>',
    );
  }
  $form['defaults']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
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

    // If the workflow has changed and if current translations should be changed,
    // then pull all nodes associated with this profile and update the workflow
    // on TMS, including the correct phase.
    if (isset($form_state['values']['profile_id'])) {
      $profile_id = $form_state['values']['profile_id'];
      lingotek_set_profile_settings($profile_id, $profile);

      if (isset($form_state['values']['prefill_phases_checkbox']) 
          && $form_state['values']['prefill_phases_checkbox']) {
        $workflow_id = $form_state['values']['workflow_id'];
        $prefill_phase = $form_state['values']['prefill_phase_select'];
  
        $entities = lingotek_get_all_entities_by_profile($profile_id);
        
        $api = LingotekApi::instance();
        $document_ids = array();
        
        // gather doc IDs for bulk submission
        foreach ($entities as $entity) {
          if (isset($entity['document_id']) && $entity['document_id']) {
            $document_ids[] = $entity['document_id'];
            lingotek_keystore($entity['type'], $entity['id'], 'workflow_id', $workflow_id);
          }
        }
        $api->changeWorkflow($document_ids, $workflow_id, $prefill_phase);
      }
    }
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
    '#title' => t('Lingotek Utilities'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
      //'#group' => 'administrative_settings',
  );

  drupal_add_js(array(
    'lingotek-disassociate-style' => array(
      'modalSize' => array(
        'type' => 'fixed',
        'width' => 450,
        'height' => 250,
      ),
      'closeImage' => theme('image', array('path' => drupal_get_path('module', 'lingotek') . '/images/close.png', 'alt' => t('Close window'), 'title' => t('Close window'))),
      'animation' => 'fadeIn',
    ),
  ), 'setting');

  $form['url'] = array(
    '#type' => 'hidden',
    // The name of the class is the #id of $btn_disassociate_translations with "-url" suffix. 
    '#attributes' => array('class' => array('ctools-lingotek-disassociate-button-url')), 
    '#value' => url(LINGOTEK_MENU_MAIN_BASE_URL . '/disassociate/'),
  );

  $btn_disassociate_translations = array(
    '#type' => 'button', 
    '#value' => t('Disassociate'), 
    '#attributes' => array('class' => array('ctools-use-modal', 'ctools-modal-lingotek-disassociate-style')), 
    '#id' => 'ctools-lingotek-disassociate-button',
  );

  $btn_callback_url_update = array(
    '#type' => 'submit',
    '#value' => t('Update URL'),
    '#submit' => array('lingotek_notify_url_update')
  );

  $btn_callback_refresh_api_cache = array(
    '#type' => 'submit',
    '#value' => t('Refresh'),
    '#submit' => array('lingotek_refresh_api_cache')
  );

  // Start: Not sure why, but without these below, the table buttons do not work properly
  $form['utilities'][] = array_merge($btn_callback_refresh_api_cache, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_callback_url_update, array('#attributes' => array('style' => 'display: none;')));
  //$form['utilities'][] = array_merge($btn_disassociate_translations, array('#attributes' => array('style' => 'display: none;')));
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
  );

  $rows[] = array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Disassociate All Translations') . ' <i>' . t('(use with caution)') . '</i>' . '<div class="description">',
          '#markup' => t("Should only be used to change the Lingotek project or TM vault associated with the node’s translation. Option to disassociate node translations on Lingotek’s servers from the copies downloaded to Drupal. Additional translation using Lingotek will require re-uploading the node’s content to restart the translation process."),
          '#suffix' => '</div>'
        )),
      array('data' => $btn_disassociate_translations)
  );

  $header = array();
  $form['utilities']['utility_table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows))
  );

  return $form;
}

function lingotek_disassociate_all_translations($entity_type = 'node', $entity_ids = array(), $extra = "") {
  ctools_include('node.pages', 'node', '');
  ctools_include('modal');
  ctools_include('ajax');

  $entity_ids = !is_array($entity_ids) ? explode(',', $entity_ids) : $entity_ids;

  $form_state = array(
    'ajax' => TRUE,
    'entity_ids' => $entity_ids,
    'entity_type' => $entity_type,
  );

  $output = ctools_modal_form_wrapper('lingotek_disassociate_all_form', $form_state);

  if (!empty($form_state['executed'])) {
    $commands = array();
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ctools_ajax_command_reload();
    print ajax_render($commands);
    drupal_exit();
  }

  print ajax_render($output);
}

function lingotek_disassociate_all_form($form, $form_state, $entity_ids = array(), $collapse_fieldset = TRUE) {

  if (isset($form_state['entity_ids']) && empty($entity_ids)) {
    $entity_ids = $form_state['entity_ids'];
    $collapse_fieldset = FALSE;
  }

  $form = array();
  if (!is_array($entity_ids)) {
    $entity_ids = array($entity_ids);
  }

  $form['disassociate_all_translations'] = array(
    '#type' => 'fieldset',
    '#title' => t('Disassociate All Translations'),
    '#collapsible' => $collapse_fieldset,
    '#collapsed' => $collapse_fieldset,
  );

  $form['disassociate_all_translations']['confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('Also remove all documents from Lingotek TMS'),
    '#default_value' => FALSE,
    );
  $form['disassociate_all_translations']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Disassociate All Translations'),
    );

  return $form;
}

function lingotek_disassociate_all_form_submit($form_state, $form) {
  $doc_ids = LingotekSync::getAllLocalDocIds();
  $api = LingotekApi::instance();
  $operations = array();

  $remove_from_tms = $form_state['disassociate_all_translations']['confirm']['#value'];

  if ($remove_from_tms) { //disassociate on TMS
    lingotek_batch_disassociate_content_worker($api, $doc_ids);
  }

  //disassociate on CMS
  LingotekSync::disassociateAllEntities();
  LingotekSync::disassociateAllChunks();
  drupal_set_message(t('All local translations have been disassociated from Lingotek.'));
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

/**
 * Utilities form.
 */
function lingotek_admin_cleanup_form($form, &$form_state, $show_fieldset = FALSE) {

  $form['cleanup'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Multilingual Preparation Utilities'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Run Selected Utilities')
      )
    ),
    '#submit' => array('lingotek_admin_cleanup_form_submit')
  );

  $cleanup_function_headers = array(
    'title' => array('data' => t('Utility')),
    'desc' => array('data' => t('Description')),
  );

  $cleanup_functions = array();

  /* TEMPLATE
  $cleanup_functions['lingotek_cleanup_'] = array(
    'title' => t('title'),
    'desc' => t('description'),
  );
   * END TEMPLATE
   */
  $cleanup_functions['lingotek_batch_identify_translations'] = array(
    'title' => '<span style="white-space: nowrap">' . t('Identify existing translations') . '</span>',
    'desc' => t('Identifies existing node translations currently untracked by the Lingotek module. The translation status for all newly discovered translations will be set to <i>current</i>.'),
  );

  $cleanup_functions['lingotek_cleanup_field_languages_for_nodes'] = array(
    'title' => t('Prepare nodes'),
    'desc' => t('Sets all <i>language neutral</i> nodes (and underlying fields and path aliases) to be @lang.', array('@lang' => language_default('name'))),
  );

  if (module_exists('entity_translation')) {
    $cleanup_functions['lingotek_cleanup_notify_entity_translation'] = array(
      'title' => t('Sync with Entity Translation'),
      'desc' => t('Reports all translations managed by Lingotek to the Entity Translation module.'),
    );
  }

  if (module_exists('comment')) {
    $cleanup_functions['lingotek_cleanup_field_languages_for_comments'] = array(
      'title' => t('Prepare comments'),
      'desc' => t('Sets all <i>language neutral</i> comments (and underlying fields) to be @lang.', array('@lang' => language_default('name'))),
    );
  }

  if (module_exists('fieldable_panels_panes')) {
    $cleanup_functions['lingotek_cleanup_field_languages_for_fieldable_panels_panes'] = array(
      'title' => t('Prepare FPPs'),
      'desc' => t('Sets all <i>language neutral</i> fieldable panels panes (FPPs) to be @lang.', array('@lang' => language_default('name'))),
    );
  }

  if (module_exists('taxonomy') && variable_get('lingotek_advanced_taxonomy_terms', FALSE)) {
    $cleanup_functions['lingotek_cleanup_field_languages_for_taxonomy_terms'] = array(
      'title' => t('Prepare taxonomy terms with custom fields'),
      'desc' => t('Sets all <i>language neutral</i> taxonomy terms managed by Lingotek (and underlying fields) to be @lang.', array('@lang' => language_default('name'))),
    );
  }

  $cleanup_functions['lingotek_admin_prepare_blocks'] = array(
    'title' => t('Prepare blocks'),
    'desc' => t('Update all blocks to be translatable in the <i>Languages</i> settings.'),
  );

  $cleanup_functions['lingotek_admin_prepare_taxonomies'] = array(
    'title' => t('Prepare taxonomy'),
    'desc' => t('Update all taxonomy vocabularies to use translation mode <i>Localize</i> in the Multilingual Options, in preparation for handling string-based translations of taxonomy vocabularies and terms.'),
  );

  $cleanup_functions['lingotek_admin_prepare_menus'] = array(
    'title' => t('Prepare menus'),
    'desc' => t('Update all menus to use <i>Translate and Localize</i> in the Multilingual Options, and update all menu links to be <i>Language neutral</i>.'),
  );

  if (module_exists('field_collection')) {
    $extra_note = '';
    if (lingotek_has_unconventional_fc_translations()) {
      $extra_note = '<div style="color:red;">' . t('Note: It appears you have existing field collection translations that may not conform with Lingotek\'s method of translating field collections.  Using this utility will delete those translations.') . '</div>';
    }
    $cleanup_functions['lingotek_cleanup_field_collection_fields'] = array(
      'title' => t('Prepare field collections'),
      'desc' => t("Sets all field-collections to be <i>language neutral</i>, allowing the underlying fields to be enabled for multilingual use.  !note", array('!note' => $extra_note)),
    );
  }

  if (module_exists('message')) {
    $cleanup_functions['lingotek_cleanup_message_types'] = array(
      'title' => t('Prepare message types'),
      'desc' => t("Sets all empty and undefined languages associated with the Message API module to be the site-default language."),
    );
  }

  $cleanup_functions['lingotek_add_missing_locales'] = array(
    'title' => t('Add missing locales'),
    'desc' => t('Fills in any missing locale values to the <i>languages</i> table.'),
  );

  $form['cleanup']['grid'] = array(
    '#type' => 'tableselect',
    '#header' => $cleanup_function_headers,
    '#options' => $cleanup_functions,
  );

  $rows = array();

  return $form;
}

/**
 * Attempt to execute each of the cleanup functions selected by the requester.
 *
 * @param array $form
 * @param array $form_state
 */
function lingotek_admin_cleanup_form_submit($form, &$form_state) {
  // All cleanup-related batch functions should exist in lingotek.util.inc
  // and their return value should be a set batch.
  $cleanup_batch = array(
    'title' => t('Lingotek Cleanup Utility Batch'),
    'operations' => array(array('lingotek_cleanup_batch_init', array())),
    'file' => 'lingotek.util.inc',
  );
  batch_set($cleanup_batch);
  foreach ($form_state['values']['grid'] as $cleanup_function) {
    if (!$cleanup_function) {
      // Function was not selected to run.
      continue;
    }
    if (!function_exists($cleanup_function)) {
      LingotekLog::error('Attempted to run non-existent cleanup function: @function', array('@function' => $cleanup_function));
      continue;
    }
    try {
      $cleanup_function();
    } catch (Exception $e) {
      LingotekLog::error('Failed to run cleanup function "@function".  Error was: @error', array('@function' => $cleanup_function, '@error' => $e->getMessage()));
      continue;
    }
  }
  // Execute all set batches and then return.
  batch_process('admin/settings/lingotek/settings');
}

function lingotek_admin_configuration_view($form_short_id = NULL, $show_fieldset = TRUE) {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $site = variable_get('site_name', 'Drupal Site');
  $show_advanced = $account->showAdvanced();

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
      'css' => array(
        '//netdna.bootstrapcdn.com/font-awesome/4.0.3/css/font-awesome.css' => array(
          'type' => 'external',
        ),
      ),
      'js' => array(drupal_get_path('module', 'lingotek') . '/js/lingotek.admin.js'),
    ),
  );

  $account_summary = array(
    drupal_get_form('lingotek_admin_account_status_form', $show_fieldset),
    drupal_get_form('lingotek_admin_connection_form', $show_fieldset),
  );

  $output['lingotek'][] = lingotek_wrap_in_fieldset($account_summary, t('Account'), array('id' => 'ltk-account'));

  foreach (lingotek_managed_entity_types($include_all = TRUE) as $machine_name => $entity_type) {
    $entity_settings = drupal_get_form('lingotek_admin_entity_bundle_profiles_form', $machine_name, $show_fieldset);
    $output['lingotek'][] = lingotek_wrap_in_fieldset($entity_settings, t('Translate @type', array('@type' => $entity_type['label'])), array('class' => array('ltk-entity')));
  }
  $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_additional_translation_settings_form', $show_fieldset), t('Translate Configuration'), array('id' => 'ltk-config'));
  $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_profiles_form', $show_fieldset), t('Translation Profiles'), array('id' => 'ltk-profiles'));
  if ($show_advanced) {
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_advanced_parsing_form', TRUE), t('Advanced Content Parsing'), array('id' => 'ltk-advanced-content-parsing'));
  }

  $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_prefs_form', $show_fieldset), t('Preferences'), array('id' => 'ltk-prefs'));
  $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_logging_form', $show_fieldset), 'Logging');

  $utilities = array(
    drupal_get_form('lingotek_admin_cleanup_form', $show_fieldset),
    drupal_get_form('lingotek_admin_utilities_form', $show_fieldset),
  );
  $output['lingotek'][] = lingotek_wrap_in_fieldset($utilities, t('Utilities'));

  return $output;
}

function lingotek_wrap_in_fieldset($form, $title, $attributes = array()) {
  $fieldset = array(
    '#type' => 'fieldset',
    '#group' => 'lingotek',
    '#title' => $title, // comes in translated
    '#attributes' => $attributes,
    'children' => $form,
  );
  return $fieldset;
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

  db_update('{block}')
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
    $results = db_select('{lingotek}', 'l')->fields('l', array('nid'))->distinct()->execute();
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
