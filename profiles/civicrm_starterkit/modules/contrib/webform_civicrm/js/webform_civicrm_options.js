// Shim for old versions of jQuery
if (typeof jQuery.fn.prop !== 'function') {
  jQuery.fn.prop = jQuery.fn.attr;
}

/**
 * Javascript Module for managing webform_civicrm options for select elements.
 */
(function ($, D) {
  function defaultBoxes(newType, defaultName) {
    var oldType = newType == 'radio' ? 'checkbox' : 'radio';
    var defaultValue = $('input[name*="[civicrm_defaults]"]:checked').val() || '';
    $('input:'+oldType+'[name*="[civicrm_defaults]"]').each(function() {
      var ele = $(this);
      var val = ele.attr('value');
      var classes = ele.attr('class');
      var id = ele.attr('id');
      if (newType == 'radio') {
        var name = defaultName + '[' + defaultValue + ']';
      }
      else {
        var name = defaultName + '[' + val + ']';
      }
      ele.replaceWith('<input type="'+newType+'" class="'+classes+'" id="'+id+'" name="'+name+'" value="'+val+'">');
    });
    if (defaultValue) {
      $('input:[name*="[civicrm_defaults]"][value="'+defaultValue+'"]').prop('checked', true);
    }
    $('input:checkbox.select-all-civi-defaults').change(function() {
      if ($(this).is(':checked')) {
        $('input.civicrm-default').not(':disabled').prop('checked', true);
      }
      else {
        $('input.civicrm-default, input.select-all-civi-defaults').prop('checked', false);
      }
    });
    $('input:radio[name*="[civicrm_defaults]"]').change(function() {
      if ($(this).is(':checked')) {
        $('input:radio[name*="[civicrm_defaults]"]').attr('name', defaultName + '[' + $(this).val() + ']');
      }
    });
  }
  
  D.behaviors.webform_civicrmOptions = {
    attach: function (context) {
      $('input.civicrm-enabled', context).once('wf-civi').change(function() {
        if ($(this).is(':checked') ) {
          $(this).parents('tr').find('input.civicrm-label, input.civicrm-default').prop('disabled', false);
        }
        else {
          $(this).parents('tr').find('input.civicrm-label, input.civicrm-default').prop('disabled', true).prop('checked', false);
        }
        if ($(this).parents('tr').find('input.civicrm-label').val() == '') {
          var val = $(this).parents('tr').find('span.civicrm-option-name').text();
          $(this).parents('tr').find('input.civicrm-label').val(val);
        }
      }).change();

      $('input.select-all-civi-options').once('wf-civi').change(function() {
        if ($(this).is(':checked') ) {
          $('input.civicrm-enabled, input.select-all-civi-options').prop('checked', true);
        }
        else {
          $('input.civicrm-enabled, input.select-all-civi-options, input.select-all-civi-defaults').prop('checked', false);
        }
        $('input.civicrm-enabled').change();
      });
      
      var defaultName = 'civicrm_options_fieldset[civicrm_defaults]';
      
      var multiple = $('input[name="extra[multiple]"]');
      if (multiple.is(':checkbox')) {
        multiple.once('wf-civi').change(function() {
          var type = $(this).is(':checked') ? 'checkbox' : 'radio';
          defaultBoxes(type, defaultName);
        }).change();
      }
      else if (multiple.attr('value') !== '1') {
        defaultBoxes('radio', defaultName);
      }
      
      $('a.tabledrag-handle, a.tabledrag-toggle-weight').not('.live-options-hide a').wrap('<div class="live-options-hide" />');
      
      $('input[name="extra[civicrm_live_options]"]').once('wf-civi').change(function() {
        if ($(this).is(':checked')) {
          switch ($(this).attr('value')) {
            case "0":
              $('.live-options-hide').show();
              $('.live-options-show').hide();
              $('.tabledrag-hide.visible').removeClass('visible').show();
              break;
            case "1":
              $('.live-options-hide').hide();
              $('.live-options-show').show();
              $('.tabledrag-hide:visible').addClass('visible').hide();
              $('input.civicrm-enabled, input.select-all-civi-options').prop('checked', true);
              $('input.civicrm-enabled').change();
              break;
          }
        }
      }).change();
    }
  };
})(jQuery, Drupal);
