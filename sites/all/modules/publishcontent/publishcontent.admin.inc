<?php

/**
 * @file
 * Contains page callbacks for publishcontent
 */

/**
 * Administration settings form.
 */
function publishcontent_config_form($form, &$form_state) {

  $form['publishcontent_method'] = array(
    '#type' => 'radios',
    '#title' => t('Quick publish method'),
    '#default_value' => variable_get('publishcontent_method', PUBLISHCONTENT_METHOD_TABS),
    '#description' => t('Choose the quick links method. With no quick links, the published checkbox will still appear on the node edit form. Note that a Drupal cache clear is required after changing this.'),
    '#options' => array(
      PUBLISHCONTENT_METHOD_NONE => t('None.'),
      PUBLISHCONTENT_METHOD_ACTION_LINKS => t('Action links on node view.'),
      PUBLISHCONTENT_METHOD_BUTTON => t('Button.'),
      PUBLISHCONTENT_METHOD_TABS => t('Tabs.'),
    ),
  );

  return system_settings_form($form);
}
