(function( $ ){
  $(document).ready(function(){
  
  $(".ui-accordion").bind("accordionchange", function(event, ui) {
     if ($(ui.newHeader).offset() != null) {
          ui.newHeader, // $ object, activated header
          $("html, body").animate({scrollTop: ($(ui.newHeader).offset().top)-100}, 500);
     }
  });
  
  
  
  });
})( jQuery );