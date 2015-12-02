(function ($) {

  Drupal.behaviors.context_accordion = {
    attach: function (context, settings) {
      $('#edit-reactions-plugins-block-selector').accordion({ header: '.form-type-checkboxes > label', autoHeight: false});
    }
  };
}(jQuery));


