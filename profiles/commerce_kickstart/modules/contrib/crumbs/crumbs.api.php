<?php


/**
 * A hook to register crumbs plugins.
 *
 * @param $api
 *   :crumbs_InjectedAPI_hookCrumbsPlugins
 *   An object with methods to register plugins.
 *   See the class definition of crumbs_InjectedAPI_hookCrumbsPlugins, which
 *   methods are available.
 */
function hook_crumbs_plugins($api) {
  $api->monoPlugin('something');
  $api->multiPlugin('somethingElse');
}


// ===================================== pseudo-interfaces =====================


/**
 * Pseudo-interface for plugin objects registered with hook_crumbs_plugins().
 * The methods defined here are all optional. We only use this interface for
 * documentation, no class actually implements it.
 */
interface crumbs_MonoPlugin_example extends crumbs_MonoPlugin {

  /**
   * Specify if this plugin is disabled by default,
   * instead of inheriting from the next matching wildcard rule.
   *
   * @return :boolean
   *   TRUE, if the plugin is disabled by default.
   */
  function disabledByDefault();

  /**
   * @param $path
   *   System path that we want to find a parent for.
   * @param $item
   *   Router item, as returned by menu_get_item(), but with a few convenience
   *   additions added in crumbs_get_router_item().
   *
   * @return
   *   The parent path suggested by this plugin.
   */
  function findParent();

  /**
   * Same signature as findParent()
   * Only called for router path node/%
   */
  function findParent__node_x($path, $item);

  /**
   * @param $path
   *   System path of the breadcrumb item that we want to find a link text for.
   * @param $item
   *   Router item, as returned by menu_get_item(), but with a few convenience
   *   additions added in crumbs_get_router_item().
   *
   * @return
   *   A string link text.
   */
  function findTitle($path, $item);

  /**
   * Same signature as findTitle()
   * Only called for router path node/%
   */
  function findTitle__node_x($path, $item);
}


// -----------------------------------------------------------------------------


/**
 * Pseudo-interface for plugin objects registered with hook_crumbs_plugins().
 * The methods defined here are all optional. We only use this interface for
 * documentation, no class actually implements it.
 */
interface crumbs_MultiPlugin_example extends crumbs_MultiPlugin {

  /**
   * Specify if some of the rules from describe() are disabled by default,
   * instead of inheriting from the next matching wildcard rule.
   *
   * @return :array
   *   Regular array, where the values identify crumbs rules or wildcards.
   *   Rule keys are relative to the plugin key.
   */
  function disabledByDefault();

  /**
   * @param $path
   *   System path that we want to find a parent for.
   * @param $item
   *   Router item, as returned by menu_get_item(), but with a few convenience
   *   additions added in crumbs_get_router_item().
   *
   * @return
   *   A key-value array, where the keys identify crumbs rules, and the values
   *   are candidates for the parent path.
   *   Rule keys are relative to the plugin key.
   */
  function findParent($path, $item);

  /**
   * Same signature as findParent()
   * Only called for router path node/%
   */
  function findParent__node_x($path, $item);

  /**
   * @param $path
   *   System path of the breadcrumb item that we want to find a link text for.
   * @param $item
   *   Router item, as returned by menu_get_item(), but with a few convenience
   *   additions added in crumbs_get_router_item().
   *
   * @return
   *   A key-value array, where the keys identify crumbs rules, and the values
   *   are candidates for the link title.
   *   Rule keys are relative to the plugin key.
   */
  function findTitle($path, $item);

  /**
   * Same signature as findParent()
   * Only called for router path node/%
   */
  function findTitle__node_x($path, $item);
}
