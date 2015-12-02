(function ($) {
  $(document).ready(function(){ 
     $("#toggle").click(function() {
        $("#zone-menu, #block-google-appliance-ga-block-search-form").slideToggle('fast');
        return false;
     });
     $("#tablet-toggle").click(function() {
        $("#mobile-menu").slideToggle('fast');
        return false;
     });
  });
})(jQuery);