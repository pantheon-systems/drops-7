<?php
/**
 * @file
 * All the menus, pages, and functionality related to administering entities.
 */

/**
 * Entity related menu items.
 */
function eck__entity__menu($entity_type, $bundle) {
  $path = eck__entity_type__path();
  $menu = array();

  // DELETE Bundles.
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/delete"] = array(
    'title' => "Delete",
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__bundle__delete_form', 3, 4),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(
      array(
        'eck administer bundles',
        'eck delete bundles',
        "eck administer {$entity_type->name} bundles",
        "eck delete {$entity_type->name} bundles",
      ),
    ),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
  );

  // EDIT Bundles.
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/edit"] = array(
    'title' => 'Edit',
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'eck__bundle__edit_form',
      $entity_type->name,
      $bundle->name,
    ),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(
      array(
        'eck administer bundles',
        'eck edit bundles',
        "eck administer {$entity_type->name} bundles",
        "eck edit {$entity_type->name} bundles",
      ),
    ),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
  );

  $admin_info = get_bundle_admin_info($entity_type->name, $bundle->name);

  // LIST Entities.
  $menu[$admin_info['path']] = array(
    'title' => "{$bundle->label}",
    'description' => "View all entites of type {$entity_type->label} with bundle {$bundle->label}",
    'page callback' => "eck__entity__list",
    'page arguments' => array($entity_type->name, $bundle->name),
    'access callback' => 'eck__multiple_access_check',
    'access arguments' => array(
      array(
        'eck administer entities',
        "eck list entities",
        "eck administer {$entity_type->name} {$bundle->name} entities",
        "eck list {$entity_type->name} {$bundle->name} entities",
      ),
    ),
    'weight' => 0,
    'file' => 'eck.entity.inc',
  );

  $menu[$admin_info['path'] . "/list"] = array(
    'title' => "Entity List",
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100,
  );

  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);

  foreach ($crud_info as $action => $info) {
    $action_label = ucfirst($action);
    $args = array();

    if (array_key_exists('entity_id', $info)) {
      $args[] = $info['entity_id'];
    }

    $args = array_merge(array($entity_type->name, $bundle->name), $args);
    $access_args = array_merge(array($action), $args);

    $menu[$info['path']] = array(
      'title' => "{$action_label} {$bundle->label}",
      'description' => "{$action_label} an entity of type {$entity_type->label} with bundle {$bundle->label}",
      'page callback' => "eck__entity__{$action}",
      'page arguments' => $args,
      'load arguments' => array($entity_type->name),
      'access callback' => 'eck__entity_menu_access',
      'access arguments' => $access_args,
      'file' => 'eck.entity.inc',
    );

    if ($action == 'view') {
      $menu[$info['path'] . "/view"] = array(
        'title' => "View",
        'type' => MENU_DEFAULT_LOCAL_TASK,
        'weight' => 0,
      );

      $weight = 1;
      foreach ($crud_info as $a => $i) {
        if ($a != 'view' && $a != 'add' && $a != "list") {
          $al = ucfirst($a);
          $view_path = $info['path'] . "/{$a}";
          $access_args = array_merge(array($a), $args);
          $menu[$view_path] = array(
            'title' => "{$al}",
            'description' => "{$action_label} an entity of type {$entity_type->label} with bundle {$bundle->label}",
            'page callback' => "eck__entity__{$a}",
            'page arguments' => $args,
            'access callback' => 'eck__entity_menu_access',
            'access arguments' => $access_args,
            'file' => 'eck.entity.inc',
            'type' => MENU_LOCAL_TASK,
            'context' => (MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE),
            'weight' => $weight,
          );
          $weight++;
        }
      }
    }
  }

  return $menu;
}

/**
 * Get admin bundle information from the entity info array.
 */
function get_bundle_admin_info($entity_type, $bundle) {
  $info = entity_get_info();

  return $info[$entity_type]['bundles'][$bundle]['admin'];
}

/**
 * Get bundle crud information from the entity info array.
 */
function get_bundle_crud_info($entity_type_name, $bundle_name) {
  $info = entity_get_info();
  return $info[$entity_type_name]['bundles'][$bundle_name]['crud'];
}

/**
 * Entity overview page callback.
 */
function eck__entity__list($entity_type_name, $bundle_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);

  $info['entity_type'] = $entity_type->name;
  $info['bundle'] = $bundle->name;

  $table = "eck_{$entity_type->name}";

  // Get all entity instances of this type.
  $query = new EntityFieldQuery();
  $query
  ->entityCondition('entity_type', $entity_type->name, '=')
  ->entityCondition('bundle', $bundle->name, '=')
  ->pager(20);

  drupal_alter('entity_overview_query', $query, $info);
  unset($info['entity_type']);
  drupal_alter("entity_{$entity_type->name}_overview_query", $query, $info);
  drupal_alter("entity_{$entity_type->name}_{$bundle->name}_overview_query", $query);

  $results = $query->execute();
  if (!empty($results)) {
    $entities = entity_load($entity_type->name, array_keys($results[$entity_type->name]));
  }
  else {
    $entities = array();
  }

  $destination = drupal_get_destination();

  // Because of the flexible paths capabilities, we are not guaranteed to see a
  // local action for the add here, so lets add a link ourselves until we figure
  // out whether there is a better solution.
  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);

  // Check that the user has permissions to add an entity:
  if (eck__entity_menu_access('add', $entity_type->name, $bundle->name)) {
    $build['actions'] = array(
      '#theme' => 'links',
      '#links' => array(
        array(
          'title' => t("Add") . " $bundle->label",
          'href' => $crud_info['add']['path'],
          'query' => $destination,
        ),
      ),
      '#attributes' => array(
        'class' => array('action-links'),
      ),
    );
  }

  // Check that the user has permissions to view entity lists:
  $check_array = array(
    'eck administer entities',
    'eck list entities',
    "eck administer {$entity_type->name} {$bundle->name} entities",
    "eck list {$entity_type->name} {$bundle->name} entities",
  );
  if (eck__multiple_access_check($check_array)) {
    $build['table'] = entity_table($entities, TRUE);
  }

  $build['pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Call back for the local action add (It adds a new entity).
 *
 * @param string $entity_type_name
 *   Entity type.
 * @param string $bundle_name
 *   Bundle.
 */
function eck__entity__add($entity_type_name, $bundle_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);

  $entity = entity_create($entity_type->name, array('type' => $bundle->name));
  return drupal_get_form("eck__entity__form_add_{$entity_type_name}_{$bundle_name}", $entity);
}

/**
 * Get the entities view.
 *
 * @param string $entity_type
 *   entity type
 * @param mixed $id
 *   The entity id or the entity object itself
 *   Normally you wouldn't need to call this function if you already have the
 *   loaded entity but there are some workflows where an object is passed.
 *   So this function handles that case as well.
 */
function eck__entity__build($entity_type, $bundle, $id, $view_mode = "full") {

  if (is_object($id) && $id->entityType() == $entity_type->name && $id->bundle() == $bundle->name) {
    return $id->view($view_mode);
  }
  elseif (is_numeric($id)) {
    $entity = entity_load_single($entity_type->name, $id);
    return $entity->view($view_mode);
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * Callback function for an entities edit page.
 *
 * @param string $entity_type_name
 *   Entity type.
 * @param string $bundle_name
 *   Bundle.
 * @param int $id
 *   the Id of the entity to be edited.
 */
function eck__entity__edit($entity_type_name, $bundle_name, $id) {
  if (is_numeric($id)) {
    $entity = entity_load_single($entity_type_name, $id);
  }
  elseif (is_object($id) and $id->entityType() === $entity_type_name) {
    $entity = $id;
  }

  return drupal_get_form("eck__entity__form_edit_{$entity_type_name}_{$bundle_name}", $entity);
}

/**
 * Callback function for the delete functionality.
 */
function eck__entity__delete($entity_type_name, $bundle_name, $entity) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  return drupal_get_form('eck__entity__delete_form', $entity_type, $bundle, $entity);
}

/**
 * Entity delete form.
 */
function eck__entity__delete_form($form, &$form_state, $entity_type, $bundle, $entity) {
  $path = eck__entity_type__path();

  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );

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
    '#value' => "{$path}/{$entity_type->name}/{$bundle->name}",
  );

  $message = t("Are you sure that you want to delete %id",
    array("%id" => entity_id($entity_type->name, $entity)));

  $caption = t("This action cannot be undone.");

  return confirm_form($form, $message, "{$path}/{$entity_type->name}", $caption, t('Delete'));

}

/**
 * Sumbmit function for the delete functionality.
 */
function eck__entity__delete_form_submit($form, &$form_state) {
  $entity = $form_state['values']['entity'];
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $submit_redirect = $form_state['values']['submit_redirect'];

  $entity->delete();

  // Ok, lets delete the entity.
  $form_state['redirect'] = $submit_redirect;
}

/**
 * Sets up an entities form.
 */
function eck__entity__form($form, &$form_state, $entity) {
  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );
  $form['#entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity->type,
  );

  $form_state[$entity->entityType()] = $entity;

  // Property Widget Handling through property_info by entity api.
  $property_info = entity_get_property_info($entity->entityType());
  $properties = array();
  $found_widget = FALSE;
  foreach ($property_info['properties'] as $pname => $pi) {
    if (array_key_exists('widget', $pi)) {
      $widget_callback = $pi['widget'];
      $widget = $widget_callback($entity);
      $properties[$pname] = $widget_callback;
      $form[$pname] = $widget_callback($entity);
      $found_widget = TRUE;
    }
  }

  if (!$found_widget) {
    // If there was no widget given through the property_info array, we look for
    // a widget in the property behaviors implemented.
    $entity_type = $entity->entityType();
    $entity_type = EntityType::loadByName($entity_type);

    $vars = array('entity' => $entity);
    $vars += $property_info;

    $widgets = eck_property_behavior_invoke_plugin($entity_type, 'default_widget', $vars);

    foreach ($widgets as $property => $widget) {
      $form[$property] = $widget;
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  // Entity Translation module integration.
  $langcode = LANGUAGE_NONE;
  if (module_exists('entity_translation') &&
    entity_translation_enabled($entity->entityType())) {
    $handler = entity_translation_get_handler($entity->entityType(), $entity);
    $langcode = $handler->getFormLanguage();
  }

  field_attach_form($entity->entityType(), $entity, $form, $form_state, $langcode);

  return $form;
}

/**
 * Validation function for entity form for validating the fields.
 */
function eck__entity__form_validate($form, &$state) {
  $entity = $state['values']['entity'];
  field_attach_form_validate($entity->entityType(), $entity, $form, $state);
}

/**
 * Submit function for entity form.
 */
function eck__entity__form_submit($form, &$state) {
  $entity = $state['values']['entity'];

  field_attach_submit($entity->entityType(), $entity, $form, $state);

  $entity_type = $entity->entityType();
  $entity_type = EntityType::loadByName($entity_type);
  $properties = $entity_type->properties;

  // Collect all form values for properties to be passed to pre_set behavior.
  $values = array();
  foreach ($properties as $property => $info) {
    // Pass form values "as are" because for example NULL values are valid.
    // The isset() does not return TRUE for array keys that correspond to a NULL
    // value, while array_key_exists() does.
    if (array_key_exists($property, $state['values'])) {
      $values[$property] = array('data' => $state['values'][$property]);
    }
  }

  // @TODO: maybe we should do this in the form validation step. That way we can
  // catch entity wrapper validation exceptions and set a form error message
  // appropriately.

  // Lets give the behavior a chance to manipulate the data before it is set.
  $data = eck_property_behavior_invoke_plugin($entity_type, 'pre_set', array('entity' => $entity), $values);

  // Now that behaviors got a chance to manipulate the values given by the user,
  // lets set them in the entity.
  $wrapper = entity_metadata_wrapper($entity_type->name, $entity);
  foreach ($properties as $property => $info) {
    // Use the value added by "pre_set" behavior callback, if any.
    if (array_key_exists($property, $data)) {
      $value = $data[$property];
    }
    // Otherwise use the un-altered value.
    elseif (isset($values[$property])) {
      $value = $values[$property]['data'];
    }
    // Else exit, preventing any value to be set to this property.
    else {
      continue;
    }

    $wrapper->{$property}->set($value);
  }

  // Let the behaviors modify the entity.
  // @todo Why do we need to pass form information to the save behavior.
  // This is related to eck_revisions. Is there a danger that the current
  // eck_revision logic will not apply when entities are manipulated from
  // code and not the UI?
  eck_property_behavior_invoke_plugin($entity_type, 'entity_save',
    array('entity' => $entity, 'form' => $form, 'form_state' => $state));

  $entity->save();

  $msg = 'Entity @entity_id - @entity_label has been saved';
  $args = array(
    '@entity_id' => entity_id($form['#entity_type'], $entity),
    '@entity_label' => entity_label($form['#entity_type'], $entity),
  );

  $context = array('entity' => $entity, 'form' => $form, 'form_state' => $state);
  drupal_alter('eck_entity_save_message', $msg, $args, $context);

  if ($msg) {
    drupal_set_message(t($msg, $args));
  }

  $uri = entity_uri($entity->entityType(), $entity);
  $state['redirect'] = $uri['path'];
}

/**
 * Creates a renderable array to show an entity.
 *
 * @param string $entity_type_name
 *   Entity type.
 * @param string $bundle_name
 *   Bundle.
 * @param mixed $id
 *   ID or Entity Object being viewed.
 */
function eck__entity__view($entity_type_name, $bundle_name, $id) {
  if (is_object($id) && $id->bundle() == $bundle_name) {
    $entity = $id;
  }
  // If the bundle does not match, we are trying to view and entity of the
  // right type but incorrect bundle.
  elseif (is_object($id) && $id->bundle() != $bundle_name) {
    drupal_not_found();
    exit();
  }
  else {
    $entity = entity_load_single($entity_type_name, $id);
  }

  return entity_view($entity_type_name, array($entity));
}

/**
 * Entity view callback.
 */
function eck__entity__view_callback($entities, $view_mode = 'full', $langcode = NULL) {
  $build = array();

  foreach ($entities as $entity) {
    $entity_type_name = $entity->entityType();
    $bundle_name = $entity->type;

    $entity_type = entity_type_load($entity_type_name);
    $bundle = bundle_load($entity_type_name, $bundle_name);

    $entity_view = eck__entity__build($entity_type, $bundle, $entity, $view_mode);
    $property_view = array();

    $formatters = eck_property_behavior_invoke_plugin($entity_type, 'default_formatter',
      array('entity' => $entity));

    foreach ($formatters as $property => $formatter) {
      $property_view[$property] = $formatter;
    }
    $entity_id = entity_id($entity_type_name, $entity);
    $entity_view = array_merge($property_view, $entity_view[$entity_type_name][$entity_id]);

    eck_property_behavior_invoke_plugin($entity_type, 'entity_view',
      array('entity' => $entity));

    $build[$entity_type_name][$entity_id] = $entity_view;
  }

  return $build;
}

/**
 * Get the value for a property from a form's state.
 */
function _eck_form_property_value($state, $property) {
  if (array_key_exists($property, $state['values'])) {
    return $state['values'][$property];
  }
  return NULL;
}
