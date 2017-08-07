<?php

class context_reaction_social_share extends context_reaction {

  function editor_form($context) {
  }

  function options_form($context) {
    $values = $this->fetch_from_context($context);
    $default = isset($values['cu_share']) ? $values['cu_share'] : 0;

    $form = array();
    $link = drupal_valid_path('admin/settings/social/share') ? 'admin/settings/social/share' : NULL;
    if ($link) {
      $form['cu_share_config'] = array(
        '#type' => 'markup',
        '#title' => 'Social Sharing Links',
        '#markup' => l('Configure Social Sharing Links', 'admin/settings/social/share'),
      );
    }
    $form['cu_share'] = array(
      '#type' => 'hidden',
      '#value' => TRUE,
    );

    return $form;
  }

  function execute(&$page) {
    foreach ($this->get_contexts() as $k => $v) {
      if (isset($v->reactions[$this->plugin]['cu_share'])) {
        return TRUE;
      }
    }
  }
}
