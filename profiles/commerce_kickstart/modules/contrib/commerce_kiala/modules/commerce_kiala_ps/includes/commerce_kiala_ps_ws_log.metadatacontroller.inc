<?php

/**
 * @file
 * The metadata controller for the Web service log entity type.
 */

/**
 * Default controller for generating some basic metadata for CRUD entity types.
 */
class CommerceKialaPSWSLogMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    if (!isset($info[$this->type])) {
      return $info;
    }

    $props = &$info[$this->type]['properties'];

    // Status: add options
    if (isset($props['status'])) {
      $props['status']['options list'] = 'commerce_kiala_ps_ws_log_status_options_list';
    }

    // Add computed fields
    $props['order'] = array(
      'label' => t('Order', array(), array('context' => 'a drupal commerce order')),
      'type' => 'commerce_order',
      'description' => t("The order the Kiala web service log belongs to."),
      'getter callback' => 'entity_property_verbatim_get',
      'computed' => TRUE,
      'clear' => array('order_id'),
    );

    return $info;
  }
}
