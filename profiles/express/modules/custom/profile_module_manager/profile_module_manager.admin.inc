<?php

/**
 * Page callback for admin/config/development/module-manager
 */
function profile_module_manager_admin_settings() {
  $form = array();

  $form['profile_module_manager_disable_ui_lock'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('profile_module_manager_disable_ui_lock', 0),
    '#title' => t('Disable UI Lock'),
    '#description' => t('Disables the lock on ability enable or disable modules required by install profile through the user interface.  This also impacts drush.'),
  );

  return system_settings_form($form);

}

/**
 * Page callback to manage modules in code.
 */
function profile_module_manager_export($form, $form_state) {

  $form = array();
  $form['help'] = array(
    '#markup' => t('The Module Manager module stores this array in the module and compares what is enabled on each site to it.'),
  );
  $output = '';

  $result = db_query("SELECT name FROM {system} WHERE status = 1 AND type = 'module'  ORDER BY name ASC");

  $output .= "  return array(\n";

  foreach ($result as $module) {
    $output .= "      '$module->name' => '$module->name',\n";
  }
  $output .= " );\n";
  $lines = explode("\n", $output);

  $form['export'] = array(
    '#title' => t('Module array output -- update in YOUR_module_manager.module'),
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => count($lines),
    '#default_value' => $output,
    '#description' => t('Module .module file.'),
  );
  return $form;
}

/**
 * Page callback to generate roles and permissions in code.
 */
function profile_module_manager_bundle_form($form, $form_state) {
  $form = array();
  $form['help'] = array(
    '#markup' => t('Bundles add functionality to your site, but this also adds complexity.  Bundles cannot be removed once enabled.'),
    '#prefix' => '<div class="module-bundle-description">',
    '#suffix' => '</div>',
  );

  $bundles = db_query("SELECT name, filename FROM {system} WHERE type = 'module' ORDER BY weight ASC, name ASC")->fetchAllAssoc('name');

  // if there are no bundles that can be enabled, it should say something?  Show as locked checkbox?

  // check for _bundle in the machine name, but also look for something in the info
  // to avoid false positives from contrib?
  foreach ($bundles as $key => $bundle) {
    if (!strpos($bundle->name, '_bundle')) {
      unset($bundles[$key]);
    }
  }
  $columns =5;
  $bundle_chunks = array_chunk($bundles, $columns, TRUE);
  foreach ($bundle_chunks as $key => $chunk) {
    $form['bundle_group_' . $key] = array(
      '#markup' => '',
      '#prefix' => '<div class="module-bundle-group">',
      '#suffix' => '</div>',
    );
    foreach ($chunk as $bundle) {
    // is it a bundle?

      //dsm($bundle);
      $info_file = str_replace(".module", ".info", $bundle->filename);
      $info = drupal_parse_info_file($info_file);
      //dsm($info);
      $enabled = module_exists($bundle->name) ? 1 : 0;

      $form['bundle_group_' . $key][$bundle->name] = array(
        '#type' => 'checkbox',
        '#default_value' => 0,
        '#title' => $info['name'],
        '#description' => $info['description'],
        '#default_value' => $enabled,
        '#disabled' => $enabled,
      );
    }
    for ($i = count($chunk); $i < $columns; $i++) {
      $form['bundle_group_' . $key]['bundle_group_spacer_' . $i] = array(
        '#markup' => '',
        '#prefix' => '<div class="module-bundle-group-spacer">',
        '#suffix' => '</div>',
      );
    }
  }


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#prefix' => '<div class="module-bundle-submit">',
    '#suffix' => '</div>',
  );

  return $form;
}

// ADD SUBMIT HANDLER THAT ENABLES MODULE AND DEPENDENCIES

/**
 * Submit handler for invite_form.
 *
 * Add entry to database and send email.
 */
function profile_module_manager_bundle_form_submit(&$form, &$form_state) {
  //dsm($form_state);
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, '_bundle') && $value) {
	  // look up depenencies & enable those first
	  module_enable(array($key), FALSE);
	}
  }
}
