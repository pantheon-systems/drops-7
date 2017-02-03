<?php

/**
 * @file
 * Defines the inline entity form controller for ECK Entities.
 */

class EckInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * Overrides EntityInlineEntityFormController::labels().
   */
  public function labels() {
    $entity_info = entity_get_info($this->entityType);
    $labels = array(
      'singular' => $entity_info['label'],
      'plural' => $entity_info['label'],
    );
    // The admin has specified the exact labels that should be used.
    if ($this->settings['override_labels']) {
      $labels = array(
        'singular' => $this->settings['label_singular'],
        'plural' => $this->settings['label_plural'],
      );
    }
    return $labels;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   */
  public function entityForm($entity_form, &$form_state) {
    $entity_form = array_merge($entity_form, eck__entity__form($entity_form, $form_state, $entity_form['#entity']));
    unset($entity_form['submit']);
    return $entity_form;
  }
}
