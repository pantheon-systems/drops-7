<?php

/**
 * @file
 * Provide views handlers for fieldable panel panes fields
 */

/**
 * Field handler to translate a node type into its readable form.
 */
class fieldable_panels_panes_handler_field_bundle extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['machine_name'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Provide machine_name option for to node type display.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['machine_name'] = array(
      '#title' => t('Output machine name'),
      '#description' => t('Display field as the bundle machine name.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['machine_name']),
      '#fieldset' => 'more',
    );
  }

  function render($values) {
    $value = $this->get_value($values);
    if (!$this->options['machine_name']) {
      $value = fieldable_panels_panes_get_bundle_label($value);
    }

    return $this->sanitize_value($value);
  }
}
