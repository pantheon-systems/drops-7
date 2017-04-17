/**
 * JS for CiviCRM-enabled webforms
 */

var wfCivi = (function ($, D) {
  'use strict';
  var setting = D.settings.webform_civicrm;
  /**
   * Public methods.
   */
  var pub = {};

  pub.existingSelect = function (num, nid, path, toHide, hideOrDisable, showEmpty, cid, fetch) {
    if (cid.charAt(0) === '-') {
      resetFields(num, nid, true, 'show', toHide, hideOrDisable, showEmpty, 500);
      // Fill name fields with name typed
      if (cid.length > 1) {
        var names = {first: '', last: ''};
        var s = cid.substr(1).split(' ');
        for (var i in s) {
          var str = s[i].substr(0,1).toUpperCase() + s[i].substr(1).toLowerCase();
          if (i < 1) {
            names.first = str;
          }
          else {
            names.last += (i > 1 ? ' ' : '') + str;
          }
        }
        names.organization = names.household = names.first + (names.last ? ' ' : '') + names.last;
        for (i in names) {
          $(':input[name$="civicrm_'+num+'_contact_1_contact_'+i+'_name]"]', '.webform-client-form-'+nid).val(names[i]);
        }
      }
      return;
    }
    resetFields(num, nid, true, 'hide', toHide, hideOrDisable, showEmpty, 500);
    if (cid && fetch) {
      $('.webform-client-form-'+nid).addClass('contact-loading');
      var params = getCids(nid);
      params.load = 'full';
      params.cid = cid;
      $.getJSON(path, params, function(data) {
        fillValues(data, nid);
        resetFields(num, nid, false, 'hide', toHide, hideOrDisable, showEmpty);
        $('.webform-client-form-'+nid).removeClass('contact-loading');
      });
    }
  };

  pub.existingInit = function ($field, num, nid, path, toHide) {
    var cid = $field.val(),
      ret = null,
      hideOrDisable = $field.attr('data-hide-method'),
      showEmpty = $field.attr('data-no-hide-blank') == '1';
    if ($field.length) {
      if ($field.is('[type=hidden]') && !cid) {
        return;
      }
      if (!cid || cid.charAt(0) !== '-') {
        resetFields(num, nid, false, 'hide', toHide, hideOrDisable, showEmpty);
      }
      if (cid) {
        if (cid == $field.attr('data-civicrm-id')) {
          ret = [{id: cid, name: $field.attr('data-civicrm-name')}];
        }
        else if ($field.is(':text')) {
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
    }
    return ret;
  };

  pub.initFileField = function(field, info) {
    info = info || {};
    var container = $('div.webform-component[class$="--' + field.replace(/_/g, '-') + '"] div.civicrm-enabled');
    if (container.length > 0) {
      if ($('.file', container).length > 0) {
        if ($('.file', container).is(':visible')) {
          $('.file', container).hide();
          info.icon = $('.file a', container).attr('href');
        }
        else {
          return;
        }
      }
      else {
        $(':visible', container).hide();
        container.append('<input type="submit" class="form-submit ajax-processed civicrm-remove-file" value="' + Drupal.t('Change') + '" onclick="wfCivi.clearFileField(\'' + field + '\'); return false;">');
      }
      container.prepend('<span class="civicrm-file-icon"><img alt="' + Drupal.t('File') + '" src="' + info.icon + '" /> ' + (info.name ? ('<a href="'+ info.file_url+ '" target="_blank">'+info.name +'</a>') : '') + '</span>');
    }
  };

  pub.clearFileField = function(field) {
    var container = $('div.webform-component[class$="--' + field.replace(/_/g, '-') + '"] div.civicrm-enabled');
    $('.civicrm-remove-file, .civicrm-file-icon', container).remove();
    $('input[type=file], input[type=submit]', container).show();
  };

  /**
   * Private methods.
   */

  var stateProvinceCache = {};

  function resetFields(num, nid, clear, op, toHide, hideOrDisable, showEmpty, speed) {
    $('div.form-item.webform-component[class*="--civicrm-'+num+'-contact-"]', '.webform-client-form-'+nid).each(function() {
      var $el = $(this);
      var name = getFieldNameFromClass($el);
      if (!name) {
        return;
      }
      var n = name.split('-');
      if (n[0] === 'civicrm' && n[1] == num && n[2] === 'contact' && n[5] !== 'existing') {
        if (clear) {
          // Reset country to default
          if (n[5] === 'country') {
            $('select.civicrm-processed', this).val(setting.defaultCountry).trigger('change', 'webform_civicrm:reset');
          } else {
            $(':input', this).not(':radio, :checkbox, :button, :submit, :file, .form-file').each(function() {
              if (this.id && $(this).val() != '') {
                $(this).val('');
                $(this).trigger('change', 'webform_civicrm:reset');
              }
            });
            $('.civicrm-remove-file', this).click();
            $('input:checkbox, input:radio', this).each(function() {
              $(this).removeAttr('checked').trigger('change', 'webform_civicrm:reset');
            });
          }
        }
        var type = (n[6] === 'name') ? 'name' : n[4];
        if ($.inArray(type, toHide) >= 0) {
          var fn = (op === 'hide' && (!showEmpty || !isFormItemBlank($el))) ? 'hide' : 'show';
          $(':input', $el).webformProp('disabled', fn === 'hide');
          $(':input', $el).webformProp('readonly', fn === 'hide');
          if (hideOrDisable === 'hide') {
            $el[fn](speed, function() {$el[fn];});
          }
        }
      }
    });
  }

  function isFormItemBlank($el) {
    var isBlank = true;
    if ($(':input:checked', $el).length) {
      return false;
    }
    $(':input', $el).not(':radio, :checkbox, :button, :submit').each(function() {
      if ($(this).val()) {
        isBlank = false;
      }
    });
    return isBlank;
  }

  function getFieldNameFromClass($el) {
    var name = false;
    $.each($el.attr('class').split(' '), function(k, val) {
      if (val.indexOf('webform-component--') === 0 && val.indexOf('--civicrm') > 0) {
        val = val.substring(val.lastIndexOf('--civicrm') + 2);
        if (val.indexOf('fieldset') < 0) {
          name = val;
        }
      }
    });
    return name;
  }

  function fillValues(data, nid) {
    $.each(data, function() {
      var fid = this.fid,
        val = this.val;
      // Handle file fields
      if (this.data_type === 'File') {
        pub.initFileField(fid, this);
        return;
      }
      // First try to find a single element - works for textfields and selects
      var $el = $('.webform-client-form-'+nid+' :input.civicrm-enabled[name$="'+fid+']"]').not(':checkbox, :radio');
      if ($el.length) {
        // For chain-select fields, store value for later if it's not available
        if ((fid.substr(fid.length - 9) === 'county_id' || fid.substr(fid.length - 11) === 'province_id') && !$('option[value='+val+']', $el).length) {
          $el.attr('data-val', val);
        }
        else if ($el.val() !== val) {
          $el.val(val).trigger('change', 'webform_civicrm:autofill');
        }
      }
      // Next go after the wrapper - for radios, dates & checkboxes
      else {
        var $wrapper = $('.webform-client-form-'+nid+' div.form-item.webform-component[class*="--'+(fid.replace(/_/g, '-'))+'"]');
        if ($wrapper.length) {
          // Date fields
          if ($wrapper.hasClass('webform-component-date')) {
            var vals = val.split('-');
            if (vals.length === 3) {
              $(':input[id$="year"]', $wrapper).val(vals[0]).trigger('change', 'webform_civicrm:autofill');
              $(':input[id$="month"]', $wrapper).val(parseInt(vals[1], 10)).trigger('change', 'webform_civicrm:autofill');
              $(':input[id$="day"]', $wrapper).val(parseInt(vals[2], 10)).trigger('change', 'webform_civicrm:autofill');
            }
          }
          // Checkboxes & radios
          else {
            $.each($.makeArray(val), function(k, v) {
              $(':input[value="'+v+'"]', $wrapper).webformProp('checked', true).trigger('change', 'webform_civicrm:autofill');
            });
          }
        }
      }
    });
  }

  function parseName(name) {
    var pos = name.lastIndexOf('[civicrm_');
    name = name.slice(1 + pos);
    pos = name.indexOf(']');
    if (pos !== -1) {
      name = name.slice(0, pos);
    }
    return name;
  }

  function populateStates(stateSelect, countryId) {
    $(stateSelect).webformProp('disabled', true);
    if (stateProvinceCache[countryId]) {
      fillOptions(stateSelect, stateProvinceCache[countryId]);
    }
    else {
      $.getJSON(setting.callbackPath+'/stateProvince/'+countryId, function(data) {
        fillOptions(stateSelect, data);
        stateProvinceCache[countryId] = data;
      });
    }
  }

  function populateCounty() {
    var
      stateSelect = $(this),
      key = parseName(stateSelect.attr('name')),
      countryId = stateSelect.parents('form').find('.civicrm-enabled[name*="['+(key.replace('state_province', 'country'))+']"]').val(),
      countySelect = stateSelect.parents('form').find('.civicrm-enabled[name*="['+(key.replace('state_province','county' ))+']"]'),
      stateVal = stateSelect.val();
    if (countySelect.length) {
      if (!stateVal) {
        fillOptions(countySelect, {'': Drupal.t('- First Choose a State -')});
      }
      else if (stateVal === '-') {
        fillOptions(countySelect, null);
      }
      else {
        $.getJSON(setting.callbackPath+'/county/'+stateVal+'-'+countryId, function(data) {
          fillOptions(countySelect, data);
        });
      }
    }
  }

  function fillOptions(element, data) {
    var $el = $(element),
      value = $el.attr('data-val') ? $el.attr('data-val') : $el.val();
    $el.find('option').remove();
    if (!$.isEmptyObject(data || [])) {
      if (!data['']) {
        var text = $el.hasClass('required') ? Drupal.t('- Select -') : Drupal.t('- None -');
        if ($el.hasClass('has-default')) {
          $el.removeClass('has-default');
        }
        else {
          $el.append('<option value="">'+text+'</option>');
        }
      }
      $.each(data, function(key, val) {
        $el.append('<option value="'+key+'">'+val+'</option>');
      });
      $el.val(value);
    }
    else {
      $el.removeClass('has-default');
      $el.append('<option value="-">'+Drupal.t('- N/A -')+'</option>');
    }
    $el.removeAttr('disabled').trigger('change', 'webform_civicrm:chainselect');
  }

  function sharedAddress(item, action, speed) {
    var name = parseName($(item).attr('name'));
    var fields = $(item).parents('form.webform-client-form').find('[name*="['+(name.replace('master_id', ''))+'"]').not('[name*=location_type_id]').not('[name*=master_id]').not('[type="hidden"]');
    if (action === 'hide') {
      fields.parent().hide(speed, function() {$(this).css('display', 'none');});
      fields.webformProp('disabled', true);
    }
    else {
      fields.removeAttr('disabled');
      fields.parent().show(speed);
    }
  }

  function countrySelect() {
    var name = parseName($(this).attr('name'));
    var countryId = $(this).val();
    var stateSelect = $(this).parents('form.webform-client-form').find('select.civicrm-enabled[name*="['+(name.replace('country', 'state_province'))+']"]');
    if (stateSelect.length) {
      populateStates(stateSelect, countryId);
    }
  }

  function getCids(nid) {
    var cids = $('.webform-client-form-'+nid).data('civicrm-ids') || {};
    $('.webform-client-form-'+nid+' .civicrm-enabled:input[name$="_contact_1_contact_existing]"]').each(function() {
      var cid = $(this).val();
      if (cid) {
        var n = parseName($(this).attr('name')).split('_');
        cids['cid' + n[1]] = cid;
      }
    });
    return cids;
  }

  function makeSelect($el) {
    var value = $el.val(),
      classes = $el.attr('class').replace('text', 'select'),
      id = $el.attr('id'),
      $form = $el.closest('form');
    if (value !== '') {
      classes = classes + ' has-default';
    }
    $el.replaceWith('<select id="'+$el.attr('id')+'" name="'+$el.attr('name')+'"' + ' class="' + classes + ' civicrm-processed" data-val="' + value + '"></select>');
    return $('#' + id, $form).change(function() {
      $(this).attr('data-val', '');
    });
  }

  D.behaviors.webform_civicrmForm = {
    attach: function (context) {
      if (!stateProvinceCache['default'] && setting) {
        stateProvinceCache['default'] = setting.defaultStates;
        stateProvinceCache[setting.defaultCountry] = setting.defaultStates;
        stateProvinceCache[''] = {'': setting.noCountry};
      }

      // Replace state/prov & county textboxes with dynamic select lists
      $('input:text.civicrm-enabled[name*="_address_state_province_id"]', context).each(function() {
        var $el = $(this);
        var key = parseName($el.attr('name'));
        var countrySelect = $el.parents('form').find('.civicrm-enabled[name*="['+(key.replace('state_province', 'country'))+']"]');
        var $county = $el.parents('form').find('.civicrm-enabled[name*="['+(key.replace('state_province', 'county'))+']"]');
        if (!$el.attr('readonly')) {
          $el = makeSelect($el);
          if ($county.length && !$county.attr('readonly')) {
            $county = makeSelect($county);
            $el.change(populateCounty);
          }

          var countryVal = 'default';
          if (countrySelect.length === 1) {
            countryVal = $(countrySelect).val();
          }
          else if (countrySelect.length > 1) {
            countryVal = $(countrySelect).filter(':checked').val();
          }
          countryVal || (countryVal = '');

          populateStates($el, countryVal);
        }
      });

      // Add handler to country field to trigger ajax refresh of corresponding state/prov
      $('form.webform-client-form .civicrm-enabled[name*="_address_country_id]"]').once('civicrm').change(countrySelect);

      // Show/hide address fields when sharing an address
      $('form.webform-client-form .civicrm-enabled[name*="_address_master_id"]').once('civicrm').change(function(){
        var action = ($(this).val() === '' || ($(this).is('input:checkbox:not(:checked)'))) ? 'show' : 'hide';
        sharedAddress(this, action, 500);
      });

      // Hide shared address fields on form load
      $('form.webform-client-form select.civicrm-enabled[name*="_address_master_id"], form.webform-client-form .civicrm-enabled[name*="_address_master_id"]:checked').each(function() {
        if ($(this).val() !== '') {
          sharedAddress(this, 'hide');
        }
      });

      // Handle image file ajax refresh
      $('div.civicrm-enabled[id*=contact-1-contact-image-url]:has(.file)', context).each(function() {
        pub.initFileField(getFieldNameFromClass($(this).parent()));
      });
    }
  };
  return pub;
})(jQuery, Drupal);
