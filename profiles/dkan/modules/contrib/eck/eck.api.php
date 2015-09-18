<?php
/**
 * @file
 * ECK's API documentation.
 */


/**
 * Respond to the creation of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being created.
 */
function hook_eck_entity_type_insert(EntityType $entity_type) {

}

/**
 * Respond to the updating of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being update.
 */
function hook_eck_entity_type_update(EntityType $entity_type) {

}

/**
 * Respond to the deletion of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being deleted.
 */
function hook_eck_entity_type_delete(EntityType $entity_type) {

}

/**
 * Defines default properties.
 *
 * A default property shows up in the property select list when a user is
 * first creating an entity type. These are meant to be commonly use properties
 * that we don't want to configure constantly. There is nothing special about
 * default properties, they are just meant to save time.
 *
 * There is also an ALTER version of this hook.
 */
function hook_eck_default_properties() {
  $default_properties = array();

  $default_properties['machine_name'] = array(
    'label' => "My Default Property",
    // @see eck_property_types().
    'type' => "text",
    // To find all of the behaviors that are available, you can use
    // ctools_get_plugins('eck', 'property_behavior');
    // or look at the interface under "manage properties"
    'behavior' => 'some_behavior',
  );
}

/**
 * Change an entity's label dynamically.
 *
 * More constrained versions of this hook also exist:
 * hook_eck_entity_<entity_type>_label
 * hook_eck_entity_<entity_type>_<bundle>_label
 *
 * This hook is mainly useful for dynamic labels, or for using values
 * in a field as labels.
 *
 * If you are storing the label of the entity in a property already, you
 * should modify the entity_info array's label key, instead of using this hook.
 *
 * @param Entity $entity
 *   The entity object.
 * @param int $entity_id
 *   The id of the entity.
 *
 * @return mixed
 *   The label for the entity.
 */
function hook_eck_entity_label($entity, $entity_id) {
  return "Somethins that should be the label for this entity";
}

/**
 * Define new property types.
 *
 * This hook is useless without also using
 * hook_eck_property_type_schema_alter().
 *
 * @return array
 *   An array with a machine name and a label for a new property type.
 */
function hook_eck_property_types() {
  return array("email" => t("Email"));
}

/**
 * Give the schema for your custom properties.
 *
 * @param array $schema
 *   A schema array.
 * @param string $type
 *   The property type.
 */
function hook_eck_property_type_schema_alter(&$schema, $type) {
  if ($type == 'email') {
    $schema = array(
      'description' => 'An email',
      'type' => 'varchar',
      'length' => 256,
      'not null' => TRUE,
      'default' => '',
    );
  }
}
