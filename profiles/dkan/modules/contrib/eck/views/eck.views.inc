<?php
/**
 * @file
 * Provide additional Views fields for entity content.
 */

/**
 * Implements hook_views_data_alter().
 */
function eck_views_data_alter(&$data) {
  foreach (EntityType::loadAll() as $entity_type) {
    $data["eck_{$entity_type->name}"]['view_link'] = array(
      'field' => array(
        'title' => t('Link'),
        'help' => t('Provide a simple link to the %entity_label content.', array('%entity_label' => $entity_type->label)),
        'handler' => 'eck_views_handler_field_link',
      ),
    );
    $data["eck_{$entity_type->name}"]['edit_link'] = array(
      'field' => array(
        'title' => t('Edit link'),
        'help' => t('Provide a simple link to edit the %entity_label content.', array('%entity_label' => $entity_type->label)),
        'handler' => 'eck_views_handler_field_link_edit',
      ),
    );
    $data["eck_{$entity_type->name}"]['delete_link'] = array(
      'field' => array(
        'title' => t('Delete link'),
        'help' => t('Provide a simple link to delete the %entity_label content.', array('%entity_label' => $entity_type->label)),
        'handler' => 'eck_views_handler_field_link_delete',
      ),
    );
    $vars = eck_property_behavior_invoke_plugin_alter($entity_type, 'views_data_alter', array('entity_type' => $entity_type, 'data' => $data));
    $data = $vars['data'];
  }
}
