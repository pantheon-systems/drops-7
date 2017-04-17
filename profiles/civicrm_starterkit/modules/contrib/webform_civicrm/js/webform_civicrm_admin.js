// Shim for old versions of jQuery
if (typeof jQuery.fn.prop !== 'function') {
  jQuery.fn.prop = jQuery.fn.attr;
}

/**
 * Javascript Module for managing the webform_civicrm admin form.
 */
var wfCiviAdmin = (function ($, D) {
  var billingEmailMsg;
  /**
   * Public methods.
   */
  var pub = {};

  pub.selectReset = function (op, id) {
    var context = $(id);
    switch (op) {
      case 'all':
        $('input:enabled:checkbox', context).prop('checked', true);
        $('select:enabled[multiple] option, select:enabled option[value="create_civicrm_webform_element"]', context).each(function() {
          $(this).prop('selected', true);
        });
        break;
      case 'none':
        $('input:enabled:checkbox', context).prop('checked', false);
        $('select:enabled:not([multiple])', context).each(function() {
          if ($(this).val() === 'create_civicrm_webform_element') {
            $('option', this).each(function() {
              $(this).prop('selected', $(this).prop('defaultSelected'));
            });
          }
          if ($(this).val() === 'create_civicrm_webform_element') {
            $('option:first-child+option', this).prop('selected', true);
          }
        });
        $('select:enabled[multiple] option', context).prop('selected', false);
        break;
      case 'reset':
        $('input:enabled:checkbox', context).each(function() {
          $(this).prop('checked', $(this).prop('defaultChecked'));
        });
        $('select:enabled option', context).each(function() {
          $(this).prop('selected', $(this).prop('defaultSelected'));
        });
        break;
    }
    $('select', context).change();
  };

  pub.participantConditional = function (fs) {
    var info = {
      roleid:$('.participant_role_id', fs).val(),
      eventid:'0',
      eventtype:$('#edit-reg-options-event-type').val()
    };
    var i, events = [];
    $('.participant_event_id :selected', fs).each(function(a, selected) {
      if ($(selected).val() !== 'create_civicrm_webform_element') {
        events.push($(selected).val());
      }
    });
    for (i in events) {
      var splitstr = events[i].split('-');
      if (events.length === 1) {
        info['eventid'] = splitstr[0];
      }
      if (i == 0) {
        info['eventtype'] = splitstr[1];
      }
      else if (info['eventtype'] !== splitstr[1]) {
        info['eventtype'] = '0';
      }
    }

    $('fieldset.extends-condition', fs).each(function() {
      var hide = true;
      var classes = $(this).attr('class').split(' ');
      for (var cl in classes) {
        var c = classes[cl].split('-');
        var type = c[0];
        if (type === 'roleid' || type === 'eventtype' || type === 'eventid') {
          for (var cid in c) {
            if (c[cid] === info[type] || ($.isArray(info[type]) && $.inArray(c[cid], info[type]) !== -1)) {
              hide = false;
            }
          }
          break;
        }
      }
      $(this)[hide? 'hide' : 'show'](300).find(':checkbox').prop('disabled', hide);
    });
  };

  /**
   * Private methods.
   */

  // Change relationship options on-the-fly when contact types are altered
  function relationshipOptions() {
    var types = contactTypes();
    $('select[id$=relationship-relationship-type-id]').each(function() {
      var selected_option = $(this).val();
      var id = $(this).attr('id').split('-');
      var contact_a = types[id[2]];
      var contact_b = types[id[4]];
      $('option', this).not('[value="0"],[value="create_civicrm_webform_element"]').remove();
      for (var i in D.settings.webform_civicrm.rTypes) {
        var t = D.settings.webform_civicrm.rTypes[i];
        var reciprocal = (t['label_a_b'] != t['label_b_a'] && t['label_b_a'] || t['type_a'] != t['type_b']);
        if ( (t['type_a'] == contact_a['type'] || !t['type_a'])
          && (t['type_b'] == contact_b['type'] || !t['type_b'])
          && ($.inArray(t['sub_type_a'], contact_a['sub_type']) > -1 || !t['sub_type_a'])
          && ($.inArray(t['sub_type_b'], contact_b['sub_type']) > -1 || !t['sub_type_b'])
        ) {
          $(this).append('<option value="'+t['id']+(reciprocal ? '_a">' : '_r">')+t['label_a_b']+'</option>');
        }
        if (reciprocal
          && (t['type_a'] == contact_b['type'] || !t['type_a'])
          && (t['type_b'] == contact_a['type'] || !t['type_b'])
          && ($.inArray(t['sub_type_a'], contact_b['sub_type']) > -1 || !t['sub_type_a'])
          && ($.inArray(t['sub_type_b'], contact_a['sub_type']) > -1 || !t['sub_type_b'])
        ) {
          $(this).append('<option value="'+t['id']+'_b">'+t['label_b_a']+'</option>');
        }
      }
      $(this).val(selected_option).change();
    });
  }

  // Change employer options on-the-fly when contact types are altered
  function employerOptions() {
    var options = '';
    $('div.contact-type-select').each(function(i) {
      var c = i + 1;
      if ($('select', this).val() == 'organization') {
        options += '<option value="' + c + '">' + getContactLabel(c) + '</option>';
      }
    });
    $('select[id$=contact-employer-id]').each(function() {
      var val = $(this).val();
      $('option', this).not('[value=0],[value=create_civicrm_webform_element]').remove();
      if (options.length > 0) {
        $(this).append(options).val(val).prop('disabled', false).removeAttr('style');
        $(this).parent().removeAttr('title');
        $('option[value=0]', this).text(Drupal.t('- None -'));
      }
      else {
        $(this).val(0).prop('disabled', true).css('color', 'gray');
        $(this).parent().attr('title', Drupal.t('To create an employer relationship, first add an organization-type contact to the webform.'));
        $('option[value=0]', this).text(Drupal.t('- first add an org -'));
      }
    });
  }

  // Fetch current contact type settings
  function contactTypes() {
    var contacts = $('#edit-number-of-contacts').val();
    var types = {};
    for (var c=1; c<=contacts; c++) {
      var sub_type = [];
      $('#edit-civicrm-'+c+'-contact-1-contact-contact-sub-type :selected').each(function(i, selected) {
        if ($(selected).val() !== 'create_civicrm_webform_element' && $(selected).val() !== '0') {
          sub_type[i] = $(selected).val();
        }
      });
      types[c] = {
            type: $('#edit-'+c+'-contact-type').val(),
        sub_type: sub_type
      };
    }
    return types
  }

  // Trim a string and strip html
  function CheckLength(str) {
    str = D.checkPlain(str);
    if (str.length > 40) {
      str = str.substr(0, 38) + '...';
    }
    return str;
  }

  // Handle contact label changing
  function changeContactLabel() {
    var c = $(this).attr('name').split('_')[0];
    var label = getContactLabel(c);
    $('.vertical-tabs-list li', '#wf-crm-configure-form').eq(c - 1).find('strong').html(c + '. ' + label);
    $('select[data-type=ContactReference] option[value=' + c + '], select[name$=address_master_id] option[value=' + c + '], .contact-label.number-' + c, '#wf-crm-configure-form').html(label);
    $('fieldset#edit-membership').trigger('summaryUpdated');
  }

  // Return the label of contact #c
  function getContactLabel(c) {
    return CheckLength($('input[name=' + c + '_webform_label]', '#wf-crm-configure-form').val());
  }

  function showHideParticipantOptions(speed) {
    if ($('select[name=participant_reg_type]').val() == '0') {
      $('#event-reg-options-wrapper').hide(speed);
    }
    else {
      $('#event-reg-options-wrapper').show(speed);
    }
  }

  // Toggle the "multiple" attribute of a select
  function changeSelect(e) {
    var $el = $(this).siblings('select');
    var triggerChange;
    var val = $el.val();
    $(this).toggleClass('select-multiple');
    if ($el.is('[multiple]')) {
      if (val && val.length > 1) {
        triggerChange = true;
      }
      if (!$el.hasClass('required') && $('option[value=""]', $el).length < 1) {
        $el.prepend('<option value="">'+ Drupal.t('- None -') +'</option>');
      }
      $el.removeAttr('multiple');
      // Choose first option if nothing is already selected
      if (!val || val.length < 1) {
        $el.val($('option:first', $el).attr('value'));
      }
    }
    else {
      $el.attr('multiple', 'multiple');
      $('option[value=""]', $el).remove();
    }
    // For ajax fields
    if (triggerChange) {
      $el.change();
    }
    e.preventDefault();
  }

  // HTML multiselect elements are awful. This is a simple/lightweight way to make them better.
  function initMultiSelect() {
    $(this).after('<a href="#" class="wf-crm-change-select civi-icon" title="'+ Drupal.t('Toggle Multiple Options') +'"></a>');
    $(this).siblings('.wf-crm-change-select').click(changeSelect);
    if (!$(this).val() || $(this).val().length < 2) {
      $(this).siblings('.wf-crm-change-select').click();
    }
    else {
      $('option[value=""]', this).remove();
    }
  }

  /**
   * Add Drupal behaviors.
   */
  D.behaviors.webform_civicrmAdmin = {
    attach: function (context) {

      employerOptions();
      showHideParticipantOptions();

      $('select[multiple]', '#wf-crm-configure-form, #webform-component-edit-form').once('wf-crm-multiselect').each(initMultiSelect);

      // Summaries for vertical tabs
      $('fieldset[id^="edit-contact-"]', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = $('select[name$="_contact_type"] option:selected', context).text();
        if ($('select[name$="_contact_sub_type[]"]', context).val()) {
          var first = true;
          $('select[name$="_contact_sub_type[]"] option:selected', context).each(function() {
            label += (first ? ' (' : ', ') + $.trim($(this).text());
            first = false;
          });
          label += ')';
        }
        return label;
      });
      $('fieldset#edit-st-message', context).once('wf-civi').drupalSetSummary(function (context) {
        if ($('[name="toggle_message"]', context).is(':checked')) {
          return CheckLength($('#edit-message', context).val());
        }
        else {
          return Drupal.t('- None -');
        }
      });
      $('fieldset#edit-prefix', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = $('[name="prefix_known"]', context).val() || $('[name="prefix_unknown"]', context).val();
        return CheckLength(label) || Drupal.t('- None -');
      });
      $('#edit-participant, #edit-contribution', context).once('wf-civi').drupalSetSummary(function (context) {
        return $('select:first option:selected', context).text();
      });
      $('fieldset#edit-activitytab', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = [];
        $('fieldset.activity-wrapper', context).each(function() {
          var caseType = $('select[name$=case_type_id]', this).val();
          var prefix = caseType && caseType != '0' ? $('select[name$=case_type_id] option:selected', this).text() + ': ' : '';
          label.push(prefix + $('select[name$=activity_type_id] option:selected', this).text());
        });
        return label.join('<br />') || Drupal.t('- None -');
      });
      $('fieldset#edit-casetab', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = [];
        $('select[name$=case_type_id]', context).each(function() {
          label.push($(this).find('option:selected').text());
        });
        return label.join('<br />') || Drupal.t('- None -');
      });
      $('fieldset#edit-membership', context).once('wf-civi').drupalSetSummary(function (context) {
        var memberships = [];
        $('select[name$=membership_type_id]', context).each(function() {
          var label = getContactLabel($(this).attr('name').split('_')[1]);
          memberships.push(label + ': ' + $(this).find('option:selected').text());
        });
        return memberships.join('<br />') || Drupal.t('- None -');
      });
      $('#edit-granttab', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = [];
        $('select[name$=grant_type_id]', context).each(function() {
          label.push($(this).find('option:selected').text());
        });
        return label.join('<br />') || Drupal.t('- None -');
      });
      $('fieldset#edit-options', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = '';
        $(':checked', context).each(function() {
          label = (label ? label + ', ' : '') + $.trim($(this).siblings('label').text());
        });
        return label || Drupal.t('- None -');
      });

      $('select[name=participant_reg_type]', context).once('wf-civi').change(function() {
        showHideParticipantOptions('fast');
      });

      // Update activity block when changing # of cases to refresh the "file on case" selector
      if ($(context).is('#civicrm-ajax-caseTab-case')) {
        if ($('select[name=activity_number_of_activity]').val() !== '0') {
          $('select[name=activity_number_of_activity]').change();
        }
      }

      $('#edit-nid', context).once('wf-civi').change(function() {
        if ($(this).is(':checked')) {
          $('#wf-crm-configure-form .vertical-tabs, .form-item-number-of-contacts').removeAttr('style');
          $('#wf-crm-configure-form .vertical-tabs-panes').removeClass('hidden');
          $('[name="number_of_contacts"]').prop('disabled', false);
        }
        else {
          $('#wf-crm-configure-form .vertical-tabs, .form-item-number-of-contacts').css('opacity', '0.4');
          $('#wf-crm-configure-form .vertical-tabs-panes').addClass('hidden');
          $('[name="number_of_contacts"]').prop('disabled', true);
        }
      }).change();

      // Show/hide 'Not you?' message settings
      if ($('#edit-toggle-message').not(':checked')) {
        $('#edit-st-message .form-item-message').hide();
      };
      $('#edit-toggle-message', context).once('wf-civi').change(function() {
        if ($(this).is(':checked')) {
          $('#edit-message').prop('disabled', false);
          $('#edit-st-message .form-item-message').show('fast');
        }
        else {
          $('#edit-message').prop('disabled', true);
          $('#edit-st-message .form-item-message').hide('fast');
        }
      }).change();

      $('select[id*=contact-type], select[id*=contact-sub-type]', context).once('wf-civi-relationship').change(function() {
        relationshipOptions();
      });

      $('#edit-number-of-contacts', context).once('wf-civi').change(function() {
        $('#wf-crm-configure-form')[0].submit();
      });

      // Show/hide custom relationship fields
      $('select[name*="relationship_relationship_type_id"]', context).once('wf-civi-rel').change(function() {
        var name = $(this).attr('name').replace('relationship_type_id[]', '');
        // Input type may be single or multi-select
        var val = typeof($(this).val()) == 'string' ? [$(this).val()] : $(this).val();
        $(':input[name*="'+name+'"][data-relationship-type]', context).each(function() {
          var rel = $(this).attr('data-relationship-type').split(',');
          var show = false;
          $.each(val, function(i, v) {
            v = v.split('_');
            if ($.inArray(v[0], rel) > -1) {
              show = true;
            }
          });
          if (show) {
            $(this).prop('disabled', false);
            $(this).parent().removeAttr('style');
          }
          else {
            $(this).parent().css('display', 'none');
            $(this).prop('disabled', true);
          }
        });
      }).change();

      $('select[name*="address_master_id"]', context).once('wf-civi').change(function() {
        var ele = $(this);
        var fs = ele.parent().parent();
        switch (ele.val()) {
          case 'create_civicrm_webform_element':
          case '0':
            $('input:checkbox', fs).prop('disabled', false);
            $('div.form-type-checkbox', fs).show();
            break;
          default:
            $('input:checkbox', fs).prop('disabled', true);
            $('div.form-type-checkbox', fs).hide();
        }
      }).change();

      // Loop through fieldsets and set icon in the tab.
      // We don't use the once() method because we need the i from the loop
      $('#wf-crm-configure-form fieldset.vertical-tabs-pane').each(function(i) {
        if (!$(this).hasClass('wf-civi-icon-processed')) {
          var clas = $(this).attr('class').split(' ');
          var name = '';
          for (var c in clas) {
            var cl = clas[c].split('-');
            if (cl[1] == 'icon') {
              if (cl[0] == 'contact') {
                name = 'name="' + (i + 1) + '_contact_type"'
              }
              $('#wf-crm-configure-form .vertical-tab-button a').eq(i).prepend('<span class="civi-icon '+cl[2]+'" '+name+'> </span>');
            }
          }
          $(this).addClass('wf-civi-icon-processed');
        }
      });

      // Respond to contact type changing
      $('select[name$="_contact_type"]').once('contact-type').change(function() {
        $('#wf-crm-configure-form .vertical-tab-button span[name="'+$(this).attr('name')+'"]').removeClass().addClass('civi-icon '+$(this).val());
        employerOptions();
      });

      // Contact label change events
      $('input[name$=_webform_label]', context).once('wf-keyup').keyup(changeContactLabel);
      $('input[name$=_webform_label]', context).once('wf-change').change(function() {
        // Trim string and replace with default if empty
        var label = $(this).val().replace(/^\s+|\s+$/g,'');
        if (!label.length) {
          var c = $(this).attr('name').split('_')[0];
          label = Drupal.t('Contact !num', {'!num': c});
        }
        $(this).val(label);
        changeContactLabel.call(this);
      });

      // Contribution honoree fields
      $('select[name$=contribution_honor_contact_id]', context).once('crm-contrib').change(function() {
        if ($(this).val() == '0') {
          $('.form-item-civicrm-1-contribution-1-contribution-honor-type-id').hide();
        }
        else {
          $('.form-item-civicrm-1-contribution-1-contribution-honor-type-id').show();
        }
      }).change();
      $('select[name$=contribution_honor_type_id]', context).once('crm-contrib').change(function() {
        var $label = $('.form-item-civicrm-1-contribution-1-contribution-honor-contact-id label');
        if ($(this).val() == 'create_civicrm_webform_element') {
          $label.html(Drupal.t('In Honor/Memory of'));
        }
        else {
          $label.html($('option:selected', this).html());
        }
      }).change();

      // Membership constraints
      $('select[name$=_membership_num_terms]', context).once('crm-mem-date').change(function(e, type) {
        var $dateWrappers = $(this).parent().siblings('[class$="-date"]');
        if ($(this).val() == '0') {
          $dateWrappers.show();
          if (type !== 'init') {
            $('input', $dateWrappers).prop('checked', true);
          }
        }
        else {
          $dateWrappers.hide().find('input').prop('checked', false);
        }
      }).trigger('change', 'init');

      function billingMessages() {
        var $pageSelect = $('[name=civicrm_1_contribution_1_contribution_contribution_page_id]');
        // Warning about contribution page with no email
        if ($pageSelect.val() !== '0' && ($('[name=civicrm_1_contact_1_email_email]:checked').length < 1 || $('[name=contact_1_number_of_email]').val() == '0')) {
          var msg = Drupal.t('You must enable an email field for !contact in order to process transactions.', {'!contact': getContactLabel(1)});
          if (!$('.wf-crm-billing-email-alert').length) {
            $pageSelect.after('<div class="messages error wf-crm-billing-email-alert">' + msg + ' <button>' + Drupal.t('Enable It') + '</button></div>');
            $('.wf-crm-billing-email-alert button').click(function() {
              $('input[name=civicrm_1_contact_1_email_email]').prop('checked', true).change();
              $('select[name=contact_1_number_of_email]').val('1').change();
              return false;
            });
            if ($('.wf-crm-billing-email-alert').is(':hidden')) {
              billingEmailMsg = CRM.alert(msg, Drupal.t('Email Required'), 'error');
            }
          }
        }
        else {
          $('.wf-crm-billing-email-alert').remove();
          billingEmailMsg && billingEmailMsg.close && billingEmailMsg.close();
        }
        // Info about paid events/memberships
        $('.wf-crm-paid-entities-info').remove();
        if ($pageSelect.val() == '0') {
          $('#edit-membership').prepend('<div class="wf-crm-paid-entities-info messages status">' + Drupal.t('Configure the Contribution settings to enable paid memberships.') + '</div>');
          $('#edit-participant').prepend('<div class="wf-crm-paid-entities-info messages status">' + Drupal.t('Configure the Contribution settings to enable paid events.') + '</div>');
        }
      }
      $('[name=civicrm_1_contribution_1_contribution_contribution_page_id], [name=civicrm_1_contact_1_email_email]', context).once('email-alert').change(billingMessages);
      billingMessages();

      // Handlers for submit-limit & tracking-mode mini-forms
      $('#configure-submit-limit', context).once('wf-civi').click(function() {
        $(this).hide();
        $('#submit-limit-wrapper').show();
      });
      $('#configure-submit-limit-cancel', context).once('wf-civi').click(function() {
        $('#submit-limit-wrapper').hide();
        $('#configure-submit-limit').show();
      });
      $('#configure-submit-limit-save', context).once('wf-civi').click(function() {
        $('[name=civicrm_1_contribution_1_contribution_contribution_page_id]').change();
      });
      $('#webform-tracking-mode', context).once('wf-civi').click(function() {
        $('[name=webform_tracking_mode]').val('strict');
        $('[name=civicrm_1_contribution_1_contribution_contribution_page_id]').change();
      });
    }
  };

  /**
   * This block uses CiviCRM's jQuery not Drupal's version
   * TODO: Move more code here! Drupal's version of jQuery is ancient.
   * TODO: change 'cj' to 'CRM.$' when we drop support for Civi v4.4
   */
  cj(function($) {
    // Inline help
    $('#wf-crm-configure-form, #webform-component-edit-form').on('click', 'a.helpicon', function () {
      var topic = $(this).attr('href').substr(1);
      CRM.help($(this).attr('title'), {q: 'webform-civicrm/help/' + topic}, D.settings.basePath);
      return false;
    });
  });

  return pub;
})(jQuery, Drupal);
