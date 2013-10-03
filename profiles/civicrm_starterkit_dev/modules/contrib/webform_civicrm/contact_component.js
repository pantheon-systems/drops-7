/**
 * Javascript Module for administering the webform_civicrm contact field.
 */

var wfCiviContact = (function ($, D) {

  var pub = {};

  pub.init = function (path) {
    var field = $('#default-contact-id');
    var cid = field.attr('defaultValue');
    var ret = null;
    if (cid) {
      if (cid == field.attr('data-civicrm-id')) {
        ret = [{id: cid, name: field.attr('data-civicrm-name')}];
      }
      else {
        // If for some reason the data is not embedded, fetch it from the server
        $.ajax({
          url: path,
          data: {cid: cid, load: 'name'},
          dataType: 'json',
          async: false,
          success: function(data) {
            if (data) {
              ret = [{id: cid, name: data}];
            }
          }
        });
      }
    }
    return ret;
  };

  D.behaviors.webform_civicrmContact = {
    attach: function (context) {
      $('#edit-extra-default', context).once('wf-civi').change(function() {
        var val = $(this).val().replace(/_/g, '-');
        $('#edit-defaults > div > .form-item', context).not('.form-item-extra-default').each(function() {
          if ($(this).hasClass('form-item-extra-default-'+val)) {
            $(this).removeAttr('style');
          }
          else {
            $(this).css('display', 'none');
            $(':checkbox', this).attr('disabled', 'disabled');
          }
        });
        if (val === 'auto' || val === 'relationship') {
          $('.form-item-extra-randomize, .form-item-extra-dupes-allowed')
            .removeAttr('style')
            .find(':checkbox')
            .removeAttr('disabled');
        }
      }).change();
      $('#edit-extra-widget', context).once('wf-civi').change(function() {
        if ($(this).val() == 'hidden') {
          $('.form-item-extra-search-prompt', context).css('display', 'none');
          $('.form-item-extra-show-hidden-contact', context).removeAttr('style');
        }
        else {
          $('.form-item-extra-search-prompt', context).removeAttr('style');
          $('.form-item-extra-show-hidden-contact', context).css('display', 'none');
        }
      }).change();

      // Warning if enforce permissions is disabled
      $('#webform-component-edit-form', context).once('wf-civi').submit(function() {
        if (!$('input[name="extra[filters][check_permissions]"]').is(':checked')) {
          return confirm(Drupal.t('Warning: You are disabling CiviCRM permission checks for this contact. Anyone with access to this webform will be able to view any contact in the database (who meets the filter criteria) by typing their contact id in the URL.'));
        }
      });
    }
  };

  return pub;
})(jQuery, Drupal);
