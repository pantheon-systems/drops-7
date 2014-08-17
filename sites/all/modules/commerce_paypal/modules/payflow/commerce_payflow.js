(function($) {

/**
 * Escapes from an iframe if the completion page is displayed within an iframe.
 */
Drupal.behaviors.commercePayflowEscapeIframe = {
  attach: function (context, settings) {
    if (top !== self) {
      if (typeof Drupal.settings.commercePayflow != 'undefined' &&
        typeof Drupal.settings.commercePayflow.page != 'undefined' &&
        Drupal.settings.commercePayflow.page == 'review') {
        window.parent.location.href = window.location.href + '?payflow-page=review';
      }
      else {
        window.parent.location.href = window.location.href;
      }
    }
  }
}

})(jQuery);
