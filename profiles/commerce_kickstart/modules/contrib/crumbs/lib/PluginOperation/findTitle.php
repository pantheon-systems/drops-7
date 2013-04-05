<?php


class crumbs_PluginOperation_findTitle extends crumbs_PluginOperation_findForPath {

  protected $method = 'findTitle';
  protected $breadcrumbParents;

  function __construct($path, array $item, array $breadcrumb_parents) {
    crumbs_PluginOperation_findForPath::__construct($path, $item);
    $this->breadcrumbParents = $breadcrumb_parents;
  }

  protected function _invoke($plugin, $method) {
    return $plugin->$method($this->path, $this->item, $this->breadcrumbParents);
  }
}
