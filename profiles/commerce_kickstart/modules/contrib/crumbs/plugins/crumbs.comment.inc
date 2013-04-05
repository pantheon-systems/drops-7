<?php


/**
 * Implements hook_crumbs_plugins().
 */
function comment_crumbs_plugins($api) {
  $api->monoPlugin('reply');
  $api->monoPlugin('comment');
  $api->monoPlugin('skip');
}


class comment_CrumbsMonoPlugin_reply implements crumbs_MonoPlugin {

  /**
   * Describe this plugin for the configuration form.
   */
  function describe($api) {
    return t('Make node/% the parent for comment/reply/%');
  }

  /**
   * findParent callback for comment/reply/%.
   * Actually, system paths cam look more like comment/reply/%/%, but the router
   * path is comment/reply/%. Complain to the people who wrote comment module.
   */
  function findParent__comment_reply_x($path, $item) {
    $nid = $item['fragments'][2];
    return 'node/'. $nid;
  }
}


class comment_CrumbsMonoPlugin_comment implements crumbs_MonoPlugin {

  /**
   * Describe this plugin for the configuration form.
   */
  function describe($api) {
    return t('Make node/% the parent for comment/%');
  }

  /**
   * Make node/% the parent for comment/%.
   * This also completes the breadcrumb for other comment/%/* paths.
   */
  function findParent__comment_x($path, $item) {
    $comment = comment_load($item['map'][1]);
    if ($comment && $comment->nid) {
      return 'node/' . $comment->nid;
    }
  }
}


class comment_CrumbsMonoPlugin_skip implements crumbs_MonoPlugin {

  /**
   * Describe this plugin for the configuration form.
   */
  function describe($api) {
    return t('Skip comment/% in the breadcrumb.');
  }

  /**
   * The default title for comment/% is "Comment permalink",
   * so not very useful to have in the breadcrumb.
   */
  function findTitle__comment_x($path, $item) {
    return FALSE;
  }
}



