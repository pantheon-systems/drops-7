<?php

/**
 * @file
 * Hooks provided by the Inline Entity Form module.
 */

/**
 * Perform alterations before an entity form is included in the IEF widget.
 *
 * @param $entity_form
 *   Nested array of form elements that comprise the entity form.
 * @param $form_state
 *   The form state of the parent form.
 */
function hook_inline_entity_form_entity_form_alter(&$entity_form, &$form_state) {
  if ($entity_form['#entity_type'] == 'commerce_line_item') {
    $entity_form['quantity']['#description'] = t('New quantity description.');
  }
}

/**
 * Perform alterations before the reference form is included in the IEF widget.
 *
 * The reference form is used to add existing entities through an autocomplete
 * field
 *
 * @param $reference_form
 *   Nested array of form elements that comprise the reference form.
 * @param $form_state
 *   The form state of the parent form.
 */
function hook_inline_entity_form_reference_form_alter(&$reference_form, &$form_state) {
  $reference_form['entity_id']['description'] = t('New autocomplete description');
}
