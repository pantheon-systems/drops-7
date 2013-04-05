<?php


class crumbs_PluginOperation_findParent extends crumbs_PluginOperation_findForPath {

  protected $method = 'findParent';

  protected function _invoke($plugin, $method) {
    $result = $plugin->$method($this->path, $this->item);
    return $result;
  }
}
