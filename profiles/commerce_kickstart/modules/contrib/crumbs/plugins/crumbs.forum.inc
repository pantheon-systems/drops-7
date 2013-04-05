<?php


/**
 * Implements hook_crumbs_plugins().
 */
function forum_crumbs_plugins($api) {

  $api->monoPlugin('forumTerm');
  $api->monoPlugin('forumTermName');
  $api->monoPlugin('forumThread');
  $api->monoPlugin('forumThreadCreate');
}


// -----------------------------------------------------------------------------


class forum_CrumbsMonoPlugin_forumTerm implements crumbs_MonoPlugin {

  function describe($api) {
    return t('Let /forum/123 be the parent path of /forum/456, if 123 is the parent of 456.');
  }

  /**
   * Forums get their parent forums as breadcrumb parent.
   * The method name matches the router path "forum/%".
   * Forums are actually taxonomy terms, just the path is different.
   */
  function findParent__forum_x($path, $item) {
    $term = $item['map'][1];
    // Load the forum term,
    // if it hasn't been loaded due to a missing wildcard loader.
    // We could use forum_forum_load() instead, but this looks too scary
    // performance-wise, and we don't actually need it.
    // So we just use taxonomy_term_load() instead.
    $term = is_numeric($term) ? taxonomy_term_load($term) : $term;

    $parents = taxonomy_get_parents($term->tid);
    $result = array();
    foreach ($parents as $parent_tid => $parent_term) {
      if ($parent_term->vocabulary_machine_name == $term->vocabulary_machine_name) {
        // The parent forum
        return 'forum/' . $parent_tid;
      }
    }
    // Forum overview
    return 'forum';
  }
}


// -----------------------------------------------------------------------------


class forum_CrumbsMonoPlugin_forumTermName implements crumbs_MonoPlugin {

  function describe($api) {
    return t('Let /forum/123 be the parent path of /forum/456, if 123 is the parent of 456.');
  }

  /**
   * Forums get their parent forums as breadcrumb parent.
   * The method name matches the router path "forum/%".
   * Forums are actually taxonomy terms, just the path is different.
   */
  function findTitle__forum_x($path, $item) {
    $term = $item['map'][1];
    // Load the forum term,
    // if it hasn't been loaded due to a missing wildcard loader.
    // We could use forum_forum_load() instead, but this looks too scary
    // performance-wise, and we don't actually need it.
    // So we just use taxonomy_term_load() instead.
    $term = is_numeric($term) ? taxonomy_term_load($term) : $term;
    if (!empty($term) && !empty($term->name)) {
      return $term->name;
    }
  }
}


// -----------------------------------------------------------------------------


class forum_CrumbsMonoPlugin_forumThread implements crumbs_MonoPlugin {

  function describe($api) {
    return t('Let /forum/456 be the parent path of /node/789, if node 789 is a thread in this forum.');
  }

  /**
   * Forum nodes get their forum terms as breadcrumb parents.
   * The method name matches the router path "node/%".
   */
  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;
    if (!empty($node->forum_tid) && _forum_node_check_node_type($node)) {
      return 'forum/' . $node->forum_tid;
    }
  }
}


// -----------------------------------------------------------------------------


class forum_CrumbsMonoPlugin_forumThreadCreate implements crumbs_MonoPlugin {

  function describe($api) {
    return t('Let /forum/456 be the parent path of /node/789, if node 789 is a thread in this forum.');
  }

  /**
   * Set a parent path for e.g. node/add/(type)/(tid), where
   * - (type) a forum-enabled node type, e.g. "forum".
   * - (tid) is the forum term tid.
   */
  function findParent($path, $item) {
    if (
      // Start with a cheap-to-compute condition,
      // so the regex can be skipped in the average case.
      substr($path, 0, 9) === 'node/add/' &&
      preg_match('#^node/add/([^/]+)/(\d+)$#', $path, $m)
    ) {
      $type = $m[1];
      $tid = (int)$m[2];
      // We need to find out if the node type is forum-enabled.
      // See _forum_node_check_node_type() in forum.module.
      $field = field_info_instance('node', 'taxonomy_forums', $type);
      if (is_array($field)) {
        // That's a node/add/(type)/(tid) page for a forum-enabled node type.
        $term = taxonomy_term_load($item['original_map'][3]);
        if ($term && $term->vocabulary_machine_name === 'forums') {
          return 'forum/' . $term->tid;
        }
      }
    }
  }

  /**
   * Set a breadcrumb item title for e.g. node/add/(type)/(tid), where
   * - (type) a forum-enabled node type, e.g. "forum".
   * - (tid) is the forum term tid.
   */
  function findTitle($path, $item) {
    if (
      // Start with a cheap-to-compute condition,
      // so the regex can be skipped in the average case.
      substr($path, 0, 9) === 'node/add/' &&
      preg_match('#^node/add/([^/]+)/(\d+)$#', $path, $m)
    ) {
      $type = $m[1];
      $tid = (int)$m[2];
      // We need to find out if the node type is forum-enabled.
      // See _forum_node_check_node_type() in forum.module.
      $field = field_info_instance('node', 'taxonomy_forums', $type);
      if (is_array($field)) {
        // That's a node/add/(type)/(tid) page for a forum-enabled node type.
        $term = taxonomy_term_load($item['original_map'][3]);
        if ($term && $term->vocabulary_machine_name === 'forums') {
          return $this->newTopicTitle($type);
        }
      }
    }
  }

  /**
   * Method that can be overwritten by subclasses.
   */
  protected function newTopicTitle($type) {
    return t('New topic');
  }
}
