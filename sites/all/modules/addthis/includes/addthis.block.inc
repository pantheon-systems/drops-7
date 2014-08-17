<?php

/**
 * @file
 * Block related hook implementations for the AddThis-module.
 */

/**
 * Implements hook_block_info().
 */
function addthis_block_info() {
  $block_info = array();
  $block_info[AddThis::BLOCK_NAME] = array(
    'info' => t('AddThis'),
    'cache' => DRUPAL_NO_CACHE,
  );
  return $block_info;
}

/**
 * Implements hook_block_view().
 *
 * #block key allows alter hooks to react on this information. Someone might
 * want to change this specific display.
 */
function addthis_block_view($block_name = '') {

  if ($block_name == AddThis::BLOCK_NAME) {

    $widget_type = AddThis::getInstance()->getBlockDisplayType();
    $widget_settings = AddThis::getInstance()->getBlockDisplaySettings();

    $block_options = array(
      '#block' => AddThis::BLOCK_NAME,
      '#display' => array(
        'settings' => $widget_settings,
      )
    );

    $markup = AddThis::getInstance()->getDisplayMarkup($widget_type, $block_options);
    return array(
      'subject' => '',
      'content' => $markup,
    );
  }
}

/**
 * Implements hook_block_configure().
 */
function addthis_block_configure($delta = '') {
  AddThis::getInstance()->includeWidgetJs();

  return array();
}

/**
 * Implements hook_block_save().
 */
function addthis_block_save($delta = '', $edit = array()) {
  variable_set(AddThis::BLOCK_WIDGET_TYPE_KEY, $edit['addthis_settings']['type']);
  variable_set(AddThis::BLOCK_WIDGET_SETTINGS_KEY, $edit['addthis_settings']['settings']);
}

/**
 * Callback submit.
 */
function _addthis_settings_form_block_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Callback to return ajax replace part.
 */
function _addthis_settings_form_block_submit_callback($form, &$form_state) {
  return $form['settings']['addthis_settings']['form'];
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function addthis_form_block_admin_configure_alter(&$form, &$form_state) {

  if ($form['module']['#value'] == 'addthis' && $form['delta']['#value'] == 'addthis_block') {
    form_load_include($form_state, 'inc', 'addthis', 'addthis.block');
    $form['#cache'] = TRUE;

    $form['settings']['addthis_settings'] = array(
      '#type' => 'fieldset',
      '#title' => 'Display settings',
    );

    // Retrieve settings.
    $addthis_settings['type'] = AddThis::getInstance()->getBlockDisplayType();
    $addthis_settings['settings'] = AddThis::getInstance()->getBlockDisplaySettings();

    // Create a addthis settings form based on the available configuration.
    $element = _addthis_settings_form(
      isset($form['addthis_settings']['form']) ? $form['addthis_settings']['form'] : array(),
      $form_state,
      isset($addthis_settings['type']) ? $addthis_settings['type'] : NULL,
      isset($addthis_settings['settings']) ? $addthis_settings['settings'] : NULL
    );

    // Change the submit and callback because our handling is different and the
    // form structure is different form the default implementation.
    $element['type']['#submit'] = array('_addthis_settings_form_block_submit');
    $element['type']['#ajax']['callback'] = '_addthis_settings_form_block_submit_callback';
    $form['settings']['addthis_settings']['form'] = $element;

    array_unshift($form['#submit'], '_addthis_settings_form_block_submit');
  }

  return $form;
}
