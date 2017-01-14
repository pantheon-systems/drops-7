/**
 * @file
 * Custom javascript.
 */
var lingotek = lingotek || {};
lingotek.forms = lingotek.forms || {};

(function ($) {
    
    // Page setup for node add/edit forms.
    lingotek.forms.init = function() {
        $("#edit-language").change(updateProfileSelectorDefault);
        $("#edit-language").change(updateVerticalTabSummary);
        $("#edit-language").change(toggleMenuSelector);
        $('#ltk-enable-from-et').bind('click',lingotek.forms.enableLtkFunc);
        $('#edit-lingotek-create-lingotek-document').change(updateVerticalTabSummary);
        $('#edit-lingotek-sync-method').change(updateVerticalTabSummary);
        $('#edit-lingotek-profile').change(updateVerticalTabSummary);
        $('#edit-lingotek-profile').change(checkForEnablement);
        $('#edit-lingotek-allow-source-overwriting').change(updateVerticalTabSummary);
        updateProfileSelectorDefault();
        updateVerticalTabSummary();
        checkForEnablement();
        toggleMenuSelector();
    };
    
    lingotek.forms.enableLtkFromET = false;
    
    lingotek.forms.enableLtkFunc = function(){
        lingotek.forms.enableLtkFromET = true;
        $('#ltk-push-once').attr('default_value',1);
        $('#ltk-push-once').attr('value',1);
        $('#ltk-push-once').attr('checked','checked');
        updateVerticalTabSummary();
        return false;
    }
    
    var toggleMenuSelector = function() {
        if ($("#edit-language").val() == 'en') { // locales tables assume English language
          $('#edit-menu-english').show();
          $('#edit-menu-non-english').hide();
        }
        else {
          $('#edit-menu-english').hide();           
          if ($("#edit-language").val() != 'und') { // show the note if neither English or Undefined
            $('#edit-menu-non-english').show();
          }
          else {
            $('#edit-menu-non-english').hide();
          }
        }
    }

    var updateProfileSelectorDefault = function() {
      // if the node already exists, then it must have a profile already: don't change it!
      if ($('#lingotek-preserve-profile').val() == "1") {
        return;
      }
      if ($('#lingotek-bundle-profiles').length > 0) {
        // get the language map for the current bundle
        var profiles_by_langcode = JSON.parse($('#lingotek-bundle-profiles').val());
        // set the #edit-lingotek-profile select box
        var langcode = $("#edit-language").val();
        if ($('#lingotek-language-specific-profiles').val() == '0') {
          $('#edit-lingotek-profile').val(profiles_by_langcode['DEFAULT']);
        }
        else if (langcode in profiles_by_langcode) {
          $('#edit-lingotek-profile').val(profiles_by_langcode[langcode]);
        }
        else {
          // The language must not be enabled for Lingotek at all.
          $('#edit-lingotek-profile').val('DISABLED');
        }
      }
    }

    var checkForEnablement = function() {
        if ($('#edit-lingotek-profile').val() != 'DISABLED') {
            $('#edit-lingotek-overwrite-warning').show();
        }
        else {
            $('#edit-lingotek-overwrite-warning').hide();
        }
    }
    
    var updateVerticalTabSummary = function() {
        var isPushedToLingotek = !isNaN(parseInt($('#edit-lingotek-document-id').val()));
        var isEntityTranslationNode = $('#ltk-entity-translation-node').val();
        var summaryMessages = [];
                
        if(!lingotek.forms.enableLtkFromET && isEntityTranslationNode && !isPushedToLingotek) {
            summaryMessages.push(Drupal.t("Entity Translation"));
            // hide form and show entity translation explanation and button
            $('#edit-lingotek-et-content').show();
            $('#edit-lingotek-lingotek-note').hide();
            $('.form-item-lingotek-profile').hide();
            $('#edit-lingotek-advanced').hide();
            $('#edit-lingotek-content').hide();
            $('#edit-lingotek-note').hide();
        }
        else {
            summaryMessages.push(Drupal.t("Lingotek"));
            // show form and hide entity translation explanation
            $('#edit-lingotek-et-content').hide();
            $('#edit-lingotek-content').show();
            $('#edit-lingotek-lingotek-note').show();
            $('.form-item-lingotek-profile').show();
            
            var language = $("#edit-language").val();
            var sourceLanguageSet = language != 'und'; 
            var autoUpload = $('#edit-lingotek-create-lingotek-document').is(":checked");
            var autoDownload = $('#edit-lingotek-sync-method').is(":checked");
            
            // Source language set or not
            if (sourceLanguageSet) {
                $('#edit-lingotek-note').hide();
                $('#edit-lingotek-content').show();
                $('#edit-lingotek-lingotek-note').show();
                $('.form-item-lingotek-lingotek-disabled').show();
                $('.form-item-lingotek-profile').show();
            }
            else {
                //summaryMessages.push(Drupal.t("source language unset"));
                $('#edit-lingotek-profile').val('DISABLED');
                $('#edit-lingotek-note').show();
                $('#edit-lingotek-content').hide();
                $('#edit-lingotek-lingotek-note').hide();
                $('.form-item-lingotek-lingotek-disabled').hide();
                $('.form-item-lingotek-profile').hide();
            }
            
            summaryMessages.push($('#edit-lingotek-profile option:selected').text());

            if($('#edit-lingotek-allow-source-overwriting').is(":visible")) {
                if($('#edit-lingotek-allow-source-overwriting').is(":checked")) {
                  $('.form-item-language-override').show();
                }
                else {
                  $('.form-item-language-override option[value=""]').attr('selected', 'selected');
                  $('.form-item-language-override').hide();
                }
            }
            else {
                var profile_lang_override = '[name=\''.concat($('#edit-lingotek-profile').val()).concat('_override\']');
                if($(profile_lang_override).val() == 'true') {
                  $('.form-item-language-override').show();
                }
                else {
                  $('.form-item-language-override option[value=""]').attr('selected', 'selected');
                  $('.form-item-language-override').hide();
                }
            }
        }
        
        var extraDetails = summaryMessages.slice(1).join(', ');
        extraDetails = extraDetails.length ? '('+extraDetails+')' : extraDetails;
        $('#lingotek_fieldset').drupalSetSummary(summaryMessages.slice(0,1)+' '+extraDetails);
    };

})(jQuery);

Drupal.behaviors.lingotekSetupStatus = {
    attach: lingotek.forms.init
}
