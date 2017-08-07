<?php

/**
 * @file
 * Views handler to display the open or closed status of a webform.
 */
class webform_handler_field_webform_status extends views_handler_field_boolean {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['type']['#options'] = array('open-closed' => t('Open/Closed')) + $form['type']['#options'];
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['type']['default'] = 'open-closed';
    return $options;
  }

  function render($values) {
    $value = $values->{$this->field_alias};
    if (!empty($this->options['not'])) {
      $value = !$value;
    }
    switch ($this->options['type']) {
      case 'yes-no':
        return $value ? t('Yes') : t('No');
      case 'true-false':
        return $value ? t('True') : t('False');
      case 'unicode-yes-no':
        return $value ? t('✔') : t('✖');
      case 'enabled-disabled':
        return $value ? t('Enabled') : t('Disabled');
      case 'boolean':
        return $value ? 1 : 0;
      case 'on-off':
        return $value ? t('On') : t('Off');
      case 'custom':
        return $value ? $this->options['type_custom_true'] : $this->options['type_custom_false'];
      case 'open-closed':
      default:
        return $value ? t('Open') : t('Closed');
    }
  }
}
