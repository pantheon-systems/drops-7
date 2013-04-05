<?php


/**
 * Injected API object for the describe() method of mono plugins.
 */
class crumbs_InjectedAPI_describeMonoPlugin {

  protected $pluginOperation;

  function __construct($plugin_operation) {
    $this->pluginOperation = $plugin_operation;
  }

  function setTitle($title) {
    $this->pluginOperation->setTitle($title);
  }
}
