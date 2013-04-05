<?php


/**
 * Implements hook_crumbs_plugins().
 *
 * This is the version for the og-7.x-1.x branch.
 */
function og_crumbs_plugins($api) {
  $api->multiPlugin('group_post');
  $api->monoPlugin('groups_overview_title');

  $api->multiPlugin('groups_overview',
    new og_CrumbsMultiPlugin_groups_overview('group-list'));
  $api->multiPlugin('my_groups_overview',
    new og_CrumbsMultiPlugin_my_groups_overview('user-groups'));
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

  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    $items = field_get_items('node', $node, 'group_audience');
    if ($items) {
      foreach ($items as $item) {
        $row = db_query("SELECT * FROM {og} WHERE gid = :gid", array(':gid' => $item['gid']))->fetchObject();
        if ($row && $row->entity_type === 'node') {
          $parent_path = $this->getParentPath($row->etid, $node);
          return array($node->type => $parent_path);
        }
      }
    }
  }

  /**
   * This method can be overridden by custom plugins that inherit from this one,
   * e.g. to set a different parent for group events than for group discussions.
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
 */
class og_CrumbsMultiPlugin_groups_overview implements crumbs_MultiPlugin {

  protected $groupsOverviewPath;

  /**
   * @param string $groups_overview_path
   *   The path to be used as parent for all group nodes.
   */
  function __construct($groups_overview_path) {
    $this->groupsOverviewPath = $groups_overview_path;
  }

  function describe($api) {
    $types = node_type_get_types();
    foreach ($types as $type) {
      if (og_is_group_type('node', $type->type)) {
        $api->addRule($type->type);
      }
    }
  }

  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    $items = field_get_items('node', $node, 'group_group');
    if ($items) {
      return array($node->type => 'group-list');
    }
  }
}


/**
 * Make t('Groups') the title for '/group-list'.
 */
class og_CrumbsMonoPlugin_groups_overview_title implements crumbs_MonoPlugin {

  function describe($api) {
    return t('Set "Group" as the title for item for "group-list".');
  }

  function findTitle($path, $item) {
    if ($item['route'] === 'group-list') {
      return t('Groups');
    }
  }
}


/**
 * Make $my_groups_path the parent path for group nodes where the current user
 * is a member.
 * The priorities can be configured per group node type.
 */
class og_CrumbsMultiPlugin_my_groups_overview implements crumbs_MultiPlugin {

  protected $myGroupsPath;

  /**
   * @param string $my_groups_path
   *   The path to be used as parent for all group nodes where the current user
   *   is a member.
   */
  function __construct($my_groups_path) {
    $this->myGroupsPath = $my_groups_path;
  }

  function describe($api) {
    $types = node_type_get_types();
    foreach ($types as $type) {
      if (og_is_group_type('node', $type->type)) {
        $api->addRule($type->type);
      }
    }
  }

  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    if (og_is_group_type('node', $node->type)) {
      $group = og_get_group('node', $node->nid);
      if (!empty($group)) {
        if (og_is_member($group->gid)) {
          return array($node->type => 'user-groups');
        }
      }
    }
  }
}
