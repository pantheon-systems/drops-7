/**
 * @file
 * Custom javascript.
 */
function lingotek_perform_action(nid, action) {
  jQuery('#edit-grid-container .form-checkbox').removeAttr('checked');
  jQuery('#edit-the-grid-' + nid).attr('checked', 'checked');
  jQuery('#edit-actions-select').val(action);
  jQuery('#edit-actions-select').trigger('change');
}

(function ($) {
  function lingotek_trigger_modal(self) {
    var $self = $(self);
    url = $self.attr('href');
    var entity_ids = [];
    $('#edit-grid-container .form-checkbox').each(function() {
      if($(this).attr('checked')) {
        val = $(this).val();
        if(val != 'on') {
          entity_ids.push(val);
        }
      }
    });
    console.log(entity_ids);
    if(entity_ids.length > 0) {
      $('#edit-actions-select').val('select');
      ob = Drupal.ajax[url];
      ob.element_settings.url = ob.options.url = ob.url = url + '/' + entity_ids.join(',');
      $self.trigger('click');
      $self.attr('href', url);
      $('.modal-header .close').click( function() {
        location.reload();
      });
    } else {
      var $console = $('#console').length ? $('#console') : $("#lingotek-console");
      $console.html(Drupal.t('<div class="messages warning"><h2 class="element-invisible">Warning message</h2>You must select at least one entity to perform this action.</div>'));
    }
  }

  var message_already_shown = false;

  Drupal.behaviors.lingotekBulkGrid = {
    attach: function (context) {
      $('.form-checkbox').change(function() {
        var cells_of_selected_row = $(this).parents("tr").children();

        var selected_set_name = cells_of_selected_row.children('.set_name').text();

        var rows_in_same_set = $("tr").children().children('.set_name:contains("' + selected_set_name + '")').parent().parent();

        var rows_with_incompletes = rows_in_same_set.children().children('.target-pending, .target-ready, .target-edited').parent().parent();
        var boxes_checked = rows_in_same_set.children().children().children("input:checkbox:checked").length;
        if ($(this).is(':checked')) {
          rows_with_incompletes.addClass('selected');
        }
        else if (boxes_checked <= 0) {
          rows_in_same_set.removeClass('selected');
        }
        else {
          // only uncheck the box that was clicked
        }
        var this_row_incomplete = $.inArray($(this).parents('tr')[0], rows_with_incompletes) !== -1;
        var other_rows_with_incompletes = rows_with_incompletes.length - this_row_incomplete;

        if (!message_already_shown && other_rows_with_incompletes > 0) {
          $('#edit-grid-container').prepend('<div class="messages warning">All items in the same config set will be updated simultaneously, therefore some items are automatically highlighted. Disassociation will occur on an individual basis and only checked items will be affected.</div>');
          message_already_shown = true;
        }
      });

      $('input#edit-actions-submit.form-submit').hide();
      $('#edit-actions-select').change(function() {
        val = $(this).val();

        if(val == 'reset' || val == 'delete') {
          lingotek_trigger_modal($('#'+val+'-link'));
        } else if(val == 'edit') {
          lingotek_trigger_modal($('#edit-settings-link'));
        } else if(val == 'workflow') {
          lingotek_trigger_modal($('#change-workflow-link'));
        } else  {
          $('input#edit-actions-submit.form-submit').trigger('click');
        }
      });

      $('#edit-limit-select').change(function() {
        $('#edit-search-submit.form-submit').trigger('click');
      });
  }
};

})(jQuery);
