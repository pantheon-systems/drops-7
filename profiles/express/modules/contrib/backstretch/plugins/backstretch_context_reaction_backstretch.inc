<?php

class backstretch_context_reaction_backstretch extends context_reaction {
  function options_form($context) {
    $values = $this->fetch_from_context($context);
    return array(
      'backstretch_url' => array(
        '#type' => 'textfield',
        '#value' => isset($values['backstretch_url']) ? $values['backstretch_url'] : '', 
        '#title' => t('Backstretch URL'),
      ),
    );
  }
  function options_form_submit($values) {
    return array('backstretch_url' => $values['backstretch_url']);
  }

  function execute() {
    foreach($this->get_contexts() as $context) {
      if (!empty($context->reactions[$this->plugin])) {
        global $conf;
        $conf['backstretch_image_url'] = $context->reactions[$this->plugin]['backstretch_url'];
      }
    }
    return array();
  }
}

