<?php


/**
 * Implements hook_crumbs_plugins().
 */
function crumbs_crumbs_plugins($api) {
  $api->monoPlugin('home_title');
  $api->multiPlugin('nodeParent');
}


class crumbs_CrumbsMonoPlugin_home_title implements crumbs_MonoPlugin {

  protected $frontNormalPath;

  function __construct() {
    $this->frontNormalPath = drupal_get_normal_path(variable_get('site_frontpage', 'node'));
  }

  function describe($api) {
    return t('Set "Home" as the title for the root item.');
  }

  function findTitle($path, array $item) {
    if ($path === $this->frontNormalPath) {
      return t('Home');
    }
  }
}


class crumbs_CrumbsMultiPlugin_nodeParent implements crumbs_MultiPlugin {

  protected $patterns;

  function __construct() {
    $this->patterns = variable_get('crumbs_node_parent_patterns', array());
  }

  function describe($api) {
    $types = node_type_get_types();
    foreach ($types as $type) {
      if (!empty($this->patterns[$type->type])) {
        $api->addRule($type->type);
      }
    }
  }

  function findParent__node_x($path, $item) {
    global $language;
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    $pattern = @$this->patterns[$node->type];
    if (!empty($pattern)) {
      if (module_exists('token')) {
        $parent = token_replace($pattern, array('node' => $node), array(
          'language' => $language,
          'callback' => 'crumbs_clean_token_values',
        ));
      }
      else {
        $parent = $pattern;
      }
      if (!empty($parent)) {
        // Only accept candidates where all tokens are fully resolved.
        // This means we can't have literal '[' in the path - so be it.
        if (FALSE === strpos($parent, '[')) {
          return array($node->type => $parent);
        }
      }
    }
  }
}
