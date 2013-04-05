<?php

/**
 * @file
 * Hooks provided by the Commerce Kiala module.
 */

/**
 * Defines the Shipping method settings info / metadata
 *
 * An admin form is provided to alter the settings provided by the Commerce
 * Kiala modules.
 *
 * If other modules define new settings ...
 * - To add to admin form, implement hook_form_commerce_kiala_settings_form_alter()
 *   and define the element keys the same as the settings key.  This allows the
 *   settings to be saved automatically by Commerce Kiala.
 *   @see commerce_kiala_track_form_commerce_kiala_settings_form_alter()
 * - If an admin for is not needed, then the default defined for the setting
 *   will always be used as the setting's value.
 *
 * The settings info array structure is as follows:
 * - group: The group name. This can be used to filter settings for a group.
 * - default: The default value used if the setting has not been overridden.
 * - sensitive: If set to TRUE, then this setting is treated as sensitive. The
 *   value will be encrypted and decrypted as needed.
 * - password: If set to TRUE, then this setting is treated as a password and
 *   sensitive. The value will be encrypted and decrypted as needed.
 *
 * @return
 *   An array of settings info arrays keyed by name.
 */
function hook_commerce_kiala_settings_info() {
  return array(
    'my_module_help_text' => array(
      'group' => 'my_module',
      'default' => t('Some description'),
    ),
    'my_module_sensitive_id' => array(
      'group' => 'my_module',
      'default' => '',
      'sensitive' => TRUE,
    ),
    'my_module_sensitive_pwd' => array(
      'group' => 'my_module',
      'default' => '',
      'sensitive' => TRUE,
      'password' => TRUE,
    ),
  );
}

/**
 * Allows modules to alter the Shipping method settings definitions.
 *
 * @param $info
 *   An array of settings info defined by enabled modules.
 *
 * @see hook_commerce_kiala_settings_info()
 */
function hook_commerce_kiala_settings_info_alter(&$info) {
  $states['my_module_help_text']['default'] = t('A better default help text');
}

// -----------------------------------------------------------------------
// Shipping Service CRUD Hooks

/**
 * Shipping Service Insert
 *
 * @param $shipping_service
 *   The shipping service to save. If the service array includes the
 *   base_rate array, its amount and currency_code values will be moved up a
 *   level to be saved to the database via drupal_write_record().
 * @param $skip_reset
 *   Boolean indicating whether or not this save should result in shipping
 *   services being reset and the menu being rebuilt; defaults to FALSE. This is
 *   useful when you intend to perform many saves at once, as menu rebuilding is
 *   very costly in terms of performance.
 */
function hook_commerce_kiala_service_insert($shipping_service, $skip_reset) {
  // perform custom operations reacting to the creation of a new shipping service
}

/**
 * Shipping Service Update
 *
 * @param $shipping_service
 *   @see hook_commerce_kiala_service_insert()
 * @param $skip_reset
 *   @see hook_commerce_kiala_service_insert()
 */
function hook_commerce_kiala_service_update($shipping_service, $skip_reset) {
  // perform custom operations reacting to the update of an existing
  // shipping service
}

/**
 * Shipping Service Delete
 *
 * @param $shipping_service
 *   @see hook_commerce_kiala_service_insert()
 * @param $skip_reset
 *   @see hook_commerce_kiala_service_insert()
 */
function hook_commerce_kiala_service_delete($shipping_service, $skip_reset) {
  // perform custom operations reacting to the deletion of an existing
  // shipping service
}


// -----------------------------------------------------------------------
// Kiala Line Item Point CRUD Hooks

/**
 * Line Item Point Insert
 *
 * @param $record
 *   The line item point object:
 *    - line_item_id
 *    - point_id
 */
function hook_commerce_kiala_line_item_point_insert($record) {
  // perform operations reacting to the creation of a new line item point
}

/**
 * Line Item Point Update
 *
 * @param $record
 *   The line item point object:
 *    - line_item_id
 *    - point_id
  * @param $record_original
 *   The original unchanged line item point.
 */
function hook_commerce_kiala_line_item_point_update($record, $record_original) {
  // perform operations reacting to the update of an existing line item point
}

/**
 * Line Item Point Delete
 *
 * This hook is called before the record is removed from the database table.
 *
 * @param $record
 *   The line item point object:
 *    - line_item_id
 *    - point_id
 */
function hook_commerce_kiala_line_item_point_delete($record) {
  // perform operations reacting to the deletion of a line item point
}
