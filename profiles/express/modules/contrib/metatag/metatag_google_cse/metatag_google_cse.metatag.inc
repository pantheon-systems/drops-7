<?php
/**
 * @file
 * Metatag integration for the Metatag:Google CSE.
 */

/**
 * Implements hook_metatag_info().
 */
function metatag_google_cse_metatag_info() {
  $info['groups']['google_cse'] = array(
    'label' => t('Google Custom Search Engine (CSE)'),
    'description' => t("Meta tags used to control the mobile browser experience. Some of these meta tags have been replaced by newer mobile browsers. These meta tags usually only need to be set globally, rather than per-page."),
    'form' => array(
      '#weight' => 80,
    ),
  );

  $weight = 80;

  // Default values for each meta tag.
  $tag_info_defaults = array(
    'class' => 'DrupalTextMetaTag',
    'group' => 'google_cse',
    'element' => array(
      '#theme' => 'metatag_google_cse',
    ),
  );

  $info['tags']['thumbnail'] = array(
    'label' => t('Thumbnail'),
    'description' => t('Use a url of a valid image.'),
    'image' => TRUE,
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['department'] = array(
    'label' => t('Department'),
    'description' => t('Department tag.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['audience'] = array(
    'label' => t('Content audience'),
    'description' => t('The content audience, e.g. "all".'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['doc_status'] = array(
    'label' => t('Document status'),
    'description' => t('The document status, e.g. "draft".'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['google_rating'] = array(
    'label' => t('Results sorting'),
    'description' => t('Works only with numeric values.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  return $info;
}
