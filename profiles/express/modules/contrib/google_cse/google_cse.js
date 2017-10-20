(function ($) {

  /**
   * Google CSE utility functions.
   */
  Drupal.googleCSE = Drupal.googleCSE || {};

  Drupal.behaviors.googleCSE = {
    attach: function (context, settings) {
      // Show watermark, if not disabled in module settings.
      if (Drupal.settings.googleCSE.showWaterMark) {
        Drupal.googleCSE.googleCSEWatermark('#search-block-form.google-cse', context);
        Drupal.googleCSE.googleCSEWatermark('#search-form.google-cse', context);
        Drupal.googleCSE.googleCSEWatermark('#google-cse-results-searchbox-form', context);
      }
    }
  };

  /**
   * Show google CSE watermark.
   */
  Drupal.googleCSE.googleCSEWatermark = function(id, context) {
    var f = $(id, context)[0];
    if (f && (f.query || f['edit-search-block-form--2'] || f['edit-keys'])) {
      var q = f.query ? f.query : (f['edit-search-block-form--2'] ? f['edit-search-block-form--2'] : f['edit-keys']);
      var n = navigator;
      var l = location;
      if (n.platform == 'Win32') {
        q.style.cssText = 'border: 1px solid #7e9db9; padding: 2px;';
      }
      var b = function () {
        if (q.value == '') {
          q.style.background = '#FFFFFF url(https://www.google.com/cse/intl/' + Drupal.settings.googleCSE.language + '/images/google_custom_search_watermark.gif) left no-repeat';
        }
      };
      var f = function () {
        q.style.background = '#ffffff';
      };
      q.onfocus = f;
      q.onblur = b;
      b();
    }
  };
})(jQuery);
