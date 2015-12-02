<?php

/**
 * @file
 * Provides some node attributes as ThemeKey properties.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */


/**
 * Implements hook_themekey_properties().
 *
 * Provides additional properties for the ThemeKey module:
 * - locale:language
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_locale_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['locale:language'] = array(
    'description' => t('Locale: Language - The code of the current site language, formatted like "en" or "de" or "neutral". See !link for the codes of your enabled languages',
      array('!link' => l(t('!path', array('!path' => 'admin/config/language')), 'admin/config/language'))),
    'validator' => 'themekey_validator_language',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  return array('attributes' => $attributes);
}


/**
 * Implements hook_themekey_paths().
 */
function themekey_locale_themekey_global() {
  global $language;

  $parameters = array();
  if (empty($language->language) || LANGUAGE_NONE == $language->language) {
    $parameters['locale:language'] = 'neutral';
  }
  else {
    $parameters['locale:language'] = $language->language;
  }

  return $parameters;
}
