<?php
/**
 * @file
 * Define Linkit target attribute plugin.
 */

$plugin = array(
  'name' => t('Target'),
  'callback' => 'linkit_attribute_plugin_target',
);

/**
 * Create a FAPI element.
 *
 * @param array $plugin
 *   This is the plugin definition.
 *
 * @param array $settings
 *   An array of settings for this attribute. This is from the profile settings.
 *
 * @return
 *   An FAPI element.
 *
 * @see LinkitProfile::setAttributes()
 */
function linkit_attribute_plugin_target($plugin, $settings = array()) {
  return array(
    '#type' => 'select',
    '#title' => $plugin['name'],
    '#options' => array(
      '' => '',
      '_blank' => t('New window (_blank)'),
      '_top' => t('Top window (_top)'),
      '_self' => t('Same window (_self)'),
      '_parent' => t('Parent window (_parent)')
    ),
    '#weight' => isset($settings['weight']) ? $settings['weight'] : 0,
  );
}
