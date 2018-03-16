/**
 * @file
 * Implement basic methods required by all of panels.
 */

(function ($) {
  Drupal.Panels = Drupal.Panels || {};

  Drupal.Panels.changed = function(item) {
    if (!item.is('.changed')) {
      item.addClass('changed');
      item.find('div.grabber span.text').append(' <span class="star">*</span> ');
    }
  };

  Drupal.Panels.AddContentModalQuickFilter = function() {
    var input_field = $('.panels-add-content-modal input[name=quickfilter]');
    input_field.data.panelsAddContentModalQuickFilter = {
      keyupTimeout: false,
      filter: function(e) {
        if (this.val().length) {
          var search_expression = this.val().toLowerCase();
          $('.panels-add-content-modal .panels-section-columns .content-type-button').each(function(i, elem) {
            if ($(elem).text().toLowerCase().search(search_expression) > -1) {
              $(elem).show();
            }
            else {
              $(elem).hide();
            }
          });
        }
        else {
          $('.panels-add-content-modal .panels-section-columns .content-type-button').show();
        }
      }
    }
    // Use timeout to reduce the iteration over the DOM tree.
    input_field.bind('keyup.AddContentModalQuickFilter', jQuery.proxy(function(e){
      var filter = $(this).data.panelsAddContentModalQuickFilter;
      if (filter.keyupTimeout) {
        window.clearTimeout(filter.timeout);
        filter.keyupTimeout = false;
      }
      // If there's only one item left and enter is hit select it right away.
      if (e.keyCode == 13 && $('.panels-add-content-modal .panels-section-columns .content-type-button:visible').length == 1) {
        $('.panels-add-content-modal .panels-section-columns .content-type-button:visible a').trigger('click');
      }
      else {
        filter.keyupTimeout = window.setTimeout(jQuery.proxy(filter.filter, this), 200);
      }
    }, input_field));
    input_field.focus();
  };

  Drupal.Panels.restripeTable = function(table) {
    // :even and :odd are reversed because jquery counts from 0 and
    // we count from 1, so we're out of sync.
    $('tbody tr:not(:hidden)', $(table))
      .removeClass('even')
      .removeClass('odd')
      .filter(':even')
        .addClass('odd')
      .end()
      .filter(':odd')
        .addClass('even');
  };
})(jQuery);
