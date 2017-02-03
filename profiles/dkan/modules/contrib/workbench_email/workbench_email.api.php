<?php

/**
 * @file
 * API documentation file for Workbench Email.
 */

/**
 * Allows modules to alter the form select list.
 *
 * If you have some custom logic that falls outside the bounds of
 * the configurable interface, you can alter the form element as
 * needed.
 *
 * hook_form_alter() does not provide a developer with access on
 * ajax events. So this drupal_alter() is provided to gain access
 * to the form element during those ajax events. #kapooya #whanger.
 *
 * @param array $form
 *   The form array.
 * @param object $email_transition
 *   The email transition object. Example, draft to needs review.
 * @param array $user_groups
 *   An associative array of editors and users.
 */
function hook_workbench_email_create_form_element_alter(&$form, $email_transition, $user_groups) {
  // $user_groups['editors'] => The workbench access editors. Could be empty.
  // $user_groups['users'] => All users available under specified role. When
  // the rid is 0, it represents the author.
  if ($user_groups['editors'] && $email_transition->from_name == 'draft') {
    $form['example_element']['#default_value'] = '';
  }
}
