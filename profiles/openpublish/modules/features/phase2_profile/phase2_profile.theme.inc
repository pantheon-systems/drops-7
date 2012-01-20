<?php
/**
 * Implementation of hook_form_FORM_ID_alter().
 * Used to add some javascript to the node edit form to manipulate the page while editing.
 */
function phase2_profile_form_profile_node_form_alter(&$form, $form_state) {
  drupal_add_js(drupal_get_path('module', 'phase2_profile') . '/theme/phase2_profile.js');
  drupal_add_css(drupal_get_path('module', 'phase2_profile') . '/theme/phase2_profile-node-form.css');
}








