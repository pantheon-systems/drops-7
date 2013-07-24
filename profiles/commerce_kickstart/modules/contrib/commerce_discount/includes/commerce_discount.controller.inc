<?php

/**
 * @file
 * The controller for the discount entity containing the CRUD operations.
 */

/**
 * Controller for discount entities; Overrides CRUD operations.
 */
class CommerceDiscountControllerExportable extends EntityAPIControllerExportable {

  /**
   * Override export, to allow embedding the commerce discount offer
   * inside the export of the comemrce discount.
   */
  public function export($entity, $prefix = '') {
    $vars = get_object_vars($entity);

    // Add commerce_discout_offer full entity export.
    $wrapper = entity_metadata_wrapper('commerce_discount', $entity);
    $discount_offer = $wrapper->commerce_discount_offer->value();
    unset($discount_offer->discount_offer_id);
    $vars['commerce_discount_offer'] = get_object_vars($discount_offer);

    unset($vars[$this->statusKey], $vars[$this->moduleKey], $vars['is_new']);
    if ($this->nameKey != $this->idKey) {
      unset($vars[$this->idKey]);
    }
    return entity_var_json_export($vars, $prefix);
  }

  /**
   * Imports an entity from a string.
   *
   * @param string $export
   *   An exported entity as serialized string.
   *
   * @return
   *   An entity object not yet saved.
   */
  public function import($export) {
    $entity = parent::import($export);

    $discount_offer = $entity->commerce_discount_offer;
    unset($entity->commerce_discount_offer);
    $entity->commerce_discount_offer = array('commerce_discount_fields' => $discount_offer);

    return $entity;
  }

  /**
   * Overridden to handle embedded dicount_offer update.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // If overwriting an existing discount, delete the referenced
    // discount_offer.
    $wrapper = entity_metadata_wrapper('commerce_discount', $entity);

    if (!empty($entity->commerce_discount_offer['commerce_discount_fields'])) {
      $wrapper->commerce_discount_offer->delete();

      $discount_offer = entity_create('commerce_discount_offer', $entity->commerce_discount_offer['commerce_discount_fields']);
      $discount_offer->save();
      $wrapper->commerce_discount_offer->set($discount_offer);
    }

    return parent::save($entity, $transaction);
  }

  /**
   * Overridden to handle deletion of rules when a discount is deleted.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    parent::delete($ids, $transaction);
    // Rebuild entities.
    entity_defaults_rebuild(array('rules_config'));
  }
}
