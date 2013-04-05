<?php


/**
 * Implements hook_crumbs_plugins().
 *
 * This is the version for the og-7.x-2.x branch.
 */
function og_crumbs_plugins($api) {
  $api->multiPlugin('group_post');

  // There are no default paths for group overview pages in og-7.x-2.x anymore.
  // We leave it to custom module developers to instantiate or override those
  // classes.
}


/**
 * Use the group node as a parent for group posts.
 * The priorities can be configured per group content type.
 */
class og_CrumbsMultiPlugin_group_post implements crumbs_MultiPlugin {

  function describe($api) {
    $types = node_type_get_types();
    foreach ($types as $type) {
      if (og_is_group_content_type('node', $type->type)) {
        $api->addRule($type->type);
      }
    }
  }

  /**
   * Attempts to find a breadcrumb parent path for node/%.
   * If that node is in a group, it will return the group page as a parent.
   *
   * @param string $path
   *   The path, e.g. node/123
   * @param array $item
   *   The router item, as returned by crumbs_get_router_item()
   */
  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    // field_get_items() performs a lot faster than og_get_entity_groups().
    // See http://drupal.org/node/1819300#comment-6633494
    // TODO:
    //   We cannot rely on the field name to always be og_group_ref.
    //   Instead, we could provide a separate plugin for each such field.
    //   This way, fields in disabled plugins get never triggered.
    $items = field_get_items('node', $node, 'og_group_ref');
    if (!empty($items)) {
      foreach ($items as $item) {
        $parent_path = $this->getParentPath($item['target_id'], $node);
        return array($node->type => $parent_path);
      }
    }
  }

  /**
   * This method can be overridden by custom plugins that inherit from this one,
   * e.g. to set a different parent for group events than for group discussions.
   *
   * @param int $group_nid
   *   Node id of the group that was found to be the parent.
   * @param stdClass $group_post
   *   The node that is in the group, and that we are trying to find a parent
   *   path for.
   *
   * @return string
   *   A parent path. The native implementation returns just node/$nid.
   *   Custom module might subclass this class and override this method, to let
   *   it return e.g. node/$nid/events, or node/$nid/forum, depending on the
   *   $group_post argument.
   */
  protected function getParentPath($group_nid, $group_post) {
    return 'node/' . $group_nid;
    /*
     * Example:
     * switch ($group_post->type) {
     *   case 'event':
     *     return 'node/' . $group_nid . '/events';
     *   case 'discussion':
     *     return 'node/' . $group_nid . '/forum';
     *   default:
     *     return 'node/' . $group_nid;
     * }
     */
  }
}


/**
 * Make $groups_overview_path the parent path for group nodes.
 * The priorities can be configured per group node type.
 *
 * This class is never instantiated in native Crumbs,
 * but it can be used in custom modules.
 */
class og_CrumbsMultiPlugin_groups_overview implements crumbs_MultiPlugin {

  protected $groupsOverviewPaths;

  /**
   * @param array|string $groups_overview_paths
   *   Either
   *     just one parent path,
   *   or
   *     an array of parent paths per node type. E.g.
   *     array(
   *       'city_group' => 'city-groups',
   *       'sports_group' => 'groups/sports',
   *     )
   *     The user is responsible to make sure that these are all group types.
   */
  function __construct($groups_overview_paths) {
    $this->groupsOverviewPath = $groups_overview_paths;
  }

  /**
   * Tells Crumbs about available rules.
   *
   * @param crumbs_InjectedAPI_describeMultiPlugin $api
   *   API object with methods that allow describing the plugin.
   */
  function describe($api) {
    if (is_array($this->groupsOverviewPaths)) {
      foreach ($this->groupsOverviewPaths as $type => $parent_path) {
        $api->addRule($type->type);
      }
    }
    else {
      $types = node_type_get_types();
      foreach ($types as $type) {
        if (og_is_group_type('node', $type->type)) {
          $api->addRule($type->type);
        }
      }
    }
  }

  /**
   * Find a parent path for node/%, if that node is a group.
   */
  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    if (og_is_group('node', $node)) {
      return $this->getGroupsOverviewPath($node);
    }
  }

  /**
   * Overridable helper method to actually find the parent path,
   * once we know it is a group node.
   *
   * @param stdClass $group_node
   *   The node at this path, of which we know it is a group node.
   */
  protected function getGroupsOverviewPath($group_node) {
    if (is_array($this->groupsOverviewPaths)) {
      if (isset($this->groupsOverviewPaths[$group_node->type])) {
        return array($group_node->type => $this->groupsOverviewPaths[$group_node->type]);
      }
      // If the node type is not in the array, we return nothing!
    }
    else {
      return array($node->type => $this->groupsOverviewPaths);
    }
  }
}


/**
 * Make $my_groups_path the parent path for group nodes where the current user
 * is a member.
 * The priorities can be configured per group node type.
 */
class og_CrumbsMultiPlugin_my_groups_overview extends og_CrumbsMultiPlugin_groups_overview {

  /**
   * Overridable helper method to actually find the parent path,
   * once we know it is a group node.
   *
   * @param stdClass $group_node
   *   The node at this path, of which we know it is a group node.
   */
  protected function getGroupsOverviewPath($group_node) {

    // Check if the current user is a group member.
    if (og_is_member('node', $node->nid)) {

      // Use the parent implementation.
      return parent::getGroupsOverviewPath($group_node);
    }
  }
}
