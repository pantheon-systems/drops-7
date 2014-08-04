/**
 * @file
 * Custom javascript.
 */

(function ($) {

Drupal.behaviors.lingotekAdminForm = {
  attach: function (context) {
 
    //when a content type checkbox is clicked
    $('.form-select', context).change( function() {
      isEnabled = $(this).val() != 'DISABLED';
      var totalChecked = $(this).parents('tr').find('.form-checkbox:checked').length;

      $(this).parents('tr').find('.form-checkbox').each( function() {
        if(isEnabled && totalChecked === 0) {
          $(this).attr('checked', isEnabled);
        } else if (!isEnabled) {
          $(this).removeAttr('checked');
        }
      })
    });
    
    // default all fields to be checked when profile is not disabled (and no fields are currently checked)
    $('.lingotek-content-settings-table').find('tr').each(function() {
      var val = $(this).find('.form-select').val();
      var count = 0;
      if (val != 'DISABLED') {
        count = $(this).find('.form-checkbox:checked').size();
        if (count == 0) {
          $(this).find('.form-checkbox').attr('checked', true);
        }
      }
    });
    //when a field checkbox is clicked
    var exemptions = [
      "lingotek_use_translation_from_drupal",
      "lingotek_prepare_config_blocks",
      "lingotek_prepare_config_taxonomies",
      "lingotek_prepare_config_menus"
      ];
    $('.field.form-checkbox', context).click( function() {
      // Abort if an exemption is found
      if ($.inArray($(this).attr("name"), exemptions) >= 0) {
        return;
      }
      
      row = $(this).parents('tr');
      if($(this).attr('checked')) {
        row.find('td:first-child .form-checkbox').each( function() {
          $(this).attr('checked', true);
        })
      } else {
        count = 0;
        row.find('.field.form-checkbox').each( function() {
          count += $(this).attr('checked') ? 1 : 0;
        })
        if(count == 0) {
          row.find('td:first-child .form-checkbox').attr('checked',false);
          row.find('.form-select').val('DISABLED');
          row.find('.form-select').trigger('change');
        }
      }
    });

    //check/uncheck dependent-function boxes when select-all checkbox is checked
    $('.select-all').change( function () {
      if ($(this).children().first().is(':checked')) {
        $('.field.form-checkbox').removeAttr('disabled').attr('checked',true);
      } else {
        $('.field.form-checkbox:not(#lingotek_use_translation_from_drupal)').attr('disabled',true);
      }
    });

    //uncheck dependent-function boxes when primary is not checked
    $('#edit-config-lingotek-translate-config-blocks').change( function () {
      if ($('#edit-config-lingotek-translate-config-blocks').is(':checked')) {
        $('#lingotek_prepare_config_blocks').removeAttr('disabled').attr('checked',true);
      } else {
        $('#lingotek_prepare_config_blocks').removeAttr('checked').attr('disabled',true);
      }
    });
    $('#edit-config-lingotek-translate-config-taxonomies').change( function () {
      if ($('#edit-config-lingotek-translate-config-taxonomies').is(':checked')) {
        $('#lingotek_prepare_config_taxonomies').removeAttr('disabled').attr('checked',true);
      } else {
        $('#lingotek_prepare_config_taxonomies').removeAttr('checked').attr('disabled',true);
      }
    });
    $('#edit-config-lingotek-translate-config-menus').change( function () {
      if ($('#edit-config-lingotek-translate-config-menus').is(':checked')) {
        $('#lingotek_prepare_config_menus').removeAttr('disabled').attr('checked',true);
      } else {
        $('#lingotek_prepare_config_menus').removeAttr('checked').attr('disabled',true);
      }
    });

    // set prep functions to disabled/enabled on initial page load
    $( function () {
      if ($('#edit-config-lingotek-translate-config-blocks').is(':checked')) {
        $('#lingotek_prepare_config_blocks').removeAttr('disabled');
      } else {
        $('#lingotek_prepare_config_blocks').attr('disabled',true);
      }

      if ($('#edit-config-lingotek-translate-config-taxonomies').is(':checked')) {
        $('#lingotek_prepare_config_taxonomies').removeAttr('disabled');
      } else {
        $('#lingotek_prepare_config_taxonomies').attr('disabled',true);
      }

      if ($('#edit-config-lingotek-translate-config-menus').is(':checked')) {
        $('#lingotek_prepare_config_menus').removeAttr('disabled');
      } else {
        $('#lingotek_prepare_config_menus').attr('disabled',true);
      }

      if ($('.form-item-config-lingotek-translate-config-views').parent().siblings().last().children().last().val() != 1) {
        $('#edit-config-lingotek-translate-config-views').attr('disabled',true);
      }
    });

    //ensure that there is a vertical tab set
    if($('.vertical-tabs').length != 0) {

      // account summary
      $('fieldset#ltk-account', context).drupalSetSummary(function (context) {
        return Drupal.t($('#account_summary').val() + ' / ' + $('#connection_summary').val());
      });

      // entity summary (used for all entity tabs in on Settings page
      $('fieldset.ltk-entity', context).drupalSetSummary(function (context) {
        $list = [];
        total = 0;

        $(context).find('select').each(function( index ) {
          var $this = $(this);
          var name = $this.attr('name');
          if(name && name.substring(0, 7) == 'profile') {
            if($this.val() != 'DISABLED') {
              $list.push($this.val());
            }
            total++;
          }
        });
        if($list.length == 0) {
          return '<span class="ltk-disabled-text">' + Drupal.t('Disabled') + '</span>';
        } else {
          return '<span class="ltk-enabled-text">' + Drupal.t('Enabled') + '</span>: ' + $list.length + '/' + total + ' ' + Drupal.t('content types');
        }
      });

      // utility enabling/disabling
      $('.ltk-entity').each(function(index) {

        var $entity_utility_options = $(this).find('.js-utility-options');
        var $entity_profile_selects = $(this).find('select');

        function turn_on() {
          $entity_utility_options.find('input[type="checkbox"]').attr('checked', true);
          $entity_utility_options.show();
        }
        function turn_off() {
          $entity_utility_options.find('input[type="checkbox"]').attr('checked', false);
          $entity_utility_options.hide();
        }
        turn_off();// disable by default

        $entity_profile_selects.each(function() {
          var $ddl = $(this);
          $ddl.data('initial', $ddl.val());// store initial value to detect need for utilities
        });
        $entity_profile_selects.change(function() {
          var $ddl = $(this);
          var initial = $ddl.data('initial');
          var current = $ddl.val();
          if (initial == 'DISABLED' && current != 'DISABLED') {
            turn_on();
          } else {
            turn_off();
          }
        });

      });

      // config summary
      $('fieldset#ltk-config', context).drupalSetSummary(function (context) {
        $list = [];
        max = 7;
        extra_text = "";
        
        $(context).find('input').each(function( index ) {
          if($(this).attr('checked') ==  'checked' || $(this).attr('checked') == '1') {
            name = $(this).attr('name');
            
            if(name.indexOf("translate_config") != -1){
                name = name.substring(name.lastIndexOf('_') + 1, name.length - 1);
                $list.push(name);
            }
            else if (name === 'lingotek_use_translation_from_drupal') {
                extra_text = "+";
                $list[$list.length-1] += extra_text;
            }
          }
        });
        if ($list.length === 0 && extra_text.length === 0) {
            return '<span class="ltk-disabled-text">' + Drupal.t('Disabled') + '</span>';
        } else if ($list.length === max) {
            return '<span class="ltk-enabled-text">' + Drupal.t('Enabled') + '</span>: all' + extra_text;
        } else {
            return '<span class="ltk-enabled-text">' + Drupal.t('Enabled') + '</span>: ' + $list.join(', ');
        }
      });

      // profiles summary
      $('fieldset#ltk-profiles', context).drupalSetSummary(function (context){
        return $(context).find('tbody tr').length + ' ' + Drupal.t('profiles')
      });

      // change enable text to be accurate for translating field collections
      $(function(){
        $('#edit-translation-field-collection-item > .form-item > .description').children().first().remove();
        $('#edit-translation-field-collection-item > .form-item > .description').children().first().text('(enable all)');
      });

      /*
      // prefs summary
      $('fieldset#ltk-prefs', context).drupalSetSummary(function (context) {
        $list = [];
        return  'Selected: '+ $(context).find('input:checkbox:checked:enabled').length + '/'+ $(context).find('input:checkbox').length;
        $(context).find('input:checkbox').each(function( index ) {
          var label = $(this).attr('checked') ? '1' : '0';
          $list.push(label);
        });
        return $list.join('-');
      });*/
    }
  }
};

})(jQuery);

function lingotekSetAll(sel, val) {
  fieldset = jQuery(sel);
  console.log(jQuery(sel));
  jQuery(sel).find('.form-select').each( function() {
    jQuery(this).val(val);
    jQuery(this).trigger('change');
  });
}
