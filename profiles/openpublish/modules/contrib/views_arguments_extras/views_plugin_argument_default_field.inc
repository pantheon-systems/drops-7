<?php

class views_plugin_argument_default_field extends views_plugin_argument_default {
  
  var $option_name = 'default_argument_field';
  function option_definition() {
    $options = parent::option_definition();
    $options['field'] = array('default' => '');
    $options['settings'] = array('default' => array());

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $default_options = $this->argument->options;
    $fields = field_info_fields();
    $options = array();
    $plugins = array();
    $plugins = views_plugin_argument_default_field();
    $type_selector = array();

    // find all types that are useed and which fields use them
    $types = array();
    foreach ($fields as $key => $field) {
      $types[$field['type']][$key] = $key;
    }
    foreach ($plugins as $id => $plugin) {
      if (array_intersect($plugin['types'], array_keys($types))) {
        //build info for type select box
        foreach ($plugin['types'] as $type_name) {
          foreach ($types[$type_name] as $fid => $fname) {
            $options[$plugin['title']]["$id::$fid"] = $fname;
          }
        }
      }
    }

    $form['field'] = array(
      '#title' => 'Field',
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $this->options['field'],
      '#ajax' => array(
        'callback' => 'views_plugin_argument_default_field_field_callback',
        'wrapper' => 'field-settings-wrapper',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
    $form['settings'] = array(
      '#title' => "Settings",
      '#type' => 'fieldset',
      '#prefix' => '<div class="field-settings-wrapper">',
      '#suffix' => '</div>',
    );
    if ($this->options['field']) {
      list($p_id, $field) = explode("::", $this->options['field']);
      $plugin = $plugins[$p_id];
      $form['settings']['#title'] = "Settings for {$plugin['name']}";
      if ($form_fun = ctools_plugin_get_function($plugin, "form callback")) {
        $plugin_form = $form_fun();
        foreach ($plugin_form as $id => $form_element) {
          $form['settings'][$id] = $form_element;
          $form['settings'][$id]['#default_value'] = $this->options['settings'][$id];
        }
      }
    }
  }
  function get_argument() {
    //$nid = parent::get_argument();
    if ($this->options['field']) {
      global $language;
      list($plugin_id, $field_name) = explode("::", $this->options['field']);
      $node = views_plugin_argument_default_get_node();

      $lang = isset($node->{$field_name}[$language->language]) ? $language->language : 'und';
      $field = isset($node->{$field_name}[$lang]) ? $node->{$field_name}[$lang] : array();

      $plugin = views_plugin_argument_default_field($plugin_id);
      if ($arg_fun = ctools_plugin_get_function($plugin, "argument callback")) {
        $return = $arg_fun($field, $this->options['settings'], $node, $this);
        return $return;
      }
    /*
    $options = $this->options['options'];
    $type = $options[$this->option_name];
    $field_name = $options[$this->option_name ."-$type"];
    $node = views_plugin_argument_default_get_node();
    $field = $node->{$field_name};
    $plugin = views_plugin_argument_default_field($type);
    if (($arg_fun = $plugin['argument callback']) && function_exists($arg_fun)) {
      $return = $arg_fun($field,$options, $node, $this);
      return $return;
    }
    */
    }
  }
}

function views_plugin_argument_default_get_node() {
    foreach (range(1, 3) as $i) {
      $node = menu_get_object('node', $i);
      if (!empty($node)) {
        return $node;
      }
    }
    foreach (range(1, 3) as $i) {
      $term = menu_get_object('taxonomy_term', $i);
      if (!empty($term)) {
        return $term;
      }
    }

    if (arg(0) == 'node' && is_numeric(arg(1))) {
      return node_load(arg(1));
    }
}


function views_plugin_argument_default_field($name=FALSE) {
  ctools_include('plugins'); 
  $defaults = ctools_get_plugins('views_arguments_extras', 'default');
  if ($name) {
    return $defaults[$name];
  }
  return $defaults;
}
