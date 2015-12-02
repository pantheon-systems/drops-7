<?php

/**
 * @file
 * Provides some user attributes as ThemeKey properties.
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
 * Provides additional properties for module ThemeKey:
 * - user:hostname
 * - user:language
 * - user:name
 * - user:uid
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_user_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['user:hostname'] = array(
    'description' => t("User: Hostname - The user hostname property which is the IP address of client machine, adjusted for reverse proxy. Text from Drupal bootstrap.inc:
      If Drupal is behind a reverse proxy, we use the X-Forwarded-For header instead of \$_SERVER['REMOTE_ADDR'], which would be the IP address of the proxy server, and not the client's."),
    'validator' => 'themekey_validator_http_host',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );
  $attributes['user:language'] = array(
    'description' => t('User: Language - The language the user has set in the settings of his profile page.
      Format is the language code (for example "en" for English or "de" for German) that can be found on !link.',
      array('!link' => l(t('!path', array('!path' => 'admin/config/regional/language/overview')), 'admin/config/regional/language/overview'))),
    'validator' => 'themekey_validator_language',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['user:name'] = array(
    'description' => t('User: Name - The username of the user. See !link for your users.', array('!link' => l(t('!path', array('!path' => 'admin/people/people')), 'admin/people/people'))),
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['user:uid'] = array(
    'description' => t('User: ID - The id of the user (uid). The user id can be found in the URL of the users profile page, "user/23" or "user/23/edit" (23 = uid). See !link for your users.',
      array('!link' => l(t('!path', array('!path' => 'admin/people/people')), 'admin/people/people'))),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['user:role'] = array(
    'description' => t("User: Role - Defined user roles (examples: 'anonymous user', 'authenticated user')"),
    'validator' => 'themekey_validator_role',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  return array('attributes' => $attributes);
}


/**
 * Implements hook_themekey_global().
 */
function themekey_user_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['user:hostname'] = !empty($user->hostname) ? $user->hostname : NULL;
  $parameters['user:language'] = !empty($user->language) ? $user->language : NULL;
  $parameters['user:name'] = !empty($user->name) ? $user->name : NULL;
  $parameters['user:uid'] = $user->uid;
  $parameters['user:role'] = $user->roles;

  return $parameters;
}
