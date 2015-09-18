<?php
/**
 * @file
 * All of the menu, pages, and forms related to bundle administration.
 */

module_load_include('inc', 'eck', 'eck.classes');

/**
 * This function creates the menu items relevant to bundle administration.
 *
 * @param /EntityType $entity_type
 *   As returned by eck__entity_type__load().
 *
 * @see eck_menu()
 */
function eck__bundle__menu($entity_type) {
  // Create the menus relavant to types.
  $path = eck__entity_type__path();
  $menu = array();

  // DELETE Entity Types.
  $menu["{$path}/{$entity_type->name}/delete"] = array(
    'title' => "Delete",
    'description' => "Delete the '{$entity_type->label}' Entity Type",
    // "eck__entity_type__delete",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__entity_type__delete_form", $entity_type->name),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(array('eck administer entity types', 'eck delete entity types')),
    'file' => 'eck.entity_type.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 3,
  );

  // MANAGE Entity Type Properties.
  $menu["{$path}/{$entity_type->name}/properties"] = array(
    'title' => "Manage properties",
    'description' => "Manage the properties of the {$entity_type->label} entity type",
    // "eck__entity_type__delete",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__properties__form", $entity_type->name),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(
      array(
        'eck administer entity types',
        "manage {$entity_type->name} properties",
      ),
    ),
    'file' => 'eck.properties.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 4,
  );

  // LIST Bundles.
  $menu["{$path}/{$entity_type->name}"] = array(
    'title' => "{$entity_type->label}",
    'description' => "View all the bundles for '{$entity_type->label}'",
    'page callback' => "eck__bundle__list",
    'page arguments' => array(3),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(
      array(
        'eck administer bundles',
        'eck list bundles',
        "eck administer {$entity_type->name} bundles",
        "eck list {$entity_type->name} bundles",
      ),
    ),
    'file' => 'eck.bundle.inc',
  );

  $menu["{$path}/{$entity_type->name}/list"] = array(
    'title' => 'Bundle List',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100,
  );

  // EDIT Entity Types.
  $menu["{$path}/{$entity_type->name}/edit"] = array(
    'title' => "Edit",
    'description' => "Edit the '{$entity_type->label}' entity type",
    'page callback' => 'drupal_get_form',
    'page arguments' => array("eck__entity_type__form", $entity_type->name),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(array('eck administer entity types', 'eck edit entity types')),
    'file' => 'eck.entity_type.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
  );

  // ADD Bundles.
  $menu["{$path}/{$entity_type->name}/add"] = array(
    'title' => "Add bundle ",
    'description' => "Add a(n) new '{$entity_type->label} Bundle'",
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__bundle__add', 3),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(
      array(
        'eck administer bundles',
        'eck add bundles',
        "eck administer {$entity_type->name} bundles",
        "eck add {$entity_type->name} bundles",
      ),
    ),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 0,
    'file' => 'eck.bundle.inc',
  );

  module_load_include('inc', 'eck', 'eck.entity');
  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
    $menu = array_merge($menu, eck__entity__menu($entity_type, $bundle));
  }

  return $menu;
}

/**
 * Page call back for the bundle overview table.
 *
 * to see and manipulate all created label of a given type.
 *
 * @param string $entity_type
 *   The name of the entity type.
 */
function eck__bundle__list($entity_type) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type);

  // Check that the user has permissions to view bundle lists.
  // @todo Do we really need this, can they be here without the right
  // permissions in the first place?
  if (eck__multiple_access_check(array(
        'eck administer bundles',
        'eck list bundles',
        "eck administer {$entity_type->name} bundles",
        "eck list {$entity_type->name} bundles",
      )
    )) {

    $header = array(t('Type'), array(
      'data' => t('Operations'),
      'colspan' => '1'));

    $rows = array();

    $bundles = Bundle::loadByEntityType($entity_type);
    usort($bundles, 'eck_alphabetical_cmp');

    foreach ($bundles as $bundle) {
      $bundle_label = $bundle->label;

      $admin_info = get_bundle_admin_info($entity_type->name, $bundle->name);

      $uri = $admin_info['path'];

      $allowed_operations = '';
      // Check that the user has permissions to delete:
      $perm_check = array(
        'eck administer bundles',
        'eck delete bundles',
        "eck administer {$entity_type->name} bundles",
        "eck delete {$entity_type->name} bundles",
      );

      if (eck__multiple_access_check($perm_check)) {
        $allowed_operations = l(t('delete'), $uri . "/delete");
      }

      // Check that the user can view lists of entities.
      $perm_check = array(
        'eck administer entities',
        'eck list entities',
        "eck administer {$entity_type->name} {$bundle->name} entities",
        "eck list {$entity_type->name} {$bundle->name} entities",
      );
      if (eck__multiple_access_check($perm_check)) {
        $label = l($bundle_label, url($uri, array('absolute' => TRUE)));
      }
      else {
        $label = $bundle_label;
      }

      $rows[] = array($label, $allowed_operations);
    }

    $build['bundle_table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );
  }

  return $build;
}

/**
 * Bundle add from callback.
 */
function eck__bundle__add($form, &$form_state, $entity_type) {
  $entity_type = entity_type_load($entity_type);
  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Type'),
    '#description' => "A Human readable name for the bundle",
  );

  $form['bundle_name'] = array(
    '#type' => 'machine_name',
    '#required' => FALSE,
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('bundle_label'),
    ),
  );

  $form['#validate'][] = 'eck__bundle__add_validate';

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Bundle add form validation callback.
 */
function eck__bundle__add_validate($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];

  // The type does not have to be unique in the table, but it should be unique
  // to its entity so we will check that here.
  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {

    if ($bundle->name == $form_state['values']['bundle_name']) {
      form_set_error('bundle', t("bundle @bundle_name already exists for this entity type",
        array("@bundle_name" => $bundle->name)));
    }
  }
}

/**
 * Add bundle form callback.
 */
function eck__bundle__add_submit($form, &$form_state) {
  $path = eck__entity_type__path();

  $entity_type = $form_state['values']['entity_type'];
  $bundle_name = $form_state['values']['bundle_name'];
  $bundle_label = $form_state['values']['bundle_label'];

  $bundle = new Bundle();
  $bundle->name = $bundle_name;
  $bundle->label = $bundle_label;
  $bundle->entity_type = $entity_type->name;
  $bundle->save();

  drupal_set_message(t('the bundle %bundle for entity type %entity_type has been created.',
      array('%bundle' => $bundle->label, '%entity_type' => $entity_type->label)));

  $form_state['redirect'] = "{$path}/{$entity_type->name}";
}

/**
 * Form constructor for the entity bundle editing form.
 *
 * @param string $entity_type_name
 *   Entity type name.
 * @param string $bundle_name
 *   Entity bundle name.
 */
function eck__bundle__edit_form($form, &$form_state, $entity_type_name, $bundle_name) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  $form = array();

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );

  // Let the behaviors to modify form.
  $vars = eck_property_behavior_invoke_plugin_alter($entity_type, 'bundle_form', array('form' => $form, 'form_state' => $form_state));
  $form = $vars['form'];
  $form_state = $vars['form_state'];

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save bundle'),
    '#weight' => 40,
  );
  return $form;
}

/**
 * Budle edit form submit callback.
 */
function eck__bundle__edit_form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  // Save all form values starting from 'config_' to the $bundle->config array.
  $bundle_config = array();
  foreach ($form_state['values'] as $name => $value) {
    if (strpos($name, 'config_') === 0) {
      // We don't need 'config_' prefix.
      $bundle_config[substr($name, 7)] = $form_state['values'][$name];
    }
  }
  $bundle->config = $bundle_config;
  // Save the bundle object to the database.
  $bundle->save();

  drupal_set_message(t('the bundle %bundle for entity type %entity_type has been updated.',
    array('%bundle' => $bundle->label, '%entity_type' => $entity_type->label)));
}

/**
 * Delete the bundle of a given entity type.
 *
 * @param \EntityType $entity_type
 *   The entity type of the bundle that will be deleted.
 * @param \Bundle $bundle
 *   The bundle to be deleted.
 *
 * @see eck.classes.inc
 */
function eck__bundle__delete($entity_type, $bundle) {
  // And finally we delete the bundle from the eck_type table.
  $bundle->delete();
  drupal_set_message(t('The bundle %bundle from the entity type %entity_type has been deleted.',
    array('%bundle' => $bundle->label, '%entity_type' => $entity_type->label)));
}

/**
 * Bundle delete form callback.
 */
function eck__bundle__delete_form($form, &$form_state, $entity_type_name, $bundle_name) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );

  $form['submit_redirect'] = array(
    '#type' => 'value',
    '#value' => "{$path}/{$entity_type->name}",
  );

  $message = t('Are you sure that you want to delete the bundle %bundle?',
    array('%bundle' => $bundle->label));

  $caption = t("All of the data (entities) associated with this bundle
  will be deleted. This action cannot be undone.");

  return confirm_form($form, $message, "{$path}/{$entity_type->name}", $caption, t('Delete'));
}

/**
 * Submit form callback to delete bundles.
 */
function eck__bundle__delete_form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $submit_redirect = $form_state['values']['submit_redirect'];

  eck__bundle__delete($entity_type, $bundle);

  // Ok, lets delete the entity.
  $form_state['redirect'] = $submit_redirect;
}

/**
 * Autocomplete functionality for entities using field labels.
 */
function eck__bundle__field_autocomplete($entity_type, $bundle, $string = "") {
  $field_label = get_bundle_field_label_info($entity_type, $bundle);
  $matches = array();

  if ($field_label) {
    $field = $field_label['field'];
    $language = $field_label['language'];
    $delta = $field_label['delta'];
    $column = $field_label['column'];
    $query = new EntityFieldQuery();
    // @todo Remove the addmetadata() call below once
    // http://drupal.org/node/997394 is fixed.
    $query->addMetadata('account', user_load(1));

    $query->entityCondition('entity_type', $entity_type->name, '=')->entityCondition('bundle', $bundle->name, '=')->fieldCondition($field, $column, $string, 'CONTAINS');

    $results = $query->execute();

    $entities = entity_load($entity_type->name, array_keys($results[$entity_type->name]));
    foreach ($entities as $id => $entity) {
      $matches[$id] = $entity->{$field}[$language][$delta][$column];
    }
  }

  drupal_json_output($matches);
}

/**
 * Helper function to determine if an entity wants to define a field as lable.
 */
function get_bundle_field_label_info($entity_type, $bundle) {

  $info = entity_get_info();

  if (array_key_exists('field label', $info[$entity_type->name]['bundles'][$bundle->name])) {
    return $info[$entity_type->name]['bundles'][$bundle->name]['field label'];
  }
  else {
    return NULL;
  }
}
