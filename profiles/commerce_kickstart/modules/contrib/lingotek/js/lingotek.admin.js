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
      $(this).parents('tr').find('.form-checkbox').each( function() {
        if(isEnabled) {
          $(this).attr('checked', isEnabled);
        } else {
          $(this).removeAttr('checked');
        }
      })
    });
    
    //when a field checkbox is clicked
    $('.field.form-checkbox', context).click( function() {
      if($(this).attr("name") == "lingotek_use_translation_from_drupal") {
        return;
      }
      
      row = $(this).parents('tr')
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

    //ensure that there is a vertical tab set
    if($('.vertical-tabs').length != 0) {
      $('fieldset.lingotek-account', context).drupalSetSummary(function (context) {
        return Drupal.t($('#account_summary').val() + ' / ' + $('#connection_summary').val());
      });

      $('fieldset.lingotek-translate-nodes', context).drupalSetSummary(function (context) {
        $list = [];
        total = 0;
        $('fieldset.lingotek-translate-nodes select').each(function( index ) {
          var name = $(this).attr('name');
          if(name && name.substring(0, 7) == 'profile') {
            
            if($(this).val() != 'DISABLED') {
              $list.push($(this).val());
            }
            total++;
          }
        });
        if($list.length == 0) {
          return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
        } else {
          return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.length + '/' + total + ' ' + Drupal.t('content types');
        }
      });

      if ($('fieldset.lingotek-translate-field-collections').length) {
        $('fieldset.lingotek-translate-field-collections', context).drupalSetSummary(function (context) {
          $list = [];
          total = 0;
          $('fieldset.lingotek-translate-field-collections select').each(function( index ) {
            var name = $(this).attr('name');
            if(name && name.substring(0, 7) == 'profile') {

              if($(this).val() != 'DISABLED') {
                $list.push($(this).val());
              }
              total++;
            }
          });
          if($list.length == 0) {
            return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
          } else {
            return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.length + '/' + total + ' ' + Drupal.t('content types');
          }
        });
      }

      $('fieldset.lingotek-translate-comments', context).drupalSetSummary(function (context) {
        $list = [];
        total = 0;
        $('#edit-lingotek-translate-comments-node-types input').each(function( index ) {
          if($(this).attr('checked') ==  'checked' || $(this).attr('checked') == '1') {
              $list.push($(this).val());
            }
            total++;
        });
        if($list.length == 0) {
          return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
        } else {
          return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.length + '/' + total + ' ' + Drupal.t('content types');
        }
      });

      $('fieldset.lingotek-translate-configuration', context).drupalSetSummary(function (context) {
        $list = [];
        max = 5;
        extra_text = "";
        
        //uncheck lingotek_use_translation_from_drupal when builtin is not checked
        $ltk_use_translation = $('#lingotek_use_translation_from_drupal');
        if ($('#edit-config-lingotek-translate-config-builtins').is(':checked')) {
            $ltk_use_translation.removeAttr('disabled');
        } else {
            $ltk_use_translation.removeAttr('checked').attr('disabled',true);
        }

        $('#edit-additional-translation input').each(function( index ) {
          if($(this).attr('checked') ==  'checked' || $(this).attr('checked') == '1') {
            name = $(this).attr('name');
            
            if(name.indexOf("config") != -1){
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
            return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
        } else if ($list.length === max) {
            return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: all' + extra_text;
        } else {
            return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.join(', ');
        }
      });

      $('fieldset.lingotek-preferences', context).drupalSetSummary(function (context) {
        $list = [];
        $('#edit-region').each(function( index ) {
          if($(this).attr('checked') ==  'checked' || $(this).attr('checked') == '1') {
            $list.push($(this).val());
          }
        });
        return Drupal.t($list.join(', '));
      });
    }
  }
};

})(jQuery);