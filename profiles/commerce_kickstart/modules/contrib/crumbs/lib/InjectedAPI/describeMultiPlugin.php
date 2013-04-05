<?php


/**
 * Injected API object for the describe() method of multi plugins.
 */
class crumbs_InjectedAPI_describeMultiPlugin {

  protected $pluginOperation;

  function __construct($plugin_operation) {
    $this->pluginOperation = $plugin_operation;
  }

  function addRule($key_suffix, $title = TRUE) {
    $this->pluginOperation->addRule($key_suffix, $title);
  }
}
