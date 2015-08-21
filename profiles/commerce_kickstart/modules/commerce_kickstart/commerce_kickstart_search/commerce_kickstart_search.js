(function ($) {

  // Disable checkboxes when the user clicks on one of them. (prevent
  // multi-clic
  Drupal.behaviors.kickstartSearch = {
    attach:function (context) {
      $('.facetapi-checkbox').bind('click', function(e) {
        $('.facetapi-checkbox').attr("disabled", true);
      });
    }
  }
})(jQuery);
