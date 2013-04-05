<?php


abstract class crumbs_PluginOperation_findForPath implements crumbs_PluginOperationInterface_find {

  // injected constructor parameters
  protected $path;
  protected $item;

  protected $method;  // child class should override this.
  protected $methodSuffix;  // child class should override this.
  protected $methods = array();

  // weight, value and key of the candidate with highest priority
  protected $candidateKey;
  protected $candidateValue;
  protected $candidateWeight;
  protected $log;

  function __construct($path, $item) {
    $this->path = $path;
    $this->item = $item;
    // Replace all characters with something that is allowed in method names.
    // while avoiding false positives.
    // Example: 'findParent__node_x()' should only match 'node/%',
    // but not 'node-_' or 'node-x', or other exotic router paths.
    // Special character router paths can not be matched by any method name,
    // so you will need to use switch() or if/else on $item['path'].
    $method_suffix = crumbs_build_method_suffix($item['path']);
    if ($method_suffix !== FALSE) {
      $this->methods[] = $this->method .'__'. $method_suffix;
    }
    $this->methods[] = $this->method;
  }

  /**
   * This should run once for each plugin object.
   * It should be called by the PluginEngine, during invokeUntilFound().
   */
  function invoke($plugin, $plugin_key, $weight_keeper) {
    $smallest_weight = $weight_keeper->getSmallestWeight();
    if (isset($this->candidateWeight) && $this->candidateWeight <= $smallest_weight) {
      // any further candidate would have a higher weight, thus lower priority,
      // than what we already have found. Thus, we can stop searching.
      return TRUE;
    }
    foreach ($this->methods as $method) {
      if (method_exists($plugin, $method)) {
        $result = $this->_invoke($plugin, $method);
        break;
      }
    }
    if ($plugin instanceof crumbs_MultiPlugin) {
      // we expect an array result.
      if (!empty($result) && is_array($result)) {
        foreach ($result as $key => $value) {
          $weight = $weight_keeper->findWeight($key);
          $this->_setValue($plugin_key .'.'. $key, $value, $weight);
        }
      }
      else {
        $this->log[$plugin_key .'.*'] = array(NULL, NULL);
      }
    }
    elseif ($plugin instanceof crumbs_MonoPlugin) {
      // we expect a simple value as a result
      if (isset($result)) {
        $weight = $weight_keeper->findWeight();
        $this->_setValue($plugin_key, $result, $weight);
      }
      else {
        $this->log[$plugin_key] = array(NULL, NULL);
      }
    }
  }

  /**
   * This runs at the end of the PluginOperation's life cycle,
   * and returns the value that was determined.
   */
  function getValue() {
    return $this->candidateValue;
  }

  function getCandidateKey() {
    return $this->candidateKey;
  }

  function getLoggedCandidates() {
    return $this->log;
  }

  protected function _setValue($key, $value, $weight) {
    if ($weight !== FALSE) {
      if (!isset($this->candidateWeight) || $weight < $this->candidateWeight) {
        $this->candidateWeight = $weight;
        $this->candidateValue = $value;
        $this->candidateKey = $key;
      }
    }
    $this->log[$key] = array($value, $weight);
  }

  abstract protected function _invoke($plugin, $method);
}
