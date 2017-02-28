(function ($) {
  $(document).ready(function(){
    $("#edit-menu-enabled").click(function() {
      var newsletterTitle = $("#edit-field-newsletter-title-und-0-value").val();
      $("#edit-menu-link-title").val(newsletterTitle);
    });
    $("#edit-field-newsletter-title-und-0-value").blur(function() {
      var newsletterTitle = $("#edit-field-newsletter-title-und-0-value").val();
      $("#edit-menu-link-title").val(newsletterTitle);
     });
  });
})(jQuery);
