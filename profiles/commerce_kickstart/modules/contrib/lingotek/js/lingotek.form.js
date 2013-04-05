/**
 * @file
 * Custom javascript.
 */
var lingotek = lingotek || {};
lingotek.forms = lingotek.forms || {};

(function ($) {

    // Page setup for node add/edit forms.
    lingotek.forms.init = function() {
        $("#edit-language").change(updateVerticalTabSummary).change();
        $('#ltk-enable-from-et').bind('click',lingotek.forms.enableLtkFunc);
        $('#edit-create-lingotek-document').change(updateVerticalTabSummary);
        $('#edit-syncmethod').change(updateVerticalTabSummary);
    };
    
    lingotek.forms.enableLtkFromET = false;
    
    lingotek.forms.enableLtkFunc = function(){
        lingotek.forms.enableLtkFromET = true;
        $('#ltk-push-once').attr('default_value',1);
        $('#ltk-push-oncee').attr('value',1);
        $('#ltk-push-once').attr('checked','checked');
        updateVerticalTabSummary();
        return false;
    }
    
    var updateVerticalTabSummary = function() {
        var isPushedToLingotek = !isNaN(parseInt($('#edit-document-id').val()));
        var isEntityTranslationNode = $('#ltk-entity-translation-node').val();
        //console.log('pushedToLingotek: '+isPushedToLingotek);
        //console.log('entityTranslationNode: '+isEntityTranslationNode);
        var summaryMessages = [];
        
        if(!lingotek.forms.enableLtkFromET && isEntityTranslationNode && !isPushedToLingotek) {
            summaryMessages.push(Drupal.t("Entity Translation"));
            // hide form and show entity translation explanation and button
            $('#edit-et-content').show();
            $('#edit-content').hide();
            $('#edit-note').hide();
        }
        else {
            summaryMessages.push(Drupal.t("Lingotek"));
            // show form and hide entity translation explanation
            $('#edit-et-content').hide();
            $('#edit-content').show();
            
            var language = $("#edit-language").val();
            var sourceLanguageSet = language != 'und'; 
            var autoUpload = $('#edit-create-lingotek-document').is(":checked");
            var autoDownload = $('#edit-syncmethod').is(":checked");
            
            summaryMessages.push( autoUpload ? Drupal.t("auto-upload") : Drupal.t("manual upload"));
            summaryMessages.push( autoDownload ? Drupal.t("auto-download") : Drupal.t("manual download"));
            
            // Source language set or not
            if (sourceLanguageSet) {
                $('#edit-note').hide();
                $('#edit-content').show();
            }
            else {
                //summaryMessages.push(Drupal.t("source language unset"));
                $('#edit-note').show();
                $('#edit-content').hide();
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
