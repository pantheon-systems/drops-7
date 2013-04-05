<?php


/**
 * Implements hook_crumbs_plugins().
 */
function path_crumbs_plugins($api) {
  // Just one plugin for the entire module.
  $api->monoPlugin();
}


/**
 * Implementation of class hook class_CrumbsParentFinder
 * on the behalf of path module.
 */
class path_CrumbsMonoPlugin implements crumbs_MonoPlugin {

  function describe($api) {
    return t('Chop off the last fragment of the path alias, consider the result as the parent path.');
  }

  function disabledByDefault() {
    return TRUE;
  }

  function findParent($path, $item) {
    if ($alias = $item['alias']) {
      return crumbs_reduce_path($alias, 1);
    }
  }
}
