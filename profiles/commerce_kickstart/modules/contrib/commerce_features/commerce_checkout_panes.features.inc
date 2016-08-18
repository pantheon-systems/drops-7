<?php
/**
 * @file
 * Integration of Commerce Checkout with Features.
 */

define('COMMERCE_CHECKOUT_PANES_INTEGRATION_NAME', 'commerce_checkout_panes');
define('COMMERCE_CHECKOUT_PANES_DEFAULT_HOOK', 'commerce_checkout_panes_default');

/**
 * Implements hook_features_export_options().
 */
function commerce_checkout_panes_features_export_options() {
  $options = array();

  foreach (commerce_checkout_panes() as $pane_id => $pane) {
    $options[$pane_id] = sprintf('%s (%s)', $pane['title'], $pane['name']);
  }

  return $options;
}

/**
 * Implements hook_features_export().
 */
function commerce_checkout_panes_features_export(array $data, array &$export, $module) {
  $panes = commerce_checkout_panes();

  foreach ($data as $pane_id) {
    $export['features'][COMMERCE_CHECKOUT_PANES_INTEGRATION_NAME][$pane_id] = $pane_id;
    $export['dependencies'][] = $panes[$pane_id]['module'];
  }

  $export['dependencies'][] = 'commerce_checkout';

  return array();
}

/**
 * Implements hook_features_export_render().
 */
function commerce_checkout_panes_features_export_render($module, array $data, $export = NULL) {
  // Need to reset static cache, otherwise Features will not be able to check
  // a state of components properly.
  commerce_checkout_panes_reset();

  $schema = drupal_get_schema('commerce_checkout_pane');
  $fields = array_fill_keys(array_keys($schema['fields']), NULL);
  $panes = commerce_checkout_panes();
  $code = array();

  $code[] = '  $panes = array();';

  foreach ($data as $pane_id) {
    if (isset($panes[$pane_id])) {
      $code[] = '';
      $code[] = sprintf('  $panes[%s] = %s;', "'$pane_id'", features_var_export(array_intersect_key($panes[$pane_id], $fields), '  '));
    }
  }

  $code[] = '';
  $code[] = '  return $panes;';

  return array(COMMERCE_CHECKOUT_PANES_DEFAULT_HOOK => implode("\n", $code));
}

/**
 * Implements hook_features_revert().
 */
function commerce_checkout_panes_features_revert($module) {
  $defaults = module_invoke($module, COMMERCE_CHECKOUT_PANES_DEFAULT_HOOK);

  if (!empty($defaults)) {
    commerce_checkout_panes_reset();
    $existing = commerce_checkout_panes();

    foreach ($defaults as $pane_id => $pane) {
      $pane['saved'] = isset($existing[$pane_id]['saved']) ? $existing[$pane_id]['saved'] : FALSE;

      commerce_checkout_pane_save($pane);
    }
  }
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_checkout_panes_features_rebuild($module) {
  commerce_checkout_panes_features_revert($module);
}

/**
 * Implements hook_features_disable_feature().
 */
function commerce_checkout_panes_features_disable_feature($module) {
  $defaults = module_invoke($module, COMMERCE_CHECKOUT_PANES_DEFAULT_HOOK);

  if (!empty($defaults)) {
    foreach ($defaults as $pane_id => $pane) {
      commerce_checkout_pane_reset($pane_id);
    }
  }
}
