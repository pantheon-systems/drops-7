<?php
/**
 * @file
 * commerce_kickstart_social.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function commerce_kickstart_social_user_default_permissions() {
  $permissions = array();

  // Exported permission: administer oauth.
  $permissions['administer oauth'] = array(
    'name' => 'administer oauth',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'oauth_common',
  );

  // Exported permission: administer oauth connector.
  $permissions['administer oauth connector'] = array(
    'name' => 'administer oauth connector',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'oauthconnector',
  );

  // Exported permission: oauth authorize any consumers.
  $permissions['oauth authorize any consumers'] = array(
    'name' => 'oauth authorize any consumers',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'oauth_common',
  );

  // Exported permission: oauth register any consumers.
  $permissions['oauth register any consumers'] = array(
    'name' => 'oauth register any consumers',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'oauth_common',
  );

  return $permissions;
}
