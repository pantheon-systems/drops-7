<?php


/**
 * Can find a parent path for a given path.
 * Contains a cache.
 */
class crumbs_ParentFinder {

  protected $pluginEngine;

  // cached parent paths
  protected $parents = array();
  protected $log = array();

  function __construct($pluginEngine) {
    $this->pluginEngine = $pluginEngine;
  }

  function getParentPath($path, &$item) {
    if (!isset($this->parents[$path])) {
      $parent_path = $this->_findParentPath($path, $item);
      if (is_string($parent_path)) {
        $parent_path = drupal_get_normal_path($parent_path);
      }
      $this->parents[$path] = $parent_path;
    }
    return $this->parents[$path];
  }

  function getLoggedCandidates($path) {
    if (is_array($this->log[$path])) {
      return $this->log[$path];
    }
    else {
      return array();
    }
  }

  protected function _findParentPath($path, &$item) {
    if ($item) {
      if (!$item['access']) {
        // Parent should be the front page.
        return FALSE;
      }
      $plugin_operation = new crumbs_PluginOperation_findParent($path, $item);
      $this->pluginEngine->invokeAll_find($plugin_operation);
      $parent_path = $plugin_operation->getValue();
      $this->log[$path] = $plugin_operation->getLoggedCandidates();
      if (isset($parent_path)) {
        $item['crumbs_candidate_key'] = $plugin_operation->getCandidateKey();
        return $parent_path;
      }
    }
    // fallback: chop off the last fragment of the system path.
    $parent_path = crumbs_reduce_path($path);
    return isset($parent_path) ? $parent_path : FALSE;
  }
}
