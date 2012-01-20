<?php

class views_plugin_argument_default_request_params extends views_plugin_argument_default {
  
  function argument_form(&$form, &$form_state) {
    $default_options = $this->argument->options;
    $form['param_name'] = array(
      '#title' => 'Param Name',
      '#type' => 'textfield',
      '#default_value' => $default_options['param_name'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array(
        'radio:options[default_action]' => array('default'),
        'radio:options[default_argument_type]' => array($this->id)
      ),
      '#dependency_count' => 2,
    );
  }
  function get_argument() {
    $options = $this->argument->options;
    $param = $options['param_name'];
    
    if($_REQUEST[$param]) {
      return $_REQUEST[$param];
    }
    else {
      return $options['wildcard'];
    }
  }
}
