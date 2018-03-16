<?php

/**
 * @file
 * Hooks provided by the Chaos Tool Suite.
 *
 * This file is divided into static hooks (hooks with string literal names) and
 * dynamic hooks (hooks with pattern-derived string names).
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Inform CTools about plugin types.
 *
 * @return array
 *  An array of plugin types, keyed by the type name.
 *  See the advanced help topic 'plugins-creating' for details of the array
 *  properties.
 */
function hook_ctools_plugin_type() {
  $plugins['my_type'] = array(
    'load themes' => TRUE,
  );

  return $plugins;
}

/**
 * This hook is used to inform the CTools plugin system about the location of a
 * directory that should be searched for files containing plugins of a
 * particular type. CTools invokes this same hook for all plugins, using the
 * two passed parameters to indicate the specific type of plugin for which it
 * is searching.
 *
 * The $plugin_type parameter is self-explanatory - it is the string name of the
 * plugin type (e.g., Panels' 'layouts' or 'styles'). The $owner parameter is
 * necessary because CTools internally namespaces plugins by the module that
 * owns them. This is an extension of Drupal best practices on avoiding global
 * namespace pollution by prepending your module name to all its functions.
 * Consequently, it is possible for two different modules to create a plugin
 * type with exactly the same name and have them operate in harmony. In fact,
 * this system renders it impossible for modules to encroach on other modules'
 * plugin namespaces.
 *
 * Given this namespacing, it is important that implementations of this hook
 * check BOTH the $owner and $plugin_type parameters before returning a path.
 * If your module does not implement plugins for the requested module/plugin
 * combination, it is safe to return nothing at all (or NULL). As a convenience,
 * it is also safe to return a path that does not exist for plugins your module
 * does not implement - see form 2 for a use case.
 *
 * Note that modules implementing a plugin also must implement this hook to
 * instruct CTools as to the location of the plugins. See form 3 for a use case.
 *
 * The conventional structure to return is "plugins/$plugin_type" - that is, a
 * 'plugins' subdirectory in your main module directory, with individual
 * directories contained therein named for the plugin type they contain.
 *
 * @param string $owner
 *   The system name of the module owning the plugin type for which a base
 *   directory location is being requested.
 * @param string $plugin_type
 *   The name of the plugin type for which a base directory is being requested.
 * @return string
 *   The path where CTools' plugin system should search for plugin files,
 *   relative to your module's root. Omit leading and trailing slashes.
 */
function hook_ctools_plugin_directory($owner, $plugin_type) {
  // Form 1 - for a module implementing only the 'content_types' plugin owned
  // by CTools, this would cause the plugin system to search the
  // <moduleroot>/plugins/content_types directory for .inc plugin files.
  if ($owner == 'ctools' && $plugin_type == 'content_types') {
    return 'plugins/content_types';
  }

  // Form 2 - if your module implements only Panels plugins, and has 'layouts'
  // and 'styles' plugins but no 'cache' or 'display_renderers', it is OK to be
  // lazy and return a directory for a plugin you don't actually implement (so
  // long as that directory doesn't exist). This lets you avoid ugly in_array()
  // logic in your conditional, and also makes it easy to add plugins of those
  // types later without having to change this hook implementation.
  if ($owner == 'panels') {
    return "plugins/$plugin_type";
  }

  // Form 3 - CTools makes no assumptions about where your plugins are located,
  // so you still have to implement this hook even for plugins created by your
  // own module.
  if ($owner == 'mymodule') {
    // Yes, this is exactly like Form 2 - just a different reasoning for it.
    return "plugins/$plugin_type";
  }
  // Finally, if nothing matches, it's safe to return nothing at all (or NULL).
}

/**
 * Alter a plugin before it has been processed.
 *
 * This hook is useful for altering flags or other information that will be
 * used or possibly overriden by the process hook if defined.
 *
 * @param $plugin
 *   An associative array defining a plugin.
 * @param $info
 *   An associative array of plugin type info.
 */
function hook_ctools_plugin_pre_alter(&$plugin, &$info) {
  // Override a function defined by the plugin.
  if ($info['type'] == 'my_type') {
    $plugin['my_flag'] = 'new_value';
  }
}

/**
 * Alter a plugin after it has been processed.
 *
 * This hook is useful for overriding the final values for a plugin after it
 * has been processed.
 *
 * @param $plugin
 *   An associative array defining a plugin.
 * @param $info
 *   An associative array of plugin type info.
 */
function hook_ctools_plugin_post_alter(&$plugin, &$info) {
  // Override a function defined by the plugin.
  if ($info['type'] == 'my_type') {
    $plugin['my_function'] = 'new_function';
  }
}

/**
 * Alter the list of modules/themes which implement a certain api.
 *
 * The hook named here is just an example, as the real existing hooks are named
 * for example 'hook_views_api_alter'.
 *
 * @param array $list
 *   An array of informations about the implementors of a certain api.
 *   The key of this array are the module names/theme names.
 */
function hook_ctools_api_hook_alter(&$list) {
  // Alter the path of the node implementation.
  $list['node']['path'] = drupal_get_path('module', 'node');
}

/**
 * Alter the available functions to be used in ctools math expression api.
 *
 * One usecase would be to create your own function in your module and
 * allow to use it in the math expression api.
 *
 * @param $functions
 *    An array which has the functions as value.
 */
function hook_ctools_math_expression_functions_alter(&$functions) {
  // Allow to convert from degrees to radiant.
  $functions[] = 'deg2rad';
}

/**
 * Alter everything.
 *
 * @param $info
 *   An associative array containing the following keys:
 *   - content: The rendered content.
 *   - title: The content's title.
 *   - no_blocks: A boolean to decide if blocks should be displayed.
 * @param $page
 *   If TRUE then this renderer owns the page and can use theme('page')
 *   for no blocks; if false, output is returned regardless of any no
 *   blocks settings.
 * @param $context
 *   An associative array containing the following keys:
 *   - args: The raw arguments behind the contexts.
 *   - contexts: The context objects in use.
 *   - task: The task object in use.
 *   - subtask: The subtask object in use.
 *   - handler: The handler object in use.
 */
function hook_ctools_render_alter(&$info, &$page, &$context) {
  if ($context['handler']->name == 'my_handler') {
    ctools_add_css('my_module.theme', 'my_module');
  }
}

/**
 * Alter a content plugin subtype.
 *
 * While content types can be altered via hook_ctools_plugin_pre_alter() or
 * hook_ctools_plugin_post_alter(), the subtypes that content types rely on
 * are special and require their own hook.
 *
 * This hook can be used to add things like 'render last' or change icons
 * or categories or to rename content on specific sites.
 */
function hook_ctools_content_subtype_alter($subtype, $plugin) {
  // Force a particular subtype of a particular plugin to render last.
  if ($plugin['module'] == 'some_plugin_module' && $plugin['name'] == 'some_plugin_name' && $subtype['subtype_id'] == 'my_subtype_id') {
    $subtype['render last'] = TRUE;
  }
}

/**
 * Alter the definition of an entity context plugin.
 *
 * @param array $plugin
 *   An associative array defining a plugin.
 * @param array $entity
 *   The entity info array of a specific entity type.
 * @param string $plugin_id
 *   The plugin ID, in the format NAME:KEY.
 */
function hook_ctools_entity_context_alter(&$plugin, &$entity, $plugin_id) {
  ctools_include('context');
  switch ($plugin_id) {
    case 'entity_id:taxonomy_term':
      $plugin['no ui'] = TRUE;
    case 'entity:user':
      $plugin = ctools_get_context('user');
      unset($plugin['no ui']);
      unset($plugin['no required context ui']);
      break;
  }
}

/**
 * Alter the definition of entity context plugins.
 *
 * @param array $plugins
 *   An associative array of plugin definitions, keyed by plugin ID.
 *
 * @see hook_ctools_entity_context_alter()
 */
function hook_ctools_entity_contexts_alter(&$plugins) {
  $plugins['entity_id:taxonomy_term']['no ui'] = TRUE;
}

/**
 * Change cleanstring settings.
 *
 * @param array $settings
 *   An associative array of cleanstring settings.
 *
 * @see ctools_cleanstring()
 */
function hook_ctools_cleanstring_alter(&$settings) {
  // Convert all strings to lower case.
  $settings['lower case'] = TRUE;
}

/**
 * Change cleanstring settings for a specific clean ID.
 *
 * @param array $settings
 *   An associative array of cleanstring settings.
 *
 * @see ctools_cleanstring()
 */
function hook_ctools_cleanstring_CLEAN_ID_alter(&$settings) {
  // Convert all strings to lower case.
  $settings['lower case'] = TRUE;
}

/**
 * @} End of "addtogroup hooks".
 */
