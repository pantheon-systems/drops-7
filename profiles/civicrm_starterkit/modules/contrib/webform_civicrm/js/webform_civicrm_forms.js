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

  pub.existingSelect = function (num, nid, path, toHide, cid, fetch) {
    if (cid.charAt(0) === '-') {
      resetFields(num, nid, true, 'show', toHide, 500);
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
    resetFields(num, nid, true, 'hide', toHide, 500);
    if (cid && fetch) {
      $('.webform-client-form-'+nid).addClass('contact-loading');
      var params = getCids(nid);
      params.load = 'full';
      params.cid = cid;
      $.get(path, params, function(data) {
        fillValues(data, nid);
        $('.webform-client-form-'+nid).removeClass('contact-loading');
      }, 'json');
    }
  };

  pub.existingInit = function (field, num, nid, path, toHide) {
    var cid, ret = null;
    if (field.length) {
      if (field.is('select')) {
        cid = $('option:selected', field).val();
      }
      else {
        cid = field.attr('defaultValue');
      }
      if (!cid || cid.charAt(0) !== '-') {
        resetFields(num, nid, false, 'hide', toHide);
      }
      if (cid) {
        if (cid == field.attr('data-civicrm-id')) {
          ret = [{id: cid, name: field.attr('data-civicrm-name')}];
        }
        else if (field.is(':text')) {
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

  pub.contactImage = function(field, url) {
    var container = $('div.webform-component.[class$="--' + field.replace(/_/g, '-') + '"] div.civicrm-enabled');
    if (container.length > 0) {
      if ($('.file', container).length > 0) {
        if ($('.file', container).is(':visible')) {
          $('.file', container).hide();
          url = $('.file a', container).attr('href');
        }
        else {
          return;
        }
      }
      else {
        $(':visible', container).hide();
        container.append('<input type="submit" class="form-submit ajax-processed civicrm-remove-image" value="' + Drupal.t('Change Image') + '" onclick="wfCivi.clearImage(\'' + field + '\'); return false;">');
      }
      container.prepend('<img class="civicrm-contact-image" alt="' + Drupal.t('Contact Image') + '" src="' + url + '" />');
    }
  };

  pub.clearImage = function(field) {
    var container = $('div.webform-component.[class$="--' + field.replace(/_/g, '-') + '"] div.civicrm-enabled');
    $('.civicrm-remove-image, .civicrm-contact-image', container).remove();
    $('input[type=file], input[type=submit]', container).show();
  };

  /**
   * Private methods.
   */

  var stateProvinceCache = {};

  function resetFields(num, nid, clear, op, toHide, speed) {
    $('div.form-item.webform-component[class*="--civicrm-'+num+'-contact-"]', '.webform-client-form-'+nid).each(function() {
      var $el = $(this);
      var name = getFieldNameFromClass($el);
      if (!name) {
        return;
      }
      var n = name.split('-');
      if (n[0] === 'civicrm' && n[1] == num && n[2] === 'contact' && n[5] !== 'existing') {
        if (clear) {
          $(':input', this).not(':radio, :checkbox, :button, :submit').val('');
          $('.civicrm-remove-image', this).click();
          $('input:checkbox, input:radio', this).each(function() {
            $(this).removeAttr('checked');
          });
          // Trigger chain select when changing country
          if (n[5] === 'country') {
            $('select.civicrm-processed', this).val(setting.defaultCountry).change();
          }
        }
        var type = (n[6] === 'name') ? 'name' : n[4];
        if ($.inArray(type, toHide) >= 0) {
          $el[op](speed, function() {$el[op];});
        }
      }
    });
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
    for (var fid in data) {
      // Handle contact image
      if (fid.slice(-9) == 'image_URL') {
        if (data[fid].length > 0) {
          pub.contactImage(fid, data[fid]);
        }
        continue;
      }
      // First try to find a single element - works for textfields and selects
      var ele = $('.webform-client-form-'+nid+' :input.civicrm-enabled[name$="'+fid+']"]').not(':checkbox, :radio');
      if (ele.length > 0) {
        // Trigger chain select when changing country
        if (fid.substr(fid.length - 10) === 'country_id') {
          if (ele.val() != data[fid]) {
            ele.val(data[fid]);
            countrySelect('#'+ele.attr('id'), data[fid.replace('country', 'state_province')]);
          }
        }
        ele.val(data[fid]);
      }
      // Next go after the wrapper - for radios, dates & checkboxes
      else {
        var wrapper = $('.webform-client-form-'+nid+' div.form-item.webform-component[class*="--'+(fid.replace(/_/g, '-'))+'"]');
        if (wrapper.length > 0) {
          // Date fields
          if (wrapper.hasClass('webform-component-date')) {
            var val = data[fid].split('-');
            if (val.length === 3) {
              $(':input[id$="year"]', wrapper).val(val[0]);
              $(':input[id$="month"]', wrapper).val(parseInt(val[1], 10));
              $(':input[id$="day"]', wrapper).val(parseInt(val[2], 10));
            }
          }
          // Checkboxes & radios
          else {
            var val = $.makeArray(data[fid]);
            for (var i in val) {
              $(':input[value="'+val[i]+'"]', wrapper).attr('checked', 'checked');
            }
          }
        }
      }
    }
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

  function populateStates(stateSelect, countryId, stateVal) {
    $(stateSelect).attr('disabled', 'disabled');
    if (stateProvinceCache[countryId]) {
      fillOptions(stateSelect, stateProvinceCache[countryId], stateVal);
    }
    else {
      $.get(setting.callbackPath+'/stateProvince/'+countryId, function(data) {
        fillOptions(stateSelect, data, stateVal, countryId);
        stateProvinceCache[countryId] = data;
      }, 'json');
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
      else {
        $.get(setting.callbackPath+'/county/'+stateVal+'-'+countryId, function(data) {
          fillOptions(countySelect, data);
        }, 'json');
      }
    }
  }

  function fillOptions(element, data, value) {
    value = value || $(element).val();
    $(element).find('option').remove();
    var dataEmpty = true;
    var noCountry = false;
    for (var key in data) {
      if (key === '') {
        noCountry = true;
      }
      dataEmpty = false;
      break;
    }
    if (!dataEmpty) {
      if (!noCountry) {
        if ($(element).hasClass('required')) {
          var text = Drupal.t('- Select -');
        }
        else {
          var text = Drupal.t('- None -');
        }
        if ($(element).hasClass('has-default')) {
          $(element).removeClass('has-default');
        }
        else {
          $(element).append('<option value="">'+text+'</option>');
        }
      }
      for (key in data) {
        $(element).append('<option value="'+key+'">'+data[key]+'</option>');
      }
      $(element).val(value);
    }
    else {
      $(element).removeClass('has-default');
      $(element).append('<option value="-">'+Drupal.t('- N/A -')+'</option>');
    }
    $(element).removeAttr('disabled').change();
  }

  function sharedAddress(item, action, speed) {
    var name = parseName($(item).attr('name'));
    var fields = $(item).parents('form.webform-client-form').find('[name*="['+(name.replace('master_id', ''))+'"]').not('[name*=location_type_id]').not('[name*=master_id]').not('[type="hidden"]');
    if (action === 'hide') {
      fields.parent().hide(speed, function() {$(this).css('display', 'none');});
      fields.attr('disabled', 'disabled');
    }
    else {
      fields.removeAttr('disabled');
      fields.parent().show(speed);
    }
  }

  function countrySelect(ele, stateVal) {
    var name = parseName($(ele).attr('name'));
    var countryId = $(ele).val();
    var stateSelect = $(ele).parents('form.webform-client-form').find('select.civicrm-enabled[name*="['+(name.replace('country', 'state_province'))+']"]');
    if (stateSelect.length) {
      $(stateSelect).val('');
      populateStates(stateSelect, countryId, stateVal);
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

  function makeSelect(ele) {
    var value = ele.val();
    var classes = ele.attr('class').replace('text', 'select');
    if (value !== '') {
      classes = classes + ' has-default';
    }
    ele.replaceWith('<select id="'+ele.attr('id')+'" name="'+ele.attr('name')+'" class="'+classes+' civicrm-processed"><option selected="selected" value="'+value+'"> </option></select>');
  }

  D.behaviors.webform_civicrmForm = {
    attach: function (context) {
      if (!stateProvinceCache['default'] && setting) {
        stateProvinceCache['default'] = setting.defaultStates;
        stateProvinceCache[setting.defaultCountry] = setting.defaultStates;
        stateProvinceCache[''] = {'': setting.noCountry};
      }

      // Replace state/prov & county textboxes with dynamic select lists
      $('input:text.civicrm-enabled[name*="_address_state_province_id"]', context).each(function(){
        var ele = $(this);
        var id = ele.attr('id');
        var key = parseName(ele.attr('name'));
        var countrySelect = ele.parents('form').find('.civicrm-enabled[name*="['+(key.replace('state_province', 'country'))+']"]');
        var county = ele.parents('form').find('.civicrm-enabled[name*="['+(key.replace('state_province', 'county'))+']"]');
        makeSelect(ele);
        county.length && makeSelect(county);

        var countryVal = 'default';
        if (countrySelect.length === 1) {
          countryVal = $(countrySelect).val();
        }
        else if (countrySelect.length > 1) {
          countryVal = $(countrySelect).filter(':checked').val();
        }
        countryVal || (countryVal = '');

        $('#'+id).change(populateCounty);
        populateStates($('#'+id), countryVal);
      });

      // Add handler to country field to trigger ajax refresh of corresponding state/prov
      $('form.webform-client-form .civicrm-enabled[name*="_address_country_id]"]').once('civicrm').change(function(){
        countrySelect(this);
      });

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
        pub.contactImage(getFieldNameFromClass($(this).parent()));
      });
    }
  };
  return pub;
})(jQuery, Drupal);
