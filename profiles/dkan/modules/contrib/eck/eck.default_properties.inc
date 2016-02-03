<?php
/**
 * @file
 * Default properties are a grouping of a property and a behavior.
 */

/**
 * Default properties form callback.
 */
function eck__default_properties__form($form, &$state, $entity_type) {
  // Now we want to display the default properties.
  $default_properties = eck_get_default_properties();
  $options = array();
  foreach ($default_properties as $property_name => $property_info) {
    $options[$property_name] = $property_info['label'];
  }

  $form['default_properties'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => t('Default Properties'),
  );

  $properties = $entity_type->properties;

  $defaults = array();
  foreach ($properties as $property => $info) {
    $defaults[$property] = $property;
  }

  $form['default_properties']['#default_value'] = $defaults;
  // :S

  return $form;
}

/**
 * Form submition callback for default properties.
 */
function eck__default_properties__form_submit($form, &$state, $entity_type) {

  $entity_type = $state['values']['entity_type'];
  $dp = $state['values']['default_properties'];
  foreach ($dp as $property => $active) {
    if ($active) {
      $info = eck_get_default_property($property);
      $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
    }
    else {
      $entity_type->removeProperty($property);
    }
  }

  $state['values']['entity_type'] = $entity_type;
}

/**
 * All default properties information.
 * @todo combine this and get_default_property().
 * 
 * @return array
 *   All of the default properties info.
 */
function eck_get_default_properties() {
  $default_properties = module_invoke_all('eck_default_properties');
  drupal_alter('eck_default_properties', $default_properties);

  return $default_properties;
}

/**
 * Get a default property by name.
 * 
 * @param string $name
 *   Default property's name.
 * 
 * @return array
 *   The default property info.
 */
function eck_get_default_property($name) {
  $default_properties = eck_get_default_properties();
  return $default_properties[$name];
}
