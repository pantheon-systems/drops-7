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
    for (key in sources) {
      sources[key].bind('keyup.displayName change.displayName', function () {
        var content = '';
        // This makes Organization name take precedence over First+Last name
        //content = (sources['orgname'].val()) ? sources['orgname'].val() : sources['firstname'].val() + ' ' + sources['lastname'].val();
        
        // This makes First+Last name take precedence over Organization name
        content = (sources['firstname'].val() || sources['lastname'].val()) ? sources['firstname'].val() + ' ' + sources['lastname'].val() : sources['orgname'].val();
        $('#edit-title').val(content);
      });
    }
    
    $("#edit-field-profile-staff-und", context).click(function (){
      if (($("#edit-field-profile-staff-und:checked", context).length) && $(".staff-information.collapsed", context).length) {
        $(".staff-information .fieldset-legend a", context).click();
      }
    });
  },
};

})(jQuery);
