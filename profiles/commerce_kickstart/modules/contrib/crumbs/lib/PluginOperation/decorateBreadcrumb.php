<?php


class crumbs_PluginOperation_decorateBreadcrumb implements crumbs_PluginOperationInterface_alter {

  protected $breadcrumb;

  function __construct(array &$breadcrumb) {
    $this->breadcrumb = &$breadcrumb;
  }

  function invoke($plugin, $plugin_key) {
    if (method_exists($plugin, 'decorateBreadcrumb')) {
      $plugin->decorateBreadcrumb($this->breadcrumb);
    }
  }
}
