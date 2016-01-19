<?php

/**
 * @file
 * Provides Entity metadata integration.
 */

/**
 * Extend the defaults meta data properties for Commerce discount entity.
 */
class CommerceDiscountMetadataController extends EntityDefaultMetadataController {

  /**
   * {@inheritdoc}
   */
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    // Update certain elements of the property info array.
    $properties['discount_id']['label'] = t('Discount ID');
    $properties['discount_id']['description'] = t('The serial numeric ID of the discount.');

    $properties['name']['label'] = t('Machine name');
    $properties['name']['description'] = t('The text machine name of the discount.');

    $properties['label']['label'] = t('Admin title');
    $properties['label']['description'] = t('The label used for the discount on administrative screens.');

    $properties['type']['description'] = t('The type of the discount (e.g. Order vs. Product).');

    $properties['status']['type'] = 'boolean';
    $properties['status']['description'] = t('The status of the discount (i.e. Active vs. Disabled).');

    $properties['export_status']['label'] = t('Export status');
    $properties['export_status']['description'] = t('The exportable status of the discount.');

    $properties['module']['description'] = t('The name of the module that defined the discount if applicable.');

    $properties['component_title']['label'] = t('Name');
    $properties['component_title']['description'] = t('The name of the discount as shown to customers (e.g. in the price component list).');

    $properties['sort_order']['label'] = t('Sort order');
    $properties['sort_order']['description'] = t('The default sort order of the rule created for the discount.');

    return $info;
  }

}
