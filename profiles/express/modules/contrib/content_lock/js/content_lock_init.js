/* -*- indent-tabs-mode: nil; js-indent-level: 2; -*- */
/**
 * @file
 *   Initialize onUnload scripts.
 */

Drupal.behaviors.content_lock = {
  attach: function(context) {
  window.content_lock_onleave = function  () {
    var nid = Drupal.settings.content_lock.nid;
    var ajax_key = Drupal.settings.content_lock.ajax_key;
    jQuery.ajax({
      url: Drupal.settings.basePath + 'ajax/content_lock/'+nid+'/canceledit',
      data: {k: ajax_key, token: Drupal.settings.content_lock.token},
      async: false,
      cache: false
    });
  }

  window.content_lock_confirm = function () {
    if (Drupal.settings.content_lock.unload_js_message_enable)
      return Drupal.t(Drupal.settings.content_lock.unload_js_message);
  }

  /* Prevent submitting the node form from being interpreted as "leaving the page" */
  jQuery(Drupal.settings.content_lock.internal_forms).submit(function () {
    userMovingWithinSite();
  });

    jQuery().onUserExit( {
      execute: content_lock_onleave,
      executeConfirm: content_lock_confirm,
      internalURLs: Drupal.settings.content_lock.internal_urls
    });
  }
};
