<?php

/**
 * @file
 * Provides documentation for the Service Links API.
 *
 * Look the online documentation for more info.
 */

/**
 * Obtains all available service links.
 *
 * @return
 *   An array containing all service links, keyed by name.
 */
function hook_service_links() {
  $links = array();

  $links['myservice'] = array(
    // The name of the service.
    'name' => 'MyService',
    // A short description for the link.
    'description' => t('Share this post on MyService'),
    // The service URL and its params.
    'link' => 'http://example.com/?url=<encoded-url>&title=<encoded-title>&summary=<encoded-teaser>',
    // The service icon. The id name and .png extension is used as default.
    'icon' => drupal_get_path('module', 'myservice') .'/myservice.png',
    // Any additional attributes to apply to the element.
    'attributes' => array(
      'class' => array('myservice-special-class'), // A special class.
      'style' => 'text-decoration: underline;', // Apply any special inline styles.
    ),
    // JavaScript to add when this link is processed, can be a string or an array.
    'javascript' => drupal_get_path('module', 'myservice') .'/myservice.js',
    // CSS to add when this link is processed, can be a string or an array.
    'css' => drupal_get_path('module', 'myservice') .'/myservice.css',
    // A PHP function invoked before the link is created, useful to add new tags.
    'preset' => 'myservice_preset',
    // A PHP function callback that is invoked when the link is created.
    'callback' => 'myservice_callback',
  );

  return $links;
}

/**
 * Example of preset function.
 *
 * @param $service
 *   The service that is being used.
 * @param $settings
 *   An array containing all the settings used.
 * @param $node
 *   An object containt the current node.
 */
function my_service_preset(&$service, &$settings, $node = NULL) {
  $settings['tags']['new-tag'] = '<new-tag>';
  $settings['subst']['new-tag'] = check_plain(variable_get('some_variable', 'default'));
}

/**
 * Example callback from the Service Links.
 *
 * @param $service
 *   The service that is being used.
 * @param $context
 *   An array containing all information about the item being shared.
 */
function myservice_callback($service, $context) {

}

/**
 * Allows alteration of the Service Links.
 *
 * @param $links
 *   The constructed array of service links.
 */
function hook_service_links_alter(&$links) {
  if (isset($links['myservice'])) {
    // Change the icon of MyService.
    $links['myservice']['icon'] = 'http://drupal.org/misc/favicon.ico';
  }
}
