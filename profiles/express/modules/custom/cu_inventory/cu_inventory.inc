<?php
/**
 * Create an array w/ profile version to send to the Inventory API
 *
 * $version int
 *
 * @return array
 *   $site_data: A partial PATCH to the site document in the Inventory.
 */
function cu_inventory_process_profile_version($version) {
  $site_data = array();
  $site_data['sid'] = variable_get('cu_sid', NULL);

  if (!$version) {
	// drupal_get_installed_schema_version doesn't appear at the
	// bootstrap level of an updated so query the value
    $version = drupal_get_installed_schema_version('express');
  }
  $site_data['statistics']['express_schema_version'] = (int)$version;

  // Ready to patch.
  cu_inventory_update_site($site_data);
}
