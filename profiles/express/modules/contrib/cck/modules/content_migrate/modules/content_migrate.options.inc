<?php
/**
 * @file content_migrate.options.inc
 * Code to implement Content Migrate hooks on behalf of the Options module.
 */

/**
 * Implements hook_content_migrate_instance_alter().
 * 
 * Use this to tweak the conversion of instance or widget settings
 * from the D6 style to the D7 style for specific
 * situations not handled by basic conversion, as when
 * formatter or widget names or settings are changed.
 */
function options_content_migrate_instance_alter(&$instance_value, $field_value) {
  switch ($instance_value['widget']['module']) {
    // Optionswidgets module became Options module
    // and widget type names changed.
    case 'optionwidgets':
      $replace = array(
        'optionwidgets_select' => 'options_select',
        'optionwidgets_buttons' => 'options_buttons',
        'optionwidgets_onoff' => 'options_onoff',
      );
      $instance_value['widget']['module'] = 'options';
      $instance_value['widget']['type'] = strtr($instance_value['widget']['type'], $replace);
      break;
  }
}
