/**
 * @file
 * JS file for workbench email
 */

(function ($) {

  Drupal.behaviors.workbenchEmail = {
    attach: function(context) {
      $('.workbench-email-notify-container').each(function(index) {
        var notify_cont = $(this);
        notify_cont.find('.workbench-email-notify-checkbox').click(function() {
          notify_cont.next('.workbench-email-auto-notify-container').toggle();
        });
      });
    }
  };

})(jQuery);
