<?php


/**
 * API object to be used as an argument for hook_crumbs_plugins()
 * This is a sandbox class, currently not used..
 */
class crumbs_InjectedAPI_hookCrumbsPlugins {

  protected $module;
  protected $plugins;
  protected $disabledKeys;

  function __construct(&$plugins, &$disabled_keys) {
    $this->plugins =& $plugins;
    $this->disabledKeys =& $disabled_keys;
  }

  /**
   * This is typically called before each invocation of hook_crumbs_plugins(),
   * to let the object know about the module implementing the hook.
   * Modules can call this directly if they want to let other modules talk to
   * the API object.
   *
   * @param $module
   *   The module name.
   */
  function setModule($module) {
    $this->module = $module;
  }

  /**
   * Register a "Multi" plugin.
   * That is, a plugin that defines more than one rule.
   *
   * @param $key
   *   Rule key, relative to module name.
   * @param $plugin
   *   Plugin object. Needs to implement crumbs_MultiPlugin.
   *   Or NULL, to have the plugin object automatically created based on a
   *   class name guessed from the $key parameter and the module name.
   */
  function monoPlugin($key = NULL, $plugin = NULL) {
    if (!isset($key)) {
      $class = $this->module . '_CrumbsMonoPlugin';
      $plugin = new $class();
      $key = $this->module;
    }
    elseif (!isset($plugin)) {
      $class = $this->module . '_CrumbsMonoPlugin_' . $key;
      $plugin = new $class();
      $key = $this->module . '.' . $key;
    }
    else {
      $class = get_class($plugin);
      $key = $this->module . '.' . $key;
    }
    if (!($plugin instanceof crumbs_MonoPlugin)) {
      throw new Exception("$class must implement class_MonoPlugin.");
    }
    $this->plugins[$key] = $plugin;
    if (method_exists($plugin, 'disabledByDefault')) {
      $disabled_by_default = $plugin->disabledByDefault();
      if ($disabled_by_default === TRUE) {
        $this->disabledKeys[$key] = $key;
      }
      elseif ($disabled_by_default !== FALSE && $disabled_by_default !== NULL) {
        throw new Exception("$class::disabledByDefault() must return TRUE, FALSE or NULL.");
      }
    }
  }

  /**
   * Register a "Multi" plugin.
   * That is, a plugin that defines more than one rule.
   *
   * @param $key
   *   Rule key, relative to module name.
   * @param $plugin
   *   Plugin object. Needs to implement crumbs_MultiPlugin.
   *   Or NULL, to have the plugin object automatically created based on a
   *   class name guessed from the $key parameter and the module name.
   */
  function multiPlugin($key, $plugin = NULL) {
    if (!isset($key)) {
      $class = $this->module . '_CrumbsMultiPlugin';
      $plugin = new $class();
      $plugin_key = $this->module;
    }
    elseif (!isset($plugin)) {
      $class = $this->module . '_CrumbsMultiPlugin_' . $key;
      $plugin = new $class();
      $plugin_key = $this->module . '.' . $key;
    }
    else {
      $class = get_class($plugin);
      $plugin_key = $this->module . '.' . $key;
    }
    if (!($plugin instanceof crumbs_MultiPlugin)) {
      throw new Exception("$class must implement class_MultiPlugin.");
    }
    $this->plugins[$plugin_key] = $plugin;
    if (method_exists($plugin, 'disabledByDefault')) {
      $disabled_by_default = $plugin->disabledByDefault();
      if ($disabled_by_default !== NULL) {
        if (!is_array($disabled_by_default)) {
          throw new Exception("$class::disabledByDefault() must return an array or NULL.");
        }
        foreach ($disabled_by_default as $suffix) {
          if (!isset($suffix) || $suffix === '') {
            throw new Exception("$class::disabledByDefault() - returned array contains an empty key.");
          }
          else {
            $key = $plugin_key . '.' . $suffix;
            $disabled_keys[$key] = $key;
          }
        }
      }
    }
  }

  /**
   * Set specific rules as disabled by default.
   *
   * @param $keys
   *   Array of keys, relative to the module name, OR
   *   a single string key, relative to the module name.
   */
  function disabledByDefault($keys = NULL) {
    if (is_array($keys)) {
      foreach ($keys as $key) {
        $this->_disabledByDefault($key);
      }
    }
    else {
      $this->_disabledByDefault($keys);
    }
  }

  protected function _disabledByDefault($key) {
    $key = isset($key) ? ($this->module . '.' . $key) : $this->module;
    $this->disabledKeys[$key] = $key;
  }
}
