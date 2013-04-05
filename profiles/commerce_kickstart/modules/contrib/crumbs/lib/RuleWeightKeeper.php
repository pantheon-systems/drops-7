<?php


class crumbs_RuleWeightKeeper {

  protected $ruleWeights;
  protected $prefixedKeepers = array();
  protected $prefixSorted = array();
  protected $soloSorted = array();

  function __construct(array $rule_weights) {
    asort($rule_weights);
    $this->ruleWeights = $rule_weights;
  }

  function prefixedWeightKeeper($prefix) {
    if (!isset($this->prefixedKeepers[$prefix])) {
      $this->prefixedKeepers[$prefix] = $this->_buildPrefixedWeightKeeper($prefix);
    }
    return $this->prefixedKeepers[$prefix];
  }

  protected function _buildPrefixedWeightKeeper($prefix) {
    $weights = array();
    $k = strlen($prefix);
    $weights[''] = $weights['*'] = $this->_findWildcardWeight($prefix);
    if (isset($this->ruleWeights[$prefix])) {
      $weights[''] = $this->ruleWeights[$prefix];
    }
    if (isset($this->ruleWeights[$prefix .'.*'])) {
      $weights['*'] = $this->ruleWeights[$prefix .'.*'];
    }
    foreach ($this->ruleWeights as $key => $weight) {
      if (strlen($key) > $k && substr($key, 0, $k+1) === ($prefix .'.')) {
        $weights[substr($key, $k+1)] = $weight;
      }
    }
    return new crumbs_RuleWeightKeeper($weights);
  }

  function getSmallestWeight() {
    foreach ($this->ruleWeights as $weight) {
      if ($weight !== FALSE) {
        return $weight;
      }
    }
    return FALSE;
  }

  /**
   * Determine the weight for the rule specified by the key.
   */
  function findWeight($key = NULL) {
    if (!isset($key)) {
      return $this->ruleWeights[''];
    }
    if (isset($this->ruleWeights[$key])) {
      return $this->ruleWeights[$key];
    }
    return $this->_findWildcardWeight($key);
  }

  protected function _findWildcardWeight($key) {
    $fragments = explode('.', $key);
    $partial_key = array_shift($fragments);
    $weight = $this->ruleWeights['*'];
    while (!empty($fragments)) {
      if (isset($this->ruleWeights[$partial_key .'.*'])) {
        $weight = $this->ruleWeights[$partial_key .'.*'];
      }
      $partial_key .= '.'. array_shift($fragments);
    }
    return $weight;
  }
}
