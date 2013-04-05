/**
 * @file
 * JS events handling for Commerce Kickstart service provider module.
 */

(function($) {

Drupal.behaviors.filterServices = {
  attach: function() {
    // Handling focus in and out on the search box.
    var search_box = $("#edit-text");
    var default_value = search_box.val();

    search_box.focus(function() {
      if (search_box.val() == default_value) {
        $(this).val('');
      }
    });

    search_box.focusout(function() {
      if (search_box.val() == '') {
        $(this).val(default_value);
      }
    });

    // Filtering the not matched text.
    search_box.keydown(function(event) {
      // Disabling enter key.
      if (event.which == 13) {
        event.preventDefault();
      }

      var keyword = $(this).val().toLowerCase();

      // Show and hide elements that contain the text.
      $(".provider-row-wrappepr .row-wrapper .second").each(function(index) {
        var parent = $(this).parents('.provider-row-wrappepr');

        if ($(this).text().toLowerCase().indexOf(keyword) !== -1) {
          parent.show();
        }
        else {
          parent.hide();
        }
      });

      // After hiding them, add odd/even class for zebra effect.
      $(".provider-row-wrappepr .row-wrapper .second:visible").each(function(index) {
        var parent = $(this).parents('.provider-row-wrappepr');
        if (index % 2 == 0) {
          parent.removeClass('even');
          parent.addClass('odd');
        }
        else {
          parent.removeClass('odd');
          parent.addClass('even');
        }
      });
    });
  }
}
})(jQuery);
