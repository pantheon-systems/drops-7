<?php


class crumbs_BreadcrumbBuilder {

  protected $pluginEngine;

  function __construct($pluginEngine) {
    $this->pluginEngine = $pluginEngine;
  }

  function buildBreadcrumb($trail) {
    $breadcrumb = array();
    foreach ($trail as $path => $item) {
      if ($item) {
        $title = $this->_findTitle($path, $item, $breadcrumb);
        if (!isset($title)) {
          $title = $item['title'];
        }
        // The item will be skipped, if $title === FALSE.
        if (isset($title) && $title !== FALSE && $title !== '') {
          $item['title'] = $title;
          $breadcrumb[] = $item;
        }
      }
    }
    $this->_decorateBreadcrumb($breadcrumb);
    return $breadcrumb;
  }

  protected function _findTitle($path, array $item, array $breadcrumb_parents) {
    $plugin_operation = new crumbs_PluginOperation_findTitle($path, $item, $breadcrumb_parents);
    $this->pluginEngine->invokeAll_find($plugin_operation);
    return $plugin_operation->getValue();
  }

  protected function _decorateBreadcrumb(array &$breadcrumb) {
    $plugin_operation = new crumbs_PluginOperation_decorateBreadcrumb($breadcrumb);
    $this->pluginEngine->invokeAll_alter($plugin_operation);
  }
}
