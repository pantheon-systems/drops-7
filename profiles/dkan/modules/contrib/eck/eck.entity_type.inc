<?php
/**
 * @file
 * ENTITY TYPE
 *
 * Entity Types represent types of data. Drupal core contains multiple
 * entity types nodes, users, vocabularies, etc.
 *
 * ECK allows you to create entity types. This file contains all
 * of the entity type specific functionality
 */

/**
 * Passthrough from hook_menu().
 *
 * It creates the menu items related to entity type management.
 */
function eck__entity_type__menu() {
  $menu = array();
  $path = eck__entity_type__path();

  // LIST Entity Types.
  // View all of the created entity types.
  $menu[$path] = array(
    'title' => 'Entity types',
    'description' => 'A centralized administrative section for entity types',
    'page callback' => 'eck__entity_type__list',
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(array('eck administer entity types', 'eck list entity types')),
    'file' => 'eck.entity_type.inc',
  );

  // ADD Entity Types.
  $menu["{$path}/add"] = array(
    'title' => 'Add entity type',
    'description' => 'Add a new entity type',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('eck__entity_type__form'),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(array('eck administer entity types', 'eck add entity types')),
    'type' => MENU_LOCAL_ACTION,
    'weight' => -1,
    'file' => 'eck.entity_type.inc',
  );

  module_load_include('inc', 'eck', 'eck.bundle');
  // Each entity type can have multiple bundles.
  // Now lets create the menus for the bundle administration of each
  // entity type.
  foreach (EntityType::loadAll() as $entity_type) {
    $menu = array_merge($menu, eck__bundle__menu($entity_type));
  }
  return $menu;
}

/**
 * Callback for the entity_type overview.
 */
function eck__entity_type__list() {
  $path = eck__entity_type__path();
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => '1'));
  $rows = array();

  $entity_types = EntityType::loadAll();
  usort($entity_types, 'eck_alphabetical_cmp');

  foreach ($entity_types as $entity_type) {
    $allowed_operations = '';
    // Check that the user has permissions to delete:
    if (eck__multiple_access_check(array('eck administer entity types', 'eck delete entity types'))) {
      $allowed_operations = l(t("delete"), "{$path}/{$entity_type->name}/delete");
    }

    if (eck__multiple_access_check(array(
      'eck administer bundles',
      'eck list bundles',
      "eck administer {$entity_type->name} bundles",
      "eck list {$entity_type->name} bundles",
    ))) {
      $label = l(t("@el", array("@el" => $entity_type->label)), "{$path}/{$entity_type->name}");
    }
    else {
      $label = t("@el", array("@el" => $entity_type->label));
    }

    $rows[] = array($label, $allowed_operations);
  }

  $build['entity_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $build;
}

/**
 * Entity type form callback.
 *
 * @param string $entity_type_name
 *   (optional) The machine name of the entity type to edit. If omitted, the
 *   form is presented for adding a new entity type.
 */
function eck__entity_type__form($form, &$state, $entity_type_name = NULL) {

  // If this form is being called on a new entity, create a new entity type
  // object, otherwise load the corresponding entity type.
  if (!isset($entity_type_name)) {
    $entity_type = new EntityType();
  }
  else {
    $entity_type = EntityType::loadByName($entity_type_name);
  }

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['entity_type_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Entity Type'),
    '#default_value' => $entity_type->label,
    '#description' => t('A human readable name for the entity type'),
    '#required' => TRUE,
    '#disabled' => !$entity_type->isNew,
  );

  $form['entity_type_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $entity_type->name,
    '#disabled' => !$entity_type->isNew,
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('entity_type_label'),
    ),
  );

  $form['#validate'][] = 'eck__entity_type__form_validate';

  // Only allow entering the desired bundle name when creating a new entity.
  if ($entity_type->isNew) {
    $form['bundle_label'] = array(
      '#type' => 'textfield',
      '#title' => 'Bundle (optional)',
      '#description' => 'A bundle with the same name as the entity type is created by default, this will override the default',
    );

    $form['bundle_name'] = array(
      '#type' => 'machine_name',
      '#required' => FALSE,
      '#machine_name' => array(
        'exists' => '_eck_fake_exists',
        'source' => array('bundle_label'),
      ),
    );
  }

  $form = eck__default_properties__form($form, $state, $entity_type);

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;

}

/**
 * Entity type form validation callback.
 */
function eck__entity_type__form_validate($form, &$state) {
  if ($form['entity_type']['#value']->isNew) {
    $info = entity_get_info();
    $entity_types = array_keys($info);
    $entity_type = $state['values']['entity_type_name'];
    if (in_array($entity_type, $entity_types)) {
      form_set_error('name', t("Entity Type '@entity_type' already exists"),
        array('@entity_type' => $entity_type));
    }
  }
}

/**
 * Submit handler for adding an entity type.
 */
function eck__entity_type__form_submit($form, &$state) {

  if ($state['values']['op'] == t("Save")) {
    // This are required so I don't have to do any checks.
    $entity_type = $state['values']['entity_type'];

    // If the entity type is new set up its name and its label
    // create its table, and add the default bundle to the
    // eck_bundle table.
    if ($entity_type->isNew) {

      // Set up name and label.
      $entity_type_name = $state['values']['entity_type_name'];
      $entity_type->name = $entity_type_name;

      $entity_type_label = $state['values']['entity_type_label'];
      $entity_type->label = $entity_type_label;

      // Add the bundle to the table.
      // Process the bundle input from the user.
      if (!empty($state['values']['bundle_name'])) {
        $bundle_name = $state['values']['bundle_name'];
        if (!empty($state['values']['bundle_label'])) {
          $bundle_label = $state['values']['bundle_label'];
        }
        else {
          $bundle_label = ucfirst($bundle_name);
        }
      }
      else {
        $bundle_name = $entity_type_name;
        $bundle_label = $entity_type_label;
      }

      // Let's set up the object and save it to the db.
      $bundle = new Bundle();
      $bundle->entity_type = $entity_type->name;
      $bundle->name = $bundle_name;
      $bundle->label = $bundle_label;
      $bundle->save();
    }

    // Lets handle the default properties.
    eck__default_properties__form_submit($form, $state, $entity_type);

    if ($entity_type->isNew) {
      drupal_set_message(t('The entity type %entity_type has been created.', array('%entity_type' => $entity_type->label)));
    }
    else {
      drupal_set_message(t('The entity type %entity_type has been updated.', array('%entity_type' => $entity_type->label)));
    }

    $entity_type->save();
  }

  $state['redirect'] = 'admin/structure/entity-type/' . $entity_type->name;
}

/**
 * Delete the entity type.
 *
 * @param EntityType $entity_type
 *   Entity type to be deleted, the $entity_type object can be loaded by using
 *   the method EntityType::loadByName($entity_type_name).
 *
 * @see eck.classes.inc
 */
function eck__entity_type__delete($entity_type) {
  $entity_type->delete();
  drupal_set_message(t('The entity type %entity_type has been deleted.', array('%entity_type' => $entity_type->label)));
}

/**
 * Delete entity type form callback.
 */
function eck__entity_type__delete_form($form, &$state, $entity_type_name) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type_name);
  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['submit_redirect'] = array(
    '#type' => 'value',
    '#value' => $path,
  );

  $message = t("Are you sure that you want to delete the entity type %entity_type?",
    array("%entity_type" => $entity_type->label));

  $caption = t("All of the data (entities and bundles) from this entity type will be deleted. This action cannot be undone.");

  return confirm_form($form, $message, $path, $caption, t('Delete'));
}

/**
 * Entity type delete form submit callback.
 */
function eck__entity_type__delete_form_submit($form, &$state) {
  $entity_type = $state['values']['entity_type'];
  $submit_redirect = $state['values']['submit_redirect'];

  // Ok, lets delete the entity type.
  eck__entity_type__delete($entity_type);

  $state['redirect'] = $submit_redirect;
}

/**
 * Create the default schema for an entity type.
 *
 * @param EntityType $entity_type
 *   Entity type as returned by eck__entity_type__load().
 *
 * Passthrough for hook_schema().
 */
function eck__entity_type__schema($entity_type) {
  $schema = array(
    'description' => "The base table for a(n) {$entity_type->name}.",
    'fields' => array(
      'id' => array(
        'description' => "The primary identifier for a(n) {$entity_type->name}.",
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type' => array(
        'description' => 'The bundle of the entity',
        'type' => 'varchar',
        'default' => '',
        'length' => 255,
        'not null' => TRUE,
      ),
    ),
    'primary key' => array('id'),
    // Add required schema metadata fields.
    // @see _drupal_schema_initialize().
    'module' => 'eck',
    'name' => "eck_{$entity_type->name}",
  );

  // Add properties to schema definition.
  eck_set_properties_schema($schema, $entity_type);

  return $schema;
}

/**
 * Generate the entity info for a specific entity.
 *
 * @param EntityType $entity_type
 *   as returned by eck__entity_type__load
 */
function eck__entity_type__info($entity_type) {
  module_load_include('inc', 'eck', 'eck.bundle');
  $info = array();
  $entity_type_label = $entity_type->label;

  $entity_class = "Entity";
  $controller_class = "EntityAPIController";

  $info[$entity_type->name] = array(
    'label' => t("@etl", array("@etl" => $entity_type_label)),
    'base table' => "eck_{$entity_type->name}",
    'entity class' => $entity_class,
    'controller class' => $controller_class,
    'form callback' => 'eck__entity__form',
    'access callback' => 'eck__entity_access',
    'module' => 'eck',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'id',
      'bundle' => 'type',
    ),
    'label callback' => 'eck__entity__label',
    'uri callback' => 'eck__entity__uri',
    // Bundles are defined by the entity types below.
    'bundles' => array(),
    // Bundle keys tell the FieldAPI how to extract information from the
    // bundle objects.
    'bundle keys' => array(
      'bundle' => 'type',
    ),
    // I guess we need at least one view mode for entity_view_modes (the module)
    // to work.
    'view modes' => array(
      'full' => array(
        'label' => t('Full'),
        'custom settings' => FALSE,
      ),
      'teaser' => array(
        'label' => t('Teaser'),
        'custom settings' => TRUE,
      ),
    ),
    // Inline entity form module integration.
    'inline entity form' => array('controller' => 'EckInlineEntityFormController'),
    'view callback' => 'eck__entity__view_callback',
  );

  $eck_path = eck__entity_type__path();
  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
    $bundle_label = $bundle->label;
    $path = "{$eck_path}/{$entity_type->name}/{$bundle->name}";

    $info[$entity_type->name]['bundles'][$bundle->name] = array(
      'label' => $bundle_label,
      'admin' => array(
        'path' => $path,
        'access callback' => 'eck__multiple_access_check',
        'access arguments' => array(
          array(
            'eck administer bundles',
            'eck edit bundles',
            "eck administer {$entity_type->name} bundles",
            "eck edit {$entity_type->name} bundles",
          ),
        ),
      ),
      'crud' => array(
        'add' => array(
          'path' => $path . "/add",
        ),
        'edit' => array(
          'path' => $path . "/%eckentity/edit",
          'entity_id' => 5,
        ),
        'delete' => array(
          'path' => $path . "/%eckentity/delete",
          'entity_id' => 5,
        ),
        'view' => array(
          'path' => "{$entity_type->name}/{$bundle->name}/%eckentity",
          'entity_id' => 2,
        ),
      ),
    );
  }

  return $info;
}

/**
 * Returns the path to the ECK admin section.
 */
function eck__entity_type__path() {
  return "admin/structure/entity-type";
}
