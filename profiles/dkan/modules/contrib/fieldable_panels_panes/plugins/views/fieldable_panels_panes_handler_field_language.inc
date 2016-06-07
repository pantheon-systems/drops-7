<?php

/**
 * @file
 * Definition of fieldable_panels_panes_handler_field_language.
 */

/**
 * Field handler to translate a language into its readable form.
 */
class fieldable_panels_panes_handler_field_language extends views_handler_field_node {

  /**
   * {@inheritdoc}
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['native_language'] = array('default' => FALSE, 'bool' => TRUE);

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['native_language'] = array(
      '#title' => t('Native language'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['native_language'],
      '#description' => t('If enabled, the native name of the language will be displayed'),
    );
  }

  /**
   * {@inheritdoc}
   */
  function render($values) {
    $languages = views_language_list(empty($this->options['native_language']) ? 'name' : 'native');
    $value = $this->get_value($values);
    $value = isset($languages[$value]) ? $languages[$value] : '';
    return $this->render_link($value, $values);
  }

}
