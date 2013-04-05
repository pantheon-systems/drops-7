<?php

/**
 * @file
 * Contains the commerce_kickstart_taxonomy_handler_field_text class.
 */

/**
 * Extend the entity_views_handler_field_text to add needed enhancements.
 */
class commerce_kickstart_taxonomy_handler_field_text extends entity_views_handler_field_text {

  /**
   * Provide options for this handler.
   */
  public function option_definition() {
    return parent::option_definition() + array(
      'replace_spaces' => array(
        'default' => FALSE,
        'bool' => TRUE,
      ),
    );
  }

  /**
   * Provide a options form for this handler.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['replace_spaces'] = array(
      '#type' => 'checkbox',
      '#title' => t('Replace spaces with dashes'),
      '#default_value' => $this->options['replace_spaces'],
    );
  }

  /**
   * Render a single field value.
   */
  public function render_single_value($value, $values) {
    $value = $this->sanitize_value($value, 'xss');
    if (!empty($this->options['replace_spaces'])) {
      $value = str_replace('-', '--', $value);
      $value = str_replace(' ', '-', $value);
    }
    return $value;
  }

}
