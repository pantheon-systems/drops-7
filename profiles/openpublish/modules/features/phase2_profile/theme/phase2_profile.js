(function ($) {

/**
 * Build the display name (title) from the First+Last names or organization name.
 */
Drupal.behaviors.phase2_profile_name = {
  /**
   * Attaches the behavior.
   *
   */
  attach: function (context, settings) {
    var sources = {
      firstname: $('#edit-field-profile-first-name-und-0-value', context).addClass('display-name-source'),
      lastname: $('#edit-field-profile-last-name-und-0-value', context).addClass('display-name-source'),
      orgname: $('#edit-field-profile-organization-und-0-value', context).addClass('display-name-source'),
    }

    var setContent = function () {
      var content = '';
      content = (sources['orgname'].val()) ? sources['orgname'].val() : sources['firstname'].val() + ' ' + sources['lastname'].val();
      // This makes First+Last name take precedence over Organization name
      content = (sources['firstname'].val() || sources['lastname'].val()) ? sources['firstname'].val() + ' ' + sources['lastname'].val() : sources['orgname'].val();
      if ($('#edit-title').hasClass('disabled') && !$('#edit-title').hasClass('custom')){
        $('#edit-title').val(content);
      }
    };

    for (key in sources) {
      sources[key].bind('keyup.displayName change.displayName', setContent);
    }
    
    //make it possible to directly edit the title
    var edit = $('<a href="javascript:" title="Enable Full Name for direct editing.">Edit</a>');
    var reset = $('<a href="javascript:" title="Reset to generated value." class="element-hidden" >Reset</a>');
    $('#edit-title').after(edit);
    $(edit).after(reset);
    $(edit).after('<span class="seperator">&nbsp;</span>');
    
    var text = '';
    edit.toggle(function(e){
      $(this).text('Disable');
      $('#edit-title').attr('readonly', '').removeClass('disabled');
      text = $('#edit-title').val();
      $(reset).addClass('element-hidden');
    },
    function(e){
      $(this).text('Edit');
      $('#edit-title').attr('readonly', 'true').addClass('disabled');
      if($('#edit-title').val() != text) {
        $('#edit-title').addClass('custom');
        $(reset).removeClass('element-hidden');
      }
    });

    reset.click(function(){
      $('#edit-title').removeClass('custom');
      $(reset).addClass('element-hidden');
      setContent();
    });

    $("#edit-field-profile-staff-und", context).click(function (){
      if (($("#edit-field-profile-staff-und:checked", context).length) && $(".staff-information.collapsed", context).length) {
        $(".staff-information .fieldset-legend a", context).click();
      }
    });
  },
};

})(jQuery);
