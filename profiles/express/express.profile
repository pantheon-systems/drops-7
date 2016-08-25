<?php
/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Allows the profile to alter the site configuration form.
 */
if (!function_exists("system_form_install_configure_form_alter")) {
  function system_form_install_configure_form_alter(&$form, $form_state) {
    $form['site_information']['site_name']['#default_value'] = 'express';
  }
}

/**
 * Implements hook_form_alter().
 *
 * Select the current install profile by default.
 */
if (!function_exists("system_form_install_select_profile_form_alter")) {
  function system_form_install_select_profile_form_alter(&$form, $form_state) {
    foreach ($form['profile'] as $key => $element) {
      $form['profile'][$key]['#value'] = 'express';
    }
  }
}


/**
 * Implements hook_install_tasks().
 */
function express_install_tasks() {
  // @TODO: This looks nothing like http://cgit.drupalcode.org/drupal/tree/includes/install.core.inc?h=7.x#n525
  // should it?
  $tasks = array();

  $tasks['express_profile_configure_form'] = array(
    'display_name' => st('Configure Express profile'),
    'type' => 'form',
  );

  $tasks['express_final'] = array();
  return $tasks;
}

function express_profile_configure_form() {
  $form = array();

  $options = array(
    'cu_core' => st('Production'),
    'cu_testing_core' => st('Testing'),
  );

  $form['express_core_version'] = array(
    '#type' => 'radios',
    '#title' => st('Which version of Express would you like to install?'),
    '#description' => st('Testing will include the "cu_testing_core" module while "Production" will include the "cu_core" module.'),
    '#options' => $options,
    '#default_value' => 'cu_core',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => st('Create and Finish'),
    '#weight' => 15,
  );

  return $form;
}

function express_profile_configure_form_submit(&$form, &$form_state) {
  variable_set('express_core_version', $form_state['values']['express_core_version']);
}

/**
 * Final configurations for Express.
 */
function express_final() {

  // MOVED HERE TO FIX FIT-1684
  module_enable(array('entityreference'));
  module_enable(array('express_layout'));

  // We know for sure that our database name is unique and thus, I'm using that
  // to append to the email.  Another option was base_path(), but that isnt
  // known during the install process.  $plus = str_replace('/', '_',
  // trim(base_path(), '/'));
  global $databases;
  $plus = $databases['default']['default']['database'];
  variable_set('site_mail', 'cudrupal+' . $plus . '@gmail.com');

  // Place the system-main block in the content region.
  $update = db_update('block')
    ->fields(array(
      'status' => 1,
      'region' => 'content',
      'weight' => 0,
    ))
    ->condition('module', 'system')
    ->condition('delta', 'main')
    ->execute();

  // Set subnaviagtion block title to <none>
  db_query("UPDATE {block} SET title = '<none>' WHERE delta = 'site_navigation_menus-1'");
  db_query("UPDATE {block} SET title = '<none>' WHERE delta = 'site_navigation_menus-4'");


  // @TODO: figure out why these are enabled by default
  module_disable(array('update'));
  theme_disable(array('bartik'));

  // Enable custom modules that you want enabled at the end of profile install.
  //if (file_exists(DRUPAL_ROOT . '/profiles/express/modules/custom/express_final/express_final.module')) {
    //module_enable(array('express_final'));
  //}

  // Enabled cu_users and rebuild secure permissions (after a static reset).
  module_enable(array('secure_permissions'));
  drupal_static_reset();

  module_enable(array('express_permissions'));

  // Add core module based on selection from profile install form.
  if ($core = variable_get('express_core_version', '')) {
    module_enable(array($core));
  }

  // Update modules to ignore.
  profile_module_manager_add_to_ignore(array('entityreference', 'express_layout', 'secure_permissions', 'express_permissions'));

  // Rebuild list of content types for disable_node_menu_item.
  $types = node_type_get_names();
  variable_set('dnmi_content_types', array_flip($types));

  drupal_flush_all_caches();
  secure_permissions_rebuild();

}



/**
 * Implements hook_themes_enabled().
 *
 * Makes sure blocks are set properly on structure/blocks for all new themes.
 */
function express_themes_enabled() {
  $query = db_update('block')
    ->fields(array(
      'region' => '-1',
      'status' => '0',
    ))
    ->execute();
  $query = db_update('block')
    ->fields(array(
      'region' => 'content',
      'status' => '1',
    ))
    ->condition('delta', 'main')
    ->execute();
  $query = db_update('block')
    ->fields(array(
      'region' => 'help',
      'status' => '1',
    ))
    ->condition('delta', 'help')
    ->execute();
}

/**
 * Implements hook_node_type_insert().
 */
function express_node_type_insert($info) {
  // rebuild list of content types for disable_node_menu_item
  if (!in_array($info->type, $types = variable_get('dnmi_content_types', array()))) {
    $types[$info->type] = $info->type;
    variable_set('dnmi_content_types', $types);
  }
}

/**
 * Implements hook_node_type_delete().
 */
function express_node_type_delete($info) {
  if (in_array($info->type, $types = variable_get('dnmi_content_types', array()))) {
    unset($types[$info->type]);
    variable_set('dnmi_content_types', $types);
  }
}

/**
 * Implements hook_menu_alter.
 * Most Express sites have a People or Person content type. There is a big difference
 * between a user and content about staff, but using People for both confuses many
 * site owners.
 */
function express_menu_alter(&$items) {
  //@TODO: move to express_settings?
  // tried but didn't work.  Not sure why, but out of time.
  $items['admin/people']['title'] = 'Users';
}
