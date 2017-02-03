(function ($) {
  $(document).ready(function(){ 
    $("#edit-menu-enabled").click(function() {
      var firstName = $("#edit-field-person-first-name-und-0-value").val();
      var lastName = $("#edit-field-person-last-name-und-0-value").val();
      $("#edit-menu-link-title").val(firstName + ' ' + lastName);
    });
    $("#edit-field-person-first-name-und-0-value, #edit-field-person-last-name-und-0-value").blur(function() {
      var firstName = $("#edit-field-person-first-name-und-0-value").val();
      var lastName = $("#edit-field-person-last-name-und-0-value").val();
      $("#edit-menu-link-title").val(firstName + ' ' + lastName);
     }); 
  });
})(jQuery);