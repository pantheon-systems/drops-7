<?php

/**
 * Implements hook_linkit_plugin_entities_alter().
 *
 * The default behavior for entities is that they will use the default entity
 * search plugin class, which will provide them with the basic methods they
 * need.
 *
 * Tho there can be some search plugins that will extend those basic method with
 * more advanced once, therefore the handlers for those plugins will have to be
 * changed.
 *
 * Make sure that your classes is included in the regisrty.
 * The easiest way to do this is by define them as
 *
 * <code> files[] = plugins/linkit_search/my_custom_plugin.class.php </code>
 *
 * @param $plugins
 *   An array of all search plugins processed within Linkit entity plugin.
 */
function hook_linkit_search_plugin_entities_alter(&$plugins) {
  $path = drupal_get_path('module', 'mymodule') . '/plugins/linkit_search';
  if (isset($plugins['my_custom_plugin'])) {
    $handler = array(
      'class' => 'MyCustomPlugin',
      'file' => 'my_custom_plugin.class.php',
      'path' => $path,
    );
    $plugins['my_custom_plugin']['handler'] = $handler;
  }
}

/**
 * Implements hook_linkit_local_hosts_alter().
 *
 * The default behavior is that only the current host is considered "local",
 * when deciding how to classify a URL. For example, if the user is on
 * http://www.example.com, then all URLs to other hosts will not be considered
 * for local URLs. This means that if your users edit content on a different host
 * from the actual public-facing site, such as https://staging.example.com,
 * then if they paste in URLs from the public site (www), none of those
 * URLs will be considered local.
 *
 * Implementing this hook will allow you to alter the list (indexed array) of
 * hosts that will be considered for internal links. Include the protocol
 * (eg, http or https).
 */
function hook_linkit_local_hosts_alter(&$local_hosts) {
  $local_hosts[] = 'http://www.example.com';
}
