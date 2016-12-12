<?php

function field_reference_delete_drush_command() {
  $commands = array(
    'delete-reference' => array(
      'description' => 'Immediately removes references to a deleted entity from fields stored in an SQL database.',
      'callback' => 'drush_field_reference_delete_references',
      'aliases' => array('dref'),
      'options' => array(
        'type' => 'Entity type.',
        'bundle' => 'Entity bundle.'
      )
    )
  );

  return $commands;
}

function drush_field_reference_delete_references() {
  $type = drush_get_option('type', NULL);

  if (!$type) {
    return drush_set_error(dt('You must specify a type of entity (ex: node, user).'));
  }

  $entities = entity_load($type);
  foreach ($entities as $entity) {
    field_reference_delete_references($entity, $type);
  }
}