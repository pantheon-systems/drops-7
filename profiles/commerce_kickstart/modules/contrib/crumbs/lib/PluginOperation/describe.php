<?php


/**
 * This class uses the PluginOperation pattern, but it does not implement any of
 * the PluginOperation interfaces. This is because it is not supposed to be used
 * with the PluginEngine, but rather from a custom function (see above).
 */
class crumbs_PluginOperation_describe {

  protected $keys = array('*' => TRUE);
  protected $keysByPlugin = array();
  protected $pluginKey;
  protected $injectedAPI_mono;
  protected $injectedAPI_multi;

  function __construct() {
    $this->injectedAPI_mono = new crumbs_InjectedAPI_describeMonoPlugin($this);
    $this->injectedAPI_multi = new crumbs_InjectedAPI_describeMultiPlugin($this);
  }

  /**
   * To be called from _crumbs_load_available_keys()
   */
  function invoke($plugin, $plugin_key) {
    $this->pluginKey = $plugin_key;
    if ($plugin instanceof crumbs_MonoPlugin) {
      $result = $plugin->describe($this->injectedAPI_mono);
      if (is_string($result)) {
        $this->setTitle($result);
      }
    }
    elseif ($plugin instanceof crumbs_MultiPlugin) {
      // That's a multi plugin.
      $result = $plugin->describe($this->injectedAPI_multi);
      if (is_array($result)) {
        foreach ($result as $key_suffix => $title) {
          $this->addRule($key_suffix, $title);
        }
      }
    }
  }

  /**
   * To be called from crumbs_InjectedAPI_describeMultiPlugin::addRule()
   */
  function addRule($key_suffix, $title) {
    $this->_addRule($this->pluginKey .'.'. $key_suffix, $title);
  }

  /**
   * To be called from crumbs_InjectedAPI_describeMonoPlugin::setTitle()
   */
  function setTitle($title) {
    $this->_addRule($this->pluginKey, $title);
  }

  protected function _addRule($key, $title) {
    $fragments = explode('.', $key);
    $partial_key = array_shift($fragments);
    while (TRUE) {
      if (empty($fragments)) break;
      $wildcard_key = $partial_key .'.*';
      $this->keys[$wildcard_key] = TRUE;
      $this->keysByPlugin[$this->pluginKey][$wildcard_key] = TRUE;
      $partial_key .= '.'. array_shift($fragments);
    }
    $this->keys[$key] = $title;
    $this->keysByPlugin[$this->pluginKey][$key] = $title;
  }

  function getKeys() {
    return $this->keys;
  }

  function getKeysByPlugin() {
    return $this->keysByPlugin;
  }
}
