<?php

/**
 * @file
 * Handles properties attached to entites created through ECK.
 */

/**
 * Assigns fields to the db schema for an entity type.
 *
 * @param array $schema
 *   A schema array.
 */
function eck_set_properties_schema(&$schema, $entity_type) {
  $properties = $entity_type->properties;

  foreach ($properties as $name => $info) {
    $type = $info['type'];
    $schema['fields'][$name] = eck_property_type_schema($type);
  }
}

/**
 * The different property types.
 */
function eck_property_types() {
  // All types: serial, int, float, numeric, varchar, char, text,
  // blob, datetime.
  $default_types = array(
    'text' => t('Text'),
    'integer' => t('Integer'),
    'decimal' => t('Decimal'),
    'positive_integer' => t('Unsigned Integer'),
    'language' => t('Language'),
  );

  $module_default_types = module_invoke_all('eck_property_types');
  $default_types = array_merge($default_types, $module_default_types);
  drupal_alter('eck_property_types', $default_types);

  // @todo As modules start to implement ECK hooks, this should be removed.
  if (module_exists('uuid') && !array_key_exists('uuid', $default_types)) {
    $default_types['uuid'] = t('UUID');
  }

  return $default_types;
}

/**
 * The property types schemas.
 */
function eck_property_type_schema($type) {
  $schema = array();

  switch ($type) {
    case 'text':
      $schema = array(
        'description' => 'Text',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    case 'decimal':
      $schema = array(
        'description' => 'Decimal',
        'type' => 'float',
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'integer':
      $schema = array(
        'type' => 'int',
        'description' => "Integer",
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'positive_integer':
      $schema = array(
        'type' => 'int',
        'description' => "Integer",
        'not null' => TRUE,
        'unsigned' => TRUE,
        'default' => 0,
      );
      break;

    case 'uuid':
      $schema = array(
        'type' => 'char',
        'length' => 36,
        'not null' => TRUE,
        'default' => '',
        'description' => 'The Universally Unique Identifier.',
      );
      break;

    case 'language':
      $schema = array(
        'description' => 'Language',
        'type' => 'varchar',
        'length' => 12,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    default:
      $schema = NULL;
      break;

  }

  // @todo Don't really like this.
  drupal_alter('eck_property_type_schema', $schema, $type);

  return $schema;
}

/**
 * Properties form callback.
 */
function eck__properties__form($form, &$state, $entity_type_name) {
  if (!isset($entity_type_name)) {
    $entity_type = new EntityType();
  }
  else {
    $entity_type = EntityType::loadByName($entity_type_name);
  }

  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);

  $form['info'] = array(
    '#markup' => "<p><strong>HELP:</strong> "
    . "The add property button will temporarily add a property to the entity "
    . "type, to finalize adding the property, click the checkbox on the left and "
    . "click save </p>");

  // When the add button is clicked, the submit callback directs the form to
  // be rebuilt. Here we check to see if a new property was added, and keep it
  // in a a list of new properties to add them to the table on each rebuild.
  $entity_type_properties = $entity_type->properties;

  if (!array_key_exists('values', $state) || !array_key_exists('new_properties', $state['values'])) {
    $new_properties = eck_get_default_properties();
    $new_properties = array_merge($new_properties, $entity_type_properties);
  }
  else {
    $new_properties = $state['values']['new_properties'];
  }

  if (!empty($state['values']['property_name'])) {
    $property_name = $state['values']['property_name'];
    $property_type = $state['values']['property_type'];

    // OK we need to do a little bit of filtering do the oddity of modifying
    // existing properties through the add Property form #fail.
    // @todo need to get a better interface.
    // But until then, we need to keep the form from changing the
    // property_type or property_name of the already existing properties.

    if (!array_key_exists($property_name, $new_properties) ||
      $new_properties[$property_name]['type'] == $property_type) {

      $new_properties[$property_name] = array(
        'label' => $state['values']['property_label'],
        'type' => $state['values']['property_type'],
      );

      if (array_key_exists('property_behavior', $state['values'])) {
        $new_properties[$state['values']['property_name']]['behavior'] = $state['values']['property_behavior'];
      }
    }
    else {
      drupal_set_message("Can not modify the type of {$property_name}", "error");
    }
  }

  $form['new_properties'] = array('#type' => 'value', '#value' => $new_properties);

  // This is the select table where all the new properties are shown.
  $header = array(
    'machine_name' => t('Machine Name'),
    'name' => t('Name'),
    'type' => t('Type'),
    'behavior' => t('Behaviour'),
  );

  $options = array();

  // @todo can we do this in a better way.. the only way I can think of is,
  // load all defaults, and then load all the properties in the entity type,
  // while modifying the before added default properties given the entity
  // type settings.

  $property_arrays = array('new_properties');

  foreach ($property_arrays as $array_name) {
    foreach (${$array_name} as $property_name => $property_info) {
      $options[$property_name] = array(
        'machine_name' => $property_name,
        'name' => $property_info['label'],
        'type' => $property_info['type'],
      );

      if (array_key_exists('behavior', $property_info)) {
        $options[$property_name]['behavior'] = $property_info['behavior'];
      }
      else {
        $options[$property_name]['behavior'] = "";
      }
    }
  }

  $form["new_properties_table_label"] = array(
    '#markup' => '<h3>Properties</h3>',
  );

  $defaults = array();
  foreach ($entity_type_properties as $property => $info) {
    $defaults[$property] = 1;
  }

  $form['new_properties_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No other properties for this entity type.'),
    '#default_value' => $defaults,
  );

  // Add a new property.
  $types[t('Generic')] = eck_property_types();

  $form["add_new_property"] = array(
    '#markup' => '<h3>Add new property</h3>',
  );

  $form['property_type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('' => t('- Please choose -')) + $types,
    '#required' => TRUE,
    '#after_build' => array('eck_deactivate_on_save'),
  );

  $form["property_label"] = array(
    '#type' => 'textfield',
    '#title' => t("Name"),
    '#description' => t("A human readable name for the property."),
    '#required' => TRUE,
    '#after_build' => array('eck_deactivate_on_save'),
  );

  $form["property_name"] = array(
    '#type' => 'machine_name',
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('property_label'),
    ),
    '#after_build' => array('eck_deactivate_on_save'),
  );

  $behavior_plugins = ctools_get_plugins('eck', 'property_behavior');
  $options = array();

  foreach ($behavior_plugins as $behavior => $info) {
    $options[$behavior] = $info['label'];
  }

  $form['property_behavior'] = array(
    '#type' => 'select',
    '#title' => t('Behavior'),
    '#options' => array('' => t('- Please choose -')) + $options,
  );

  $form['property_add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Property'),
  );

  $form['line_break'] = array(
    '#markup' => '<br \><hr \><br \>',
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Deactivate on save.
 */
function eck_deactivate_on_save($element, &$state) {
  if (array_key_exists('input', $state) && array_key_exists('op', $state['input']) &&
    $state['input']['op'] == t('Save')) {
    isset($element['#element_validate']) ? $element['#element_validate'] = NULL : NULL;
    isset($element['#needs_validation']) ? $element['#needs_validation'] = NULL : NULL;
  }

  return $element;
}

/**
 * Properties form validation callback.
 */
function eck__properties__form_validate($form, &$state) {
  if ($state['values']['op'] == t('Add Property')) {
    if (isset($state['values']['property_behavior'])) {
      $property_behavior = $state['values']['property_behavior'];
      $behavior_plugins = ctools_get_plugins('eck', 'property_behavior');
      if (isset($behavior_plugins[$property_behavior]['unique']) && $behavior_plugins[$property_behavior]['unique']) {
        foreach ($state['values']['new_properties'] as $property_name => $property_info) {
          if ($property_info['behavior'] == $property_behavior) {
            form_set_error('property_behavior',
              t("There can be only one property with '@name' behavior per entity.",
                array('@name' => $behavior_plugins[$property_behavior]['label'])));
          }
        }
      }
    }
  }
}

/**
 * Properties form submit callback.
 */
function eck__properties__form_submit($form, &$state) {
  if ($state['values']['op'] == t('Add Property')) {
    $state['rebuild'] = TRUE;
  }
  elseif ($state['values']['op'] == t('Save')) {
    // Here we want to add the properties to the entity type and save it.
    $entity_type = $state['values']['entity_type'];

    foreach ($state['values']['new_properties_table'] as $property => $active) {
      if ($active) {
        $info = $state['values']['new_properties'][$property];
        if (array_key_exists('behavior', $info)) {
          $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
        }
        else {
          $entity_type->addProperty($property, $info['label'], $info['type']);
        }
      }
      else {
        $entity_type->removeProperty($property);
      }
    }

    $entity_type->save();

    // Lets flush the cache so new behaviors and properties will get set up
    // correctly.
    drupal_flush_all_caches();
  }
}
