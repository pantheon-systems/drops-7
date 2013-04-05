(function ($) {

  $(function () {
    Drupal.ajax.prototype.commands.commerce_kickstart_inline_help_open = Drupal.KickstartHelp.Kickstart_display_open;
    Drupal.ajax.prototype.commands.commerce_kickstart_inline_help_close = Drupal.KickstartHelp.Kickstart_display_close;
  });

  // Make sure our objects are defined.
  Drupal.KickstartHelp = Drupal.KickstartHelp || {};

  /**
   * AJAX responder command.
   */
  Drupal.KickstartHelp.Kickstart_display_open = function (ajax, response, status) {

    textArea = $('#commerce-kickstart-inline-help');
    openButton = $('#commerce-kickstart-inline-help-button #edit-commerce-kickstart-inline-help-button');
    openButtonContainer = $('#commerce-kickstart-inline-help-button');
    closeButton = $('#edit-commerce-kickstart-inline-help-close-button');

    textArea.show('slow');
    openButtonContainer.hide();
    closeButton.show();
    Drupal.attachBehaviors();
  }
  /**
   * AJAX responder command.
   */
  Drupal.KickstartHelp.Kickstart_display_close = function (ajax, response, status) {

    textArea = $('#commerce-kickstart-inline-help');
    openButton = $('#commerce-kickstart-inline-help-button #edit-commerce-kickstart-inline-help-button');
    openButtonContainer = $('#commerce-kickstart-inline-help-button');
    closeButton = $('#edit-commerce-kickstart-inline-help-close-button');

    textArea.hide('slow');
    openButtonContainer.show();
    closeButton.hide();
    Drupal.attachBehaviors();
  }

  Drupal.KickstartHelp.show = function() {
    if (!$('body').find('.help-show-more').length > 0) {
      $('.kickstart-help-full-content').before($('<p><a href="#" class="help-show-more">Show more</a></p>'));
    }
    $('.kickstart-help-full-content').hide();
    $('.help-show-more').click(function () {
      $('.kickstart-help-full-content').toggle('slow');
      if ($('.help-show-more').html() == 'Show more') {
        $('.help-show-more').html('Show less');
      }
      else {
        $('.help-show-more').html('Show more');
      }
    });
    // Check if the help section is hidden. If so, hide the open button.
      if ($('#commerce-kickstart-inline-help').is(":visible")) {
        $('#commerce-kickstart-inline-help-button').hide();
      }

  };

  Drupal.behaviors.KickstartHelp = {
    attach:function (context) {
      $('#commerce-kickstart-inline-help').once('help-processed', function() {
        Drupal.KickstartHelp.show();
      });
    }
  };

})(jQuery);

