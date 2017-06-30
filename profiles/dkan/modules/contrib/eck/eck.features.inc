<?php
/**
 * @file
 * Integration with the Features module.
 */

/**
 * @{
 * ECK Entity Types.
 */

/**
 * Implements hook_features_export_options().
 */
function eck_entity_type_features_export_options() {
  module_load_include('inc', 'eck', 'eck.entity_type');
  $entity_types = array();
  foreach (EntityType::loadAll() as $entity_type) {
    $entity_types[$entity_type->name] = $entity_type->label;
  }
  return $entity_types;
}

/**
 * Implements hook_features_export().
 */
function eck_entity_type_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  foreach ($data as $entity_type) {
    // Export the entity type.
    $export['features']['eck_entity_type'][$entity_type] = $entity_type;
    $export['dependencies']['eck'] = 'eck';
    // @todo We need to add dependencies on the modules implementing the
    // property behaviors currently they are all implemented by ECK but in the
    // future, people might have their custom behaviors.. or we might have
    // behaviors provided by contrib.
    $export['dependencies']['features'] = 'features';
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function eck_entity_type_features_export_render($module, $data, $export = NULL) {
  // I am guessing that using FALSE and TRUE to initialize the array is
  // arbritrary, rigth?
  $elements = array(
    'name' => FALSE,
    'label' => TRUE,
    'properties' => FALSE,
  );

  $output = array();
  $output[] = '  $items = array(';

  foreach ($data as $entity_type_name) {
    $entity_type = EntityType::loadByName($entity_type_name);

    $elements['name'] = $entity_type->name;
    $elements['label'] = $entity_type->label;
    $elements['properties'] = $entity_type->properties;
    $output[] = '    ' . "'{$entity_type->name}' => " . features_var_export($elements, '    ') . ",";
  }

  $output[] = '  );';
  $output[] = '  return $items;';

  return array('eck_entity_type_info' => implode("\n", $output));
}

/**
 * Implements hook_features_revert().
 */
function eck_entity_type_features_revert($module) {
  eck_entity_type_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 */
function eck_entity_type_features_rebuild($module) {

  if ($default_entities = features_get_default('eck_entity_type', $module)) {
    foreach ($default_entities as $entity_type_name => $entity_type_info) {

      // Load the existing entity type, if one exists.
      $entity_type = EntityType::loadByName($entity_type_name);
      if (empty($entity_type->id)) {
        $entity_type = new EntityType();
      }

      // Look at all of the existing entities properties, and if one exists
      // that does not exist in the code definition, delete it.
      foreach ($entity_type->properties as $property_key => $property) {
        if (!isset($entity_type_info['properties'][$property_key])) {
          $entity_type->removeProperty($property_key);
        }
      }

      // Look at all properties as defined in code, and add or change them
      // Depending on wether they already exist or not.
      foreach ($entity_type_info as $key => $value) {
        if ($key == 'properties') {
          // Loop through the new properties.
          foreach ($entity_type_info['properties'] as $property_key => $property) {
            // If the property already exists, update the behavior.
            if (isset($entity_type->properties[$property_key])) {
              $entity_type->changeBehavior($property_key, $property['behavior']);
            }
            else {
              // Property didn't already exist, so lets create it.
              $entity_type->addProperty($property_key, $property['label'], $property['type'], $property['behavior']);
            }
          }
        }
        else {
          $entity_type->{$key} = $value;
        }
      }

      $entity_type->save();
    }
    eck_clean_up();
  }
}

/**
 * @} End of ECK Entity Types.
 */

/**
 * @{
 * ECK Bundles
 */

/**
 * Implements hook_features_export_options().
 */
function eck_bundle_features_export_options() {
  $bundles = array();
  foreach (Bundle::loadAll() as $bundle) {
    // @todo Ideally, all bundles should be accessible to features, but
    // we're currently just targeting those created through ECK.
    $entity_type = EntityType::loadByName($bundle->entity_type);
    $bundles[$bundle->machine_name] = "{$entity_type->label}:{$bundle->label}";
  }
  return $bundles;
}

/**
 * Implements hook_features_export().
 */
function eck_bundle_features_export($data, &$export, $module_name = '') {
  $pipe = array();
  // What is this for?
  $map = features_get_default_map('eck');

  foreach ($data as $type) {

    // Export the entity type.
    $export['features']['eck_bundle'][$type] = $type;
    $export['dependencies']['eck'] = 'eck';
    $export['dependencies']['features'] = 'features';

    // Export fields.
    // @TODO: Features already supports this, just gotta find the right calls.
    $bundle = Bundle::loadByMachineName($type);
    $fields = field_info_instances($bundle->entity_type, $bundle->name);
    foreach ($fields as $field) {
      $pipe['field'][] = "{$bundle->entity_type}-{$field['bundle']}-{$field['field_name']}";
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function eck_bundle_features_export_render($module, $data, $export = NULL) {
  $elements = array(
    'machine_name' => NULL,
    'entity_type' => NULL,
    'name' => NULL,
    'label' => NULL,
    'config' => NULL,
  );
  $output = array();
  $output[] = '  $items = array(';

  foreach ($data as $bundle_machine_name) {
    $bundle = Bundle::loadByMachineName($bundle_machine_name);
    unset($bundle->id);

    foreach ($elements as $key => $value) {
      $elements[$key] = $bundle->{$key};
    }

    $output[] = "    '{$bundle->machine_name}' => " . features_var_export($elements, '    ') . ",";
  }

  $output[] = '  );';

  $output[] = '  return $items;';

  return array('eck_bundle_info' => implode("\n", $output));
}

/**
 * Implements hook_features_revert().
 */
function eck_bundle_features_revert($module) {
  eck_bundle_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 *
 * Rebuilds eck entities from code defaults.
 */
function eck_bundle_features_rebuild($module) {
  if ($default_types = features_get_default('eck_bundle', $module)) {
    // Reset the bundles cache.
    Bundle::loadAll(TRUE);

    foreach ($default_types as $bundle_machine_name => $bundle_info) {

      $bundle = Bundle::loadByMachineName($bundle_machine_name);
      if (empty($bundle->id)) {
        $bundle = new Bundle();
      }
      foreach ($bundle_info as $key => $value) {
        $bundle->{$key} = $value;
      }
      $bundle->save();
    }
  }
}

/**
 * @} End of ECK Bundles
 */
