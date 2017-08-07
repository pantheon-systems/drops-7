<?php

/**
 * @file
 * Bean translation handler for the translation module.
 */

/**
 * Bean translation handler.
 *
 * Overrides default behaviours for Bean properties.
 */
class EntityTranslationBeanHandler extends EntityTranslationDefaultHandler {

  public function __construct($entity_type, $entity_info, $entity) {
    parent::__construct('bean', $entity_info, $entity);
  }

  public function removeTranslation($langcode) {
    $translations_key = $this->getTranslationsKey();
    if (empty($translations_key)) {
      return;
    }

    $hook_info = array('hook' => 'delete');
    if (!empty($langcode)) {
      unset($this->entity->{$translations_key}->data[$langcode]);
      // Keep track that the current translation has been removed.
      $this->entity->{$translations_key}->hook[$langcode] = $hook_info;
    }
    elseif (!empty($this->entity->{$translations_key}->data)) {
      $keys = array_keys($this->entity->{$translations_key}->data);
      $values = array_fill(0, count($keys), $hook_info);
      // Keep track that the all translations have been removed.
      $this->entity->{$translations_key}->hook = array_combine($keys, $values);
      // Actually remove translations.
      $this->entity->{$translations_key}->data = array();
    }

    // Remove field translations.
    foreach (field_info_instances($this->entityType, $this->bundle) as $instance) {
      $field_name = $instance['field_name'];
      $field = field_info_field($field_name);

      if ($field['translatable']) {
        if (!empty($langcode)) {
          $this->entity->{$field_name}[$langcode] = array();
        }
        else {
          $this->entity->{$field_name} = array();
        }
      }
    }

    // Clear the cache for this entity.
    entity_get_controller($this->entityType)->resetCache(array($this->getEntityBid()));
  }

  public function getAccess($op) {
    return entity_access($op, 'bean', $this->entity);
  }

  protected function getEntityId() {
    if (isset($this->entity->delta)) {
      return $this->entity->delta;
    }
    return parent::getEntityId();
  }

  protected function getEntityBid() {
    if (isset($this->entity->entityId)) {
      return $this->entity->entityId;
    }
    return parent::getEntityId();
  }

  /**
   * @inheritdoc
   */
  public function isNewEntity() {
    return empty($this->entityId);
  }

}
