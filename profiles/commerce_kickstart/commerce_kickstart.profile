<?php

/**
 * Implements hook_image_default_styles().
 */
function commerce_kickstart_image_default_styles() {
  $styles = array();
  $styles['frontpage_block'] = array(
    'name' => 'frontpage_block',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '270',
          'height' => '305',
        ),
        'weight' => '1',
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_form_alter().
 *
 * Allows the profile to alter the site configuration form.
 */
function commerce_kickstart_form_install_configure_form_alter(&$form, $form_state) {
  // When using Drush, let it set the default password.
  if (drupal_is_cli()) {
    return;
  }
  // Set a default name for the dev site and change title's label.
  $form['site_information']['site_name']['#title'] = 'Store name';
  $form['site_information']['site_mail']['#title'] = 'Store email address';
  $form['site_information']['site_name']['#default_value'] = st('Commerce Kickstart');

  // Set a default country so we can benefit from it on Address Fields.
  $form['server_settings']['site_default_country']['#default_value'] = 'US';

  // Use "admin" as the default username.
  $form['admin_account']['account']['name']['#default_value'] = 'admin';

  // Set the default admin password.
  $form['admin_account']['account']['pass']['#value'] = 'admin';

  // Hide Update Notifications.
  $form['update_notifications']['#access'] = FALSE;

  // Add informations about the default username and password.
  $form['admin_account']['account']['commerce_kickstart_name'] = array(
    '#type' => 'item',
    '#title' => st('Username'),
    '#markup' => 'admin'
  );
  $form['admin_account']['account']['commerce_kickstart_password'] = array(
    '#type' => 'item',
    '#title' => st('Password'),
    '#markup' => 'admin'
  );
  $form['admin_account']['account']['commerce_kickstart_informations'] = array(
    '#markup' => '<p>' . t('This information will be emailed to the store email address.') . '</p>'
  );
  $form['admin_account']['override_account_informations'] = array(
    '#type' => 'checkbox',
    '#title' => t('Change my username and password.'),
  );
  $form['admin_account']['setup_account'] = array(
    '#type' => 'container',
    '#parents' => array('admin_account'),
    '#states' => array(
      'invisible' => array(
        'input[name="override_account_informations"]' => array('checked' => FALSE),
      ),
    ),
  );

  // Make a "copy" of the original name and pass form fields.
  $form['admin_account']['setup_account']['account']['name'] = $form['admin_account']['account']['name'];
  $form['admin_account']['setup_account']['account']['pass'] = $form['admin_account']['account']['pass'];
  $form['admin_account']['setup_account']['account']['pass']['#value'] = array('pass1' => 'admin', 'pass2' => 'admin');

  // Use "admin" as the default username.
  $form['admin_account']['account']['name']['#access'] = FALSE;

  // Make the password "hidden".
  $form['admin_account']['account']['pass']['#type'] = 'hidden';
  $form['admin_account']['account']['mail']['#access'] = FALSE;

  // Add a custom validation that needs to be trigger before the original one,
  // where we can copy the site's mail as the admin account's mail.
  array_unshift($form['#validate'], 'commerce_kickstart_custom_setting');
}

/**
 * Validate callback; Populate the admin account mail, user and password with
 * custom values.
 */
function commerce_kickstart_custom_setting(&$form, &$form_state) {
  $form_state['values']['account']['mail'] = $form_state['values']['site_mail'];
  // Use our custom values only the corresponding checkbox is checked.
  if ($form_state['values']['override_account_informations'] == TRUE) {
    if ($form_state['input']['pass']['pass1'] == $form_state['input']['pass']['pass2']) {
      $form_state['values']['account']['name'] = $form_state['values']['name'];
      $form_state['values']['account']['pass'] = $form_state['input']['pass']['pass1'];
    }
    else {
      form_set_error('pass', st('The specified passwords do not match.'));
    }
  }
}

/**
 * Implements hook_system_info_alter().
 *
 * Hides conflicting features (so demo store users don't see the "no demo store"
 * features, and the other way around).
 */
function commerce_kickstart_system_info_alter(&$info, $file, $type) {
  // Don't run during installation.
  if (variable_get('install_task') != 'done') {
    return;
  }

  $install_demo_store = variable_get('commerce_kickstart_demo_store', FALSE);
  if ($install_demo_store) {
    $hide_modules = array(
      'commerce_kickstart_lite_product',
    );
  }
  else {
    $hide_modules = array(
      'commerce_kickstart_product',
    );
  }

  if ($type == 'module' && in_array($file->name, $hide_modules)) {
    $info['hidden'] = TRUE;
  }
}

/**
 * Provides a list of Crumbs plugins and their weights.
 */
function commerce_kickstart_crumbs_get_info() {
  $crumbs = array(
    'crumbs.home_title' => 0
  );

  foreach (module_implements('commerce_kickstart_crumb_info') as $module) {
    // The module-provided item might be just the name of the plugin, or it
    // might be an array in the form of $plugin_name => $weight.
    foreach (module_invoke($module, 'commerce_kickstart_crumb_info') as $crumb) {
      if (is_array($crumb)) {
        $crumbs += $crumb;
      }
      else {
        $crumbs[$crumb] = count($crumbs);
      }
    }
  }

  // Add the fallback wildcard.
  $crumbs['*'] = count($crumbs);

  asort($crumbs);

  return $crumbs;
}

/**
 * Rebuilds a feature using the same logic as features_modules_enabled().
 *
 * Called from the hook_enable() of every Kickstart feature.
 *
 * features_modules_enabled() runs too late, so when a feature's hook_enable()
 * or hook_install() runs, the feature hasn't been rebuild yet, no exported
 * structures exist in the system and can't be modified.
 * It also rebuilds all features at once, which makes it prone to timeouts.
 * This is why Kickstart disables features_modules_enabled() and rebuilds
 * each feature manually in its hook_enable() hook.
 */
function commerce_kickstart_rebuild_feature($module) {
  $feature = features_load_feature($module, TRUE);
  $items[$module] = array_keys($feature->info['features']);
  // Need to include any new files.
  features_include_defaults(NULL, TRUE);
  _features_restore('enable', $items);
  // Rebuild the list of features includes.
  features_include(TRUE);
  // Reorders components to match hook order and removes non-existant.
  $all_components = array_keys(features_get_components());
  foreach ($items as $module => $components) {
    $items[$module] = array_intersect($all_components, $components);
  }
  _features_restore('rebuild', $items);
}

/**
 * Implements hook_features_api_alter().
 *
 * Commerce Kickstart provides different Features that can be utilized
 * individually, which means there are conflicting field bases. This allows
 * the feature structure to exist, including customizations by users.
 */
function commerce_kickstart_features_api_alter(&$components) {
  if (isset($components['field_base'])) {
    $components['field_base']['duplicates'] = FEATURES_DUPLICATES_ALLOWED;
  }
}

/**
 * Implements hook_field_default_field_bases_alter().
 *
 * Helper alter to aid in Features Override of Features 1.x override exports
 * of Fields and Field Base config.
 */
function commerce_kickstart_field_default_field_bases_alter(&$fields) {
  if (module_exists('features_override')) {
    $possible_alters = commerce_kickstart_get_fields_default_alters();
    drupal_alter('field_default_fields_alter', $possible_alters);
    foreach ($possible_alters as $identifier => $field_default) {
      // Check if the alter added a field base value.
      $field_name = $field_default['field_name'];
      if (!isset($field_default['field_base']) || !isset($fields[$field_name])) {
        continue;
      }
      $fields[$field_name] = drupal_array_merge_deep($fields[$field_name], $field_default['field_base']);
    }
  }
}

/**
 * Implements hook_field_default_field_instances_alter().
 *
 * Helper alter to aid in Features Override of Features 1.x override exports
 * of Fields and Field Instance config.
 */
function commerce_kickstart_field_default_field_instances_alter(&$fields) {
  if (module_exists('features_override')) {
    $possible_alters = commerce_kickstart_get_fields_default_alters();
    drupal_alter('field_default_fields', $possible_alters);
    foreach ($possible_alters as $identifier => $field_default) {
      // Check if the alter added a field instance value.
      if (!isset($field_default['field_instance']) || !isset($fields[$identifier])) {
        continue;
      }
      $fields[$identifier] = drupal_array_merge_deep($fields[$identifier], $field_default['field_instance']);
    }
  }
}

/**
 * Gets Features Override alters for field from 1.x
 */
function commerce_kickstart_get_fields_default_alters() {
  $cache = drupal_static(__FUNCTION__, array());
  if (empty($cache)) {
    module_load_include('inc', 'features', 'features.export');
    features_include();
    // Features 1.x labeled all field data same as field instance in 2.x
    features_include_defaults('field_instance');
    $default_hook = features_get_default_hooks('field_instance');

    // Invoke each Feature to see if they provide default field instances,
    // so that we can have all possible field identifiers.
    foreach (array_keys(features_get_features()) as $module) {
      if (module_hook($module, $default_hook)) {
        $cache = array_merge($cache, call_user_func("{$module}_{$default_hook}"));
      }
    }
  }
  return $cache;
}
