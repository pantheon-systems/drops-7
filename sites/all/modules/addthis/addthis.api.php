<?php
/**
 * @file
 * This is currently a stub file that will be used to describe the addthis
 * implementation API.
 */

/**
 * Implements hook_TYPE_alter().
 *
 * @param array $options
 *   $options contains an array with configurations settings for used in the
 *   creation of the markup. The following elements may be in here.
 *
 *   - '#entity_type': The entity type this markup is define when called by a
 *                     field.
 *   - '#entity': Is the entity object when called by a field.
 *   - '#display': Is always defined and provide all the formatter
 *                 configuration.
 *   - '#url': The link to the entity when the entity has a url.
 */
function hook_addthis_markup_options_alter(&$options) {
  global $base_root;

  // Change the url used on the share buttons.
  $options['#url'] = $base_root . request_uri();

  // To apply different service this to the block implementation try this.
  if (isset($options['#block']) && $options['#display']['type'] == 'addthis_basic_toolbox') {

    // Change the var below to add other services.
    $displayed_services = 'twitter,google_plusone,facebook';
    $options['#display']['settings']['share_services'] = $displayed_services;
    $options['#display']['settings']['buttons_size'] = AddThis::CSS_16x16;

  }
}

/**
 * Implements hook_TYPE_alter().
 *
 * @param array $markup
 *   $markup contains an array with the structure of the addthis markup.
 */
function hook_addthis_markup_alter(&$markup) {

  // Let's add a custom CSS class for given a particular design to our
  // twitter button, so we can change the look.
  if (!empty($markup['twitter'])) {
    $markup['twitter']['#attributes']['class'][] = "custom_twitter_class";
  }

  // Or change button size for Google +1 for example.
  if (!empty($markup['google_plusone'])) {
    $markup['google_plusone']['#attributes']['g:plusone:size'] = 'small';
  }
}
