<?php

class context_sitewide_public_context_condition extends context_condition {
  function condition_values() {
    return array(1 => t('Always active'));
  }

  function editor_form($context = NULL) {
    $form = parent::editor_form($context);
    $form[1]['#title'] = t('Always active');
    $form['#weight'] = -10;
    return $form;
  }

  function execute($value) {
    foreach ($this->get_contexts($value) as $context) {
      if (!path_is_admin(current_path())) {
        $this->condition_met($context, $value);
      }
    }
  }
}
