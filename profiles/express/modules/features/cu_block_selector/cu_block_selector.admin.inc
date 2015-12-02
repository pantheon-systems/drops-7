<?php

/**
 * Page callback for admin/config/system/cu_block_selector.
 */
function cu_block_selector_admin_settings() {
  $all_fields = _cu_block_selector_all_fields();
  foreach ($all_fields as $field_name => $field) {
    $options[$field_name] = $field['label'];
  }

  // This code toggles the correct checkboxes when overrides are toggled. You can't have the overrides without their corresponding fields.
  $onchange_js = "
    if(this.value == 'field_override_sidebar_first') {
      document.getElementById('edit-cu-block-selector-fields-field-sidebar-first').checked = document.getElementById('edit-cu-block-selector-fields-field-override-sidebar-first').checked;
    }
    if(this.value == 'field_override_sidebar_second') {
      document.getElementById('edit-cu-block-selector-fields-field-sidebar-second').checked = document.getElementById('edit-cu-block-selector-fields-field-override-sidebar-second').checked;
    }
  ";
  $form = array();
  $form['cu_block_selector_types'] = array(
    '#title' => t('Content types'),
    '#description' => t('Activate CU Block Selector on the selected content types.'),
    '#type' => 'checkboxes',
    '#options' => node_type_get_names(),
    '#default_value' => variable_get('cu_block_selector_types', array()),
  );
  $form['cu_block_selector_fields'] = array(
    '#title' => t('Fields'),
    '#description' => t('Activate CU Block Selector on the following fields.'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => variable_get('cu_block_selector_fields', array()),
    '#attributes' => array('onchange' => $onchange_js),
  );
  $form['#submit'][] = 'cu_block_selector_admin_settings_submit';
  return system_settings_form($form);
}

/**
 * Submit handler for block selector admin settings form.
 */
function cu_block_selector_admin_settings_submit($form, &$form_state) {
  // Grab $previous and $new types/fields.
  // array_diff($array, array(0)) removes any 0 values from the array.
  $previous_types = array_diff(variable_get('cu_block_selector_types', array()), array(0));
  $new_types = array_diff($form_state['values']['cu_block_selector_types'], array(0));
  $previous_fields = variable_get('cu_block_selector_fields', array());
  $new_fields = array_diff($form_state['values']['cu_block_selector_fields'], array(0));

  // Calculate diffs between fields and types arrays.
  $removed_fields = array_diff($previous_fields, $new_fields);
  $added_fields = array_diff($new_fields, $previous_fields);
  $removed_types = array_diff($previous_types, $new_types);
  $added_types = array_diff($new_types, $previous_types);

  // Content types that remained the same (not added, not removed).
  $same_types = array_diff(array_diff($new_types, $added_types), $removed_types);

  $all_fields = array_keys(_cu_block_selector_all_fields());

  // Remove all fields from 'removed' content types.
  foreach ($removed_types as $remove_type) {
    _cu_block_selector_remove_field_instances($remove_type, $all_fields);
  }

  // Remove all removed fields from previous content types.
  foreach ($previous_types as $previous_type) {
    _cu_block_selector_remove_field_instances($previous_type, $removed_fields);
  }

  // Add newly selected fields to new content types.
  foreach ($added_types as $add_type) {
    _cu_block_selector_add_field_instances($add_type, $new_fields);
  }
  // Add added fields to 'same' content types (which havent been removed).
  foreach ($same_types as $same_type) {
    _cu_block_selector_add_field_instances($same_type, $added_fields);
  }
}
