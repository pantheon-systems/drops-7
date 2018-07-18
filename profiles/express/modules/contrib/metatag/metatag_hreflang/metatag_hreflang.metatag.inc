<?php

/**
 * @file
 * Metatag integration for the Metatag:hreflang module.
 */

/**
 * Implements hook_metatag_bundled_config_alter().
 *
 * This provides recommended defaults that should be sufficient for most sites.
 */
function metatag_hreflang_metatag_bundled_config_alter(array &$configs) {
  // This only makes sense if either the Translation or Entity Translation
  // modules are enabled.
  if (!(module_exists('translation') || module_exists('entity_translation'))) {
    return;
  }

  foreach ($configs as &$config) {
    switch ($config->instance) {
      case 'node':
        // The x-default should default to the source language.
        $config->config += array(
          'hreflang_xdefault' => array('value' => '[node:url-original]'),
        );

        // Add all of the other hreflang values.
        $languages = language_list('enabled');
        if (!empty($languages[1])) {
          foreach (array_keys($languages[1]) as $langcode) {
            $config->config += array(
              'hreflang_' . $langcode => array('value' => '[node:url-' . $langcode . ']'),
            );
          }
        }
        break;
    }
  }
}

/**
 * Implements hook_metatag_info().
 */
function metatag_hreflang_metatag_info() {
  $info['groups']['hreflang'] = array(
    'label' => t('Alternative language links (hreflang)'),
    'description' => t('These meta tags are designed to point visitors to versions of the current page in other languages. It is recommended to use the default "[node:url-LANGCODE]" tokens for the hreflang values, they will only be output if a translation exists for that locale. Also, it is suggested to use the "[node:source:url]" token for the default locale.'),
    'form' => array(
      '#weight' => 60,
    ),
  );

  $weight = 100;

  // Default values for each meta tag.
  $tag_info_defaults = array(
    'description' => '',
    'class' => 'DrupalLinkMetaTag',
    'group' => 'hreflang',
  );

  $info['tags']['hreflang_xdefault'] = array(
    'label' => t('Default locale (x-default)'),
    'description' => t('This should point to the version of the page that is for the main or primary locale, e.g. the original version of an article that is translated into other languages.'),
    'weight' => $weight,
    'element' => array(
      '#theme' => 'metatag_link_hreflang',
      '#hreflang' => 'x-default',
    ),
  ) + $tag_info_defaults;
  if (variable_get('metatag_hreflang_allow_dupe', FALSE)) {
    $info['tags']['hreflang_xdefault']['description'] .= ' ' . t('If this string matches one of the values below the other value will not be removed.');
  }
  else {
    $info['tags']['hreflang_xdefault']['description'] .= ' ' . t('If this string matches one of the values below that other tag will be removed, thus ensuring that there is only one hreflang meta tag for each possible variation of this page.');
  }

  // Add a meta tag for each locale enabled.
  $languages = language_list('enabled');
  if (!empty($languages[1])) {
    foreach ($languages[1] as $langcode => $locale) {
      // Getting granular with these so they're all grouped together.
      $weight += 0.01;
      $info['tags']['hreflang_' . $langcode] = array(
        'label' => t('hreflang value for :language (:native)', array(':language' => t($locale->name), ':native' => $locale->native)),
        'weight' => $weight,
        'element' => array(
          '#theme' => 'metatag_link_hreflang',
          '#hreflang' => $langcode,
        ),
      ) + $tag_info_defaults;
    }
  }

  return $info;
}
