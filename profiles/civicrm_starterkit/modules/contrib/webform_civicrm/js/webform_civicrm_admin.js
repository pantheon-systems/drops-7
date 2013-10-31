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
        $('input:enabled:checkbox', context).attr('checked', 'checked');
        $('select:enabled[multiple] option, select:enabled option[value="create_civicrm_webform_element"]', context).each(function() {
          $(this).attr('selected', 'selected');
        });
        break;
      case 'none':
        $('input:enabled:checkbox', context).attr('checked', '');
        $('select:enabled:not([multiple])', context).each(function() {
          if ($(this).val() === 'create_civicrm_webform_element') {
            $('option', this).each(function() {
              $(this).attr('selected', $(this).attr('defaultSelected'));
            });
          }
          if ($(this).val() === 'create_civicrm_webform_element') {
            $('option:first-child+option', this).attr('selected', 'selected');
          }
        });
        $('select:enabled[multiple] option', context).each(function() {
          $(this).attr('selected', '');
        });
        break;
      case 'reset':
        $('input:enabled:checkbox', context).each(function() {
          $(this).attr('checked', $(this).attr('defaultChecked'));
        });
        $('select:enabled option', context).each(function() {
          $(this).attr('selected', $(this).attr('defaultSelected'));
        });
        break;
    }
    $('select', context).change();
  };

  pub.participantConditional = function (fs) {
    var info = {
      roleid:$(fs + ' .participant_role_id').val(),
      eventid:'0',
      eventtype:$('#edit-reg-options-event-type').val()
    };
    var events = [];
    var i = 0;
    $(fs + ' .participant_event_id :selected').each(function(a, selected) {
      if ($(selected).val() !== 'create_civicrm_webform_element') {
        events[i++] = $(selected).val();
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

    $(fs + ' fieldset.extends-condition').each(function() {
      var hide = true;
      var classes = $(this).attr('class').split(' ');
      for (var cl in classes) {
        var c = classes[cl].split('-');
        var type = c[0];
        if (type === 'roleid' || type === 'eventtype' || type === 'eventid') {
          for (var cid in c) {
            if (c[cid] === info[type]) {
              hide = false;
            }
          }
          break;
        }
      }
      if (hide) {
        $(this).find(':checkbox').attr('disabled', 'disabled');
        $(this).hide(300);
      }
      else {
        $(this).find(':checkbox').removeAttr('disabled');
        $(this).show(300);
      }
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
      if ($(this).find('option[value='+selected_option+']').size()) {
        $(this).val(selected_option);
      }
      else {
        $(this).val("0").change();
      }
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
        $(this).append(options).val(val).removeAttr('disabled').removeAttr('style');
        $(this).parent().removeAttr('title');
        $('option[value=0]', this).text(Drupal.t('- None -'));
      }
      else {
        $(this).val(0).attr('disabled', 'disabled').css('color', 'gray');
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
        if ($(selected).val() !== 'create_civicrm_webform_element') {
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

  /**
   * Add Drupal behaviors.
   */
  D.behaviors.webform_civicrmAdmin = {
    attach: function (context) {

      employerOptions();
      showHideParticipantOptions();

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
        if ($('[name="toggle_message"]', context).attr('checked')) {
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
      $('fieldset#edit-act', context).once('wf-civi').drupalSetSummary(function (context) {
        var label = $('select[name="activity_type_id"] option:selected', context).text();
        if ($('select[name="case_type_id"] option:selected', context).val() > 0) {
          label = $('select[name="case_type_id"] option:selected', context).text() + ' ' + label;
        }
        return label;
      });
      $('fieldset#edit-membership', context).once('wf-civi').drupalSetSummary(function (context) {
        var memberships = [];
        $('select[name$=membership_type_id]', context).each(function() {
          var label = getContactLabel($(this).attr('name').split('_')[1]);
          memberships.push(label + ': ' + $(this).find('option:selected').text());
        });
        return memberships.join('<br>') || Drupal.t('- None -');
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

      $('#edit-nid', context).once('wf-civi').change(function() {
        if ($(this).is(':checked')) {
          $('#wf-crm-configure-form .vertical-tabs, .form-item-number-of-contacts').removeAttr('style');
          $('#wf-crm-configure-form .vertical-tabs-panes').removeClass('hidden');
          $('[name="number_of_contacts"]').removeAttr('disabled');
        }
        else {
          $('#wf-crm-configure-form .vertical-tabs, .form-item-number-of-contacts').css('opacity', '0.4');
          $('#wf-crm-configure-form .vertical-tabs-panes').addClass('hidden');
          $('[name="number_of_contacts"]').attr('disabled','disabled');
        }
      }).change();

      $('#edit-toggle-message', context).once('wf-civi').change(function() {
        if($(this).is(':checked')) {
          $('#edit-message').removeAttr('disabled');
        }
        else {
          $('#edit-message').attr('disabled','disabled');
        }
      }).change();

      $('select[id*=contact-type], select[id*=contact-sub-type]', context).once('wf-civi-relationship').change(function() {
        relationshipOptions();
      });

      $('#edit-number-of-contacts', context).once('wf-civi').change(function() {
        $('#wf-crm-configure-form')[0].submit();
      });

      $('select[name*="relationship_relationship_type_id"]', context).once('wf-civi').change(function() {
        var name = $(this).attr('name').replace('relationship_type_id', '');
        var val = $(this).val().split('_');
        $(':input[name*="'+name+'"][data-relationship-type]', context).each(function() {
          var rel = $(this).attr('data-relationship-type').split(',');
          if ($.inArray(val[0], rel) > -1) {
            $(this).removeAttr('disabled');
            $(this).parent().removeAttr('style');
          }
          else {
            $(this).parent().css('display', 'none');
            $(this).attr('disabled', 'disabled');
          }
        });
      }).change();

      $('select[name*="address_master_id"]', context).once('wf-civi').change(function() {
        var ele = $(this);
        var fs = ele.parent().parent();
        switch (ele.val()) {
          case 'create_civicrm_webform_element':
          case '0':
            $('input:checkbox', fs).removeAttr('disabled');
            $('div.form-type-checkbox', fs).show();
            break;
          default:
            $('input:checkbox', fs).attr('disabled', 'disabled');
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

      // Change activity subject to match survey/petition
      $('select[name$="_activity_survey_id"]', context).once('wf-civi').change(function() {
        var val = $(this).val();
        if (val != '0' && val != 'create_civicrm_webform_element') {
          var label = $(this).find('option[value=' + val + ']').text();
          $('#wf-crm-configure-form input[name=activity_subject]').val(label);
        }
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
      $('select[name$=_membership_num_terms]', context).once('crm-mem-date').change(function() {
        var $dateWrappers = $(this).parent().siblings('[class$="-date"]');
        if ($(this).val() == '0') {
          $dateWrappers.show().find('input').attr('checked', 'checked');
        }
        else {
          $dateWrappers.hide().find('input').removeAttr('checked');
        }
      }).change();

      function billingMessages() {
        var $pageSelect = $('[name=civicrm_1_contribution_1_contribution_contribution_page_id]');
        // Warning about contribution page with no email
        if ($pageSelect.val() !== '0' && ($('[name=civicrm_1_contact_1_email_email]:checked').length < 1 || $('[name=contact_1_number_of_email]').val() == '0')) {
          var msg = Drupal.t('You must enable an email field for !contact in order to process transactions.', {'!contact': getContactLabel(1)});
          if (!$('.wf-crm-billing-email-alert').length) {
            $pageSelect.after('<div class="messages error wf-crm-billing-email-alert">' + msg + '</div>');
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
   * Todo: Move more code here! Drupal's version of jQuery is ancient.
   */
  cj(function($) {
    // Inline help
    $('#wf-crm-configure-form').on('click', 'a.helpicon', function () {
      var topic = $(this).attr('href').substr(1);
      CRM.help($(this).attr('title'), {q: 'webform-civicrm/help/' + topic}, D.settings.basePath);
      return false;
    });
  });

  return pub;
})(jQuery, Drupal);
