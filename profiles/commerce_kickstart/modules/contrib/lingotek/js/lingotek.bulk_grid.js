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
  function lingotekTriggerModal(self) {
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
  
  Drupal.behaviors.lingotekBulkGrid = {
    attach: function (context) {
            
      $('input#edit-actions-submit.form-submit').hide();
      $('#edit-actions-select').change(function() {
        val = $(this).val();
        
        if(val == 'reset' || val == 'delete') {
          lingotekTriggerModal($('#'+val+'-link'));
        } else if(val == 'edit') {
          lingotekTriggerModal($('#edit-settings-link'));
        } else if(val == 'workflow') {
          lingotekTriggerModal($('#change-workflow-link'));
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
