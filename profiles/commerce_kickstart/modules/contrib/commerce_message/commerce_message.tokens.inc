<?php

/**
 * @file
 * Token integration for the Commerce order backoffice module.
 */
/**
 * Implements hook_token_info().
 */
function commerce_message_token_info() {
  $info['tokens']['commerce-order']['site-logo'] = array(
    'name' => t('Site logo'),
    'description' => t('The logo of the site.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function commerce_message_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  foreach ($tokens as $name => $original) {
    if ($name == 'site-logo') {
      $replacements[$original] = theme('image', array('path' => theme_get_setting('logo', variable_get('theme_default')), 'alt' => variable_get('site_name'), 'title' => variable_get('site_name')));
    }
  }

  return $replacements;
}
