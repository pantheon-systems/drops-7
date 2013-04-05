<?php


/**
 * Implements hook_crumbs_plugins().
 */
function blog_crumbs_plugins($api) {
  $api->monoPlugin('authorPage');
  $api->monoPlugin('authorName');
}


class blog_CrumbsMonoPlugin_authorPage implements crumbs_MonoPlugin {

  function describe($api) {
    return t("Use the blog author's user account as the parent page, and 'Blog' as the title.");
  }

  /**
   * Still under constructon..
   */
  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    if ($node->type === 'blog') {
      return 'blog/' . $node->uid;
    }
  }
}


class blog_CrumbsMonoPlugin_authorName implements crumbs_MonoPlugin {

  function describe($api) {
    return t("Set '!name's blog' as the blog title, where !name is the author's username");
  }

  /**
   * Still under constructon..
   */
  function findTitle__blog_x($path, $item) {
    $user = $item['map'][1];
    // Load the user if it hasn't been loaded due to a missing wildcard loader.
    $user = is_numeric($user) ? user_load($user) : $user;

    return t("!name's blog", array('!name' => format_username($user)));
  }
}
